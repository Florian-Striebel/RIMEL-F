







#if !defined(LLVM_CLANG_INDEX_INDEXDATACONSUMER_H)
#define LLVM_CLANG_INDEX_INDEXDATACONSUMER_H

#include "clang/Index/IndexSymbol.h"
#include "clang/Lex/Preprocessor.h"

namespace clang {
class ASTContext;
class DeclContext;
class Expr;
class FileID;
class IdentifierInfo;
class ImportDecl;
class MacroInfo;

namespace index {

class IndexDataConsumer {
public:
struct ASTNodeInfo {
const Expr *OrigE;
const Decl *OrigD;
const Decl *Parent;
const DeclContext *ContainerDC;
};

virtual ~IndexDataConsumer() = default;

virtual void initialize(ASTContext &Ctx) {}

virtual void setPreprocessor(std::shared_ptr<Preprocessor> PP) {}


virtual bool handleDeclOccurrence(const Decl *D, SymbolRoleSet Roles,
ArrayRef<SymbolRelation> Relations,
SourceLocation Loc, ASTNodeInfo ASTNode) {
return true;
}


virtual bool handleMacroOccurrence(const IdentifierInfo *Name,
const MacroInfo *MI, SymbolRoleSet Roles,
SourceLocation Loc) {
return true;
}






virtual bool handleModuleOccurrence(const ImportDecl *ImportD,
const Module *Mod, SymbolRoleSet Roles,
SourceLocation Loc) {
return true;
}

virtual void finish() {}
};

}
}

#endif
