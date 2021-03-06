







#if !defined(LLVM_CLANG_LIB_ARCMIGRATE_TRANSFORMS_H)
#define LLVM_CLANG_LIB_ARCMIGRATE_TRANSFORMS_H

#include "clang/AST/ParentMap.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/SaveAndRestore.h"

namespace clang {
class Decl;
class Stmt;
class BlockDecl;
class ObjCMethodDecl;
class FunctionDecl;

namespace arcmt {
class MigrationPass;

namespace trans {

class MigrationContext;





void rewriteAutoreleasePool(MigrationPass &pass);
void rewriteUnbridgedCasts(MigrationPass &pass);
void makeAssignARCSafe(MigrationPass &pass);
void removeRetainReleaseDeallocFinalize(MigrationPass &pass);
void removeZeroOutPropsInDeallocFinalize(MigrationPass &pass);
void rewriteUnusedInitDelegate(MigrationPass &pass);
void checkAPIUses(MigrationPass &pass);

void removeEmptyStatementsAndDeallocFinalize(MigrationPass &pass);

class BodyContext {
MigrationContext &MigrateCtx;
ParentMap PMap;
Stmt *TopStmt;

public:
BodyContext(MigrationContext &MigrateCtx, Stmt *S)
: MigrateCtx(MigrateCtx), PMap(S), TopStmt(S) {}

MigrationContext &getMigrationContext() { return MigrateCtx; }
ParentMap &getParentMap() { return PMap; }
Stmt *getTopStmt() { return TopStmt; }
};

class ObjCImplementationContext {
MigrationContext &MigrateCtx;
ObjCImplementationDecl *ImpD;

public:
ObjCImplementationContext(MigrationContext &MigrateCtx,
ObjCImplementationDecl *D)
: MigrateCtx(MigrateCtx), ImpD(D) {}

MigrationContext &getMigrationContext() { return MigrateCtx; }
ObjCImplementationDecl *getImplementationDecl() { return ImpD; }
};

class ASTTraverser {
public:
virtual ~ASTTraverser();
virtual void traverseTU(MigrationContext &MigrateCtx) { }
virtual void traverseBody(BodyContext &BodyCtx) { }
virtual void traverseObjCImplementation(ObjCImplementationContext &ImplCtx) {}
};

class MigrationContext {
std::vector<ASTTraverser *> Traversers;

public:
MigrationPass &Pass;

struct GCAttrOccurrence {
enum AttrKind { Weak, Strong } Kind;
SourceLocation Loc;
QualType ModifiedType;
Decl *Dcl;


bool FullyMigratable;
};
std::vector<GCAttrOccurrence> GCAttrs;
llvm::DenseSet<SourceLocation> AttrSet;
llvm::DenseSet<SourceLocation> RemovedAttrSet;



llvm::DenseSet<SourceLocation> AtPropsWeak;

explicit MigrationContext(MigrationPass &pass) : Pass(pass) {}
~MigrationContext();

typedef std::vector<ASTTraverser *>::iterator traverser_iterator;
traverser_iterator traversers_begin() { return Traversers.begin(); }
traverser_iterator traversers_end() { return Traversers.end(); }

void addTraverser(ASTTraverser *traverser) {
Traversers.push_back(traverser);
}

bool isGCOwnedNonObjC(QualType T);
bool removePropertyAttribute(StringRef fromAttr, SourceLocation atLoc) {
return rewritePropertyAttribute(fromAttr, StringRef(), atLoc);
}
bool rewritePropertyAttribute(StringRef fromAttr, StringRef toAttr,
SourceLocation atLoc);
bool addPropertyAttribute(StringRef attr, SourceLocation atLoc);

void traverse(TranslationUnitDecl *TU);

void dumpGCAttrs();
};

class PropertyRewriteTraverser : public ASTTraverser {
public:
void traverseObjCImplementation(ObjCImplementationContext &ImplCtx) override;
};

class BlockObjCVariableTraverser : public ASTTraverser {
public:
void traverseBody(BodyContext &BodyCtx) override;
};

class ProtectedScopeTraverser : public ASTTraverser {
public:
void traverseBody(BodyContext &BodyCtx) override;
};



class GCAttrsTraverser : public ASTTraverser {
public:
void traverseTU(MigrationContext &MigrateCtx) override;
};

class GCCollectableCallsTraverser : public ASTTraverser {
public:
void traverseBody(BodyContext &BodyCtx) override;
};






bool canApplyWeak(ASTContext &Ctx, QualType type,
bool AllowOnUnknownClass = false);

bool isPlusOneAssign(const BinaryOperator *E);
bool isPlusOne(const Expr *E);





SourceLocation findLocationAfterSemi(SourceLocation loc, ASTContext &Ctx,
bool IsDecl = false);





SourceLocation findSemiAfterLocation(SourceLocation loc, ASTContext &Ctx,
bool IsDecl = false);

bool hasSideEffects(Expr *E, ASTContext &Ctx);
bool isGlobalVar(Expr *E);

StringRef getNilString(MigrationPass &Pass);

template <typename BODY_TRANS>
class BodyTransform : public RecursiveASTVisitor<BodyTransform<BODY_TRANS> > {
MigrationPass &Pass;
Decl *ParentD;

typedef RecursiveASTVisitor<BodyTransform<BODY_TRANS> > base;
public:
BodyTransform(MigrationPass &pass) : Pass(pass), ParentD(nullptr) { }

bool TraverseStmt(Stmt *rootS) {
if (rootS)
BODY_TRANS(Pass).transformBody(rootS, ParentD);
return true;
}

bool TraverseObjCMethodDecl(ObjCMethodDecl *D) {
SaveAndRestore<Decl *> SetParent(ParentD, D);
return base::TraverseObjCMethodDecl(D);
}
};

typedef llvm::DenseSet<Expr *> ExprSet;

void clearRefsIn(Stmt *S, ExprSet &refs);
template <typename iterator>
void clearRefsIn(iterator begin, iterator end, ExprSet &refs) {
for (; begin != end; ++begin)
clearRefsIn(*begin, refs);
}

void collectRefs(ValueDecl *D, Stmt *S, ExprSet &refs);

void collectRemovables(Stmt *S, ExprSet &exprs);

}

}

}

#endif
