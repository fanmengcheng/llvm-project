// RUN: %clang_cc1 -fsyntax-only -cxx-abi itanium -verify %s
// RUN: %clang_cc1 -fsyntax-only -cxx-abi microsoft -DMSABI -verify %s

typedef typeof(sizeof(int)) size_t;

// PR7803
namespace test0 {
  class A {
  public:
    static void operator delete(void *p) {};
    virtual ~A();
  };

  class B : protected A {
  public:
    ~B();
  };

  class C : protected B {
  public:
    using B::operator delete;
    ~C();
  };

  // Shouldn't have an error.
  C::~C() {}
}

namespace test1 {
  class A {
  public:
#ifdef MSABI
    // expected-note@+2 {{declared here}}
#endif
    static void operator delete(void *p) {}; // expected-note {{member 'operator delete' declared here}}
    virtual ~A();
  };

  class B : protected A {
  public:
#ifdef MSABI
    // expected-note@+2 {{declared here}}
#endif
    static void operator delete(void *, size_t) {}; // expected-note {{member 'operator delete' declared here}}
    ~B();
  };

  class C : protected B {
  public:
    using A::operator delete;
    using B::operator delete;

#ifdef MSABI
    // expected-error@+2 {{multiple suitable 'operator delete' functions in 'C'}}
#endif
    ~C();
  };

  C::~C() {} // expected-error {{multiple suitable 'operator delete' functions in 'C'}}
}

// ...at the point of definition of a virtual destructor...
namespace test2 {
  struct A {
#ifdef MSABI
    // expected-error@+3 {{no suitable member 'operator delete' in 'A'}}
    // expected-note@+3 {{declared here}}
#endif
    virtual ~A();
    static void operator delete(void*, const int &);
  };

  struct B {
#ifdef MSABI
    // expected-error@+2 {{no suitable member 'operator delete' in 'B'}}
#endif
    virtual ~B();
#ifdef MSABI
    // expected-note@+2 {{declared here}}
#endif
    static void operator delete(void*, const int &); // expected-note {{declared here}}
  };
  B::~B() {} // expected-error {{no suitable member 'operator delete' in 'B'}}

  struct CBase { virtual ~CBase(); };
  struct C : CBase { // expected-error {{no suitable member 'operator delete' in 'C'}}
    static void operator delete(void*, const int &); // expected-note {{declared here}}
  };
  void test() {
    C c; // expected-note {{first required here}}
  }
}

// PR7346
namespace test3 {
  struct A {
#ifdef MSABI
    // expected-error@+2 {{no suitable member 'operator delete' in 'A'}}
#endif
    virtual ~A();
#ifdef MSABI
    // expected-note@+2 {{declared here}}
#endif
    static void operator delete(void*, const int &);
  };

  struct B : A {
    virtual ~B() {}
    static void operator delete(void*);
  };
}
