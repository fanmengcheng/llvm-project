// RUN: %clang_cc1 -fsyntax-only -triple cheri-unknown-freebsd %s -verify
// expected-no-diagnostics
void
foo(void)
{
	int *b = (int[]){0,1,2};
}
