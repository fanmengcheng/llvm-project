; RUN: llc < %s | FileCheck %s

target datalayout = "e-m:e-p:32:32-i64:64-v128:64:128-n32-S64"
target triple = "armv7--none-eabi"

define i32 @f(i64 %z) {
	ret i32 0
}

!llvm.module.flags = !{!0, !1}

!0 = metadata !{i32 1, metadata !"wchar_size", i32 4}
!1 = metadata !{i32 1, metadata !"min_enum_size", i32 1}

; CHECK: .eabi_attribute 18, 4   @ Tag_ABI_PCS_wchar_t
; CHECK: .eabi_attribute 26, 1   @ Tag_ABI_enum_size
