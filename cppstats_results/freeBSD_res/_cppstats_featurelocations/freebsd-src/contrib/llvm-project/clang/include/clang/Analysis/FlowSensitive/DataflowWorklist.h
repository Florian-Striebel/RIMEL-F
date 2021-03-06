










#if !defined(LLVM_CLANG_ANALYSIS_FLOWSENSITIVE_DATAFLOWWORKLIST_H)
#define LLVM_CLANG_ANALYSIS_FLOWSENSITIVE_DATAFLOWWORKLIST_H

#include "clang/Analysis/Analyses/PostOrderCFGView.h"
#include "clang/Analysis/CFG.h"
#include "llvm/ADT/PriorityQueue.h"

namespace clang {


template <typename Comp, unsigned QueueSize> class DataflowWorklistBase {
llvm::BitVector EnqueuedBlocks;
PostOrderCFGView *POV;
llvm::PriorityQueue<const CFGBlock *,
SmallVector<const CFGBlock *, QueueSize>, Comp>
WorkList;

public:
DataflowWorklistBase(const CFG &Cfg, PostOrderCFGView *POV, Comp C)
: EnqueuedBlocks(Cfg.getNumBlockIDs()), POV(POV), WorkList(C) {}

const PostOrderCFGView *getCFGView() const { return POV; }

void enqueueBlock(const CFGBlock *Block) {
if (Block && !EnqueuedBlocks[Block->getBlockID()]) {
EnqueuedBlocks[Block->getBlockID()] = true;
WorkList.push(Block);
}
}

const CFGBlock *dequeue() {
if (WorkList.empty())
return nullptr;
const CFGBlock *B = WorkList.top();
WorkList.pop();
EnqueuedBlocks[B->getBlockID()] = false;
return B;
}
};

struct ReversePostOrderCompare {
PostOrderCFGView::BlockOrderCompare Cmp;
bool operator()(const CFGBlock *lhs, const CFGBlock *rhs) const {
return Cmp(rhs, lhs);
}
};




struct ForwardDataflowWorklist
: DataflowWorklistBase<ReversePostOrderCompare, 20> {
ForwardDataflowWorklist(const CFG &Cfg, AnalysisDeclContext &Ctx)
: DataflowWorklistBase(
Cfg, Ctx.getAnalysis<PostOrderCFGView>(),
ReversePostOrderCompare{
Ctx.getAnalysis<PostOrderCFGView>()->getComparator()}) {}

void enqueueSuccessors(const CFGBlock *Block) {
for (auto B : Block->succs())
enqueueBlock(B);
}
};




struct BackwardDataflowWorklist
: DataflowWorklistBase<PostOrderCFGView::BlockOrderCompare, 20> {
BackwardDataflowWorklist(const CFG &Cfg, AnalysisDeclContext &Ctx)
: DataflowWorklistBase(
Cfg, Ctx.getAnalysis<PostOrderCFGView>(),
Ctx.getAnalysis<PostOrderCFGView>()->getComparator()) {}

void enqueuePredecessors(const CFGBlock *Block) {
for (auto B : Block->preds())
enqueueBlock(B);
}
};

}

#endif
