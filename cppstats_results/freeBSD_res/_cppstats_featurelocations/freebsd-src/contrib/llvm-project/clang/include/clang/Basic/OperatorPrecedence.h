












#if !defined(LLVM_CLANG_BASIC_OPERATORPRECEDENCE_H)
#define LLVM_CLANG_BASIC_OPERATORPRECEDENCE_H

#include "clang/Basic/TokenKinds.h"

namespace clang {





namespace prec {
enum Level {
Unknown = 0,
Comma = 1,
Assignment = 2,
Conditional = 3,
LogicalOr = 4,
LogicalAnd = 5,
InclusiveOr = 6,
ExclusiveOr = 7,
And = 8,
Equality = 9,
Relational = 10,
Spaceship = 11,
Shift = 12,
Additive = 13,
Multiplicative = 14,
PointerToMember = 15
};
}


prec::Level getBinOpPrecedence(tok::TokenKind Kind, bool GreaterThanIsOperator,
bool CPlusPlus11);

}

#endif
