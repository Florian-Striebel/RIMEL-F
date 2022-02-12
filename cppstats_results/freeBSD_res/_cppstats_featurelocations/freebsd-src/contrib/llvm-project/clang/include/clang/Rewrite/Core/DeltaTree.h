











#if !defined(LLVM_CLANG_REWRITE_CORE_DELTATREE_H)
#define LLVM_CLANG_REWRITE_CORE_DELTATREE_H

namespace clang {








class DeltaTree {
void *Root;

public:
DeltaTree();


DeltaTree(const DeltaTree &RHS);

DeltaTree &operator=(const DeltaTree &) = delete;
~DeltaTree();




int getDeltaAt(unsigned FileIndex) const;




void AddDelta(unsigned FileIndex, int Delta);
};

}

#endif
