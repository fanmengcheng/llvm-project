// RUN: cd %S && %clang -### -g %s -c 2>&1 | FileCheck -check-prefix=CHECK-PWD %s
// CHECK-PWD: {{"-fdebug-compilation-dir" ".*Driver.*"}}
