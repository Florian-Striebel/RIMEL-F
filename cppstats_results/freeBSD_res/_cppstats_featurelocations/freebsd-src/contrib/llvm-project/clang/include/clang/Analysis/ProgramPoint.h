












#if !defined(LLVM_CLANG_ANALYSIS_PROGRAMPOINT_H)
#define LLVM_CLANG_ANALYSIS_PROGRAMPOINT_H

#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/CFG.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/DataTypes.h"
#include <cassert>
#include <string>
#include <utility>

namespace clang {

class AnalysisDeclContext;
class FunctionDecl;
class LocationContext;




class ProgramPointTag {
public:
ProgramPointTag(void *tagKind = nullptr) : TagKind(tagKind) {}
virtual ~ProgramPointTag();
virtual StringRef getTagDescription() const = 0;


const void *getTagKind() const { return TagKind; }

private:
const void *const TagKind;
};

class SimpleProgramPointTag : public ProgramPointTag {
std::string Desc;
public:
SimpleProgramPointTag(StringRef MsgProvider, StringRef Msg);
StringRef getTagDescription() const override;
};

class ProgramPoint {
public:
enum Kind { BlockEdgeKind,
BlockEntranceKind,
BlockExitKind,
PreStmtKind,
PreStmtPurgeDeadSymbolsKind,
PostStmtPurgeDeadSymbolsKind,
PostStmtKind,
PreLoadKind,
PostLoadKind,
PreStoreKind,
PostStoreKind,
PostConditionKind,
PostLValueKind,
PostAllocatorCallKind,
MinPostStmtKind = PostStmtKind,
MaxPostStmtKind = PostAllocatorCallKind,
PostInitializerKind,
CallEnterKind,
CallExitBeginKind,
CallExitEndKind,
FunctionExitKind,
PreImplicitCallKind,
PostImplicitCallKind,
MinImplicitCallKind = PreImplicitCallKind,
MaxImplicitCallKind = PostImplicitCallKind,
LoopExitKind,
EpsilonKind};

private:
const void *Data1;
llvm::PointerIntPair<const void *, 2, unsigned> Data2;



llvm::PointerIntPair<const LocationContext *, 2, unsigned> L;

llvm::PointerIntPair<const ProgramPointTag *, 2, unsigned> Tag;

protected:
ProgramPoint() = default;
ProgramPoint(const void *P,
Kind k,
const LocationContext *l,
const ProgramPointTag *tag = nullptr)
: Data1(P),
Data2(nullptr, (((unsigned) k) >> 0) & 0x3),
L(l, (((unsigned) k) >> 2) & 0x3),
Tag(tag, (((unsigned) k) >> 4) & 0x3) {
assert(getKind() == k);
assert(getLocationContext() == l);
assert(getData1() == P);
}

ProgramPoint(const void *P1,
const void *P2,
Kind k,
const LocationContext *l,
const ProgramPointTag *tag = nullptr)
: Data1(P1),
Data2(P2, (((unsigned) k) >> 0) & 0x3),
L(l, (((unsigned) k) >> 2) & 0x3),
Tag(tag, (((unsigned) k) >> 4) & 0x3) {}

protected:
const void *getData1() const { return Data1; }
const void *getData2() const { return Data2.getPointer(); }
void setData2(const void *d) { Data2.setPointer(d); }

public:


ProgramPoint withTag(const ProgramPointTag *tag) const {
return ProgramPoint(getData1(), getData2(), getKind(),
getLocationContext(), tag);
}



template<typename T>
T castAs() const {
assert(T::isKind(*this));
T t;
ProgramPoint& PP = t;
PP = *this;
return t;
}



template<typename T>
Optional<T> getAs() const {
if (!T::isKind(*this))
return None;
T t;
ProgramPoint& PP = t;
PP = *this;
return t;
}

Kind getKind() const {
unsigned x = Tag.getInt();
x <<= 2;
x |= L.getInt();
x <<= 2;
x |= Data2.getInt();
return (Kind) x;
}



bool isPurgeKind() {
Kind K = getKind();
return (K == PostStmtPurgeDeadSymbolsKind ||
K == PreStmtPurgeDeadSymbolsKind);
}

const ProgramPointTag *getTag() const { return Tag.getPointer(); }

const LocationContext *getLocationContext() const {
return L.getPointer();
}

const StackFrameContext *getStackFrame() const {
return getLocationContext()->getStackFrame();
}


unsigned getHashValue() const {
llvm::FoldingSetNodeID ID;
Profile(ID);
return ID.ComputeHash();
}

bool operator==(const ProgramPoint & RHS) const {
return Data1 == RHS.Data1 &&
Data2 == RHS.Data2 &&
L == RHS.L &&
Tag == RHS.Tag;
}

bool operator!=(const ProgramPoint &RHS) const {
return Data1 != RHS.Data1 ||
Data2 != RHS.Data2 ||
L != RHS.L ||
Tag != RHS.Tag;
}

void Profile(llvm::FoldingSetNodeID& ID) const {
ID.AddInteger((unsigned) getKind());
ID.AddPointer(getData1());
ID.AddPointer(getData2());
ID.AddPointer(getLocationContext());
ID.AddPointer(getTag());
}

void printJson(llvm::raw_ostream &Out, const char *NL = "\n") const;

LLVM_DUMP_METHOD void dump() const;

static ProgramPoint getProgramPoint(const Stmt *S, ProgramPoint::Kind K,
const LocationContext *LC,
const ProgramPointTag *tag);
};

class BlockEntrance : public ProgramPoint {
public:
BlockEntrance(const CFGBlock *B, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: ProgramPoint(B, BlockEntranceKind, L, tag) {
assert(B && "BlockEntrance requires non-null block");
}

const CFGBlock *getBlock() const {
return reinterpret_cast<const CFGBlock*>(getData1());
}

Optional<CFGElement> getFirstElement() const {
const CFGBlock *B = getBlock();
return B->empty() ? Optional<CFGElement>() : B->front();
}

private:
friend class ProgramPoint;
BlockEntrance() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == BlockEntranceKind;
}
};

class BlockExit : public ProgramPoint {
public:
BlockExit(const CFGBlock *B, const LocationContext *L)
: ProgramPoint(B, BlockExitKind, L) {}

const CFGBlock *getBlock() const {
return reinterpret_cast<const CFGBlock*>(getData1());
}

const Stmt *getTerminator() const {
return getBlock()->getTerminatorStmt();
}

private:
friend class ProgramPoint;
BlockExit() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == BlockExitKind;
}
};

class StmtPoint : public ProgramPoint {
public:
StmtPoint(const Stmt *S, const void *p2, Kind k, const LocationContext *L,
const ProgramPointTag *tag)
: ProgramPoint(S, p2, k, L, tag) {
assert(S);
}

const Stmt *getStmt() const { return (const Stmt*) getData1(); }

template <typename T>
const T* getStmtAs() const { return dyn_cast<T>(getStmt()); }

protected:
StmtPoint() = default;
private:
friend class ProgramPoint;
static bool isKind(const ProgramPoint &Location) {
unsigned k = Location.getKind();
return k >= PreStmtKind && k <= MaxPostStmtKind;
}
};


class PreStmt : public StmtPoint {
public:
PreStmt(const Stmt *S, const LocationContext *L, const ProgramPointTag *tag,
const Stmt *SubStmt = nullptr)
: StmtPoint(S, SubStmt, PreStmtKind, L, tag) {}

const Stmt *getSubStmt() const { return (const Stmt*) getData2(); }

private:
friend class ProgramPoint;
PreStmt() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PreStmtKind;
}
};

class PostStmt : public StmtPoint {
protected:
PostStmt() = default;
PostStmt(const Stmt *S, const void *data, Kind k, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: StmtPoint(S, data, k, L, tag) {}

public:
explicit PostStmt(const Stmt *S, Kind k, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: StmtPoint(S, nullptr, k, L, tag) {}

explicit PostStmt(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: StmtPoint(S, nullptr, PostStmtKind, L, tag) {}

private:
friend class ProgramPoint;
static bool isKind(const ProgramPoint &Location) {
unsigned k = Location.getKind();
return k >= MinPostStmtKind && k <= MaxPostStmtKind;
}
};

class FunctionExitPoint : public ProgramPoint {
public:
explicit FunctionExitPoint(const ReturnStmt *S,
const LocationContext *LC,
const ProgramPointTag *tag = nullptr)
: ProgramPoint(S, FunctionExitKind, LC, tag) {}

const CFGBlock *getBlock() const {
return &getLocationContext()->getCFG()->getExit();
}

const ReturnStmt *getStmt() const {
return reinterpret_cast<const ReturnStmt *>(getData1());
}

private:
friend class ProgramPoint;
FunctionExitPoint() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == FunctionExitKind;
}
};


class PostCondition : public PostStmt {
public:
PostCondition(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: PostStmt(S, PostConditionKind, L, tag) {}

private:
friend class ProgramPoint;
PostCondition() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostConditionKind;
}
};

class LocationCheck : public StmtPoint {
protected:
LocationCheck() = default;
LocationCheck(const Stmt *S, const LocationContext *L,
ProgramPoint::Kind K, const ProgramPointTag *tag)
: StmtPoint(S, nullptr, K, L, tag) {}

private:
friend class ProgramPoint;
static bool isKind(const ProgramPoint &location) {
unsigned k = location.getKind();
return k == PreLoadKind || k == PreStoreKind;
}
};

class PreLoad : public LocationCheck {
public:
PreLoad(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: LocationCheck(S, L, PreLoadKind, tag) {}

private:
friend class ProgramPoint;
PreLoad() = default;
static bool isKind(const ProgramPoint &location) {
return location.getKind() == PreLoadKind;
}
};

class PreStore : public LocationCheck {
public:
PreStore(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: LocationCheck(S, L, PreStoreKind, tag) {}

private:
friend class ProgramPoint;
PreStore() = default;
static bool isKind(const ProgramPoint &location) {
return location.getKind() == PreStoreKind;
}
};

class PostLoad : public PostStmt {
public:
PostLoad(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: PostStmt(S, PostLoadKind, L, tag) {}

private:
friend class ProgramPoint;
PostLoad() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostLoadKind;
}
};


class PostStore : public PostStmt {
public:



PostStore(const Stmt *S, const LocationContext *L, const void *Loc,
const ProgramPointTag *tag = nullptr)
: PostStmt(S, PostStoreKind, L, tag) {
assert(getData2() == nullptr);
setData2(Loc);
}



const void *getLocationValue() const {
return getData2();
}

private:
friend class ProgramPoint;
PostStore() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostStoreKind;
}
};

class PostLValue : public PostStmt {
public:
PostLValue(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: PostStmt(S, PostLValueKind, L, tag) {}

private:
friend class ProgramPoint;
PostLValue() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostLValueKind;
}
};



class PreStmtPurgeDeadSymbols : public StmtPoint {
public:
PreStmtPurgeDeadSymbols(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: StmtPoint(S, nullptr, PreStmtPurgeDeadSymbolsKind, L, tag) { }

private:
friend class ProgramPoint;
PreStmtPurgeDeadSymbols() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PreStmtPurgeDeadSymbolsKind;
}
};



class PostStmtPurgeDeadSymbols : public StmtPoint {
public:
PostStmtPurgeDeadSymbols(const Stmt *S, const LocationContext *L,
const ProgramPointTag *tag = nullptr)
: StmtPoint(S, nullptr, PostStmtPurgeDeadSymbolsKind, L, tag) { }

private:
friend class ProgramPoint;
PostStmtPurgeDeadSymbols() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostStmtPurgeDeadSymbolsKind;
}
};

class BlockEdge : public ProgramPoint {
public:
BlockEdge(const CFGBlock *B1, const CFGBlock *B2, const LocationContext *L)
: ProgramPoint(B1, B2, BlockEdgeKind, L) {
assert(B1 && "BlockEdge: source block must be non-null");
assert(B2 && "BlockEdge: destination block must be non-null");
}

const CFGBlock *getSrc() const {
return static_cast<const CFGBlock*>(getData1());
}

const CFGBlock *getDst() const {
return static_cast<const CFGBlock*>(getData2());
}

private:
friend class ProgramPoint;
BlockEdge() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == BlockEdgeKind;
}
};

class PostInitializer : public ProgramPoint {
public:





PostInitializer(const CXXCtorInitializer *I,
const void *Loc,
const LocationContext *L)
: ProgramPoint(I, Loc, PostInitializerKind, L) {}

const CXXCtorInitializer *getInitializer() const {
return static_cast<const CXXCtorInitializer *>(getData1());
}


const void *getLocationValue() const {
return getData2();
}

private:
friend class ProgramPoint;
PostInitializer() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostInitializerKind;
}
};




class ImplicitCallPoint : public ProgramPoint {
public:
ImplicitCallPoint(const Decl *D, SourceLocation Loc, Kind K,
const LocationContext *L, const ProgramPointTag *Tag)
: ProgramPoint(Loc.getPtrEncoding(), D, K, L, Tag) {}

const Decl *getDecl() const { return static_cast<const Decl *>(getData2()); }
SourceLocation getLocation() const {
return SourceLocation::getFromPtrEncoding(getData1());
}

protected:
ImplicitCallPoint() = default;
private:
friend class ProgramPoint;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() >= MinImplicitCallKind &&
Location.getKind() <= MaxImplicitCallKind;
}
};




class PreImplicitCall : public ImplicitCallPoint {
public:
PreImplicitCall(const Decl *D, SourceLocation Loc, const LocationContext *L,
const ProgramPointTag *Tag = nullptr)
: ImplicitCallPoint(D, Loc, PreImplicitCallKind, L, Tag) {}

private:
friend class ProgramPoint;
PreImplicitCall() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PreImplicitCallKind;
}
};




class PostImplicitCall : public ImplicitCallPoint {
public:
PostImplicitCall(const Decl *D, SourceLocation Loc, const LocationContext *L,
const ProgramPointTag *Tag = nullptr)
: ImplicitCallPoint(D, Loc, PostImplicitCallKind, L, Tag) {}

private:
friend class ProgramPoint;
PostImplicitCall() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostImplicitCallKind;
}
};

class PostAllocatorCall : public StmtPoint {
public:
PostAllocatorCall(const Stmt *S, const LocationContext *L,
const ProgramPointTag *Tag = nullptr)
: StmtPoint(S, nullptr, PostAllocatorCallKind, L, Tag) {}

private:
friend class ProgramPoint;
PostAllocatorCall() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == PostAllocatorCallKind;
}
};



class CallEnter : public ProgramPoint {
public:
CallEnter(const Stmt *stmt, const StackFrameContext *calleeCtx,
const LocationContext *callerCtx)
: ProgramPoint(stmt, calleeCtx, CallEnterKind, callerCtx, nullptr) {}

const Stmt *getCallExpr() const {
return static_cast<const Stmt *>(getData1());
}

const StackFrameContext *getCalleeContext() const {
return static_cast<const StackFrameContext *>(getData2());
}


const CFGBlock *getEntry() const {
const StackFrameContext *CalleeCtx = getCalleeContext();
const CFG *CalleeCFG = CalleeCtx->getCFG();
return &(CalleeCFG->getEntry());
}

private:
friend class ProgramPoint;
CallEnter() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == CallEnterKind;
}
};










class CallExitBegin : public ProgramPoint {
public:

CallExitBegin(const StackFrameContext *L, const ReturnStmt *RS)
: ProgramPoint(RS, CallExitBeginKind, L, nullptr) { }

const ReturnStmt *getReturnStmt() const {
return static_cast<const ReturnStmt *>(getData1());
}

private:
friend class ProgramPoint;
CallExitBegin() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == CallExitBeginKind;
}
};



class CallExitEnd : public ProgramPoint {
public:

CallExitEnd(const StackFrameContext *CalleeCtx,
const LocationContext *CallerCtx)
: ProgramPoint(CalleeCtx, CallExitEndKind, CallerCtx, nullptr) {}

const StackFrameContext *getCalleeContext() const {
return static_cast<const StackFrameContext *>(getData1());
}

private:
friend class ProgramPoint;
CallExitEnd() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == CallExitEndKind;
}
};







class LoopExit : public ProgramPoint {
public:
LoopExit(const Stmt *LoopStmt, const LocationContext *LC)
: ProgramPoint(LoopStmt, nullptr, LoopExitKind, LC) {}

const Stmt *getLoopStmt() const {
return static_cast<const Stmt *>(getData1());
}

private:
friend class ProgramPoint;
LoopExit() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == LoopExitKind;
}
};



class EpsilonPoint : public ProgramPoint {
public:
EpsilonPoint(const LocationContext *L, const void *Data1,
const void *Data2 = nullptr,
const ProgramPointTag *tag = nullptr)
: ProgramPoint(Data1, Data2, EpsilonKind, L, tag) {}

const void *getData() const { return getData1(); }

private:
friend class ProgramPoint;
EpsilonPoint() = default;
static bool isKind(const ProgramPoint &Location) {
return Location.getKind() == EpsilonKind;
}
};

}


namespace llvm {

template <> struct DenseMapInfo<clang::ProgramPoint> {

static inline clang::ProgramPoint getEmptyKey() {
uintptr_t x =
reinterpret_cast<uintptr_t>(DenseMapInfo<void*>::getEmptyKey()) & ~0x7;
return clang::BlockEntrance(reinterpret_cast<clang::CFGBlock*>(x), nullptr);
}

static inline clang::ProgramPoint getTombstoneKey() {
uintptr_t x =
reinterpret_cast<uintptr_t>(DenseMapInfo<void*>::getTombstoneKey()) & ~0x7;
return clang::BlockEntrance(reinterpret_cast<clang::CFGBlock*>(x), nullptr);
}

static unsigned getHashValue(const clang::ProgramPoint &Loc) {
return Loc.getHashValue();
}

static bool isEqual(const clang::ProgramPoint &L,
const clang::ProgramPoint &R) {
return L == R;
}

};

}

#endif
