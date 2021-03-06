














#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MPICHECKER_MPITYPES_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MPICHECKER_MPITYPES_H

#include "clang/StaticAnalyzer/Checkers/MPIFunctionClassifier.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"
#include "llvm/ADT/SmallSet.h"

namespace clang {
namespace ento {
namespace mpi {

class Request {
public:
enum State : unsigned char { Nonblocking, Wait };

Request(State S) : CurrentState{S} {}

void Profile(llvm::FoldingSetNodeID &Id) const {
Id.AddInteger(CurrentState);
}

bool operator==(const Request &ToCompare) const {
return CurrentState == ToCompare.CurrentState;
}

const State CurrentState;
};





struct RequestMap {};
typedef llvm::ImmutableMap<const clang::ento::MemRegion *,
clang::ento::mpi::Request>
RequestMapImpl;

}

template <>
struct ProgramStateTrait<mpi::RequestMap>
: public ProgramStatePartialTrait<mpi::RequestMapImpl> {
static void *GDMIndex() {
static int index = 0;
return &index;
}
};

}
}
#endif
