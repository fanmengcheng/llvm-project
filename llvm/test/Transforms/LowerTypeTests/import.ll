; RUN: opt -S -lowertypetests -lowertypetests-summary-action=import -lowertypetests-read-summary=%S/Inputs/import.yaml < %s | FileCheck %s

target datalayout = "e-p:64:64"

declare i1 @llvm.type.test(i8* %ptr, metadata %bitset) nounwind readnone

; CHECK-DAG: @__typeid_single_global_addr = external hidden global i8
; CHECK-DAG: @__typeid_inline6_global_addr = external hidden global i8
; CHECK-DAG: @__typeid_inline6_align = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_inline6_size_m1 = external hidden global i8, !absolute_symbol !1
; CHECK-DAG: @__typeid_inline6_inline_bits = external hidden global i8, !absolute_symbol !2
; CHECK-DAG: @__typeid_inline5_global_addr = external hidden global i8
; CHECK-DAG: @__typeid_inline5_align = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_inline5_size_m1 = external hidden global i8, !absolute_symbol !3
; CHECK-DAG: @__typeid_inline5_inline_bits = external hidden global i8, !absolute_symbol !4
; CHECK-DAG: @__typeid_bytearray32_global_addr = external hidden global i8
; CHECK-DAG: @__typeid_bytearray32_align = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_bytearray32_size_m1 = external hidden global i8, !absolute_symbol !4
; CHECK-DAG: @__typeid_bytearray32_byte_array = external hidden global i8
; CHECK-DAG: @__typeid_bytearray32_bit_mask = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_bytearray7_global_addr = external hidden global i8
; CHECK-DAG: @__typeid_bytearray7_align = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_bytearray7_size_m1 = external hidden global i8, !absolute_symbol !5
; CHECK-DAG: @__typeid_bytearray7_byte_array = external hidden global i8
; CHECK-DAG: @__typeid_bytearray7_bit_mask = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_allones32_global_addr = external hidden global i8
; CHECK-DAG: @__typeid_allones32_align = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_allones32_size_m1 = external hidden global i8, !absolute_symbol !4
; CHECK-DAG: @__typeid_allones7_global_addr = external hidden global i8
; CHECK-DAG: @__typeid_allones7_align = external hidden global i8, !absolute_symbol !0
; CHECK-DAG: @__typeid_allones7_size_m1 = external hidden global i8, !absolute_symbol !5

; CHECK: define i1 @allones7(i8* [[p:%.*]])
define i1 @allones7(i8* %p) {
  ; CHECK-NEXT: [[pi:%.*]] = ptrtoint i8* [[p]] to i64
  ; CHECK-NEXT: [[sub:%.*]] = sub i64 [[pi]], ptrtoint (i8* @__typeid_allones7_global_addr to i64)
  ; CHECK-NEXT: [[lshr:%.*]] = lshr i64 [[sub]], zext (i8 ptrtoint (i8* @__typeid_allones7_align to i8) to i64)
  ; CHECK-NEXT: [[shl:%.*]] = shl i64 [[sub]], zext (i8 sub (i8 64, i8 ptrtoint (i8* @__typeid_allones7_align to i8)) to i64)
  ; CHECK-NEXT: [[or:%.*]] = or i64 [[lshr]], [[shl]]
  ; CHECK-NEXT: [[ule:%.*]] = icmp ule i64 [[or]], ptrtoint (i8* @__typeid_allones7_size_m1 to i64)
  ; CHECK-NEXT: ret i1 [[ule]]
  %x = call i1 @llvm.type.test(i8* %p, metadata !"allones7")
  ret i1 %x
}

; CHECK: define i1 @allones32(i8* [[p:%.*]])
define i1 @allones32(i8* %p) {
  ; CHECK-NEXT: [[pi:%.*]] = ptrtoint i8* [[p]] to i64
  ; CHECK-NEXT: [[sub:%.*]] = sub i64 [[pi]], ptrtoint (i8* @__typeid_allones32_global_addr to i64)
  ; CHECK-NEXT: [[lshr:%.*]] = lshr i64 [[sub]], zext (i8 ptrtoint (i8* @__typeid_allones32_align to i8) to i64)
  ; CHECK-NEXT: [[shl:%.*]] = shl i64 [[sub]], zext (i8 sub (i8 64, i8 ptrtoint (i8* @__typeid_allones32_align to i8)) to i64)
  ; CHECK-NEXT: [[or:%.*]] = or i64 [[lshr]], [[shl]]
  ; CHECK-NEXT: [[ule:%.*]] = icmp ule i64 [[or]], ptrtoint (i8* @__typeid_allones32_size_m1 to i64)
  ; CHECK-NEXT: ret i1 [[ule]]
  %x = call i1 @llvm.type.test(i8* %p, metadata !"allones32")
  ret i1 %x
}

; CHECK: define i1 @bytearray7(i8* [[p:%.*]])
define i1 @bytearray7(i8* %p) {
  ; CHECK-NEXT: [[pi:%.*]] = ptrtoint i8* [[p]] to i64
  ; CHECK-NEXT: [[sub:%.*]] = sub i64 [[pi]], ptrtoint (i8* @__typeid_bytearray7_global_addr to i64)
  ; CHECK-NEXT: [[lshr:%.*]] = lshr i64 [[sub]], zext (i8 ptrtoint (i8* @__typeid_bytearray7_align to i8) to i64)
  ; CHECK-NEXT: [[shl:%.*]] = shl i64 [[sub]], zext (i8 sub (i8 64, i8 ptrtoint (i8* @__typeid_bytearray7_align to i8)) to i64)
  ; CHECK-NEXT: [[or:%.*]] = or i64 [[lshr]], [[shl]]
  ; CHECK-NEXT: [[ule:%.*]] = icmp ule i64 [[or]], ptrtoint (i8* @__typeid_bytearray7_size_m1 to i64)
  ; CHECK-NEXT: br i1 [[ule]], label %[[t:.*]], label %[[f:.*]]

  ; CHECK: [[t]]:
  ; CHECK-NEXT: [[gep:%.*]] = getelementptr i8, i8* @__typeid_bytearray7_byte_array, i64 [[or]]
  ; CHECK-NEXT: [[load:%.*]] = load i8, i8* [[gep]]
  ; CHECK-NEXT: [[and:%.*]] = and i8 [[load]], ptrtoint (i8* @__typeid_bytearray7_bit_mask to i8)
  ; CHECK-NEXT: [[ne:%.*]] = icmp ne i8 [[and]], 0
  ; CHECK-NEXT: br label %[[f]]

  ; CHECK: [[f]]:
  ; CHECK-NEXT: [[phi:%.*]] = phi i1 [ false, %0 ], [ [[ne]], %[[t]] ]
  ; CHECK-NEXT: ret i1 [[phi]]
  %x = call i1 @llvm.type.test(i8* %p, metadata !"bytearray7")
  ret i1 %x
}

; CHECK: define i1 @bytearray32(i8* [[p:%.*]])
define i1 @bytearray32(i8* %p) {
  ; CHECK-NEXT: [[pi:%.*]] = ptrtoint i8* [[p]] to i64
  ; CHECK-NEXT: [[sub:%.*]] = sub i64 [[pi]], ptrtoint (i8* @__typeid_bytearray32_global_addr to i64)
  ; CHECK-NEXT: [[lshr:%.*]] = lshr i64 [[sub]], zext (i8 ptrtoint (i8* @__typeid_bytearray32_align to i8) to i64)
  ; CHECK-NEXT: [[shl:%.*]] = shl i64 [[sub]], zext (i8 sub (i8 64, i8 ptrtoint (i8* @__typeid_bytearray32_align to i8)) to i64)
  ; CHECK-NEXT: [[or:%.*]] = or i64 [[lshr]], [[shl]]
  ; CHECK-NEXT: [[ule:%.*]] = icmp ule i64 [[or]], ptrtoint (i8* @__typeid_bytearray32_size_m1 to i64)
  ; CHECK-NEXT: br i1 [[ule]], label %[[t:.*]], label %[[f:.*]]

  ; CHECK: [[t]]:
  ; CHECK-NEXT: [[gep:%.*]] = getelementptr i8, i8* @__typeid_bytearray32_byte_array, i64 [[or]]
  ; CHECK-NEXT: [[load:%.*]] = load i8, i8* [[gep]]
  ; CHECK-NEXT: [[and:%.*]] = and i8 [[load]], ptrtoint (i8* @__typeid_bytearray32_bit_mask to i8)
  ; CHECK-NEXT: [[ne:%.*]] = icmp ne i8 [[and]], 0
  ; CHECK-NEXT: br label %[[f]]

  ; CHECK: [[f]]:
  ; CHECK-NEXT: [[phi:%.*]] = phi i1 [ false, %0 ], [ [[ne]], %[[t]] ]
  ; CHECK-NEXT: ret i1 [[phi]]
  %x = call i1 @llvm.type.test(i8* %p, metadata !"bytearray32")
  ret i1 %x
}

; CHECK: define i1 @inline5(i8* [[p:%.*]])
define i1 @inline5(i8* %p) {
  ; CHECK-NEXT: [[pi:%.*]] = ptrtoint i8* [[p]] to i64
  ; CHECK-NEXT: [[sub:%.*]] = sub i64 [[pi]], ptrtoint (i8* @__typeid_inline5_global_addr to i64)
  ; CHECK-NEXT: [[lshr:%.*]] = lshr i64 [[sub]], zext (i8 ptrtoint (i8* @__typeid_inline5_align to i8) to i64)
  ; CHECK-NEXT: [[shl:%.*]] = shl i64 [[sub]], zext (i8 sub (i8 64, i8 ptrtoint (i8* @__typeid_inline5_align to i8)) to i64)
  ; CHECK-NEXT: [[or:%.*]] = or i64 [[lshr]], [[shl]]
  ; CHECK-NEXT: [[ule:%.*]] = icmp ule i64 [[or]], ptrtoint (i8* @__typeid_inline5_size_m1 to i64)
  ; CHECK-NEXT: br i1 [[ule]], label %[[t:.*]], label %[[f:.*]]

  ; CHECK: [[t]]:
  ; CHECK-NEXT: [[trunc:%.*]] = trunc i64 [[or]] to i32
  ; CHECK-NEXT: [[and:%.*]] = and i32 [[trunc]], 31
  ; CHECK-NEXT: [[shl2:%.*]] = shl i32 1, [[and]]
  ; CHECK-NEXT: [[and2:%.*]] = and i32 ptrtoint (i8* @__typeid_inline5_inline_bits to i32), [[shl2]]
  ; CHECK-NEXT: [[ne:%.*]] = icmp ne i32 [[and2]], 0
  ; CHECK-NEXT: br label %[[f]]

  ; CHECK: [[f]]:
  ; CHECK-NEXT: [[phi:%.*]] = phi i1 [ false, %0 ], [ [[ne]], %[[t]] ]
  ; CHECK-NEXT: ret i1 [[phi]]
  %x = call i1 @llvm.type.test(i8* %p, metadata !"inline5")
  ret i1 %x
}

; CHECK: define i1 @inline6(i8* [[p:%.*]])
define i1 @inline6(i8* %p) {
  ; CHECK-NEXT: [[pi:%.*]] = ptrtoint i8* [[p]] to i64
  ; CHECK-NEXT: [[sub:%.*]] = sub i64 [[pi]], ptrtoint (i8* @__typeid_inline6_global_addr to i64)
  ; CHECK-NEXT: [[lshr:%.*]] = lshr i64 [[sub]], zext (i8 ptrtoint (i8* @__typeid_inline6_align to i8) to i64)
  ; CHECK-NEXT: [[shl:%.*]] = shl i64 [[sub]], zext (i8 sub (i8 64, i8 ptrtoint (i8* @__typeid_inline6_align to i8)) to i64)
  ; CHECK-NEXT: [[or:%.*]] = or i64 [[lshr]], [[shl]]
  ; CHECK-NEXT: [[ule:%.*]] = icmp ule i64 [[or]], ptrtoint (i8* @__typeid_inline6_size_m1 to i64)
  ; CHECK-NEXT: br i1 [[ule]], label %[[t:.*]], label %[[f:.*]]

  ; CHECK: [[t]]:
  ; CHECK-NEXT: [[and:%.*]] = and i64 [[or]], 63
  ; CHECK-NEXT: [[shl2:%.*]] = shl i64 1, [[and]]
  ; CHECK-NEXT: [[and2:%.*]] = and i64 ptrtoint (i8* @__typeid_inline6_inline_bits to i64), [[shl2]]
  ; CHECK-NEXT: [[ne:%.*]] = icmp ne i64 [[and2]], 0
  ; CHECK-NEXT: br label %[[f]]

  ; CHECK: [[f]]:
  ; CHECK-NEXT: [[phi:%.*]] = phi i1 [ false, %0 ], [ [[ne]], %[[t]] ]
  ; CHECK-NEXT: ret i1 [[phi]]
  %x = call i1 @llvm.type.test(i8* %p, metadata !"inline6")
  ret i1 %x
}

; CHECK: define i1 @single(i8* [[p:%.*]])
define i1 @single(i8* %p) {
  ; CHECK-NEXT: [[pi:%.*]] = ptrtoint i8* [[p]] to i64
  ; CHECK-NEXT: [[eq:%.*]] = icmp eq i64 [[pi]], ptrtoint (i8* @__typeid_single_global_addr to i64)
  ; CHECK-NEXT: ret i1 [[eq]]
  %x = call i1 @llvm.type.test(i8* %p, metadata !"single")
  ret i1 %x
}

; CHECK: !0 = !{i64 0, i64 256}
; CHECK: !1 = !{i64 0, i64 64}
; CHECK: !2 = !{i64 -1, i64 -1}
; CHECK: !3 = !{i64 0, i64 32}
; CHECK: !4 = !{i64 0, i64 4294967296}
; CHECK: !5 = !{i64 0, i64 128}
