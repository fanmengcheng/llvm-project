; RUN: opt %loadPolly %defaultOpts -polly-codegen -enable-polly-vector  -dce -S %s | FileCheck %s
; ModuleID = 'simple_vec_stride_x.s'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-unknown-linux-gnu"

@A = common global [1024 x float] zeroinitializer, align 16
@B = common global [1024 x float] zeroinitializer, align 16

define void @simple_vec_stride_x() nounwind {
bb:
  br label %bb2

bb2:                                              ; preds = %bb5, %bb
  %indvar = phi i64 [ %indvar.next, %bb5 ], [ 0, %bb ]
  %tmp = mul i64 %indvar, 2
  %scevgep = getelementptr [1024 x float]* @B, i64 0, i64 %tmp
  %scevgep1 = getelementptr [1024 x float]* @A, i64 0, i64 %tmp
  %exitcond = icmp ne i64 %indvar, 4
  br i1 %exitcond, label %bb3, label %bb6

bb3:                                              ; preds = %bb2
  %tmp4 = load float* %scevgep1, align 8
  store float %tmp4, float* %scevgep, align 8
  br label %bb5

bb5:                                              ; preds = %bb3
  %indvar.next = add i64 %indvar, 1
  br label %bb2

bb6:                                              ; preds = %bb2
  ret void
}

define i32 @main() nounwind {
bb:
  call void @simple_vec_stride_x()
  %tmp = load float* getelementptr inbounds ([1024 x float]* @A, i64 0, i64 42), align 8
  %tmp1 = fptosi float %tmp to i32
  ret i32 %tmp1
}

; CHECK: load float* %p_scevgep1.moved.to.bb3
; CHECK: insertelement <4 x float> undef, float %tmp4_p_scalar_, i32 0
; CHECK: load float* %p_scevgep1.moved.to.bb34
; CHECK: insertelement <4 x float> %tmp4_p_vec_, float %tmp4_p_scalar_10, i32 1
; CHECK: load float* %p_scevgep1.moved.to.bb35
; CHECK: insertelement <4 x float> %tmp4_p_vec_11, float %tmp4_p_scalar_12, i32 2
; CHECK: load float* %p_scevgep1.moved.to.bb36
; CHECK: insertelement <4 x float> %tmp4_p_vec_13, float %tmp4_p_scalar_14, i32 3
; CHECK: extractelement <4 x float> %tmp4_p_vec_15, i32 0
; CHECK: store float %0, float* %p_scevgep.moved.to.bb3
; CHECK: extractelement <4 x float> %tmp4_p_vec_15, i32 1
; CHECK: store float %1, float* %p_scevgep.moved.to.bb37
; CHECK: extractelement <4 x float> %tmp4_p_vec_15, i32 2
; CHECK: store float %2, float* %p_scevgep.moved.to.bb38
; CHECK: extractelement <4 x float> %tmp4_p_vec_15, i32 3
; CHECK: store float %3, float* %p_scevgep.moved.to.bb39


