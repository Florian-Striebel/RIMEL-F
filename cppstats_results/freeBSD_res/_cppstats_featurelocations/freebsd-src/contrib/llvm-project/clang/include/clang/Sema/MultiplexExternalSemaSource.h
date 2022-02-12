










#if !defined(LLVM_CLANG_SEMA_MULTIPLEXEXTERNALSEMASOURCE_H)
#define LLVM_CLANG_SEMA_MULTIPLEXEXTERNALSEMASOURCE_H

#include "clang/Sema/ExternalSemaSource.h"
#include "clang/Sema/Weak.h"
#include "llvm/ADT/SmallVector.h"
#include <utility>

namespace clang {

class CXXConstructorDecl;
class CXXRecordDecl;
class DeclaratorDecl;
struct ExternalVTableUse;
class LookupResult;
class NamespaceDecl;
class Scope;
class Sema;
class TypedefNameDecl;
class ValueDecl;
class VarDecl;





class MultiplexExternalSemaSource : public ExternalSemaSource {

static char ID;

private:
SmallVector<ExternalSemaSource *, 2> Sources;

public:







MultiplexExternalSemaSource(ExternalSemaSource& s1, ExternalSemaSource& s2);

~MultiplexExternalSemaSource() override;





void addSource(ExternalSemaSource &source);







Decl *GetExternalDecl(uint32_t ID) override;



void CompleteRedeclChain(const Decl *D) override;


Selector GetExternalSelector(uint32_t ID) override;



uint32_t GetNumExternalSelectors() override;



Stmt *GetExternalDeclStmt(uint64_t Offset) override;



CXXBaseSpecifier *GetExternalCXXBaseSpecifiers(uint64_t Offset) override;



CXXCtorInitializer **GetExternalCXXCtorInitializers(uint64_t Offset) override;

ExtKind hasExternalDefinitions(const Decl *D) override;



bool FindExternalVisibleDeclsByName(const DeclContext *DC,
DeclarationName Name) override;



void completeVisibleDeclsMap(const DeclContext *DC) override;






void
FindExternalLexicalDecls(const DeclContext *DC,
llvm::function_ref<bool(Decl::Kind)> IsKindWeWant,
SmallVectorImpl<Decl *> &Result) override;




void FindFileRegionDecls(FileID File, unsigned Offset,unsigned Length,
SmallVectorImpl<Decl *> &Decls) override;



void CompleteType(TagDecl *Tag) override;







void CompleteType(ObjCInterfaceDecl *Class) override;


void ReadComments() override;




void StartedDeserializing() override;



void FinishedDeserializing() override;



void StartTranslationUnit(ASTConsumer *Consumer) override;



void PrintStats() override;


Module *getModule(unsigned ID) override;




























bool
layoutRecordType(const RecordDecl *Record,
uint64_t &Size, uint64_t &Alignment,
llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
llvm::DenseMap<const CXXRecordDecl *,
CharUnits> &VirtualBaseOffsets) override;



void getMemoryBufferSizes(MemoryBufferSizes &sizes) const override;








void InitializeSema(Sema &S) override;


void ForgetSema() override;



void ReadMethodPool(Selector Sel) override;



void updateOutOfDateSelector(Selector Sel) override;



void
ReadKnownNamespaces(SmallVectorImpl<NamespaceDecl*> &Namespaces) override;



void ReadUndefinedButUsed(
llvm::MapVector<NamedDecl *, SourceLocation> &Undefined) override;

void ReadMismatchingDeleteExpressions(llvm::MapVector<
FieldDecl *, llvm::SmallVector<std::pair<SourceLocation, bool>, 4>> &
Exprs) override;









bool LookupUnqualified(LookupResult &R, Scope *S) override;








void ReadTentativeDefinitions(SmallVectorImpl<VarDecl*> &Defs) override;








void ReadUnusedFileScopedDecls(
SmallVectorImpl<const DeclaratorDecl*> &Decls) override;








void ReadDelegatingConstructors(
SmallVectorImpl<CXXConstructorDecl*> &Decls) override;








void ReadExtVectorDecls(SmallVectorImpl<TypedefNameDecl*> &Decls) override;







void ReadUnusedLocalTypedefNameCandidates(
llvm::SmallSetVector<const TypedefNameDecl *, 4> &Decls) override;








void ReadReferencedSelectors(SmallVectorImpl<std::pair<Selector,
SourceLocation> > &Sels) override;








void ReadWeakUndeclaredIdentifiers(
SmallVectorImpl<std::pair<IdentifierInfo*, WeakInfo> > &WI) override;






void ReadUsedVTables(SmallVectorImpl<ExternalVTableUse> &VTables) override;








void ReadPendingInstantiations(
SmallVectorImpl<std::pair<ValueDecl*, SourceLocation> >& Pending) override;







void ReadLateParsedTemplates(
llvm::MapVector<const FunctionDecl *, std::unique_ptr<LateParsedTemplate>>
&LPTMap) override;







void ReadDeclsToCheckForDeferredDiags(
llvm::SmallSetVector<Decl *, 4> &Decls) override;



TypoCorrection CorrectTypo(const DeclarationNameInfo &Typo,
int LookupKind, Scope *S, CXXScopeSpec *SS,
CorrectionCandidateCallback &CCC,
DeclContext *MemberContext,
bool EnteringContext,
const ObjCObjectPointerType *OPT) override;











bool MaybeDiagnoseMissingCompleteType(SourceLocation Loc,
QualType T) override;



bool isA(const void *ClassID) const override {
return ClassID == &ID || ExternalSemaSource::isA(ClassID);
}
static bool classof(const ExternalASTSource *S) { return S->isA(&ID); }

};

}

#endif
