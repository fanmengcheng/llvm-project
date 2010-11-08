//===--- PPCallbacks.h - Callbacks for Preprocessor actions -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the PPCallbacks interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LEX_PPCALLBACKS_H
#define LLVM_CLANG_LEX_PPCALLBACKS_H

#include "clang/Lex/DirectoryLookup.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/StringRef.h"
#include <string>

namespace clang {
  class SourceLocation;
  class Token;
  class IdentifierInfo;
  class MacroInfo;

/// PPCallbacks - This interface provides a way to observe the actions of the
/// preprocessor as it does its thing.  Clients can define their hooks here to
/// implement preprocessor level tools.
class PPCallbacks {
public:
  virtual ~PPCallbacks();

  enum FileChangeReason {
    EnterFile, ExitFile, SystemHeaderPragma, RenameFile
  };

  /// FileChanged - This callback is invoked whenever a source file is
  /// entered or exited.  The SourceLocation indicates the new location, and
  /// EnteringFile indicates whether this is because we are entering a new
  /// #include'd file (when true) or whether we're exiting one because we ran
  /// off the end (when false).
  virtual void FileChanged(SourceLocation Loc, FileChangeReason Reason,
                           SrcMgr::CharacteristicKind FileType) {
  }

  /// FileSkipped - This callback is invoked whenever a source file is
  /// skipped as the result of header guard optimization.  ParentFile
  /// is the file that #includes the skipped file.  FilenameTok is the
  /// token in ParentFile that indicates the skipped file.
  virtual void FileSkipped(const FileEntry &ParentFile,
                           const Token &FilenameTok,
                           SrcMgr::CharacteristicKind FileType) {
  }

  /// \brief This callback is invoked whenever an inclusion directive of
  /// any kind (\c #include, \c #import, etc.) has been processed, regardless
  /// of whether the inclusion will actually result in an inclusion.
  ///
  /// \param HashLoc The location of the '#' that starts the inclusion 
  /// directive.
  ///
  /// \param IncludeTok The token that indicates the kind of inclusion 
  /// directive, e.g., 'include' or 'import'.
  ///
  /// \param FileName The name of the file being included, as written in the 
  /// source code.
  ///
  /// \param IsAngled Whether the file name was enclosed in angle brackets;
  /// otherwise, it was enclosed in quotes.
  ///
  /// \param File The actual file that may be included by this inclusion 
  /// directive.
  ///
  /// \param EndLoc The location of the last token within the inclusion
  /// directive.
  virtual void InclusionDirective(SourceLocation HashLoc,
                                  const Token &IncludeTok,
                                  llvm::StringRef FileName,
                                  bool IsAngled,
                                  const FileEntry *File,
                                  SourceLocation EndLoc) {
  }

  /// EndOfMainFile - This callback is invoked when the end of the main file is
  /// reach, no subsequent callbacks will be made.
  virtual void EndOfMainFile() {
  }

  /// Ident - This callback is invoked when a #ident or #sccs directive is read.
  /// \param Loc The location of the directive.
  /// \param str The text of the directive.
  ///
  virtual void Ident(SourceLocation Loc, const std::string &str) {
  }

  /// PragmaComment - This callback is invoked when a #pragma comment directive
  /// is read.
  ///
  virtual void PragmaComment(SourceLocation Loc, const IdentifierInfo *Kind,
                             const std::string &Str) {
  }

  /// PragmaMessage - This callback is invoked when a #pragma message directive
  /// is read.
  /// \param Loc The location of the message directive.
  /// \param str The text of the message directive.
  ///
  virtual void PragmaMessage(SourceLocation Loc, llvm::StringRef Str) {
  }

  /// MacroExpands - This is called by
  /// Preprocessor::HandleMacroExpandedIdentifier when a macro invocation is
  /// found.
  virtual void MacroExpands(const Token &Id, const MacroInfo* MI) {
  }

  /// MacroDefined - This hook is called whenever a macro definition is seen.
  virtual void MacroDefined(const IdentifierInfo *II, const MacroInfo *MI) {
  }

  /// MacroUndefined - This hook is called whenever a macro #undef is seen.
  /// MI is released immediately following this callback.
  virtual void MacroUndefined(SourceLocation Loc, const IdentifierInfo *II,
                              const MacroInfo *MI) {
  }

  /// If -- This hook is called whenever an #if is seen.
  /// \param Range The SourceRange of the expression being tested.
  // FIXME: better to pass in a list (or tree!) of Tokens.
  virtual void If(SourceRange Range) {
  }

  /// Elif -- This hook is called whenever an #elif is seen.
  /// \param Range The SourceRange of the expression being tested.
  // FIXME: better to pass in a list (or tree!) of Tokens.
  virtual void Elif(SourceRange Range) {
  }

  /// Ifdef -- This hook is called whenever an #ifdef is seen.
  /// \param Loc The location of the token being tested.
  /// \param II Information on the token being tested.
  virtual void Ifdef(SourceLocation Loc, const IdentifierInfo* II) {
  }

  /// Ifndef -- This hook is called whenever an #ifndef is seen.
  /// \param Loc The location of the token being tested.
  /// \param II Information on the token being tested.
  virtual void Ifndef(SourceLocation Loc, const IdentifierInfo* II) {
  }

  /// Else -- This hook is called whenever an #else is seen.
  virtual void Else() {
  }

  /// Endif -- This hook is called whenever an #endif is seen.
  virtual void Endif() {
  }
};

/// PPChainedCallbacks - Simple wrapper class for chaining callbacks.
class PPChainedCallbacks : public PPCallbacks {
  PPCallbacks *First, *Second;

public:
  PPChainedCallbacks(PPCallbacks *_First, PPCallbacks *_Second)
    : First(_First), Second(_Second) {}
  ~PPChainedCallbacks() {
    delete Second;
    delete First;
  }

  virtual void FileChanged(SourceLocation Loc, FileChangeReason Reason,
                           SrcMgr::CharacteristicKind FileType) {
    First->FileChanged(Loc, Reason, FileType);
    Second->FileChanged(Loc, Reason, FileType);
  }

  virtual void FileSkipped(const FileEntry &ParentFile,
                           const Token &FilenameTok,
                           SrcMgr::CharacteristicKind FileType) {
    First->FileSkipped(ParentFile, FilenameTok, FileType);
    Second->FileSkipped(ParentFile, FilenameTok, FileType);
  }

  virtual void EndOfMainFile() {
    First->EndOfMainFile();
    Second->EndOfMainFile();
  }

  virtual void Ident(SourceLocation Loc, const std::string &str) {
    First->Ident(Loc, str);
    Second->Ident(Loc, str);
  }

  virtual void PragmaComment(SourceLocation Loc, const IdentifierInfo *Kind,
                             const std::string &Str) {
    First->PragmaComment(Loc, Kind, Str);
    Second->PragmaComment(Loc, Kind, Str);
  }

  virtual void PragmaMessage(SourceLocation Loc, llvm::StringRef Str) {
    First->PragmaMessage(Loc, Str);
    Second->PragmaMessage(Loc, Str);
  }

  virtual void MacroExpands(const Token &Id, const MacroInfo* MI) {
    First->MacroExpands(Id, MI);
    Second->MacroExpands(Id, MI);
  }

  virtual void MacroDefined(const IdentifierInfo *II, const MacroInfo *MI) {
    First->MacroDefined(II, MI);
    Second->MacroDefined(II, MI);
  }

  virtual void MacroUndefined(SourceLocation Loc, const IdentifierInfo *II,
                              const MacroInfo *MI) {
    First->MacroUndefined(Loc, II, MI);
    Second->MacroUndefined(Loc, II, MI);
  }

  /// If -- This hook is called whenever an #if is seen.
  virtual void If(SourceRange Range) {
    First->If(Range);
    Second->If(Range);
  }

  /// Elif -- This hook is called whenever an #if is seen.
  virtual void Elif(SourceRange Range) {
    First->Elif(Range);
    Second->Elif(Range);
  }

  /// Ifdef -- This hook is called whenever an #ifdef is seen.
  virtual void Ifdef(SourceLocation Loc, const IdentifierInfo* II) {
    First->Ifdef(Loc, II);
    Second->Ifdef(Loc, II);
  }

  /// Ifndef -- This hook is called whenever an #ifndef is seen.
  virtual void Ifndef(SourceLocation Loc, const IdentifierInfo* II) {
    First->Ifndef(Loc, II);
    Second->Ifndef(Loc, II);
  }

  /// Else -- This hook is called whenever an #else is seen.
  virtual void Else() {
    First->Else();
    Second->Else();
  }

  /// Endif -- This hook is called whenever an #endif is seen.
  virtual void Endif() {
    First->Endif();
    Second->Endif();
  }
};

}  // end namespace clang

#endif
