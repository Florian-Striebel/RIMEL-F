











#if !defined(LLVM_CLANG_AST_COMMENTLEXER_H)
#define LLVM_CLANG_AST_COMMENTLEXER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace comments {

class Lexer;
class TextTokenRetokenizer;
struct CommandInfo;
class CommandTraits;

namespace tok {
enum TokenKind {
eof,
newline,
text,
unknown_command,
backslash_command,
at_command,
verbatim_block_begin,
verbatim_block_line,
verbatim_block_end,
verbatim_line_name,
verbatim_line_text,
html_start_tag,
html_ident,
html_equals,
html_quoted_string,
html_greater,
html_slash_greater,
html_end_tag
};
}


class Token {
friend class Lexer;
friend class TextTokenRetokenizer;


SourceLocation Loc;


tok::TokenKind Kind;






unsigned IntVal;



unsigned Length;


const char *TextPtr;

public:
SourceLocation getLocation() const LLVM_READONLY { return Loc; }
void setLocation(SourceLocation SL) { Loc = SL; }

SourceLocation getEndLocation() const LLVM_READONLY {
if (Length == 0 || Length == 1)
return Loc;
return Loc.getLocWithOffset(Length - 1);
}

tok::TokenKind getKind() const LLVM_READONLY { return Kind; }
void setKind(tok::TokenKind K) { Kind = K; }

bool is(tok::TokenKind K) const LLVM_READONLY { return Kind == K; }
bool isNot(tok::TokenKind K) const LLVM_READONLY { return Kind != K; }

unsigned getLength() const LLVM_READONLY { return Length; }
void setLength(unsigned L) { Length = L; }

StringRef getText() const LLVM_READONLY {
assert(is(tok::text));
return StringRef(TextPtr, IntVal);
}

void setText(StringRef Text) {
assert(is(tok::text));
TextPtr = Text.data();
IntVal = Text.size();
}

StringRef getUnknownCommandName() const LLVM_READONLY {
assert(is(tok::unknown_command));
return StringRef(TextPtr, IntVal);
}

void setUnknownCommandName(StringRef Name) {
assert(is(tok::unknown_command));
TextPtr = Name.data();
IntVal = Name.size();
}

unsigned getCommandID() const LLVM_READONLY {
assert(is(tok::backslash_command) || is(tok::at_command));
return IntVal;
}

void setCommandID(unsigned ID) {
assert(is(tok::backslash_command) || is(tok::at_command));
IntVal = ID;
}

unsigned getVerbatimBlockID() const LLVM_READONLY {
assert(is(tok::verbatim_block_begin) || is(tok::verbatim_block_end));
return IntVal;
}

void setVerbatimBlockID(unsigned ID) {
assert(is(tok::verbatim_block_begin) || is(tok::verbatim_block_end));
IntVal = ID;
}

StringRef getVerbatimBlockText() const LLVM_READONLY {
assert(is(tok::verbatim_block_line));
return StringRef(TextPtr, IntVal);
}

void setVerbatimBlockText(StringRef Text) {
assert(is(tok::verbatim_block_line));
TextPtr = Text.data();
IntVal = Text.size();
}

unsigned getVerbatimLineID() const LLVM_READONLY {
assert(is(tok::verbatim_line_name));
return IntVal;
}

void setVerbatimLineID(unsigned ID) {
assert(is(tok::verbatim_line_name));
IntVal = ID;
}

StringRef getVerbatimLineText() const LLVM_READONLY {
assert(is(tok::verbatim_line_text));
return StringRef(TextPtr, IntVal);
}

void setVerbatimLineText(StringRef Text) {
assert(is(tok::verbatim_line_text));
TextPtr = Text.data();
IntVal = Text.size();
}

StringRef getHTMLTagStartName() const LLVM_READONLY {
assert(is(tok::html_start_tag));
return StringRef(TextPtr, IntVal);
}

void setHTMLTagStartName(StringRef Name) {
assert(is(tok::html_start_tag));
TextPtr = Name.data();
IntVal = Name.size();
}

StringRef getHTMLIdent() const LLVM_READONLY {
assert(is(tok::html_ident));
return StringRef(TextPtr, IntVal);
}

void setHTMLIdent(StringRef Name) {
assert(is(tok::html_ident));
TextPtr = Name.data();
IntVal = Name.size();
}

StringRef getHTMLQuotedString() const LLVM_READONLY {
assert(is(tok::html_quoted_string));
return StringRef(TextPtr, IntVal);
}

void setHTMLQuotedString(StringRef Str) {
assert(is(tok::html_quoted_string));
TextPtr = Str.data();
IntVal = Str.size();
}

StringRef getHTMLTagEndName() const LLVM_READONLY {
assert(is(tok::html_end_tag));
return StringRef(TextPtr, IntVal);
}

void setHTMLTagEndName(StringRef Name) {
assert(is(tok::html_end_tag));
TextPtr = Name.data();
IntVal = Name.size();
}

void dump(const Lexer &L, const SourceManager &SM) const;
};


class Lexer {
private:
Lexer(const Lexer &) = delete;
void operator=(const Lexer &) = delete;



llvm::BumpPtrAllocator &Allocator;

DiagnosticsEngine &Diags;

const CommandTraits &Traits;

const char *const BufferStart;
const char *const BufferEnd;

const char *BufferPtr;



const char *CommentEnd;

SourceLocation FileLoc;




bool ParseCommands;

enum LexerCommentState : uint8_t {
LCS_BeforeComment,
LCS_InsideBCPLComment,
LCS_InsideCComment,
LCS_BetweenComments
};


LexerCommentState CommentState;

enum LexerState : uint8_t {

LS_Normal,



LS_VerbatimBlockFirstLine,



LS_VerbatimBlockBody,



LS_VerbatimLineText,


LS_HTMLStartTag,


LS_HTMLEndTag
};


LexerState State;



SmallString<16> VerbatimBlockEndCommandName;



StringRef resolveHTMLNamedCharacterReference(StringRef Name) const;


StringRef resolveHTMLDecimalCharacterReference(StringRef Name) const;


StringRef resolveHTMLHexCharacterReference(StringRef Name) const;

void formTokenWithChars(Token &Result, const char *TokEnd,
tok::TokenKind Kind);

void formTextToken(Token &Result, const char *TokEnd) {
StringRef Text(BufferPtr, TokEnd - BufferPtr);
formTokenWithChars(Result, TokEnd, tok::text);
Result.setText(Text);
}

SourceLocation getSourceLocation(const char *Loc) const {
assert(Loc >= BufferStart && Loc <= BufferEnd &&
"Location out of range for this buffer!");

const unsigned CharNo = Loc - BufferStart;
return FileLoc.getLocWithOffset(CharNo);
}

DiagnosticBuilder Diag(SourceLocation Loc, unsigned DiagID) {
return Diags.Report(Loc, DiagID);
}


void skipLineStartingDecorations();


void lexCommentText(Token &T);

void setupAndLexVerbatimBlock(Token &T, const char *TextBegin, char Marker,
const CommandInfo *Info);

void lexVerbatimBlockFirstLine(Token &T);

void lexVerbatimBlockBody(Token &T);

void setupAndLexVerbatimLine(Token &T, const char *TextBegin,
const CommandInfo *Info);

void lexVerbatimLineText(Token &T);

void lexHTMLCharacterReference(Token &T);

void setupAndLexHTMLStartTag(Token &T);

void lexHTMLStartTag(Token &T);

void setupAndLexHTMLEndTag(Token &T);

void lexHTMLEndTag(Token &T);

public:
Lexer(llvm::BumpPtrAllocator &Allocator, DiagnosticsEngine &Diags,
const CommandTraits &Traits, SourceLocation FileLoc,
const char *BufferStart, const char *BufferEnd,
bool ParseCommands = true);

void lex(Token &T);

StringRef getSpelling(const Token &Tok, const SourceManager &SourceMgr) const;
};

}
}

#endif

