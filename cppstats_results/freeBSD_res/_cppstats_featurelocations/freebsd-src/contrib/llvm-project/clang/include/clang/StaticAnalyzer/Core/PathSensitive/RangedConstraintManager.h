











#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CORE_RANGEDCONSTRAINTMANAGER_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CORE_RANGEDCONSTRAINTMANAGER_H

#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SimpleConstraintManager.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/Support/Allocator.h"

namespace clang {

namespace ento {




class Range {
public:
Range(const llvm::APSInt &From, const llvm::APSInt &To) : Impl(&From, &To) {
assert(From <= To);
}

Range(const llvm::APSInt &Point) : Range(Point, Point) {}

bool Includes(const llvm::APSInt &Point) const {
return From() <= Point && Point <= To();
}
const llvm::APSInt &From() const { return *Impl.first; }
const llvm::APSInt &To() const { return *Impl.second; }
const llvm::APSInt *getConcreteValue() const {
return &From() == &To() ? &From() : nullptr;
}

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.AddPointer(&From());
ID.AddPointer(&To());
}
void dump(raw_ostream &OS) const;



bool operator<(const Range &RHS) const { return From() < RHS.From(); }

bool operator==(const Range &RHS) const { return Impl == RHS.Impl; }
bool operator!=(const Range &RHS) const { return !operator==(RHS); }

private:
std::pair<const llvm::APSInt *, const llvm::APSInt *> Impl;
};








class RangeSet {
public:
class Factory;

private:



















using ImplType = llvm::SmallVector<Range, 4>;

struct ContainerType : public ImplType, public llvm::FoldingSetNode {
void Profile(llvm::FoldingSetNodeID &ID) const {
for (const Range &It : *this) {
It.Profile(ID);
}
}
};





using UnderlyingType = const ContainerType *;
UnderlyingType Impl;

public:
using const_iterator = ImplType::const_iterator;

const_iterator begin() const { return Impl->begin(); }
const_iterator end() const { return Impl->end(); }
size_t size() const { return Impl->size(); }

bool isEmpty() const { return Impl->empty(); }

class Factory {
public:
Factory(BasicValueFactory &BV) : ValueFactory(BV) {}






RangeSet add(RangeSet LHS, RangeSet RHS);





RangeSet add(RangeSet Original, Range Element);





RangeSet add(RangeSet Original, const llvm::APSInt &Point);

RangeSet getEmptySet() { return &EmptySet; }



RangeSet getRangeSet(Range Origin);
RangeSet getRangeSet(const llvm::APSInt &From, const llvm::APSInt &To) {
return getRangeSet(Range(From, To));
}
RangeSet getRangeSet(const llvm::APSInt &Origin) {
return getRangeSet(Origin, Origin);
}






RangeSet intersect(RangeSet LHS, RangeSet RHS);











RangeSet intersect(RangeSet What, llvm::APSInt Lower, llvm::APSInt Upper);







RangeSet intersect(RangeSet What, llvm::APSInt Point);





RangeSet deletePoint(RangeSet From, const llvm::APSInt &Point);



























RangeSet negate(RangeSet What);


BasicValueFactory &getValueFactory() const { return ValueFactory; }

private:

RangeSet makePersistent(ContainerType &&From);

ContainerType *construct(ContainerType &&From);

RangeSet intersect(const ContainerType &LHS, const ContainerType &RHS);



BasicValueFactory &ValueFactory;



llvm::SpecificBumpPtrAllocator<ContainerType> Arena;


llvm::FoldingSet<ContainerType> Cache;
static ContainerType EmptySet;
};

RangeSet(const RangeSet &) = default;
RangeSet &operator=(const RangeSet &) = default;
RangeSet(RangeSet &&) = default;
RangeSet &operator=(RangeSet &&) = default;
~RangeSet() = default;


RangeSet(Factory &F, const llvm::APSInt &From, const llvm::APSInt &To)
: RangeSet(F.getRangeSet(From, To)) {}


RangeSet(Factory &F, const llvm::APSInt &Point)
: RangeSet(F.getRangeSet(Point)) {}

static void Profile(llvm::FoldingSetNodeID &ID, const RangeSet &RS) {
ID.AddPointer(RS.Impl);
}



void Profile(llvm::FoldingSetNodeID &ID) const { Profile(ID, *this); }




const llvm::APSInt *getConcreteValue() const {
return Impl->size() == 1 ? begin()->getConcreteValue() : nullptr;
}




const llvm::APSInt &getMinValue() const;



const llvm::APSInt &getMaxValue() const;





bool contains(llvm::APSInt Point) const { return containsImpl(Point); }

void dump(raw_ostream &OS) const;

bool operator==(const RangeSet &Other) const { return *Impl == *Other.Impl; }
bool operator!=(const RangeSet &Other) const { return !(*this == Other); }

private:
RangeSet(ContainerType *RawContainer) : Impl(RawContainer) {}
RangeSet(UnderlyingType Ptr) : Impl(Ptr) {}











bool pin(llvm::APSInt &Lower, llvm::APSInt &Upper) const;
bool pin(llvm::APSInt &Point) const;



bool containsImpl(llvm::APSInt &Point) const;

friend class Factory;
};

using ConstraintMap = llvm::ImmutableMap<SymbolRef, RangeSet>;
ConstraintMap getConstraintMap(ProgramStateRef State);

class RangedConstraintManager : public SimpleConstraintManager {
public:
RangedConstraintManager(ExprEngine *EE, SValBuilder &SB)
: SimpleConstraintManager(EE, SB) {}

~RangedConstraintManager() override;





ProgramStateRef assumeSym(ProgramStateRef State, SymbolRef Sym,
bool Assumption) override;

ProgramStateRef assumeSymInclusiveRange(ProgramStateRef State, SymbolRef Sym,
const llvm::APSInt &From,
const llvm::APSInt &To,
bool InRange) override;

ProgramStateRef assumeSymUnsupported(ProgramStateRef State, SymbolRef Sym,
bool Assumption) override;

protected:

virtual ProgramStateRef assumeSymRel(ProgramStateRef State, SymbolRef Sym,
BinaryOperator::Opcode op,
const llvm::APSInt &Int);








virtual ProgramStateRef assumeSymNE(ProgramStateRef State, SymbolRef Sym,
const llvm::APSInt &V,
const llvm::APSInt &Adjustment) = 0;

virtual ProgramStateRef assumeSymEQ(ProgramStateRef State, SymbolRef Sym,
const llvm::APSInt &V,
const llvm::APSInt &Adjustment) = 0;

virtual ProgramStateRef assumeSymLT(ProgramStateRef State, SymbolRef Sym,
const llvm::APSInt &V,
const llvm::APSInt &Adjustment) = 0;

virtual ProgramStateRef assumeSymGT(ProgramStateRef State, SymbolRef Sym,
const llvm::APSInt &V,
const llvm::APSInt &Adjustment) = 0;

virtual ProgramStateRef assumeSymLE(ProgramStateRef State, SymbolRef Sym,
const llvm::APSInt &V,
const llvm::APSInt &Adjustment) = 0;

virtual ProgramStateRef assumeSymGE(ProgramStateRef State, SymbolRef Sym,
const llvm::APSInt &V,
const llvm::APSInt &Adjustment) = 0;

virtual ProgramStateRef assumeSymWithinInclusiveRange(
ProgramStateRef State, SymbolRef Sym, const llvm::APSInt &From,
const llvm::APSInt &To, const llvm::APSInt &Adjustment) = 0;

virtual ProgramStateRef assumeSymOutsideInclusiveRange(
ProgramStateRef State, SymbolRef Sym, const llvm::APSInt &From,
const llvm::APSInt &To, const llvm::APSInt &Adjustment) = 0;




private:
static void computeAdjustment(SymbolRef &Sym, llvm::APSInt &Adjustment);
};




SymbolRef simplify(ProgramStateRef State, SymbolRef Sym);

}
}

REGISTER_FACTORY_WITH_PROGRAMSTATE(ConstraintMap)

#endif
