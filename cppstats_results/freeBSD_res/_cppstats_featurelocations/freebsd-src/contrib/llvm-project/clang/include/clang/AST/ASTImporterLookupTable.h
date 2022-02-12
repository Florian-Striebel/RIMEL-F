












#if !defined(LLVM_CLANG_AST_ASTIMPORTERLOOKUPTABLE_H)
#define LLVM_CLANG_AST_ASTIMPORTERLOOKUPTABLE_H

#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclarationName.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"

namespace clang {

class ASTContext;
class NamedDecl;
class DeclContext;




















class ASTImporterLookupTable {





using DeclList = llvm::SmallSetVector<NamedDecl *, 2>;
using NameMap = llvm::SmallDenseMap<DeclarationName, DeclList, 4>;
using DCMap = llvm::DenseMap<DeclContext *, NameMap>;

void add(DeclContext *DC, NamedDecl *ND);
void remove(DeclContext *DC, NamedDecl *ND);

DCMap LookupTable;

public:
ASTImporterLookupTable(TranslationUnitDecl &TU);
void add(NamedDecl *ND);
void remove(NamedDecl *ND);











void update(NamedDecl *ND, DeclContext *OldDC);
using LookupResult = DeclList;
LookupResult lookup(DeclContext *DC, DeclarationName Name) const;



bool contains(DeclContext *DC, NamedDecl *ND) const;
void dump(DeclContext *DC) const;
void dump() const;
};

}

#endif
