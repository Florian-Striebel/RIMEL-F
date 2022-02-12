











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHDIAGNOSTICCONSUMERS_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHDIAGNOSTICCONSUMERS_H

#include "clang/Analysis/PathDiagnostic.h"

#include <string>
#include <vector>

namespace clang {

class AnalyzerOptions;
class MacroExpansionContext;
class Preprocessor;

namespace cross_tu {
class CrossTranslationUnitContext;
}

namespace ento {

class PathDiagnosticConsumer;
typedef std::vector<PathDiagnosticConsumer*> PathDiagnosticConsumers;

#define ANALYSIS_DIAGNOSTICS(NAME, CMDFLAG, DESC, CREATEFN) void CREATEFN(PathDiagnosticConsumerOptions Diagopts, PathDiagnosticConsumers &C, const std::string &Prefix, const Preprocessor &PP, const cross_tu::CrossTranslationUnitContext &CTU, const MacroExpansionContext &MacroExpansions);





#include "clang/StaticAnalyzer/Core/Analyses.def"

}
}

#endif
