













#if !defined(LLVM_CLANG_ANALYSIS_RETAINSUMMARY_MANAGER_H)
#define LLVM_CLANG_ANALYSIS_RETAINSUMMARY_MANAGER_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/ImmutableMap.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/ParentMap.h"
#include "clang/Analysis/AnyCall.h"
#include "clang/Analysis/SelectorExtras.h"
#include "llvm/ADT/STLExtras.h"

using namespace clang;

namespace clang {
namespace ento {


enum class ObjKind {

CF,


ObjC,


AnyObj,


Generalized,



OS
};

enum ArgEffectKind {

DoNothing,



Autorelease,


Dealloc,


DecRef,



DecRefBridgedTransferred,


IncRef,



UnretainedOutParameter,



RetainedOutParameter,



RetainedOutParameterOnZero,



RetainedOutParameterOnNonZero,




MayEscape,



StopTracking,








StopTrackingHard,






DecRefAndStopTrackingHard,
};



class ArgEffect {
ArgEffectKind K;
ObjKind O;
public:
explicit ArgEffect(ArgEffectKind K = DoNothing, ObjKind O = ObjKind::AnyObj)
: K(K), O(O) {}

ArgEffectKind getKind() const { return K; }
ObjKind getObjKind() const { return O; }

ArgEffect withKind(ArgEffectKind NewK) {
return ArgEffect(NewK, O);
}

bool operator==(const ArgEffect &Other) const {
return K == Other.K && O == Other.O;
}
};



class RetEffect {
public:
enum Kind {


NoRet,


OwnedSymbol,




NotOwnedSymbol,



OwnedWhenTrackedReceiver,





NoRetHard
};

private:
Kind K;
ObjKind O;

RetEffect(Kind k, ObjKind o = ObjKind::AnyObj) : K(k), O(o) {}

public:
Kind getKind() const { return K; }

ObjKind getObjKind() const { return O; }

bool isOwned() const {
return K == OwnedSymbol || K == OwnedWhenTrackedReceiver;
}

bool notOwned() const {
return K == NotOwnedSymbol;
}

bool operator==(const RetEffect &Other) const {
return K == Other.K && O == Other.O;
}

static RetEffect MakeOwnedWhenTrackedReceiver() {
return RetEffect(OwnedWhenTrackedReceiver, ObjKind::ObjC);
}

static RetEffect MakeOwned(ObjKind o) {
return RetEffect(OwnedSymbol, o);
}
static RetEffect MakeNotOwned(ObjKind o) {
return RetEffect(NotOwnedSymbol, o);
}
static RetEffect MakeNoRet() {
return RetEffect(NoRet);
}
static RetEffect MakeNoRetHard() {
return RetEffect(NoRetHard);
}
};


class ObjCSummaryKey {
IdentifierInfo* II;
Selector S;
public:
ObjCSummaryKey(IdentifierInfo* ii, Selector s)
: II(ii), S(s) {}

ObjCSummaryKey(const ObjCInterfaceDecl *d, Selector s)
: II(d ? d->getIdentifier() : nullptr), S(s) {}

ObjCSummaryKey(Selector s)
: II(nullptr), S(s) {}

IdentifierInfo *getIdentifier() const { return II; }
Selector getSelector() const { return S; }
};

}
}

using namespace ento;

namespace llvm {




template <> struct FoldingSetTrait<ArgEffect> {
static inline void Profile(const ArgEffect X, FoldingSetNodeID &ID) {
ID.AddInteger((unsigned) X.getKind());
ID.AddInteger((unsigned) X.getObjKind());
}
};
template <> struct FoldingSetTrait<RetEffect> {
static inline void Profile(const RetEffect &X, FoldingSetNodeID &ID) {
ID.AddInteger((unsigned) X.getKind());
ID.AddInteger((unsigned) X.getObjKind());
}
};

template <> struct DenseMapInfo<ObjCSummaryKey> {
static inline ObjCSummaryKey getEmptyKey() {
return ObjCSummaryKey(DenseMapInfo<IdentifierInfo*>::getEmptyKey(),
DenseMapInfo<Selector>::getEmptyKey());
}

static inline ObjCSummaryKey getTombstoneKey() {
return ObjCSummaryKey(DenseMapInfo<IdentifierInfo*>::getTombstoneKey(),
DenseMapInfo<Selector>::getTombstoneKey());
}

static unsigned getHashValue(const ObjCSummaryKey &V) {
typedef std::pair<IdentifierInfo*, Selector> PairTy;
return DenseMapInfo<PairTy>::getHashValue(PairTy(V.getIdentifier(),
V.getSelector()));
}

static bool isEqual(const ObjCSummaryKey& LHS, const ObjCSummaryKey& RHS) {
return LHS.getIdentifier() == RHS.getIdentifier() &&
LHS.getSelector() == RHS.getSelector();
}

};

}


namespace clang {
namespace ento {



typedef llvm::ImmutableMap<unsigned, ArgEffect> ArgEffects;


class RetainSummary {



ArgEffects Args;



ArgEffect DefaultArgEffect;



ArgEffect Receiver;


ArgEffect This;



RetEffect Ret;

public:
RetainSummary(ArgEffects A,
RetEffect R,
ArgEffect defaultEff,
ArgEffect ReceiverEff,
ArgEffect ThisEff)
: Args(A), DefaultArgEffect(defaultEff), Receiver(ReceiverEff),
This(ThisEff), Ret(R) {}



ArgEffect getArg(unsigned idx) const {
if (const ArgEffect *AE = Args.lookup(idx))
return *AE;

return DefaultArgEffect;
}

void addArg(ArgEffects::Factory &af, unsigned idx, ArgEffect e) {
Args = af.add(Args, idx, e);
}


void setDefaultArgEffect(ArgEffect E) {
DefaultArgEffect = E;
}


RetEffect getRetEffect() const { return Ret; }


void setRetEffect(RetEffect E) { Ret = E; }



void setReceiverEffect(ArgEffect e) { Receiver = e; }



ArgEffect getReceiverEffect() const { return Receiver; }



ArgEffect getThisEffect() const { return This; }

ArgEffect getDefaultEffect() const { return DefaultArgEffect; }


void setThisEffect(ArgEffect e) { This = e; }

bool isNoop() const {
return Ret == RetEffect::MakeNoRet() && Receiver.getKind() == DoNothing
&& DefaultArgEffect.getKind() == MayEscape && This.getKind() == DoNothing
&& Args.isEmpty();
}




bool operator==(const RetainSummary &Other) const {
return Args == Other.Args && DefaultArgEffect == Other.DefaultArgEffect &&
Receiver == Other.Receiver && This == Other.This && Ret == Other.Ret;
}


void Profile(llvm::FoldingSetNodeID& ID) const {
ID.Add(Args);
ID.Add(DefaultArgEffect);
ID.Add(Receiver);
ID.Add(This);
ID.Add(Ret);
}


bool isSimple() const {
return Args.isEmpty();
}

ArgEffects getArgEffects() const { return Args; }

private:
ArgEffect getDefaultArgEffect() const { return DefaultArgEffect; }

friend class RetainSummaryManager;
};

class ObjCSummaryCache {
typedef llvm::DenseMap<ObjCSummaryKey, const RetainSummary *> MapTy;
MapTy M;
public:
ObjCSummaryCache() {}

const RetainSummary * find(const ObjCInterfaceDecl *D, Selector S) {


ObjCSummaryKey K(D, S);
MapTy::iterator I = M.find(K);

if (I != M.end())
return I->second;
if (!D)
return nullptr;







for (ObjCInterfaceDecl *C=D->getSuperClass() ;; C=C->getSuperClass()) {
if ((I = M.find(ObjCSummaryKey(C, S))) != M.end())
break;

if (!C)
return nullptr;
}



const RetainSummary *Summ = I->second;
M[K] = Summ;
return Summ;
}

const RetainSummary *find(IdentifierInfo* II, Selector S) {


MapTy::iterator I = M.find(ObjCSummaryKey(II, S));

if (I == M.end())
I = M.find(ObjCSummaryKey(S));

return I == M.end() ? nullptr : I->second;
}

const RetainSummary *& operator[](ObjCSummaryKey K) {
return M[K];
}

const RetainSummary *& operator[](Selector S) {
return M[ ObjCSummaryKey(S) ];
}
};

class RetainSummaryTemplate;

class RetainSummaryManager {
typedef llvm::DenseMap<const FunctionDecl*, const RetainSummary *>
FuncSummariesTy;

typedef ObjCSummaryCache ObjCMethodSummariesTy;

typedef llvm::FoldingSetNodeWrapper<RetainSummary> CachedSummaryNode;


ASTContext &Ctx;


const bool ARCEnabled;


const bool TrackObjCAndCFObjects;


const bool TrackOSObjects;


FuncSummariesTy FuncSummaries;



ObjCMethodSummariesTy ObjCClassMethodSummaries;


ObjCMethodSummariesTy ObjCMethodSummaries;



llvm::BumpPtrAllocator BPAlloc;


ArgEffects::Factory AF;



RetEffect ObjCAllocRetE;



RetEffect ObjCInitRetE;



llvm::FoldingSet<CachedSummaryNode> SimpleSummaries;


const RetainSummary *getOSSummaryCreateRule(const FunctionDecl *FD);


const RetainSummary *getOSSummaryGetRule(const FunctionDecl *FD);


const RetainSummary *getOSSummaryRetainRule(const FunctionDecl *FD);


const RetainSummary *getOSSummaryReleaseRule(const FunctionDecl *FD);


const RetainSummary *getOSSummaryFreeRule(const FunctionDecl *FD);

const RetainSummary *getUnarySummary(const FunctionType* FT,
ArgEffectKind AE);

const RetainSummary *getCFSummaryCreateRule(const FunctionDecl *FD);
const RetainSummary *getCFSummaryGetRule(const FunctionDecl *FD);
const RetainSummary *getCFCreateGetRuleSummary(const FunctionDecl *FD);

const RetainSummary *getPersistentSummary(const RetainSummary &OldSumm);

const RetainSummary *
getPersistentSummary(RetEffect RetEff, ArgEffects ScratchArgs,
ArgEffect ReceiverEff = ArgEffect(DoNothing),
ArgEffect DefaultEff = ArgEffect(MayEscape),
ArgEffect ThisEff = ArgEffect(DoNothing)) {
RetainSummary Summ(ScratchArgs, RetEff, DefaultEff, ReceiverEff, ThisEff);
return getPersistentSummary(Summ);
}

const RetainSummary *getDoNothingSummary() {
return getPersistentSummary(RetEffect::MakeNoRet(),
ArgEffects(AF.getEmptyMap()),
ArgEffect(DoNothing), ArgEffect(DoNothing));
}

const RetainSummary *getDefaultSummary() {
return getPersistentSummary(RetEffect::MakeNoRet(),
ArgEffects(AF.getEmptyMap()),
ArgEffect(DoNothing), ArgEffect(MayEscape));
}

const RetainSummary *getPersistentStopSummary() {
return getPersistentSummary(
RetEffect::MakeNoRet(), ArgEffects(AF.getEmptyMap()),
ArgEffect(StopTracking), ArgEffect(StopTracking));
}

void InitializeClassMethodSummaries();
void InitializeMethodSummaries();

void addNSObjectClsMethSummary(Selector S, const RetainSummary *Summ) {
ObjCClassMethodSummaries[S] = Summ;
}

void addNSObjectMethSummary(Selector S, const RetainSummary *Summ) {
ObjCMethodSummaries[S] = Summ;
}

void addClassMethSummary(const char* Cls, const char* name,
const RetainSummary *Summ, bool isNullary = true) {
IdentifierInfo* ClsII = &Ctx.Idents.get(Cls);
Selector S = isNullary ? GetNullarySelector(name, Ctx)
: GetUnarySelector(name, Ctx);
ObjCClassMethodSummaries[ObjCSummaryKey(ClsII, S)] = Summ;
}

void addInstMethSummary(const char* Cls, const char* nullaryName,
const RetainSummary *Summ) {
IdentifierInfo* ClsII = &Ctx.Idents.get(Cls);
Selector S = GetNullarySelector(nullaryName, Ctx);
ObjCMethodSummaries[ObjCSummaryKey(ClsII, S)] = Summ;
}

template <typename... Keywords>
void addMethodSummary(IdentifierInfo *ClsII, ObjCMethodSummariesTy &Summaries,
const RetainSummary *Summ, Keywords *... Kws) {
Selector S = getKeywordSelector(Ctx, Kws...);
Summaries[ObjCSummaryKey(ClsII, S)] = Summ;
}

template <typename... Keywords>
void addInstMethSummary(const char *Cls, const RetainSummary *Summ,
Keywords *... Kws) {
addMethodSummary(&Ctx.Idents.get(Cls), ObjCMethodSummaries, Summ, Kws...);
}

template <typename... Keywords>
void addClsMethSummary(const char *Cls, const RetainSummary *Summ,
Keywords *... Kws) {
addMethodSummary(&Ctx.Idents.get(Cls), ObjCClassMethodSummaries, Summ,
Kws...);
}

template <typename... Keywords>
void addClsMethSummary(IdentifierInfo *II, const RetainSummary *Summ,
Keywords *... Kws) {
addMethodSummary(II, ObjCClassMethodSummaries, Summ, Kws...);
}

const RetainSummary * generateSummary(const FunctionDecl *FD,
bool &AllowAnnotations);


const RetainSummary *getSummaryForOSObject(const FunctionDecl *FD,
StringRef FName, QualType RetTy);


const RetainSummary *getSummaryForObjCOrCFObject(
const FunctionDecl *FD,
StringRef FName,
QualType RetTy,
const FunctionType *FT,
bool &AllowAnnotations);




bool applyParamAnnotationEffect(const ParmVarDecl *pd, unsigned parm_idx,
const NamedDecl *FD,
RetainSummaryTemplate &Template);

public:
RetainSummaryManager(ASTContext &ctx, bool trackObjCAndCFObjects,
bool trackOSObjects)
: Ctx(ctx), ARCEnabled((bool)Ctx.getLangOpts().ObjCAutoRefCount),
TrackObjCAndCFObjects(trackObjCAndCFObjects),
TrackOSObjects(trackOSObjects), AF(BPAlloc),
ObjCAllocRetE(ARCEnabled ? RetEffect::MakeNotOwned(ObjKind::ObjC)
: RetEffect::MakeOwned(ObjKind::ObjC)),
ObjCInitRetE(ARCEnabled ? RetEffect::MakeNotOwned(ObjKind::ObjC)
: RetEffect::MakeOwnedWhenTrackedReceiver()) {
InitializeClassMethodSummaries();
InitializeMethodSummaries();
}

enum class BehaviorSummary {

NoOp,


Identity,


IdentityThis,


IdentityOrZero
};

Optional<BehaviorSummary> canEval(const CallExpr *CE, const FunctionDecl *FD,
bool &hasTrustedImplementationAnnotation);



static bool isKnownSmartPointer(QualType QT);

bool isTrustedReferenceCountImplementation(const Decl *FD);

const RetainSummary *getSummary(AnyCall C,
bool HasNonZeroCallbackArg=false,
bool IsReceiverUnconsumedSelf=false,
QualType ReceiverType={});

RetEffect getObjAllocRetEffect() const { return ObjCAllocRetE; }

private:



const RetainSummary *getMethodSummary(const ObjCMethodDecl *MD);

const RetainSummary *getFunctionSummary(const FunctionDecl *FD);

const RetainSummary *getMethodSummary(Selector S, const ObjCInterfaceDecl *ID,
const ObjCMethodDecl *MD,
QualType RetTy,
ObjCMethodSummariesTy &CachedSummaries);

const RetainSummary *
getInstanceMethodSummary(const ObjCMessageExpr *ME, QualType ReceiverType);

const RetainSummary *getClassMethodSummary(const ObjCMessageExpr *ME);

const RetainSummary *getStandardMethodSummary(const ObjCMethodDecl *MD,
Selector S, QualType RetTy);


Optional<RetEffect> getRetEffectFromAnnotations(QualType RetTy,
const Decl *D);

void updateSummaryFromAnnotations(const RetainSummary *&Summ,
const ObjCMethodDecl *MD);

void updateSummaryFromAnnotations(const RetainSummary *&Summ,
const FunctionDecl *FD);

const RetainSummary *updateSummaryForNonZeroCallbackArg(const RetainSummary *S,
AnyCall &C);













void updateSummaryForReceiverUnconsumedSelf(const RetainSummary *&S);


void updateSummaryForArgumentTypes(const AnyCall &C, const RetainSummary *&RS);








template <class T>
Optional<ObjKind> hasAnyEnabledAttrOf(const Decl *D, QualType QT);

template <class T1, class T2, class... Others>
Optional<ObjKind> hasAnyEnabledAttrOf(const Decl *D, QualType QT);

friend class RetainSummaryTemplate;
};






class RetainSummaryTemplate {
RetainSummaryManager &Manager;
const RetainSummary *&RealSummary;
RetainSummary ScratchSummary;
bool Accessed;
public:
RetainSummaryTemplate(const RetainSummary *&real, RetainSummaryManager &mgr)
: Manager(mgr), RealSummary(real), ScratchSummary(*real), Accessed(false) {}

~RetainSummaryTemplate() {
if (Accessed)
RealSummary = Manager.getPersistentSummary(ScratchSummary);
}

RetainSummary &operator*() {
Accessed = true;
return ScratchSummary;
}

RetainSummary *operator->() {
Accessed = true;
return &ScratchSummary;
}
};

}
}

#endif
