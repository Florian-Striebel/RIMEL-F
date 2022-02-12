












#if !defined(LLVM_CLANG_AST_OPERATIONKINDS_H)
#define LLVM_CLANG_AST_OPERATIONKINDS_H

namespace clang {


enum CastKind {
#define CAST_OPERATION(Name) CK_##Name,
#include "clang/AST/OperationKinds.def"
};

enum BinaryOperatorKind {
#define BINARY_OPERATION(Name, Spelling) BO_##Name,
#include "clang/AST/OperationKinds.def"
};

enum UnaryOperatorKind {
#define UNARY_OPERATION(Name, Spelling) UO_##Name,
#include "clang/AST/OperationKinds.def"
};


enum ObjCBridgeCastKind {


OBC_Bridge,


OBC_BridgeTransfer,


OBC_BridgeRetained
};

}

#endif
