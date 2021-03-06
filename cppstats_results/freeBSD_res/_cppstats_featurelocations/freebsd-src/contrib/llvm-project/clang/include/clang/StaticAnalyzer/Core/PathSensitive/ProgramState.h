











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_PROGRAMSTATE_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_PROGRAMSTATE_H

#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ConstraintManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/DynamicTypeInfo.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/Environment.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SValBuilder.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/Store.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/ImmutableMap.h"
#include "llvm/Support/Allocator.h"
#include <utility>

namespace llvm {
class APSInt;
}

namespace clang {
class ASTContext;

namespace ento {

class AnalysisManager;
class CallEvent;
class CallEventManager;

typedef std::unique_ptr<ConstraintManager>(*ConstraintManagerCreator)(
ProgramStateManager &, ExprEngine *);
typedef std::unique_ptr<StoreManager>(*StoreManagerCreator)(
ProgramStateManager &);





template <typename T> struct ProgramStatePartialTrait;

template <typename T> struct ProgramStateTrait {
typedef typename T::data_type data_type;
static inline void *MakeVoidPtr(data_type D) { return (void*) D; }
static inline data_type MakeData(void *const* P) {
return P ? (data_type) *P : (data_type) 0;
}
};













class ProgramState : public llvm::FoldingSetNode {
public:
typedef llvm::ImmutableSet<llvm::APSInt*> IntSetTy;
typedef llvm::ImmutableMap<void*, void*> GenericDataMap;

private:
void operator=(const ProgramState& R) = delete;

friend class ProgramStateManager;
friend class ExplodedGraph;
friend class ExplodedNode;

ProgramStateManager *stateMgr;
Environment Env;
Store store;
GenericDataMap GDM;
unsigned refCount;



ProgramStateRef makeWithStore(const StoreRef &store) const;

void setStore(const StoreRef &storeRef);

public:

ProgramState(ProgramStateManager *mgr, const Environment& env,
StoreRef st, GenericDataMap gdm);



ProgramState(const ProgramState &RHS);

~ProgramState();

int64_t getID() const;


ProgramStateManager &getStateManager() const {
return *stateMgr;
}

AnalysisManager &getAnalysisManager() const;


ConstraintManager &getConstraintManager() const;



const Environment& getEnvironment() const { return Env; }



Store getStore() const { return store; }



GenericDataMap getGDM() const { return GDM; }

void setGDM(GenericDataMap gdm) { GDM = gdm; }




static void Profile(llvm::FoldingSetNodeID& ID, const ProgramState *V) {
V->Env.Profile(ID);
ID.AddPointer(V->store);
V->GDM.Profile(ID);
}



void Profile(llvm::FoldingSetNodeID& ID) const {
Profile(ID, this);
}

BasicValueFactory &getBasicVals() const;
SymbolManager &getSymbolManager() const;
































LLVM_NODISCARD ProgramStateRef assume(DefinedOrUnknownSVal cond,
bool assumption) const;






LLVM_NODISCARD std::pair<ProgramStateRef, ProgramStateRef>
assume(DefinedOrUnknownSVal cond) const;

LLVM_NODISCARD ProgramStateRef
assumeInBound(DefinedOrUnknownSVal idx, DefinedOrUnknownSVal upperBound,
bool assumption, QualType IndexType = QualType()) const;







LLVM_NODISCARD ProgramStateRef assumeInclusiveRange(DefinedOrUnknownSVal Val,
const llvm::APSInt &From,
const llvm::APSInt &To,
bool assumption) const;






LLVM_NODISCARD std::pair<ProgramStateRef, ProgramStateRef>
assumeInclusiveRange(DefinedOrUnknownSVal Val, const llvm::APSInt &From,
const llvm::APSInt &To) const;



ConditionTruthVal isNonNull(SVal V) const;



ConditionTruthVal isNull(SVal V) const;


ConditionTruthVal areEqual(SVal Lhs, SVal Rhs) const;


const VarRegion* getRegion(const VarDecl *D, const LocationContext *LC) const;







LLVM_NODISCARD ProgramStateRef BindExpr(const Stmt *S,
const LocationContext *LCtx, SVal V,
bool Invalidate = true) const;

LLVM_NODISCARD ProgramStateRef bindLoc(Loc location, SVal V,
const LocationContext *LCtx,
bool notifyChanges = true) const;

LLVM_NODISCARD ProgramStateRef bindLoc(SVal location, SVal V,
const LocationContext *LCtx) const;







LLVM_NODISCARD ProgramStateRef
bindDefaultInitial(SVal loc, SVal V, const LocationContext *LCtx) const;



LLVM_NODISCARD ProgramStateRef
bindDefaultZero(SVal loc, const LocationContext *LCtx) const;

LLVM_NODISCARD ProgramStateRef killBinding(Loc LV) const;



















LLVM_NODISCARD ProgramStateRef
invalidateRegions(ArrayRef<const MemRegion *> Regions, const Expr *E,
unsigned BlockCount, const LocationContext *LCtx,
bool CausesPointerEscape, InvalidatedSymbols *IS = nullptr,
const CallEvent *Call = nullptr,
RegionAndSymbolInvalidationTraits *ITraits = nullptr) const;

LLVM_NODISCARD ProgramStateRef
invalidateRegions(ArrayRef<SVal> Regions, const Expr *E,
unsigned BlockCount, const LocationContext *LCtx,
bool CausesPointerEscape, InvalidatedSymbols *IS = nullptr,
const CallEvent *Call = nullptr,
RegionAndSymbolInvalidationTraits *ITraits = nullptr) const;



LLVM_NODISCARD ProgramStateRef enterStackFrame(
const CallEvent &Call, const StackFrameContext *CalleeCtx) const;


SVal getSelfSVal(const LocationContext *LC) const;


Loc getLValue(const CXXBaseSpecifier &BaseSpec, const SubRegion *Super) const;


Loc getLValue(const CXXRecordDecl *BaseClass, const SubRegion *Super,
bool IsVirtual) const;


Loc getLValue(const Expr *Call, unsigned Index,
const LocationContext *LC) const;


Loc getLValue(const VarDecl *D, const LocationContext *LC) const;

Loc getLValue(const CompoundLiteralExpr *literal,
const LocationContext *LC) const;


SVal getLValue(const ObjCIvarDecl *decl, SVal base) const;


SVal getLValue(const FieldDecl *decl, SVal Base) const;


SVal getLValue(const IndirectFieldDecl *decl, SVal Base) const;


SVal getLValue(QualType ElementType, SVal Idx, SVal Base) const;


SVal getSVal(const Stmt *S, const LocationContext *LCtx) const;

SVal getSValAsScalarOrLoc(const Stmt *Ex, const LocationContext *LCtx) const;



SVal getSVal(Loc LV, QualType T = QualType()) const;


SVal getRawSVal(Loc LV, QualType T= QualType()) const;



SVal getSVal(const MemRegion* R, QualType T = QualType()) const;





SVal getSValAsScalarOrLoc(const MemRegion *R) const;

using region_iterator = const MemRegion **;








bool scanReachableSymbols(SVal val, SymbolVisitor& visitor) const;



bool scanReachableSymbols(llvm::iterator_range<region_iterator> Reachable,
SymbolVisitor &visitor) const;

template <typename CB> CB scanReachableSymbols(SVal val) const;
template <typename CB> CB
scanReachableSymbols(llvm::iterator_range<region_iterator> Reachable) const;





void *const* FindGDM(void *K) const;

template <typename T>
LLVM_NODISCARD ProgramStateRef
add(typename ProgramStateTrait<T>::key_type K) const;

template <typename T>
typename ProgramStateTrait<T>::data_type
get() const {
return ProgramStateTrait<T>::MakeData(FindGDM(ProgramStateTrait<T>::GDMIndex()));
}

template<typename T>
typename ProgramStateTrait<T>::lookup_type
get(typename ProgramStateTrait<T>::key_type key) const {
void *const* d = FindGDM(ProgramStateTrait<T>::GDMIndex());
return ProgramStateTrait<T>::Lookup(ProgramStateTrait<T>::MakeData(d), key);
}

template <typename T>
typename ProgramStateTrait<T>::context_type get_context() const;

template <typename T>
LLVM_NODISCARD ProgramStateRef
remove(typename ProgramStateTrait<T>::key_type K) const;

template <typename T>
LLVM_NODISCARD ProgramStateRef
remove(typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::context_type C) const;

template <typename T> LLVM_NODISCARD ProgramStateRef remove() const;

template <typename T>
LLVM_NODISCARD ProgramStateRef
set(typename ProgramStateTrait<T>::data_type D) const;

template <typename T>
LLVM_NODISCARD ProgramStateRef
set(typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::value_type E) const;

template <typename T>
LLVM_NODISCARD ProgramStateRef
set(typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::value_type E,
typename ProgramStateTrait<T>::context_type C) const;

template<typename T>
bool contains(typename ProgramStateTrait<T>::key_type key) const {
void *const* d = FindGDM(ProgramStateTrait<T>::GDMIndex());
return ProgramStateTrait<T>::Contains(ProgramStateTrait<T>::MakeData(d), key);
}


void printJson(raw_ostream &Out, const LocationContext *LCtx = nullptr,
const char *NL = "\n", unsigned int Space = 0,
bool IsDot = false) const;

void printDOT(raw_ostream &Out, const LocationContext *LCtx = nullptr,
unsigned int Space = 0) const;

void dump() const;

private:
friend void ProgramStateRetain(const ProgramState *state);
friend void ProgramStateRelease(const ProgramState *state);



ProgramStateRef
invalidateRegionsImpl(ArrayRef<SVal> Values,
const Expr *E, unsigned BlockCount,
const LocationContext *LCtx,
bool ResultsInSymbolEscape,
InvalidatedSymbols *IS,
RegionAndSymbolInvalidationTraits *HTraits,
const CallEvent *Call) const;
};





class ProgramStateManager {
friend class ProgramState;
friend void ProgramStateRelease(const ProgramState *state);
private:

ExprEngine *Eng;

EnvironmentManager EnvMgr;
std::unique_ptr<StoreManager> StoreMgr;
std::unique_ptr<ConstraintManager> ConstraintMgr;

ProgramState::GenericDataMap::Factory GDMFactory;

typedef llvm::DenseMap<void*,std::pair<void*,void (*)(void*)> > GDMContextsTy;
GDMContextsTy GDMContexts;



llvm::FoldingSet<ProgramState> StateSet;


std::unique_ptr<SValBuilder> svalBuilder;


std::unique_ptr<CallEventManager> CallEventMgr;


llvm::BumpPtrAllocator &Alloc;


std::vector<ProgramState *> freeStates;

public:
ProgramStateManager(ASTContext &Ctx,
StoreManagerCreator CreateStoreManager,
ConstraintManagerCreator CreateConstraintManager,
llvm::BumpPtrAllocator& alloc,
ExprEngine *expreng);

~ProgramStateManager();

ProgramStateRef getInitialState(const LocationContext *InitLoc);

ASTContext &getContext() { return svalBuilder->getContext(); }
const ASTContext &getContext() const { return svalBuilder->getContext(); }

BasicValueFactory &getBasicVals() {
return svalBuilder->getBasicValueFactory();
}

SValBuilder &getSValBuilder() {
return *svalBuilder;
}

const SValBuilder &getSValBuilder() const {
return *svalBuilder;
}

SymbolManager &getSymbolManager() {
return svalBuilder->getSymbolManager();
}
const SymbolManager &getSymbolManager() const {
return svalBuilder->getSymbolManager();
}

llvm::BumpPtrAllocator& getAllocator() { return Alloc; }

MemRegionManager& getRegionManager() {
return svalBuilder->getRegionManager();
}
const MemRegionManager &getRegionManager() const {
return svalBuilder->getRegionManager();
}

CallEventManager &getCallEventManager() { return *CallEventMgr; }

StoreManager &getStoreManager() { return *StoreMgr; }
ConstraintManager &getConstraintManager() { return *ConstraintMgr; }
ExprEngine &getOwningEngine() { return *Eng; }

ProgramStateRef
removeDeadBindingsFromEnvironmentAndStore(ProgramStateRef St,
const StackFrameContext *LCtx,
SymbolReaper &SymReaper);

public:

SVal ArrayToPointer(Loc Array, QualType ElementTy) {
return StoreMgr->ArrayToPointer(Array, ElementTy);
}


ProgramStateRef addGDM(ProgramStateRef St, void *Key, void *Data);
ProgramStateRef removeGDM(ProgramStateRef state, void *Key);



void iterBindings(ProgramStateRef state, StoreManager::BindingsHandler& F) {
StoreMgr->iterBindings(state->getStore(), F);
}

ProgramStateRef getPersistentState(ProgramState &Impl);
ProgramStateRef getPersistentStateWithGDM(ProgramStateRef FromState,
ProgramStateRef GDMState);

bool haveEqualConstraints(ProgramStateRef S1, ProgramStateRef S2) const {
return ConstraintMgr->haveEqualConstraints(S1, S2);
}

bool haveEqualEnvironments(ProgramStateRef S1, ProgramStateRef S2) const {
return S1->Env == S2->Env;
}

bool haveEqualStores(ProgramStateRef S1, ProgramStateRef S2) const {
return S1->store == S2->store;
}




















template <typename T>
ProgramStateRef set(ProgramStateRef st, typename ProgramStateTrait<T>::data_type D) {
return addGDM(st, ProgramStateTrait<T>::GDMIndex(),
ProgramStateTrait<T>::MakeVoidPtr(D));
}

template<typename T>
ProgramStateRef set(ProgramStateRef st,
typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::value_type V,
typename ProgramStateTrait<T>::context_type C) {

return addGDM(st, ProgramStateTrait<T>::GDMIndex(),
ProgramStateTrait<T>::MakeVoidPtr(ProgramStateTrait<T>::Set(st->get<T>(), K, V, C)));
}

template <typename T>
ProgramStateRef add(ProgramStateRef st,
typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::context_type C) {
return addGDM(st, ProgramStateTrait<T>::GDMIndex(),
ProgramStateTrait<T>::MakeVoidPtr(ProgramStateTrait<T>::Add(st->get<T>(), K, C)));
}

template <typename T>
ProgramStateRef remove(ProgramStateRef st,
typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::context_type C) {

return addGDM(st, ProgramStateTrait<T>::GDMIndex(),
ProgramStateTrait<T>::MakeVoidPtr(ProgramStateTrait<T>::Remove(st->get<T>(), K, C)));
}

template <typename T>
ProgramStateRef remove(ProgramStateRef st) {
return removeGDM(st, ProgramStateTrait<T>::GDMIndex());
}

void *FindGDMContext(void *index,
void *(*CreateContext)(llvm::BumpPtrAllocator&),
void (*DeleteContext)(void*));

template <typename T>
typename ProgramStateTrait<T>::context_type get_context() {
void *p = FindGDMContext(ProgramStateTrait<T>::GDMIndex(),
ProgramStateTrait<T>::CreateContext,
ProgramStateTrait<T>::DeleteContext);

return ProgramStateTrait<T>::MakeContext(p);
}
};






inline ConstraintManager &ProgramState::getConstraintManager() const {
return stateMgr->getConstraintManager();
}

inline const VarRegion* ProgramState::getRegion(const VarDecl *D,
const LocationContext *LC) const
{
return getStateManager().getRegionManager().getVarRegion(D, LC);
}

inline ProgramStateRef ProgramState::assume(DefinedOrUnknownSVal Cond,
bool Assumption) const {
if (Cond.isUnknown())
return this;

return getStateManager().ConstraintMgr
->assume(this, Cond.castAs<DefinedSVal>(), Assumption);
}

inline std::pair<ProgramStateRef , ProgramStateRef >
ProgramState::assume(DefinedOrUnknownSVal Cond) const {
if (Cond.isUnknown())
return std::make_pair(this, this);

return getStateManager().ConstraintMgr
->assumeDual(this, Cond.castAs<DefinedSVal>());
}

inline ProgramStateRef ProgramState::assumeInclusiveRange(
DefinedOrUnknownSVal Val, const llvm::APSInt &From, const llvm::APSInt &To,
bool Assumption) const {
if (Val.isUnknown())
return this;

assert(Val.getAs<NonLoc>() && "Only NonLocs are supported!");

return getStateManager().ConstraintMgr->assumeInclusiveRange(
this, Val.castAs<NonLoc>(), From, To, Assumption);
}

inline std::pair<ProgramStateRef, ProgramStateRef>
ProgramState::assumeInclusiveRange(DefinedOrUnknownSVal Val,
const llvm::APSInt &From,
const llvm::APSInt &To) const {
if (Val.isUnknown())
return std::make_pair(this, this);

assert(Val.getAs<NonLoc>() && "Only NonLocs are supported!");

return getStateManager().ConstraintMgr->assumeInclusiveRangeDual(
this, Val.castAs<NonLoc>(), From, To);
}

inline ProgramStateRef ProgramState::bindLoc(SVal LV, SVal V, const LocationContext *LCtx) const {
if (Optional<Loc> L = LV.getAs<Loc>())
return bindLoc(*L, V, LCtx);
return this;
}

inline Loc ProgramState::getLValue(const CXXBaseSpecifier &BaseSpec,
const SubRegion *Super) const {
const auto *Base = BaseSpec.getType()->getAsCXXRecordDecl();
return loc::MemRegionVal(
getStateManager().getRegionManager().getCXXBaseObjectRegion(
Base, Super, BaseSpec.isVirtual()));
}

inline Loc ProgramState::getLValue(const CXXRecordDecl *BaseClass,
const SubRegion *Super,
bool IsVirtual) const {
return loc::MemRegionVal(
getStateManager().getRegionManager().getCXXBaseObjectRegion(
BaseClass, Super, IsVirtual));
}

inline Loc ProgramState::getLValue(const VarDecl *VD,
const LocationContext *LC) const {
return getStateManager().StoreMgr->getLValueVar(VD, LC);
}

inline Loc ProgramState::getLValue(const CompoundLiteralExpr *literal,
const LocationContext *LC) const {
return getStateManager().StoreMgr->getLValueCompoundLiteral(literal, LC);
}

inline SVal ProgramState::getLValue(const ObjCIvarDecl *D, SVal Base) const {
return getStateManager().StoreMgr->getLValueIvar(D, Base);
}

inline SVal ProgramState::getLValue(const FieldDecl *D, SVal Base) const {
return getStateManager().StoreMgr->getLValueField(D, Base);
}

inline SVal ProgramState::getLValue(const IndirectFieldDecl *D,
SVal Base) const {
StoreManager &SM = *getStateManager().StoreMgr;
for (const auto *I : D->chain()) {
Base = SM.getLValueField(cast<FieldDecl>(I), Base);
}

return Base;
}

inline SVal ProgramState::getLValue(QualType ElementType, SVal Idx, SVal Base) const{
if (Optional<NonLoc> N = Idx.getAs<NonLoc>())
return getStateManager().StoreMgr->getLValueElement(ElementType, *N, Base);
return UnknownVal();
}

inline SVal ProgramState::getSVal(const Stmt *Ex,
const LocationContext *LCtx) const{
return Env.getSVal(EnvironmentEntry(Ex, LCtx),
*getStateManager().svalBuilder);
}

inline SVal
ProgramState::getSValAsScalarOrLoc(const Stmt *S,
const LocationContext *LCtx) const {
if (const Expr *Ex = dyn_cast<Expr>(S)) {
QualType T = Ex->getType();
if (Ex->isGLValue() || Loc::isLocType(T) ||
T->isIntegralOrEnumerationType())
return getSVal(S, LCtx);
}

return UnknownVal();
}

inline SVal ProgramState::getRawSVal(Loc LV, QualType T) const {
return getStateManager().StoreMgr->getBinding(getStore(), LV, T);
}

inline SVal ProgramState::getSVal(const MemRegion* R, QualType T) const {
return getStateManager().StoreMgr->getBinding(getStore(),
loc::MemRegionVal(R),
T);
}

inline BasicValueFactory &ProgramState::getBasicVals() const {
return getStateManager().getBasicVals();
}

inline SymbolManager &ProgramState::getSymbolManager() const {
return getStateManager().getSymbolManager();
}

template<typename T>
ProgramStateRef ProgramState::add(typename ProgramStateTrait<T>::key_type K) const {
return getStateManager().add<T>(this, K, get_context<T>());
}

template <typename T>
typename ProgramStateTrait<T>::context_type ProgramState::get_context() const {
return getStateManager().get_context<T>();
}

template<typename T>
ProgramStateRef ProgramState::remove(typename ProgramStateTrait<T>::key_type K) const {
return getStateManager().remove<T>(this, K, get_context<T>());
}

template<typename T>
ProgramStateRef ProgramState::remove(typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::context_type C) const {
return getStateManager().remove<T>(this, K, C);
}

template <typename T>
ProgramStateRef ProgramState::remove() const {
return getStateManager().remove<T>(this);
}

template<typename T>
ProgramStateRef ProgramState::set(typename ProgramStateTrait<T>::data_type D) const {
return getStateManager().set<T>(this, D);
}

template<typename T>
ProgramStateRef ProgramState::set(typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::value_type E) const {
return getStateManager().set<T>(this, K, E, get_context<T>());
}

template<typename T>
ProgramStateRef ProgramState::set(typename ProgramStateTrait<T>::key_type K,
typename ProgramStateTrait<T>::value_type E,
typename ProgramStateTrait<T>::context_type C) const {
return getStateManager().set<T>(this, K, E, C);
}

template <typename CB>
CB ProgramState::scanReachableSymbols(SVal val) const {
CB cb(this);
scanReachableSymbols(val, cb);
return cb;
}

template <typename CB>
CB ProgramState::scanReachableSymbols(
llvm::iterator_range<region_iterator> Reachable) const {
CB cb(this);
scanReachableSymbols(Reachable, cb);
return cb;
}





class ScanReachableSymbols {
typedef llvm::DenseSet<const void*> VisitedItems;

VisitedItems visited;
ProgramStateRef state;
SymbolVisitor &visitor;
public:
ScanReachableSymbols(ProgramStateRef st, SymbolVisitor &v)
: state(std::move(st)), visitor(v) {}

bool scan(nonloc::LazyCompoundVal val);
bool scan(nonloc::CompoundVal val);
bool scan(SVal val);
bool scan(const MemRegion *R);
bool scan(const SymExpr *sym);
};

}

}

#endif
