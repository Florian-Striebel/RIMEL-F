











#if !defined(LLVM_CLANG_AST_DECL_H)
#define LLVM_CLANG_AST_DECL_H

#include "clang/AST/APValue.h"
#include "clang/AST/ASTContextAllocate.h"
#include "clang/AST/DeclAccessPair.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/Redeclarable.h"
#include "clang/AST/Type.h"
#include "clang/Basic/AddressSpaces.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/Linkage.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/PragmaKinds.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Basic/Visibility.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace clang {

class ASTContext;
struct ASTTemplateArgumentListInfo;
class Attr;
class CompoundStmt;
class DependentFunctionTemplateSpecializationInfo;
class EnumDecl;
class Expr;
class FunctionTemplateDecl;
class FunctionTemplateSpecializationInfo;
class FunctionTypeLoc;
class LabelStmt;
class MemberSpecializationInfo;
class Module;
class NamespaceDecl;
class ParmVarDecl;
class RecordDecl;
class Stmt;
class StringLiteral;
class TagDecl;
class TemplateArgumentList;
class TemplateArgumentListInfo;
class TemplateParameterList;
class TypeAliasTemplateDecl;
class TypeLoc;
class UnresolvedSetImpl;
class VarTemplateDecl;


class TranslationUnitDecl : public Decl,
public DeclContext,
public Redeclarable<TranslationUnitDecl> {
using redeclarable_base = Redeclarable<TranslationUnitDecl>;

TranslationUnitDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

TranslationUnitDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

TranslationUnitDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

ASTContext &Ctx;



NamespaceDecl *AnonymousNamespace = nullptr;

explicit TranslationUnitDecl(ASTContext &ctx);

virtual void anchor();

public:
using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::getMostRecentDecl;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::isFirstDecl;
using redeclarable_base::redecls;
using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;

ASTContext &getASTContext() const { return Ctx; }

NamespaceDecl *getAnonymousNamespace() const { return AnonymousNamespace; }
void setAnonymousNamespace(NamespaceDecl *D) { AnonymousNamespace = D; }

static TranslationUnitDecl *Create(ASTContext &C);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == TranslationUnit; }
static DeclContext *castToDeclContext(const TranslationUnitDecl *D) {
return static_cast<DeclContext *>(const_cast<TranslationUnitDecl*>(D));
}
static TranslationUnitDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<TranslationUnitDecl *>(const_cast<DeclContext*>(DC));
}
};



class PragmaCommentDecl final
: public Decl,
private llvm::TrailingObjects<PragmaCommentDecl, char> {
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend TrailingObjects;

PragmaMSCommentKind CommentKind;

PragmaCommentDecl(TranslationUnitDecl *TU, SourceLocation CommentLoc,
PragmaMSCommentKind CommentKind)
: Decl(PragmaComment, TU, CommentLoc), CommentKind(CommentKind) {}

virtual void anchor();

public:
static PragmaCommentDecl *Create(const ASTContext &C, TranslationUnitDecl *DC,
SourceLocation CommentLoc,
PragmaMSCommentKind CommentKind,
StringRef Arg);
static PragmaCommentDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned ArgSize);

PragmaMSCommentKind getCommentKind() const { return CommentKind; }

StringRef getArg() const { return getTrailingObjects<char>(); }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == PragmaComment; }
};



class PragmaDetectMismatchDecl final
: public Decl,
private llvm::TrailingObjects<PragmaDetectMismatchDecl, char> {
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend TrailingObjects;

size_t ValueStart;

PragmaDetectMismatchDecl(TranslationUnitDecl *TU, SourceLocation Loc,
size_t ValueStart)
: Decl(PragmaDetectMismatch, TU, Loc), ValueStart(ValueStart) {}

virtual void anchor();

public:
static PragmaDetectMismatchDecl *Create(const ASTContext &C,
TranslationUnitDecl *DC,
SourceLocation Loc, StringRef Name,
StringRef Value);
static PragmaDetectMismatchDecl *
CreateDeserialized(ASTContext &C, unsigned ID, unsigned NameValueSize);

StringRef getName() const { return getTrailingObjects<char>(); }
StringRef getValue() const { return getTrailingObjects<char>() + ValueStart; }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == PragmaDetectMismatch; }
};


















class ExternCContextDecl : public Decl, public DeclContext {
explicit ExternCContextDecl(TranslationUnitDecl *TU)
: Decl(ExternCContext, TU, SourceLocation()),
DeclContext(ExternCContext) {}

virtual void anchor();

public:
static ExternCContextDecl *Create(const ASTContext &C,
TranslationUnitDecl *TU);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ExternCContext; }
static DeclContext *castToDeclContext(const ExternCContextDecl *D) {
return static_cast<DeclContext *>(const_cast<ExternCContextDecl*>(D));
}
static ExternCContextDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<ExternCContextDecl *>(const_cast<DeclContext*>(DC));
}
};






class NamedDecl : public Decl {



DeclarationName Name;

virtual void anchor();

private:
NamedDecl *getUnderlyingDeclImpl() LLVM_READONLY;

protected:
NamedDecl(Kind DK, DeclContext *DC, SourceLocation L, DeclarationName N)
: Decl(DK, DC, L), Name(N) {}

public:





IdentifierInfo *getIdentifier() const { return Name.getAsIdentifierInfo(); }





StringRef getName() const {
assert(Name.isIdentifier() && "Name is not a simple identifier");
return getIdentifier() ? getIdentifier()->getName() : "";
}












std::string getNameAsString() const { return Name.getAsString(); }



virtual void printName(raw_ostream &os) const;
















DeclarationName getDeclName() const { return Name; }


void setDeclName(DeclarationName N) { Name = N; }









void printQualifiedName(raw_ostream &OS) const;
void printQualifiedName(raw_ostream &OS, const PrintingPolicy &Policy) const;





void printNestedNameSpecifier(raw_ostream &OS) const;
void printNestedNameSpecifier(raw_ostream &OS,
const PrintingPolicy &Policy) const;


std::string getQualifiedNameAsString() const;







virtual void getNameForDiagnostic(raw_ostream &OS,
const PrintingPolicy &Policy,
bool Qualified) const;











bool declarationReplaces(NamedDecl *OldD, bool IsKnownNewer = true) const;


bool hasLinkage() const;

using Decl::isModulePrivate;
using Decl::setModulePrivate;


bool isCXXClassMember() const {
const DeclContext *DC = getDeclContext();




if (isa<EnumDecl>(DC))
DC = DC->getRedeclContext();

return DC->isRecord();
}



bool isCXXInstanceMember() const;



ReservedIdentifierStatus isReserved(const LangOptions &LangOpts) const;






Linkage getLinkageInternal() const;



Linkage getFormalLinkage() const {
return clang::getFormalLinkage(getLinkageInternal());
}


bool hasExternalFormalLinkage() const {
return isExternalFormalLinkage(getLinkageInternal());
}

bool isExternallyVisible() const {
return clang::isExternallyVisible(getLinkageInternal());
}



bool isExternallyDeclarable() const {
return isExternallyVisible() && !getOwningModuleForLinkage();
}


Visibility getVisibility() const {
return getLinkageAndVisibility().getVisibility();
}


LinkageInfo getLinkageAndVisibility() const;


enum ExplicitVisibilityKind {



VisibilityForType,




VisibilityForValue
};



Optional<Visibility>
getExplicitVisibility(ExplicitVisibilityKind kind) const;



bool isLinkageValid() const;







bool hasLinkageBeenComputed() const {
return hasCachedLinkage();
}



NamedDecl *getUnderlyingDecl() {

if (this->getKind() != UsingShadow &&
this->getKind() != ConstructorUsingShadow &&
this->getKind() != ObjCCompatibleAlias &&
this->getKind() != NamespaceAlias)
return this;

return getUnderlyingDeclImpl();
}
const NamedDecl *getUnderlyingDecl() const {
return const_cast<NamedDecl*>(this)->getUnderlyingDecl();
}

NamedDecl *getMostRecentDecl() {
return cast<NamedDecl>(static_cast<Decl *>(this)->getMostRecentDecl());
}
const NamedDecl *getMostRecentDecl() const {
return const_cast<NamedDecl*>(this)->getMostRecentDecl();
}

ObjCStringFormatFamily getObjCFStringFormattingFamily() const;

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K >= firstNamed && K <= lastNamed; }
};

inline raw_ostream &operator<<(raw_ostream &OS, const NamedDecl &ND) {
ND.printName(OS);
return OS;
}






class LabelDecl : public NamedDecl {
LabelStmt *TheStmt;
StringRef MSAsmName;
bool MSAsmNameResolved = false;




SourceLocation LocStart;

LabelDecl(DeclContext *DC, SourceLocation IdentL, IdentifierInfo *II,
LabelStmt *S, SourceLocation StartL)
: NamedDecl(Label, DC, IdentL, II), TheStmt(S), LocStart(StartL) {}

void anchor() override;

public:
static LabelDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation IdentL, IdentifierInfo *II);
static LabelDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation IdentL, IdentifierInfo *II,
SourceLocation GnuLabelL);
static LabelDecl *CreateDeserialized(ASTContext &C, unsigned ID);

LabelStmt *getStmt() const { return TheStmt; }
void setStmt(LabelStmt *T) { TheStmt = T; }

bool isGnuLocal() const { return LocStart != getLocation(); }
void setLocStart(SourceLocation L) { LocStart = L; }

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(LocStart, getLocation());
}

bool isMSAsmLabel() const { return !MSAsmName.empty(); }
bool isResolvedMSAsmLabel() const { return isMSAsmLabel() && MSAsmNameResolved; }
void setMSAsmLabel(StringRef Name);
StringRef getMSAsmLabel() const { return MSAsmName; }
void setMSAsmLabelResolved() { MSAsmNameResolved = true; }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Label; }
};


class NamespaceDecl : public NamedDecl, public DeclContext,
public Redeclarable<NamespaceDecl>
{


SourceLocation LocStart;


SourceLocation RBraceLoc;





llvm::PointerIntPair<NamespaceDecl *, 1, bool> AnonOrFirstNamespaceAndInline;

NamespaceDecl(ASTContext &C, DeclContext *DC, bool Inline,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, NamespaceDecl *PrevDecl);

using redeclarable_base = Redeclarable<NamespaceDecl>;

NamespaceDecl *getNextRedeclarationImpl() override;
NamespaceDecl *getPreviousDeclImpl() override;
NamespaceDecl *getMostRecentDeclImpl() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static NamespaceDecl *Create(ASTContext &C, DeclContext *DC,
bool Inline, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id,
NamespaceDecl *PrevDecl);

static NamespaceDecl *CreateDeserialized(ASTContext &C, unsigned ID);

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;










bool isAnonymousNamespace() const {
return !getIdentifier();
}


bool isInline() const {
return AnonOrFirstNamespaceAndInline.getInt();
}


void setInline(bool Inline) {
AnonOrFirstNamespaceAndInline.setInt(Inline);
}


bool isRedundantInlineQualifierFor(DeclarationName Name) const {
if (!isInline())
return false;
auto X = lookup(Name);
auto Y = getParent()->lookup(Name);
return std::distance(X.begin(), X.end()) ==
std::distance(Y.begin(), Y.end());
}


NamespaceDecl *getOriginalNamespace();


const NamespaceDecl *getOriginalNamespace() const;




bool isOriginalNamespace() const;



NamespaceDecl *getAnonymousNamespace() const {
return getOriginalNamespace()->AnonOrFirstNamespaceAndInline.getPointer();
}

void setAnonymousNamespace(NamespaceDecl *D) {
getOriginalNamespace()->AnonOrFirstNamespaceAndInline.setPointer(D);
}


NamespaceDecl *getCanonicalDecl() override {
return getOriginalNamespace();
}
const NamespaceDecl *getCanonicalDecl() const {
return getOriginalNamespace();
}

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(LocStart, RBraceLoc);
}

SourceLocation getBeginLoc() const LLVM_READONLY { return LocStart; }
SourceLocation getRBraceLoc() const { return RBraceLoc; }
void setLocStart(SourceLocation L) { LocStart = L; }
void setRBraceLoc(SourceLocation L) { RBraceLoc = L; }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Namespace; }
static DeclContext *castToDeclContext(const NamespaceDecl *D) {
return static_cast<DeclContext *>(const_cast<NamespaceDecl*>(D));
}
static NamespaceDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<NamespaceDecl *>(const_cast<DeclContext*>(DC));
}
};




class ValueDecl : public NamedDecl {
QualType DeclType;

void anchor() override;

protected:
ValueDecl(Kind DK, DeclContext *DC, SourceLocation L,
DeclarationName N, QualType T)
: NamedDecl(DK, DC, L, N), DeclType(T) {}

public:
QualType getType() const { return DeclType; }
void setType(QualType newType) { DeclType = newType; }



bool isWeak() const;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K >= firstValue && K <= lastValue; }
};



struct QualifierInfo {
NestedNameSpecifierLoc QualifierLoc;





unsigned NumTemplParamLists = 0;






TemplateParameterList** TemplParamLists = nullptr;

QualifierInfo() = default;
QualifierInfo(const QualifierInfo &) = delete;
QualifierInfo& operator=(const QualifierInfo &) = delete;


void setTemplateParameterListsInfo(ASTContext &Context,
ArrayRef<TemplateParameterList *> TPLists);
};



class DeclaratorDecl : public ValueDecl {



struct ExtInfo : public QualifierInfo {
TypeSourceInfo *TInfo;
Expr *TrailingRequiresClause = nullptr;
};

llvm::PointerUnion<TypeSourceInfo *, ExtInfo *> DeclInfo;



SourceLocation InnerLocStart;

bool hasExtInfo() const { return DeclInfo.is<ExtInfo*>(); }
ExtInfo *getExtInfo() { return DeclInfo.get<ExtInfo*>(); }
const ExtInfo *getExtInfo() const { return DeclInfo.get<ExtInfo*>(); }

protected:
DeclaratorDecl(Kind DK, DeclContext *DC, SourceLocation L,
DeclarationName N, QualType T, TypeSourceInfo *TInfo,
SourceLocation StartL)
: ValueDecl(DK, DC, L, N, T), DeclInfo(TInfo), InnerLocStart(StartL) {}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

TypeSourceInfo *getTypeSourceInfo() const {
return hasExtInfo()
? getExtInfo()->TInfo
: DeclInfo.get<TypeSourceInfo*>();
}

void setTypeSourceInfo(TypeSourceInfo *TI) {
if (hasExtInfo())
getExtInfo()->TInfo = TI;
else
DeclInfo = TI;
}


SourceLocation getInnerLocStart() const { return InnerLocStart; }
void setInnerLocStart(SourceLocation L) { InnerLocStart = L; }



SourceLocation getOuterLocStart() const;

SourceRange getSourceRange() const override LLVM_READONLY;

SourceLocation getBeginLoc() const LLVM_READONLY {
return getOuterLocStart();
}



NestedNameSpecifier *getQualifier() const {
return hasExtInfo() ? getExtInfo()->QualifierLoc.getNestedNameSpecifier()
: nullptr;
}




NestedNameSpecifierLoc getQualifierLoc() const {
return hasExtInfo() ? getExtInfo()->QualifierLoc
: NestedNameSpecifierLoc();
}

void setQualifierInfo(NestedNameSpecifierLoc QualifierLoc);




Expr *getTrailingRequiresClause() {
return hasExtInfo() ? getExtInfo()->TrailingRequiresClause
: nullptr;
}

const Expr *getTrailingRequiresClause() const {
return hasExtInfo() ? getExtInfo()->TrailingRequiresClause
: nullptr;
}

void setTrailingRequiresClause(Expr *TrailingRequiresClause);

unsigned getNumTemplateParameterLists() const {
return hasExtInfo() ? getExtInfo()->NumTemplParamLists : 0;
}

TemplateParameterList *getTemplateParameterList(unsigned index) const {
assert(index < getNumTemplateParameterLists());
return getExtInfo()->TemplParamLists[index];
}

void setTemplateParameterListsInfo(ASTContext &Context,
ArrayRef<TemplateParameterList *> TPLists);

SourceLocation getTypeSpecStartLoc() const;
SourceLocation getTypeSpecEndLoc() const;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K >= firstDeclarator && K <= lastDeclarator;
}
};




struct EvaluatedStmt {

bool WasEvaluated : 1;


bool IsEvaluating : 1;





bool HasConstantInitialization : 1;






bool HasConstantDestruction : 1;



bool HasICEInit : 1;
bool CheckedForICEInit : 1;

Stmt *Value;
APValue Evaluated;

EvaluatedStmt()
: WasEvaluated(false), IsEvaluating(false),
HasConstantInitialization(false), HasConstantDestruction(false),
HasICEInit(false), CheckedForICEInit(false) {}
};


class VarDecl : public DeclaratorDecl, public Redeclarable<VarDecl> {
public:

enum InitializationStyle {

CInit,


CallInit,


ListInit
};


enum TLSKind {

TLS_None,


TLS_Static,


TLS_Dynamic
};




static const char *getStorageClassSpecifierString(StorageClass SC);

protected:







using InitType = llvm::PointerUnion<Stmt *, EvaluatedStmt *>;



mutable InitType Init;

private:
friend class ASTDeclReader;
friend class ASTNodeImporter;
friend class StmtIteratorBase;

class VarDeclBitfields {
friend class ASTDeclReader;
friend class VarDecl;

unsigned SClass : 3;
unsigned TSCSpec : 2;
unsigned InitStyle : 2;



unsigned ARCPseudoStrong : 1;
};
enum { NumVarDeclBits = 8 };

protected:
enum { NumParameterIndexBits = 8 };

enum DefaultArgKind {
DAK_None,
DAK_Unparsed,
DAK_Uninstantiated,
DAK_Normal
};

enum { NumScopeDepthOrObjCQualsBits = 7 };

class ParmVarDeclBitfields {
friend class ASTDeclReader;
friend class ParmVarDecl;

unsigned : NumVarDeclBits;



unsigned HasInheritedDefaultArg : 1;





unsigned DefaultArgKind : 2;


unsigned IsKNRPromoted : 1;


unsigned IsObjCMethodParam : 1;





unsigned ScopeDepthOrObjCQuals : NumScopeDepthOrObjCQualsBits;



unsigned ParameterIndex : NumParameterIndexBits;
};

class NonParmVarDeclBitfields {
friend class ASTDeclReader;
friend class ImplicitParamDecl;
friend class VarDecl;

unsigned : NumVarDeclBits;




unsigned IsThisDeclarationADemotedDefinition : 1;



unsigned ExceptionVar : 1;




unsigned NRVOVariable : 1;



unsigned CXXForRangeDecl : 1;


unsigned ObjCForDecl : 1;


unsigned IsInline : 1;


unsigned IsInlineSpecified : 1;


unsigned IsConstexpr : 1;



unsigned IsInitCapture : 1;




unsigned PreviousDeclInSameBlockScope : 1;



unsigned ImplicitParamKind : 3;

unsigned EscapingByref : 1;
};

union {
unsigned AllBits;
VarDeclBitfields VarDeclBits;
ParmVarDeclBitfields ParmVarDeclBits;
NonParmVarDeclBitfields NonParmVarDeclBits;
};

VarDecl(Kind DK, ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id, QualType T,
TypeSourceInfo *TInfo, StorageClass SC);

using redeclarable_base = Redeclarable<VarDecl>;

VarDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

VarDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

VarDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

public:
using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;

static VarDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, QualType T, TypeSourceInfo *TInfo,
StorageClass S);

static VarDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;



StorageClass getStorageClass() const {
return (StorageClass) VarDeclBits.SClass;
}
void setStorageClass(StorageClass SC);

void setTSCSpec(ThreadStorageClassSpecifier TSC) {
VarDeclBits.TSCSpec = TSC;
assert(VarDeclBits.TSCSpec == TSC && "truncation");
}
ThreadStorageClassSpecifier getTSCSpec() const {
return static_cast<ThreadStorageClassSpecifier>(VarDeclBits.TSCSpec);
}
TLSKind getTLSKind() const;



bool hasLocalStorage() const {
if (getStorageClass() == SC_None) {




if (getType().getAddressSpace() == LangAS::opencl_constant)
return false;

return !isFileVarDecl() && getTSCSpec() == TSCS_unspecified;
}


if (getStorageClass() == SC_Register && !isLocalVarDeclOrParm())
return false;




return getStorageClass() >= SC_Auto;
}



bool isStaticLocal() const {
return (getStorageClass() == SC_Static ||

(getStorageClass() == SC_None && getTSCSpec() == TSCS_thread_local))
&& !isFileVarDecl();
}



bool hasExternalStorage() const {
return getStorageClass() == SC_Extern ||
getStorageClass() == SC_PrivateExtern;
}





bool hasGlobalStorage() const { return !hasLocalStorage(); }


StorageDuration getStorageDuration() const {
return hasLocalStorage() ? SD_Automatic :
getTSCSpec() ? SD_Thread : SD_Static;
}


LanguageLinkage getLanguageLinkage() const;


bool isExternC() const;



bool isInExternCContext() const;



bool isInExternCXXContext() const;






bool isLocalVarDecl() const {
if (getKind() != Decl::Var && getKind() != Decl::Decomposition)
return false;
if (const DeclContext *DC = getLexicalDeclContext())
return DC->getRedeclContext()->isFunctionOrMethod();
return false;
}


bool isLocalVarDeclOrParm() const {
return isLocalVarDecl() || getKind() == Decl::ParmVar;
}


bool isFunctionOrMethodVarDecl() const {
if (getKind() != Decl::Var && getKind() != Decl::Decomposition)
return false;
const DeclContext *DC = getLexicalDeclContext()->getRedeclContext();
return DC->isFunctionOrMethod() && DC->getDeclKind() != Decl::Block;
}










bool isStaticDataMember() const {

return getKind() != Decl::ParmVar && getDeclContext()->isRecord();
}

VarDecl *getCanonicalDecl() override;
const VarDecl *getCanonicalDecl() const {
return const_cast<VarDecl*>(this)->getCanonicalDecl();
}

enum DefinitionKind {

DeclarationOnly,


TentativeDefinition,


Definition
};




DefinitionKind isThisDeclarationADefinition(ASTContext &) const;
DefinitionKind isThisDeclarationADefinition() const {
return isThisDeclarationADefinition(getASTContext());
}


DefinitionKind hasDefinition(ASTContext &) const;
DefinitionKind hasDefinition() const {
return hasDefinition(getASTContext());
}



VarDecl *getActingDefinition();
const VarDecl *getActingDefinition() const {
return const_cast<VarDecl*>(this)->getActingDefinition();
}


VarDecl *getDefinition(ASTContext &);
const VarDecl *getDefinition(ASTContext &C) const {
return const_cast<VarDecl*>(this)->getDefinition(C);
}
VarDecl *getDefinition() {
return getDefinition(getASTContext());
}
const VarDecl *getDefinition() const {
return const_cast<VarDecl*>(this)->getDefinition();
}



bool isOutOfLine() const override;


bool isFileVarDecl() const {
Kind K = getKind();
if (K == ParmVar || K == ImplicitParam)
return false;

if (getLexicalDeclContext()->getRedeclContext()->isFileContext())
return true;

if (isStaticDataMember())
return true;

return false;
}



const Expr *getAnyInitializer() const {
const VarDecl *D;
return getAnyInitializer(D);
}



const Expr *getAnyInitializer(const VarDecl *&D) const;

bool hasInit() const;
const Expr *getInit() const {
return const_cast<VarDecl *>(this)->getInit();
}
Expr *getInit();


Stmt **getInitAddress();

void setInit(Expr *I);




VarDecl *getInitializingDeclaration();
const VarDecl *getInitializingDeclaration() const {
return const_cast<VarDecl *>(this)->getInitializingDeclaration();
}








bool mightBeUsableInConstantExpressions(const ASTContext &C) const;




bool isUsableInConstantExpressions(const ASTContext &C) const;

EvaluatedStmt *ensureEvaluatedStmt() const;
EvaluatedStmt *getEvaluatedStmt() const;





APValue *evaluateValue() const;
APValue *evaluateValue(SmallVectorImpl<PartialDiagnosticAt> &Notes) const;




APValue *getEvaluatedValue() const;







bool evaluateDestruction(SmallVectorImpl<PartialDiagnosticAt> &Notes) const;







bool hasConstantInitialization() const;




bool hasICEInitializer(const ASTContext &Context) const;




bool checkForConstantInitialization(
SmallVectorImpl<PartialDiagnosticAt> &Notes) const;

void setInitStyle(InitializationStyle Style) {
VarDeclBits.InitStyle = Style;
}











InitializationStyle getInitStyle() const {
return static_cast<InitializationStyle>(VarDeclBits.InitStyle);
}


bool isDirectInit() const {
return getInitStyle() != CInit;
}


bool isThisDeclarationADemotedDefinition() const {
return isa<ParmVarDecl>(this) ? false :
NonParmVarDeclBits.IsThisDeclarationADemotedDefinition;
}






void demoteThisDefinitionToDeclaration() {
assert(isThisDeclarationADefinition() && "Not a definition!");
assert(!isa<ParmVarDecl>(this) && "Cannot demote ParmVarDecls!");
NonParmVarDeclBits.IsThisDeclarationADemotedDefinition = 1;
}



bool isExceptionVariable() const {
return isa<ParmVarDecl>(this) ? false : NonParmVarDeclBits.ExceptionVar;
}
void setExceptionVariable(bool EV) {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.ExceptionVar = EV;
}











bool isNRVOVariable() const {
return isa<ParmVarDecl>(this) ? false : NonParmVarDeclBits.NRVOVariable;
}
void setNRVOVariable(bool NRVO) {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.NRVOVariable = NRVO;
}



bool isCXXForRangeDecl() const {
return isa<ParmVarDecl>(this) ? false : NonParmVarDeclBits.CXXForRangeDecl;
}
void setCXXForRangeDecl(bool FRD) {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.CXXForRangeDecl = FRD;
}



bool isObjCForDecl() const {
return NonParmVarDeclBits.ObjCForDecl;
}

void setObjCForDecl(bool FRD) {
NonParmVarDeclBits.ObjCForDecl = FRD;
}








bool isARCPseudoStrong() const { return VarDeclBits.ARCPseudoStrong; }
void setARCPseudoStrong(bool PS) { VarDeclBits.ARCPseudoStrong = PS; }


bool isInline() const {
return isa<ParmVarDecl>(this) ? false : NonParmVarDeclBits.IsInline;
}
bool isInlineSpecified() const {
return isa<ParmVarDecl>(this) ? false
: NonParmVarDeclBits.IsInlineSpecified;
}
void setInlineSpecified() {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.IsInline = true;
NonParmVarDeclBits.IsInlineSpecified = true;
}
void setImplicitlyInline() {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.IsInline = true;
}


bool isConstexpr() const {
return isa<ParmVarDecl>(this) ? false : NonParmVarDeclBits.IsConstexpr;
}
void setConstexpr(bool IC) {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.IsConstexpr = IC;
}


bool isInitCapture() const {
return isa<ParmVarDecl>(this) ? false : NonParmVarDeclBits.IsInitCapture;
}
void setInitCapture(bool IC) {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.IsInitCapture = IC;
}



bool isParameterPack() const;



bool isPreviousDeclInSameBlockScope() const {
return isa<ParmVarDecl>(this)
? false
: NonParmVarDeclBits.PreviousDeclInSameBlockScope;
}
void setPreviousDeclInSameBlockScope(bool Same) {
assert(!isa<ParmVarDecl>(this));
NonParmVarDeclBits.PreviousDeclInSameBlockScope = Same;
}




bool isEscapingByref() const;



bool isNonEscapingByref() const;

void setEscapingByref() {
NonParmVarDeclBits.EscapingByref = true;
}


bool hasDependentAlignment() const;



VarDecl *getTemplateInstantiationPattern() const;




VarDecl *getInstantiatedFromStaticDataMember() const;




TemplateSpecializationKind getTemplateSpecializationKind() const;




TemplateSpecializationKind
getTemplateSpecializationKindForInstantiation() const;




SourceLocation getPointOfInstantiation() const;




MemberSpecializationInfo *getMemberSpecializationInfo() const;



void setTemplateSpecializationKind(TemplateSpecializationKind TSK,
SourceLocation PointOfInstantiation = SourceLocation());



void setInstantiationOfStaticDataMember(VarDecl *VD,
TemplateSpecializationKind TSK);












VarTemplateDecl *getDescribedVarTemplate() const;

void setDescribedVarTemplate(VarTemplateDecl *Template);




bool isKnownToBeDefined() const;



bool isNoDestroy(const ASTContext &) const;



QualType::DestructionKind needsDestruction(const ASTContext &Ctx) const;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K >= firstVar && K <= lastVar; }
};

class ImplicitParamDecl : public VarDecl {
void anchor() override;

public:



enum ImplicitParamKind : unsigned {

ObjCSelf,


ObjCCmd,


CXXThis,


CXXVTT,


CapturedContext,


Other,
};


static ImplicitParamDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation IdLoc, IdentifierInfo *Id,
QualType T, ImplicitParamKind ParamKind);
static ImplicitParamDecl *Create(ASTContext &C, QualType T,
ImplicitParamKind ParamKind);

static ImplicitParamDecl *CreateDeserialized(ASTContext &C, unsigned ID);

ImplicitParamDecl(ASTContext &C, DeclContext *DC, SourceLocation IdLoc,
IdentifierInfo *Id, QualType Type,
ImplicitParamKind ParamKind)
: VarDecl(ImplicitParam, C, DC, IdLoc, IdLoc, Id, Type,
nullptr, SC_None) {
NonParmVarDeclBits.ImplicitParamKind = ParamKind;
setImplicit();
}

ImplicitParamDecl(ASTContext &C, QualType Type, ImplicitParamKind ParamKind)
: VarDecl(ImplicitParam, C, nullptr, SourceLocation(),
SourceLocation(), nullptr, Type,
nullptr, SC_None) {
NonParmVarDeclBits.ImplicitParamKind = ParamKind;
setImplicit();
}


ImplicitParamKind getParameterKind() const {
return static_cast<ImplicitParamKind>(NonParmVarDeclBits.ImplicitParamKind);
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ImplicitParam; }
};


class ParmVarDecl : public VarDecl {
public:
enum { MaxFunctionScopeDepth = 255 };
enum { MaxFunctionScopeIndex = 255 };

protected:
ParmVarDecl(Kind DK, ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id, QualType T,
TypeSourceInfo *TInfo, StorageClass S, Expr *DefArg)
: VarDecl(DK, C, DC, StartLoc, IdLoc, Id, T, TInfo, S) {
assert(ParmVarDeclBits.HasInheritedDefaultArg == false);
assert(ParmVarDeclBits.DefaultArgKind == DAK_None);
assert(ParmVarDeclBits.IsKNRPromoted == false);
assert(ParmVarDeclBits.IsObjCMethodParam == false);
setDefaultArg(DefArg);
}

public:
static ParmVarDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id,
QualType T, TypeSourceInfo *TInfo,
StorageClass S, Expr *DefArg);

static ParmVarDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;

void setObjCMethodScopeInfo(unsigned parameterIndex) {
ParmVarDeclBits.IsObjCMethodParam = true;
setParameterIndex(parameterIndex);
}

void setScopeInfo(unsigned scopeDepth, unsigned parameterIndex) {
assert(!ParmVarDeclBits.IsObjCMethodParam);

ParmVarDeclBits.ScopeDepthOrObjCQuals = scopeDepth;
assert(ParmVarDeclBits.ScopeDepthOrObjCQuals == scopeDepth
&& "truncation!");

setParameterIndex(parameterIndex);
}

bool isObjCMethodParameter() const {
return ParmVarDeclBits.IsObjCMethodParam;
}


bool isDestroyedInCallee() const;

unsigned getFunctionScopeDepth() const {
if (ParmVarDeclBits.IsObjCMethodParam) return 0;
return ParmVarDeclBits.ScopeDepthOrObjCQuals;
}

static constexpr unsigned getMaxFunctionScopeDepth() {
return (1u << NumScopeDepthOrObjCQualsBits) - 1;
}


unsigned getFunctionScopeIndex() const {
return getParameterIndex();
}

ObjCDeclQualifier getObjCDeclQualifier() const {
if (!ParmVarDeclBits.IsObjCMethodParam) return OBJC_TQ_None;
return ObjCDeclQualifier(ParmVarDeclBits.ScopeDepthOrObjCQuals);
}
void setObjCDeclQualifier(ObjCDeclQualifier QTVal) {
assert(ParmVarDeclBits.IsObjCMethodParam);
ParmVarDeclBits.ScopeDepthOrObjCQuals = QTVal;
}









bool isKNRPromoted() const {
return ParmVarDeclBits.IsKNRPromoted;
}
void setKNRPromoted(bool promoted) {
ParmVarDeclBits.IsKNRPromoted = promoted;
}

Expr *getDefaultArg();
const Expr *getDefaultArg() const {
return const_cast<ParmVarDecl *>(this)->getDefaultArg();
}

void setDefaultArg(Expr *defarg);



SourceRange getDefaultArgRange() const;
void setUninstantiatedDefaultArg(Expr *arg);
Expr *getUninstantiatedDefaultArg();
const Expr *getUninstantiatedDefaultArg() const {
return const_cast<ParmVarDecl *>(this)->getUninstantiatedDefaultArg();
}



bool hasDefaultArg() const;










bool hasUnparsedDefaultArg() const {
return ParmVarDeclBits.DefaultArgKind == DAK_Unparsed;
}

bool hasUninstantiatedDefaultArg() const {
return ParmVarDeclBits.DefaultArgKind == DAK_Uninstantiated;
}





void setUnparsedDefaultArg() {
ParmVarDeclBits.DefaultArgKind = DAK_Unparsed;
}

bool hasInheritedDefaultArg() const {
return ParmVarDeclBits.HasInheritedDefaultArg;
}

void setHasInheritedDefaultArg(bool I = true) {
ParmVarDeclBits.HasInheritedDefaultArg = I;
}

QualType getOriginalType() const;





void setOwningFunction(DeclContext *FD) { setDeclContext(FD); }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ParmVar; }

private:
enum { ParameterIndexSentinel = (1 << NumParameterIndexBits) - 1 };

void setParameterIndex(unsigned parameterIndex) {
if (parameterIndex >= ParameterIndexSentinel) {
setParameterIndexLarge(parameterIndex);
return;
}

ParmVarDeclBits.ParameterIndex = parameterIndex;
assert(ParmVarDeclBits.ParameterIndex == parameterIndex && "truncation!");
}
unsigned getParameterIndex() const {
unsigned d = ParmVarDeclBits.ParameterIndex;
return d == ParameterIndexSentinel ? getParameterIndexLarge() : d;
}

void setParameterIndexLarge(unsigned parameterIndex);
unsigned getParameterIndexLarge() const;
};

enum class MultiVersionKind {
None,
Target,
CPUSpecific,
CPUDispatch
};











class FunctionDecl : public DeclaratorDecl,
public DeclContext,
public Redeclarable<FunctionDecl> {


public:

enum TemplatedKind {

TK_NonTemplate,

TK_FunctionTemplate,


TK_MemberSpecialization,



TK_FunctionTemplateSpecialization,


TK_DependentFunctionTemplateSpecialization
};



class DefaultedFunctionInfo final
: llvm::TrailingObjects<DefaultedFunctionInfo, DeclAccessPair> {
friend TrailingObjects;
unsigned NumLookups;

public:
static DefaultedFunctionInfo *Create(ASTContext &Context,
ArrayRef<DeclAccessPair> Lookups);


ArrayRef<DeclAccessPair> getUnqualifiedLookups() const {
return {getTrailingObjects<DeclAccessPair>(), NumLookups};
}
};

private:



ParmVarDecl **ParamInfo = nullptr;



union {

LazyDeclStmtPtr Body;

DefaultedFunctionInfo *DefaultedInfo;
};

unsigned ODRHash;








SourceLocation EndRangeLoc;













llvm::PointerUnion<FunctionTemplateDecl *,
MemberSpecializationInfo *,
FunctionTemplateSpecializationInfo *,
DependentFunctionTemplateSpecializationInfo *>
TemplateOrSpecialization;



DeclarationNameLoc DNLoc;






















void setFunctionTemplateSpecialization(ASTContext &C,
FunctionTemplateDecl *Template,
const TemplateArgumentList *TemplateArgs,
void *InsertPos,
TemplateSpecializationKind TSK,
const TemplateArgumentListInfo *TemplateArgsAsWritten,
SourceLocation PointOfInstantiation);



void setInstantiationOfMemberFunction(ASTContext &C, FunctionDecl *FD,
TemplateSpecializationKind TSK);

void setParams(ASTContext &C, ArrayRef<ParmVarDecl *> NewParamInfo);




bool isDeletedBit() const { return FunctionDeclBits.IsDeleted; }


bool hasODRHash() const { return FunctionDeclBits.HasODRHash; }


void setHasODRHash(bool B = true) { FunctionDeclBits.HasODRHash = B; }

protected:
FunctionDecl(Kind DK, ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, StorageClass S, bool isInlineSpecified,
ConstexprSpecKind ConstexprKind,
Expr *TrailingRequiresClause = nullptr);

using redeclarable_base = Redeclarable<FunctionDecl>;

FunctionDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

FunctionDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

FunctionDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;

static FunctionDecl *
Create(ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation NLoc, DeclarationName N, QualType T,
TypeSourceInfo *TInfo, StorageClass SC, bool isInlineSpecified = false,
bool hasWrittenPrototype = true,
ConstexprSpecKind ConstexprKind = ConstexprSpecKind::Unspecified,
Expr *TrailingRequiresClause = nullptr) {
DeclarationNameInfo NameInfo(N, NLoc);
return FunctionDecl::Create(C, DC, StartLoc, NameInfo, T, TInfo, SC,
isInlineSpecified, hasWrittenPrototype,
ConstexprKind, TrailingRequiresClause);
}

static FunctionDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, StorageClass SC,
bool isInlineSpecified, bool hasWrittenPrototype,
ConstexprSpecKind ConstexprKind,
Expr *TrailingRequiresClause);

static FunctionDecl *CreateDeserialized(ASTContext &C, unsigned ID);

DeclarationNameInfo getNameInfo() const {
return DeclarationNameInfo(getDeclName(), getLocation(), DNLoc);
}

void getNameForDiagnostic(raw_ostream &OS, const PrintingPolicy &Policy,
bool Qualified) const override;

void setRangeEnd(SourceLocation E) { EndRangeLoc = E; }


SourceLocation getEllipsisLoc() const {
const auto *FPT = getType()->getAs<FunctionProtoType>();
if (FPT && FPT->isVariadic())
return FPT->getEllipsisLoc();
return SourceLocation();
}

SourceRange getSourceRange() const override LLVM_READONLY;




















bool hasBody(const FunctionDecl *&Definition) const;

bool hasBody() const override {
const FunctionDecl* Definition;
return hasBody(Definition);
}



bool hasTrivialBody() const;












bool isDefined(const FunctionDecl *&Definition,
bool CheckForPendingFriendDefinition = false) const;

bool isDefined() const {
const FunctionDecl* Definition;
return isDefined(Definition);
}


FunctionDecl *getDefinition() {
const FunctionDecl *Definition;
if (isDefined(Definition))
return const_cast<FunctionDecl *>(Definition);
return nullptr;
}
const FunctionDecl *getDefinition() const {
return const_cast<FunctionDecl *>(this)->getDefinition();
}







Stmt *getBody(const FunctionDecl *&Definition) const;

Stmt *getBody() const override {
const FunctionDecl* Definition;
return getBody(Definition);
}










bool isThisDeclarationADefinition() const {
return isDeletedAsWritten() || isDefaulted() ||
doesThisDeclarationHaveABody() || hasSkippedBody() ||
willHaveBody() || hasDefiningAttr();
}




bool isThisDeclarationInstantiatedFromAFriendDefinition() const;


bool doesThisDeclarationHaveABody() const {
return (!FunctionDeclBits.HasDefaultedFunctionInfo && Body) ||
isLateTemplateParsed();
}

void setBody(Stmt *B);
void setLazyBody(uint64_t Offset) {
FunctionDeclBits.HasDefaultedFunctionInfo = false;
Body = LazyDeclStmtPtr(Offset);
}

void setDefaultedFunctionInfo(DefaultedFunctionInfo *Info);
DefaultedFunctionInfo *getDefaultedFunctionInfo() const;


bool isVariadic() const;


bool isVirtualAsWritten() const {
return FunctionDeclBits.IsVirtualAsWritten;
}


void setVirtualAsWritten(bool V) { FunctionDeclBits.IsVirtualAsWritten = V; }



bool isPure() const { return FunctionDeclBits.IsPure; }
void setPure(bool P = true);


bool isLateTemplateParsed() const {
return FunctionDeclBits.IsLateTemplateParsed;
}


void setLateTemplateParsed(bool ILT = true) {
FunctionDeclBits.IsLateTemplateParsed = ILT;
}





bool isTrivial() const { return FunctionDeclBits.IsTrivial; }
void setTrivial(bool IT) { FunctionDeclBits.IsTrivial = IT; }

bool isTrivialForCall() const { return FunctionDeclBits.IsTrivialForCall; }
void setTrivialForCall(bool IT) { FunctionDeclBits.IsTrivialForCall = IT; }



bool isDefaulted() const { return FunctionDeclBits.IsDefaulted; }
void setDefaulted(bool D = true) { FunctionDeclBits.IsDefaulted = D; }


bool isExplicitlyDefaulted() const {
return FunctionDeclBits.IsExplicitlyDefaulted;
}


void setExplicitlyDefaulted(bool ED = true) {
FunctionDeclBits.IsExplicitlyDefaulted = ED;
}



bool isUserProvided() const {
auto *DeclAsWritten = this;
if (FunctionDecl *Pattern = getTemplateInstantiationPattern())
DeclAsWritten = Pattern;
return !(DeclAsWritten->isDeleted() ||
DeclAsWritten->getCanonicalDecl()->isDefaulted());
}




bool hasImplicitReturnZero() const {
return FunctionDeclBits.HasImplicitReturnZero;
}




void setHasImplicitReturnZero(bool IRZ) {
FunctionDeclBits.HasImplicitReturnZero = IRZ;
}





bool hasPrototype() const {
return hasWrittenPrototype() || hasInheritedPrototype();
}


bool hasWrittenPrototype() const {
return FunctionDeclBits.HasWrittenPrototype;
}


void setHasWrittenPrototype(bool P = true) {
FunctionDeclBits.HasWrittenPrototype = P;
}



bool hasInheritedPrototype() const {
return FunctionDeclBits.HasInheritedPrototype;
}



void setHasInheritedPrototype(bool P = true) {
FunctionDeclBits.HasInheritedPrototype = P;
}


bool isConstexpr() const {
return getConstexprKind() != ConstexprSpecKind::Unspecified;
}
void setConstexprKind(ConstexprSpecKind CSK) {
FunctionDeclBits.ConstexprKind = static_cast<uint64_t>(CSK);
}
ConstexprSpecKind getConstexprKind() const {
return static_cast<ConstexprSpecKind>(FunctionDeclBits.ConstexprKind);
}
bool isConstexprSpecified() const {
return getConstexprKind() == ConstexprSpecKind::Constexpr;
}
bool isConsteval() const {
return getConstexprKind() == ConstexprSpecKind::Consteval;
}







bool instantiationIsPending() const {
return FunctionDeclBits.InstantiationIsPending;
}



void setInstantiationIsPending(bool IC) {
FunctionDeclBits.InstantiationIsPending = IC;
}


bool usesSEHTry() const { return FunctionDeclBits.UsesSEHTry; }
void setUsesSEHTry(bool UST) { FunctionDeclBits.UsesSEHTry = UST; }




















bool isDeleted() const {
return getCanonicalDecl()->FunctionDeclBits.IsDeleted;
}

bool isDeletedAsWritten() const {
return FunctionDeclBits.IsDeleted && !isDefaulted();
}

void setDeletedAsWritten(bool D = true) { FunctionDeclBits.IsDeleted = D; }



bool isMain() const;



bool isMSVCRTEntryPoint() const;














bool isReservedGlobalPlacementOperator() const;






















bool isReplaceableGlobalAllocationFunction(
Optional<unsigned> *AlignmentParam = nullptr,
bool *IsNothrow = nullptr) const;


bool isInlineBuiltinDeclaration() const;


bool isDestroyingOperatorDelete() const;


LanguageLinkage getLanguageLinkage() const;



bool isExternC() const;



bool isInExternCContext() const;



bool isInExternCXXContext() const;


bool isGlobal() const;



bool isNoReturn() const;


bool hasSkippedBody() const { return FunctionDeclBits.HasSkippedBody; }
void setHasSkippedBody(bool Skipped = true) {
FunctionDeclBits.HasSkippedBody = Skipped;
}


bool willHaveBody() const { return FunctionDeclBits.WillHaveBody; }
void setWillHaveBody(bool V = true) { FunctionDeclBits.WillHaveBody = V; }


bool isMultiVersion() const {
return getCanonicalDecl()->FunctionDeclBits.IsMultiVersion;
}



void setIsMultiVersion(bool V = true) {
getCanonicalDecl()->FunctionDeclBits.IsMultiVersion = V;
}




MultiVersionKind getMultiVersionKind() const;




bool isCPUDispatchMultiVersion() const;


bool isCPUSpecificMultiVersion() const;



bool isTargetMultiVersion() const;







void getAssociatedConstraints(SmallVectorImpl<const Expr *> &AC) const {
if (auto *TRC = getTrailingRequiresClause())
AC.push_back(TRC);
}

void setPreviousDeclaration(FunctionDecl * PrevDecl);

FunctionDecl *getCanonicalDecl() override;
const FunctionDecl *getCanonicalDecl() const {
return const_cast<FunctionDecl*>(this)->getCanonicalDecl();
}

unsigned getBuiltinID(bool ConsiderWrapperFunctions = false) const;


ArrayRef<ParmVarDecl *> parameters() const {
return {ParamInfo, getNumParams()};
}
MutableArrayRef<ParmVarDecl *> parameters() {
return {ParamInfo, getNumParams()};
}


using param_iterator = MutableArrayRef<ParmVarDecl *>::iterator;
using param_const_iterator = ArrayRef<ParmVarDecl *>::const_iterator;

bool param_empty() const { return parameters().empty(); }
param_iterator param_begin() { return parameters().begin(); }
param_iterator param_end() { return parameters().end(); }
param_const_iterator param_begin() const { return parameters().begin(); }
param_const_iterator param_end() const { return parameters().end(); }
size_t param_size() const { return parameters().size(); }




unsigned getNumParams() const;

const ParmVarDecl *getParamDecl(unsigned i) const {
assert(i < getNumParams() && "Illegal param #");
return ParamInfo[i];
}
ParmVarDecl *getParamDecl(unsigned i) {
assert(i < getNumParams() && "Illegal param #");
return ParamInfo[i];
}
void setParams(ArrayRef<ParmVarDecl *> NewParamInfo) {
setParams(getASTContext(), NewParamInfo);
}




unsigned getMinRequiredArguments() const;







bool hasOneParamOrDefaultArgs() const;





FunctionTypeLoc getFunctionTypeLoc() const;

QualType getReturnType() const {
return getType()->castAs<FunctionType>()->getReturnType();
}




SourceRange getReturnTypeSourceRange() const;





SourceRange getParametersSourceRange() const;



QualType getDeclaredReturnType() const {
auto *TSI = getTypeSourceInfo();
QualType T = TSI ? TSI->getType() : getType();
return T->castAs<FunctionType>()->getReturnType();
}


ExceptionSpecificationType getExceptionSpecType() const {
auto *TSI = getTypeSourceInfo();
QualType T = TSI ? TSI->getType() : getType();
const auto *FPT = T->getAs<FunctionProtoType>();
return FPT ? FPT->getExceptionSpecType() : EST_None;
}



SourceRange getExceptionSpecSourceRange() const;


QualType getCallResultType() const {
return getType()->castAs<FunctionType>()->getCallResultType(
getASTContext());
}



StorageClass getStorageClass() const {
return static_cast<StorageClass>(FunctionDeclBits.SClass);
}


void setStorageClass(StorageClass SClass) {
FunctionDeclBits.SClass = SClass;
}



bool isInlineSpecified() const { return FunctionDeclBits.IsInlineSpecified; }


void setInlineSpecified(bool I) {
FunctionDeclBits.IsInlineSpecified = I;
FunctionDeclBits.IsInline = I;
}


void setImplicitlyInline(bool I = true) { FunctionDeclBits.IsInline = I; }




bool isInlined() const { return FunctionDeclBits.IsInline; }

bool isInlineDefinitionExternallyVisible() const;

bool isMSExternInline() const;

bool doesDeclarationForceExternallyVisibleDefinition() const;

bool isStatic() const { return getStorageClass() == SC_Static; }



bool isOverloadedOperator() const {
return getOverloadedOperator() != OO_None;
}

OverloadedOperatorKind getOverloadedOperator() const;

const IdentifierInfo *getLiteralIdentifier() const;






















FunctionDecl *getInstantiatedFromMemberFunction() const;


TemplatedKind getTemplatedKind() const;




MemberSpecializationInfo *getMemberSpecializationInfo() const;



void setInstantiationOfMemberFunction(FunctionDecl *FD,
TemplateSpecializationKind TSK) {
setInstantiationOfMemberFunction(getASTContext(), FD, TSK);
}













FunctionTemplateDecl *getDescribedFunctionTemplate() const;

void setDescribedFunctionTemplate(FunctionTemplateDecl *Template);



bool isFunctionTemplateSpecialization() const {
return getPrimaryTemplate() != nullptr;
}




FunctionTemplateSpecializationInfo *getTemplateSpecializationInfo() const;




bool isImplicitlyInstantiable() const;



bool isTemplateInstantiation() const;









FunctionDecl *
getTemplateInstantiationPattern(bool ForDefinition = true) const;






FunctionTemplateDecl *getPrimaryTemplate() const;






const TemplateArgumentList *getTemplateSpecializationArgs() const;








const ASTTemplateArgumentListInfo*
getTemplateSpecializationArgsAsWritten() const;




















void setFunctionTemplateSpecialization(FunctionTemplateDecl *Template,
const TemplateArgumentList *TemplateArgs,
void *InsertPos,
TemplateSpecializationKind TSK = TSK_ImplicitInstantiation,
const TemplateArgumentListInfo *TemplateArgsAsWritten = nullptr,
SourceLocation PointOfInstantiation = SourceLocation()) {
setFunctionTemplateSpecialization(getASTContext(), Template, TemplateArgs,
InsertPos, TSK, TemplateArgsAsWritten,
PointOfInstantiation);
}



void setDependentTemplateSpecialization(ASTContext &Context,
const UnresolvedSetImpl &Templates,
const TemplateArgumentListInfo &TemplateArgs);

DependentFunctionTemplateSpecializationInfo *
getDependentSpecializationInfo() const;



TemplateSpecializationKind getTemplateSpecializationKind() const;



TemplateSpecializationKind
getTemplateSpecializationKindForInstantiation() const;



void setTemplateSpecializationKind(TemplateSpecializationKind TSK,
SourceLocation PointOfInstantiation = SourceLocation());







SourceLocation getPointOfInstantiation() const;



bool isOutOfLine() const override;





unsigned getMemoryFunctionKind() const;



unsigned getODRHash();



unsigned getODRHash() const;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K >= firstFunction && K <= lastFunction;
}
static DeclContext *castToDeclContext(const FunctionDecl *D) {
return static_cast<DeclContext *>(const_cast<FunctionDecl*>(D));
}
static FunctionDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<FunctionDecl *>(const_cast<DeclContext*>(DC));
}
};


class FieldDecl : public DeclaratorDecl, public Mergeable<FieldDecl> {
unsigned BitField : 1;
unsigned Mutable : 1;
mutable unsigned CachedFieldIndex : 30;





enum InitStorageKind {



ISK_NoInit = (unsigned) ICIS_NoInit,



ISK_InClassCopyInit = (unsigned) ICIS_CopyInit,



ISK_InClassListInit = (unsigned) ICIS_ListInit,



ISK_CapturedVLAType,
};



struct InitAndBitWidth {
Expr *Init;
Expr *BitWidth;
};










llvm::PointerIntPair<void *, 2, InitStorageKind> InitStorage;

protected:
FieldDecl(Kind DK, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id,
QualType T, TypeSourceInfo *TInfo, Expr *BW, bool Mutable,
InClassInitStyle InitStyle)
: DeclaratorDecl(DK, DC, IdLoc, Id, T, TInfo, StartLoc),
BitField(false), Mutable(Mutable), CachedFieldIndex(0),
InitStorage(nullptr, (InitStorageKind) InitStyle) {
if (BW)
setBitWidth(BW);
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static FieldDecl *Create(const ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, QualType T,
TypeSourceInfo *TInfo, Expr *BW, bool Mutable,
InClassInitStyle InitStyle);

static FieldDecl *CreateDeserialized(ASTContext &C, unsigned ID);



unsigned getFieldIndex() const;


bool isMutable() const { return Mutable; }


bool isBitField() const { return BitField; }


bool isUnnamedBitfield() const { return isBitField() && !getDeclName(); }





bool isAnonymousStructOrUnion() const;

Expr *getBitWidth() const {
if (!BitField)
return nullptr;
void *Ptr = InitStorage.getPointer();
if (getInClassInitStyle())
return static_cast<InitAndBitWidth*>(Ptr)->BitWidth;
return static_cast<Expr*>(Ptr);
}

unsigned getBitWidthValue(const ASTContext &Ctx) const;



void setBitWidth(Expr *Width) {
assert(!hasCapturedVLAType() && !BitField &&
"bit width or captured type already set");
assert(Width && "no bit width specified");
InitStorage.setPointer(
InitStorage.getInt()
? new (getASTContext())
InitAndBitWidth{getInClassInitializer(), Width}
: static_cast<void*>(Width));
BitField = true;
}



void removeBitWidth() {
assert(isBitField() && "no bitfield width to remove");
InitStorage.setPointer(getInClassInitializer());
BitField = false;
}




bool isZeroLengthBitField(const ASTContext &Ctx) const;




bool isZeroSize(const ASTContext &Ctx) const;


InClassInitStyle getInClassInitStyle() const {
InitStorageKind storageKind = InitStorage.getInt();
return (storageKind == ISK_CapturedVLAType
? ICIS_NoInit : (InClassInitStyle) storageKind);
}


bool hasInClassInitializer() const {
return getInClassInitStyle() != ICIS_NoInit;
}




Expr *getInClassInitializer() const {
if (!hasInClassInitializer())
return nullptr;
void *Ptr = InitStorage.getPointer();
if (BitField)
return static_cast<InitAndBitWidth*>(Ptr)->Init;
return static_cast<Expr*>(Ptr);
}


void setInClassInitializer(Expr *Init) {
assert(hasInClassInitializer() && !getInClassInitializer());
if (BitField)
static_cast<InitAndBitWidth*>(InitStorage.getPointer())->Init = Init;
else
InitStorage.setPointer(Init);
}


void removeInClassInitializer() {
assert(hasInClassInitializer() && "no initializer to remove");
InitStorage.setPointerAndInt(getBitWidth(), ISK_NoInit);
}



bool hasCapturedVLAType() const {
return InitStorage.getInt() == ISK_CapturedVLAType;
}


const VariableArrayType *getCapturedVLAType() const {
return hasCapturedVLAType() ? static_cast<const VariableArrayType *>(
InitStorage.getPointer())
: nullptr;
}


void setCapturedVLAType(const VariableArrayType *VLAType);






const RecordDecl *getParent() const {
return dyn_cast<RecordDecl>(getDeclContext());
}

RecordDecl *getParent() {
return dyn_cast<RecordDecl>(getDeclContext());
}

SourceRange getSourceRange() const override LLVM_READONLY;


FieldDecl *getCanonicalDecl() override { return getFirstDecl(); }
const FieldDecl *getCanonicalDecl() const { return getFirstDecl(); }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K >= firstField && K <= lastField; }
};





class EnumConstantDecl : public ValueDecl, public Mergeable<EnumConstantDecl> {
Stmt *Init;
llvm::APSInt Val;

protected:
EnumConstantDecl(DeclContext *DC, SourceLocation L,
IdentifierInfo *Id, QualType T, Expr *E,
const llvm::APSInt &V)
: ValueDecl(EnumConstant, DC, L, Id, T), Init((Stmt*)E), Val(V) {}

public:
friend class StmtIteratorBase;

static EnumConstantDecl *Create(ASTContext &C, EnumDecl *DC,
SourceLocation L, IdentifierInfo *Id,
QualType T, Expr *E,
const llvm::APSInt &V);
static EnumConstantDecl *CreateDeserialized(ASTContext &C, unsigned ID);

const Expr *getInitExpr() const { return (const Expr*) Init; }
Expr *getInitExpr() { return (Expr*) Init; }
const llvm::APSInt &getInitVal() const { return Val; }

void setInitExpr(Expr *E) { Init = (Stmt*) E; }
void setInitVal(const llvm::APSInt &V) { Val = V; }

SourceRange getSourceRange() const override LLVM_READONLY;


EnumConstantDecl *getCanonicalDecl() override { return getFirstDecl(); }
const EnumConstantDecl *getCanonicalDecl() const { return getFirstDecl(); }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == EnumConstant; }
};



class IndirectFieldDecl : public ValueDecl,
public Mergeable<IndirectFieldDecl> {
NamedDecl **Chaining;
unsigned ChainingSize;

IndirectFieldDecl(ASTContext &C, DeclContext *DC, SourceLocation L,
DeclarationName N, QualType T,
MutableArrayRef<NamedDecl *> CH);

void anchor() override;

public:
friend class ASTDeclReader;

static IndirectFieldDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, IdentifierInfo *Id,
QualType T, llvm::MutableArrayRef<NamedDecl *> CH);

static IndirectFieldDecl *CreateDeserialized(ASTContext &C, unsigned ID);

using chain_iterator = ArrayRef<NamedDecl *>::const_iterator;

ArrayRef<NamedDecl *> chain() const {
return llvm::makeArrayRef(Chaining, ChainingSize);
}
chain_iterator chain_begin() const { return chain().begin(); }
chain_iterator chain_end() const { return chain().end(); }

unsigned getChainingSize() const { return ChainingSize; }

FieldDecl *getAnonField() const {
assert(chain().size() >= 2);
return cast<FieldDecl>(chain().back());
}

VarDecl *getVarDecl() const {
assert(chain().size() >= 2);
return dyn_cast<VarDecl>(chain().front());
}

IndirectFieldDecl *getCanonicalDecl() override { return getFirstDecl(); }
const IndirectFieldDecl *getCanonicalDecl() const { return getFirstDecl(); }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == IndirectField; }
};


class TypeDecl : public NamedDecl {
friend class ASTContext;





mutable const Type *TypeForDecl = nullptr;


SourceLocation LocStart;

void anchor() override;

protected:
TypeDecl(Kind DK, DeclContext *DC, SourceLocation L, IdentifierInfo *Id,
SourceLocation StartL = SourceLocation())
: NamedDecl(DK, DC, L, Id), LocStart(StartL) {}

public:




const Type *getTypeForDecl() const { return TypeForDecl; }
void setTypeForDecl(const Type *TD) { TypeForDecl = TD; }

SourceLocation getBeginLoc() const LLVM_READONLY { return LocStart; }
void setLocStart(SourceLocation L) { LocStart = L; }
SourceRange getSourceRange() const override LLVM_READONLY {
if (LocStart.isValid())
return SourceRange(LocStart, getLocation());
else
return SourceRange(getLocation());
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K >= firstType && K <= lastType; }
};


class TypedefNameDecl : public TypeDecl, public Redeclarable<TypedefNameDecl> {
struct alignas(8) ModedTInfo {
TypeSourceInfo *first;
QualType second;
};



mutable llvm::PointerIntPair<
llvm::PointerUnion<TypeSourceInfo *, ModedTInfo *>, 2>
MaybeModedTInfo;

void anchor() override;

protected:
TypedefNameDecl(Kind DK, ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, TypeSourceInfo *TInfo)
: TypeDecl(DK, DC, IdLoc, Id, StartLoc), redeclarable_base(C),
MaybeModedTInfo(TInfo, 0) {}

using redeclarable_base = Redeclarable<TypedefNameDecl>;

TypedefNameDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

TypedefNameDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

TypedefNameDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

public:
using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;

bool isModed() const {
return MaybeModedTInfo.getPointer().is<ModedTInfo *>();
}

TypeSourceInfo *getTypeSourceInfo() const {
return isModed() ? MaybeModedTInfo.getPointer().get<ModedTInfo *>()->first
: MaybeModedTInfo.getPointer().get<TypeSourceInfo *>();
}

QualType getUnderlyingType() const {
return isModed() ? MaybeModedTInfo.getPointer().get<ModedTInfo *>()->second
: MaybeModedTInfo.getPointer()
.get<TypeSourceInfo *>()
->getType();
}

void setTypeSourceInfo(TypeSourceInfo *newType) {
MaybeModedTInfo.setPointer(newType);
}

void setModedTypeSourceInfo(TypeSourceInfo *unmodedTSI, QualType modedTy) {
MaybeModedTInfo.setPointer(new (getASTContext(), 8)
ModedTInfo({unmodedTSI, modedTy}));
}


TypedefNameDecl *getCanonicalDecl() override { return getFirstDecl(); }
const TypedefNameDecl *getCanonicalDecl() const { return getFirstDecl(); }






TagDecl *getAnonDeclWithTypedefName(bool AnyRedecl = false) const;



bool isTransparentTag() const {
if (MaybeModedTInfo.getInt())
return MaybeModedTInfo.getInt() & 0x2;
return isTransparentTagSlow();
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K >= firstTypedefName && K <= lastTypedefName;
}

private:
bool isTransparentTagSlow() const;
};



class TypedefDecl : public TypedefNameDecl {
TypedefDecl(ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id, TypeSourceInfo *TInfo)
: TypedefNameDecl(Typedef, C, DC, StartLoc, IdLoc, Id, TInfo) {}

public:
static TypedefDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, TypeSourceInfo *TInfo);
static TypedefDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Typedef; }
};



class TypeAliasDecl : public TypedefNameDecl {

TypeAliasTemplateDecl *Template;

TypeAliasDecl(ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id, TypeSourceInfo *TInfo)
: TypedefNameDecl(TypeAlias, C, DC, StartLoc, IdLoc, Id, TInfo),
Template(nullptr) {}

public:
static TypeAliasDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, TypeSourceInfo *TInfo);
static TypeAliasDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;

TypeAliasTemplateDecl *getDescribedAliasTemplate() const { return Template; }
void setDescribedAliasTemplate(TypeAliasTemplateDecl *TAT) { Template = TAT; }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == TypeAlias; }
};


class TagDecl : public TypeDecl,
public DeclContext,
public Redeclarable<TagDecl> {


public:

using TagKind = TagTypeKind;

private:
SourceRange BraceRange;



using ExtInfo = QualifierInfo;









llvm::PointerUnion<TypedefNameDecl *, ExtInfo *> TypedefNameDeclOrQualifier;

bool hasExtInfo() const { return TypedefNameDeclOrQualifier.is<ExtInfo *>(); }
ExtInfo *getExtInfo() { return TypedefNameDeclOrQualifier.get<ExtInfo *>(); }
const ExtInfo *getExtInfo() const {
return TypedefNameDeclOrQualifier.get<ExtInfo *>();
}

protected:
TagDecl(Kind DK, TagKind TK, const ASTContext &C, DeclContext *DC,
SourceLocation L, IdentifierInfo *Id, TagDecl *PrevDecl,
SourceLocation StartL);

using redeclarable_base = Redeclarable<TagDecl>;

TagDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

TagDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

TagDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}




void completeDefinition();


void setBeingDefined(bool V = true) { TagDeclBits.IsBeingDefined = V; }





void setMayHaveOutOfDateDef(bool V = true) {
TagDeclBits.MayHaveOutOfDateDef = V;
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;

SourceRange getBraceRange() const { return BraceRange; }
void setBraceRange(SourceRange R) { BraceRange = R; }



SourceLocation getInnerLocStart() const { return getBeginLoc(); }



SourceLocation getOuterLocStart() const;
SourceRange getSourceRange() const override LLVM_READONLY;

TagDecl *getCanonicalDecl() override;
const TagDecl *getCanonicalDecl() const {
return const_cast<TagDecl*>(this)->getCanonicalDecl();
}



bool isThisDeclarationADefinition() const {
return isCompleteDefinition();
}


bool isCompleteDefinition() const { return TagDeclBits.IsCompleteDefinition; }


void setCompleteDefinition(bool V = true) {
TagDeclBits.IsCompleteDefinition = V;
}



bool isCompleteDefinitionRequired() const {
return TagDeclBits.IsCompleteDefinitionRequired;
}



void setCompleteDefinitionRequired(bool V = true) {
TagDeclBits.IsCompleteDefinitionRequired = V;
}


bool isBeingDefined() const { return TagDeclBits.IsBeingDefined; }



bool isEmbeddedInDeclarator() const {
return TagDeclBits.IsEmbeddedInDeclarator;
}



void setEmbeddedInDeclarator(bool isInDeclarator) {
TagDeclBits.IsEmbeddedInDeclarator = isInDeclarator;
}


bool isFreeStanding() const { return TagDeclBits.IsFreeStanding; }


void setFreeStanding(bool isFreeStanding = true) {
TagDeclBits.IsFreeStanding = isFreeStanding;
}





bool mayHaveOutOfDateDef() const { return TagDeclBits.MayHaveOutOfDateDef; }




bool isDependentType() const { return isDependentContext(); }






void startDefinition();









TagDecl *getDefinition() const;

StringRef getKindName() const {
return TypeWithKeyword::getTagTypeKindName(getTagKind());
}

TagKind getTagKind() const {
return static_cast<TagKind>(TagDeclBits.TagDeclKind);
}

void setTagKind(TagKind TK) { TagDeclBits.TagDeclKind = TK; }

bool isStruct() const { return getTagKind() == TTK_Struct; }
bool isInterface() const { return getTagKind() == TTK_Interface; }
bool isClass() const { return getTagKind() == TTK_Class; }
bool isUnion() const { return getTagKind() == TTK_Union; }
bool isEnum() const { return getTagKind() == TTK_Enum; }
















bool hasNameForLinkage() const {
return (getDeclName() || getTypedefNameForAnonDecl());
}

TypedefNameDecl *getTypedefNameForAnonDecl() const {
return hasExtInfo() ? nullptr
: TypedefNameDeclOrQualifier.get<TypedefNameDecl *>();
}

void setTypedefNameForAnonDecl(TypedefNameDecl *TDD);



NestedNameSpecifier *getQualifier() const {
return hasExtInfo() ? getExtInfo()->QualifierLoc.getNestedNameSpecifier()
: nullptr;
}




NestedNameSpecifierLoc getQualifierLoc() const {
return hasExtInfo() ? getExtInfo()->QualifierLoc
: NestedNameSpecifierLoc();
}

void setQualifierInfo(NestedNameSpecifierLoc QualifierLoc);

unsigned getNumTemplateParameterLists() const {
return hasExtInfo() ? getExtInfo()->NumTemplParamLists : 0;
}

TemplateParameterList *getTemplateParameterList(unsigned i) const {
assert(i < getNumTemplateParameterLists());
return getExtInfo()->TemplParamLists[i];
}

void setTemplateParameterListsInfo(ASTContext &Context,
ArrayRef<TemplateParameterList *> TPLists);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K >= firstTag && K <= lastTag; }

static DeclContext *castToDeclContext(const TagDecl *D) {
return static_cast<DeclContext *>(const_cast<TagDecl*>(D));
}

static TagDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<TagDecl *>(const_cast<DeclContext*>(DC));
}
};




class EnumDecl : public TagDecl {

















llvm::PointerUnion<const Type *, TypeSourceInfo *> IntegerType;





QualType PromotionType;




MemberSpecializationInfo *SpecializationInfo = nullptr;




unsigned ODRHash;

EnumDecl(ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id, EnumDecl *PrevDecl,
bool Scoped, bool ScopedUsingClassTag, bool Fixed);

void anchor() override;

void setInstantiationOfMemberEnum(ASTContext &C, EnumDecl *ED,
TemplateSpecializationKind TSK);



void setNumPositiveBits(unsigned Num) {
EnumDeclBits.NumPositiveBits = Num;
assert(EnumDeclBits.NumPositiveBits == Num && "can't store this bitcount");
}



void setNumNegativeBits(unsigned Num) { EnumDeclBits.NumNegativeBits = Num; }

public:


void setScoped(bool Scoped = true) { EnumDeclBits.IsScoped = Scoped; }





void setScopedUsingClassTag(bool ScopedUCT = true) {
EnumDeclBits.IsScopedUsingClassTag = ScopedUCT;
}



void setFixed(bool Fixed = true) { EnumDeclBits.IsFixed = Fixed; }

private:

bool hasODRHash() const { return EnumDeclBits.HasODRHash; }
void setHasODRHash(bool Hash = true) { EnumDeclBits.HasODRHash = Hash; }

public:
friend class ASTDeclReader;

EnumDecl *getCanonicalDecl() override {
return cast<EnumDecl>(TagDecl::getCanonicalDecl());
}
const EnumDecl *getCanonicalDecl() const {
return const_cast<EnumDecl*>(this)->getCanonicalDecl();
}

EnumDecl *getPreviousDecl() {
return cast_or_null<EnumDecl>(
static_cast<TagDecl *>(this)->getPreviousDecl());
}
const EnumDecl *getPreviousDecl() const {
return const_cast<EnumDecl*>(this)->getPreviousDecl();
}

EnumDecl *getMostRecentDecl() {
return cast<EnumDecl>(static_cast<TagDecl *>(this)->getMostRecentDecl());
}
const EnumDecl *getMostRecentDecl() const {
return const_cast<EnumDecl*>(this)->getMostRecentDecl();
}

EnumDecl *getDefinition() const {
return cast_or_null<EnumDecl>(TagDecl::getDefinition());
}

static EnumDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, EnumDecl *PrevDecl,
bool IsScoped, bool IsScopedUsingClassTag,
bool IsFixed);
static EnumDecl *CreateDeserialized(ASTContext &C, unsigned ID);






void completeDefinition(QualType NewType,
QualType PromotionType,
unsigned NumPositiveBits,
unsigned NumNegativeBits);


using enumerator_iterator = specific_decl_iterator<EnumConstantDecl>;
using enumerator_range =
llvm::iterator_range<specific_decl_iterator<EnumConstantDecl>>;

enumerator_range enumerators() const {
return enumerator_range(enumerator_begin(), enumerator_end());
}

enumerator_iterator enumerator_begin() const {
const EnumDecl *E = getDefinition();
if (!E)
E = this;
return enumerator_iterator(E->decls_begin());
}

enumerator_iterator enumerator_end() const {
const EnumDecl *E = getDefinition();
if (!E)
E = this;
return enumerator_iterator(E->decls_end());
}


QualType getPromotionType() const { return PromotionType; }


void setPromotionType(QualType T) { PromotionType = T; }




QualType getIntegerType() const {
if (!IntegerType)
return QualType();
if (const Type *T = IntegerType.dyn_cast<const Type*>())
return QualType(T, 0);
return IntegerType.get<TypeSourceInfo*>()->getType().getUnqualifiedType();
}


void setIntegerType(QualType T) { IntegerType = T.getTypePtrOrNull(); }


void setIntegerTypeSourceInfo(TypeSourceInfo *TInfo) { IntegerType = TInfo; }



TypeSourceInfo *getIntegerTypeSourceInfo() const {
return IntegerType.dyn_cast<TypeSourceInfo*>();
}



SourceRange getIntegerTypeRange() const LLVM_READONLY;



unsigned getNumPositiveBits() const { return EnumDeclBits.NumPositiveBits; }










unsigned getNumNegativeBits() const { return EnumDeclBits.NumNegativeBits; }


bool isScoped() const { return EnumDeclBits.IsScoped; }


bool isScopedUsingClassTag() const {
return EnumDeclBits.IsScopedUsingClassTag;
}



bool isFixed() const { return EnumDeclBits.IsFixed; }

unsigned getODRHash();


bool isComplete() const {


return isCompleteDefinition() || IntegerType;
}



bool isClosed() const;



bool isClosedFlag() const;



bool isClosedNonFlag() const;



EnumDecl *getTemplateInstantiationPattern() const;




EnumDecl *getInstantiatedFromMemberEnum() const;




TemplateSpecializationKind getTemplateSpecializationKind() const;



void setTemplateSpecializationKind(TemplateSpecializationKind TSK,
SourceLocation PointOfInstantiation = SourceLocation());




MemberSpecializationInfo *getMemberSpecializationInfo() const {
return SpecializationInfo;
}



void setInstantiationOfMemberEnum(EnumDecl *ED,
TemplateSpecializationKind TSK) {
setInstantiationOfMemberEnum(getASTContext(), ED, TSK);
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Enum; }
};





class RecordDecl : public TagDecl {


public:
friend class DeclContext;




enum ArgPassingKind : unsigned {

APK_CanPassInRegs,








APK_CannotPassInRegs,




APK_CanNeverPassInRegs
};

protected:
RecordDecl(Kind DK, TagKind TK, const ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, RecordDecl *PrevDecl);

public:
static RecordDecl *Create(const ASTContext &C, TagKind TK, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, RecordDecl* PrevDecl = nullptr);
static RecordDecl *CreateDeserialized(const ASTContext &C, unsigned ID);

RecordDecl *getPreviousDecl() {
return cast_or_null<RecordDecl>(
static_cast<TagDecl *>(this)->getPreviousDecl());
}
const RecordDecl *getPreviousDecl() const {
return const_cast<RecordDecl*>(this)->getPreviousDecl();
}

RecordDecl *getMostRecentDecl() {
return cast<RecordDecl>(static_cast<TagDecl *>(this)->getMostRecentDecl());
}
const RecordDecl *getMostRecentDecl() const {
return const_cast<RecordDecl*>(this)->getMostRecentDecl();
}

bool hasFlexibleArrayMember() const {
return RecordDeclBits.HasFlexibleArrayMember;
}

void setHasFlexibleArrayMember(bool V) {
RecordDeclBits.HasFlexibleArrayMember = V;
}












bool isAnonymousStructOrUnion() const {
return RecordDeclBits.AnonymousStructOrUnion;
}

void setAnonymousStructOrUnion(bool Anon) {
RecordDeclBits.AnonymousStructOrUnion = Anon;
}

bool hasObjectMember() const { return RecordDeclBits.HasObjectMember; }
void setHasObjectMember(bool val) { RecordDeclBits.HasObjectMember = val; }

bool hasVolatileMember() const { return RecordDeclBits.HasVolatileMember; }

void setHasVolatileMember(bool val) {
RecordDeclBits.HasVolatileMember = val;
}

bool hasLoadedFieldsFromExternalStorage() const {
return RecordDeclBits.LoadedFieldsFromExternalStorage;
}

void setHasLoadedFieldsFromExternalStorage(bool val) const {
RecordDeclBits.LoadedFieldsFromExternalStorage = val;
}


bool isNonTrivialToPrimitiveDefaultInitialize() const {
return RecordDeclBits.NonTrivialToPrimitiveDefaultInitialize;
}

void setNonTrivialToPrimitiveDefaultInitialize(bool V) {
RecordDeclBits.NonTrivialToPrimitiveDefaultInitialize = V;
}

bool isNonTrivialToPrimitiveCopy() const {
return RecordDeclBits.NonTrivialToPrimitiveCopy;
}

void setNonTrivialToPrimitiveCopy(bool V) {
RecordDeclBits.NonTrivialToPrimitiveCopy = V;
}

bool isNonTrivialToPrimitiveDestroy() const {
return RecordDeclBits.NonTrivialToPrimitiveDestroy;
}

void setNonTrivialToPrimitiveDestroy(bool V) {
RecordDeclBits.NonTrivialToPrimitiveDestroy = V;
}

bool hasNonTrivialToPrimitiveDefaultInitializeCUnion() const {
return RecordDeclBits.HasNonTrivialToPrimitiveDefaultInitializeCUnion;
}

void setHasNonTrivialToPrimitiveDefaultInitializeCUnion(bool V) {
RecordDeclBits.HasNonTrivialToPrimitiveDefaultInitializeCUnion = V;
}

bool hasNonTrivialToPrimitiveDestructCUnion() const {
return RecordDeclBits.HasNonTrivialToPrimitiveDestructCUnion;
}

void setHasNonTrivialToPrimitiveDestructCUnion(bool V) {
RecordDeclBits.HasNonTrivialToPrimitiveDestructCUnion = V;
}

bool hasNonTrivialToPrimitiveCopyCUnion() const {
return RecordDeclBits.HasNonTrivialToPrimitiveCopyCUnion;
}

void setHasNonTrivialToPrimitiveCopyCUnion(bool V) {
RecordDeclBits.HasNonTrivialToPrimitiveCopyCUnion = V;
}




bool canPassInRegisters() const {
return getArgPassingRestrictions() == APK_CanPassInRegs;
}

ArgPassingKind getArgPassingRestrictions() const {
return static_cast<ArgPassingKind>(RecordDeclBits.ArgPassingRestrictions);
}

void setArgPassingRestrictions(ArgPassingKind Kind) {
RecordDeclBits.ArgPassingRestrictions = Kind;
}

bool isParamDestroyedInCallee() const {
return RecordDeclBits.ParamDestroyedInCallee;
}

void setParamDestroyedInCallee(bool V) {
RecordDeclBits.ParamDestroyedInCallee = V;
}














bool isInjectedClassName() const;



bool isLambda() const;



bool isCapturedRecord() const;



void setCapturedRecord();









RecordDecl *getDefinition() const {
return cast_or_null<RecordDecl>(TagDecl::getDefinition());
}




bool isOrContainsUnion() const;




using field_iterator = specific_decl_iterator<FieldDecl>;
using field_range = llvm::iterator_range<specific_decl_iterator<FieldDecl>>;

field_range fields() const { return field_range(field_begin(), field_end()); }
field_iterator field_begin() const;

field_iterator field_end() const {
return field_iterator(decl_iterator());
}


bool field_empty() const {
return field_begin() == field_end();
}


virtual void completeDefinition();

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K >= firstRecord && K <= lastRecord;
}




bool isMsStruct(const ASTContext &C) const;




bool mayInsertExtraPadding(bool EmitRemark = false) const;



const FieldDecl *findFirstNamedDataMember() const;

private:

void LoadFieldsFromExternalStorage() const;
};

class FileScopeAsmDecl : public Decl {
StringLiteral *AsmString;
SourceLocation RParenLoc;

FileScopeAsmDecl(DeclContext *DC, StringLiteral *asmstring,
SourceLocation StartL, SourceLocation EndL)
: Decl(FileScopeAsm, DC, StartL), AsmString(asmstring), RParenLoc(EndL) {}

virtual void anchor();

public:
static FileScopeAsmDecl *Create(ASTContext &C, DeclContext *DC,
StringLiteral *Str, SourceLocation AsmLoc,
SourceLocation RParenLoc);

static FileScopeAsmDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceLocation getAsmLoc() const { return getLocation(); }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }
SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getAsmLoc(), getRParenLoc());
}

const StringLiteral *getAsmString() const { return AsmString; }
StringLiteral *getAsmString() { return AsmString; }
void setAsmString(StringLiteral *Asm) { AsmString = Asm; }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == FileScopeAsm; }
};




class BlockDecl : public Decl, public DeclContext {


public:


class Capture {
enum {
flag_isByRef = 0x1,
flag_isNested = 0x2
};


llvm::PointerIntPair<VarDecl*, 2> VariableAndFlags;




Expr *CopyExpr;

public:
Capture(VarDecl *variable, bool byRef, bool nested, Expr *copy)
: VariableAndFlags(variable,
(byRef ? flag_isByRef : 0) | (nested ? flag_isNested : 0)),
CopyExpr(copy) {}


VarDecl *getVariable() const { return VariableAndFlags.getPointer(); }



bool isByRef() const { return VariableAndFlags.getInt() & flag_isByRef; }

bool isEscapingByref() const {
return getVariable()->isEscapingByref();
}

bool isNonEscapingByref() const {
return getVariable()->isNonEscapingByref();
}



bool isNested() const { return VariableAndFlags.getInt() & flag_isNested; }

bool hasCopyExpr() const { return CopyExpr != nullptr; }
Expr *getCopyExpr() const { return CopyExpr; }
void setCopyExpr(Expr *e) { CopyExpr = e; }
};

private:



ParmVarDecl **ParamInfo = nullptr;
unsigned NumParams = 0;

Stmt *Body = nullptr;
TypeSourceInfo *SignatureAsWritten = nullptr;

const Capture *Captures = nullptr;
unsigned NumCaptures = 0;

unsigned ManglingNumber = 0;
Decl *ManglingContextDecl = nullptr;

protected:
BlockDecl(DeclContext *DC, SourceLocation CaretLoc);

public:
static BlockDecl *Create(ASTContext &C, DeclContext *DC, SourceLocation L);
static BlockDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceLocation getCaretLocation() const { return getLocation(); }

bool isVariadic() const { return BlockDeclBits.IsVariadic; }
void setIsVariadic(bool value) { BlockDeclBits.IsVariadic = value; }

CompoundStmt *getCompoundBody() const { return (CompoundStmt*) Body; }
Stmt *getBody() const override { return (Stmt*) Body; }
void setBody(CompoundStmt *B) { Body = (Stmt*) B; }

void setSignatureAsWritten(TypeSourceInfo *Sig) { SignatureAsWritten = Sig; }
TypeSourceInfo *getSignatureAsWritten() const { return SignatureAsWritten; }


ArrayRef<ParmVarDecl *> parameters() const {
return {ParamInfo, getNumParams()};
}
MutableArrayRef<ParmVarDecl *> parameters() {
return {ParamInfo, getNumParams()};
}


using param_iterator = MutableArrayRef<ParmVarDecl *>::iterator;
using param_const_iterator = ArrayRef<ParmVarDecl *>::const_iterator;

bool param_empty() const { return parameters().empty(); }
param_iterator param_begin() { return parameters().begin(); }
param_iterator param_end() { return parameters().end(); }
param_const_iterator param_begin() const { return parameters().begin(); }
param_const_iterator param_end() const { return parameters().end(); }
size_t param_size() const { return parameters().size(); }

unsigned getNumParams() const { return NumParams; }

const ParmVarDecl *getParamDecl(unsigned i) const {
assert(i < getNumParams() && "Illegal param #");
return ParamInfo[i];
}
ParmVarDecl *getParamDecl(unsigned i) {
assert(i < getNumParams() && "Illegal param #");
return ParamInfo[i];
}

void setParams(ArrayRef<ParmVarDecl *> NewParamInfo);



bool hasCaptures() const { return NumCaptures || capturesCXXThis(); }



unsigned getNumCaptures() const { return NumCaptures; }

using capture_const_iterator = ArrayRef<Capture>::const_iterator;

ArrayRef<Capture> captures() const { return {Captures, NumCaptures}; }

capture_const_iterator capture_begin() const { return captures().begin(); }
capture_const_iterator capture_end() const { return captures().end(); }

bool capturesCXXThis() const { return BlockDeclBits.CapturesCXXThis; }
void setCapturesCXXThis(bool B = true) { BlockDeclBits.CapturesCXXThis = B; }

bool blockMissingReturnType() const {
return BlockDeclBits.BlockMissingReturnType;
}

void setBlockMissingReturnType(bool val = true) {
BlockDeclBits.BlockMissingReturnType = val;
}

bool isConversionFromLambda() const {
return BlockDeclBits.IsConversionFromLambda;
}

void setIsConversionFromLambda(bool val = true) {
BlockDeclBits.IsConversionFromLambda = val;
}

bool doesNotEscape() const { return BlockDeclBits.DoesNotEscape; }
void setDoesNotEscape(bool B = true) { BlockDeclBits.DoesNotEscape = B; }

bool canAvoidCopyToHeap() const {
return BlockDeclBits.CanAvoidCopyToHeap;
}
void setCanAvoidCopyToHeap(bool B = true) {
BlockDeclBits.CanAvoidCopyToHeap = B;
}

bool capturesVariable(const VarDecl *var) const;

void setCaptures(ASTContext &Context, ArrayRef<Capture> Captures,
bool CapturesCXXThis);

unsigned getBlockManglingNumber() const { return ManglingNumber; }

Decl *getBlockManglingContextDecl() const { return ManglingContextDecl; }

void setBlockMangling(unsigned Number, Decl *Ctx) {
ManglingNumber = Number;
ManglingContextDecl = Ctx;
}

SourceRange getSourceRange() const override LLVM_READONLY;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Block; }
static DeclContext *castToDeclContext(const BlockDecl *D) {
return static_cast<DeclContext *>(const_cast<BlockDecl*>(D));
}
static BlockDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<BlockDecl *>(const_cast<DeclContext*>(DC));
}
};


class CapturedDecl final
: public Decl,
public DeclContext,
private llvm::TrailingObjects<CapturedDecl, ImplicitParamDecl *> {
protected:
size_t numTrailingObjects(OverloadToken<ImplicitParamDecl>) {
return NumParams;
}

private:

unsigned NumParams;


unsigned ContextParam;


llvm::PointerIntPair<Stmt *, 1, bool> BodyAndNothrow;

explicit CapturedDecl(DeclContext *DC, unsigned NumParams);

ImplicitParamDecl *const *getParams() const {
return getTrailingObjects<ImplicitParamDecl *>();
}

ImplicitParamDecl **getParams() {
return getTrailingObjects<ImplicitParamDecl *>();
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend TrailingObjects;

static CapturedDecl *Create(ASTContext &C, DeclContext *DC,
unsigned NumParams);
static CapturedDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned NumParams);

Stmt *getBody() const override;
void setBody(Stmt *B);

bool isNothrow() const;
void setNothrow(bool Nothrow = true);

unsigned getNumParams() const { return NumParams; }

ImplicitParamDecl *getParam(unsigned i) const {
assert(i < NumParams);
return getParams()[i];
}
void setParam(unsigned i, ImplicitParamDecl *P) {
assert(i < NumParams);
getParams()[i] = P;
}


ArrayRef<ImplicitParamDecl *> parameters() const {
return {getParams(), getNumParams()};
}
MutableArrayRef<ImplicitParamDecl *> parameters() {
return {getParams(), getNumParams()};
}


ImplicitParamDecl *getContextParam() const {
assert(ContextParam < NumParams);
return getParam(ContextParam);
}
void setContextParam(unsigned i, ImplicitParamDecl *P) {
assert(i < NumParams);
ContextParam = i;
setParam(i, P);
}
unsigned getContextParamPosition() const { return ContextParam; }

using param_iterator = ImplicitParamDecl *const *;
using param_range = llvm::iterator_range<param_iterator>;


param_iterator param_begin() const { return getParams(); }

param_iterator param_end() const { return getParams() + NumParams; }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Captured; }
static DeclContext *castToDeclContext(const CapturedDecl *D) {
return static_cast<DeclContext *>(const_cast<CapturedDecl *>(D));
}
static CapturedDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<CapturedDecl *>(const_cast<DeclContext *>(DC));
}
};











class ImportDecl final : public Decl,
llvm::TrailingObjects<ImportDecl, SourceLocation> {
friend class ASTContext;
friend class ASTDeclReader;
friend class ASTReader;
friend TrailingObjects;


Module *ImportedModule = nullptr;









llvm::PointerIntPair<ImportDecl *, 1, bool> NextLocalImportAndComplete;

ImportDecl(DeclContext *DC, SourceLocation StartLoc, Module *Imported,
ArrayRef<SourceLocation> IdentifierLocs);

ImportDecl(DeclContext *DC, SourceLocation StartLoc, Module *Imported,
SourceLocation EndLoc);

ImportDecl(EmptyShell Empty) : Decl(Import, Empty) {}

bool isImportComplete() const { return NextLocalImportAndComplete.getInt(); }

void setImportComplete(bool C) { NextLocalImportAndComplete.setInt(C); }



ImportDecl *getNextLocalImport() const {
return NextLocalImportAndComplete.getPointer();
}

void setNextLocalImport(ImportDecl *Import) {
NextLocalImportAndComplete.setPointer(Import);
}

public:

static ImportDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, Module *Imported,
ArrayRef<SourceLocation> IdentifierLocs);



static ImportDecl *CreateImplicit(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, Module *Imported,
SourceLocation EndLoc);


static ImportDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned NumLocations);


Module *getImportedModule() const { return ImportedModule; }






ArrayRef<SourceLocation> getIdentifierLocs() const;

SourceRange getSourceRange() const override LLVM_READONLY;

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Import; }
};







class ExportDecl final : public Decl, public DeclContext {
virtual void anchor();

private:
friend class ASTDeclReader;


SourceLocation RBraceLoc;

ExportDecl(DeclContext *DC, SourceLocation ExportLoc)
: Decl(Export, DC, ExportLoc), DeclContext(Export),
RBraceLoc(SourceLocation()) {}

public:
static ExportDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation ExportLoc);
static ExportDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceLocation getExportLoc() const { return getLocation(); }
SourceLocation getRBraceLoc() const { return RBraceLoc; }
void setRBraceLoc(SourceLocation L) { RBraceLoc = L; }

bool hasBraces() const { return RBraceLoc.isValid(); }

SourceLocation getEndLoc() const LLVM_READONLY {
if (hasBraces())
return RBraceLoc;


return decls_empty() ? getLocation() : decls_begin()->getEndLoc();
}

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getLocation(), getEndLoc());
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Export; }
static DeclContext *castToDeclContext(const ExportDecl *D) {
return static_cast<DeclContext *>(const_cast<ExportDecl*>(D));
}
static ExportDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<ExportDecl *>(const_cast<DeclContext*>(DC));
}
};


class EmptyDecl : public Decl {
EmptyDecl(DeclContext *DC, SourceLocation L) : Decl(Empty, DC, L) {}

virtual void anchor();

public:
static EmptyDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L);
static EmptyDecl *CreateDeserialized(ASTContext &C, unsigned ID);

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Empty; }
};



inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &PD,
const NamedDecl *ND) {
PD.AddTaggedVal(reinterpret_cast<intptr_t>(ND),
DiagnosticsEngine::ak_nameddecl);
return PD;
}

template<typename decl_type>
void Redeclarable<decl_type>::setPreviousDecl(decl_type *PrevDecl) {


assert(RedeclLink.isFirst() &&
"setPreviousDecl on a decl already in a redeclaration chain");

if (PrevDecl) {



First = PrevDecl->getFirstDecl();
assert(First->RedeclLink.isFirst() && "Expected first");
decl_type *MostRecent = First->getNextRedeclaration();
RedeclLink = PreviousDeclLink(cast<decl_type>(MostRecent));



static_cast<decl_type*>(this)->IdentifierNamespace |=
MostRecent->getIdentifierNamespace() &
(Decl::IDNS_Ordinary | Decl::IDNS_Tag | Decl::IDNS_Type);
} else {

First = static_cast<decl_type*>(this);
}


First->RedeclLink.setLatest(static_cast<decl_type*>(this));

assert(!isa<NamedDecl>(static_cast<decl_type*>(this)) ||
cast<NamedDecl>(static_cast<decl_type*>(this))->isLinkageValid());
}







inline bool IsEnumDeclComplete(EnumDecl *ED) {
return ED->isComplete();
}





inline bool IsEnumDeclScoped(EnumDecl *ED) {
return ED->isScoped();
}




static constexpr StringRef getOpenMPVariantManglingSeparatorStr() {
return "$ompvariant";
}

}

#endif
