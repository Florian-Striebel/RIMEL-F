































































#if !defined(LLVM_CLANG_STATICANALYZER_UNINITIALIZEDOBJECT_H)
#define LLVM_CLANG_STATICANALYZER_UNINITIALIZEDOBJECT_H

#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"

namespace clang {
namespace ento {

struct UninitObjCheckerOptions {
bool IsPedantic = false;
bool ShouldConvertNotesToWarnings = false;
bool CheckPointeeInitialization = false;
std::string IgnoredRecordsWithFieldPattern;
bool IgnoreGuardedFields = false;
};





class FieldNode {
protected:
const FieldRegion *FR;



~FieldNode() = default;

public:
FieldNode(const FieldRegion *FR) : FR(FR) {}



FieldNode() = delete;
FieldNode(const FieldNode &) = delete;
FieldNode(FieldNode &&) = delete;
FieldNode &operator=(const FieldNode &) = delete;
FieldNode &operator=(const FieldNode &&) = delete;

void Profile(llvm::FoldingSetNodeID &ID) const { ID.AddPointer(this); }


bool isSameRegion(const FieldRegion *OtherFR) const {



if (FR == nullptr)
return false;

return FR == OtherFR;
}

const FieldRegion *getRegion() const { return FR; }
const FieldDecl *getDecl() const {
assert(FR);
return FR->getDecl();
}











virtual void printNoteMsg(llvm::raw_ostream &Out) const = 0;


virtual void printPrefix(llvm::raw_ostream &Out) const = 0;


virtual void printNode(llvm::raw_ostream &Out) const = 0;



virtual void printSeparator(llvm::raw_ostream &Out) const = 0;

virtual bool isBase() const { return false; }
};



std::string getVariableName(const FieldDecl *Field);









class FieldChainInfo {
public:
using FieldChain = llvm::ImmutableList<const FieldNode &>;

private:
FieldChain::Factory &ChainFactory;
FieldChain Chain;

FieldChainInfo(FieldChain::Factory &F, FieldChain NewChain)
: FieldChainInfo(F) {
Chain = NewChain;
}

public:
FieldChainInfo() = delete;
FieldChainInfo(FieldChain::Factory &F) : ChainFactory(F) {}
FieldChainInfo(const FieldChainInfo &Other) = default;


template <class FieldNodeT> FieldChainInfo add(const FieldNodeT &FN);



template <class FieldNodeT> FieldChainInfo replaceHead(const FieldNodeT &FN);

bool contains(const FieldRegion *FR) const;
bool isEmpty() const { return Chain.isEmpty(); }

const FieldNode &getHead() const { return Chain.getHead(); }
const FieldRegion *getUninitRegion() const { return getHead().getRegion(); }

void printNoteMsg(llvm::raw_ostream &Out) const;
};

using UninitFieldMap = std::map<const FieldRegion *, llvm::SmallString<50>>;


class FindUninitializedFields {
ProgramStateRef State;
const TypedValueRegion *const ObjectR;

const UninitObjCheckerOptions Opts;
bool IsAnyFieldInitialized = false;

FieldChainInfo::FieldChain::Factory ChainFactory;











UninitFieldMap UninitFields;

public:


FindUninitializedFields(ProgramStateRef State,
const TypedValueRegion *const R,
const UninitObjCheckerOptions &Opts);



std::pair<ProgramStateRef, const UninitFieldMap &> getResults() {
return {State, UninitFields};
}





bool isAnyFieldInitialized() { return IsAnyFieldInitialized; }

private:




















































bool isUnionUninit(const TypedValueRegion *R);



bool isNonUnionUninit(const TypedValueRegion *R, FieldChainInfo LocalChain);




bool isDereferencableUninit(const FieldRegion *FR, FieldChainInfo LocalChain);


bool isPrimitiveUninit(const SVal &V);















bool addFieldToUninits(FieldChainInfo LocalChain,
const MemRegion *PointeeR = nullptr);
};



inline bool isPrimitiveType(const QualType &T) {
return T->isBuiltinType() || T->isEnumeralType() ||
T->isFunctionType() || T->isAtomicType() ||
T->isVectorType() || T->isScalarType();
}

inline bool isDereferencableType(const QualType &T) {
return T->isAnyPointerType() || T->isReferenceType();
}



template <class FieldNodeT>
inline FieldChainInfo FieldChainInfo::add(const FieldNodeT &FN) {
assert(!contains(FN.getRegion()) &&
"Can't add a field that is already a part of the "
"fieldchain! Is this a cyclic reference?");

FieldChainInfo NewChain = *this;
NewChain.Chain = ChainFactory.add(FN, Chain);
return NewChain;
}

template <class FieldNodeT>
inline FieldChainInfo FieldChainInfo::replaceHead(const FieldNodeT &FN) {
FieldChainInfo NewChain(ChainFactory, Chain.getTail());
return NewChain.add(FN);
}

}
}

#endif
