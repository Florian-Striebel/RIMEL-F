












#if !defined(LLVM_CLANG_AST_COMMENTBRIEFPARSER_H)
#define LLVM_CLANG_AST_COMMENTBRIEFPARSER_H

#include "clang/AST/CommentLexer.h"

namespace clang {
namespace comments {








class BriefParser {
Lexer &L;

const CommandTraits &Traits;


Token Tok;

SourceLocation ConsumeToken() {
SourceLocation Loc = Tok.getLocation();
L.lex(Tok);
return Loc;
}

public:
BriefParser(Lexer &L, const CommandTraits &Traits);


std::string Parse();
};

}
}

#endif

