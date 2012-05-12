# RUN: not llvm-mc -triple i386-unknown-unknown %s |& FileCheck %s

.macro foo
.err
.endm

.purgem bar
# CHECK: error: macro 'bar' is not defined

.purgem foo
foo
# CHECK: error: invalid instruction mnemonic 'foo'
