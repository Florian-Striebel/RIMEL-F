










#if !defined(LLVM_CLANG_SEMA_EXTERNALSEMASOURCE_H)
#define LLVM_CLANG_SEMA_EXTERNALSEMASOURCE_H

#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/Type.h"
#include "clang/Sema/TypoCorrection.h"
#include "clang/Sema/Weak.h"
#include "llvm/ADT/MapVector.h"
#include <utility>

namespace llvm {
template <class T, unsigned n> class SmallSetVector;
}

namespace clang {

class CXXConstructorDecl;
class CXXDeleteExpr;
class CXXRecordDecl;
class DeclaratorDecl;
class LookupResult;
struct ObjCMethodList;
class Scope;
class Sema;
class TypedefNameDecl;
class ValueDecl;
class VarDecl;
struct LateParsedTemplate;



struct ExternalVTableUse {
CXXRecordDecl *Record;
SourceLocation Location;
bool DefinitionRequired;
};




class ExternalSemaSource : public ExternalASTSource {

static char ID;

public:
ExternalSemaSource() = default;

~ExternalSemaSource() override;




virtual void InitializeSema(Sema &S) {}


virtual void ForgetSema() {}



virtual void ReadMethodPool(Selector Sel);



virtual void updateOutOfDateSelector(Selector Sel);



virtual void ReadKnownNamespaces(
SmallVectorImpl<NamespaceDecl *> &Namespaces);



virtual void
ReadUndefinedButUsed(llvm::MapVector<NamedDecl *, SourceLocation> &Undefined);

virtual void ReadMismatchingDeleteExpressions(llvm::MapVector<
FieldDecl *, llvm::SmallVector<std::pair<SourceLocation, bool>, 4>> &);









virtual bool LookupUnqualified(LookupResult &R, Scope *S) { return false; }








virtual void ReadTentativeDefinitions(
SmallVectorImpl<VarDecl *> &TentativeDefs) {}








virtual void ReadUnusedFileScopedDecls(
SmallVectorImpl<const DeclaratorDecl *> &Decls) {}








virtual void ReadDelegatingConstructors(
SmallVectorImpl<CXXConstructorDecl *> &Decls) {}








virtual void ReadExtVectorDecls(SmallVectorImpl<TypedefNameDecl *> &Decls) {}







virtual void ReadUnusedLocalTypedefNameCandidates(
llvm::SmallSetVector<const TypedefNameDecl *, 4> &Decls) {}








virtual void ReadReferencedSelectors(
SmallVectorImpl<std::pair<Selector, SourceLocation> > &Sels) {}








virtual void ReadWeakUndeclaredIdentifiers(
SmallVectorImpl<std::pair<IdentifierInfo *, WeakInfo> > &WI) {}






virtual void ReadUsedVTables(SmallVectorImpl<ExternalVTableUse> &VTables) {}








virtual void ReadPendingInstantiations(
SmallVectorImpl<std::pair<ValueDecl *,
SourceLocation> > &Pending) {}







virtual void ReadLateParsedTemplates(
llvm::MapVector<const FunctionDecl *, std::unique_ptr<LateParsedTemplate>>
&LPTMap) {}







virtual void
ReadDeclsToCheckForDeferredDiags(llvm::SmallSetVector<Decl *, 4> &Decls) {}








virtual TypoCorrection CorrectTypo(const DeclarationNameInfo &Typo,
int LookupKind, Scope *S, CXXScopeSpec *SS,
CorrectionCandidateCallback &CCC,
DeclContext *MemberContext,
bool EnteringContext,
const ObjCObjectPointerType *OPT) {
return TypoCorrection();
}










virtual bool MaybeDiagnoseMissingCompleteType(SourceLocation Loc,
QualType T) {
return false;
}



bool isA(const void *ClassID) const override {
return ClassID == &ID || ExternalASTSource::isA(ClassID);
}
static bool classof(const ExternalASTSource *S) { return S->isA(&ID); }

};

}

#endif
