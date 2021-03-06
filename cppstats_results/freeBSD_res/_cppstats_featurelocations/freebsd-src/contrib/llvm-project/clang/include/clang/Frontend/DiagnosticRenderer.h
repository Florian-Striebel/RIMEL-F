













#if !defined(LLVM_CLANG_FRONTEND_DIAGNOSTICRENDERER_H)
#define LLVM_CLANG_FRONTEND_DIAGNOSTICRENDERER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"

namespace clang {

class LangOptions;
class SourceManager;

using DiagOrStoredDiag =
llvm::PointerUnion<const Diagnostic *, const StoredDiagnostic *>;













class DiagnosticRenderer {
protected:
const LangOptions &LangOpts;
IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;






SourceLocation LastLoc;





SourceLocation LastIncludeLoc;





DiagnosticsEngine::Level LastLevel = DiagnosticsEngine::Ignored;

DiagnosticRenderer(const LangOptions &LangOpts,
DiagnosticOptions *DiagOpts);

virtual ~DiagnosticRenderer();

virtual void emitDiagnosticMessage(FullSourceLoc Loc, PresumedLoc PLoc,
DiagnosticsEngine::Level Level,
StringRef Message,
ArrayRef<CharSourceRange> Ranges,
DiagOrStoredDiag Info) = 0;

virtual void emitDiagnosticLoc(FullSourceLoc Loc, PresumedLoc PLoc,
DiagnosticsEngine::Level Level,
ArrayRef<CharSourceRange> Ranges) = 0;

virtual void emitCodeContext(FullSourceLoc Loc,
DiagnosticsEngine::Level Level,
SmallVectorImpl<CharSourceRange> &Ranges,
ArrayRef<FixItHint> Hints) = 0;

virtual void emitIncludeLocation(FullSourceLoc Loc, PresumedLoc PLoc) = 0;
virtual void emitImportLocation(FullSourceLoc Loc, PresumedLoc PLoc,
StringRef ModuleName) = 0;
virtual void emitBuildingModuleLocation(FullSourceLoc Loc, PresumedLoc PLoc,
StringRef ModuleName) = 0;

virtual void beginDiagnostic(DiagOrStoredDiag D,
DiagnosticsEngine::Level Level) {}
virtual void endDiagnostic(DiagOrStoredDiag D,
DiagnosticsEngine::Level Level) {}

private:
void emitBasicNote(StringRef Message);
void emitIncludeStack(FullSourceLoc Loc, PresumedLoc PLoc,
DiagnosticsEngine::Level Level);
void emitIncludeStackRecursively(FullSourceLoc Loc);
void emitImportStack(FullSourceLoc Loc);
void emitImportStackRecursively(FullSourceLoc Loc, StringRef ModuleName);
void emitModuleBuildStack(const SourceManager &SM);
void emitCaret(FullSourceLoc Loc, DiagnosticsEngine::Level Level,
ArrayRef<CharSourceRange> Ranges, ArrayRef<FixItHint> Hints);
void emitSingleMacroExpansion(FullSourceLoc Loc,
DiagnosticsEngine::Level Level,
ArrayRef<CharSourceRange> Ranges);
void emitMacroExpansions(FullSourceLoc Loc, DiagnosticsEngine::Level Level,
ArrayRef<CharSourceRange> Ranges,
ArrayRef<FixItHint> Hints);

public:












void emitDiagnostic(FullSourceLoc Loc, DiagnosticsEngine::Level Level,
StringRef Message, ArrayRef<CharSourceRange> Ranges,
ArrayRef<FixItHint> FixItHints,
DiagOrStoredDiag D = (Diagnostic *)nullptr);

void emitStoredDiagnostic(StoredDiagnostic &Diag);
};



class DiagnosticNoteRenderer : public DiagnosticRenderer {
public:
DiagnosticNoteRenderer(const LangOptions &LangOpts,
DiagnosticOptions *DiagOpts)
: DiagnosticRenderer(LangOpts, DiagOpts) {}

~DiagnosticNoteRenderer() override;

void emitIncludeLocation(FullSourceLoc Loc, PresumedLoc PLoc) override;

void emitImportLocation(FullSourceLoc Loc, PresumedLoc PLoc,
StringRef ModuleName) override;

void emitBuildingModuleLocation(FullSourceLoc Loc, PresumedLoc PLoc,
StringRef ModuleName) override;

virtual void emitNote(FullSourceLoc Loc, StringRef Message) = 0;
};

}

#endif
