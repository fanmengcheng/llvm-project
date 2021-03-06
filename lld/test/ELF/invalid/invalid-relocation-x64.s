## invalid-relocation-x64.elf contains relocations with invalid relocation number.
## Next yaml code was used to create initial binary. After that it
## was modified with hex-editor to replace known relocations with fake ones,
## that have 0x98 and 0x98 numbers.
!ELF
FileHeader:
  Class:           ELFCLASS64
  Data:            ELFDATA2LSB
  OSABI:           ELFOSABI_FREEBSD
  Type:            ET_REL
  Machine:         EM_X86_64
Sections:
  - Name:            .text
    Type:            SHT_PROGBITS
    Flags:           [ SHF_ALLOC ]
  - Name:            .rela.text
    Type:            SHT_RELA
    Link:            .symtab
    Info:            .text
    Relocations:
      - Offset:          0x0000000000000000
        Symbol:          ''
        Type:            R_X86_64_NONE
      - Offset:          0x0000000000000000
        Symbol:          ''
        Type:            R_X86_64_NONE

# RUN: not ld.lld %p/Inputs/invalid-relocation-x64.elf -o %t2 2>&1 | FileCheck %s
# CHECK: unrecognized reloc 152
# CHECK: unrecognized reloc 153
