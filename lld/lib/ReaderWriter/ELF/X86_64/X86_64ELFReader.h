//===- lib/ReaderWriter/ELF/X86_64/X86_64ELFReader.h ----------------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLD_READER_WRITER_X86_64_X86_64_ELF_READER_H
#define LLD_READER_WRITER_X86_64_X86_64_ELF_READER_H

#include "ELFReader.h"
#include "X86_64ELFFile.h"

namespace lld {
namespace elf {

struct X86_64DynamicFileCreateELFTraits {
  typedef llvm::ErrorOr<std::unique_ptr<lld::SharedLibraryFile>> result_type;

  template <class ELFT>
  static result_type create(std::unique_ptr<llvm::MemoryBuffer> mb,
                            bool useUndefines) {
    return lld::elf::X86_64DynamicFile<ELFT>::create(std::move(mb),
                                                     useUndefines);
  }
};

struct X86_64ELFFileCreateELFTraits {
  typedef llvm::ErrorOr<std::unique_ptr<lld::File>> result_type;

  template <class ELFT>
  static result_type create(std::unique_ptr<llvm::MemoryBuffer> mb,
                            bool atomizeStrings) {
    return lld::elf::X86_64ELFFile<ELFT>::create(std::move(mb), atomizeStrings);
  }
};

class X86_64ELFObjectReader : public ELFObjectReader {
public:
  X86_64ELFObjectReader(bool atomizeStrings)
      : ELFObjectReader(atomizeStrings) {}

  std::error_code
  parseFile(std::unique_ptr<MemoryBuffer> &mb, const class Registry &,
            std::vector<std::unique_ptr<File>> &result) const override {
    std::size_t maxAlignment =
        1ULL << llvm::countTrailingZeros(uintptr_t(mb->getBufferStart()));
    auto f = createELF<X86_64ELFFileCreateELFTraits>(
        llvm::object::getElfArchType(mb->getBuffer()), maxAlignment,
        std::move(mb), _atomizeStrings);
    if (std::error_code ec = f.getError())
      return ec;
    result.push_back(std::move(*f));
    return std::error_code();
  }
};

class X86_64ELFDSOReader : public ELFDSOReader {
public:
  X86_64ELFDSOReader(bool useUndefines) : ELFDSOReader(useUndefines) {}

  std::error_code
  parseFile(std::unique_ptr<MemoryBuffer> &mb, const class Registry &,
            std::vector<std::unique_ptr<File>> &result) const override {
    std::size_t maxAlignment =
        1ULL << llvm::countTrailingZeros(uintptr_t(mb->getBufferStart()));
    auto f = createELF<X86_64DynamicFileCreateELFTraits>(
        llvm::object::getElfArchType(mb->getBuffer()), maxAlignment,
        std::move(mb), _useUndefines);
    if (std::error_code ec = f.getError())
      return ec;
    result.push_back(std::move(*f));
    return std::error_code();
  }
};

} // namespace elf
} // namespace lld

#endif // LLD_READER_WRITER_ELF_X86_64_X86_64_READER_H
