












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_ANALYSISMANAGER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_ANALYSISMANAGER_H

#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/PathDiagnostic.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/StaticAnalyzer/Core/AnalyzerOptions.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"
#include "clang/StaticAnalyzer/Core/PathDiagnosticConsumers.h"

namespace clang {

class CodeInjector;

namespace ento {
class CheckerManager;

class AnalysisManager : public BugReporterData {
virtual void anchor();
AnalysisDeclContextManager AnaCtxMgr;

ASTContext &Ctx;
Preprocessor &PP;
const LangOptions &LangOpts;
PathDiagnosticConsumers PathConsumers;


StoreManagerCreator CreateStoreMgr;
ConstraintManagerCreator CreateConstraintMgr;

CheckerManager *CheckerMgr;

public:
AnalyzerOptions &options;

AnalysisManager(ASTContext &ctx, Preprocessor &PP,
const PathDiagnosticConsumers &Consumers,
StoreManagerCreator storemgr,
ConstraintManagerCreator constraintmgr,
CheckerManager *checkerMgr, AnalyzerOptions &Options,
CodeInjector *injector = nullptr);

~AnalysisManager() override;

void ClearContexts() {
AnaCtxMgr.clear();
}

AnalysisDeclContextManager& getAnalysisDeclContextManager() {
return AnaCtxMgr;
}

Preprocessor &getPreprocessor() override { return PP; }

StoreManagerCreator getStoreManagerCreator() {
return CreateStoreMgr;
}

AnalyzerOptions& getAnalyzerOptions() override {
return options;
}

ConstraintManagerCreator getConstraintManagerCreator() {
return CreateConstraintMgr;
}

CheckerManager *getCheckerManager() const { return CheckerMgr; }

ASTContext &getASTContext() override {
return Ctx;
}

SourceManager &getSourceManager() override {
return getASTContext().getSourceManager();
}

const LangOptions &getLangOpts() const {
return LangOpts;
}

ArrayRef<PathDiagnosticConsumer*> getPathDiagnosticConsumers() override {
return PathConsumers;
}

void FlushDiagnostics();

bool shouldVisualize() const {
return options.visualizeExplodedGraphWithGraphViz;
}

bool shouldInlineCall() const {
return options.getIPAMode() != IPAK_None;
}

CFG *getCFG(Decl const *D) {
return AnaCtxMgr.getContext(D)->getCFG();
}

template <typename T>
T *getAnalysis(Decl const *D) {
return AnaCtxMgr.getContext(D)->getAnalysis<T>();
}

ParentMap &getParentMap(Decl const *D) {
return AnaCtxMgr.getContext(D)->getParentMap();
}

AnalysisDeclContext *getAnalysisDeclContext(const Decl *D) {
return AnaCtxMgr.getContext(D);
}

static bool isInCodeFile(SourceLocation SL, const SourceManager &SM) {
if (SM.isInMainFile(SL))
return true;





SourceLocation IL = SM.getIncludeLoc(SM.getFileID(SL));
if (!IL.isValid() || !SM.isInMainFile(IL))
return false;


if (SM.getFilename(IL).contains("UnifiedSource")) {


return llvm::StringSwitch<bool>(SM.getFilename(SL).rsplit('.').second)
.Cases("c", "m", "mm", "C", "cc", "cp", true)
.Cases("cpp", "CPP", "c++", "cxx", "cppm", true)
.Default(false);
}

return false;
}

bool isInCodeFile(SourceLocation SL) {
const SourceManager &SM = getASTContext().getSourceManager();
return isInCodeFile(SL, SM);
}
};

}

}

#endif
