# REQUIRES: x86
# RUN: llvm-mc -filetype=obj -triple=x86_64-unknown-linux %s -o %t.o
# RUN: echo "SECTIONS { }" > %t0.script
# RUN: ld.lld -shared %t.o -o %t0.out --script %t0.script
# RUN: llvm-objdump -section-headers %t0.out | FileCheck %s --check-prefix=GOT
# RUN: llvm-objdump -s -section=.got -section=.got.plt %t0.out \
# RUN:   | FileCheck %s --check-prefix=GOTDATA

# GOT:     Sections:
# GOT:      9  .got         00000008 00000000000001b0 DATA
# GOT-NEXT: 10 .got.plt     00000020 00000000000001b8 DATA
# GOTDATA:     Contents of section .got:
# GOTDATA-NEXT:  01b0 00000000 00000000
# GOTDATA-NEXT: Contents of section .got.plt:
# GOTDATA-NEXT:  01b8 e0000000 00000000 00000000 00000000
# GOTDATA-NEXT:  01c8 00000000 00000000 d6000000 00000000

# RUN: echo "SECTIONS { .mygot : { *(.got) *(.got.plt) } }" > %t1.script
# RUN: ld.lld -shared %t.o -o %t1.out --script %t1.script
# RUN: llvm-objdump -section-headers %t1.out | FileCheck %s --check-prefix=MYGOT
# RUN: llvm-objdump -s -section=.mygot %t1.out | FileCheck %s --check-prefix=MYGOTDATA

# MYGOT:     Sections:
# MYGOT:      9  .mygot     00000028 00000000000001b0 DATA
# MYGOT-NOT:  .got
# MYGOT-NOT:  .got.plt
# MYGOTDATA:      01b0 00000000 00000000 e0000000 00000000
# MYGOTDATA-NEXT: 01c0 00000000 00000000 00000000 00000000
# MYGOTDATA-NEXT: 01d0 d6000000 00000000

mov bar@gotpcrel(%rip), %rax
call foo@plt
