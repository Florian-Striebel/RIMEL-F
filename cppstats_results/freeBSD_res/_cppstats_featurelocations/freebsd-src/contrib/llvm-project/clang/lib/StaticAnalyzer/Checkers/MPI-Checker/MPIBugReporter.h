













#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MPICHECKER_MPIBUGREPORTER_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MPICHECKER_MPIBUGREPORTER_H

#include "MPITypes.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"

namespace clang {
namespace ento {
namespace mpi {

class MPIBugReporter {
public:
MPIBugReporter(const CheckerBase &CB) {
UnmatchedWaitBugType.reset(new BugType(&CB, "Unmatched wait", MPIError));
DoubleNonblockingBugType.reset(
new BugType(&CB, "Double nonblocking", MPIError));
MissingWaitBugType.reset(new BugType(&CB, "Missing wait", MPIError));
}









void reportDoubleNonblocking(const CallEvent &MPICallEvent,
const Request &Req,
const MemRegion *const RequestRegion,
const ExplodedNode *const ExplNode,
BugReporter &BReporter) const;







void reportMissingWait(const Request &Req,
const MemRegion *const RequestRegion,
const ExplodedNode *const ExplNode,
BugReporter &BReporter) const;







void reportUnmatchedWait(const CallEvent &CE,
const MemRegion *const RequestRegion,
const ExplodedNode *const ExplNode,
BugReporter &BReporter) const;

private:
const std::string MPIError = "MPI Error";


std::unique_ptr<BugType> UnmatchedWaitBugType;
std::unique_ptr<BugType> MissingWaitBugType;
std::unique_ptr<BugType> DoubleNonblockingBugType;



class RequestNodeVisitor : public BugReporterVisitor {
public:
RequestNodeVisitor(const MemRegion *const MemoryRegion,
const std::string &ErrText)
: RequestRegion(MemoryRegion), ErrorText(ErrText) {}

void Profile(llvm::FoldingSetNodeID &ID) const override {
static int X = 0;
ID.AddPointer(&X);
ID.AddPointer(RequestRegion);
}

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;

private:
const MemRegion *const RequestRegion;
bool IsNodeFound = false;
std::string ErrorText;
};
};

}
}
}

#endif
