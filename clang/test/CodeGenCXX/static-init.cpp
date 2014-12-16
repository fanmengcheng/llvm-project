// RUN: %clang_cc1 %s -triple=x86_64-pc-linuxs -emit-llvm -o - | FileCheck %s

// CHECK: @_ZZ1hvE1i = internal global i32 0, align 4
// CHECK: @base_req = global [4 x i8] c"foo\00", align 1
// CHECK: @base_req_uchar = global [4 x i8] c"bar\00", align 1

// CHECK: @_ZZN5test31BC1EvE1u = internal global { i8, [3 x i8] } { i8 97, [3 x i8] undef }, align 4
// CHECK: @_ZZN5test1L6getvarEiE3var = internal constant [4 x i32] [i32 1, i32 0, i32 2, i32 4], align 16

// CHECK: @_ZZ2h2vE1i = linkonce_odr global i32 0, comdat $_Z2h2v
// CHECK: @_ZGVZ2h2vE1i = linkonce_odr global i64 0, comdat $_Z2h2v

struct A {
  A();
  ~A();
};

void f() {
  // CHECK: load atomic i8* bitcast (i64* @_ZGVZ1fvE1a to i8*) acquire, align 1
  // CHECK: call i32 @__cxa_guard_acquire
  // CHECK: call void @_ZN1AC1Ev
  // CHECK: call i32 @__cxa_atexit(void (i8*)* bitcast (void (%struct.A*)* @_ZN1AD1Ev to void (i8*)*), i8* getelementptr inbounds (%struct.A* @_ZZ1fvE1a, i32 0, i32 0), i8* @__dso_handle)
  // CHECK: call void @__cxa_guard_release
  static A a;
}

void g() {
  // CHECK: call noalias i8* @_Znwm(i64 1)
  // CHECK: call void @_ZN1AC1Ev(
  static A& a = *new A;
}

int a();
void h() {
  static const int i = a();
}

// CHECK: define linkonce_odr void @_Z2h2v() {{.*}} comdat $_Z2h2v
inline void h2() {
  static int i = a();
}

void h3() {
  h2();
}

// PR6980: this shouldn't crash
namespace test0 {
  struct A { A(); };
  __attribute__((noreturn)) int throw_exception();

  void test() {
    throw_exception();
    static A r;
  }
}

namespace test1 {
  // CHECK-LABEL: define internal i32 @_ZN5test1L6getvarEi(
  static inline int getvar(int index) {
    static const int var[] = { 1, 0, 2, 4 };
    return var[index];
  }

  void test() { (void) getvar(2); }
}

// Make sure we emit the initializer correctly for the following:
char base_req[] = { "foo" };
unsigned char base_req_uchar[] = { "bar" };

namespace union_static_local {
  // CHECK-LABEL: define internal void @_ZZN18union_static_local4testEvEN1c4mainEv
  // CHECK: call void @_ZN18union_static_local1fEPNS_1xE(%"union.union_static_local::x"* bitcast ({ [2 x i8*] }* @_ZZN18union_static_local4testEvE3foo to %"union.union_static_local::x"*))
  union x { long double y; const char *x[2]; };
  void f(union x*);
  void test() {
    static union x foo = { .x = { "a", "b" } };
    struct c {
      static void main() {
        f(&foo);
      }
    };
    c::main();
  }
}

// rdar://problem/11091093
//   Static variables should be consistent across constructor
//   or destructor variants.
namespace test2 {
  struct A {
    A();
    ~A();
  };

  struct B : virtual A {
    B();
    ~B();
  };

  // If we ever implement this as a delegate ctor call, just change
  // this to take variadic arguments or something.
  extern int foo();
  B::B() {
    static int x = foo();
  }
  // CHECK-LABEL: define void @_ZN5test21BC2Ev
  // CHECK:   load atomic i8* bitcast (i64* @_ZGVZN5test21BC1EvE1x to i8*) acquire,
  // CHECK:   call i32 @__cxa_guard_acquire(i64* @_ZGVZN5test21BC1EvE1x)
  // CHECK:   [[T0:%.*]] = call i32 @_ZN5test23fooEv()
  // CHECK:   store i32 [[T0]], i32* @_ZZN5test21BC1EvE1x,
  // CHECK:   call void @__cxa_guard_release(i64* @_ZGVZN5test21BC1EvE1x)

  // CHECK-LABEL: define void @_ZN5test21BC1Ev
  // CHECK:   load atomic i8* bitcast (i64* @_ZGVZN5test21BC1EvE1x to i8*) acquire,
  // CHECK:   call i32 @__cxa_guard_acquire(i64* @_ZGVZN5test21BC1EvE1x)
  // CHECK:   [[T0:%.*]] = call i32 @_ZN5test23fooEv()
  // CHECK:   store i32 [[T0]], i32* @_ZZN5test21BC1EvE1x,
  // CHECK:   call void @__cxa_guard_release(i64* @_ZGVZN5test21BC1EvE1x)

  // This is just for completeness, because we actually emit this
  // using a delegate dtor call.
  B::~B() {
    static int y = foo();
  }
  // CHECK-LABEL: define void @_ZN5test21BD2Ev(
  // CHECK:   load atomic i8* bitcast (i64* @_ZGVZN5test21BD1EvE1y to i8*) acquire,
  // CHECK:   call i32 @__cxa_guard_acquire(i64* @_ZGVZN5test21BD1EvE1y)
  // CHECK:   [[T0:%.*]] = call i32 @_ZN5test23fooEv()
  // CHECK:   store i32 [[T0]], i32* @_ZZN5test21BD1EvE1y,
  // CHECK:   call void @__cxa_guard_release(i64* @_ZGVZN5test21BD1EvE1y)

  // CHECK-LABEL: define void @_ZN5test21BD1Ev(
  // CHECK:   call void @_ZN5test21BD2Ev(
}

// This shouldn't error out.
namespace test3 {
  struct A {
    A();
    ~A();
  };

  struct B : virtual A {
    B();
    ~B();
  };

  B::B() {
    union U { char x; int i; };
    static U u = { 'a' };
  }
  // CHECK-LABEL: define void @_ZN5test31BC2Ev(
  // CHECK-LABEL: define void @_ZN5test31BC1Ev(
}
