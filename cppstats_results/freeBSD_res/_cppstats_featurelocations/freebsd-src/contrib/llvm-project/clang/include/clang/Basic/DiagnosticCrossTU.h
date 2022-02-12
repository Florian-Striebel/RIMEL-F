







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICCROSSTU_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICCROSSTU_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define CROSSTUSTART
#include "clang/Basic/DiagnosticCrossTUKinds.inc"
#undef DIAG
NUM_BUILTIN_CROSSTU_DIAGNOSTICS
};
}
}

#endif
