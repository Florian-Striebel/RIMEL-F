













#if !defined(LLVM_CLANG_AST_ODRHASH_H)
#define LLVM_CLANG_AST_ODRHASH_H

#include "clang/AST/DeclarationName.h"
#include "clang/AST/Type.h"
#include "clang/AST/TemplateBase.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {

class Decl;
class IdentifierInfo;
class NestedNameSpecifier;
class Stmt;
class TemplateParameterList;








class ODRHash {


llvm::DenseMap<DeclarationName, unsigned> DeclNameMap;


llvm::SmallVector<bool, 128> Bools;

llvm::FoldingSetNodeID ID;

public:
ODRHash() {}



void AddCXXRecordDecl(const CXXRecordDecl *Record);




void AddFunctionDecl(const FunctionDecl *Function, bool SkipBody = false);



void AddEnumDecl(const EnumDecl *Enum);



void AddSubDecl(const Decl *D);


void clear();


unsigned CalculateHash();


void AddDecl(const Decl *D);
void AddType(const Type *T);
void AddQualType(QualType T);
void AddStmt(const Stmt *S);
void AddIdentifierInfo(const IdentifierInfo *II);
void AddNestedNameSpecifier(const NestedNameSpecifier *NNS);
void AddTemplateName(TemplateName Name);
void AddDeclarationName(DeclarationName Name, bool TreatAsDecl = false);
void AddTemplateArgument(TemplateArgument TA);
void AddTemplateParameterList(const TemplateParameterList *TPL);


void AddBoolean(bool value);

static bool isDeclToBeProcessed(const Decl* D, const DeclContext *Parent);

private:
void AddDeclarationNameImpl(DeclarationName Name);
};

}

#endif
