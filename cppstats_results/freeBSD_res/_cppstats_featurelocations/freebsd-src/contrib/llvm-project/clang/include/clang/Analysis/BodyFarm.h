












#if !defined(LLVM_CLANG_LIB_ANALYSIS_BODYFARM_H)
#define LLVM_CLANG_LIB_ANALYSIS_BODYFARM_H

#include "clang/AST/DeclBase.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"

namespace clang {

class ASTContext;
class FunctionDecl;
class ObjCMethodDecl;
class ObjCPropertyDecl;
class Stmt;
class CodeInjector;

class BodyFarm {
public:
BodyFarm(ASTContext &C, CodeInjector *injector) : C(C), Injector(injector) {}


Stmt *getBody(const FunctionDecl *D);


Stmt *getBody(const ObjCMethodDecl *D);


BodyFarm(const BodyFarm &other) = delete;

private:
typedef llvm::DenseMap<const Decl *, Optional<Stmt *>> BodyMap;

ASTContext &C;
BodyMap Bodies;
CodeInjector *Injector;
};
}

#endif
