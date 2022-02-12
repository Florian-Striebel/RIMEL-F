











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CHECKERHELPERS_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CHECKERHELPERS_H

#include "clang/AST/OperationKinds.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/OperatorKinds.h"
#include "llvm/ADT/Optional.h"
#include <tuple>

namespace clang {

class Expr;
class VarDecl;
class QualType;
class AttributedType;
class Preprocessor;

namespace ento {

bool containsMacro(const Stmt *S);
bool containsEnum(const Stmt *S);
bool containsStaticLocal(const Stmt *S);
bool containsBuiltinOffsetOf(const Stmt *S);
template <class T> bool containsStmt(const Stmt *S) {
if (isa<T>(S))
return true;

for (const Stmt *Child : S->children())
if (Child && containsStmt<T>(Child))
return true;

return false;
}

std::pair<const clang::VarDecl *, const clang::Expr *>
parseAssignment(const Stmt *S);







enum class Nullability : char {
Contradicted,



Nullable,
Unspecified,
Nonnull
};


Nullability getNullabilityAnnotation(QualType Type);




llvm::Optional<int> tryExpandAsInteger(StringRef Macro, const Preprocessor &PP);

class OperatorKind {
union {
BinaryOperatorKind Bin;
UnaryOperatorKind Un;
} Op;
bool IsBinary;

public:
explicit OperatorKind(BinaryOperatorKind Bin) : Op{Bin}, IsBinary{true} {}
explicit OperatorKind(UnaryOperatorKind Un) : IsBinary{false} { Op.Un = Un; }
bool IsBinaryOp() const { return IsBinary; }

BinaryOperatorKind GetBinaryOpUnsafe() const {
assert(IsBinary && "cannot get binary operator - we have a unary operator");
return Op.Bin;
}

Optional<BinaryOperatorKind> GetBinaryOp() const {
if (IsBinary)
return Op.Bin;
return {};
}

UnaryOperatorKind GetUnaryOpUnsafe() const {
assert(!IsBinary &&
"cannot get unary operator - we have a binary operator");
return Op.Un;
}

Optional<UnaryOperatorKind> GetUnaryOp() const {
if (!IsBinary)
return Op.Un;
return {};
}
};

OperatorKind operationKindFromOverloadedOperator(OverloadedOperatorKind OOK,
bool IsBinary);

}

}

#endif
