











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_ENVIRONMENT_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_ENVIRONMENT_H

#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "llvm/ADT/ImmutableMap.h"
#include <utility>

namespace clang {

class Stmt;

namespace ento {

class SValBuilder;
class SymbolReaper;





class EnvironmentEntry : public std::pair<const Stmt *,
const StackFrameContext *> {
public:
EnvironmentEntry(const Stmt *s, const LocationContext *L);

const Stmt *getStmt() const { return first; }
const LocationContext *getLocationContext() const { return second; }


static void Profile(llvm::FoldingSetNodeID &ID,
const EnvironmentEntry &E) {
ID.AddPointer(E.getStmt());
ID.AddPointer(E.getLocationContext());
}

void Profile(llvm::FoldingSetNodeID &ID) const {
Profile(ID, *this);
}
};


class Environment {
private:
friend class EnvironmentManager;

using BindingsTy = llvm::ImmutableMap<EnvironmentEntry, SVal>;

BindingsTy ExprBindings;

Environment(BindingsTy eb) : ExprBindings(eb) {}

SVal lookupExpr(const EnvironmentEntry &E) const;

public:
using iterator = BindingsTy::iterator;

iterator begin() const { return ExprBindings.begin(); }
iterator end() const { return ExprBindings.end(); }



SVal getSVal(const EnvironmentEntry &E, SValBuilder &svalBuilder) const;



static void Profile(llvm::FoldingSetNodeID& ID, const Environment* env) {
env->ExprBindings.Profile(ID);
}



void Profile(llvm::FoldingSetNodeID& ID) const {
Profile(ID, this);
}

bool operator==(const Environment& RHS) const {
return ExprBindings == RHS.ExprBindings;
}

void printJson(raw_ostream &Out, const ASTContext &Ctx,
const LocationContext *LCtx = nullptr, const char *NL = "\n",
unsigned int Space = 0, bool IsDot = false) const;
};

class EnvironmentManager {
private:
using FactoryTy = Environment::BindingsTy::Factory;

FactoryTy F;

public:
EnvironmentManager(llvm::BumpPtrAllocator &Allocator) : F(Allocator) {}

Environment getInitialEnvironment() {
return Environment(F.getEmptyMap());
}


Environment bindExpr(Environment Env, const EnvironmentEntry &E, SVal V,
bool Invalidate);

Environment removeDeadBindings(Environment Env,
SymbolReaper &SymReaper,
ProgramStateRef state);
};

}

}

#endif
