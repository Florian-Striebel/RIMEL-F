











#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_TAINT_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_TAINT_H

#include "clang/StaticAnalyzer/Core/BugReporter/BugReporterVisitors.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang {
namespace ento {
namespace taint {



using TaintTagType = unsigned;

static constexpr TaintTagType TaintTagGeneric = 0;


LLVM_NODISCARD ProgramStateRef addTaint(ProgramStateRef State, const Stmt *S,
const LocationContext *LCtx,
TaintTagType Kind = TaintTagGeneric);


LLVM_NODISCARD ProgramStateRef addTaint(ProgramStateRef State, SVal V,
TaintTagType Kind = TaintTagGeneric);


LLVM_NODISCARD ProgramStateRef addTaint(ProgramStateRef State, SymbolRef Sym,
TaintTagType Kind = TaintTagGeneric);



LLVM_NODISCARD ProgramStateRef addTaint(ProgramStateRef State,
const MemRegion *R,
TaintTagType Kind = TaintTagGeneric);

LLVM_NODISCARD ProgramStateRef removeTaint(ProgramStateRef State, SVal V);

LLVM_NODISCARD ProgramStateRef removeTaint(ProgramStateRef State,
const MemRegion *R);

LLVM_NODISCARD ProgramStateRef removeTaint(ProgramStateRef State,
SymbolRef Sym);





LLVM_NODISCARD ProgramStateRef addPartialTaint(
ProgramStateRef State, SymbolRef ParentSym, const SubRegion *SubRegion,
TaintTagType Kind = TaintTagGeneric);


bool isTainted(ProgramStateRef State, const Stmt *S,
const LocationContext *LCtx,
TaintTagType Kind = TaintTagGeneric);


bool isTainted(ProgramStateRef State, SVal V,
TaintTagType Kind = TaintTagGeneric);


bool isTainted(ProgramStateRef State, SymbolRef Sym,
TaintTagType Kind = TaintTagGeneric);



bool isTainted(ProgramStateRef State, const MemRegion *Reg,
TaintTagType Kind = TaintTagGeneric);

void printTaint(ProgramStateRef State, raw_ostream &Out, const char *nl = "\n",
const char *sep = "");

LLVM_DUMP_METHOD void dumpTaint(ProgramStateRef State);



class TaintBugVisitor final : public BugReporterVisitor {
private:
const SVal V;

public:
TaintBugVisitor(const SVal V) : V(V) {}
void Profile(llvm::FoldingSetNodeID &ID) const override { ID.Add(V); }

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;
};

}
}
}

#endif
