//===-- asan_fake_stack.h ---------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of AddressSanitizer, an address sanity checker.
//
// ASan-private header for asan_fake_stack.cc, implements FakeStack.
//===----------------------------------------------------------------------===//

#ifndef ASAN_FAKE_STACK_H
#define ASAN_FAKE_STACK_H

#include "sanitizer_common/sanitizer_common.h"

namespace __asan {

// Fake stack frame contains local variables of one function.
struct FakeFrame {
  uptr magic;  // Modified by the instrumented code.
  uptr descr;  // Modified by the instrumented code.
  uptr pc;     // Modified by the instrumented code.
  u64 real_stack : 48;
  u64 class_id : 16;
};

// For each thread we create a fake stack and place stack objects on this fake
// stack instead of the real stack. The fake stack is not really a stack but
// a fast malloc-like allocator so that when a function exits the fake stack
// is not popped but remains there for quite some time until gets used again.
// So, we poison the objects on the fake stack when function returns.
// It helps us find use-after-return bugs.
//
// The FakeStack objects is allocated by a single mmap call and has no other
// pointers. The size of the fake stack depends on the actual thread stack size
// and thus can not be a constant.
// stack_size is a power of two greater or equal to the thread's stack size;
// we store it as its logarithm (stack_size_log).
// FakeStack has kNumberOfSizeClasses (11) size classes, each size class
// is a power of two, starting from 64 bytes. Each size class occupies
// stack_size bytes and thus can allocate
// NumberOfFrames=(stack_size/BytesInSizeClass) fake frames (also a power of 2).
// For each size class we have NumberOfFrames allocation flags,
// each flag indicates whether the given frame is currently allocated.
// All flags for size classes 0 .. 10 are stored in a single contiguous region
// followed by another contiguous region which contains the actual memory for
// size classes. The addresses are computed by GetFlags and GetFrame without
// any memory accesses solely based on 'this' and stack_size_log.
// Allocate() flips the appropriate allocation flag atomically, thus achieving
// async-signal safety.
// This allocator does not have quarantine per se, but it tries to allocate the
// frames in round robin fasion to maximize the delay between a deallocation
// and the next allocation.
//
// FIXME: handle throw/longjmp/clone, i.e. garbage collect the unwinded frames.
class FakeStack {
  static const uptr kMinStackFrameSizeLog = 6;  // Min frame is 64B.
  static const uptr kMaxStackFrameSizeLog = 16;  // Max stack frame is 64K.

 public:
  static const uptr kNumberOfSizeClasses =
       kMaxStackFrameSizeLog - kMinStackFrameSizeLog + 1;

  // CTOR: create the FakeStack as a single mmap-ed object.
  static FakeStack *Create(uptr stack_size_log) {
    if (stack_size_log < 15)
      stack_size_log = 15;
    FakeStack *res = reinterpret_cast<FakeStack *>(
        MmapOrDie(RequiredSize(stack_size_log), "FakeStack"));
    res->stack_size_log_ = stack_size_log;
    return res;
  }

  void Destroy() {
    UnmapOrDie(this, RequiredSize(stack_size_log_));
  }

  // stack_size_log is at least 15 (stack_size >= 32K).
  static uptr SizeRequiredForFlags(uptr stack_size_log) {
    return 1UL << (stack_size_log + 1 - kMinStackFrameSizeLog);
  }

  // Each size class occupies stack_size bytes.
  static uptr SizeRequiredForFrames(uptr stack_size_log) {
    return (1ULL << stack_size_log) * kNumberOfSizeClasses;
  }

  // Number of bytes requires for the whole object.
  static uptr RequiredSize(uptr stack_size_log) {
    return kFlagsOffset + SizeRequiredForFlags(stack_size_log) +
           SizeRequiredForFrames(stack_size_log);
  }

  // Offset of the given flag from the first flag.
  // The flags for class 0 begin at offset  000000000
  // The flags for class 1 begin at offset  100000000
  // ....................2................  110000000
  // ....................3................  111000000
  // and so on.
  static uptr FlagsOffset(uptr stack_size_log, uptr class_id) {
    uptr t = kNumberOfSizeClasses - 1 - class_id;
    const uptr all_ones = (1 << (kNumberOfSizeClasses - 1)) - 1;
    return ((all_ones >> t) << t) << (stack_size_log - 15);
  }

  static uptr NumberOfFrames(uptr stack_size_log, uptr class_id) {
    return 1UL << (stack_size_log - kMinStackFrameSizeLog - class_id);
  }

  // Divide n by the numbe of frames in size class.
  static uptr ModuloNumberOfFrames(uptr stack_size_log, uptr class_id, uptr n) {
    return n & (NumberOfFrames(stack_size_log, class_id) - 1);
  }

  // The the pointer to the flags of the given class_id.
  u8 *GetFlags(uptr stack_size_log, uptr class_id) {
    return reinterpret_cast<u8 *>(this) + kFlagsOffset +
           FlagsOffset(stack_size_log, class_id);
  }

  // Get frame by class_id and pos.
  u8 *GetFrame(uptr stack_size_log, uptr class_id, uptr pos) {
    return reinterpret_cast<u8 *>(this) + kFlagsOffset +
           SizeRequiredForFlags(stack_size_log) +
           (1 << stack_size_log) * class_id + BytesInSizeClass(class_id) * pos;
  }

  // Allocate the fake frame.
  FakeFrame *Allocate(uptr stack_size_log, uptr class_id, uptr real_stack);

  // Deallocate the fake frame.
  void Deallocate(FakeFrame *ff, uptr stack_size_log, uptr class_id,
                  uptr real_stack);

  // Poison the entire FakeStack's shadow with the magic value.
  void PoisonAll(u8 magic);

  // Return the beginning of the FakeFrame or 0 if the address is not ours.
  uptr AddrIsInFakeStack(uptr addr);

  // Number of bytes in a fake frame of this size class.
  static uptr BytesInSizeClass(uptr class_id) {
    return 1UL << (class_id + kMinStackFrameSizeLog);
  }

  uptr stack_size_log() const { return stack_size_log_; }

 private:
  FakeStack() { }
  static const uptr kFlagsOffset = 4096;  // There is were flags begin.
  // Must match the number of uses of DEFINE_STACK_MALLOC_FREE_WITH_CLASS_ID
  COMPILER_CHECK(kNumberOfSizeClasses == 11);
  static const uptr kMaxStackMallocSize = 1 << kMaxStackFrameSizeLog;

  uptr hint_position_[kNumberOfSizeClasses];
  uptr stack_size_log_;
};

}  // namespace __asan

#endif  // ASAN_FAKE_STACK_H
