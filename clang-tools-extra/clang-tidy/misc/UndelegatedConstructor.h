//===--- UndelegatedConstructor.h - clang-tidy ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNDELEGATED_CONSTRUCTOR_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNDELEGATED_CONSTRUCTOR_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {

/// \brief Finds creation of temporary objects in constructors that look like a
/// function call to another constructor of the same class. The user most likely
/// meant to use a delegating constructor or base class initializer.
class UndelegatedConstructorCheck : public ClangTidyCheck {
public:
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNDELEGATED_CONSTRUCTOR_H
