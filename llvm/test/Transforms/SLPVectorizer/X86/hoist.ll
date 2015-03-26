; RUN: opt < %s -basicaa -slp-vectorizer -dce -S -mtriple=i386-apple-macosx10.8.0 -mcpu=corei7-avx | FileCheck %s

target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:128:128-n8:16:32-S128"
target triple = "i386-apple-macosx10.9.0"

;int foo(int *A, int n, int k) {
;  for (int i=0; i < 10000; i+=4) {
;    A[i]   += n;
;    A[i+1] += k;
;    A[i+2] += n;
;    A[i+3] += k;
;  }
;}

; preheader:
;CHECK: entry
;CHECK-NEXT: insertelement
;CHECK-NEXT: insertelement
;CHECK-NEXT: insertelement
;CHECK-NEXT: insertelement
; loop body:
;CHECK: phi
;CHECK: load <4 x i32>
;CHECK: add nsw <4 x i32>
;CHECK: store <4 x i32>
;CHECK: ret
define i32 @foo(i32* nocapture %A, i32 %n, i32 %k) {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %i.024 = phi i32 [ 0, %entry ], [ %add10, %for.body ]
  %arrayidx = getelementptr inbounds i32, i32* %A, i32 %i.024
  %0 = load i32, i32* %arrayidx, align 4
  %add = add nsw i32 %0, %n
  store i32 %add, i32* %arrayidx, align 4
  %add121 = or i32 %i.024, 1
  %arrayidx2 = getelementptr inbounds i32, i32* %A, i32 %add121
  %1 = load i32, i32* %arrayidx2, align 4
  %add3 = add nsw i32 %1, %k
  store i32 %add3, i32* %arrayidx2, align 4
  %add422 = or i32 %i.024, 2
  %arrayidx5 = getelementptr inbounds i32, i32* %A, i32 %add422
  %2 = load i32, i32* %arrayidx5, align 4
  %add6 = add nsw i32 %2, %n
  store i32 %add6, i32* %arrayidx5, align 4
  %add723 = or i32 %i.024, 3
  %arrayidx8 = getelementptr inbounds i32, i32* %A, i32 %add723
  %3 = load i32, i32* %arrayidx8, align 4
  %add9 = add nsw i32 %3, %k
  store i32 %add9, i32* %arrayidx8, align 4
  %add10 = add nsw i32 %i.024, 4
  %cmp = icmp slt i32 %add10, 10000
  br i1 %cmp, label %for.body, label %for.end

for.end:                                          ; preds = %for.body
  ret i32 undef
}

