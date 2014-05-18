// RUN: %clang_cc1 -analyze -analyzer-checker=core,osx.cocoa.RetainCount,debug.ExprInspection -analyzer-store=region -verify -Wno-objc-root-class %s

void clang_analyzer_eval(bool);
void clang_analyzer_checkInlined(bool);

@interface IntWrapper
@property (readonly) int &value;
@end

@implementation IntWrapper
@synthesize value;
@end

void testReferenceConsistency(IntWrapper *w) {
  clang_analyzer_eval(w.value == w.value); // expected-warning{{TRUE}}
  clang_analyzer_eval(&w.value == &w.value); // expected-warning{{TRUE}}

  if (w.value != 42)
    return;

  clang_analyzer_eval(w.value == 42); // expected-warning{{TRUE}}
}

void testReferenceAssignment(IntWrapper *w) {
  w.value = 42;
  clang_analyzer_eval(w.value == 42); // expected-warning{{TRUE}}
}


// FIXME: Handle C++ structs, which need to go through the copy constructor.

struct IntWrapperStruct {
  int value;
};

@interface StructWrapper
@property IntWrapperStruct inner;
@end

@implementation StructWrapper
@synthesize inner;
@end

void testConsistencyStruct(StructWrapper *w) {
  clang_analyzer_eval(w.inner.value == w.inner.value); // expected-warning{{UNKNOWN}}

  int origValue = w.inner.value;
  if (origValue != 42)
    return;

  clang_analyzer_eval(w.inner.value == 42); // expected-warning{{UNKNOWN}}
}


class CustomCopy {
public:
  CustomCopy() : value(0) {}
  CustomCopy(const CustomCopy &other) {
    clang_analyzer_checkInlined(false);
  }
  int value;
};

@interface CustomCopyWrapper
@property CustomCopy inner;
@end

@implementation CustomCopyWrapper
@synthesize inner;
@end

void testConsistencyCustomCopy(CustomCopyWrapper *w) {
  clang_analyzer_eval(w.inner.value == w.inner.value); // expected-warning{{UNKNOWN}}

  int origValue = w.inner.value;
  if (origValue != 42)
    return;

  clang_analyzer_eval(w.inner.value == 42); // expected-warning{{UNKNOWN}}
}
