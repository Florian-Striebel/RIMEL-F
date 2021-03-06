












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_WORKLIST_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_WORKLIST_H

#include "clang/StaticAnalyzer/Core/PathSensitive/BlockCounter.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExplodedGraph.h"
#include <cassert>

namespace clang {

class CFGBlock;

namespace ento {

class WorkListUnit {
ExplodedNode *node;
BlockCounter counter;
const CFGBlock *block;
unsigned blockIdx;

public:
WorkListUnit(ExplodedNode *N, BlockCounter C,
const CFGBlock *B, unsigned idx)
: node(N),
counter(C),
block(B),
blockIdx(idx) {}

explicit WorkListUnit(ExplodedNode *N, BlockCounter C)
: node(N),
counter(C),
block(nullptr),
blockIdx(0) {}


ExplodedNode *getNode() const { return node; }


BlockCounter getBlockCounter() const { return counter; }


const CFGBlock *getBlock() const { return block; }


unsigned getIndex() const { return blockIdx; }
};

class WorkList {
BlockCounter CurrentCounter;
public:
virtual ~WorkList();
virtual bool hasWork() const = 0;

virtual void enqueue(const WorkListUnit& U) = 0;

void enqueue(ExplodedNode *N, const CFGBlock *B, unsigned idx) {
enqueue(WorkListUnit(N, CurrentCounter, B, idx));
}

void enqueue(ExplodedNode *N) {
assert(N->getLocation().getKind() != ProgramPoint::PostStmtKind);
enqueue(WorkListUnit(N, CurrentCounter));
}

virtual WorkListUnit dequeue() = 0;

void setBlockCounter(BlockCounter C) { CurrentCounter = C; }
BlockCounter getBlockCounter() const { return CurrentCounter; }

static std::unique_ptr<WorkList> makeDFS();
static std::unique_ptr<WorkList> makeBFS();
static std::unique_ptr<WorkList> makeBFSBlockDFSContents();
static std::unique_ptr<WorkList> makeUnexploredFirst();
static std::unique_ptr<WorkList> makeUnexploredFirstPriorityQueue();
static std::unique_ptr<WorkList> makeUnexploredFirstPriorityLocationQueue();
};

}

}

#endif
