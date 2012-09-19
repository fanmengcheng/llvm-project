// RUN: %clang_cc1 -fsyntax-only -verify -std=c++11 %s

// Core DR 532.
namespace PR8130 {
  struct A { };

  template<class T> struct B {
    template<class R> int &operator*(R&);
  };

  template<class T, class R> float &operator*(T&, R&);
  void test() {
    A a;
    B<A> b;
    int &ir = b * a;
  }
}

namespace OrderWithStaticMember {
  struct A {
    template<class T> int g(T**, int=0) { return 0; }
    template<class T> static int g(T*) { return 1; }
  };
  void f() {
    A a;
    int **p;
    a.g(p);
  }
}
