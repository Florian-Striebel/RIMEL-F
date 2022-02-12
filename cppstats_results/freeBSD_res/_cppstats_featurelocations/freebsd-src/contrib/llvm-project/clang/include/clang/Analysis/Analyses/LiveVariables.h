











#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_LIVEVARIABLES_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_LIVEVARIABLES_H

#include "clang/AST/Decl.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "llvm/ADT/ImmutableSet.h"

namespace clang {

class CFG;
class CFGBlock;
class Stmt;
class DeclRefExpr;
class SourceManager;

class LiveVariables : public ManagedAnalysis {
public:
class LivenessValues {
public:

llvm::ImmutableSet<const Expr *> liveExprs;
llvm::ImmutableSet<const VarDecl *> liveDecls;
llvm::ImmutableSet<const BindingDecl *> liveBindings;

bool equals(const LivenessValues &V) const;

LivenessValues()
: liveExprs(nullptr), liveDecls(nullptr), liveBindings(nullptr) {}

LivenessValues(llvm::ImmutableSet<const Expr *> liveExprs,
llvm::ImmutableSet<const VarDecl *> LiveDecls,
llvm::ImmutableSet<const BindingDecl *> LiveBindings)
: liveExprs(liveExprs), liveDecls(LiveDecls),
liveBindings(LiveBindings) {}

bool isLive(const Expr *E) const;
bool isLive(const VarDecl *D) const;

friend class LiveVariables;
};

class Observer {
virtual void anchor();
public:
virtual ~Observer() {}



virtual void observeStmt(const Stmt *S,
const CFGBlock *currentBlock,
const LivenessValues& V) {}



virtual void observerKill(const DeclRefExpr *DR) {}
};

~LiveVariables() override;


static std::unique_ptr<LiveVariables>
computeLiveness(AnalysisDeclContext &analysisContext, bool killAtAssign);



bool isLive(const CFGBlock *B, const VarDecl *D);





bool isLive(const Stmt *S, const VarDecl *D);



bool isLive(const Stmt *Loc, const Expr *Val);



void dumpBlockLiveness(const SourceManager &M);



void dumpExprLiveness(const SourceManager &M);

void runOnAllBlocks(Observer &obs);

static std::unique_ptr<LiveVariables>
create(AnalysisDeclContext &analysisContext) {
return computeLiveness(analysisContext, true);
}

static const void *getTag();

private:
LiveVariables(void *impl);
void *impl;
};

class RelaxedLiveVariables : public LiveVariables {
public:
static std::unique_ptr<LiveVariables>
create(AnalysisDeclContext &analysisContext) {
return computeLiveness(analysisContext, false);
}

static const void *getTag();
};

}

#endif
