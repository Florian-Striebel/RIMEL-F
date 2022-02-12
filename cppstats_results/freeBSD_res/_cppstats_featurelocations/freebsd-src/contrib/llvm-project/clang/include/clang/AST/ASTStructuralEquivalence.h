












#if !defined(LLVM_CLANG_AST_ASTSTRUCTURALEQUIVALENCE_H)
#define LLVM_CLANG_AST_ASTSTRUCTURALEQUIVALENCE_H

#include "clang/AST/DeclBase.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/Optional.h"
#include <queue>
#include <utility>

namespace clang {

class ASTContext;
class Decl;
class DiagnosticBuilder;
class QualType;
class RecordDecl;
class SourceLocation;




enum class StructuralEquivalenceKind {
Default,
Minimal,
};

struct StructuralEquivalenceContext {

ASTContext &FromCtx, &ToCtx;



std::queue<std::pair<Decl *, Decl *>> DeclsToCheck;



llvm::DenseSet<std::pair<Decl *, Decl *>> VisitedDecls;



llvm::DenseSet<std::pair<Decl *, Decl *>> &NonEquivalentDecls;

StructuralEquivalenceKind EqKind;



bool StrictTypeSpelling;


bool ErrorOnTagTypeMismatch;


bool Complain;


bool LastDiagFromC2 = false;

StructuralEquivalenceContext(
ASTContext &FromCtx, ASTContext &ToCtx,
llvm::DenseSet<std::pair<Decl *, Decl *>> &NonEquivalentDecls,
StructuralEquivalenceKind EqKind,
bool StrictTypeSpelling = false, bool Complain = true,
bool ErrorOnTagTypeMismatch = false)
: FromCtx(FromCtx), ToCtx(ToCtx), NonEquivalentDecls(NonEquivalentDecls),
EqKind(EqKind), StrictTypeSpelling(StrictTypeSpelling),
ErrorOnTagTypeMismatch(ErrorOnTagTypeMismatch), Complain(Complain) {}

DiagnosticBuilder Diag1(SourceLocation Loc, unsigned DiagID);
DiagnosticBuilder Diag2(SourceLocation Loc, unsigned DiagID);







bool IsEquivalent(Decl *D1, Decl *D2);






bool IsEquivalent(QualType T1, QualType T2);






bool IsEquivalent(Stmt *S1, Stmt *S2);











static llvm::Optional<unsigned>
findUntaggedStructOrUnionIndex(RecordDecl *Anon);



unsigned getApplicableDiagnostic(unsigned ErrorDiagnostic);

private:




bool Finish();




bool CheckCommonEquivalence(Decl *D1, Decl *D2);




bool CheckKindSpecificEquivalence(Decl *D1, Decl *D2);
};

}

#endif
