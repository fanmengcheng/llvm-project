//===-- Internalize.cpp - Mark functions internal -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass loops over all of the functions and variables in the input module.
// If the function or variable is not in the list of external names given to
// the pass it is marked as internal.
//
// This transformation would not be legal in a regular compilation, but it gets
// extra information from the linker about what is safe.
//
// For example: Internalizing a function with external linkage. Only if we are
// told it is only used from within this module, it is safe to do it.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/IPO.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/GlobalStatus.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include <fstream>
#include <set>
using namespace llvm;

#define DEBUG_TYPE "internalize"

STATISTIC(NumAliases  , "Number of aliases internalized");
STATISTIC(NumFunctions, "Number of functions internalized");
STATISTIC(NumGlobals  , "Number of global vars internalized");

// APIFile - A file which contains a list of symbols that should not be marked
// external.
static cl::opt<std::string>
APIFile("internalize-public-api-file", cl::value_desc("filename"),
        cl::desc("A file containing list of symbol names to preserve"));

// APIList - A list of symbols that should not be marked internal.
static cl::list<std::string>
APIList("internalize-public-api-list", cl::value_desc("list"),
        cl::desc("A list of symbol names to preserve"),
        cl::CommaSeparated);

namespace {
  class InternalizePass : public ModulePass {
    StringSet<> ExternalNames;

  public:
    static char ID; // Pass identification, replacement for typeid
    explicit InternalizePass();
    explicit InternalizePass(ArrayRef<const char *> ExportList);
    explicit InternalizePass(StringSet<> ExportList);
    void LoadFile(const char *Filename);
    bool maybeInternalize(GlobalValue &GV,
                          const std::set<const Comdat *> &ExternalComdats);
    void checkComdatVisibility(GlobalValue &GV,
                               std::set<const Comdat *> &ExternalComdats);
    bool runOnModule(Module &M) override;

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesCFG();
      AU.addPreserved<CallGraphWrapperPass>();
    }
  };
} // end anonymous namespace

char InternalizePass::ID = 0;
INITIALIZE_PASS(InternalizePass, "internalize",
                "Internalize Global Symbols", false, false)

InternalizePass::InternalizePass() : ModulePass(ID) {
  initializeInternalizePassPass(*PassRegistry::getPassRegistry());
  if (!APIFile.empty())           // If a filename is specified, use it.
    LoadFile(APIFile.c_str());
  ExternalNames.insert(APIList.begin(), APIList.end());
}

InternalizePass::InternalizePass(ArrayRef<const char *> ExportList)
    : ModulePass(ID) {
  initializeInternalizePassPass(*PassRegistry::getPassRegistry());
  for(ArrayRef<const char *>::const_iterator itr = ExportList.begin();
        itr != ExportList.end(); itr++) {
    ExternalNames.insert(*itr);
  }
}

InternalizePass::InternalizePass(StringSet<> ExportList)
    : ModulePass(ID), ExternalNames(std::move(ExportList)) {}

void InternalizePass::LoadFile(const char *Filename) {
  // Load the APIFile...
  std::ifstream In(Filename);
  if (!In.good()) {
    errs() << "WARNING: Internalize couldn't load file '" << Filename
         << "'! Continuing as if it's empty.\n";
    return; // Just continue as if the file were empty
  }
  while (In) {
    std::string Symbol;
    In >> Symbol;
    if (!Symbol.empty())
      ExternalNames.insert(Symbol);
  }
}

static bool isExternallyVisible(const GlobalValue &GV,
                                const StringSet<> &ExternalNames) {
  // Function must be defined here
  if (GV.isDeclaration())
    return true;

  // Available externally is really just a "declaration with a body".
  if (GV.hasAvailableExternallyLinkage())
    return true;

  // Assume that dllexported symbols are referenced elsewhere
  if (GV.hasDLLExportStorageClass())
    return true;

  // Marked to keep external?
  if (!GV.hasLocalLinkage() && ExternalNames.count(GV.getName()))
    return true;

  return false;
}

// Internalize GV if it is possible to do so, i.e. it is not externally visible
// and is not a member of an externally visible comdat.
bool InternalizePass::maybeInternalize(
    GlobalValue &GV, const std::set<const Comdat *> &ExternalComdats) {
  if (Comdat *C = GV.getComdat()) {
    if (ExternalComdats.count(C))
      return false;

    // If a comdat is not externally visible we can drop it.
    if (auto GO = dyn_cast<GlobalObject>(&GV))
      GO->setComdat(nullptr);

    if (GV.hasLocalLinkage())
      return false;
  } else {
    if (GV.hasLocalLinkage())
      return false;

    if (isExternallyVisible(GV, ExternalNames))
      return false;
  }

  GV.setVisibility(GlobalValue::DefaultVisibility);
  GV.setLinkage(GlobalValue::InternalLinkage);
  return true;
}

// If GV is part of a comdat and is externally visible, keep track of its
// comdat so that we don't internalize any of its members.
void InternalizePass::checkComdatVisibility(
    GlobalValue &GV, std::set<const Comdat *> &ExternalComdats) {
  Comdat *C = GV.getComdat();
  if (!C)
    return;

  if (isExternallyVisible(GV, ExternalNames))
    ExternalComdats.insert(C);
}

bool InternalizePass::runOnModule(Module &M) {
  CallGraphWrapperPass *CGPass = getAnalysisIfAvailable<CallGraphWrapperPass>();
  CallGraph *CG = CGPass ? &CGPass->getCallGraph() : nullptr;
  CallGraphNode *ExternalNode = CG ? CG->getExternalCallingNode() : nullptr;

  SmallPtrSet<GlobalValue *, 8> Used;
  collectUsedGlobalVariables(M, Used, false);

  // Collect comdat visiblity information for the module.
  std::set<const Comdat *> ExternalComdats;
  if (!M.getComdatSymbolTable().empty()) {
    for (Function &F : M)
      checkComdatVisibility(F, ExternalComdats);
    for (GlobalVariable &GV : M.globals())
      checkComdatVisibility(GV, ExternalComdats);
    for (GlobalAlias &GA : M.aliases())
      checkComdatVisibility(GA, ExternalComdats);
  }

  // We must assume that globals in llvm.used have a reference that not even
  // the linker can see, so we don't internalize them.
  // For llvm.compiler.used the situation is a bit fuzzy. The assembler and
  // linker can drop those symbols. If this pass is running as part of LTO,
  // one might think that it could just drop llvm.compiler.used. The problem
  // is that even in LTO llvm doesn't see every reference. For example,
  // we don't see references from function local inline assembly. To be
  // conservative, we internalize symbols in llvm.compiler.used, but we
  // keep llvm.compiler.used so that the symbol is not deleted by llvm.
  for (GlobalValue *V : Used) {
    ExternalNames.insert(V->getName());
  }

  // Mark all functions not in the api as internal.
  for (Function &I : M) {
    if (!maybeInternalize(I, ExternalComdats))
      continue;

    if (ExternalNode)
      // Remove a callgraph edge from the external node to this function.
      ExternalNode->removeOneAbstractEdgeTo((*CG)[&I]);

    ++NumFunctions;
    DEBUG(dbgs() << "Internalizing func " << I.getName() << "\n");
  }

  // Never internalize the llvm.used symbol.  It is used to implement
  // attribute((used)).
  // FIXME: Shouldn't this just filter on llvm.metadata section??
  ExternalNames.insert("llvm.used");
  ExternalNames.insert("llvm.compiler.used");

  // Never internalize anchors used by the machine module info, else the info
  // won't find them.  (see MachineModuleInfo.)
  ExternalNames.insert("llvm.global_ctors");
  ExternalNames.insert("llvm.global_dtors");
  ExternalNames.insert("llvm.global.annotations");

  // Never internalize symbols code-gen inserts.
  // FIXME: We should probably add this (and the __stack_chk_guard) via some
  // type of call-back in CodeGen.
  ExternalNames.insert("__stack_chk_fail");
  ExternalNames.insert("__stack_chk_guard");

  // Mark all global variables with initializers that are not in the api as
  // internal as well.
  for (Module::global_iterator I = M.global_begin(), E = M.global_end();
       I != E; ++I) {
    if (!maybeInternalize(*I, ExternalComdats))
      continue;

    ++NumGlobals;
    DEBUG(dbgs() << "Internalized gvar " << I->getName() << "\n");
  }

  // Mark all aliases that are not in the api as internal as well.
  for (Module::alias_iterator I = M.alias_begin(), E = M.alias_end();
       I != E; ++I) {
    if (!maybeInternalize(*I, ExternalComdats))
      continue;

    ++NumAliases;
    DEBUG(dbgs() << "Internalized alias " << I->getName() << "\n");
  }

  // We do not keep track of whether this pass changed the module because
  // it adds unnecessary complexity:
  // 1) This pass will generally be near the start of the pass pipeline, so
  //    there will be no analyses to invalidate.
  // 2) This pass will most likely end up changing the module and it isn't worth
  //    worrying about optimizing the case where the module is unchanged.
  return true;
}

ModulePass *llvm::createInternalizePass() { return new InternalizePass(); }

ModulePass *llvm::createInternalizePass(ArrayRef<const char *> ExportList) {
  return new InternalizePass(ExportList);
}

ModulePass *llvm::createInternalizePass(StringSet<> ExportList) {
  return new InternalizePass(std::move(ExportList));
}
