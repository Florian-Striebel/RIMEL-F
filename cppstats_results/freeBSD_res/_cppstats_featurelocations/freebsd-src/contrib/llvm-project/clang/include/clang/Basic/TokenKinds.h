












#if !defined(LLVM_CLANG_BASIC_TOKENKINDS_H)
#define LLVM_CLANG_BASIC_TOKENKINDS_H

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Support/Compiler.h"

namespace clang {

namespace tok {


enum TokenKind : unsigned short {
#define TOK(X) X,
#include "clang/Basic/TokenKinds.def"
NUM_TOKENS
};



enum PPKeywordKind {
#define PPKEYWORD(X) pp_##X,
#include "clang/Basic/TokenKinds.def"
NUM_PP_KEYWORDS
};



enum ObjCKeywordKind {
#define OBJC_AT_KEYWORD(X) objc_##X,
#include "clang/Basic/TokenKinds.def"
NUM_OBJC_KEYWORDS
};


enum OnOffSwitch {
OOS_ON, OOS_OFF, OOS_DEFAULT
};





const char *getTokenName(TokenKind Kind) LLVM_READNONE;








const char *getPunctuatorSpelling(TokenKind Kind) LLVM_READNONE;



const char *getKeywordSpelling(TokenKind Kind) LLVM_READNONE;


inline bool isAnyIdentifier(TokenKind K) {
return (K == tok::identifier) || (K == tok::raw_identifier);
}



inline bool isStringLiteral(TokenKind K) {
return K == tok::string_literal || K == tok::wide_string_literal ||
K == tok::utf8_string_literal || K == tok::utf16_string_literal ||
K == tok::utf32_string_literal;
}



inline bool isLiteral(TokenKind K) {
return K == tok::numeric_constant || K == tok::char_constant ||
K == tok::wide_char_constant || K == tok::utf8_char_constant ||
K == tok::utf16_char_constant || K == tok::utf32_char_constant ||
isStringLiteral(K) || K == tok::header_name;
}


bool isAnnotation(TokenKind K);


bool isPragmaAnnotation(TokenKind K);

}
}

namespace llvm {
template <> struct DenseMapInfo<clang::tok::PPKeywordKind> {
static inline clang::tok::PPKeywordKind getEmptyKey() {
return clang::tok::PPKeywordKind::pp_not_keyword;
}
static inline clang::tok::PPKeywordKind getTombstoneKey() {
return clang::tok::PPKeywordKind::NUM_PP_KEYWORDS;
}
static unsigned getHashValue(const clang::tok::PPKeywordKind &Val) {
return static_cast<unsigned>(Val);
}
static bool isEqual(const clang::tok::PPKeywordKind &LHS,
const clang::tok::PPKeywordKind &RHS) {
return LHS == RHS;
}
};
}

#endif
