// RUN: %clang -DADD_I32 -fsanitize=unsigned-integer-overflow %s -o %t && %t 2>&1 | FileCheck %s --check-prefix=ADD_I32
// RUN: %clang -DADD_I64 -fsanitize=unsigned-integer-overflow %s -o %t && %t 2>&1 | FileCheck %s --check-prefix=ADD_I64
// RUN: %clang -DADD_I128 -fsanitize=unsigned-integer-overflow %s -o %t && %t 2>&1 | FileCheck %s --check-prefix=ADD_I128

#include <stdint.h>

int main() {
  // These promote to 'int'.
  (void)(uint8_t(0xff) + uint8_t(0xff));
  (void)(uint16_t(0xf0fff) + uint16_t(0x0fff));

#ifdef ADD_I32
  uint32_t k = 0x87654321;
  k += 0xedcba987;
  // CHECK-ADD_I32: uadd-overflow.cpp:14:5: fatal error: unsigned integer overflow: 2271560481 + 3989547399 cannot be represented in type 'uint32_t' (aka 'unsigned int')
#endif

#ifdef ADD_I64
  (void)(uint64_t(10000000000000000000ull) + uint64_t(9000000000000000000ull));
  // CHECK-ADD_I64: 10000000000000000000 + 9000000000000000000 cannot be represented in type 'unsigned long'
#endif

#ifdef ADD_I128
  (void)((__uint128_t(1) << 127) + (__uint128_t(1) << 127));
  // CHECK-ADD_I128: 0x80000000000000000000000000000000 + 0x80000000000000000000000000000000 cannot be represented in type 'unsigned __int128'
#endif
}
