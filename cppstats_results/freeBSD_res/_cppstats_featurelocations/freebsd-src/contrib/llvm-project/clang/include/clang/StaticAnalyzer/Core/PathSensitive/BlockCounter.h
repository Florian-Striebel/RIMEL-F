













#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_BLOCKCOUNTER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_BLOCKCOUNTER_H

#include "llvm/Support/Allocator.h"

namespace clang {

class StackFrameContext;

namespace ento {




class BlockCounter {
void *Data;

BlockCounter(void *D) : Data(D) {}

public:
BlockCounter() : Data(nullptr) {}

unsigned getNumVisited(const StackFrameContext *CallSite,
unsigned BlockID) const;

class Factory {
void *F;
public:
Factory(llvm::BumpPtrAllocator& Alloc);
~Factory();

BlockCounter GetEmptyCounter();
BlockCounter IncrementCount(BlockCounter BC,
const StackFrameContext *CallSite,
unsigned BlockID);
};

friend class Factory;
};

}

}

#endif
