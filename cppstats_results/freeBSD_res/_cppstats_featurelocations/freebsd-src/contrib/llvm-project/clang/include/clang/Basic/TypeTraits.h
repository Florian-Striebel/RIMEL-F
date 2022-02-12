












#if !defined(LLVM_CLANG_BASIC_TYPETRAITS_H)
#define LLVM_CLANG_BASIC_TYPETRAITS_H

#include "llvm/Support/Compiler.h"

namespace clang {

enum TypeTrait {
#define TYPE_TRAIT_1(Spelling, Name, Key) UTT_##Name,
#include "clang/Basic/TokenKinds.def"
UTT_Last = -1
#define TYPE_TRAIT_1(Spelling, Name, Key) +1
#include "clang/Basic/TokenKinds.def"
,
#define TYPE_TRAIT_2(Spelling, Name, Key) BTT_##Name,
#include "clang/Basic/TokenKinds.def"
BTT_Last = UTT_Last
#define TYPE_TRAIT_2(Spelling, Name, Key) +1
#include "clang/Basic/TokenKinds.def"
,
#define TYPE_TRAIT_N(Spelling, Name, Key) TT_##Name,
#include "clang/Basic/TokenKinds.def"
TT_Last = BTT_Last
#define TYPE_TRAIT_N(Spelling, Name, Key) +1
#include "clang/Basic/TokenKinds.def"
};


enum ArrayTypeTrait {
#define ARRAY_TYPE_TRAIT(Spelling, Name, Key) ATT_##Name,
#include "clang/Basic/TokenKinds.def"
ATT_Last = -1
#define ARRAY_TYPE_TRAIT(Spelling, Name, Key) +1
#include "clang/Basic/TokenKinds.def"
};


enum UnaryExprOrTypeTrait {
#define UNARY_EXPR_OR_TYPE_TRAIT(Spelling, Name, Key) UETT_##Name,
#define CXX11_UNARY_EXPR_OR_TYPE_TRAIT(Spelling, Name, Key) UETT_##Name,
#include "clang/Basic/TokenKinds.def"
UETT_Last = -1
#define UNARY_EXPR_OR_TYPE_TRAIT(Spelling, Name, Key) +1
#define CXX11_UNARY_EXPR_OR_TYPE_TRAIT(Spelling, Name, Key) +1
#include "clang/Basic/TokenKinds.def"
};


const char *getTraitName(TypeTrait T) LLVM_READONLY;
const char *getTraitName(ArrayTypeTrait T) LLVM_READONLY;
const char *getTraitName(UnaryExprOrTypeTrait T) LLVM_READONLY;


const char *getTraitSpelling(TypeTrait T) LLVM_READONLY;
const char *getTraitSpelling(ArrayTypeTrait T) LLVM_READONLY;
const char *getTraitSpelling(UnaryExprOrTypeTrait T) LLVM_READONLY;
}

#endif
