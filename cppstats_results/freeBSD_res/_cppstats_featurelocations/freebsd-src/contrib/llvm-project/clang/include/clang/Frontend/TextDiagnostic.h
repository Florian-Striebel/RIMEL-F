













#if !defined(LLVM_CLANG_FRONTEND_TEXTDIAGNOSTIC_H)
#define LLVM_CLANG_FRONTEND_TEXTDIAGNOSTIC_H

#include "clang/Frontend/DiagnosticRenderer.h"

namespace clang {













class TextDiagnostic : public DiagnosticRenderer {
raw_ostream &OS;

public:
TextDiagnostic(raw_ostream &OS,
const LangOptions &LangOpts,
DiagnosticOptions *DiagOpts);

~TextDiagnostic() override;








static void printDiagnosticLevel(raw_ostream &OS,
DiagnosticsEngine::Level Level,
bool ShowColors);

















static void printDiagnosticMessage(raw_ostream &OS, bool IsSupplemental,
StringRef Message, unsigned CurrentColumn,
unsigned Columns, bool ShowColors);

protected:
void emitDiagnosticMessage(FullSourceLoc Loc, PresumedLoc PLoc,
DiagnosticsEngine::Level Level, StringRef Message,
ArrayRef<CharSourceRange> Ranges,
DiagOrStoredDiag D) override;

void emitDiagnosticLoc(FullSourceLoc Loc, PresumedLoc PLoc,
DiagnosticsEngine::Level Level,
ArrayRef<CharSourceRange> Ranges) override;

void emitCodeContext(FullSourceLoc Loc, DiagnosticsEngine::Level Level,
SmallVectorImpl<CharSourceRange> &Ranges,
ArrayRef<FixItHint> Hints) override {
emitSnippetAndCaret(Loc, Level, Ranges, Hints);
}

void emitIncludeLocation(FullSourceLoc Loc, PresumedLoc PLoc) override;

void emitImportLocation(FullSourceLoc Loc, PresumedLoc PLoc,
StringRef ModuleName) override;

void emitBuildingModuleLocation(FullSourceLoc Loc, PresumedLoc PLoc,
StringRef ModuleName) override;

private:
void emitFilename(StringRef Filename, const SourceManager &SM);

void emitSnippetAndCaret(FullSourceLoc Loc, DiagnosticsEngine::Level Level,
SmallVectorImpl<CharSourceRange> &Ranges,
ArrayRef<FixItHint> Hints);

void emitSnippet(StringRef SourceLine);

void emitParseableFixits(ArrayRef<FixItHint> Hints, const SourceManager &SM);
};

}

#endif
