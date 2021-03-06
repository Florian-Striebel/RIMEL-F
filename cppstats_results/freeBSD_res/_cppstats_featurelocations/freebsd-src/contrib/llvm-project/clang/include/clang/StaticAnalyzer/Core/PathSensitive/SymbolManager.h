












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SYMBOLMANAGER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SYMBOLMANAGER_H

#include "clang/AST/Expr.h"
#include "clang/AST/Type.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/MemRegion.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/StoreRef.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymExpr.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/Support/Allocator.h"
#include <cassert>

namespace clang {

class ASTContext;
class Stmt;

namespace ento {

class BasicValueFactory;
class StoreManager;


class SymbolRegionValue : public SymbolData {
const TypedValueRegion *R;

public:
SymbolRegionValue(SymbolID sym, const TypedValueRegion *r)
: SymbolData(SymbolRegionValueKind, sym), R(r) {
assert(r);
assert(isValidTypeForSymbol(r->getValueType()));
}

const TypedValueRegion* getRegion() const { return R; }

static void Profile(llvm::FoldingSetNodeID& profile, const TypedValueRegion* R) {
profile.AddInteger((unsigned) SymbolRegionValueKind);
profile.AddPointer(R);
}

void Profile(llvm::FoldingSetNodeID& profile) override {
Profile(profile, R);
}

StringRef getKindStr() const override;

void dumpToStream(raw_ostream &os) const override;
const MemRegion *getOriginRegion() const override { return getRegion(); }

QualType getType() const override;


static bool classof(const SymExpr *SE) {
return SE->getKind() == SymbolRegionValueKind;
}
};



class SymbolConjured : public SymbolData {
const Stmt *S;
QualType T;
unsigned Count;
const LocationContext *LCtx;
const void *SymbolTag;

public:
SymbolConjured(SymbolID sym, const Stmt *s, const LocationContext *lctx,
QualType t, unsigned count, const void *symbolTag)
: SymbolData(SymbolConjuredKind, sym), S(s), T(t), Count(count),
LCtx(lctx), SymbolTag(symbolTag) {





assert(lctx);
assert(isValidTypeForSymbol(t));
}

const Stmt *getStmt() const { return S; }
unsigned getCount() const { return Count; }
const void *getTag() const { return SymbolTag; }

QualType getType() const override;

StringRef getKindStr() const override;

void dumpToStream(raw_ostream &os) const override;

static void Profile(llvm::FoldingSetNodeID& profile, const Stmt *S,
QualType T, unsigned Count, const LocationContext *LCtx,
const void *SymbolTag) {
profile.AddInteger((unsigned) SymbolConjuredKind);
profile.AddPointer(S);
profile.AddPointer(LCtx);
profile.Add(T);
profile.AddInteger(Count);
profile.AddPointer(SymbolTag);
}

void Profile(llvm::FoldingSetNodeID& profile) override {
Profile(profile, S, T, Count, LCtx, SymbolTag);
}


static bool classof(const SymExpr *SE) {
return SE->getKind() == SymbolConjuredKind;
}
};



class SymbolDerived : public SymbolData {
SymbolRef parentSymbol;
const TypedValueRegion *R;

public:
SymbolDerived(SymbolID sym, SymbolRef parent, const TypedValueRegion *r)
: SymbolData(SymbolDerivedKind, sym), parentSymbol(parent), R(r) {
assert(parent);
assert(r);
assert(isValidTypeForSymbol(r->getValueType()));
}

SymbolRef getParentSymbol() const { return parentSymbol; }
const TypedValueRegion *getRegion() const { return R; }

QualType getType() const override;

StringRef getKindStr() const override;

void dumpToStream(raw_ostream &os) const override;
const MemRegion *getOriginRegion() const override { return getRegion(); }

static void Profile(llvm::FoldingSetNodeID& profile, SymbolRef parent,
const TypedValueRegion *r) {
profile.AddInteger((unsigned) SymbolDerivedKind);
profile.AddPointer(r);
profile.AddPointer(parent);
}

void Profile(llvm::FoldingSetNodeID& profile) override {
Profile(profile, parentSymbol, R);
}


static bool classof(const SymExpr *SE) {
return SE->getKind() == SymbolDerivedKind;
}
};




class SymbolExtent : public SymbolData {
const SubRegion *R;

public:
SymbolExtent(SymbolID sym, const SubRegion *r)
: SymbolData(SymbolExtentKind, sym), R(r) {
assert(r);
}

const SubRegion *getRegion() const { return R; }

QualType getType() const override;

StringRef getKindStr() const override;

void dumpToStream(raw_ostream &os) const override;

static void Profile(llvm::FoldingSetNodeID& profile, const SubRegion *R) {
profile.AddInteger((unsigned) SymbolExtentKind);
profile.AddPointer(R);
}

void Profile(llvm::FoldingSetNodeID& profile) override {
Profile(profile, R);
}


static bool classof(const SymExpr *SE) {
return SE->getKind() == SymbolExtentKind;
}
};





class SymbolMetadata : public SymbolData {
const MemRegion* R;
const Stmt *S;
QualType T;
const LocationContext *LCtx;
unsigned Count;
const void *Tag;

public:
SymbolMetadata(SymbolID sym, const MemRegion* r, const Stmt *s, QualType t,
const LocationContext *LCtx, unsigned count, const void *tag)
: SymbolData(SymbolMetadataKind, sym), R(r), S(s), T(t), LCtx(LCtx),
Count(count), Tag(tag) {
assert(r);
assert(s);
assert(isValidTypeForSymbol(t));
assert(LCtx);
assert(tag);
}

const MemRegion *getRegion() const { return R; }
const Stmt *getStmt() const { return S; }
const LocationContext *getLocationContext() const { return LCtx; }
unsigned getCount() const { return Count; }
const void *getTag() const { return Tag; }

QualType getType() const override;

StringRef getKindStr() const override;

void dumpToStream(raw_ostream &os) const override;

static void Profile(llvm::FoldingSetNodeID& profile, const MemRegion *R,
const Stmt *S, QualType T, const LocationContext *LCtx,
unsigned Count, const void *Tag) {
profile.AddInteger((unsigned) SymbolMetadataKind);
profile.AddPointer(R);
profile.AddPointer(S);
profile.Add(T);
profile.AddPointer(LCtx);
profile.AddInteger(Count);
profile.AddPointer(Tag);
}

void Profile(llvm::FoldingSetNodeID& profile) override {
Profile(profile, R, S, T, LCtx, Count, Tag);
}


static bool classof(const SymExpr *SE) {
return SE->getKind() == SymbolMetadataKind;
}
};


class SymbolCast : public SymExpr {
const SymExpr *Operand;


QualType FromTy;


QualType ToTy;

public:
SymbolCast(const SymExpr *In, QualType From, QualType To)
: SymExpr(SymbolCastKind), Operand(In), FromTy(From), ToTy(To) {
assert(In);
assert(isValidTypeForSymbol(From));


}

unsigned computeComplexity() const override {
if (Complexity == 0)
Complexity = 1 + Operand->computeComplexity();
return Complexity;
}

QualType getType() const override { return ToTy; }

const SymExpr *getOperand() const { return Operand; }

void dumpToStream(raw_ostream &os) const override;

static void Profile(llvm::FoldingSetNodeID& ID,
const SymExpr *In, QualType From, QualType To) {
ID.AddInteger((unsigned) SymbolCastKind);
ID.AddPointer(In);
ID.Add(From);
ID.Add(To);
}

void Profile(llvm::FoldingSetNodeID& ID) override {
Profile(ID, Operand, FromTy, ToTy);
}


static bool classof(const SymExpr *SE) {
return SE->getKind() == SymbolCastKind;
}
};


class BinarySymExpr : public SymExpr {
BinaryOperator::Opcode Op;
QualType T;

protected:
BinarySymExpr(Kind k, BinaryOperator::Opcode op, QualType t)
: SymExpr(k), Op(op), T(t) {
assert(classof(this));



assert(isValidTypeForSymbol(t) && !Loc::isLocType(t));
}

public:


QualType getType() const override { return T; }

BinaryOperator::Opcode getOpcode() const { return Op; }


static bool classof(const SymExpr *SE) {
Kind k = SE->getKind();
return k >= BEGIN_BINARYSYMEXPRS && k <= END_BINARYSYMEXPRS;
}

protected:
static unsigned computeOperandComplexity(const SymExpr *Value) {
return Value->computeComplexity();
}
static unsigned computeOperandComplexity(const llvm::APSInt &Value) {
return 1;
}

static const llvm::APSInt *getPointer(const llvm::APSInt &Value) {
return &Value;
}
static const SymExpr *getPointer(const SymExpr *Value) { return Value; }

static void dumpToStreamImpl(raw_ostream &os, const SymExpr *Value);
static void dumpToStreamImpl(raw_ostream &os, const llvm::APSInt &Value);
static void dumpToStreamImpl(raw_ostream &os, BinaryOperator::Opcode op);
};


template <class LHSTYPE, class RHSTYPE, SymExpr::Kind ClassKind>
class BinarySymExprImpl : public BinarySymExpr {
LHSTYPE LHS;
RHSTYPE RHS;

public:
BinarySymExprImpl(LHSTYPE lhs, BinaryOperator::Opcode op, RHSTYPE rhs,
QualType t)
: BinarySymExpr(ClassKind, op, t), LHS(lhs), RHS(rhs) {
assert(getPointer(lhs));
assert(getPointer(rhs));
}

void dumpToStream(raw_ostream &os) const override {
dumpToStreamImpl(os, LHS);
dumpToStreamImpl(os, getOpcode());
dumpToStreamImpl(os, RHS);
}

LHSTYPE getLHS() const { return LHS; }
RHSTYPE getRHS() const { return RHS; }

unsigned computeComplexity() const override {
if (Complexity == 0)
Complexity =
computeOperandComplexity(RHS) + computeOperandComplexity(LHS);
return Complexity;
}

static void Profile(llvm::FoldingSetNodeID &ID, LHSTYPE lhs,
BinaryOperator::Opcode op, RHSTYPE rhs, QualType t) {
ID.AddInteger((unsigned)ClassKind);
ID.AddPointer(getPointer(lhs));
ID.AddInteger(op);
ID.AddPointer(getPointer(rhs));
ID.Add(t);
}

void Profile(llvm::FoldingSetNodeID &ID) override {
Profile(ID, LHS, getOpcode(), RHS, getType());
}


static bool classof(const SymExpr *SE) { return SE->getKind() == ClassKind; }
};


using SymIntExpr = BinarySymExprImpl<const SymExpr *, const llvm::APSInt &,
SymExpr::Kind::SymIntExprKind>;


using IntSymExpr = BinarySymExprImpl<const llvm::APSInt &, const SymExpr *,
SymExpr::Kind::IntSymExprKind>;


using SymSymExpr = BinarySymExprImpl<const SymExpr *, const SymExpr *,
SymExpr::Kind::SymSymExprKind>;

class SymbolManager {
using DataSetTy = llvm::FoldingSet<SymExpr>;
using SymbolDependTy =
llvm::DenseMap<SymbolRef, std::unique_ptr<SymbolRefSmallVectorTy>>;

DataSetTy DataSet;



SymbolDependTy SymbolDependencies;

unsigned SymbolCounter = 0;
llvm::BumpPtrAllocator& BPAlloc;
BasicValueFactory &BV;
ASTContext &Ctx;

public:
SymbolManager(ASTContext &ctx, BasicValueFactory &bv,
llvm::BumpPtrAllocator& bpalloc)
: SymbolDependencies(16), BPAlloc(bpalloc), BV(bv), Ctx(ctx) {}

static bool canSymbolicate(QualType T);


const SymbolRegionValue* getRegionValueSymbol(const TypedValueRegion* R);

const SymbolConjured* conjureSymbol(const Stmt *E,
const LocationContext *LCtx,
QualType T,
unsigned VisitCount,
const void *SymbolTag = nullptr);

const SymbolConjured* conjureSymbol(const Expr *E,
const LocationContext *LCtx,
unsigned VisitCount,
const void *SymbolTag = nullptr) {
return conjureSymbol(E, LCtx, E->getType(), VisitCount, SymbolTag);
}

const SymbolDerived *getDerivedSymbol(SymbolRef parentSymbol,
const TypedValueRegion *R);

const SymbolExtent *getExtentSymbol(const SubRegion *R);





const SymbolMetadata *getMetadataSymbol(const MemRegion *R, const Stmt *S,
QualType T,
const LocationContext *LCtx,
unsigned VisitCount,
const void *SymbolTag = nullptr);

const SymbolCast* getCastSymbol(const SymExpr *Operand,
QualType From, QualType To);

const SymIntExpr *getSymIntExpr(const SymExpr *lhs, BinaryOperator::Opcode op,
const llvm::APSInt& rhs, QualType t);

const SymIntExpr *getSymIntExpr(const SymExpr &lhs, BinaryOperator::Opcode op,
const llvm::APSInt& rhs, QualType t) {
return getSymIntExpr(&lhs, op, rhs, t);
}

const IntSymExpr *getIntSymExpr(const llvm::APSInt& lhs,
BinaryOperator::Opcode op,
const SymExpr *rhs, QualType t);

const SymSymExpr *getSymSymExpr(const SymExpr *lhs, BinaryOperator::Opcode op,
const SymExpr *rhs, QualType t);

QualType getType(const SymExpr *SE) const {
return SE->getType();
}




void addSymbolDependency(const SymbolRef Primary, const SymbolRef Dependent);

const SymbolRefSmallVectorTy *getDependentSymbols(const SymbolRef Primary);

ASTContext &getContext() { return Ctx; }
BasicValueFactory &getBasicVals() { return BV; }
};


class SymbolReaper {
enum SymbolStatus {
NotProcessed,
HaveMarkedDependents
};

using SymbolSetTy = llvm::DenseSet<SymbolRef>;
using SymbolMapTy = llvm::DenseMap<SymbolRef, SymbolStatus>;
using RegionSetTy = llvm::DenseSet<const MemRegion *>;

SymbolMapTy TheLiving;
SymbolSetTy MetadataInUse;

RegionSetTy RegionRoots;

const StackFrameContext *LCtx;
const Stmt *Loc;
SymbolManager& SymMgr;
StoreRef reapedStore;
llvm::DenseMap<const MemRegion *, unsigned> includedRegionCache;

public:







SymbolReaper(const StackFrameContext *Ctx, const Stmt *s,
SymbolManager &symmgr, StoreManager &storeMgr)
: LCtx(Ctx), Loc(s), SymMgr(symmgr), reapedStore(nullptr, storeMgr) {}

const LocationContext *getLocationContext() const { return LCtx; }

bool isLive(SymbolRef sym);
bool isLiveRegion(const MemRegion *region);
bool isLive(const Expr *ExprVal, const LocationContext *LCtx) const;
bool isLive(const VarRegion *VR, bool includeStoreBindings = false) const;






void markLive(SymbolRef sym);








void markInUse(SymbolRef sym);

using region_iterator = RegionSetTy::const_iterator;

region_iterator region_begin() const { return RegionRoots.begin(); }
region_iterator region_end() const { return RegionRoots.end(); }





bool isDead(SymbolRef sym) {
return !isLive(sym);
}

void markLive(const MemRegion *region);
void markElementIndicesLive(const MemRegion *region);



void setReapedStore(StoreRef st) { reapedStore = st; }

private:

void markDependentsLive(SymbolRef sym);
};

class SymbolVisitor {
protected:
~SymbolVisitor() = default;

public:
SymbolVisitor() = default;
SymbolVisitor(const SymbolVisitor &) = default;
SymbolVisitor(SymbolVisitor &&) {}





virtual bool VisitSymbol(SymbolRef sym) = 0;
virtual bool VisitMemRegion(const MemRegion *) { return true; }
};

}

}

#endif
