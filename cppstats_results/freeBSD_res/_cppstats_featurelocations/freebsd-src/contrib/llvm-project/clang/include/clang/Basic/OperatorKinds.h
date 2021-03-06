












#if !defined(LLVM_CLANG_BASIC_OPERATORKINDS_H)
#define LLVM_CLANG_BASIC_OPERATORKINDS_H

namespace clang {



enum OverloadedOperatorKind : int {
OO_None,
#define OVERLOADED_OPERATOR(Name,Spelling,Token,Unary,Binary,MemberOnly) OO_##Name,

#include "clang/Basic/OperatorKinds.def"
NUM_OVERLOADED_OPERATORS
};



const char *getOperatorSpelling(OverloadedOperatorKind Operator);



inline OverloadedOperatorKind
getRewrittenOverloadedOperator(OverloadedOperatorKind Kind) {
switch (Kind) {
case OO_Less:
case OO_LessEqual:
case OO_Greater:
case OO_GreaterEqual:
return OO_Spaceship;

case OO_ExclaimEqual:
return OO_EqualEqual;

default:
return OO_None;
}
}


inline bool isCompoundAssignmentOperator(OverloadedOperatorKind Kind) {
return Kind >= OO_PlusEqual && Kind <= OO_PipeEqual;
}

}

#endif
