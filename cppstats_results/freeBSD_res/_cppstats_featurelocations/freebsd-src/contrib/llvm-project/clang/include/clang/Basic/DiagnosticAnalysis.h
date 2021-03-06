







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICANALYSIS_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICANALYSIS_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define ANALYSISSTART
#include "clang/Basic/DiagnosticAnalysisKinds.inc"
#undef DIAG
NUM_BUILTIN_ANALYSIS_DIAGNOSTICS
};
}
}

#endif
