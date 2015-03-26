; RUN: llc -O3 -filetype=obj < %s | llvm-dwarfdump -debug-dump=info - | FileCheck %s

target datalayout = "e-p:32:32:32-i1:8:32-i8:8:32-i16:16:32-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:32:64-v128:32:128-a0:0:32-n32"
target triple = "thumbv7-apple-darwin10"

@x1 = internal global i8 1, align 1
@x2 = internal global i8 1, align 1
@x3 = internal global i8 1, align 1
@x4 = internal global i8 1, align 1
@x5 = global i8 1, align 1

; Check debug info output for merged global.
; DW_AT_location
; 0x03 DW_OP_addr
; 0x.. .long __MergedGlobals
; 0x10 DW_OP_constu
; 0x.. offset
; 0x22 DW_OP_plus

; CHECK: DW_TAG_variable
; CHECK-NOT: DW_TAG
; CHECK:    DW_AT_name {{.*}} "x1"
; CHECK-NOT: {{DW_TAG|NULL}}
; CHECK:    DW_AT_location [DW_FORM_exprloc]        (<0x8> 03 [[ADDR:.. .. .. ..]] 10 00 22  )
; CHECK: DW_TAG_variable
; CHECK-NOT: DW_TAG
; CHECK:    DW_AT_name {{.*}} "x2"
; CHECK-NOT: {{DW_TAG|NULL}}
; CHECK:    DW_AT_location [DW_FORM_exprloc]        (<0x8> 03 [[ADDR]] 10 01 22  )

define zeroext i8 @get1(i8 zeroext %a) nounwind optsize {
entry:
  tail call void @llvm.dbg.value(metadata i8 %a, i64 0, metadata !10, metadata !MDExpression()), !dbg !30
  %0 = load i8, i8* @x1, align 4, !dbg !30
  tail call void @llvm.dbg.value(metadata i8 %0, i64 0, metadata !11, metadata !MDExpression()), !dbg !30
  store i8 %a, i8* @x1, align 4, !dbg !30
  ret i8 %0, !dbg !31
}

declare void @llvm.dbg.value(metadata, i64, metadata, metadata) nounwind readnone

define zeroext i8 @get2(i8 zeroext %a) nounwind optsize {
entry:
  tail call void @llvm.dbg.value(metadata i8 %a, i64 0, metadata !18, metadata !MDExpression()), !dbg !32
  %0 = load i8, i8* @x2, align 4, !dbg !32
  tail call void @llvm.dbg.value(metadata i8 %0, i64 0, metadata !19, metadata !MDExpression()), !dbg !32
  store i8 %a, i8* @x2, align 4, !dbg !32
  ret i8 %0, !dbg !33
}

define zeroext i8 @get3(i8 zeroext %a) nounwind optsize {
entry:
  tail call void @llvm.dbg.value(metadata i8 %a, i64 0, metadata !21, metadata !MDExpression()), !dbg !34
  %0 = load i8, i8* @x3, align 4, !dbg !34
  tail call void @llvm.dbg.value(metadata i8 %0, i64 0, metadata !22, metadata !MDExpression()), !dbg !34
  store i8 %a, i8* @x3, align 4, !dbg !34
  ret i8 %0, !dbg !35
}

define zeroext i8 @get4(i8 zeroext %a) nounwind optsize {
entry:
  tail call void @llvm.dbg.value(metadata i8 %a, i64 0, metadata !24, metadata !MDExpression()), !dbg !36
  %0 = load i8, i8* @x4, align 4, !dbg !36
  tail call void @llvm.dbg.value(metadata i8 %0, i64 0, metadata !25, metadata !MDExpression()), !dbg !36
  store i8 %a, i8* @x4, align 4, !dbg !36
  ret i8 %0, !dbg !37
}

define zeroext i8 @get5(i8 zeroext %a) nounwind optsize {
entry:
  tail call void @llvm.dbg.value(metadata i8 %a, i64 0, metadata !27, metadata !MDExpression()), !dbg !38
  %0 = load i8, i8* @x5, align 4, !dbg !38
  tail call void @llvm.dbg.value(metadata i8 %0, i64 0, metadata !28, metadata !MDExpression()), !dbg !38
  store i8 %a, i8* @x5, align 4, !dbg !38
  ret i8 %0, !dbg !39
}

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!49}

!0 = !MDSubprogram(name: "get1", linkageName: "get1", line: 4, isLocal: false, isDefinition: true, virtualIndex: 6, flags: DIFlagPrototyped, isOptimized: true, scopeLine: 4, file: !47, scope: !1, type: !3, function: i8 (i8)* @get1, variables: !42)
!1 = !MDFile(filename: "foo.c", directory: "/tmp/")
!2 = !MDCompileUnit(language: DW_LANG_C89, producer: "4.2.1 (Based on Apple Inc. build 5658) (LLVM build 2369.8)", isOptimized: true, emissionKind: 0, file: !47, enums: !48, retainedTypes: !48, subprograms: !40, globals: !41, imports:  !48)
!3 = !MDSubroutineType(types: !4)
!4 = !{!5, !5}
!5 = !MDBasicType(tag: DW_TAG_base_type, name: "_Bool", size: 8, align: 8, encoding: DW_ATE_boolean)
!6 = !MDSubprogram(name: "get2", linkageName: "get2", line: 7, isLocal: false, isDefinition: true, virtualIndex: 6, flags: DIFlagPrototyped, isOptimized: true, scopeLine: 7, file: !47, scope: !1, type: !3, function: i8 (i8)* @get2, variables: !43)
!7 = !MDSubprogram(name: "get3", linkageName: "get3", line: 10, isLocal: false, isDefinition: true, virtualIndex: 6, flags: DIFlagPrototyped, isOptimized: true, scopeLine: 10, file: !47, scope: !1, type: !3, function: i8 (i8)* @get3, variables: !44)
!8 = !MDSubprogram(name: "get4", linkageName: "get4", line: 13, isLocal: false, isDefinition: true, virtualIndex: 6, flags: DIFlagPrototyped, isOptimized: true, scopeLine: 13, file: !47, scope: !1, type: !3, function: i8 (i8)* @get4, variables: !45)
!9 = !MDSubprogram(name: "get5", linkageName: "get5", line: 16, isLocal: false, isDefinition: true, virtualIndex: 6, flags: DIFlagPrototyped, isOptimized: true, scopeLine: 16, file: !47, scope: !1, type: !3, function: i8 (i8)* @get5, variables: !46)
!10 = !MDLocalVariable(tag: DW_TAG_arg_variable, name: "a", line: 4, arg: 0, scope: !0, file: !1, type: !5)
!11 = !MDLocalVariable(tag: DW_TAG_auto_variable, name: "b", line: 4, scope: !12, file: !1, type: !5)
!12 = distinct !MDLexicalBlock(line: 4, column: 0, file: !47, scope: !0)
!13 = !MDGlobalVariable(name: "x1", line: 3, isLocal: true, isDefinition: true, scope: !1, file: !1, type: !5, variable: i8* @x1)
!14 = !MDGlobalVariable(name: "x2", line: 6, isLocal: true, isDefinition: true, scope: !1, file: !1, type: !5, variable: i8* @x2)
!15 = !MDGlobalVariable(name: "x3", line: 9, isLocal: true, isDefinition: true, scope: !1, file: !1, type: !5, variable: i8* @x3)
!16 = !MDGlobalVariable(name: "x4", line: 12, isLocal: true, isDefinition: true, scope: !1, file: !1, type: !5, variable: i8* @x4)
!17 = !MDGlobalVariable(name: "x5", line: 15, isLocal: false, isDefinition: true, scope: !1, file: !1, type: !5, variable: i8* @x5)
!18 = !MDLocalVariable(tag: DW_TAG_arg_variable, name: "a", line: 7, arg: 0, scope: !6, file: !1, type: !5)
!19 = !MDLocalVariable(tag: DW_TAG_auto_variable, name: "b", line: 7, scope: !20, file: !1, type: !5)
!20 = distinct !MDLexicalBlock(line: 7, column: 0, file: !47, scope: !6)
!21 = !MDLocalVariable(tag: DW_TAG_arg_variable, name: "a", line: 10, arg: 0, scope: !7, file: !1, type: !5)
!22 = !MDLocalVariable(tag: DW_TAG_auto_variable, name: "b", line: 10, scope: !23, file: !1, type: !5)
!23 = distinct !MDLexicalBlock(line: 10, column: 0, file: !47, scope: !7)
!24 = !MDLocalVariable(tag: DW_TAG_arg_variable, name: "a", line: 13, arg: 0, scope: !8, file: !1, type: !5)
!25 = !MDLocalVariable(tag: DW_TAG_auto_variable, name: "b", line: 13, scope: !26, file: !1, type: !5)
!26 = distinct !MDLexicalBlock(line: 13, column: 0, file: !47, scope: !8)
!27 = !MDLocalVariable(tag: DW_TAG_arg_variable, name: "a", line: 16, arg: 0, scope: !9, file: !1, type: !5)
!28 = !MDLocalVariable(tag: DW_TAG_auto_variable, name: "b", line: 16, scope: !29, file: !1, type: !5)
!29 = distinct !MDLexicalBlock(line: 16, column: 0, file: !47, scope: !9)
!30 = !MDLocation(line: 4, scope: !0)
!31 = !MDLocation(line: 4, scope: !12)
!32 = !MDLocation(line: 7, scope: !6)
!33 = !MDLocation(line: 7, scope: !20)
!34 = !MDLocation(line: 10, scope: !7)
!35 = !MDLocation(line: 10, scope: !23)
!36 = !MDLocation(line: 13, scope: !8)
!37 = !MDLocation(line: 13, scope: !26)
!38 = !MDLocation(line: 16, scope: !9)
!39 = !MDLocation(line: 16, scope: !29)
!40 = !{!0, !6, !7, !8, !9}
!41 = !{!13, !14, !15, !16, !17}
!42 = !{!10, !11}
!43 = !{!18, !19}
!44 = !{!21, !22}
!45 = !{!24, !25}
!46 = !{!27, !28}
!47 = !MDFile(filename: "foo.c", directory: "/tmp/")
!48 = !{}
!49 = !{i32 1, !"Debug Info Version", i32 3}
