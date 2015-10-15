# RUN: llvm-mc %s -triple=mips-unknown-linux -show-encoding -mcpu=mips64r6 -mattr=micromips | FileCheck %s
a:
        .set noat
        addiur1sp $7, 4          # CHECK: addiur1sp $7, 4     # encoding: [0x6f,0x83]
        addiur2 $6, $7, -1       # CHECK: addiur2 $6, $7, -1  # encoding: [0x6f,0x7e]
        addiur2 $6, $7, 12       # CHECK: addiur2 $6, $7, 12  # encoding: [0x6f,0x76]
        addius5 $7, -2           # CHECK: addius5 $7, -2      # encoding: [0x4c,0xfc]
        addiusp -1028            # CHECK: addiusp -1028       # encoding: [0x4f,0xff]
        addiusp -1032            # CHECK: addiusp -1032       # encoding: [0x4f,0xfd]
        addiusp 1024             # CHECK: addiusp 1024        # encoding: [0x4c,0x01]
        addiusp 1028             # CHECK: addiusp 1028        # encoding: [0x4c,0x03]
        addiusp -16              # CHECK: addiusp -16         # encoding: [0x4f,0xf9]
        b 132                    # CHECK: bc16 132            # encoding: [0xcc,0x42]
        bc16 132                 # CHECK: bc16 132            # encoding: [0xcc,0x42]
        beqzc16 $6, 20           # CHECK: beqzc16 $6, 20      # encoding: [0x8f,0x0a]
        bnezc16 $6, 20           # CHECK: bnezc16 $6, 20      # encoding: [0xaf,0x0a]
        daui $3, $4, 5           # CHECK: daui $3, $4, 5      # encoding: [0xf0,0x64,0x00,0x05]
        dahi $3, 4               # CHECK: dahi $3, 4          # encoding: [0x42,0x23,0x00,0x04]
        dati $3, 4               # CHECK: dati $3, 4          # encoding: [0x42,0x03,0x00,0x04]
        dext $9, $6, 3, 7        # CHECK: dext $9, $6, 3, 7   # encoding: [0x59,0x26,0x30,0xec]
        dextm $9, $6, 3, 7       # CHECK: dextm $9, $6, 3, 7  # encoding: [0x59,0x26,0x30,0xe4]
        dextu $9, $6, 3, 7       # CHECK: dextu $9, $6, 3, 7  # encoding: [0x59,0x26,0x30,0xd4]
        dalign $4, $2, $3, 5     # CHECK: dalign $4, $2, $3, 5  # encoding: [0x58,0x43,0x25,0x1c]
        lw $3, 32($gp)           # CHECK: lw $3, 32($gp)        # encoding: [0x65,0x88]
        lw $3, 24($sp)           # CHECK: lw $3, 24($sp)        # encoding: [0x48,0x66]
        lw16 $4, 8($17)          # CHECK: lw16 $4, 8($17)       # encoding: [0x6a,0x12]
        lhu16 $3, 4($16)         # CHECK: lhu16 $3, 4($16)      # encoding: [0x29,0x82]
        lbu16 $3, 4($17)         # CHECK: lbu16 $3, 4($17)      # encoding: [0x09,0x94]
        lbu16 $3, -1($17)        # CHECK: lbu16 $3, -1($17)     # encoding: [0x09,0x9f]
        ddiv $3, $4, $5          # CHECK: ddiv $3, $4, $5     # encoding: [0x58,0x64,0x29,0x18]
        dmod $3, $4, $5          # CHECK: dmod $3, $4, $5     # encoding: [0x58,0x64,0x29,0x58]
        ddivu $3, $4, $5         # CHECK: ddivu $3, $4, $5    # encoding: [0x58,0x64,0x29,0x98]
        dmodu $3, $4, $5         # CHECK: dmodu $3, $4, $5    # encoding: [0x58,0x64,0x29,0xd8]
        add.s $f3, $f4, $f5      # CHECK: add.s $f3, $f4, $f5 # encoding: [0x54,0xa4,0x18,0x30]
        add.d $f2, $f4, $f6      # CHECK: add.d $f2, $f4, $f6 # encoding: [0x54,0xc4,0x11,0x30]
        sub.s $f3, $f4, $f5      # CHECK: sub.s $f3, $f4, $f5 # encoding: [0x54,0xa4,0x18,0x70]
        sub.d $f2, $f4, $f6      # CHECK: sub.d $f2, $f4, $f6 # encoding: [0x54,0xc4,0x11,0x70]
        mul.s $f3, $f4, $f5      # CHECK: mul.s $f3, $f4, $f5 # encoding: [0x54,0xa4,0x18,0xb0]
        mul.d $f2, $f4, $f6      # CHECK: mul.d $f2, $f4, $f6 # encoding: [0x54,0xc4,0x11,0xb0]
        div.s $f3, $f4, $f5      # CHECK: div.s $f3, $f4, $f5 # encoding: [0x54,0xa4,0x18,0xf0]
        div.d $f2, $f4, $f6      # CHECK: div.d $f2, $f4, $f6 # encoding: [0x54,0xc4,0x11,0xf0]
        maddf.s $f3, $f4, $f5    # CHECK: maddf.s $f3, $f4, $f5 # encoding: [0x54,0xa4,0x19,0xb8]
        maddf.d $f3, $f4, $f5    # CHECK: maddf.d $f3, $f4, $f5 # encoding: [0x54,0xa4,0x1b,0xb8]
        msubf.s $f3, $f4, $f5    # CHECK: msubf.s $f3, $f4, $f5 # encoding: [0x54,0xa4,0x19,0xf8]
        msubf.d $f3, $f4, $f5    # CHECK: msubf.d $f3, $f4, $f5 # encoding: [0x54,0xa4,0x1b,0xf8]
        mov.s $f6, $f7           # CHECK: mov.s $f6, $f7      # encoding: [0x54,0xc7,0x00,0x7b]
        mov.d $f4, $f6           # CHECK: mov.d $f4, $f6      # encoding: [0x54,0x86,0x20,0x7b]
        neg.s $f6, $f7           # CHECK: neg.s $f6, $f7      # encoding: [0x54,0xc7,0x0b,0x7b]
        neg.d $f4, $f6           # CHECK: neg.d $f4, $f6      # encoding: [0x54,0x86,0x2b,0x7b]
        max.s $f5, $f4, $f3      # CHECK: max.s $f5, $f4, $f3      # encoding: [0x54,0x64,0x28,0x0b]
        max.d $f5, $f4, $f3      # CHECK: max.d $f5, $f4, $f3      # encoding: [0x54,0x64,0x2a,0x0b]
        maxa.s $f5, $f4, $f3     # CHECK: maxa.s $f5, $f4, $f3     # encoding: [0x54,0x64,0x28,0x2b]
        maxa.d $f5, $f4, $f3     # CHECK: maxa.d $f5, $f4, $f3     # encoding: [0x54,0x64,0x2a,0x2b]
        min.s $f5, $f4, $f3      # CHECK: min.s $f5, $f4, $f3      # encoding: [0x54,0x64,0x28,0x03]
        min.d $f5, $f4, $f3      # CHECK: min.d $f5, $f4, $f3      # encoding: [0x54,0x64,0x2a,0x03]
        mina.s $f5, $f4, $f3     # CHECK: mina.s $f5, $f4, $f3     # encoding: [0x54,0x64,0x28,0x23]
        mina.d $f5, $f4, $f3     # CHECK: mina.d $f5, $f4, $f3     # encoding: [0x54,0x64,0x2a,0x23]
        cmp.af.s $f2, $f3, $f4   # CHECK: cmp.af.s $f2, $f3, $f4   # encoding: [0x54,0x83,0x10,0x05]
        cmp.af.d $f2, $f3, $f4   # CHECK: cmp.af.d $f2, $f3, $f4   # encoding: [0x54,0x83,0x10,0x15]
        cmp.un.s $f2, $f3, $f4   # CHECK: cmp.un.s $f2, $f3, $f4   # encoding: [0x54,0x83,0x10,0x45]
        cmp.un.d $f2, $f3, $f4   # CHECK: cmp.un.d $f2, $f3, $f4   # encoding: [0x54,0x83,0x10,0x55]
        cmp.eq.s $f2, $f3, $f4   # CHECK: cmp.eq.s $f2, $f3, $f4   # encoding: [0x54,0x83,0x10,0x85]
        cmp.eq.d $f2, $f3, $f4   # CHECK: cmp.eq.d $f2, $f3, $f4   # encoding: [0x54,0x83,0x10,0x95]
        cmp.ueq.s $f2, $f3, $f4  # CHECK: cmp.ueq.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x10,0xc5]
        cmp.ueq.d $f2, $f3, $f4  # CHECK: cmp.ueq.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x10,0xd5]
        cmp.lt.s $f2, $f3, $f4   # CHECK: cmp.lt.s  $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0x05]
        cmp.lt.d $f2, $f3, $f4   # CHECK: cmp.lt.d  $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0x15]
        cmp.ult.s $f2, $f3, $f4  # CHECK: cmp.ult.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0x45]
        cmp.ult.d $f2, $f3, $f4  # CHECK: cmp.ult.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0x55]
        cmp.le.s $f2, $f3, $f4   # CHECK: cmp.le.s  $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0x85]
        cmp.le.d $f2, $f3, $f4   # CHECK: cmp.le.d  $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0x95]
        cmp.ule.s $f2, $f3, $f4  # CHECK: cmp.ule.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0xc5]
        cmp.ule.d $f2, $f3, $f4  # CHECK: cmp.ule.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x11,0xd5]
        cmp.saf.s $f2, $f3, $f4  # CHECK: cmp.saf.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x12,0x05]
        cmp.saf.d $f2, $f3, $f4  # CHECK: cmp.saf.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x12,0x15]
        cmp.sun.s $f2, $f3, $f4  # CHECK: cmp.sun.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x12,0x45]
        cmp.sun.d $f2, $f3, $f4  # CHECK: cmp.sun.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x12,0x55]
        cmp.seq.s $f2, $f3, $f4  # CHECK: cmp.seq.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x12,0x85]
        cmp.seq.d $f2, $f3, $f4  # CHECK: cmp.seq.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x12,0x95]
        cmp.sueq.s $f2, $f3, $f4 # CHECK: cmp.sueq.s $f2, $f3, $f4 # encoding: [0x54,0x83,0x12,0xc5]
        cmp.sueq.d $f2, $f3, $f4 # CHECK: cmp.sueq.d $f2, $f3, $f4 # encoding: [0x54,0x83,0x12,0xd5]
        cmp.slt.s $f2, $f3, $f4  # CHECK: cmp.slt.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x13,0x05]
        cmp.slt.d $f2, $f3, $f4  # CHECK: cmp.slt.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x13,0x15]
        cmp.sult.s $f2, $f3, $f4 # CHECK: cmp.sult.s $f2, $f3, $f4 # encoding: [0x54,0x83,0x13,0x45]
        cmp.sult.d $f2, $f3, $f4 # CHECK: cmp.sult.d $f2, $f3, $f4 # encoding: [0x54,0x83,0x13,0x55]
        cmp.sle.s $f2, $f3, $f4  # CHECK: cmp.sle.s $f2, $f3, $f4  # encoding: [0x54,0x83,0x13,0x85]
        cmp.sle.d $f2, $f3, $f4  # CHECK: cmp.sle.d $f2, $f3, $f4  # encoding: [0x54,0x83,0x13,0x95]
        cmp.sule.s $f2, $f3, $f4 # CHECK: cmp.sule.s $f2, $f3, $f4 # encoding: [0x54,0x83,0x13,0xc5]
        cmp.sule.d $f2, $f3, $f4 # CHECK: cmp.sule.d $f2, $f3, $f4 # encoding: [0x54,0x83,0x13,0xd5]
        cvt.l.s $f3, $f4         # CHECK: cvt.l.s $f3, $f4         # encoding: [0x54,0x64,0x01,0x3b]
        cvt.l.d $f3, $f4         # CHECK: cvt.l.d $f3, $f4         # encoding: [0x54,0x64,0x41,0x3b]
        cvt.w.s $f3, $f4         # CHECK: cvt.w.s $f3, $f4         # encoding: [0x54,0x64,0x09,0x3b]
        cvt.w.d $f3, $f4         # CHECK: cvt.w.d $f3, $f4         # encoding: [0x54,0x64,0x49,0x3b]
        cvt.d.s $f2, $f4         # CHECK: cvt.d.s $f2, $f4         # encoding: [0x54,0x44,0x13,0x7b]
        cvt.d.w $f2, $f4         # CHECK: cvt.d.w $f2, $f4         # encoding: [0x54,0x44,0x33,0x7b]
        cvt.d.l $f2, $f4         # CHECK: cvt.d.l $f2, $f4         # encoding: [0x54,0x44,0x53,0x7b]
        cvt.s.d $f2, $f4         # CHECK: cvt.s.d $f2, $f4         # encoding: [0x54,0x44,0x1b,0x7b]
        cvt.s.w $f3, $f4         # CHECK: cvt.s.w $f3, $f4         # encoding: [0x54,0x64,0x3b,0x7b]
        cvt.s.l $f3, $f4         # CHECK: cvt.s.l $f3, $f4         # encoding: [0x54,0x64,0x5b,0x7b]
        teq $8, $9               # CHECK: teq $8, $9          # encoding: [0x01,0x28,0x00,0x3c]
        teq $5, $7, 15           # CHECK: teq $5, $7, 15      # encoding: [0x00,0xe5,0xf0,0x3c]
        tge $7, $10              # CHECK: tge $7, $10         # encoding: [0x01,0x47,0x02,0x3c]
        tge $7, $19, 15          # CHECK: tge $7, $19, 15     # encoding: [0x02,0x67,0xf2,0x3c]
        tgeu $22, $gp            # CHECK: tgeu $22, $gp       # encoding: [0x03,0x96,0x04,0x3c]
        tgeu $20, $14, 15        # CHECK: tgeu $20, $14, 15   # encoding: [0x01,0xd4,0xf4,0x3c]
        tlt $15, $13             # CHECK: tlt $15, $13        # encoding: [0x01,0xaf,0x08,0x3c]
        tlt $2, $19, 15          # CHECK: tlt $2, $19, 15     # encoding: [0x02,0x62,0xf8,0x3c]
        tltu $11, $16            # CHECK: tltu $11, $16       # encoding: [0x02,0x0b,0x0a,0x3c]
        tltu $16, $sp, 15        # CHECK: tltu $16, $sp, 15   # encoding: [0x03,0xb0,0xfa,0x3c]
        tne $6, $17              # CHECK: tne $6, $17         # encoding: [0x02,0x26,0x0c,0x3c]
        tne $7, $8, 15           # CHECK: tne $7, $8, 15      # encoding: [0x01,0x07,0xfc,0x3c]
        cachee 1, 8($5)          # CHECK: cachee 1, 8($5)     # encoding: [0x60,0x25,0xa6,0x08]
        wrpgpr $3, $4            # CHECK: wrpgpr $3, $4       # encoding: [0x00,0x64,0xf1,0x7c]
        wsbh $3, $4              # CHECK: wsbh $3, $4         # encoding: [0x00,0x64,0x7b,0x3c]
        jalr $9                  # CHECK: jalr $9             # encoding: [0x45,0x2b]
        jrc16 $9                 # CHECK: jrc16 $9            # encoding: [0x45,0x23]
        jrcaddiusp 20            # CHECK: jrcaddiusp 20       # encoding: [0x44,0xb3]
        break16 8                # CHECK: break16 8                # encoding: [0x46,0x1b]
        li16 $3, -1              # CHECK: li16 $3, -1              # encoding: [0xed,0xff]
        move16 $3, $5            # CHECK: move16 $3, $5            # encoding: [0x0c,0x65]
        sdbbp16 8                # CHECK: sdbbp16 8                # encoding: [0x46,0x3b]
        subu16 $5, $16, $3       # CHECK: subu16 $5, $16, $3       # encoding: [0x04,0x3b]
        xor16 $17, $5            # CHECK: xor16 $17, $5            # encoding: [0x44,0xd8]

1:
