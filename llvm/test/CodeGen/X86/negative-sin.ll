; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=avx | FileCheck %s

declare double @sin(double %f)

; When the subs are strict, they can't be removed because of signed zero.

define double @strict(double %e) nounwind {
; CHECK-LABEL: strict:
; CHECK:       # BB#0:
; CHECK-NEXT:    pushq %rax
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    callq sin
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    popq %rax
; CHECK-NEXT:    retq
;
  %f = fsub double 0.0, %e
  %g = call double @sin(double %f) readonly
  %h = fsub double 0.0, %g
  ret double %h
}

; FIXME:
; 'fast' implies no-signed-zeros, so the negates fold away.
; The 'sin' does not need any fast-math-flags for this transform.

define double @fast(double %e) nounwind {
; CHECK-LABEL: fast:
; CHECK:       # BB#0:
; CHECK-NEXT:    pushq %rax
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    callq sin
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    popq %rax
; CHECK-NEXT:    retq
;
  %f = fsub fast double 0.0, %e
  %g = call double @sin(double %f) readonly
  %h = fsub fast double 0.0, %g
  ret double %h
}

; FIXME:
; No-signed-zeros is all that we need for this transform.

define double @nsz(double %e) nounwind {
; CHECK-LABEL: nsz:
; CHECK:       # BB#0:
; CHECK-NEXT:    pushq %rax
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    callq sin
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    popq %rax
; CHECK-NEXT:    retq
;
  %f = fsub nsz double 0.0, %e
  %g = call double @sin(double %f) readonly
  %h = fsub nsz double 0.0, %g
  ret double %h
}

; FIXME:
; The 1st negate is strict, so we can't kill that sub, but the 2nd disappears.

define double @semi_strict1(double %e) nounwind {
; CHECK-LABEL: semi_strict1:
; CHECK:       # BB#0:
; CHECK-NEXT:    pushq %rax
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    callq sin
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    popq %rax
; CHECK-NEXT:    retq
;
  %f = fsub double 0.0, %e
  %g = call double @sin(double %f) readonly
  %h = fsub nsz double 0.0, %g
  ret double %h
}

; FIXME:
; The 2nd negate is strict, so we can't kill it. It becomes an add of zero instead.

define double @semi_strict2(double %e) nounwind {
; CHECK-LABEL: semi_strict2:
; CHECK:       # BB#0:
; CHECK-NEXT:    pushq %rax
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    callq sin
; CHECK-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; CHECK-NEXT:    vsubsd %xmm0, %xmm1, %xmm0
; CHECK-NEXT:    popq %rax
; CHECK-NEXT:    retq
;
  %f = fsub nsz double 0.0, %e
  %g = call double @sin(double %f) readonly
  %h = fsub double 0.0, %g
  ret double %h
}

; FIXME:
; Auto-upgrade function attribute to IR-level fast-math-flags.

define double @fn_attr(double %e) nounwind #0 {
; CHECK-LABEL: fn_attr:
; CHECK:       # BB#0:
; CHECK-NEXT:    jmp sin
;
  %f = fsub double 0.0, %e
  %g = call double @sin(double %f) readonly
  %h = fsub double 0.0, %g
  ret double %h
}

attributes #0 = { "unsafe-fp-math"="true" }

