













#if !defined(LLVM_CLANG_LIB_FORMAT_USINGDECLARATIONSSORTER_H)
#define LLVM_CLANG_LIB_FORMAT_USINGDECLARATIONSSORTER_H

#include "TokenAnalyzer.h"

namespace clang {
namespace format {

class UsingDeclarationsSorter : public TokenAnalyzer {
public:
UsingDeclarationsSorter(const Environment &Env, const FormatStyle &Style);

std::pair<tooling::Replacements, unsigned>
analyze(TokenAnnotator &Annotator,
SmallVectorImpl<AnnotatedLine *> &AnnotatedLines,
FormatTokenLexer &Tokens) override;
};

}
}

#endif
