; Check that various LLVM idioms get lowered to NVPTX as expected.

; RUN: llc < %s -march=nvptx -mcpu=sm_20 | FileCheck %s
; RUN: llc < %s -march=nvptx64 -mcpu=sm_20 | FileCheck %s

; CHECK-LABEL: abs_i16(
define i16 @abs_i16(i16 %a) {
; CHECK: abs.s16
  %neg = sub i16 0, %a
  %abs.cond = icmp sge i16 %a, 0
  %abs = select i1 %abs.cond, i16 %a, i16 %neg
  ret i16 %abs
}

; CHECK-LABEL: abs_i32(
define i32 @abs_i32(i32 %a) {
; CHECK: abs.s32
  %neg = sub i32 0, %a
  %abs.cond = icmp sge i32 %a, 0
  %abs = select i1 %abs.cond, i32 %a, i32 %neg
  ret i32 %abs
}

; CHECK-LABEL: abs_i64(
define i64 @abs_i64(i64 %a) {
; CHECK: abs.s64
  %neg = sub i64 0, %a
  %abs.cond = icmp sge i64 %a, 0
  %abs = select i1 %abs.cond, i64 %a, i64 %neg
  ret i64 %abs
}

; The speculate-and-select idiom here on llvm.sqrt should be lowered to the
; regular NVPTX sqrt instruction, as it has defined behavior on negative
; inputs.
;
; CHECK-LABEL: sqrt_f32(
define float @sqrt_f32(float %a) {
; CHECK-NOT: set.
; CHECK: sqrt.rn.f32
; CHECK-NOT: set.
  %1 = fcmp olt float %a, -0.000000e+00
  %sqrt = call float @llvm.sqrt.f32(float %a)
  %ret = select i1 %1, float 0x7FF8000000000000, float %sqrt
  ret float %ret
}
; CHECK-LABEL: sqrt_f64(
define double @sqrt_f64(double %a) {
; CHECK-NOT: set.
; CHECK: sqrt.rn.f64
; CHECK-NOT: set.
  %1 = fcmp olt double %a, -0.000000e+00
  %sqrt = call double @llvm.sqrt.f64(double %a)
  %ret = select i1 %1, double 0x7FF8000000000000, double %sqrt
  ret double %ret
}

declare float @llvm.sqrt.f32(float)
declare double @llvm.sqrt.f64(double)
