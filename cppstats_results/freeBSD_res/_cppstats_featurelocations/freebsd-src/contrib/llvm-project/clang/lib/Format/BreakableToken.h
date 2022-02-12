















#if !defined(LLVM_CLANG_LIB_FORMAT_BREAKABLETOKEN_H)
#define LLVM_CLANG_LIB_FORMAT_BREAKABLETOKEN_H

#include "Encoding.h"
#include "TokenAnnotator.h"
#include "WhitespaceManager.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Regex.h"
#include <utility>

namespace clang {
namespace format {



bool switchesFormatting(const FormatToken &Token);

struct FormatStyle;






















































class BreakableToken {
public:

typedef std::pair<StringRef::size_type, unsigned> Split;

virtual ~BreakableToken() {}


virtual unsigned getLineCount() const = 0;









virtual unsigned getRangeLength(unsigned LineIndex, unsigned Offset,
StringRef::size_type Length,
unsigned StartColumn) const = 0;













virtual unsigned getRemainingLength(unsigned LineIndex, unsigned Offset,
unsigned StartColumn) const {
return getRangeLength(LineIndex, Offset, StringRef::npos, StartColumn);
}








virtual unsigned getContentStartColumn(unsigned LineIndex,
bool Break) const = 0;












virtual unsigned getContentIndent(unsigned LineIndex) const { return 0; }






virtual Split getSplit(unsigned LineIndex, unsigned TailOffset,
unsigned ColumnLimit, unsigned ContentStartColumn,
const llvm::Regex &CommentPragmasRegex) const = 0;


virtual void insertBreak(unsigned LineIndex, unsigned TailOffset, Split Split,
unsigned ContentIndent,
WhitespaceManager &Whitespaces) const = 0;





unsigned getLengthAfterCompression(unsigned RemainingTokenColumns,
Split Split) const;



virtual void compressWhitespace(unsigned LineIndex, unsigned TailOffset,
Split Split,
WhitespaceManager &Whitespaces) const = 0;


virtual bool supportsReflow() const { return false; }












virtual Split getReflowSplit(unsigned LineIndex,
const llvm::Regex &CommentPragmasRegex) const {
return Split(StringRef::npos, 0);
}


virtual void reflow(unsigned LineIndex,
WhitespaceManager &Whitespaces) const {}



virtual bool introducesBreakBeforeToken() const { return false; }


virtual void adaptStartOfLine(unsigned LineIndex,
WhitespaceManager &Whitespaces) const {}







virtual Split getSplitAfterLastLine(unsigned TailOffset) const {
return Split(StringRef::npos, 0);
}



void replaceWhitespaceAfterLastLine(unsigned TailOffset,
Split SplitAfterLastLine,
WhitespaceManager &Whitespaces) const {
insertBreak(getLineCount() - 1, TailOffset, SplitAfterLastLine,
0, Whitespaces);
}




virtual void updateNextToken(LineState &State) const {}

protected:
BreakableToken(const FormatToken &Tok, bool InPPDirective,
encoding::Encoding Encoding, const FormatStyle &Style)
: Tok(Tok), InPPDirective(InPPDirective), Encoding(Encoding),
Style(Style) {}

const FormatToken &Tok;
const bool InPPDirective;
const encoding::Encoding Encoding;
const FormatStyle &Style;
};

class BreakableStringLiteral : public BreakableToken {
public:




BreakableStringLiteral(const FormatToken &Tok, unsigned StartColumn,
StringRef Prefix, StringRef Postfix,
unsigned UnbreakableTailLength, bool InPPDirective,
encoding::Encoding Encoding, const FormatStyle &Style);

Split getSplit(unsigned LineIndex, unsigned TailOffset, unsigned ColumnLimit,
unsigned ContentStartColumn,
const llvm::Regex &CommentPragmasRegex) const override;
void insertBreak(unsigned LineIndex, unsigned TailOffset, Split Split,
unsigned ContentIndent,
WhitespaceManager &Whitespaces) const override;
void compressWhitespace(unsigned LineIndex, unsigned TailOffset, Split Split,
WhitespaceManager &Whitespaces) const override {}
unsigned getLineCount() const override;
unsigned getRangeLength(unsigned LineIndex, unsigned Offset,
StringRef::size_type Length,
unsigned StartColumn) const override;
unsigned getRemainingLength(unsigned LineIndex, unsigned Offset,
unsigned StartColumn) const override;
unsigned getContentStartColumn(unsigned LineIndex, bool Break) const override;

protected:

unsigned StartColumn;

StringRef Prefix;

StringRef Postfix;

StringRef Line;


unsigned UnbreakableTailLength;
};

class BreakableComment : public BreakableToken {
protected:




BreakableComment(const FormatToken &Token, unsigned StartColumn,
bool InPPDirective, encoding::Encoding Encoding,
const FormatStyle &Style);

public:
bool supportsReflow() const override { return true; }
unsigned getLineCount() const override;
Split getSplit(unsigned LineIndex, unsigned TailOffset, unsigned ColumnLimit,
unsigned ContentStartColumn,
const llvm::Regex &CommentPragmasRegex) const override;
void compressWhitespace(unsigned LineIndex, unsigned TailOffset, Split Split,
WhitespaceManager &Whitespaces) const override;

protected:

const FormatToken &tokenAt(unsigned LineIndex) const;



virtual bool mayReflow(unsigned LineIndex,
const llvm::Regex &CommentPragmasRegex) const = 0;






SmallVector<StringRef, 16> Lines;




SmallVector<StringRef, 16> Content;




SmallVector<FormatToken *, 16> Tokens;











SmallVector<int, 16> ContentColumn;


unsigned StartColumn;









StringRef ReflowPrefix = " ";
};

class BreakableBlockComment : public BreakableComment {
public:
BreakableBlockComment(const FormatToken &Token, unsigned StartColumn,
unsigned OriginalStartColumn, bool FirstInLine,
bool InPPDirective, encoding::Encoding Encoding,
const FormatStyle &Style, bool UseCRLF);

Split getSplit(unsigned LineIndex, unsigned TailOffset, unsigned ColumnLimit,
unsigned ContentStartColumn,
const llvm::Regex &CommentPragmasRegex) const override;
unsigned getRangeLength(unsigned LineIndex, unsigned Offset,
StringRef::size_type Length,
unsigned StartColumn) const override;
unsigned getRemainingLength(unsigned LineIndex, unsigned Offset,
unsigned StartColumn) const override;
unsigned getContentStartColumn(unsigned LineIndex, bool Break) const override;
unsigned getContentIndent(unsigned LineIndex) const override;
void insertBreak(unsigned LineIndex, unsigned TailOffset, Split Split,
unsigned ContentIndent,
WhitespaceManager &Whitespaces) const override;
Split getReflowSplit(unsigned LineIndex,
const llvm::Regex &CommentPragmasRegex) const override;
void reflow(unsigned LineIndex,
WhitespaceManager &Whitespaces) const override;
bool introducesBreakBeforeToken() const override;
void adaptStartOfLine(unsigned LineIndex,
WhitespaceManager &Whitespaces) const override;
Split getSplitAfterLastLine(unsigned TailOffset) const override;

bool mayReflow(unsigned LineIndex,
const llvm::Regex &CommentPragmasRegex) const override;



static const llvm::StringSet<> ContentIndentingJavadocAnnotations;

private:








void adjustWhitespace(unsigned LineIndex, int IndentDelta);











unsigned IndentAtLineBreak;



bool LastLineNeedsDecoration;


StringRef Decoration;



unsigned DecorationColumn;



bool DelimitersOnNewline;



unsigned UnbreakableTailLength;
};

class BreakableLineCommentSection : public BreakableComment {
public:
BreakableLineCommentSection(const FormatToken &Token, unsigned StartColumn,
bool InPPDirective, encoding::Encoding Encoding,
const FormatStyle &Style);

unsigned getRangeLength(unsigned LineIndex, unsigned Offset,
StringRef::size_type Length,
unsigned StartColumn) const override;
unsigned getContentStartColumn(unsigned LineIndex, bool Break) const override;
void insertBreak(unsigned LineIndex, unsigned TailOffset, Split Split,
unsigned ContentIndent,
WhitespaceManager &Whitespaces) const override;
Split getReflowSplit(unsigned LineIndex,
const llvm::Regex &CommentPragmasRegex) const override;
void reflow(unsigned LineIndex,
WhitespaceManager &Whitespaces) const override;
void adaptStartOfLine(unsigned LineIndex,
WhitespaceManager &Whitespaces) const override;
void updateNextToken(LineState &State) const override;
bool mayReflow(unsigned LineIndex,
const llvm::Regex &CommentPragmasRegex) const override;

private:






SmallVector<StringRef, 16> OriginalPrefix;













SmallVector<std::string, 16> Prefix;



SmallVector<int, 16> PrefixSpaceChange;








FormatToken *LastLineTok = nullptr;
};
}
}

#endif
