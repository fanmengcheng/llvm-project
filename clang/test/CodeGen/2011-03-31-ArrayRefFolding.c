// RUN: %clang_cc1 -emit-llvm -o - -triple i386-apple-darwin -Os %s | FileCheck %s
// PR9571

struct t {
  int x;
};

extern struct t *cfun;

int f(void) {
  if (!(cfun + 0))
    // CHECK: icmp eq %struct.t* %tmp, null
    return 0;
  return cfun->x;
}
