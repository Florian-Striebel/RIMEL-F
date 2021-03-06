












#if !defined(LLVM_CLANG_AST_ASTIMPORTER_H)
#define LLVM_CLANG_AST_ASTIMPORTER_H

#include "clang/AST/APValue.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Error.h"
#include <utility>

namespace clang {

class ASTContext;
class ASTImporterSharedState;
class Attr;
class CXXBaseSpecifier;
class CXXCtorInitializer;
class Decl;
class DeclContext;
class Expr;
class FileManager;
class NamedDecl;
class Stmt;
class TagDecl;
class TranslationUnitDecl;
class TypeSourceInfo;

class ImportError : public llvm::ErrorInfo<ImportError> {
public:

enum ErrorKind {
NameConflict,
UnsupportedConstruct,
Unknown
};

ErrorKind Error;

static char ID;

ImportError() : Error(Unknown) {}
ImportError(const ImportError &Other) : Error(Other.Error) {}
ImportError &operator=(const ImportError &Other) {
Error = Other.Error;
return *this;
}
ImportError(ErrorKind Error) : Error(Error) { }

std::string toString() const;

void log(raw_ostream &OS) const override;
std::error_code convertToErrorCode() const override;
};








llvm::SmallVector<Decl*, 2> getCanonicalForwardRedeclChain(Decl* D);



class ASTImporter {
friend class ASTNodeImporter;
public:
using NonEquivalentDeclSet = llvm::DenseSet<std::pair<Decl *, Decl *>>;
using ImportedCXXBaseSpecifierMap =
llvm::DenseMap<const CXXBaseSpecifier *, CXXBaseSpecifier *>;

enum class ODRHandlingType { Conservative, Liberal };













































































class ImportPathTy {
public:
using VecTy = llvm::SmallVector<Decl *, 32>;

void push(Decl *D) {
Nodes.push_back(D);
++Aux[D];
}

void pop() {
if (Nodes.empty())
return;
--Aux[Nodes.back()];
Nodes.pop_back();
}


bool hasCycleAtBack() const {
auto Pos = Aux.find(Nodes.back());
return Pos != Aux.end() && Pos->second > 1;
}

using Cycle = llvm::iterator_range<VecTy::const_reverse_iterator>;
Cycle getCycleAtBack() const {
assert(Nodes.size() >= 2);
return Cycle(Nodes.rbegin(),
std::find(Nodes.rbegin() + 1, Nodes.rend(), Nodes.back()) +
1);
}


VecTy copyCycleAtBack() const {
auto R = getCycleAtBack();
return VecTy(R.begin(), R.end());
}

private:

VecTy Nodes;



llvm::SmallDenseMap<Decl *, int, 32> Aux;
};

private:
std::shared_ptr<ASTImporterSharedState> SharedState = nullptr;


ImportPathTy ImportPath;



using SavedImportPathsForOneDecl =
llvm::SmallVector<ImportPathTy::VecTy, 32>;
using SavedImportPathsTy =
llvm::SmallDenseMap<Decl *, SavedImportPathsForOneDecl, 32>;
SavedImportPathsTy SavedImportPaths;


ASTContext &ToContext, &FromContext;


FileManager &ToFileManager, &FromFileManager;


bool Minimal;

ODRHandlingType ODRHandling;


bool LastDiagFromFrom = false;



llvm::DenseMap<const Type *, const Type *> ImportedTypes;



llvm::DenseMap<Decl *, Decl *> ImportedDecls;







llvm::DenseMap<Decl *, ImportError> ImportDeclErrors;



llvm::DenseMap<Decl *, Decl *> ImportedFromDecls;



llvm::DenseMap<Stmt *, Stmt *> ImportedStmts;



llvm::DenseMap<FileID, FileID> ImportedFileIDs;




ImportedCXXBaseSpecifierMap ImportedCXXBaseSpecifiers;



NonEquivalentDeclSet NonEquivalentDecls;

using FoundDeclsTy = SmallVector<NamedDecl *, 2>;
FoundDeclsTy findDeclsInToCtx(DeclContext *DC, DeclarationName Name);

void AddToLookupTable(Decl *ToD);

protected:



virtual Expected<Decl *> ImportImpl(Decl *From);


virtual bool returnWithErrorInTest() { return false; };

public:
















ASTImporter(ASTContext &ToContext, FileManager &ToFileManager,
ASTContext &FromContext, FileManager &FromFileManager,
bool MinimalImport,
std::shared_ptr<ASTImporterSharedState> SharedState = nullptr);

virtual ~ASTImporter();



bool isMinimalImport() const { return Minimal; }

void setODRHandling(ODRHandlingType T) { ODRHandling = T; }






template <typename ImportT>
LLVM_NODISCARD llvm::Error importInto(ImportT &To, const ImportT &From) {
auto ToOrErr = Import(From);
if (ToOrErr)
To = *ToOrErr;
return ToOrErr.takeError();
}


llvm::Expected<ExprWithCleanups::CleanupObject>
Import(ExprWithCleanups::CleanupObject From);





llvm::Expected<const Type *> Import(const Type *FromT);





llvm::Expected<QualType> Import(QualType FromT);






llvm::Expected<TypeSourceInfo *> Import(TypeSourceInfo *FromTSI);






llvm::Expected<Attr *> Import(const Attr *FromAttr);






llvm::Expected<Decl *> Import(Decl *FromD);
llvm::Expected<const Decl *> Import(const Decl *FromD) {
return Import(const_cast<Decl *>(FromD));
}




Decl *GetAlreadyImportedOrNull(const Decl *FromD) const;



TranslationUnitDecl *GetFromTU(Decl *ToD);




template <typename DeclT>
llvm::Optional<DeclT *> getImportedFromDecl(const DeclT *ToD) const {
auto FromI = ImportedFromDecls.find(ToD);
if (FromI == ImportedFromDecls.end())
return {};
auto *FromD = dyn_cast<DeclT>(FromI->second);
if (!FromD)
return {};
return FromD;
}






llvm::Expected<DeclContext *> ImportContext(DeclContext *FromDC);






llvm::Expected<Expr *> Import(Expr *FromE);






llvm::Expected<Stmt *> Import(Stmt *FromS);






llvm::Expected<NestedNameSpecifier *> Import(NestedNameSpecifier *FromNNS);






llvm::Expected<NestedNameSpecifierLoc>
Import(NestedNameSpecifierLoc FromNNS);



llvm::Expected<TemplateName> Import(TemplateName From);






llvm::Expected<SourceLocation> Import(SourceLocation FromLoc);






llvm::Expected<SourceRange> Import(SourceRange FromRange);






llvm::Expected<DeclarationName> Import(DeclarationName FromName);






IdentifierInfo *Import(const IdentifierInfo *FromId);






llvm::Expected<Selector> Import(Selector FromSel);






llvm::Expected<FileID> Import(FileID, bool IsBuiltin = false);






llvm::Expected<CXXCtorInitializer *> Import(CXXCtorInitializer *FromInit);






llvm::Expected<CXXBaseSpecifier *> Import(const CXXBaseSpecifier *FromSpec);






llvm::Expected<APValue> Import(const APValue &FromValue);



LLVM_NODISCARD llvm::Error ImportDefinition(Decl *From);




























virtual Expected<DeclarationName>
HandleNameConflict(DeclarationName Name, DeclContext *DC, unsigned IDNS,
NamedDecl **Decls, unsigned NumDecls);


ASTContext &getToContext() const { return ToContext; }


ASTContext &getFromContext() const { return FromContext; }


FileManager &getToFileManager() const { return ToFileManager; }


FileManager &getFromFileManager() const { return FromFileManager; }


DiagnosticBuilder ToDiag(SourceLocation Loc, unsigned DiagID);


DiagnosticBuilder FromDiag(SourceLocation Loc, unsigned DiagID);


NonEquivalentDeclSet &getNonEquivalentDecls() { return NonEquivalentDecls; }





virtual void CompleteDecl(Decl* D);



virtual void Imported(Decl *From, Decl *To) {}

void RegisterImportedDecl(Decl *FromD, Decl *ToD);




Decl *MapImported(Decl *From, Decl *To);







virtual Decl *GetOriginalDecl(Decl *To) { return nullptr; }




llvm::Optional<ImportError> getImportDeclErrorIfAny(Decl *FromD) const;


void setImportDeclError(Decl *From, ImportError Error);



bool IsStructurallyEquivalent(QualType From, QualType To,
bool Complain = true);





static llvm::Optional<unsigned> getFieldIndex(Decl *F);
};

}

#endif
