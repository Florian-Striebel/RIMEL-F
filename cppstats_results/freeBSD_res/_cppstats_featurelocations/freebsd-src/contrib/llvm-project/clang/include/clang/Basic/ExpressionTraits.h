












#if !defined(LLVM_CLANG_BASIC_EXPRESSIONTRAITS_H)
#define LLVM_CLANG_BASIC_EXPRESSIONTRAITS_H

#include "llvm/Support/Compiler.h"

namespace clang {

enum ExpressionTrait {
#define EXPRESSION_TRAIT(Spelling, Name, Key) ET_##Name,
#include "clang/Basic/TokenKinds.def"
ET_Last = -1
#define EXPRESSION_TRAIT(Spelling, Name, Key) +1
#include "clang/Basic/TokenKinds.def"
};


const char *getTraitName(ExpressionTrait T) LLVM_READONLY;


const char *getTraitSpelling(ExpressionTrait T) LLVM_READONLY;

}

#endif
