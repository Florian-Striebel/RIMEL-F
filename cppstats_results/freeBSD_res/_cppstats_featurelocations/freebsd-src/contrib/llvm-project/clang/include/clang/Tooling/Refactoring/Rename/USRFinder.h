













#if !defined(LLVM_CLANG_TOOLING_REFACTOR_RENAME_USR_FINDER_H)
#define LLVM_CLANG_TOOLING_REFACTOR_RENAME_USR_FINDER_H

#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include <string>
#include <vector>

namespace clang {

class ASTContext;
class Decl;
class SourceLocation;
class NamedDecl;

namespace tooling {



const NamedDecl *getNamedDeclAt(const ASTContext &Context,
const SourceLocation Point);




const NamedDecl *getNamedDeclFor(const ASTContext &Context,
const std::string &Name);


std::string getUSRForDecl(const Decl *Decl);

}
}

#endif
