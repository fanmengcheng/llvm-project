// RUN: %clang -frewrite-map-file %S/Inputs/rewrite.map -### %s 2>&1 | FileCheck %s -check-prefix CHECK-SINGLE

// CHECK-SINGLE: "-frewrite-map-file" "{{.*[\\/]}}rewrite.map"

// RUN: %clang -frewrite-map-file %S/Inputs/rewrite-1.map -frewrite-map-file %S/Inputs/rewrite-2.map -### %s 2>&1 | FileCheck %s -check-prefix CHECK-MULTIPLE

// CHECK-MULTIPLE: "-frewrite-map-file" "{{.*[\\/]}}rewrite-1.map" "-frewrite-map-file" "{{.*[\\/]}}rewrite-2.map"

// RUN: %clang -frewrite-map-file=%S/Inputs/rewrite.map -### %s 2>&1 | FileCheck %s -check-prefix CHECK-SINGLE-EQ

// CHECK-SINGLE-EQ: "-frewrite-map-file" "{{.*[\\/]}}rewrite.map"

// RUN: %clang -frewrite-map-file=%S/Inputs/rewrite-1.map -frewrite-map-file=%S/Inputs/rewrite-2.map -### %s 2>&1 | FileCheck %s -check-prefix CHECK-MULTIPLE-EQ

// CHECK-MULTIPLE-EQ: "-frewrite-map-file" "{{.*[\\/]}}rewrite-1.map"
// CHECK-MULTIPLE-EQ: "-frewrite-map-file" "{{.*[\\/]}}rewrite-2.map"

// RUN: %clang -frewrite-map-file %S/Inputs/rewrite-1.map -frewrite-map-file=%S/Inputs/rewrite-2.map -### %s 2>&1 | FileCheck %s -check-prefix CHECK-MIXED

// CHECK-MIXED: "-frewrite-map-file" "{{.*[\\/]}}rewrite-1.map" "-frewrite-map-file" "{{.*[\\/]}}rewrite-2.map"

