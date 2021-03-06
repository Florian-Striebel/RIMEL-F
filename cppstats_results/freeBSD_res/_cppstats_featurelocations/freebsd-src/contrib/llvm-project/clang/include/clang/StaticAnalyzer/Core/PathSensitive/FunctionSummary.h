











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_FUNCTIONSUMMARY_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_FUNCTIONSUMMARY_H

#include "clang/AST/Decl.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallBitVector.h"
#include <cassert>
#include <deque>
#include <utility>

namespace clang {
namespace ento {

using SetOfDecls = std::deque<Decl *>;
using SetOfConstDecls = llvm::DenseSet<const Decl *>;

class FunctionSummariesTy {
class FunctionSummary {
public:

llvm::SmallBitVector VisitedBasicBlocks;


unsigned TotalBasicBlocks : 30;



unsigned InlineChecked : 1;


unsigned MayInline : 1;


unsigned TimesInlined : 32;

FunctionSummary()
: TotalBasicBlocks(0), InlineChecked(0), MayInline(0),
TimesInlined(0) {}
};

using MapTy = llvm::DenseMap<const Decl *, FunctionSummary>;
MapTy Map;

public:
MapTy::iterator findOrInsertSummary(const Decl *D) {
MapTy::iterator I = Map.find(D);
if (I != Map.end())
return I;

using KVPair = std::pair<const Decl *, FunctionSummary>;

I = Map.insert(KVPair(D, FunctionSummary())).first;
assert(I != Map.end());
return I;
}

void markMayInline(const Decl *D) {
MapTy::iterator I = findOrInsertSummary(D);
I->second.InlineChecked = 1;
I->second.MayInline = 1;
}

void markShouldNotInline(const Decl *D) {
MapTy::iterator I = findOrInsertSummary(D);
I->second.InlineChecked = 1;
I->second.MayInline = 0;
}

void markReachedMaxBlockCount(const Decl *D) {
markShouldNotInline(D);
}

Optional<bool> mayInline(const Decl *D) {
MapTy::const_iterator I = Map.find(D);
if (I != Map.end() && I->second.InlineChecked)
return I->second.MayInline;
return None;
}

void markVisitedBasicBlock(unsigned ID, const Decl* D, unsigned TotalIDs) {
MapTy::iterator I = findOrInsertSummary(D);
llvm::SmallBitVector &Blocks = I->second.VisitedBasicBlocks;
assert(ID < TotalIDs);
if (TotalIDs > Blocks.size()) {
Blocks.resize(TotalIDs);
I->second.TotalBasicBlocks = TotalIDs;
}
Blocks.set(ID);
}

unsigned getNumVisitedBasicBlocks(const Decl* D) {
MapTy::const_iterator I = Map.find(D);
if (I != Map.end())
return I->second.VisitedBasicBlocks.count();
return 0;
}

unsigned getNumTimesInlined(const Decl* D) {
MapTy::const_iterator I = Map.find(D);
if (I != Map.end())
return I->second.TimesInlined;
return 0;
}

void bumpNumTimesInlined(const Decl* D) {
MapTy::iterator I = findOrInsertSummary(D);
I->second.TimesInlined++;
}


unsigned getPercentBlocksReachable(const Decl *D) {
MapTy::const_iterator I = Map.find(D);
if (I != Map.end())
return ((I->second.VisitedBasicBlocks.count() * 100) /
I->second.TotalBasicBlocks);
return 0;
}

unsigned getTotalNumBasicBlocks();
unsigned getTotalNumVisitedBasicBlocks();
};

}
}

#endif
