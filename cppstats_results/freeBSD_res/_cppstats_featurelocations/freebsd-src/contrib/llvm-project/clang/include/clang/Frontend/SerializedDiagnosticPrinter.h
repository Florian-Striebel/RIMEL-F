







#if !defined(LLVM_CLANG_FRONTEND_SERIALIZEDDIAGNOSTICPRINTER_H)
#define LLVM_CLANG_FRONTEND_SERIALIZEDDIAGNOSTICPRINTER_H

#include "clang/Basic/LLVM.h"
#include "clang/Frontend/SerializedDiagnostics.h"
#include "llvm/Bitstream/BitstreamWriter.h"

namespace llvm {
class raw_ostream;
}

namespace clang {
class DiagnosticConsumer;
class DiagnosticsEngine;
class DiagnosticOptions;

namespace serialized_diags {









std::unique_ptr<DiagnosticConsumer> create(StringRef OutputFile,
DiagnosticOptions *Diags,
bool MergeChildRecords = false);

}
}

#endif
