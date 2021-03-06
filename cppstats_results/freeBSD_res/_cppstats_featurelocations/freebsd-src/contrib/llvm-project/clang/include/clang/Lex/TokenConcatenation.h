











#if !defined(LLVM_CLANG_LEX_TOKENCONCATENATION_H)
#define LLVM_CLANG_LEX_TOKENCONCATENATION_H

#include "clang/Basic/TokenKinds.h"

namespace clang {
class Preprocessor;
class Token;









class TokenConcatenation {
const Preprocessor &PP;

enum AvoidConcatInfo {


aci_never_avoid_concat = 0,




aci_custom_firstchar = 1,




aci_custom = 2,



aci_avoid_equal = 4
};




char TokenInfo[tok::NUM_TOKENS];
public:
TokenConcatenation(const Preprocessor &PP);

bool AvoidConcat(const Token &PrevPrevTok,
const Token &PrevTok,
const Token &Tok) const;

private:


bool IsIdentifierStringPrefix(const Token &Tok) const;
};
}

#endif
