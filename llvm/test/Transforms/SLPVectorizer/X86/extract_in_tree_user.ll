; RUN: opt < %s -basicaa -slp-vectorizer -S -mtriple=i386-apple-macosx10.9.0 -mcpu=corei7-avx | FileCheck %s

target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"

@a = common global i64* null, align 8

; Function Attrs: nounwind ssp uwtable
define i32 @fn1() {
entry:
  %0 = load i64*, i64** @a, align 8
  %add.ptr = getelementptr inbounds i64, i64* %0, i64 11
  %1 = ptrtoint i64* %add.ptr to i64
  store i64 %1, i64* %add.ptr, align 8
  %add.ptr1 = getelementptr inbounds i64, i64* %0, i64 56
  %2 = ptrtoint i64* %add.ptr1 to i64
  %arrayidx2 = getelementptr inbounds i64, i64* %0, i64 12
  store i64 %2, i64* %arrayidx2, align 8
  ret i32 undef
; CHECK-LABEL: @fn1(
; CHECK: extractelement <2 x i64*>
; CHECK: ret
}


declare float @llvm.powi.f32(float, i32)
define void @fn2(i32* %a, i32* %b, float* %c) {
entry:
  %i0 = load i32, i32* %a, align 4
  %i1 = load i32, i32* %b, align 4
  %add1 = add i32 %i0, %i1
  %fp1 = sitofp i32 %add1 to float
  %call1 = tail call float @llvm.powi.f32(float %fp1,i32 %add1) nounwind readnone

  %arrayidx2 = getelementptr inbounds i32, i32* %a, i32 1
  %i2 = load i32, i32* %arrayidx2, align 4
  %arrayidx3 = getelementptr inbounds i32, i32* %b, i32 1
  %i3 = load i32, i32* %arrayidx3, align 4
  %add2 = add i32 %i2, %i3
  %fp2 = sitofp i32 %add2 to float
  %call2 = tail call float @llvm.powi.f32(float %fp2,i32 %add1) nounwind readnone

  %arrayidx4 = getelementptr inbounds i32, i32* %a, i32 2
  %i4 = load i32, i32* %arrayidx4, align 4
  %arrayidx5 = getelementptr inbounds i32, i32* %b, i32 2
  %i5 = load i32, i32* %arrayidx5, align 4
  %add3 = add i32 %i4, %i5
  %fp3 = sitofp i32 %add3 to float
  %call3 = tail call float @llvm.powi.f32(float %fp3,i32 %add1) nounwind readnone

  %arrayidx6 = getelementptr inbounds i32, i32* %a, i32 3
  %i6 = load i32, i32* %arrayidx6, align 4
  %arrayidx7 = getelementptr inbounds i32, i32* %b, i32 3
  %i7 = load i32, i32* %arrayidx7, align 4
  %add4 = add i32 %i6, %i7
  %fp4 = sitofp i32 %add4 to float
  %call4 = tail call float @llvm.powi.f32(float %fp4,i32 %add1) nounwind readnone

  store float %call1, float* %c, align 4
  %arrayidx8 = getelementptr inbounds float, float* %c, i32 1
  store float %call2, float* %arrayidx8, align 4
  %arrayidx9 = getelementptr inbounds float, float* %c, i32 2
  store float %call3, float* %arrayidx9, align 4
  %arrayidx10 = getelementptr inbounds float, float* %c, i32 3
  store float %call4, float* %arrayidx10, align 4
  ret void

; CHECK-LABEL: @fn2(
; CHECK: extractelement <4 x i32>
; CHECK: ret
}
