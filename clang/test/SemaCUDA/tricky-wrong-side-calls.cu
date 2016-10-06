// rUN: %clang_cc1 -std=c++11 -fsyntax-only -verify -DHOST -verify-ignore-unexpected=note %s
// RUN: %clang_cc1 -std=c++11 -fsyntax-only -fcuda-is-device -verify -verify-ignore-unexpected=note %s

#include "Inputs/cuda.h"

__device__ int device_fn() { return 0; }

struct MemberInitializer {
  int a = device_fn();
};

struct BaseWithDeviceConstructor {
  __device__ BaseWithDeviceConstructor() { device_fn(); }
  // Also try this with a host+device constructor.
};
struct DerivedFromBaseWithDeviceConstructor : BaseWithDeviceConstructor {};

struct BaseWithDeviceDestructor {
  __device__ ~BaseWithDeviceDestructor() { device_fn(); }
};
struct DerivedFromBaseWithDeviceDestructor : BaseWithDeviceDestructor {};

void host_fn() {
  MemberInitializer mi; // expected-error {{}}
  BaseWithDeviceConstructor bc; // expected-error {{no matching constructor}}
  BaseWithDeviceDestructor bd; // expected-error {{}}
  DerivedFromBaseWithDeviceConstructor dfbc; // expected-error {{no matching constructor}}
  DerivedFromBaseWithDeviceDestructor dfbd; // expected-error {{}}
}

struct ConstructorDefaultArg {
  ConstructorDefaultArg(int = device_fn()) {}  // expected-error {{}}
};

void host_fn_with_default_arg(int = device_fn()) {}  // expected-error {{}}

struct A {
  __device__ void operator delete(void*) {}
  virtual ~A() {}  // expected-error {{}}
};
// TODO: Same tests as above, but with HD functions.

__constant__ int x;
void host_fn() {
  x = 42;  // should be disallowed; can't write to constant memory like this.
  int y = x;  // should be disallowed; can't read from constant memory like this.
}
