












#if !defined(LLVM_CLANG_AST_DECLOPENMP_H)
#define LLVM_CLANG_AST_DECLOPENMP_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/OpenMPClause.h"
#include "clang/AST/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/TrailingObjects.h"

namespace clang {



template <typename U> class OMPDeclarativeDirective : public U {
friend class ASTDeclReader;
friend class ASTDeclWriter;


MutableArrayRef<OMPClause *> getClauses() {
if (!Data)
return llvm::None;
return Data->getClauses();
}

protected:

OMPChildren *Data = nullptr;


template <typename... Params>
OMPDeclarativeDirective(Params &&... P) : U(std::forward<Params>(P)...) {}

template <typename T, typename... Params>
static T *createDirective(const ASTContext &C, DeclContext *DC,
ArrayRef<OMPClause *> Clauses, unsigned NumChildren,
Params &&... P) {
auto *Inst = new (C, DC, size(Clauses.size(), NumChildren))
T(DC, std::forward<Params>(P)...);
Inst->Data = OMPChildren::Create(Inst + 1, Clauses,
nullptr, NumChildren);
Inst->Data->setClauses(Clauses);
return Inst;
}

template <typename T, typename... Params>
static T *createEmptyDirective(const ASTContext &C, unsigned ID,
unsigned NumClauses, unsigned NumChildren,
Params &&... P) {
auto *Inst = new (C, ID, size(NumClauses, NumChildren))
T(nullptr, std::forward<Params>(P)...);
Inst->Data = OMPChildren::CreateEmpty(
Inst + 1, NumClauses, false, NumChildren);
return Inst;
}

static size_t size(unsigned NumClauses, unsigned NumChildren) {
return OMPChildren::size(NumClauses, false,
NumChildren);
}

public:

unsigned getNumClauses() const {
if (!Data)
return 0;
return Data->getNumClauses();
}





OMPClause *getClause(unsigned I) const { return clauses()[I]; }

ArrayRef<OMPClause *> clauses() const {
if (!Data)
return llvm::None;
return Data->getClauses();
}
};













class OMPThreadPrivateDecl final : public OMPDeclarativeDirective<Decl> {
friend class OMPDeclarativeDirective<Decl>;

virtual void anchor();

OMPThreadPrivateDecl(DeclContext *DC = nullptr,
SourceLocation L = SourceLocation())
: OMPDeclarativeDirective<Decl>(OMPThreadPrivate, DC, L) {}

ArrayRef<const Expr *> getVars() const {
auto **Storage = reinterpret_cast<Expr **>(Data->getChildren().data());
return llvm::makeArrayRef(Storage, Data->getNumChildren());
}

MutableArrayRef<Expr *> getVars() {
auto **Storage = reinterpret_cast<Expr **>(Data->getChildren().data());
return llvm::makeMutableArrayRef(Storage, Data->getNumChildren());
}

void setVars(ArrayRef<Expr *> VL);

public:
static OMPThreadPrivateDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L,
ArrayRef<Expr *> VL);
static OMPThreadPrivateDecl *CreateDeserialized(ASTContext &C,
unsigned ID, unsigned N);

typedef MutableArrayRef<Expr *>::iterator varlist_iterator;
typedef ArrayRef<const Expr *>::iterator varlist_const_iterator;
typedef llvm::iterator_range<varlist_iterator> varlist_range;
typedef llvm::iterator_range<varlist_const_iterator> varlist_const_range;

unsigned varlist_size() const { return Data->getNumChildren(); }
bool varlist_empty() const { return Data->getChildren().empty(); }

varlist_range varlists() {
return varlist_range(varlist_begin(), varlist_end());
}
varlist_const_range varlists() const {
return varlist_const_range(varlist_begin(), varlist_end());
}
varlist_iterator varlist_begin() { return getVars().begin(); }
varlist_iterator varlist_end() { return getVars().end(); }
varlist_const_iterator varlist_begin() const { return getVars().begin(); }
varlist_const_iterator varlist_end() const { return getVars().end(); }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == OMPThreadPrivate; }
};











class OMPDeclareReductionDecl final : public ValueDecl, public DeclContext {


public:
enum InitKind {
CallInit,
DirectInit,
CopyInit
};

private:
friend class ASTDeclReader;

Expr *Combiner = nullptr;

Expr *Initializer = nullptr;

Expr *In = nullptr;

Expr *Out = nullptr;

Expr *Priv = nullptr;

Expr *Orig = nullptr;




LazyDeclPtr PrevDeclInScope;

void anchor() override;

OMPDeclareReductionDecl(Kind DK, DeclContext *DC, SourceLocation L,
DeclarationName Name, QualType Ty,
OMPDeclareReductionDecl *PrevDeclInScope);

void setPrevDeclInScope(OMPDeclareReductionDecl *Prev) {
PrevDeclInScope = Prev;
}

public:

static OMPDeclareReductionDecl *
Create(ASTContext &C, DeclContext *DC, SourceLocation L, DeclarationName Name,
QualType T, OMPDeclareReductionDecl *PrevDeclInScope);

static OMPDeclareReductionDecl *CreateDeserialized(ASTContext &C,
unsigned ID);


Expr *getCombiner() { return Combiner; }
const Expr *getCombiner() const { return Combiner; }

Expr *getCombinerIn() { return In; }
const Expr *getCombinerIn() const { return In; }

Expr *getCombinerOut() { return Out; }
const Expr *getCombinerOut() const { return Out; }

void setCombiner(Expr *E) { Combiner = E; }

void setCombinerData(Expr *InE, Expr *OutE) {
In = InE;
Out = OutE;
}



Expr *getInitializer() { return Initializer; }
const Expr *getInitializer() const { return Initializer; }

InitKind getInitializerKind() const {
return static_cast<InitKind>(OMPDeclareReductionDeclBits.InitializerKind);
}

Expr *getInitOrig() { return Orig; }
const Expr *getInitOrig() const { return Orig; }

Expr *getInitPriv() { return Priv; }
const Expr *getInitPriv() const { return Priv; }

void setInitializer(Expr *E, InitKind IK) {
Initializer = E;
OMPDeclareReductionDeclBits.InitializerKind = IK;
}

void setInitializerData(Expr *OrigE, Expr *PrivE) {
Orig = OrigE;
Priv = PrivE;
}



OMPDeclareReductionDecl *getPrevDeclInScope();
const OMPDeclareReductionDecl *getPrevDeclInScope() const;

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == OMPDeclareReduction; }
static DeclContext *castToDeclContext(const OMPDeclareReductionDecl *D) {
return static_cast<DeclContext *>(const_cast<OMPDeclareReductionDecl *>(D));
}
static OMPDeclareReductionDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<OMPDeclareReductionDecl *>(
const_cast<DeclContext *>(DC));
}
};









class OMPDeclareMapperDecl final : public OMPDeclarativeDirective<ValueDecl>,
public DeclContext {
friend class OMPDeclarativeDirective<ValueDecl>;
friend class ASTDeclReader;
friend class ASTDeclWriter;


Expr *MapperVarRef = nullptr;


DeclarationName VarName;

LazyDeclPtr PrevDeclInScope;

void anchor() override;

OMPDeclareMapperDecl(DeclContext *DC, SourceLocation L, DeclarationName Name,
QualType Ty, DeclarationName VarName,
OMPDeclareMapperDecl *PrevDeclInScope)
: OMPDeclarativeDirective<ValueDecl>(OMPDeclareMapper, DC, L, Name, Ty),
DeclContext(OMPDeclareMapper), VarName(VarName),
PrevDeclInScope(PrevDeclInScope) {}

void setPrevDeclInScope(OMPDeclareMapperDecl *Prev) {
PrevDeclInScope = Prev;
}

public:

static OMPDeclareMapperDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, DeclarationName Name,
QualType T, DeclarationName VarName,
ArrayRef<OMPClause *> Clauses,
OMPDeclareMapperDecl *PrevDeclInScope);

static OMPDeclareMapperDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned N);

using clauselist_iterator = MutableArrayRef<OMPClause *>::iterator;
using clauselist_const_iterator = ArrayRef<const OMPClause *>::iterator;
using clauselist_range = llvm::iterator_range<clauselist_iterator>;
using clauselist_const_range =
llvm::iterator_range<clauselist_const_iterator>;

unsigned clauselist_size() const { return Data->getNumClauses(); }
bool clauselist_empty() const { return Data->getClauses().empty(); }

clauselist_range clauselists() {
return clauselist_range(clauselist_begin(), clauselist_end());
}
clauselist_const_range clauselists() const {
return clauselist_const_range(clauselist_begin(), clauselist_end());
}
clauselist_iterator clauselist_begin() { return Data->getClauses().begin(); }
clauselist_iterator clauselist_end() { return Data->getClauses().end(); }
clauselist_const_iterator clauselist_begin() const {
return Data->getClauses().begin();
}
clauselist_const_iterator clauselist_end() const {
return Data->getClauses().end();
}


Expr *getMapperVarRef() { return cast_or_null<Expr>(Data->getChildren()[0]); }
const Expr *getMapperVarRef() const {
return cast_or_null<Expr>(Data->getChildren()[0]);
}

void setMapperVarRef(Expr *MapperVarRefE) {
Data->getChildren()[0] = MapperVarRefE;
}


DeclarationName getVarName() { return VarName; }



OMPDeclareMapperDecl *getPrevDeclInScope();
const OMPDeclareMapperDecl *getPrevDeclInScope() const;

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == OMPDeclareMapper; }
static DeclContext *castToDeclContext(const OMPDeclareMapperDecl *D) {
return static_cast<DeclContext *>(const_cast<OMPDeclareMapperDecl *>(D));
}
static OMPDeclareMapperDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<OMPDeclareMapperDecl *>(const_cast<DeclContext *>(DC));
}
};








class OMPCapturedExprDecl final : public VarDecl {
friend class ASTDeclReader;
void anchor() override;

OMPCapturedExprDecl(ASTContext &C, DeclContext *DC, IdentifierInfo *Id,
QualType Type, TypeSourceInfo *TInfo,
SourceLocation StartLoc)
: VarDecl(OMPCapturedExpr, C, DC, StartLoc, StartLoc, Id, Type, TInfo,
SC_None) {
setImplicit();
}

public:
static OMPCapturedExprDecl *Create(ASTContext &C, DeclContext *DC,
IdentifierInfo *Id, QualType T,
SourceLocation StartLoc);

static OMPCapturedExprDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == OMPCapturedExpr; }
};








class OMPRequiresDecl final : public OMPDeclarativeDirective<Decl> {
friend class OMPDeclarativeDirective<Decl>;
friend class ASTDeclReader;

virtual void anchor();

OMPRequiresDecl(DeclContext *DC, SourceLocation L)
: OMPDeclarativeDirective<Decl>(OMPRequires, DC, L) {}

public:

static OMPRequiresDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, ArrayRef<OMPClause *> CL);

static OMPRequiresDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned N);

using clauselist_iterator = MutableArrayRef<OMPClause *>::iterator;
using clauselist_const_iterator = ArrayRef<const OMPClause *>::iterator;
using clauselist_range = llvm::iterator_range<clauselist_iterator>;
using clauselist_const_range = llvm::iterator_range<clauselist_const_iterator>;

unsigned clauselist_size() const { return Data->getNumClauses(); }
bool clauselist_empty() const { return Data->getClauses().empty(); }

clauselist_range clauselists() {
return clauselist_range(clauselist_begin(), clauselist_end());
}
clauselist_const_range clauselists() const {
return clauselist_const_range(clauselist_begin(), clauselist_end());
}
clauselist_iterator clauselist_begin() { return Data->getClauses().begin(); }
clauselist_iterator clauselist_end() { return Data->getClauses().end(); }
clauselist_const_iterator clauselist_begin() const {
return Data->getClauses().begin();
}
clauselist_const_iterator clauselist_end() const {
return Data->getClauses().end();
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == OMPRequires; }
};














class OMPAllocateDecl final : public OMPDeclarativeDirective<Decl> {
friend class OMPDeclarativeDirective<Decl>;
friend class ASTDeclReader;

virtual void anchor();

OMPAllocateDecl(DeclContext *DC, SourceLocation L)
: OMPDeclarativeDirective<Decl>(OMPAllocate, DC, L) {}

ArrayRef<const Expr *> getVars() const {
auto **Storage = reinterpret_cast<Expr **>(Data->getChildren().data());
return llvm::makeArrayRef(Storage, Data->getNumChildren());
}

MutableArrayRef<Expr *> getVars() {
auto **Storage = reinterpret_cast<Expr **>(Data->getChildren().data());
return llvm::makeMutableArrayRef(Storage, Data->getNumChildren());
}

void setVars(ArrayRef<Expr *> VL);

public:
static OMPAllocateDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, ArrayRef<Expr *> VL,
ArrayRef<OMPClause *> CL);
static OMPAllocateDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned NVars, unsigned NClauses);

typedef MutableArrayRef<Expr *>::iterator varlist_iterator;
typedef ArrayRef<const Expr *>::iterator varlist_const_iterator;
typedef llvm::iterator_range<varlist_iterator> varlist_range;
typedef llvm::iterator_range<varlist_const_iterator> varlist_const_range;
using clauselist_iterator = MutableArrayRef<OMPClause *>::iterator;
using clauselist_const_iterator = ArrayRef<const OMPClause *>::iterator;
using clauselist_range = llvm::iterator_range<clauselist_iterator>;
using clauselist_const_range = llvm::iterator_range<clauselist_const_iterator>;

unsigned varlist_size() const { return Data->getNumChildren(); }
bool varlist_empty() const { return Data->getChildren().empty(); }
unsigned clauselist_size() const { return Data->getNumClauses(); }
bool clauselist_empty() const { return Data->getClauses().empty(); }

varlist_range varlists() {
return varlist_range(varlist_begin(), varlist_end());
}
varlist_const_range varlists() const {
return varlist_const_range(varlist_begin(), varlist_end());
}
varlist_iterator varlist_begin() { return getVars().begin(); }
varlist_iterator varlist_end() { return getVars().end(); }
varlist_const_iterator varlist_begin() const { return getVars().begin(); }
varlist_const_iterator varlist_end() const { return getVars().end(); }

clauselist_range clauselists() {
return clauselist_range(clauselist_begin(), clauselist_end());
}
clauselist_const_range clauselists() const {
return clauselist_const_range(clauselist_begin(), clauselist_end());
}
clauselist_iterator clauselist_begin() { return Data->getClauses().begin(); }
clauselist_iterator clauselist_end() { return Data->getClauses().end(); }
clauselist_const_iterator clauselist_begin() const {
return Data->getClauses().begin();
}
clauselist_const_iterator clauselist_end() const {
return Data->getClauses().end();
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == OMPAllocate; }
};

}

#endif
