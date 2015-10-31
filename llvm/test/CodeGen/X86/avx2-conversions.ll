; RUN: llc < %s -mtriple=x86_64-apple-darwin -mcpu=core-avx2 -mattr=+avx2 | FileCheck %s

define <4 x i32> @trunc4(<4 x i64> %A) nounwind {
; CHECK-LABEL: trunc4:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vmovdqa {{.*#+}} ymm1 = <0,2,4,6,u,u,u,u>
; CHECK-NEXT:    vpermd %ymm0, %ymm1, %ymm0
; CHECK-NEXT:    vzeroupper
; CHECK-NEXT:    retq
  %B = trunc <4 x i64> %A to <4 x i32>
  ret <4 x i32>%B
}

define <8 x i16> @trunc8(<8 x i32> %A) nounwind {
; CHECK-LABEL: trunc8:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpshufb {{.*#+}} ymm0 = ymm0[0,1,4,5,8,9,12,13],zero,zero,zero,zero,zero,zero,zero,zero,ymm0[16,17,20,21,24,25,28,29],zero,zero,zero,zero,zero,zero,zero,zero
; CHECK-NEXT:    vpermq {{.*#+}} ymm0 = ymm0[0,2,2,3]
; CHECK-NEXT:    vzeroupper
; CHECK-NEXT:    retq
  %B = trunc <8 x i32> %A to <8 x i16>
  ret <8 x i16>%B
}

define <4 x i64> @sext4(<4 x i32> %A) nounwind {
; CHECK-LABEL: sext4:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxdq %xmm0, %ymm0
; CHECK-NEXT:    retq
  %B = sext <4 x i32> %A to <4 x i64>
  ret <4 x i64>%B
}

define <8 x i32> @sext8(<8 x i16> %A) nounwind {
; CHECK-LABEL: sext8:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxwd %xmm0, %ymm0
; CHECK-NEXT:    retq
  %B = sext <8 x i16> %A to <8 x i32>
  ret <8 x i32>%B
}

define <4 x i64> @zext4(<4 x i32> %A) nounwind {
; CHECK-LABEL: zext4:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovzxdq {{.*#+}} ymm0 = xmm0[0],zero,xmm0[1],zero,xmm0[2],zero,xmm0[3],zero
; CHECK-NEXT:    retq
  %B = zext <4 x i32> %A to <4 x i64>
  ret <4 x i64>%B
}

define <8 x i32> @zext8(<8 x i16> %A) nounwind {
; CHECK-LABEL: zext8:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovzxwd {{.*#+}} ymm0 = xmm0[0],zero,xmm0[1],zero,xmm0[2],zero,xmm0[3],zero,xmm0[4],zero,xmm0[5],zero,xmm0[6],zero,xmm0[7],zero
; CHECK-NEXT:    retq
  %B = zext <8 x i16> %A to <8 x i32>
  ret <8 x i32>%B
}

define <8 x i32> @zext_8i8_8i32(<8 x i8> %A) nounwind {
; CHECK-LABEL: zext_8i8_8i32:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpand {{.*}}(%rip), %xmm0, %xmm0
; CHECK-NEXT:    vpmovzxwd {{.*#+}} ymm0 = xmm0[0],zero,xmm0[1],zero,xmm0[2],zero,xmm0[3],zero,xmm0[4],zero,xmm0[5],zero,xmm0[6],zero,xmm0[7],zero
; CHECK-NEXT:    retq
  %B = zext <8 x i8> %A to <8 x i32>
  ret <8 x i32>%B
}

define <16 x i16> @zext_16i8_16i16(<16 x i8> %z) {
; CHECK-LABEL: zext_16i8_16i16:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovzxbw {{.*#+}} ymm0 = xmm0[0],zero,xmm0[1],zero,xmm0[2],zero,xmm0[3],zero,xmm0[4],zero,xmm0[5],zero,xmm0[6],zero,xmm0[7],zero,xmm0[8],zero,xmm0[9],zero,xmm0[10],zero,xmm0[11],zero,xmm0[12],zero,xmm0[13],zero,xmm0[14],zero,xmm0[15],zero
; CHECK-NEXT:    retq
  %t = zext <16 x i8> %z to <16 x i16>
  ret <16 x i16> %t
}

define <16 x i16> @sext_16i8_16i16(<16 x i8> %z) {
; CHECK-LABEL: sext_16i8_16i16:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxbw %xmm0, %ymm0
; CHECK-NEXT:    retq
  %t = sext <16 x i8> %z to <16 x i16>
  ret <16 x i16> %t
}

define <16 x i8> @trunc_16i16_16i8(<16 x i16> %z) {
; CHECK-LABEL: trunc_16i16_16i8:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vextracti128 $1, %ymm0, %xmm1
; CHECK-NEXT:    vmovdqa {{.*#+}} xmm2 = <0,2,4,6,8,10,12,14,u,u,u,u,u,u,u,u>
; CHECK-NEXT:    vpshufb %xmm2, %xmm1, %xmm1
; CHECK-NEXT:    vpshufb %xmm2, %xmm0, %xmm0
; CHECK-NEXT:    vpunpcklqdq {{.*#+}} xmm0 = xmm0[0],xmm1[0]
; CHECK-NEXT:    vzeroupper
; CHECK-NEXT:    retq
  %t = trunc <16 x i16> %z to <16 x i8>
  ret <16 x i8> %t
}

define <4 x i64> @load_sext_test1(<4 x i32> *%ptr) {
; CHECK-LABEL: load_sext_test1:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxdq (%rdi), %ymm0
; CHECK-NEXT:    retq
 %X = load <4 x i32>, <4 x i32>* %ptr
 %Y = sext <4 x i32> %X to <4 x i64>
 ret <4 x i64>%Y
}

define <4 x i64> @load_sext_test2(<4 x i8> *%ptr) {
; CHECK-LABEL: load_sext_test2:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxbq (%rdi), %ymm0
; CHECK-NEXT:    retq
 %X = load <4 x i8>, <4 x i8>* %ptr
 %Y = sext <4 x i8> %X to <4 x i64>
 ret <4 x i64>%Y
}

define <4 x i64> @load_sext_test3(<4 x i16> *%ptr) {
; CHECK-LABEL: load_sext_test3:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxwq (%rdi), %ymm0
; CHECK-NEXT:    retq
 %X = load <4 x i16>, <4 x i16>* %ptr
 %Y = sext <4 x i16> %X to <4 x i64>
 ret <4 x i64>%Y
}

define <8 x i32> @load_sext_test4(<8 x i16> *%ptr) {
; CHECK-LABEL: load_sext_test4:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxwd (%rdi), %ymm0
; CHECK-NEXT:    retq
 %X = load <8 x i16>, <8 x i16>* %ptr
 %Y = sext <8 x i16> %X to <8 x i32>
 ret <8 x i32>%Y
}

define <8 x i32> @load_sext_test5(<8 x i8> *%ptr) {
; CHECK-LABEL: load_sext_test5:
; CHECK:       ## BB#0:
; CHECK-NEXT:    vpmovsxbd (%rdi), %ymm0
; CHECK-NEXT:    retq
 %X = load <8 x i8>, <8 x i8>* %ptr
 %Y = sext <8 x i8> %X to <8 x i32>
 ret <8 x i32>%Y
}
