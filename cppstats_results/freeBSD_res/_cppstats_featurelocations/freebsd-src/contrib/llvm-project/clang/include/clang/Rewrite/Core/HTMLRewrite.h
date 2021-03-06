












#if !defined(LLVM_CLANG_REWRITE_CORE_HTMLREWRITE_H)
#define LLVM_CLANG_REWRITE_CORE_HTMLREWRITE_H

#include "clang/Basic/SourceLocation.h"
#include <string>

namespace clang {

class Rewriter;
class RewriteBuffer;
class Preprocessor;

namespace html {





void HighlightRange(Rewriter &R, SourceLocation B, SourceLocation E,
const char *StartTag, const char *EndTag,
bool IsTokenRange = true);





inline void HighlightRange(Rewriter &R, SourceRange Range,
const char *StartTag, const char *EndTag) {
HighlightRange(R, Range.getBegin(), Range.getEnd(), StartTag, EndTag);
}



void HighlightRange(RewriteBuffer &RB, unsigned B, unsigned E,
const char *BufferStart,
const char *StartTag, const char *EndTag);



void EscapeText(Rewriter& R, FileID FID,
bool EscapeSpaces = false, bool ReplaceTabs = false);





std::string EscapeText(StringRef s,
bool EscapeSpaces = false, bool ReplaceTabs = false);

void AddLineNumbers(Rewriter& R, FileID FID);

void AddHeaderFooterInternalBuiltinCSS(Rewriter &R, FileID FID,
StringRef title);



void SyntaxHighlight(Rewriter &R, FileID FID, const Preprocessor &PP);





void HighlightMacros(Rewriter &R, FileID FID, const Preprocessor &PP);

}
}

#endif
