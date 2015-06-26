// RUN: %clang_cc1 -g -emit-llvm -o - %s | FileCheck %s

void t1() __attribute__((nodebug));

void t1()
{
  int a = 10;
  a++;
}

void t2()
{
  int b = 10;
  b++;
}

// With nodebug, IR should have no llvm.dbg.* calls, or !dbg annotations.
// CHECK-LABEL: @t1
// CHECK-NOT:   dbg
// CHECK:       }

// For sanity, check those things do occur normally.
// CHECK-LABEL: @t2
// CHECK:       call{{.*}}llvm.dbg
// CHECK:       !dbg
// CHECK:       }

// We should see a function description for t2 but not t1.
// CHECK-NOT: DISubprogram(name: "t1"
// CHECK:     DISubprogram(name: "t2"
// CHECK-NOT: DISubprogram(name: "t1"

