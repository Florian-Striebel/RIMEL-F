







#if !defined(LLVM_CLANG_ANALYSIS_MACROEXPANSIONCONTEXT_H)
#define LLVM_CLANG_ANALYSIS_MACROEXPANSIONCONTEXT_H

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {

namespace detail {
class MacroExpansionRangeRecorder;
}
















































class MacroExpansionContext {
public:



explicit MacroExpansionContext(const LangOptions &LangOpts);




void registerForPreprocessor(Preprocessor &PP);





Optional<StringRef> getExpandedText(SourceLocation MacroExpansionLoc) const;





Optional<StringRef> getOriginalText(SourceLocation MacroExpansionLoc) const;

LLVM_DUMP_METHOD void dumpExpansionRangesToStream(raw_ostream &OS) const;
LLVM_DUMP_METHOD void dumpExpandedTextsToStream(raw_ostream &OS) const;
LLVM_DUMP_METHOD void dumpExpansionRanges() const;
LLVM_DUMP_METHOD void dumpExpandedTexts() const;

private:
friend class detail::MacroExpansionRangeRecorder;
using MacroExpansionText = SmallString<40>;
using ExpansionMap = llvm::DenseMap<SourceLocation, MacroExpansionText>;
using ExpansionRangeMap = llvm::DenseMap<SourceLocation, SourceLocation>;



ExpansionMap ExpandedTokens;



ExpansionRangeMap ExpansionRanges;

Preprocessor *PP = nullptr;
SourceManager *SM = nullptr;
const LangOptions &LangOpts;




void onTokenLexed(const Token &Tok);
};
}

#endif
