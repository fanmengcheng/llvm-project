; RUN: llc < %s -march=nvptx -mcpu=sm_20 -nvptx-prec-divf32=0 -nvptx-prec-sqrtf32=0 \
; RUN:   | FileCheck %s

target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v32:32:32-v64:64:64-v128:128:128-n16:32:64"

declare float @llvm.sqrt.f32(float)
declare double @llvm.sqrt.f64(double)

; CHECK-LABEL test_rsqrt32
define float @test_rsqrt32(float %a) #0 {
; CHECK: rsqrt.approx.f32
  %val = tail call float @llvm.sqrt.f32(float %a)
  %ret = fdiv float 1.0, %val
  ret float %ret
}

; CHECK-LABEL test_rsqrt_ftz32
define float @test_rsqrt_ftz32(float %a) #0 #1 {
; CHECK: rsqrt.approx.ftz.f32
  %val = tail call float @llvm.sqrt.f32(float %a)
  %ret = fdiv float 1.0, %val
  ret float %ret
}

; CHECK-LABEL test_rsqrt64
define double @test_rsqrt64(double %a) #0 {
; CHECK: rsqrt.approx.f64
  %val = tail call double @llvm.sqrt.f64(double %a)
  %ret = fdiv double 1.0, %val
  ret double %ret
}

attributes #0 = { "unsafe-fp-math" = "true" }
attributes #1 = { "nvptx-f32ftz" = "true" }
