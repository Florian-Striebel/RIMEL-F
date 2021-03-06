







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICSERIALIZATION_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICSERIALIZATION_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define SERIALIZATIONSTART
#include "clang/Basic/DiagnosticSerializationKinds.inc"
#undef DIAG
NUM_BUILTIN_SERIALIZATION_DIAGNOSTICS
};
}
}

#endif
