// RUN: %clang_cc1 -triple x86_64-apple-darwin10 -fobjc-nonfragile-abi -fsyntax-only -fobjc-arc -fobjc-no-arc-runtime -x objective-c %s.result
// RUN: arcmt-test --args -triple x86_64-apple-macosx10.6 -fobjc-nonfragile-abi -fsyntax-only %s > %t
// RUN: diff %t %s.result

#include "Common.h"

@interface Foo : NSObject {
  NSObject *x;
}
@property (readonly,assign) id x;
@end

@implementation Foo
@synthesize x;
@end
