















#if !defined(LLVM_CLANG_ANALYSIS_ANALYSISDECLCONTEXT_H)
#define LLVM_CLANG_ANALYSIS_ANALYSISDECLCONTEXT_H

#include "clang/AST/DeclBase.h"
#include "clang/Analysis/BodyFarm.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CodeInjector.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Allocator.h"
#include <functional>
#include <memory>

namespace clang {

class AnalysisDeclContextManager;
class ASTContext;
class BlockDecl;
class BlockInvocationContext;
class CFGReverseBlockReachabilityAnalysis;
class CFGStmtMap;
class ImplicitParamDecl;
class LocationContext;
class LocationContextManager;
class ParentMap;
class StackFrameContext;
class Stmt;
class VarDecl;



class ManagedAnalysis {
protected:
ManagedAnalysis() = default;

public:
virtual ~ManagedAnalysis();











};



class AnalysisDeclContext {


AnalysisDeclContextManager *ADCMgr;

const Decl *const D;

std::unique_ptr<CFG> cfg, completeCFG;
std::unique_ptr<CFGStmtMap> cfgStmtMap;

CFG::BuildOptions cfgBuildOptions;
CFG::BuildOptions::ForcedBlkExprs *forcedBlkExprs = nullptr;

bool builtCFG = false;
bool builtCompleteCFG = false;
std::unique_ptr<ParentMap> PM;
std::unique_ptr<CFGReverseBlockReachabilityAnalysis> CFA;

llvm::BumpPtrAllocator A;

llvm::DenseMap<const BlockDecl *, void *> *ReferencedBlockVars = nullptr;

void *ManagedAnalyses = nullptr;

public:
AnalysisDeclContext(AnalysisDeclContextManager *Mgr, const Decl *D);

AnalysisDeclContext(AnalysisDeclContextManager *Mgr, const Decl *D,
const CFG::BuildOptions &BuildOptions);

~AnalysisDeclContext();

ASTContext &getASTContext() const { return D->getASTContext(); }

const Decl *getDecl() const { return D; }

AnalysisDeclContextManager *getManager() const { return ADCMgr; }

CFG::BuildOptions &getCFGBuildOptions() { return cfgBuildOptions; }

const CFG::BuildOptions &getCFGBuildOptions() const {
return cfgBuildOptions;
}




bool getAddEHEdges() const { return cfgBuildOptions.AddEHEdges; }
bool getUseUnoptimizedCFG() const {
return !cfgBuildOptions.PruneTriviallyFalseEdges;
}
bool getAddImplicitDtors() const { return cfgBuildOptions.AddImplicitDtors; }
bool getAddInitializers() const { return cfgBuildOptions.AddInitializers; }

void registerForcedBlockExpression(const Stmt *stmt);
const CFGBlock *getBlockForRegisteredExpression(const Stmt *stmt);


Stmt *getBody() const;




Stmt *getBody(bool &IsAutosynthesized) const;






bool isBodyAutosynthesized() const;







bool isBodyAutosynthesizedFromModelFile() const;

CFG *getCFG();

CFGStmtMap *getCFGStmtMap();

CFGReverseBlockReachabilityAnalysis *getCFGReachablityAnalysis();


CFG *getUnoptimizedCFG();

void dumpCFG(bool ShowColors);





bool isCFGBuilt() const { return builtCFG; }

ParentMap &getParentMap();

using referenced_decls_iterator = const VarDecl *const *;

llvm::iterator_range<referenced_decls_iterator>
getReferencedBlockVars(const BlockDecl *BD);



const ImplicitParamDecl *getSelfDecl() const;


const StackFrameContext *getStackFrame(LocationContext const *ParentLC,
const Stmt *S, const CFGBlock *Blk,
unsigned BlockCount, unsigned Index);


const BlockInvocationContext *
getBlockInvocationContext(const LocationContext *ParentLC,
const BlockDecl *BD, const void *Data);



template <typename T> T *getAnalysis() {
const void *tag = T::getTag();
std::unique_ptr<ManagedAnalysis> &data = getAnalysisImpl(tag);
if (!data)
data = T::create(*this);
return static_cast<T *>(data.get());
}


static bool isInStdNamespace(const Decl *D);

static std::string getFunctionName(const Decl *D);

private:
std::unique_ptr<ManagedAnalysis> &getAnalysisImpl(const void *tag);

LocationContextManager &getLocationContextManager();
};





class LocationContext : public llvm::FoldingSetNode {
public:
enum ContextKind { StackFrame, Block };

private:
ContextKind Kind;



AnalysisDeclContext *Ctx;

const LocationContext *Parent;
int64_t ID;

protected:
LocationContext(ContextKind k, AnalysisDeclContext *ctx,
const LocationContext *parent, int64_t ID)
: Kind(k), Ctx(ctx), Parent(parent), ID(ID) {}

public:
virtual ~LocationContext();

ContextKind getKind() const { return Kind; }

int64_t getID() const { return ID; }

AnalysisDeclContext *getAnalysisDeclContext() const { return Ctx; }

const LocationContext *getParent() const { return Parent; }

bool isParentOf(const LocationContext *LC) const;

const Decl *getDecl() const { return Ctx->getDecl(); }

CFG *getCFG() const { return Ctx->getCFG(); }

template <typename T> T *getAnalysis() const { return Ctx->getAnalysis<T>(); }

const ParentMap &getParentMap() const { return Ctx->getParentMap(); }


const ImplicitParamDecl *getSelfDecl() const { return Ctx->getSelfDecl(); }

const StackFrameContext *getStackFrame() const;


virtual bool inTopFrame() const;

virtual void Profile(llvm::FoldingSetNodeID &ID) = 0;




LLVM_DUMP_METHOD void dumpStack(raw_ostream &Out) const;












void printJson(
raw_ostream &Out, const char *NL = "\n", unsigned int Space = 0,
bool IsDot = false,
std::function<void(const LocationContext *)> printMoreInfoPerContext =
[](const LocationContext *) {}) const;

LLVM_DUMP_METHOD void dump() const;

static void ProfileCommon(llvm::FoldingSetNodeID &ID, ContextKind ck,
AnalysisDeclContext *ctx,
const LocationContext *parent, const void *data);
};


class StackFrameContext : public LocationContext {
friend class LocationContextManager;


const Stmt *CallSite;


const CFGBlock *Block;




const unsigned BlockCount;


const unsigned Index;

StackFrameContext(AnalysisDeclContext *ADC, const LocationContext *ParentLC,
const Stmt *S, const CFGBlock *Block, unsigned BlockCount,
unsigned Index, int64_t ID)
: LocationContext(StackFrame, ADC, ParentLC, ID), CallSite(S),
Block(Block), BlockCount(BlockCount), Index(Index) {}

public:
~StackFrameContext() override = default;

const Stmt *getCallSite() const { return CallSite; }

const CFGBlock *getCallSiteBlock() const { return Block; }

bool inTopFrame() const override { return getParent() == nullptr; }

unsigned getIndex() const { return Index; }

CFGElement getCallSiteCFGElement() const { return (*Block)[Index]; }

void Profile(llvm::FoldingSetNodeID &ID) override;

static void Profile(llvm::FoldingSetNodeID &ID, AnalysisDeclContext *ADC,
const LocationContext *ParentLC, const Stmt *S,
const CFGBlock *Block, unsigned BlockCount,
unsigned Index) {
ProfileCommon(ID, StackFrame, ADC, ParentLC, S);
ID.AddPointer(Block);
ID.AddInteger(BlockCount);
ID.AddInteger(Index);
}

static bool classof(const LocationContext *LC) {
return LC->getKind() == StackFrame;
}
};


class BlockInvocationContext : public LocationContext {
friend class LocationContextManager;

const BlockDecl *BD;


const void *Data;

BlockInvocationContext(AnalysisDeclContext *ADC,
const LocationContext *ParentLC, const BlockDecl *BD,
const void *Data, int64_t ID)
: LocationContext(Block, ADC, ParentLC, ID), BD(BD), Data(Data) {}

public:
~BlockInvocationContext() override = default;

const BlockDecl *getBlockDecl() const { return BD; }

const void *getData() const { return Data; }

void Profile(llvm::FoldingSetNodeID &ID) override;

static void Profile(llvm::FoldingSetNodeID &ID, AnalysisDeclContext *ADC,
const LocationContext *ParentLC, const BlockDecl *BD,
const void *Data) {
ProfileCommon(ID, Block, ADC, ParentLC, BD);
ID.AddPointer(Data);
}

static bool classof(const LocationContext *LC) {
return LC->getKind() == Block;
}
};

class LocationContextManager {
llvm::FoldingSet<LocationContext> Contexts;


int64_t NewID = 0;

public:
~LocationContextManager();










const StackFrameContext *getStackFrame(AnalysisDeclContext *ADC,
const LocationContext *ParentLC,
const Stmt *S, const CFGBlock *Block,
unsigned BlockCount, unsigned Index);







const BlockInvocationContext *
getBlockInvocationContext(AnalysisDeclContext *ADC,
const LocationContext *ParentLC,
const BlockDecl *BD, const void *Data);


void clear();
};

class AnalysisDeclContextManager {
using ContextMap =
llvm::DenseMap<const Decl *, std::unique_ptr<AnalysisDeclContext>>;

ContextMap Contexts;
LocationContextManager LocCtxMgr;
CFG::BuildOptions cfgBuildOptions;



std::unique_ptr<CodeInjector> Injector;



BodyFarm FunctionBodyFarm;



bool SynthesizeBodies;

public:
AnalysisDeclContextManager(
ASTContext &ASTCtx, bool useUnoptimizedCFG = false,
bool addImplicitDtors = false, bool addInitializers = false,
bool addTemporaryDtors = false, bool addLifetime = false,
bool addLoopExit = false, bool addScopes = false,
bool synthesizeBodies = false, bool addStaticInitBranches = false,
bool addCXXNewAllocator = true, bool addRichCXXConstructors = true,
bool markElidedCXXConstructors = true, bool addVirtualBaseBranches = true,
CodeInjector *injector = nullptr);

AnalysisDeclContext *getContext(const Decl *D);

bool getUseUnoptimizedCFG() const {
return !cfgBuildOptions.PruneTriviallyFalseEdges;
}

CFG::BuildOptions &getCFGBuildOptions() { return cfgBuildOptions; }


bool synthesizeBodies() const { return SynthesizeBodies; }




const StackFrameContext *getStackFrame(const Decl *D) {
return LocCtxMgr.getStackFrame(getContext(D), nullptr, nullptr, nullptr, 0,
0);
}


const StackFrameContext *getStackFrame(AnalysisDeclContext *ADC,
const LocationContext *Parent,
const Stmt *S, const CFGBlock *Block,
unsigned BlockCount, unsigned Index) {
return LocCtxMgr.getStackFrame(ADC, Parent, S, Block, BlockCount, Index);
}

BodyFarm &getBodyFarm();


void clear();

private:
friend class AnalysisDeclContext;

LocationContextManager &getLocationContextManager() { return LocCtxMgr; }
};

}

#endif
