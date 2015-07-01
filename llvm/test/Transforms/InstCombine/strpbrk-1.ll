; Test that the strpbrk library call simplifier works correctly.
;
; RUN: opt < %s -instcombine -S | FileCheck %s

target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:128:128"

@hello = constant [12 x i8] c"hello world\00"
@w = constant [2 x i8] c"w\00"
@null = constant [1 x i8] zeroinitializer

declare i8* @strpbrk(i8*, i8*)

; Check strpbrk(s, "") -> NULL.

define i8* @test_simplify1(i8* %str) {
; CHECK-LABEL: @test_simplify1(
  %pat = getelementptr [1 x i8], [1 x i8]* @null, i32 0, i32 0

  %ret = call i8* @strpbrk(i8* %str, i8* %pat)
  ret i8* %ret
; CHECK-NEXT: ret i8* null
}

; Check strpbrk("", s) -> NULL.

define i8* @test_simplify2(i8* %pat) {
; CHECK-LABEL: @test_simplify2(
  %str = getelementptr [1 x i8], [1 x i8]* @null, i32 0, i32 0

  %ret = call i8* @strpbrk(i8* %str, i8* %pat)
  ret i8* %ret
; CHECK-NEXT: ret i8* null
}

; Check strpbrk(s1, s2), where s1 and s2 are constants.

define i8* @test_simplify3() {
; CHECK-LABEL: @test_simplify3(
  %str = getelementptr [12 x i8], [12 x i8]* @hello, i32 0, i32 0
  %pat = getelementptr [2 x i8], [2 x i8]* @w, i32 0, i32 0

  %ret = call i8* @strpbrk(i8* %str, i8* %pat)
  ret i8* %ret
; CHECK-NEXT: ret i8* getelementptr inbounds ([12 x i8], [12 x i8]* @hello, i32 0, i32 6)
}

; Check strpbrk(s, "a") -> strchr(s, 'a').

define i8* @test_simplify4(i8* %str) {
; CHECK-LABEL: @test_simplify4(
  %pat = getelementptr [2 x i8], [2 x i8]* @w, i32 0, i32 0

  %ret = call i8* @strpbrk(i8* %str, i8* %pat)
; CHECK-NEXT: [[VAR:%[a-z]+]] = call i8* @strchr(i8* %str, i32 119)
  ret i8* %ret
; CHECK-NEXT: ret i8* [[VAR]]
}

; Check cases that shouldn't be simplified.

define i8* @test_no_simplify1(i8* %str, i8* %pat) {
; CHECK-LABEL: @test_no_simplify1(

  %ret = call i8* @strpbrk(i8* %str, i8* %pat)
; CHECK-NEXT: %ret = call i8* @strpbrk(i8* %str, i8* %pat)
  ret i8* %ret
; CHECK-NEXT: ret i8* %ret
}
