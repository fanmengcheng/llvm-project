// RUN: %clang_cc1 -fsyntax-only -verify %s
// pr7390

void f(const SEL& v2) {}
void g() {
  f(@selector(dealloc));

  SEL s = @selector(dealloc);
  SEL* ps = &s;

  @selector(dealloc) = s;  // expected-error {{expression is not assignable}}

  SEL* ps2 = &@selector(dealloc);
}
