












#if !defined(LLVM_CLANG_ANALYSIS_CFG_H)
#define LLVM_CLANG_ANALYSIS_CFG_H

#include "clang/Analysis/Support/BumpVector.h"
#include "clang/Analysis/ConstructionContext.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/raw_ostream.h"
#include <bitset>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <vector>

namespace clang {

class ASTContext;
class BinaryOperator;
class CFG;
class CXXBaseSpecifier;
class CXXBindTemporaryExpr;
class CXXCtorInitializer;
class CXXDeleteExpr;
class CXXDestructorDecl;
class CXXNewExpr;
class CXXRecordDecl;
class Decl;
class FieldDecl;
class LangOptions;
class VarDecl;


class CFGElement {
public:
enum Kind {

Initializer,
ScopeBegin,
ScopeEnd,
NewAllocator,
LifetimeEnds,
LoopExit,

Statement,
Constructor,
CXXRecordTypedCall,
STMT_BEGIN = Statement,
STMT_END = CXXRecordTypedCall,

AutomaticObjectDtor,
DeleteDtor,
BaseDtor,
MemberDtor,
TemporaryDtor,
DTOR_BEGIN = AutomaticObjectDtor,
DTOR_END = TemporaryDtor
};

protected:

llvm::PointerIntPair<void *, 2> Data1;
llvm::PointerIntPair<void *, 2> Data2;

CFGElement(Kind kind, const void *Ptr1, const void *Ptr2 = nullptr)
: Data1(const_cast<void*>(Ptr1), ((unsigned) kind) & 0x3),
Data2(const_cast<void*>(Ptr2), (((unsigned) kind) >> 2) & 0x3) {
assert(getKind() == kind);
}

CFGElement() = default;

public:


template<typename T>
T castAs() const {
assert(T::isKind(*this));
T t;
CFGElement& e = t;
e = *this;
return t;
}



template<typename T>
Optional<T> getAs() const {
if (!T::isKind(*this))
return None;
T t;
CFGElement& e = t;
e = *this;
return t;
}

Kind getKind() const {
unsigned x = Data2.getInt();
x <<= 2;
x |= Data1.getInt();
return (Kind) x;
}

void dumpToStream(llvm::raw_ostream &OS) const;

void dump() const {
dumpToStream(llvm::errs());
}
};

class CFGStmt : public CFGElement {
public:
explicit CFGStmt(Stmt *S, Kind K = Statement) : CFGElement(K, S) {
assert(isKind(*this));
}

const Stmt *getStmt() const {
return static_cast<const Stmt *>(Data1.getPointer());
}

private:
friend class CFGElement;

static bool isKind(const CFGElement &E) {
return E.getKind() >= STMT_BEGIN && E.getKind() <= STMT_END;
}

protected:
CFGStmt() = default;
};




class CFGConstructor : public CFGStmt {
public:
explicit CFGConstructor(CXXConstructExpr *CE, const ConstructionContext *C)
: CFGStmt(CE, Constructor) {
assert(C);
Data2.setPointer(const_cast<ConstructionContext *>(C));
}

const ConstructionContext *getConstructionContext() const {
return static_cast<ConstructionContext *>(Data2.getPointer());
}

private:
friend class CFGElement;

CFGConstructor() = default;

static bool isKind(const CFGElement &E) {
return E.getKind() == Constructor;
}
};







class CFGCXXRecordTypedCall : public CFGStmt {
public:


static bool isCXXRecordTypedCall(Expr *E) {
assert(isa<CallExpr>(E) || isa<ObjCMessageExpr>(E));



return !E->isGLValue() &&
E->getType().getCanonicalType()->getAsCXXRecordDecl();
}

explicit CFGCXXRecordTypedCall(Expr *E, const ConstructionContext *C)
: CFGStmt(E, CXXRecordTypedCall) {
assert(isCXXRecordTypedCall(E));
assert(C && (isa<TemporaryObjectConstructionContext>(C) ||

isa<ReturnedValueConstructionContext>(C) ||
isa<VariableConstructionContext>(C) ||
isa<ConstructorInitializerConstructionContext>(C) ||
isa<ArgumentConstructionContext>(C)));
Data2.setPointer(const_cast<ConstructionContext *>(C));
}

const ConstructionContext *getConstructionContext() const {
return static_cast<ConstructionContext *>(Data2.getPointer());
}

private:
friend class CFGElement;

CFGCXXRecordTypedCall() = default;

static bool isKind(const CFGElement &E) {
return E.getKind() == CXXRecordTypedCall;
}
};



class CFGInitializer : public CFGElement {
public:
explicit CFGInitializer(CXXCtorInitializer *initializer)
: CFGElement(Initializer, initializer) {}

CXXCtorInitializer* getInitializer() const {
return static_cast<CXXCtorInitializer*>(Data1.getPointer());
}

private:
friend class CFGElement;

CFGInitializer() = default;

static bool isKind(const CFGElement &E) {
return E.getKind() == Initializer;
}
};


class CFGNewAllocator : public CFGElement {
public:
explicit CFGNewAllocator(const CXXNewExpr *S)
: CFGElement(NewAllocator, S) {}


const CXXNewExpr *getAllocatorExpr() const {
return static_cast<CXXNewExpr *>(Data1.getPointer());
}

private:
friend class CFGElement;

CFGNewAllocator() = default;

static bool isKind(const CFGElement &elem) {
return elem.getKind() == NewAllocator;
}
};







class CFGLoopExit : public CFGElement {
public:
explicit CFGLoopExit(const Stmt *stmt) : CFGElement(LoopExit, stmt) {}

const Stmt *getLoopStmt() const {
return static_cast<Stmt *>(Data1.getPointer());
}

private:
friend class CFGElement;

CFGLoopExit() = default;

static bool isKind(const CFGElement &elem) {
return elem.getKind() == LoopExit;
}
};


class CFGLifetimeEnds : public CFGElement {
public:
explicit CFGLifetimeEnds(const VarDecl *var, const Stmt *stmt)
: CFGElement(LifetimeEnds, var, stmt) {}

const VarDecl *getVarDecl() const {
return static_cast<VarDecl *>(Data1.getPointer());
}

const Stmt *getTriggerStmt() const {
return static_cast<Stmt *>(Data2.getPointer());
}

private:
friend class CFGElement;

CFGLifetimeEnds() = default;

static bool isKind(const CFGElement &elem) {
return elem.getKind() == LifetimeEnds;
}
};



class CFGScopeBegin : public CFGElement {
public:
CFGScopeBegin() {}
CFGScopeBegin(const VarDecl *VD, const Stmt *S)
: CFGElement(ScopeBegin, VD, S) {}


const Stmt *getTriggerStmt() const {
return static_cast<Stmt*>(Data2.getPointer());
}


const VarDecl *getVarDecl() const {
return static_cast<VarDecl *>(Data1.getPointer());
}

private:
friend class CFGElement;
static bool isKind(const CFGElement &E) {
Kind kind = E.getKind();
return kind == ScopeBegin;
}
};



class CFGScopeEnd : public CFGElement {
public:
CFGScopeEnd() {}
CFGScopeEnd(const VarDecl *VD, const Stmt *S) : CFGElement(ScopeEnd, VD, S) {}

const VarDecl *getVarDecl() const {
return static_cast<VarDecl *>(Data1.getPointer());
}

const Stmt *getTriggerStmt() const {
return static_cast<Stmt *>(Data2.getPointer());
}

private:
friend class CFGElement;
static bool isKind(const CFGElement &E) {
Kind kind = E.getKind();
return kind == ScopeEnd;
}
};



class CFGImplicitDtor : public CFGElement {
protected:
CFGImplicitDtor() = default;

CFGImplicitDtor(Kind kind, const void *data1, const void *data2 = nullptr)
: CFGElement(kind, data1, data2) {
assert(kind >= DTOR_BEGIN && kind <= DTOR_END);
}

public:
const CXXDestructorDecl *getDestructorDecl(ASTContext &astContext) const;
bool isNoReturn(ASTContext &astContext) const;

private:
friend class CFGElement;

static bool isKind(const CFGElement &E) {
Kind kind = E.getKind();
return kind >= DTOR_BEGIN && kind <= DTOR_END;
}
};




class CFGAutomaticObjDtor: public CFGImplicitDtor {
public:
CFGAutomaticObjDtor(const VarDecl *var, const Stmt *stmt)
: CFGImplicitDtor(AutomaticObjectDtor, var, stmt) {}

const VarDecl *getVarDecl() const {
return static_cast<VarDecl*>(Data1.getPointer());
}


const Stmt *getTriggerStmt() const {
return static_cast<Stmt*>(Data2.getPointer());
}

private:
friend class CFGElement;

CFGAutomaticObjDtor() = default;

static bool isKind(const CFGElement &elem) {
return elem.getKind() == AutomaticObjectDtor;
}
};


class CFGDeleteDtor : public CFGImplicitDtor {
public:
CFGDeleteDtor(const CXXRecordDecl *RD, const CXXDeleteExpr *DE)
: CFGImplicitDtor(DeleteDtor, RD, DE) {}

const CXXRecordDecl *getCXXRecordDecl() const {
return static_cast<CXXRecordDecl*>(Data1.getPointer());
}


const CXXDeleteExpr *getDeleteExpr() const {
return static_cast<CXXDeleteExpr *>(Data2.getPointer());
}

private:
friend class CFGElement;

CFGDeleteDtor() = default;

static bool isKind(const CFGElement &elem) {
return elem.getKind() == DeleteDtor;
}
};



class CFGBaseDtor : public CFGImplicitDtor {
public:
CFGBaseDtor(const CXXBaseSpecifier *base)
: CFGImplicitDtor(BaseDtor, base) {}

const CXXBaseSpecifier *getBaseSpecifier() const {
return static_cast<const CXXBaseSpecifier*>(Data1.getPointer());
}

private:
friend class CFGElement;

CFGBaseDtor() = default;

static bool isKind(const CFGElement &E) {
return E.getKind() == BaseDtor;
}
};



class CFGMemberDtor : public CFGImplicitDtor {
public:
CFGMemberDtor(const FieldDecl *field)
: CFGImplicitDtor(MemberDtor, field, nullptr) {}

const FieldDecl *getFieldDecl() const {
return static_cast<const FieldDecl*>(Data1.getPointer());
}

private:
friend class CFGElement;

CFGMemberDtor() = default;

static bool isKind(const CFGElement &E) {
return E.getKind() == MemberDtor;
}
};



class CFGTemporaryDtor : public CFGImplicitDtor {
public:
CFGTemporaryDtor(CXXBindTemporaryExpr *expr)
: CFGImplicitDtor(TemporaryDtor, expr, nullptr) {}

const CXXBindTemporaryExpr *getBindTemporaryExpr() const {
return static_cast<const CXXBindTemporaryExpr *>(Data1.getPointer());
}

private:
friend class CFGElement;

CFGTemporaryDtor() = default;

static bool isKind(const CFGElement &E) {
return E.getKind() == TemporaryDtor;
}
};



class CFGTerminator {
public:
enum Kind {


StmtBranch,



TemporaryDtorsBranch,



VirtualBaseBranch,




NumKindsMinusOne = VirtualBaseBranch
};

private:
static constexpr int KindBits = 2;
static_assert((1 << KindBits) > NumKindsMinusOne,
"Not enough room for kind!");
llvm::PointerIntPair<Stmt *, KindBits> Data;

public:
CFGTerminator() { assert(!isValid()); }
CFGTerminator(Stmt *S, Kind K = StmtBranch) : Data(S, K) {}

bool isValid() const { return Data.getOpaqueValue() != nullptr; }
Stmt *getStmt() { return Data.getPointer(); }
const Stmt *getStmt() const { return Data.getPointer(); }
Kind getKind() const { return static_cast<Kind>(Data.getInt()); }

bool isStmtBranch() const {
return getKind() == StmtBranch;
}
bool isTemporaryDtorsBranch() const {
return getKind() == TemporaryDtorsBranch;
}
bool isVirtualBaseBranch() const {
return getKind() == VirtualBaseBranch;
}
};



























class CFGBlock {
class ElementList {
using ImplTy = BumpVector<CFGElement>;

ImplTy Impl;

public:
ElementList(BumpVectorContext &C) : Impl(C, 4) {}

using iterator = std::reverse_iterator<ImplTy::iterator>;
using const_iterator = std::reverse_iterator<ImplTy::const_iterator>;
using reverse_iterator = ImplTy::iterator;
using const_reverse_iterator = ImplTy::const_iterator;
using const_reference = ImplTy::const_reference;

void push_back(CFGElement e, BumpVectorContext &C) { Impl.push_back(e, C); }

reverse_iterator insert(reverse_iterator I, size_t Cnt, CFGElement E,
BumpVectorContext &C) {
return Impl.insert(I, Cnt, E, C);
}

const_reference front() const { return Impl.back(); }
const_reference back() const { return Impl.front(); }

iterator begin() { return Impl.rbegin(); }
iterator end() { return Impl.rend(); }
const_iterator begin() const { return Impl.rbegin(); }
const_iterator end() const { return Impl.rend(); }
reverse_iterator rbegin() { return Impl.begin(); }
reverse_iterator rend() { return Impl.end(); }
const_reverse_iterator rbegin() const { return Impl.begin(); }
const_reverse_iterator rend() const { return Impl.end(); }

CFGElement operator[](size_t i) const {
assert(i < Impl.size());
return Impl[Impl.size() - 1 - i];
}

size_t size() const { return Impl.size(); }
bool empty() const { return Impl.empty(); }
};




template <bool IsConst> class ElementRefImpl {

template <bool IsOtherConst> friend class ElementRefImpl;

using CFGBlockPtr =
std::conditional_t<IsConst, const CFGBlock *, CFGBlock *>;

using CFGElementPtr =
std::conditional_t<IsConst, const CFGElement *, CFGElement *>;

protected:
CFGBlockPtr Parent;
size_t Index;

public:
ElementRefImpl(CFGBlockPtr Parent, size_t Index)
: Parent(Parent), Index(Index) {}

template <bool IsOtherConst>
ElementRefImpl(ElementRefImpl<IsOtherConst> Other)
: ElementRefImpl(Other.Parent, Other.Index) {}

size_t getIndexInBlock() const { return Index; }

CFGBlockPtr getParent() { return Parent; }
CFGBlockPtr getParent() const { return Parent; }

bool operator<(ElementRefImpl Other) const {
return std::make_pair(Parent, Index) <
std::make_pair(Other.Parent, Other.Index);
}

bool operator==(ElementRefImpl Other) const {
return Parent == Other.Parent && Index == Other.Index;
}

bool operator!=(ElementRefImpl Other) const { return !(*this == Other); }
CFGElement operator*() const { return (*Parent)[Index]; }
CFGElementPtr operator->() const { return &*(Parent->begin() + Index); }

void dumpToStream(llvm::raw_ostream &OS) const {
OS << getIndexInBlock() + 1 << ": ";
(*this)->dumpToStream(OS);
}

void dump() const {
dumpToStream(llvm::errs());
}
};

template <bool IsReverse, bool IsConst> class ElementRefIterator {

template <bool IsOtherReverse, bool IsOtherConst>
friend class ElementRefIterator;

using CFGBlockRef =
std::conditional_t<IsConst, const CFGBlock *, CFGBlock *>;

using UnderlayingIteratorTy = std::conditional_t<
IsConst,
std::conditional_t<IsReverse, ElementList::const_reverse_iterator,
ElementList::const_iterator>,
std::conditional_t<IsReverse, ElementList::reverse_iterator,
ElementList::iterator>>;

using IteratorTraits = typename std::iterator_traits<UnderlayingIteratorTy>;
using ElementRef = typename CFGBlock::ElementRefImpl<IsConst>;

public:
using difference_type = typename IteratorTraits::difference_type;
using value_type = ElementRef;
using pointer = ElementRef *;
using iterator_category = typename IteratorTraits::iterator_category;

private:
CFGBlockRef Parent;
UnderlayingIteratorTy Pos;

public:
ElementRefIterator(CFGBlockRef Parent, UnderlayingIteratorTy Pos)
: Parent(Parent), Pos(Pos) {}

template <bool IsOtherConst>
ElementRefIterator(ElementRefIterator<false, IsOtherConst> E)
: ElementRefIterator(E.Parent, E.Pos.base()) {}

template <bool IsOtherConst>
ElementRefIterator(ElementRefIterator<true, IsOtherConst> E)
: ElementRefIterator(E.Parent, llvm::make_reverse_iterator(E.Pos)) {}

bool operator<(ElementRefIterator Other) const {
assert(Parent == Other.Parent);
return Pos < Other.Pos;
}

bool operator==(ElementRefIterator Other) const {
return Parent == Other.Parent && Pos == Other.Pos;
}

bool operator!=(ElementRefIterator Other) const {
return !(*this == Other);
}

private:
template <bool IsOtherConst>
static size_t
getIndexInBlock(CFGBlock::ElementRefIterator<true, IsOtherConst> E) {
return E.Parent->size() - (E.Pos - E.Parent->rbegin()) - 1;
}

template <bool IsOtherConst>
static size_t
getIndexInBlock(CFGBlock::ElementRefIterator<false, IsOtherConst> E) {
return E.Pos - E.Parent->begin();
}

public:
value_type operator*() { return {Parent, getIndexInBlock(*this)}; }

difference_type operator-(ElementRefIterator Other) const {
return Pos - Other.Pos;
}

ElementRefIterator operator++() {
++this->Pos;
return *this;
}
ElementRefIterator operator++(int) {
ElementRefIterator Ret = *this;
++*this;
return Ret;
}
ElementRefIterator operator+(size_t count) {
this->Pos += count;
return *this;
}
ElementRefIterator operator-(size_t count) {
this->Pos -= count;
return *this;
}
};

public:

ElementList Elements;




Stmt *Label = nullptr;



CFGTerminator Terminator;




const Stmt *LoopTarget = nullptr;


unsigned BlockID;

public:





class AdjacentBlock {
enum Kind {
AB_Normal,
AB_Unreachable,
AB_Alternate
};

CFGBlock *ReachableBlock;
llvm::PointerIntPair<CFGBlock *, 2> UnreachableBlock;

public:

AdjacentBlock(CFGBlock *B, bool IsReachable);



AdjacentBlock(CFGBlock *B, CFGBlock *AlternateBlock);


CFGBlock *getReachableBlock() const {
return ReachableBlock;
}


CFGBlock *getPossiblyUnreachableBlock() const {
return UnreachableBlock.getPointer();
}



operator CFGBlock*() const {
return getReachableBlock();
}

CFGBlock& operator *() const {
return *getReachableBlock();
}

CFGBlock* operator ->() const {
return getReachableBlock();
}

bool isReachable() const {
Kind K = (Kind) UnreachableBlock.getInt();
return K == AB_Normal || K == AB_Alternate;
}
};

private:

using AdjacentBlocks = BumpVector<AdjacentBlock>;
AdjacentBlocks Preds;
AdjacentBlocks Succs;










unsigned HasNoReturnElement : 1;


CFG *Parent;

public:
explicit CFGBlock(unsigned blockid, BumpVectorContext &C, CFG *parent)
: Elements(C), Terminator(nullptr), BlockID(blockid), Preds(C, 1),
Succs(C, 1), HasNoReturnElement(false), Parent(parent) {}


using iterator = ElementList::iterator;
using const_iterator = ElementList::const_iterator;
using reverse_iterator = ElementList::reverse_iterator;
using const_reverse_iterator = ElementList::const_reverse_iterator;

size_t getIndexInCFG() const;

CFGElement front() const { return Elements.front(); }
CFGElement back() const { return Elements.back(); }

iterator begin() { return Elements.begin(); }
iterator end() { return Elements.end(); }
const_iterator begin() const { return Elements.begin(); }
const_iterator end() const { return Elements.end(); }

reverse_iterator rbegin() { return Elements.rbegin(); }
reverse_iterator rend() { return Elements.rend(); }
const_reverse_iterator rbegin() const { return Elements.rbegin(); }
const_reverse_iterator rend() const { return Elements.rend(); }

using CFGElementRef = ElementRefImpl<false>;
using ConstCFGElementRef = ElementRefImpl<true>;

using ref_iterator = ElementRefIterator<false, false>;
using ref_iterator_range = llvm::iterator_range<ref_iterator>;
using const_ref_iterator = ElementRefIterator<false, true>;
using const_ref_iterator_range = llvm::iterator_range<const_ref_iterator>;

using reverse_ref_iterator = ElementRefIterator<true, false>;
using reverse_ref_iterator_range = llvm::iterator_range<reverse_ref_iterator>;

using const_reverse_ref_iterator = ElementRefIterator<true, true>;
using const_reverse_ref_iterator_range =
llvm::iterator_range<const_reverse_ref_iterator>;

ref_iterator ref_begin() { return {this, begin()}; }
ref_iterator ref_end() { return {this, end()}; }
const_ref_iterator ref_begin() const { return {this, begin()}; }
const_ref_iterator ref_end() const { return {this, end()}; }

reverse_ref_iterator rref_begin() { return {this, rbegin()}; }
reverse_ref_iterator rref_end() { return {this, rend()}; }
const_reverse_ref_iterator rref_begin() const { return {this, rbegin()}; }
const_reverse_ref_iterator rref_end() const { return {this, rend()}; }

ref_iterator_range refs() { return {ref_begin(), ref_end()}; }
const_ref_iterator_range refs() const { return {ref_begin(), ref_end()}; }
reverse_ref_iterator_range rrefs() { return {rref_begin(), rref_end()}; }
const_reverse_ref_iterator_range rrefs() const {
return {rref_begin(), rref_end()};
}

unsigned size() const { return Elements.size(); }
bool empty() const { return Elements.empty(); }

CFGElement operator[](size_t i) const { return Elements[i]; }


using pred_iterator = AdjacentBlocks::iterator;
using const_pred_iterator = AdjacentBlocks::const_iterator;
using pred_reverse_iterator = AdjacentBlocks::reverse_iterator;
using const_pred_reverse_iterator = AdjacentBlocks::const_reverse_iterator;
using pred_range = llvm::iterator_range<pred_iterator>;
using pred_const_range = llvm::iterator_range<const_pred_iterator>;

using succ_iterator = AdjacentBlocks::iterator;
using const_succ_iterator = AdjacentBlocks::const_iterator;
using succ_reverse_iterator = AdjacentBlocks::reverse_iterator;
using const_succ_reverse_iterator = AdjacentBlocks::const_reverse_iterator;
using succ_range = llvm::iterator_range<succ_iterator>;
using succ_const_range = llvm::iterator_range<const_succ_iterator>;

pred_iterator pred_begin() { return Preds.begin(); }
pred_iterator pred_end() { return Preds.end(); }
const_pred_iterator pred_begin() const { return Preds.begin(); }
const_pred_iterator pred_end() const { return Preds.end(); }

pred_reverse_iterator pred_rbegin() { return Preds.rbegin(); }
pred_reverse_iterator pred_rend() { return Preds.rend(); }
const_pred_reverse_iterator pred_rbegin() const { return Preds.rbegin(); }
const_pred_reverse_iterator pred_rend() const { return Preds.rend(); }

pred_range preds() {
return pred_range(pred_begin(), pred_end());
}

pred_const_range preds() const {
return pred_const_range(pred_begin(), pred_end());
}

succ_iterator succ_begin() { return Succs.begin(); }
succ_iterator succ_end() { return Succs.end(); }
const_succ_iterator succ_begin() const { return Succs.begin(); }
const_succ_iterator succ_end() const { return Succs.end(); }

succ_reverse_iterator succ_rbegin() { return Succs.rbegin(); }
succ_reverse_iterator succ_rend() { return Succs.rend(); }
const_succ_reverse_iterator succ_rbegin() const { return Succs.rbegin(); }
const_succ_reverse_iterator succ_rend() const { return Succs.rend(); }

succ_range succs() {
return succ_range(succ_begin(), succ_end());
}

succ_const_range succs() const {
return succ_const_range(succ_begin(), succ_end());
}

unsigned succ_size() const { return Succs.size(); }
bool succ_empty() const { return Succs.empty(); }

unsigned pred_size() const { return Preds.size(); }
bool pred_empty() const { return Preds.empty(); }


class FilterOptions {
public:
unsigned IgnoreNullPredecessors : 1;
unsigned IgnoreDefaultsWithCoveredEnums : 1;

FilterOptions()
: IgnoreNullPredecessors(1), IgnoreDefaultsWithCoveredEnums(0) {}
};

static bool FilterEdge(const FilterOptions &F, const CFGBlock *Src,
const CFGBlock *Dst);

template <typename IMPL, bool IsPred>
class FilteredCFGBlockIterator {
private:
IMPL I, E;
const FilterOptions F;
const CFGBlock *From;

public:
explicit FilteredCFGBlockIterator(const IMPL &i, const IMPL &e,
const CFGBlock *from,
const FilterOptions &f)
: I(i), E(e), F(f), From(from) {
while (hasMore() && Filter(*I))
++I;
}

bool hasMore() const { return I != E; }

FilteredCFGBlockIterator &operator++() {
do { ++I; } while (hasMore() && Filter(*I));
return *this;
}

const CFGBlock *operator*() const { return *I; }

private:
bool Filter(const CFGBlock *To) {
return IsPred ? FilterEdge(F, To, From) : FilterEdge(F, From, To);
}
};

using filtered_pred_iterator =
FilteredCFGBlockIterator<const_pred_iterator, true>;

using filtered_succ_iterator =
FilteredCFGBlockIterator<const_succ_iterator, false>;

filtered_pred_iterator filtered_pred_start_end(const FilterOptions &f) const {
return filtered_pred_iterator(pred_begin(), pred_end(), this, f);
}

filtered_succ_iterator filtered_succ_start_end(const FilterOptions &f) const {
return filtered_succ_iterator(succ_begin(), succ_end(), this, f);
}



void setTerminator(CFGTerminator Term) { Terminator = Term; }
void setLabel(Stmt *Statement) { Label = Statement; }
void setLoopTarget(const Stmt *loopTarget) { LoopTarget = loopTarget; }
void setHasNoReturnElement() { HasNoReturnElement = true; }



bool isInevitablySinking() const;

CFGTerminator getTerminator() const { return Terminator; }

Stmt *getTerminatorStmt() { return Terminator.getStmt(); }
const Stmt *getTerminatorStmt() const { return Terminator.getStmt(); }







const Expr *getLastCondition() const;

Stmt *getTerminatorCondition(bool StripParens = true);

const Stmt *getTerminatorCondition(bool StripParens = true) const {
return const_cast<CFGBlock*>(this)->getTerminatorCondition(StripParens);
}

const Stmt *getLoopTarget() const { return LoopTarget; }

Stmt *getLabel() { return Label; }
const Stmt *getLabel() const { return Label; }

bool hasNoReturnElement() const { return HasNoReturnElement; }

unsigned getBlockID() const { return BlockID; }

CFG *getParent() const { return Parent; }

void dump() const;

void dump(const CFG *cfg, const LangOptions &LO, bool ShowColors = false) const;
void print(raw_ostream &OS, const CFG* cfg, const LangOptions &LO,
bool ShowColors) const;

void printTerminator(raw_ostream &OS, const LangOptions &LO) const;
void printTerminatorJson(raw_ostream &Out, const LangOptions &LO,
bool AddQuotes) const;

void printAsOperand(raw_ostream &OS, bool ) {
OS << "BB#" << getBlockID();
}


void addSuccessor(AdjacentBlock Succ, BumpVectorContext &C);

void appendStmt(Stmt *statement, BumpVectorContext &C) {
Elements.push_back(CFGStmt(statement), C);
}

void appendConstructor(CXXConstructExpr *CE, const ConstructionContext *CC,
BumpVectorContext &C) {
Elements.push_back(CFGConstructor(CE, CC), C);
}

void appendCXXRecordTypedCall(Expr *E,
const ConstructionContext *CC,
BumpVectorContext &C) {
Elements.push_back(CFGCXXRecordTypedCall(E, CC), C);
}

void appendInitializer(CXXCtorInitializer *initializer,
BumpVectorContext &C) {
Elements.push_back(CFGInitializer(initializer), C);
}

void appendNewAllocator(CXXNewExpr *NE,
BumpVectorContext &C) {
Elements.push_back(CFGNewAllocator(NE), C);
}

void appendScopeBegin(const VarDecl *VD, const Stmt *S,
BumpVectorContext &C) {
Elements.push_back(CFGScopeBegin(VD, S), C);
}

void prependScopeBegin(const VarDecl *VD, const Stmt *S,
BumpVectorContext &C) {
Elements.insert(Elements.rbegin(), 1, CFGScopeBegin(VD, S), C);
}

void appendScopeEnd(const VarDecl *VD, const Stmt *S, BumpVectorContext &C) {
Elements.push_back(CFGScopeEnd(VD, S), C);
}

void prependScopeEnd(const VarDecl *VD, const Stmt *S, BumpVectorContext &C) {
Elements.insert(Elements.rbegin(), 1, CFGScopeEnd(VD, S), C);
}

void appendBaseDtor(const CXXBaseSpecifier *BS, BumpVectorContext &C) {
Elements.push_back(CFGBaseDtor(BS), C);
}

void appendMemberDtor(FieldDecl *FD, BumpVectorContext &C) {
Elements.push_back(CFGMemberDtor(FD), C);
}

void appendTemporaryDtor(CXXBindTemporaryExpr *E, BumpVectorContext &C) {
Elements.push_back(CFGTemporaryDtor(E), C);
}

void appendAutomaticObjDtor(VarDecl *VD, Stmt *S, BumpVectorContext &C) {
Elements.push_back(CFGAutomaticObjDtor(VD, S), C);
}

void appendLifetimeEnds(VarDecl *VD, Stmt *S, BumpVectorContext &C) {
Elements.push_back(CFGLifetimeEnds(VD, S), C);
}

void appendLoopExit(const Stmt *LoopStmt, BumpVectorContext &C) {
Elements.push_back(CFGLoopExit(LoopStmt), C);
}

void appendDeleteDtor(CXXRecordDecl *RD, CXXDeleteExpr *DE, BumpVectorContext &C) {
Elements.push_back(CFGDeleteDtor(RD, DE), C);
}




iterator beginAutomaticObjDtorsInsert(iterator I, size_t Cnt,
BumpVectorContext &C) {
return iterator(Elements.insert(I.base(), Cnt,
CFGAutomaticObjDtor(nullptr, nullptr), C));
}
iterator insertAutomaticObjDtor(iterator I, VarDecl *VD, Stmt *S) {
*I = CFGAutomaticObjDtor(VD, S);
return ++I;
}




iterator beginLifetimeEndsInsert(iterator I, size_t Cnt,
BumpVectorContext &C) {
return iterator(
Elements.insert(I.base(), Cnt, CFGLifetimeEnds(nullptr, nullptr), C));
}
iterator insertLifetimeEnds(iterator I, VarDecl *VD, Stmt *S) {
*I = CFGLifetimeEnds(VD, S);
return ++I;
}




iterator beginScopeEndInsert(iterator I, size_t Cnt, BumpVectorContext &C) {
return iterator(
Elements.insert(I.base(), Cnt, CFGScopeEnd(nullptr, nullptr), C));
}
iterator insertScopeEnd(iterator I, VarDecl *VD, Stmt *S) {
*I = CFGScopeEnd(VD, S);
return ++I;
}
};



class CFGCallback {
public:
CFGCallback() = default;
virtual ~CFGCallback() = default;

virtual void compareAlwaysTrue(const BinaryOperator *B, bool isAlwaysTrue) {}
virtual void compareBitwiseEquality(const BinaryOperator *B,
bool isAlwaysTrue) {}
virtual void compareBitwiseOr(const BinaryOperator *B) {}
};








class CFG {
public:




class BuildOptions {
std::bitset<Stmt::lastStmtConstant> alwaysAddMask;

public:
using ForcedBlkExprs = llvm::DenseMap<const Stmt *, const CFGBlock *>;

ForcedBlkExprs **forcedBlkExprs = nullptr;
CFGCallback *Observer = nullptr;
bool PruneTriviallyFalseEdges = true;
bool AddEHEdges = false;
bool AddInitializers = false;
bool AddImplicitDtors = false;
bool AddLifetime = false;
bool AddLoopExit = false;
bool AddTemporaryDtors = false;
bool AddScopes = false;
bool AddStaticInitBranches = false;
bool AddCXXNewAllocator = false;
bool AddCXXDefaultInitExprInCtors = false;
bool AddCXXDefaultInitExprInAggregates = false;
bool AddRichCXXConstructors = false;
bool MarkElidedCXXConstructors = false;
bool AddVirtualBaseBranches = false;
bool OmitImplicitValueInitializers = false;

BuildOptions() = default;

bool alwaysAdd(const Stmt *stmt) const {
return alwaysAddMask[stmt->getStmtClass()];
}

BuildOptions &setAlwaysAdd(Stmt::StmtClass stmtClass, bool val = true) {
alwaysAddMask[stmtClass] = val;
return *this;
}

BuildOptions &setAllAlwaysAdd() {
alwaysAddMask.set();
return *this;
}
};


static std::unique_ptr<CFG> buildCFG(const Decl *D, Stmt *AST, ASTContext *C,
const BuildOptions &BO);



CFGBlock *createBlock();




void setEntry(CFGBlock *B) { Entry = B; }



void setIndirectGotoBlock(CFGBlock *B) { IndirectGotoBlock = B; }





using CFGBlockListTy = BumpVector<CFGBlock *>;
using iterator = CFGBlockListTy::iterator;
using const_iterator = CFGBlockListTy::const_iterator;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;

CFGBlock & front() { return *Blocks.front(); }
CFGBlock & back() { return *Blocks.back(); }

iterator begin() { return Blocks.begin(); }
iterator end() { return Blocks.end(); }
const_iterator begin() const { return Blocks.begin(); }
const_iterator end() const { return Blocks.end(); }

iterator nodes_begin() { return iterator(Blocks.begin()); }
iterator nodes_end() { return iterator(Blocks.end()); }

llvm::iterator_range<iterator> nodes() { return {begin(), end()}; }
llvm::iterator_range<const_iterator> const_nodes() const {
return {begin(), end()};
}

const_iterator nodes_begin() const { return const_iterator(Blocks.begin()); }
const_iterator nodes_end() const { return const_iterator(Blocks.end()); }

reverse_iterator rbegin() { return Blocks.rbegin(); }
reverse_iterator rend() { return Blocks.rend(); }
const_reverse_iterator rbegin() const { return Blocks.rbegin(); }
const_reverse_iterator rend() const { return Blocks.rend(); }

llvm::iterator_range<reverse_iterator> reverse_nodes() {
return {rbegin(), rend()};
}
llvm::iterator_range<const_reverse_iterator> const_reverse_nodes() const {
return {rbegin(), rend()};
}

CFGBlock & getEntry() { return *Entry; }
const CFGBlock & getEntry() const { return *Entry; }
CFGBlock & getExit() { return *Exit; }
const CFGBlock & getExit() const { return *Exit; }

CFGBlock * getIndirectGotoBlock() { return IndirectGotoBlock; }
const CFGBlock * getIndirectGotoBlock() const { return IndirectGotoBlock; }

using try_block_iterator = std::vector<const CFGBlock *>::const_iterator;

try_block_iterator try_blocks_begin() const {
return TryDispatchBlocks.begin();
}

try_block_iterator try_blocks_end() const {
return TryDispatchBlocks.end();
}

void addTryDispatchBlock(const CFGBlock *block) {
TryDispatchBlocks.push_back(block);
}





void addSyntheticDeclStmt(const DeclStmt *Synthetic,
const DeclStmt *Source) {
assert(Synthetic->isSingleDecl() && "Can handle single declarations only");
assert(Synthetic != Source && "Don't include original DeclStmts in map");
assert(!SyntheticDeclStmts.count(Synthetic) && "Already in map");
SyntheticDeclStmts[Synthetic] = Source;
}

using synthetic_stmt_iterator =
llvm::DenseMap<const DeclStmt *, const DeclStmt *>::const_iterator;
using synthetic_stmt_range = llvm::iterator_range<synthetic_stmt_iterator>;






synthetic_stmt_iterator synthetic_stmt_begin() const {
return SyntheticDeclStmts.begin();
}


synthetic_stmt_iterator synthetic_stmt_end() const {
return SyntheticDeclStmts.end();
}


synthetic_stmt_range synthetic_stmts() const {
return synthetic_stmt_range(synthetic_stmt_begin(), synthetic_stmt_end());
}





template <typename Callback> void VisitBlockStmts(Callback &O) const {
for (const_iterator I = begin(), E = end(); I != E; ++I)
for (CFGBlock::const_iterator BI = (*I)->begin(), BE = (*I)->end();
BI != BE; ++BI) {
if (Optional<CFGStmt> stmt = BI->getAs<CFGStmt>())
O(const_cast<Stmt *>(stmt->getStmt()));
}
}






unsigned getNumBlockIDs() const { return NumBlockIDs; }




unsigned size() const { return NumBlockIDs; }





bool isLinear() const;





void viewCFG(const LangOptions &LO) const;
void print(raw_ostream &OS, const LangOptions &LO, bool ShowColors) const;
void dump(const LangOptions &LO, bool ShowColors) const;





CFG() : Blocks(BlkBVC, 10) {}

llvm::BumpPtrAllocator& getAllocator() {
return BlkBVC.getAllocator();
}

BumpVectorContext &getBumpVectorContext() {
return BlkBVC;
}

private:
CFGBlock *Entry = nullptr;
CFGBlock *Exit = nullptr;


CFGBlock* IndirectGotoBlock = nullptr;

unsigned NumBlockIDs = 0;

BumpVectorContext BlkBVC;

CFGBlockListTy Blocks;



std::vector<const CFGBlock *> TryDispatchBlocks;



llvm::DenseMap<const DeclStmt *, const DeclStmt *> SyntheticDeclStmts;
};

}





namespace llvm {



template <> struct simplify_type< ::clang::CFGTerminator> {
using SimpleType = ::clang::Stmt *;

static SimpleType getSimplifiedValue(::clang::CFGTerminator Val) {
return Val.getStmt();
}
};



template <> struct GraphTraits< ::clang::CFGBlock *> {
using NodeRef = ::clang::CFGBlock *;
using ChildIteratorType = ::clang::CFGBlock::succ_iterator;

static NodeRef getEntryNode(::clang::CFGBlock *BB) { return BB; }
static ChildIteratorType child_begin(NodeRef N) { return N->succ_begin(); }
static ChildIteratorType child_end(NodeRef N) { return N->succ_end(); }
};

template <> struct GraphTraits<clang::CFGBlock>
: GraphTraits<clang::CFGBlock *> {};

template <> struct GraphTraits< const ::clang::CFGBlock *> {
using NodeRef = const ::clang::CFGBlock *;
using ChildIteratorType = ::clang::CFGBlock::const_succ_iterator;

static NodeRef getEntryNode(const clang::CFGBlock *BB) { return BB; }
static ChildIteratorType child_begin(NodeRef N) { return N->succ_begin(); }
static ChildIteratorType child_end(NodeRef N) { return N->succ_end(); }
};

template <> struct GraphTraits<const clang::CFGBlock>
: GraphTraits<clang::CFGBlock *> {};

template <> struct GraphTraits<Inverse< ::clang::CFGBlock *>> {
using NodeRef = ::clang::CFGBlock *;
using ChildIteratorType = ::clang::CFGBlock::const_pred_iterator;

static NodeRef getEntryNode(Inverse<::clang::CFGBlock *> G) {
return G.Graph;
}

static ChildIteratorType child_begin(NodeRef N) { return N->pred_begin(); }
static ChildIteratorType child_end(NodeRef N) { return N->pred_end(); }
};

template <> struct GraphTraits<Inverse<clang::CFGBlock>>
: GraphTraits<clang::CFGBlock *> {};

template <> struct GraphTraits<Inverse<const ::clang::CFGBlock *>> {
using NodeRef = const ::clang::CFGBlock *;
using ChildIteratorType = ::clang::CFGBlock::const_pred_iterator;

static NodeRef getEntryNode(Inverse<const ::clang::CFGBlock *> G) {
return G.Graph;
}

static ChildIteratorType child_begin(NodeRef N) { return N->pred_begin(); }
static ChildIteratorType child_end(NodeRef N) { return N->pred_end(); }
};

template <> struct GraphTraits<const Inverse<clang::CFGBlock>>
: GraphTraits<clang::CFGBlock *> {};



template <> struct GraphTraits< ::clang::CFG* >
: public GraphTraits< ::clang::CFGBlock *> {
using nodes_iterator = ::clang::CFG::iterator;

static NodeRef getEntryNode(::clang::CFG *F) { return &F->getEntry(); }
static nodes_iterator nodes_begin(::clang::CFG* F) { return F->nodes_begin();}
static nodes_iterator nodes_end(::clang::CFG* F) { return F->nodes_end(); }
static unsigned size(::clang::CFG* F) { return F->size(); }
};

template <> struct GraphTraits<const ::clang::CFG* >
: public GraphTraits<const ::clang::CFGBlock *> {
using nodes_iterator = ::clang::CFG::const_iterator;

static NodeRef getEntryNode(const ::clang::CFG *F) { return &F->getEntry(); }

static nodes_iterator nodes_begin( const ::clang::CFG* F) {
return F->nodes_begin();
}

static nodes_iterator nodes_end( const ::clang::CFG* F) {
return F->nodes_end();
}

static unsigned size(const ::clang::CFG* F) {
return F->size();
}
};

template <> struct GraphTraits<Inverse< ::clang::CFG *>>
: public GraphTraits<Inverse< ::clang::CFGBlock *>> {
using nodes_iterator = ::clang::CFG::iterator;

static NodeRef getEntryNode(::clang::CFG *F) { return &F->getExit(); }
static nodes_iterator nodes_begin( ::clang::CFG* F) {return F->nodes_begin();}
static nodes_iterator nodes_end( ::clang::CFG* F) { return F->nodes_end(); }
};

template <> struct GraphTraits<Inverse<const ::clang::CFG *>>
: public GraphTraits<Inverse<const ::clang::CFGBlock *>> {
using nodes_iterator = ::clang::CFG::const_iterator;

static NodeRef getEntryNode(const ::clang::CFG *F) { return &F->getExit(); }

static nodes_iterator nodes_begin(const ::clang::CFG* F) {
return F->nodes_begin();
}

static nodes_iterator nodes_end(const ::clang::CFG* F) {
return F->nodes_end();
}
};

}

#endif
