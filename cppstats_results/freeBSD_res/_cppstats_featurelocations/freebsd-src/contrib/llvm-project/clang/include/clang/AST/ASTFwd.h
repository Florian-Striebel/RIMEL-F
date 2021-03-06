












#if !defined(LLVM_CLANG_AST_ASTFWD_H)
#define LLVM_CLANG_AST_ASTFWD_H

namespace clang {

class Decl;
#define DECL(DERIVED, BASE) class DERIVED##Decl;
#include "clang/AST/DeclNodes.inc"
class Stmt;
#define STMT(DERIVED, BASE) class DERIVED;
#include "clang/AST/StmtNodes.inc"
class Type;
#define TYPE(DERIVED, BASE) class DERIVED##Type;
#include "clang/AST/TypeNodes.inc"
class CXXCtorInitializer;
class OMPClause;
#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) class Class;
#include "llvm/Frontend/OpenMP/OMP.inc"

}

#endif
