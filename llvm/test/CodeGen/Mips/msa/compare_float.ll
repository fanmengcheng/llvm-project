; RUN: llc -march=mips -mattr=+msa < %s | FileCheck %s

declare <4 x float> @llvm.mips.fmax.w(<4 x float>, <4 x float>) nounwind
declare <2 x double> @llvm.mips.fmax.d(<2 x double>, <2 x double>) nounwind
declare <4 x float> @llvm.mips.fmin.w(<4 x float>, <4 x float>) nounwind
declare <2 x double> @llvm.mips.fmin.d(<2 x double>, <2 x double>) nounwind

define void @false_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: false_v4f32:

  %1 = load <4 x float>* %a
  %2 = load <4 x float>* %b
  %3 = fcmp false <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  store <4 x i32> %4, <4 x i32>* %c
  ret void

  ; (setcc $a, $b, SETFALSE) is always folded, so we won't get fcaf:
  ; CHECK-DAG: ldi.b [[R1:\$w[0-9]+]], 0
  ; CHECK-DAG: st.b [[R1]], 0($4)
  ; CHECK: .size false_v4f32
}

define void @false_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: false_v2f64:

  %1 = load <2 x double>* %a
  %2 = load <2 x double>* %b
  %3 = fcmp false <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  store <2 x i64> %4, <2 x i64>* %c
  ret void

  ; FIXME: This code is correct, but poor. Ideally it would be similar to
  ;        the code in @false_v4f32
  ; CHECK-DAG: ldi.b [[R1:\$w[0-9]+]], 0
  ; CHECK-DAG: slli.d [[R3:\$w[0-9]+]], [[R1]], 63
  ; CHECK-DAG: srai.d [[R4:\$w[0-9]+]], [[R3]], 63
  ; CHECK-DAG: st.d [[R4]], 0($4)
  ; CHECK: .size false_v2f64
}

define void @oeq_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: oeq_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp oeq <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fceq.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size oeq_v4f32
}

define void @oeq_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: oeq_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp oeq <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fceq.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size oeq_v2f64
}

define void @oge_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: oge_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp oge <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcle.w [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size oge_v4f32
}

define void @oge_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: oge_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp oge <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcle.d [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size oge_v2f64
}

define void @ogt_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: ogt_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ogt <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fclt.w [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size ogt_v4f32
}

define void @ogt_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: ogt_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ogt <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fclt.d [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size ogt_v2f64
}

define void @ole_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: ole_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ole <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcle.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size ole_v4f32
}

define void @ole_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: ole_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ole <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcle.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size ole_v2f64
}

define void @olt_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: olt_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp olt <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fclt.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size olt_v4f32
}

define void @olt_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: olt_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp olt <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fclt.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size olt_v2f64
}

define void @one_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: one_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp one <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcne.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size one_v4f32
}

define void @one_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: one_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp one <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcne.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size one_v2f64
}

define void @ord_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: ord_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ord <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcor.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size ord_v4f32
}

define void @ord_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: ord_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ord <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcor.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size ord_v2f64
}

define void @ueq_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: ueq_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ueq <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcueq.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size ueq_v4f32
}

define void @ueq_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: ueq_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ueq <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcueq.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size ueq_v2f64
}

define void @uge_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: uge_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp uge <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcule.w [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size uge_v4f32
}

define void @uge_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: uge_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp uge <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcule.d [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size uge_v2f64
}

define void @ugt_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: ugt_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ugt <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcult.w [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size ugt_v4f32
}

define void @ugt_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: ugt_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ugt <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcult.d [[R3:\$w[0-9]+]], [[R2]], [[R1]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size ugt_v2f64
}

define void @ule_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: ule_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ule <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcule.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size ule_v4f32
}

define void @ule_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: ule_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ule <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcule.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size ule_v2f64
}

define void @ult_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: ult_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ult <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcult.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size ult_v4f32
}

define void @ult_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: ult_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp ult <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcult.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size ult_v2f64
}

define void @uno_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: uno_v4f32:

  %1 = load <4 x float>* %a
  ; CHECK-DAG: ld.w [[R1:\$w[0-9]+]], 0($5)
  %2 = load <4 x float>* %b
  ; CHECK-DAG: ld.w [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp uno <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  ; CHECK-DAG: fcun.w [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <4 x i32> %4, <4 x i32>* %c
  ; CHECK-DAG: st.w [[R3]], 0($4)

  ret void
  ; CHECK: .size uno_v4f32
}

define void @uno_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: uno_v2f64:

  %1 = load <2 x double>* %a
  ; CHECK-DAG: ld.d [[R1:\$w[0-9]+]], 0($5)
  %2 = load <2 x double>* %b
  ; CHECK-DAG: ld.d [[R2:\$w[0-9]+]], 0($6)
  %3 = fcmp uno <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  ; CHECK-DAG: fcun.d [[R3:\$w[0-9]+]], [[R1]], [[R2]]
  store <2 x i64> %4, <2 x i64>* %c
  ; CHECK-DAG: st.d [[R3]], 0($4)

  ret void
  ; CHECK: .size uno_v2f64
}

define void @true_v4f32(<4 x i32>* %c, <4 x float>* %a, <4 x float>* %b) nounwind {
  ; CHECK: true_v4f32:

  %1 = load <4 x float>* %a
  %2 = load <4 x float>* %b
  %3 = fcmp true <4 x float> %1, %2
  %4 = sext <4 x i1> %3 to <4 x i32>
  store <4 x i32> %4, <4 x i32>* %c
  ret void

  ; (setcc $a, $b, SETTRUE) is always folded, so we won't get fcaf:
  ; CHECK-DAG: ldi.b [[R1:\$w[0-9]+]], -1
  ; CHECK-DAG: st.b [[R1]], 0($4)
  ; CHECK: .size true_v4f32
}

define void @true_v2f64(<2 x i64>* %c, <2 x double>* %a, <2 x double>* %b) nounwind {
  ; CHECK: true_v2f64:

  %1 = load <2 x double>* %a
  %2 = load <2 x double>* %b
  %3 = fcmp true <2 x double> %1, %2
  %4 = sext <2 x i1> %3 to <2 x i64>
  store <2 x i64> %4, <2 x i64>* %c
  ret void

  ; FIXME: This code is correct, but poor. Ideally it would be similar to
  ;        the code in @true_v4f32
  ; CHECK-DAG: ldi.d [[R1:\$w[0-9]+]], 1
  ; CHECK-DAG: slli.d [[R3:\$w[0-9]+]], [[R1]], 63
  ; CHECK-DAG: srai.d [[R4:\$w[0-9]+]], [[R3]], 63
  ; CHECK-DAG: st.d [[R4]], 0($4)
  ; CHECK: .size true_v2f64
}
