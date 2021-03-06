







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICCOMMENT_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICCOMMENT_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define COMMENTSTART
#include "clang/Basic/DiagnosticCommentKinds.inc"
#undef DIAG
NUM_BUILTIN_COMMENT_DIAGNOSTICS
};
}
}

#endif
