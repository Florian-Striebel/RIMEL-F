












#if !defined(LLVM_CLANG_FRONTEND_TEXTDIAGNOSTICPRINTER_H)
#define LLVM_CLANG_FRONTEND_TEXTDIAGNOSTICPRINTER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include <memory>

namespace clang {
class DiagnosticOptions;
class LangOptions;
class TextDiagnostic;

class TextDiagnosticPrinter : public DiagnosticConsumer {
raw_ostream &OS;
IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;


std::unique_ptr<TextDiagnostic> TextDiag;


std::string Prefix;

unsigned OwnsOutputStream : 1;

public:
TextDiagnosticPrinter(raw_ostream &os, DiagnosticOptions *diags,
bool OwnsOutputStream = false);
~TextDiagnosticPrinter() override;




void setPrefix(std::string Value) { Prefix = std::move(Value); }

void BeginSourceFile(const LangOptions &LO, const Preprocessor *PP) override;
void EndSourceFile() override;
void HandleDiagnostic(DiagnosticsEngine::Level Level,
const Diagnostic &Info) override;
};

}

#endif
