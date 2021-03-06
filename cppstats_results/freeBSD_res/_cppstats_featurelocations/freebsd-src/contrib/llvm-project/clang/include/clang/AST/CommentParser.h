











#if !defined(LLVM_CLANG_AST_COMMENTPARSER_H)
#define LLVM_CLANG_AST_COMMENTPARSER_H

#include "clang/AST/Comment.h"
#include "clang/AST/CommentLexer.h"
#include "clang/AST/CommentSema.h"
#include "clang/Basic/Diagnostic.h"
#include "llvm/Support/Allocator.h"

namespace clang {
class SourceManager;

namespace comments {
class CommandTraits;


class Parser {
Parser(const Parser &) = delete;
void operator=(const Parser &) = delete;

friend class TextTokenRetokenizer;

Lexer &L;

Sema &S;


llvm::BumpPtrAllocator &Allocator;


const SourceManager &SourceMgr;

DiagnosticsEngine &Diags;

DiagnosticBuilder Diag(SourceLocation Loc, unsigned DiagID) {
return Diags.Report(Loc, DiagID);
}

const CommandTraits &Traits;



Token Tok;


SmallVector<Token, 8> MoreLATokens;

void consumeToken() {
if (MoreLATokens.empty())
L.lex(Tok);
else
Tok = MoreLATokens.pop_back_val();
}

void putBack(const Token &OldTok) {
MoreLATokens.push_back(Tok);
Tok = OldTok;
}

void putBack(ArrayRef<Token> Toks) {
if (Toks.empty())
return;

MoreLATokens.push_back(Tok);
MoreLATokens.append(Toks.rbegin(), std::prev(Toks.rend()));

Tok = Toks[0];
}

bool isTokBlockCommand() {
return (Tok.is(tok::backslash_command) || Tok.is(tok::at_command)) &&
Traits.getCommandInfo(Tok.getCommandID())->IsBlockCommand;
}

public:
Parser(Lexer &L, Sema &S, llvm::BumpPtrAllocator &Allocator,
const SourceManager &SourceMgr, DiagnosticsEngine &Diags,
const CommandTraits &Traits);


void parseParamCommandArgs(ParamCommandComment *PC,
TextTokenRetokenizer &Retokenizer);


void parseTParamCommandArgs(TParamCommandComment *TPC,
TextTokenRetokenizer &Retokenizer);

void parseBlockCommandArgs(BlockCommandComment *BC,
TextTokenRetokenizer &Retokenizer,
unsigned NumArgs);

BlockCommandComment *parseBlockCommand();
InlineCommandComment *parseInlineCommand();

HTMLStartTagComment *parseHTMLStartTag();
HTMLEndTagComment *parseHTMLEndTag();

BlockContentComment *parseParagraphOrBlockCommand();

VerbatimBlockComment *parseVerbatimBlock();
VerbatimLineComment *parseVerbatimLine();
BlockContentComment *parseBlockContent();
FullComment *parseFullComment();
};

}
}

#endif

