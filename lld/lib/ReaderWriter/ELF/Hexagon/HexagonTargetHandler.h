//===- lib/ReaderWriter/ELF/Hexagon/HexagonTargetHandler.h ----------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLD_READER_WRITER_ELF_HEXAGON_TARGET_HANDLER_H
#define LLD_READER_WRITER_ELF_HEXAGON_TARGET_HANDLER_H

#include "DefaultTargetHandler.h"
#include "ExecutableAtoms.h"
#include "HexagonRelocationHandler.h"
#include "HexagonSectionChunks.h"
#include "TargetLayout.h"

namespace lld {
namespace elf {
typedef llvm::object::ELFType<llvm::support::little, 4, false> HexagonELFType;
class HexagonTargetInfo;


/// \brief Handle Hexagon specific Atoms
template <class HexagonELFType>
class HexagonTargetAtomHandler LLVM_FINAL :
    public TargetAtomHandler<HexagonELFType> {
  typedef llvm::object::Elf_Sym_Impl<HexagonELFType> Elf_Sym;
  typedef llvm::object::Elf_Shdr_Impl<HexagonELFType> Elf_Shdr;
public:

  virtual DefinedAtom::ContentType
  contentType(const ELFDefinedAtom<HexagonELFType> *atom) const {
    return contentType(atom->section(), atom->symbol());
  }

  virtual DefinedAtom::ContentType
  contentType(const Elf_Shdr *section, const Elf_Sym *sym) const {
    switch (sym->st_shndx) {
    // Common symbols
    case llvm::ELF::SHN_HEXAGON_SCOMMON:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_1:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_2:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_4:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_8:
      return DefinedAtom::typeZeroFillFast;

    default:
      if (section->sh_flags & llvm::ELF::SHF_HEX_GPREL)
        return DefinedAtom::typeDataFast;
      else
        llvm_unreachable("unknown symbol type");
    }
  }

  virtual DefinedAtom::ContentPermissions
  contentPermissions(const ELFDefinedAtom<HexagonELFType> *atom) const {
    // All of the hexagon specific symbols belong in the data segment
    return DefinedAtom::permRW_;
  }

  virtual int64_t getType(const Elf_Sym *sym) const {
    switch (sym->st_shndx) {
    // Common symbols
    case llvm::ELF::SHN_HEXAGON_SCOMMON:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_1:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_2:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_4:
    case llvm::ELF::SHN_HEXAGON_SCOMMON_8:
      return llvm::ELF::STT_COMMON;

    default:
      return sym->getType();
    }
  }
};

/// \brief TargetLayout for Hexagon
template <class HexagonELFType>
class HexagonTargetLayout LLVM_FINAL : public TargetLayout<HexagonELFType> {

public:
  enum HexagonSectionOrder {
    ORDER_SDATA = 205
  };

  HexagonTargetLayout(const HexagonTargetInfo &hti)
      : TargetLayout<HexagonELFType>(hti), _sdataSection(nullptr) {
    _sdataSection = new (_alloc) SDataSection<HexagonELFType>(hti);
  }

  /// \brief Return the section order for a input section
  virtual Layout::SectionOrder getSectionOrder(
      StringRef name, int32_t contentType, int32_t contentPermissions) {
    if ((contentType == DefinedAtom::typeDataFast) ||
       (contentType == DefinedAtom::typeZeroFillFast))
      return ORDER_SDATA;

    return DefaultLayout<HexagonELFType>::getSectionOrder(name, contentType,
                                                          contentPermissions);
  }

  /// \brief This maps the input sections to the output section names
  virtual StringRef getSectionName(StringRef name, const int32_t contentType,
                                   const int32_t contentPermissions) {
    if ((contentType == DefinedAtom::typeDataFast) ||
       (contentType == DefinedAtom::typeZeroFillFast))
      return ".sdata";
    return DefaultLayout<HexagonELFType>::getSectionName(name, contentType,
                                                         contentPermissions);
  }

  /// \brief Gets or creates a section.
  virtual AtomSection<HexagonELFType> *
  createSection(StringRef name, int32_t contentType,
                DefinedAtom::ContentPermissions contentPermissions,
                Layout::SectionOrder sectionOrder) {
    if ((contentType == DefinedAtom::typeDataFast) ||
       (contentType == DefinedAtom::typeZeroFillFast))
      return _sdataSection;
    return DefaultLayout<HexagonELFType>::createSection(
        name, contentType, contentPermissions, sectionOrder);
  }

  /// \brief get the segment type for the section thats defined by the target
  virtual Layout::SegmentType
  getSegmentType(Section<HexagonELFType> *section) const {
    if (section->order() == ORDER_SDATA)
      return PT_LOAD;

    return DefaultLayout<HexagonELFType>::getSegmentType(section);
  }

private:
  llvm::BumpPtrAllocator _alloc;
  SDataSection<HexagonELFType> *_sdataSection;
};

/// \brief TargetHandler for Hexagon 
class HexagonTargetHandler LLVM_FINAL :
    public DefaultTargetHandler<HexagonELFType> {
public:
  HexagonTargetHandler(HexagonTargetInfo &targetInfo);

  virtual TargetLayout<HexagonELFType> &targetLayout() {
    return _targetLayout;
  }

  virtual TargetAtomHandler<HexagonELFType> &targetAtomHandler() {
    return _targetAtomHandler;
  }

  virtual const HexagonTargetRelocationHandler &getRelocationHandler() const {
    return _relocationHandler;
  }

private:
  HexagonTargetRelocationHandler _relocationHandler;
  HexagonTargetLayout<HexagonELFType> _targetLayout;
  HexagonTargetAtomHandler<HexagonELFType> _targetAtomHandler;
};
} // end namespace elf
} // end namespace lld

#endif
