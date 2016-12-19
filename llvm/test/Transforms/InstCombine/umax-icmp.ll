; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -S -instcombine < %s | FileCheck %s

; If we have a umax feeding an unsigned or equality icmp that shares an
; operand with the umax, the compare should always be folded.
; Test all 4 foldable predicates (eq,ne,ugt,ule) * 4 commutation
; possibilities for each predicate. Note that folds to true/false
; (predicate = uge/ult) or folds to an existing instruction should be
; handled by InstSimplify.

; umax(X, Y) == X --> X >= Y

define i1 @eq_umax1(i32 %x, i32 %y) {
; CHECK-LABEL: @eq_umax1(
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 %x, %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp eq i32 %sel, %x
  ret i1 %cmp2
}

; Commute max operands.

define i1 @eq_umax2(i32 %x, i32 %y) {
; CHECK-LABEL: @eq_umax2(
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 %x, %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp eq i32 %sel, %x
  ret i1 %cmp2
}

; Disguise the icmp predicate by commuting the max op to the RHS.

define i1 @eq_umax3(i32 %a, i32 %y) {
; CHECK-LABEL: @eq_umax3(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp eq i32 %x, %sel
  ret i1 %cmp2
}

; Commute max operands.

define i1 @eq_umax4(i32 %a, i32 %y) {
; CHECK-LABEL: @eq_umax4(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp eq i32 %x, %sel
  ret i1 %cmp2
}

; umax(X, Y) <= X --> X >= Y

define i1 @ule_umax1(i32 %x, i32 %y) {
; CHECK-LABEL: @ule_umax1(
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 %x, %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ule i32 %sel, %x
  ret i1 %cmp2
}

; Commute max operands.

define i1 @ule_umax2(i32 %x, i32 %y) {
; CHECK-LABEL: @ule_umax2(
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 %x, %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ule i32 %sel, %x
  ret i1 %cmp2
}

; Disguise the icmp predicate by commuting the max op to the RHS.

define i1 @ule_umax3(i32 %a, i32 %y) {
; CHECK-LABEL: @ule_umax3(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp uge i32 %x, %sel
  ret i1 %cmp2
}

; Commute max operands.

define i1 @ule_umax4(i32 %a, i32 %y) {
; CHECK-LABEL: @ule_umax4(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP2:%.*]] = icmp uge i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp uge i32 %x, %sel
  ret i1 %cmp2
}

; umax(X, Y) != X --> X < Y

define i1 @ne_umax1(i32 %x, i32 %y) {
; CHECK-LABEL: @ne_umax1(
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 %x, %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ne i32 %sel, %x
  ret i1 %cmp2
}

; Commute max operands.

define i1 @ne_umax2(i32 %x, i32 %y) {
; CHECK-LABEL: @ne_umax2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt i32 %y, %x
; CHECK-NEXT:    ret i1 [[CMP1]]
;
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ne i32 %sel, %x
  ret i1 %cmp2
}

; Disguise the icmp predicate by commuting the max op to the RHS.

define i1 @ne_umax3(i32 %a, i32 %y) {
; CHECK-LABEL: @ne_umax3(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ne i32 %x, %sel
  ret i1 %cmp2
}

; Commute max operands.

define i1 @ne_umax4(i32 %a, i32 %y) {
; CHECK-LABEL: @ne_umax4(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP1]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ne i32 %x, %sel
  ret i1 %cmp2
}

; umax(X, Y) > X --> X < Y

define i1 @ugt_umax1(i32 %x, i32 %y) {
; CHECK-LABEL: @ugt_umax1(
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 %x, %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ugt i32 %sel, %x
  ret i1 %cmp2
}

; Commute max operands.

define i1 @ugt_umax2(i32 %x, i32 %y) {
; CHECK-LABEL: @ugt_umax2(
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ugt i32 %y, %x
; CHECK-NEXT:    ret i1 [[CMP1]]
;
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ugt i32 %sel, %x
  ret i1 %cmp2
}

; Disguise the icmp predicate by commuting the max op to the RHS.

define i1 @ugt_umax3(i32 %a, i32 %y) {
; CHECK-LABEL: @ugt_umax3(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP2:%.*]] = icmp ult i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP2]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %x, %y
  %sel = select i1 %cmp1, i32 %x, i32 %y
  %cmp2 = icmp ult i32 %x, %sel
  ret i1 %cmp2
}

; Commute max operands.

define i1 @ugt_umax4(i32 %a, i32 %y) {
; CHECK-LABEL: @ugt_umax4(
; CHECK-NEXT:    [[X:%.*]] = add i32 %a, 3
; CHECK-NEXT:    [[CMP1:%.*]] = icmp ult i32 [[X]], %y
; CHECK-NEXT:    ret i1 [[CMP1]]
;
  %x = add i32 %a, 3 ; thwart complexity-based canonicalization
  %cmp1 = icmp ugt i32 %y, %x
  %sel = select i1 %cmp1, i32 %y, i32 %x
  %cmp2 = icmp ult i32 %x, %sel
  ret i1 %cmp2
}

