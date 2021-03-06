







#if !defined(LLVM_CLANG_TOOLING_REFACTORING_EXTRACT_SOURCE_EXTRACTION_H)
#define LLVM_CLANG_TOOLING_REFACTORING_EXTRACT_SOURCE_EXTRACTION_H

#include "clang/Basic/LLVM.h"

namespace clang {

class LangOptions;
class SourceManager;
class SourceRange;
class Stmt;

namespace tooling {


class ExtractionSemicolonPolicy {
public:
bool isNeededInExtractedFunction() const {
return IsNeededInExtractedFunction;
}

bool isNeededInOriginalFunction() const { return IsNeededInOriginalFunction; }



static ExtractionSemicolonPolicy compute(const Stmt *S,
SourceRange &ExtractedRange,
const SourceManager &SM,
const LangOptions &LangOpts);

private:
ExtractionSemicolonPolicy(bool IsNeededInExtractedFunction,
bool IsNeededInOriginalFunction)
: IsNeededInExtractedFunction(IsNeededInExtractedFunction),
IsNeededInOriginalFunction(IsNeededInOriginalFunction) {}
bool IsNeededInExtractedFunction;
bool IsNeededInOriginalFunction;
};

}
}

#endif
