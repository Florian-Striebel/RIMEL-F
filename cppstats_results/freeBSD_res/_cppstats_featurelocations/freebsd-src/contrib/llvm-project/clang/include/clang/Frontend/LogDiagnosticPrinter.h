







#if !defined(LLVM_CLANG_FRONTEND_LOGDIAGNOSTICPRINTER_H)
#define LLVM_CLANG_FRONTEND_LOGDIAGNOSTICPRINTER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
class DiagnosticOptions;
class LangOptions;

class LogDiagnosticPrinter : public DiagnosticConsumer {
struct DiagEntry {

std::string Message;


std::string Filename;


unsigned Line;


unsigned Column;


unsigned DiagnosticID;


std::string WarningOption;


DiagnosticsEngine::Level DiagnosticLevel;
};

void EmitDiagEntry(llvm::raw_ostream &OS,
const LogDiagnosticPrinter::DiagEntry &DE);




raw_ostream &OS;
std::unique_ptr<raw_ostream> StreamOwner;
const LangOptions *LangOpts;
IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;

SourceLocation LastWarningLoc;
FullSourceLoc LastLoc;

SmallVector<DiagEntry, 8> Entries;

std::string MainFilename;
std::string DwarfDebugFlags;

public:
LogDiagnosticPrinter(raw_ostream &OS, DiagnosticOptions *Diags,
std::unique_ptr<raw_ostream> StreamOwner);

void setDwarfDebugFlags(StringRef Value) {
DwarfDebugFlags = std::string(Value);
}

void BeginSourceFile(const LangOptions &LO, const Preprocessor *PP) override {
LangOpts = &LO;
}

void EndSourceFile() override;

void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
const Diagnostic &Info) override;
};

}

#endif
