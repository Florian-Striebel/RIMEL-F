













#if !defined(LLVM_CLANG_AST_ASTIMPORTERSHAREDSTATE_H)
#define LLVM_CLANG_AST_ASTIMPORTERSHAREDSTATE_H

#include "clang/AST/ASTImporterLookupTable.h"
#include "clang/AST/Decl.h"
#include "llvm/ADT/DenseMap.h"

#include "clang/AST/ASTImporter.h"

namespace clang {

class TranslationUnitDecl;



class ASTImporterSharedState {


std::unique_ptr<ASTImporterLookupTable> LookupTable;







llvm::DenseMap<Decl *, ImportError> ImportErrors;




public:
ASTImporterSharedState() = default;

ASTImporterSharedState(TranslationUnitDecl &ToTU) {
LookupTable = std::make_unique<ASTImporterLookupTable>(ToTU);
}

ASTImporterLookupTable *getLookupTable() { return LookupTable.get(); }

void addDeclToLookup(Decl *D) {
if (LookupTable)
if (auto *ND = dyn_cast<NamedDecl>(D))
LookupTable->add(ND);
}

void removeDeclFromLookup(Decl *D) {
if (LookupTable)
if (auto *ND = dyn_cast<NamedDecl>(D))
LookupTable->remove(ND);
}

llvm::Optional<ImportError> getImportDeclErrorIfAny(Decl *ToD) const {
auto Pos = ImportErrors.find(ToD);
if (Pos != ImportErrors.end())
return Pos->second;
else
return Optional<ImportError>();
}

void setImportDeclError(Decl *To, ImportError Error) {
ImportErrors[To] = Error;
}
};

}
#endif
