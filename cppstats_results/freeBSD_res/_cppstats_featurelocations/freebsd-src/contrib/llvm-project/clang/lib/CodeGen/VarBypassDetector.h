












#if !defined(LLVM_CLANG_LIB_CODEGEN_VARBYPASSDETECTOR_H)
#define LLVM_CLANG_LIB_CODEGEN_VARBYPASSDETECTOR_H

#include "clang/AST/Decl.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {

class Decl;
class Stmt;
class VarDecl;

namespace CodeGen {










class VarBypassDetector {


llvm::SmallVector<std::pair<unsigned, const VarDecl *>, 48> Scopes;

llvm::SmallVector<std::pair<const Stmt *, unsigned>, 16> FromScopes;

llvm::DenseMap<const Stmt *, unsigned> ToScopes;

llvm::DenseSet<const VarDecl *> Bypasses;

bool AlwaysBypassed = false;

public:
void Init(const Stmt *Body);



bool IsBypassed(const VarDecl *D) const {
return AlwaysBypassed || Bypasses.find(D) != Bypasses.end();
}

private:
bool BuildScopeInformation(const Decl *D, unsigned &ParentScope);
bool BuildScopeInformation(const Stmt *S, unsigned &origParentScope);
void Detect();
void Detect(unsigned From, unsigned To);
};
}
}

#endif
