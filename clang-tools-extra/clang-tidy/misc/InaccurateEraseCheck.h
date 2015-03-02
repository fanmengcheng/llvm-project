//===--- InaccurateEraseCheck.h - clang-tidy---------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_INACCURATE_ERASE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_INACCURATE_ERASE_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace misc {

/// \brief Checks for inaccurate use of \c erase() method.
///
/// Algorithms like \c remove() do not actually remove any element from the
/// container but return an iterator to the first redundant element at the end
/// of the container. These redundant elements must be removed using the
/// \c erase() method. This check warns when not all of the elements will be
/// removed due to using an inappropriate overload.
class InaccurateEraseCheck : public ClangTidyCheck {
public:
  InaccurateEraseCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_INACCURATE_ERASE_H
