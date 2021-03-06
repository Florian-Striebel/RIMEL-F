













#if !defined(LLVM_CLANG_LIB_FORMAT_FORMATTOKENLEXER_H)
#define LLVM_CLANG_LIB_FORMAT_FORMATTOKENLEXER_H

#include "Encoding.h"
#include "FormatToken.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Format/Format.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Regex.h"

#include <stack>

namespace clang {
namespace format {

enum LexerState {
NORMAL,
TEMPLATE_STRING,
TOKEN_STASHED,
};

class FormatTokenLexer {
public:
FormatTokenLexer(const SourceManager &SourceMgr, FileID ID, unsigned Column,
const FormatStyle &Style, encoding::Encoding Encoding,
llvm::SpecificBumpPtrAllocator<FormatToken> &Allocator,
IdentifierTable &IdentTable);

ArrayRef<FormatToken *> lex();

const AdditionalKeywords &getKeywords() { return Keywords; }

private:
void tryMergePreviousTokens();

bool tryMergeLessLess();
bool tryMergeNSStringLiteral();
bool tryMergeJSPrivateIdentifier();
bool tryMergeCSharpStringLiteral();
bool tryMergeCSharpKeywordVariables();
bool tryMergeNullishCoalescingEqual();
bool tryTransformCSharpForEach();
bool tryMergeForEach();
bool tryTransformTryUsageForC();

bool tryMergeTokens(ArrayRef<tok::TokenKind> Kinds, TokenType NewType);


bool precedesOperand(FormatToken *Tok);

bool canPrecedeRegexLiteral(FormatToken *Prev);





void tryParseJSRegexLiteral();










void handleTemplateStrings();

void handleCSharpVerbatimAndInterpolatedStrings();

void tryParsePythonComment();

bool tryMerge_TMacro();

bool tryMergeConflictMarkers();

FormatToken *getStashedToken();

FormatToken *getNextToken();

FormatToken *FormatTok;
bool IsFirstToken;
std::stack<LexerState> StateStack;
unsigned Column;
unsigned TrailingWhitespace;
std::unique_ptr<Lexer> Lex;
const SourceManager &SourceMgr;
FileID ID;
const FormatStyle &Style;
IdentifierTable &IdentTable;
AdditionalKeywords Keywords;
encoding::Encoding Encoding;
llvm::SpecificBumpPtrAllocator<FormatToken> &Allocator;

unsigned FirstInLineIndex;
SmallVector<FormatToken *, 16> Tokens;

llvm::SmallMapVector<IdentifierInfo *, TokenType, 8> Macros;

bool FormattingDisabled;

llvm::Regex MacroBlockBeginRegex;
llvm::Regex MacroBlockEndRegex;


static const llvm::StringSet<> CSharpAttributeTargets;

void readRawToken(FormatToken &Tok);

void resetLexer(unsigned Offset);
};

}
}

#endif
