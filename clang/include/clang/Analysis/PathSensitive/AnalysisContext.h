//=== AnalysisContext.h - Analysis context for Path Sens analysis --*- C++ -*-//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines AnalysisContext, a class that manages the analysis context
// data for path sensitive analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_ANALYSIS_ANALYSISCONTEXT_H
#define LLVM_CLANG_ANALYSIS_ANALYSISCONTEXT_H

#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/FoldingSet.h"
#include <map>

namespace clang {

class Decl;
class Stmt;
class CFG;
class LiveVariables;
class ParentMap;

/// AnalysisContext contains the context data for the function or method under
/// analysis.
class AnalysisContext {
  Decl *D;
  Stmt *Body;

  // AnalysisContext owns the following data.
  CFG *cfg;
  LiveVariables *liveness;
  ParentMap *PM;

public:
  AnalysisContext() : D(0), Body(0), cfg(0), liveness(0), PM(0) {}
  ~AnalysisContext();

  void setDecl(Decl* d) { D = d; }
  Decl *getDecl() { return D; }
  Stmt *getBody();
  CFG *getCFG();
  ParentMap &getParentMap();
  LiveVariables *getLiveVariables();
};

class AnalysisContextManager {
  std::map<Decl*, AnalysisContext> Contexts;

public:
  typedef std::map<Decl*, AnalysisContext>::iterator iterator;

  AnalysisContext *getContext(Decl *D);
};

class LocationContext : public llvm::FoldingSetNode {
public:
  enum ContextKind { StackFrame, Scope };

private:
  ContextKind Kind;
  AnalysisContext *Ctx;
  LocationContext *Parent;

protected:
  LocationContext(ContextKind k, AnalysisContext *ctx, LocationContext *parent)
    : Kind(k), Ctx(ctx), Parent(parent) {}

public:
  ContextKind getKind() const { return Kind; }

  AnalysisContext *getAnalysisContext() const { return Ctx; }

  LocationContext *getParent() const { return Parent; }

  void Profile(llvm::FoldingSetNodeID &ID) {
    Profile(ID, Kind, Ctx, Parent);
  }

  static void Profile(llvm::FoldingSetNodeID &ID, ContextKind k,
                      AnalysisContext *ctx, LocationContext *parent);

  static bool classof(const LocationContext*) { return true; }
};

class StackFrameContext : public LocationContext {
  Stmt *CallSite;

public:
  StackFrameContext(AnalysisContext *ctx, LocationContext *parent, Stmt *s)
    : LocationContext(StackFrame, ctx, parent), CallSite(s) {}

  void Profile(llvm::FoldingSetNodeID &ID) {
    Profile(ID, getAnalysisContext(), getParent(), CallSite);
  }

  static void Profile(llvm::FoldingSetNodeID &ID, AnalysisContext *ctx,
                      LocationContext *parent, Stmt *s);

  static bool classof(const LocationContext* Ctx) { 
    return Ctx->getKind() == StackFrame; 
  }
};

class ScopeContext : public LocationContext {
  Stmt *Enter;

public:
  ScopeContext(AnalysisContext *ctx, LocationContext *parent, Stmt *s)
    : LocationContext(Scope, ctx, parent), Enter(s) {}

  void Profile(llvm::FoldingSetNodeID &ID) {
    Profile(ID, getAnalysisContext(), getParent(), Enter);
  }

  static void Profile(llvm::FoldingSetNodeID &ID, AnalysisContext *ctx,
                      LocationContext *parent, Stmt *s);

  static bool classof(const LocationContext* Ctx) { 
    return Ctx->getKind() == Scope; 
  }
};

class LocationContextManager {
  llvm::FoldingSet<LocationContext> Contexts;

public:
  StackFrameContext *getStackFrame(AnalysisContext *ctx,LocationContext *parent,
                                   Stmt *s);

  ScopeContext *getScope(AnalysisContext *ctx,LocationContext *parent, Stmt *s);

};

}

#endif
