











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_STORE_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_STORE_H

#include "clang/AST/Type.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/MemRegion.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SValBuilder.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/StoreRef.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymExpr.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <cstdint>
#include <memory>

namespace clang {

class ASTContext;
class CastExpr;
class CompoundLiteralExpr;
class CXXBasePath;
class Decl;
class Expr;
class LocationContext;
class ObjCIvarDecl;
class StackFrameContext;

namespace ento {

class CallEvent;
class ProgramStateManager;
class ScanReachableSymbols;
class SymbolReaper;

using InvalidatedSymbols = llvm::DenseSet<SymbolRef>;

class StoreManager {
protected:
SValBuilder &svalBuilder;
ProgramStateManager &StateMgr;


MemRegionManager &MRMgr;
ASTContext &Ctx;

StoreManager(ProgramStateManager &stateMgr);

public:
virtual ~StoreManager() = default;








virtual SVal getBinding(Store store, Loc loc, QualType T = QualType()) = 0;











virtual Optional<SVal> getDefaultBinding(Store store, const MemRegion *R) = 0;









Optional<SVal> getDefaultBinding(nonloc::LazyCompoundVal lcv) {
return getDefaultBinding(lcv.getStore(), lcv.getRegion());
}








virtual StoreRef Bind(Store store, Loc loc, SVal val) = 0;




virtual StoreRef BindDefaultInitial(Store store, const MemRegion *R,
SVal V) = 0;



virtual StoreRef BindDefaultZero(Store store, const MemRegion *R) = 0;




virtual StoreRef killBinding(Store ST, Loc L) = 0;



virtual StoreRef getInitialStore(const LocationContext *InitLoc) = 0;



MemRegionManager& getRegionManager() { return MRMgr; }

SValBuilder& getSValBuilder() { return svalBuilder; }

virtual Loc getLValueVar(const VarDecl *VD, const LocationContext *LC) {
return svalBuilder.makeLoc(MRMgr.getVarRegion(VD, LC));
}

Loc getLValueCompoundLiteral(const CompoundLiteralExpr *CL,
const LocationContext *LC) {
return loc::MemRegionVal(MRMgr.getCompoundLiteralRegion(CL, LC));
}

virtual SVal getLValueIvar(const ObjCIvarDecl *decl, SVal base);

virtual SVal getLValueField(const FieldDecl *D, SVal Base) {
return getLValueFieldOrIvar(D, Base);
}

virtual SVal getLValueElement(QualType elementType, NonLoc offset, SVal Base);



virtual SVal ArrayToPointer(Loc Array, QualType ElementTy) = 0;



SVal evalDerivedToBase(SVal Derived, const CastExpr *Cast);


SVal evalDerivedToBase(SVal Derived, const CXXBasePath &CastPath);


SVal evalDerivedToBase(SVal Derived, QualType DerivedPtrType,
bool IsVirtual);












SVal attemptDownCast(SVal Base, QualType DerivedPtrType, bool &Failed);

const ElementRegion *GetElementZeroRegion(const SubRegion *R, QualType T);




Optional<const MemRegion *> castRegion(const MemRegion *region,
QualType CastToTy);

virtual StoreRef removeDeadBindings(Store store, const StackFrameContext *LCtx,
SymbolReaper &SymReaper) = 0;

virtual bool includedInBindings(Store store,
const MemRegion *region) const = 0;



virtual void incrementReferenceCount(Store store) {}




virtual void decrementReferenceCount(Store store) {}

using InvalidatedRegions = SmallVector<const MemRegion *, 8>;
























virtual StoreRef invalidateRegions(Store store,
ArrayRef<SVal> Values,
const Expr *E, unsigned Count,
const LocationContext *LCtx,
const CallEvent *Call,
InvalidatedSymbols &IS,
RegionAndSymbolInvalidationTraits &ITraits,
InvalidatedRegions *InvalidatedTopLevel,
InvalidatedRegions *Invalidated) = 0;



StoreRef enterStackFrame(Store store,
const CallEvent &Call,
const StackFrameContext *CalleeCtx);




virtual bool scanReachableSymbols(Store S, const MemRegion *R,
ScanReachableSymbols &Visitor) = 0;

virtual void printJson(raw_ostream &Out, Store S, const char *NL,
unsigned int Space, bool IsDot) const = 0;

class BindingsHandler {
public:
virtual ~BindingsHandler();


virtual bool HandleBinding(StoreManager& SMgr, Store store,
const MemRegion *region, SVal val) = 0;
};

class FindUniqueBinding : public BindingsHandler {
SymbolRef Sym;
const MemRegion* Binding = nullptr;
bool First = true;

public:
FindUniqueBinding(SymbolRef sym) : Sym(sym) {}

explicit operator bool() { return First && Binding; }

bool HandleBinding(StoreManager& SMgr, Store store, const MemRegion* R,
SVal val) override;
const MemRegion *getRegion() { return Binding; }
};


virtual void iterBindings(Store store, BindingsHandler& f) = 0;

protected:
const ElementRegion *MakeElementRegion(const SubRegion *baseRegion,
QualType pointeeTy,
uint64_t index = 0);

private:
SVal getLValueFieldOrIvar(const Decl *decl, SVal base);
};

inline StoreRef::StoreRef(Store store, StoreManager & smgr)
: store(store), mgr(smgr) {
if (store)
mgr.incrementReferenceCount(store);
}

inline StoreRef::StoreRef(const StoreRef &sr)
: store(sr.store), mgr(sr.mgr)
{
if (store)
mgr.incrementReferenceCount(store);
}

inline StoreRef::~StoreRef() {
if (store)
mgr.decrementReferenceCount(store);
}

inline StoreRef &StoreRef::operator=(StoreRef const &newStore) {
assert(&newStore.mgr == &mgr);
if (store != newStore.store) {
mgr.incrementReferenceCount(newStore.store);
mgr.decrementReferenceCount(store);
store = newStore.getStore();
}
return *this;
}


std::unique_ptr<StoreManager>
CreateRegionStoreManager(ProgramStateManager &StMgr);
std::unique_ptr<StoreManager>
CreateFieldsOnlyRegionStoreManager(ProgramStateManager &StMgr);

}

}

#endif
