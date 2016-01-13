// RUN: %clang_cc1 %s -emit-llvm -o - | FileCheck %s

struct X { int *XX; int Y;};

void foo() {
  // CHECK: @foo.nate = internal global i32 0
  static int nate = 0;
  struct X bob = { &nate, 14 };
  bar(&bob);
}
