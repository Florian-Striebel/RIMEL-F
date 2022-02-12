












#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_UNINITIALIZEDVALUES_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_UNINITIALIZEDVALUES_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {

class AnalysisDeclContext;
class CFG;
class DeclContext;
class Expr;
class Stmt;
class VarDecl;


class UninitUse {
public:
struct Branch {
const Stmt *Terminator;
unsigned Output;
};

private:

const Expr *User;


bool UninitAfterCall = false;


bool UninitAfterDecl = false;


bool AlwaysUninit;



SmallVector<Branch, 2> UninitBranches;

public:
UninitUse(const Expr *User, bool AlwaysUninit)
: User(User), AlwaysUninit(AlwaysUninit) {}

void addUninitBranch(Branch B) {
UninitBranches.push_back(B);
}

void setUninitAfterCall() { UninitAfterCall = true; }
void setUninitAfterDecl() { UninitAfterDecl = true; }


const Expr *getUser() const { return User; }


enum Kind {

Maybe,


Sometimes,



AfterDecl,



AfterCall,


Always
};


Kind getKind() const {
return AlwaysUninit ? Always :
UninitAfterCall ? AfterCall :
UninitAfterDecl ? AfterDecl :
!branch_empty() ? Sometimes : Maybe;
}

using branch_iterator = SmallVectorImpl<Branch>::const_iterator;


branch_iterator branch_begin() const { return UninitBranches.begin(); }
branch_iterator branch_end() const { return UninitBranches.end(); }
bool branch_empty() const { return UninitBranches.empty(); }
};

class UninitVariablesHandler {
public:
UninitVariablesHandler() = default;
virtual ~UninitVariablesHandler();


virtual void handleUseOfUninitVariable(const VarDecl *vd,
const UninitUse &use) {}


virtual void handleConstRefUseOfUninitVariable(const VarDecl *vd,
const UninitUse &use) {}




virtual void handleSelfInit(const VarDecl *vd) {}
};

struct UninitVariablesAnalysisStats {
unsigned NumVariablesAnalyzed;
unsigned NumBlockVisits;
};

void runUninitializedVariablesAnalysis(const DeclContext &dc, const CFG &cfg,
AnalysisDeclContext &ac,
UninitVariablesHandler &handler,
UninitVariablesAnalysisStats &stats);

}

#endif
