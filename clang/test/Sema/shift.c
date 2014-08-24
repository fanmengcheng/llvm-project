// RUN: %clang_cc1 -Wall -Wshift-sign-overflow -ffreestanding -fsyntax-only -verify %s

#include <limits.h>

#define WORD_BIT (sizeof(int) * CHAR_BIT)

enum {
  X = 1 << 0,
  Y = 1 << 1,
  Z = 1 << 2
};

void test() {
  char c;

  c = 0 << 0;
  c = 0 << 1;
  c = 1 << 0;
  c = 1 << -0;
  c = 1 >> -0;
  c = 1 << -1; // expected-warning {{shift count is negative}}
  c = 1 >> -1; // expected-warning {{shift count is negative}}
  c = 1 << c;
  c <<= 0;
  c >>= 0;
  c <<= 1;
  c >>= 1;
  c <<= -1; // expected-warning {{shift count is negative}}
  c >>= -1; // expected-warning {{shift count is negative}}
  c <<= 999999; // expected-warning {{shift count >= width of type}}
  c >>= 999999; // expected-warning {{shift count >= width of type}}
  c <<= CHAR_BIT; // expected-warning {{shift count >= width of type}}
  c >>= CHAR_BIT; // expected-warning {{shift count >= width of type}}
  c <<= CHAR_BIT+1; // expected-warning {{shift count >= width of type}}
  c >>= CHAR_BIT+1; // expected-warning {{shift count >= width of type}}
  (void)((long)c << CHAR_BIT);

  int i;
  i = 1 << (WORD_BIT - 2);
  i = 2 << (WORD_BIT - 1); // expected-warning {{bits to represent, but 'int' only has}}
  i = 1 << (WORD_BIT - 1); // expected-warning {{sets the sign bit of the shift expression}}
  i = -1 << (WORD_BIT - 1);
  i = 0 << (WORD_BIT - 1);
  i = (char)1 << (WORD_BIT - 2);

  unsigned u;
  u = 1U << (WORD_BIT - 1);
  u = 5U << (WORD_BIT - 1);

  long long int lli;
  lli = INT_MIN << 2; // expected-warning {{bits to represent, but 'int' only has}}
  lli = 1LL << (sizeof(long long) * CHAR_BIT - 2);
}

#define a 0
#define ashift 8
enum { b = (a << ashift) };

// Don't warn for negative shifts in code that is unreachable.
void test_pr5544() {
  (void) (((1) > 63 && (1) < 128 ? (((unsigned long long) 1)<<((1)-64)) : (unsigned long long) 0)); // no-warning
}

void test_shift_too_much(char x) {
  if (0)
    (void) (x >> 80); // no-warning
  (void) (x >> 80); // expected-warning {{shift count >= width of type}}
}
