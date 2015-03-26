; RUN: opt %loadPolly -polly-detect-unprofitable -polly-code-generator=isl -polly-detect -analyze < %s | FileCheck %s
;
; CHECK: Valid Region for Scop:
;
;    void jd(int *A, int *B, int c) {
;      for (int i = 0; i < 1024; i++)
;        A[i] = B[c];
;    }
;
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"

define void @jd(i32* %A, i32* %B, i32 %c) {
entry:
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %indvars.iv = phi i64 [ %indvars.iv.next, %for.inc ], [ 0, %entry ]
  %exitcond = icmp ne i64 %indvars.iv, 1024
  br i1 %exitcond, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %idxprom = sext i32 %c to i64
  %arrayidx = getelementptr inbounds i32, i32* %B, i64 %idxprom
  %tmp = load i32, i32* %arrayidx, align 4
  %arrayidx2 = getelementptr inbounds i32, i32* %A, i64 %indvars.iv
  store i32 %tmp, i32* %arrayidx2, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}
