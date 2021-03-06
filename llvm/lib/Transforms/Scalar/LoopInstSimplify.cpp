//===- LoopInstSimplify.cpp - Loop Instruction Simplification Pass --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass performs lightweight instruction simplification on loop bodies.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Scalar/LoopInstSimplify.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopPassManager.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
using namespace llvm;

#define DEBUG_TYPE "loop-instsimplify"

STATISTIC(NumSimplified, "Number of redundant instructions simplified");

static bool SimplifyLoopInst(Loop *L, DominatorTree *DT, LoopInfo *LI,
                             AssumptionCache *AC,
                             const TargetLibraryInfo *TLI) {
  SmallVector<BasicBlock *, 8> ExitBlocks;
  L->getUniqueExitBlocks(ExitBlocks);
  array_pod_sort(ExitBlocks.begin(), ExitBlocks.end());

  SmallPtrSet<const Instruction *, 8> S1, S2, *ToSimplify = &S1, *Next = &S2;

  // The bit we are stealing from the pointer represents whether this basic
  // block is the header of a subloop, in which case we only process its phis.
  typedef PointerIntPair<BasicBlock *, 1> WorklistItem;
  SmallVector<WorklistItem, 16> VisitStack;
  SmallPtrSet<BasicBlock *, 32> Visited;

  bool Changed = false;
  bool LocalChanged;
  do {
    LocalChanged = false;

    VisitStack.clear();
    Visited.clear();

    VisitStack.push_back(WorklistItem(L->getHeader(), false));

    while (!VisitStack.empty()) {
      WorklistItem Item = VisitStack.pop_back_val();
      BasicBlock *BB = Item.getPointer();
      bool IsSubloopHeader = Item.getInt();
      const DataLayout &DL = L->getHeader()->getModule()->getDataLayout();

      // Simplify instructions in the current basic block.
      for (BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE;) {
        Instruction *I = &*BI++;

        // The first time through the loop ToSimplify is empty and we try to
        // simplify all instructions. On later iterations ToSimplify is not
        // empty and we only bother simplifying instructions that are in it.
        if (!ToSimplify->empty() && !ToSimplify->count(I))
          continue;

        // Don't bother simplifying unused instructions.
        if (!I->use_empty()) {
          Value *V = SimplifyInstruction(I, DL, TLI, DT, AC);
          if (V && LI->replacementPreservesLCSSAForm(I, V)) {
            // Mark all uses for resimplification next time round the loop.
            for (User *U : I->users())
              Next->insert(cast<Instruction>(U));

            I->replaceAllUsesWith(V);
            LocalChanged = true;
            ++NumSimplified;
          }
        }
        if (RecursivelyDeleteTriviallyDeadInstructions(I, TLI)) {
          // RecursivelyDeleteTriviallyDeadInstruction can remove more than one
          // instruction, so simply incrementing the iterator does not work.
          // When instructions get deleted re-iterate instead.
          BI = BB->begin();
          BE = BB->end();
          LocalChanged = true;
        }

        if (IsSubloopHeader && !isa<PHINode>(I))
          break;
      }

      // Add all successors to the worklist, except for loop exit blocks and the
      // bodies of subloops. We visit the headers of loops so that we can
      // process
      // their phis, but we contract the rest of the subloop body and only
      // follow
      // edges leading back to the original loop.
      for (succ_iterator SI = succ_begin(BB), SE = succ_end(BB); SI != SE;
           ++SI) {
        BasicBlock *SuccBB = *SI;
        if (!Visited.insert(SuccBB).second)
          continue;

        const Loop *SuccLoop = LI->getLoopFor(SuccBB);
        if (SuccLoop && SuccLoop->getHeader() == SuccBB &&
            L->contains(SuccLoop)) {
          VisitStack.push_back(WorklistItem(SuccBB, true));

          SmallVector<BasicBlock *, 8> SubLoopExitBlocks;
          SuccLoop->getExitBlocks(SubLoopExitBlocks);

          for (unsigned i = 0; i < SubLoopExitBlocks.size(); ++i) {
            BasicBlock *ExitBB = SubLoopExitBlocks[i];
            if (LI->getLoopFor(ExitBB) == L && Visited.insert(ExitBB).second)
              VisitStack.push_back(WorklistItem(ExitBB, false));
          }

          continue;
        }

        bool IsExitBlock =
            std::binary_search(ExitBlocks.begin(), ExitBlocks.end(), SuccBB);
        if (IsExitBlock)
          continue;

        VisitStack.push_back(WorklistItem(SuccBB, false));
      }
    }

    // Place the list of instructions to simplify on the next loop iteration
    // into ToSimplify.
    std::swap(ToSimplify, Next);
    Next->clear();

    Changed |= LocalChanged;
  } while (LocalChanged);

  return Changed;
}

namespace {
class LoopInstSimplifyLegacyPass : public LoopPass {
public:
  static char ID; // Pass ID, replacement for typeid
  LoopInstSimplifyLegacyPass() : LoopPass(ID) {
    initializeLoopInstSimplifyLegacyPassPass(*PassRegistry::getPassRegistry());
  }

  bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    if (skipLoop(L))
      return false;
    DominatorTreeWrapperPass *DTWP =
        getAnalysisIfAvailable<DominatorTreeWrapperPass>();
    DominatorTree *DT = DTWP ? &DTWP->getDomTree() : nullptr;
    LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    AssumptionCache *AC =
        &getAnalysis<AssumptionCacheTracker>().getAssumptionCache(
            *L->getHeader()->getParent());
    const TargetLibraryInfo *TLI =
        &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();

    return SimplifyLoopInst(L, DT, LI, AC, TLI);
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<AssumptionCacheTracker>();
    AU.addRequired<TargetLibraryInfoWrapperPass>();
    AU.setPreservesCFG();
    getLoopAnalysisUsage(AU);
  }
};
}

PreservedAnalyses LoopInstSimplifyPass::run(Loop &L,
                                            LoopAnalysisManager &AM) {
  const auto &FAM =
      AM.getResult<FunctionAnalysisManagerLoopProxy>(L).getManager();
  Function *F = L.getHeader()->getParent();

  // Use getCachedResult because Loop pass cannot trigger a function analysis.
  auto *DT = FAM.getCachedResult<DominatorTreeAnalysis>(*F);
  auto *LI = FAM.getCachedResult<LoopAnalysis>(*F);
  auto *AC = FAM.getCachedResult<AssumptionAnalysis>(*F);
  const auto *TLI = FAM.getCachedResult<TargetLibraryAnalysis>(*F);
  assert((LI && AC && TLI) && "Analyses for Loop Inst Simplify not available");

  if (!SimplifyLoopInst(&L, DT, LI, AC, TLI))
    return PreservedAnalyses::all();

  return getLoopPassPreservedAnalyses();
}

char LoopInstSimplifyLegacyPass::ID = 0;
INITIALIZE_PASS_BEGIN(LoopInstSimplifyLegacyPass, "loop-instsimplify",
                      "Simplify instructions in loops", false, false)
INITIALIZE_PASS_DEPENDENCY(AssumptionCacheTracker)
INITIALIZE_PASS_DEPENDENCY(LoopPass)
INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)
INITIALIZE_PASS_END(LoopInstSimplifyLegacyPass, "loop-instsimplify",
                    "Simplify instructions in loops", false, false)

Pass *llvm::createLoopInstSimplifyPass() {
  return new LoopInstSimplifyLegacyPass();
}
