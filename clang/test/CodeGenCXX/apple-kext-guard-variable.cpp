// RUN: %clang -mtriple=x86_64-apple-darwin10 -S -mkernel -Xclang -verify %s

// rdar://problem/9143356

int foo();
void test() {
  static int y = 0;
  static int x = foo(); // expected-error {{this initialization requires a guard variable, which the kernel does not support}}
}
