











#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_POSTORDERCFGVIEW_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_POSTORDERCFGVIEW_H

#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/CFG.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/PostOrderIterator.h"
#include <utility>
#include <vector>

namespace clang {

class PostOrderCFGView : public ManagedAnalysis {
virtual void anchor();

public:






class CFGBlockSet {
llvm::BitVector VisitedBlockIDs;

public:


struct iterator { using value_type = const CFGBlock *; };

CFGBlockSet() = default;
CFGBlockSet(const CFG *G) : VisitedBlockIDs(G->getNumBlockIDs(), false) {}



std::pair<llvm::NoneType, bool> insert(const CFGBlock *Block) {




if (!Block)
return std::make_pair(None, false);
if (VisitedBlockIDs.test(Block->getBlockID()))
return std::make_pair(None, false);
VisitedBlockIDs.set(Block->getBlockID());
return std::make_pair(None, true);
}




bool alreadySet(const CFGBlock *Block) {
return VisitedBlockIDs.test(Block->getBlockID());
}
};

private:
using po_iterator = llvm::po_iterator<const CFG *, CFGBlockSet, true>;
std::vector<const CFGBlock *> Blocks;

using BlockOrderTy = llvm::DenseMap<const CFGBlock *, unsigned>;
BlockOrderTy BlockOrder;

public:
friend struct BlockOrderCompare;

using iterator = std::vector<const CFGBlock *>::reverse_iterator;
using const_iterator = std::vector<const CFGBlock *>::const_reverse_iterator;

PostOrderCFGView(const CFG *cfg);

iterator begin() { return Blocks.rbegin(); }
iterator end() { return Blocks.rend(); }

const_iterator begin() const { return Blocks.rbegin(); }
const_iterator end() const { return Blocks.rend(); }

bool empty() const { return begin() == end(); }

struct BlockOrderCompare {
const PostOrderCFGView &POV;

public:
BlockOrderCompare(const PostOrderCFGView &pov) : POV(pov) {}

bool operator()(const CFGBlock *b1, const CFGBlock *b2) const;
};

BlockOrderCompare getComparator() const {
return BlockOrderCompare(*this);
}


static const void *getTag();

static std::unique_ptr<PostOrderCFGView>
create(AnalysisDeclContext &analysisContext);
};

}

#endif
