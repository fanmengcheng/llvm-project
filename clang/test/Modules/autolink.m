// RUN: rm -rf %t
// RUN: %clang_cc1 -emit-pch -fmodules-cache-path=%t -fmodules -o %t.pch -I %S/Inputs -x objective-c-header %S/Inputs/autolink-sub3.pch
// RUN: %clang_cc1 -emit-llvm -o - -fmodules-cache-path=%t -fmodules -F %S/Inputs -I %S/Inputs -include-pch %t.pch %s | FileCheck %s
// RUN: %clang_cc1 -emit-llvm -fno-autolink -o - -fmodules-cache-path=%t -fmodules -F %S/Inputs -I %S/Inputs -include-pch %t.pch %s | FileCheck --check-prefix=CHECK-AUTOLINK-DISABLED %s

@import autolink.sub2;

int f() {
  return autolink_sub2();
}

@import autolink;

int g() {
  return autolink;
}

@import Module.SubFramework;
const char *get_module_subframework() {
  return module_subframework;
}

@import DependsOnModule.SubFramework;
float *get_module_subframework_dep() {
  return sub_framework;
}

@import NoUmbrella;
int use_no_umbrella() {
  return no_umbrella_A;
}

int use_autolink_sub3() {
  return autolink_sub3();
}

// NOTE: "autolink_sub" is intentionally not linked.

// CHECK: !llvm.module.flags = !{{{.*}}}
// CHECK: !{{[0-9]+}} = !{i32 6, !"Linker Options", ![[AUTOLINK_OPTIONS:[0-9]+]]}
// CHECK: ![[AUTOLINK_OPTIONS]] = !{![[AUTOLINK_PCH:[0-9]+]], ![[AUTOLINK_FRAMEWORK:[0-9]+]], ![[AUTOLINK:[0-9]+]], ![[DEPENDSONMODULE:[0-9]+]], ![[MODULE:[0-9]+]], ![[NOUMBRELLA:[0-9]+]]}
// CHECK: ![[AUTOLINK_PCH]] = !{!"{{(\\01|-l|/DEFAULTLIB:)}}autolink_from_pch{{(\.lib)?}}"}
// CHECK: ![[AUTOLINK_FRAMEWORK]] = !{!"-framework", !"autolink_framework"}
// CHECK: ![[AUTOLINK]] = !{!"{{(\\01|-l|/DEFAULTLIB:)}}autolink{{(\.lib)?}}"}
// CHECK: ![[DEPENDSONMODULE]] = !{!"-framework", !"DependsOnModule"}
// CHECK: ![[MODULE]] = !{!"-framework", !"Module"}
// CHECK: ![[NOUMBRELLA]] = !{!"-framework", !"NoUmbrella"}

// CHECK-AUTOLINK-DISABLED: !llvm.module.flags
// CHECK-AUTOLINK-DISABLED-NOT: "Linker Options"
