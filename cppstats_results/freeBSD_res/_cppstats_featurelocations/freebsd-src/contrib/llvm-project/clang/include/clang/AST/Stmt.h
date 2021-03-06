











#if !defined(LLVM_CLANG_AST_STMT_H)
#define LLVM_CLANG_AST_STMT_H

#include "clang/AST/DeclGroup.h"
#include "clang/AST/DependenceFlags.h"
#include "clang/AST/StmtIterator.h"
#include "clang/Basic/CapturedStmt.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/BitmaskEnum.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>

namespace llvm {

class FoldingSetNodeID;

}

namespace clang {

class ASTContext;
class Attr;
class CapturedDecl;
class Decl;
class Expr;
class AddrLabelExpr;
class LabelDecl;
class ODRHash;
class PrinterHelper;
struct PrintingPolicy;
class RecordDecl;
class SourceManager;
class StringLiteral;
class Token;
class VarDecl;







class alignas(void *) Stmt {
public:
enum StmtClass {
NoStmtClass = 0,
#define STMT(CLASS, PARENT) CLASS##Class,
#define STMT_RANGE(BASE, FIRST, LAST) first##BASE##Constant=FIRST##Class, last##BASE##Constant=LAST##Class,

#define LAST_STMT_RANGE(BASE, FIRST, LAST) first##BASE##Constant=FIRST##Class, last##BASE##Constant=LAST##Class

#define ABSTRACT_STMT(STMT)
#include "clang/AST/StmtNodes.inc"
};


protected:
friend class ASTStmtReader;
friend class ASTStmtWriter;

void *operator new(size_t bytes) noexcept {
llvm_unreachable("Stmts cannot be allocated with regular 'new'.");
}

void operator delete(void *data) noexcept {
llvm_unreachable("Stmts cannot be released with regular 'delete'.");
}



class StmtBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class Stmt;


unsigned sClass : 8;
};
enum { NumStmtBits = 8 };

class NullStmtBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class NullStmt;

unsigned : NumStmtBits;






unsigned HasLeadingEmptyMacro : 1;


SourceLocation SemiLoc;
};

class CompoundStmtBitfields {
friend class ASTStmtReader;
friend class CompoundStmt;

unsigned : NumStmtBits;

unsigned NumStmts : 32 - NumStmtBits;


SourceLocation LBraceLoc;
};

class LabelStmtBitfields {
friend class LabelStmt;

unsigned : NumStmtBits;

SourceLocation IdentLoc;
};

class AttributedStmtBitfields {
friend class ASTStmtReader;
friend class AttributedStmt;

unsigned : NumStmtBits;


unsigned NumAttrs : 32 - NumStmtBits;


SourceLocation AttrLoc;
};

class IfStmtBitfields {
friend class ASTStmtReader;
friend class IfStmt;

unsigned : NumStmtBits;


unsigned IsConstexpr : 1;


unsigned HasElse : 1;


unsigned HasVar : 1;


unsigned HasInit : 1;


SourceLocation IfLoc;
};

class SwitchStmtBitfields {
friend class SwitchStmt;

unsigned : NumStmtBits;


unsigned HasInit : 1;


unsigned HasVar : 1;




unsigned AllEnumCasesCovered : 1;


SourceLocation SwitchLoc;
};

class WhileStmtBitfields {
friend class ASTStmtReader;
friend class WhileStmt;

unsigned : NumStmtBits;


unsigned HasVar : 1;


SourceLocation WhileLoc;
};

class DoStmtBitfields {
friend class DoStmt;

unsigned : NumStmtBits;


SourceLocation DoLoc;
};

class ForStmtBitfields {
friend class ForStmt;

unsigned : NumStmtBits;


SourceLocation ForLoc;
};

class GotoStmtBitfields {
friend class GotoStmt;
friend class IndirectGotoStmt;

unsigned : NumStmtBits;


SourceLocation GotoLoc;
};

class ContinueStmtBitfields {
friend class ContinueStmt;

unsigned : NumStmtBits;


SourceLocation ContinueLoc;
};

class BreakStmtBitfields {
friend class BreakStmt;

unsigned : NumStmtBits;


SourceLocation BreakLoc;
};

class ReturnStmtBitfields {
friend class ReturnStmt;

unsigned : NumStmtBits;


unsigned HasNRVOCandidate : 1;


SourceLocation RetLoc;
};

class SwitchCaseBitfields {
friend class SwitchCase;
friend class CaseStmt;

unsigned : NumStmtBits;



unsigned CaseStmtIsGNURange : 1;


SourceLocation KeywordLoc;
};



class ExprBitfields {
friend class ASTStmtReader;
friend class AtomicExpr;
friend class BlockDeclRefExpr;
friend class CallExpr;
friend class CXXConstructExpr;
friend class CXXDependentScopeMemberExpr;
friend class CXXNewExpr;
friend class CXXUnresolvedConstructExpr;
friend class DeclRefExpr;
friend class DependentScopeDeclRefExpr;
friend class DesignatedInitExpr;
friend class Expr;
friend class InitListExpr;
friend class ObjCArrayLiteral;
friend class ObjCDictionaryLiteral;
friend class ObjCMessageExpr;
friend class OffsetOfExpr;
friend class OpaqueValueExpr;
friend class OverloadExpr;
friend class ParenListExpr;
friend class PseudoObjectExpr;
friend class ShuffleVectorExpr;

unsigned : NumStmtBits;

unsigned ValueKind : 2;
unsigned ObjectKind : 3;
unsigned Dependent : llvm::BitWidth<ExprDependence>;
};
enum { NumExprBits = NumStmtBits + 5 + llvm::BitWidth<ExprDependence> };

class ConstantExprBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class ConstantExpr;

unsigned : NumExprBits;


unsigned ResultKind : 2;


unsigned APValueKind : 4;



unsigned IsUnsigned : 1;




unsigned BitWidth : 7;



unsigned HasCleanup : 1;


unsigned IsImmediateInvocation : 1;
};

class PredefinedExprBitfields {
friend class ASTStmtReader;
friend class PredefinedExpr;

unsigned : NumExprBits;



unsigned Kind : 4;



unsigned HasFunctionName : 1;


SourceLocation Loc;
};

class DeclRefExprBitfields {
friend class ASTStmtReader;
friend class DeclRefExpr;

unsigned : NumExprBits;

unsigned HasQualifier : 1;
unsigned HasTemplateKWAndArgsInfo : 1;
unsigned HasFoundDecl : 1;
unsigned HadMultipleCandidates : 1;
unsigned RefersToEnclosingVariableOrCapture : 1;
unsigned NonOdrUseReason : 2;


SourceLocation Loc;
};


class FloatingLiteralBitfields {
friend class FloatingLiteral;

unsigned : NumExprBits;

unsigned Semantics : 3;
unsigned IsExact : 1;
};

class StringLiteralBitfields {
friend class ASTStmtReader;
friend class StringLiteral;

unsigned : NumExprBits;



unsigned Kind : 3;




unsigned CharByteWidth : 3;

unsigned IsPascal : 1;



unsigned NumConcatenated;
};

class CharacterLiteralBitfields {
friend class CharacterLiteral;

unsigned : NumExprBits;

unsigned Kind : 3;
};

class UnaryOperatorBitfields {
friend class UnaryOperator;

unsigned : NumExprBits;

unsigned Opc : 5;
unsigned CanOverflow : 1;




unsigned HasFPFeatures : 1;

SourceLocation Loc;
};

class UnaryExprOrTypeTraitExprBitfields {
friend class UnaryExprOrTypeTraitExpr;

unsigned : NumExprBits;

unsigned Kind : 3;
unsigned IsType : 1;
};

class ArrayOrMatrixSubscriptExprBitfields {
friend class ArraySubscriptExpr;
friend class MatrixSubscriptExpr;

unsigned : NumExprBits;

SourceLocation RBracketLoc;
};

class CallExprBitfields {
friend class CallExpr;

unsigned : NumExprBits;

unsigned NumPreArgs : 1;


unsigned UsesADL : 1;


unsigned HasFPFeatures : 1;


unsigned : 24 - 3 - NumExprBits;




unsigned OffsetToTrailingObjects : 8;
};
enum { NumCallExprBits = 32 };

class MemberExprBitfields {
friend class ASTStmtReader;
friend class MemberExpr;

unsigned : NumExprBits;


unsigned IsArrow : 1;





unsigned HasQualifierOrFoundDecl : 1;






unsigned HasTemplateKWAndArgsInfo : 1;



unsigned HadMultipleCandidates : 1;




unsigned NonOdrUseReason : 2;


SourceLocation OperatorLoc;
};

class CastExprBitfields {
friend class CastExpr;
friend class ImplicitCastExpr;

unsigned : NumExprBits;

unsigned Kind : 7;
unsigned PartOfExplicitCast : 1;


unsigned HasFPFeatures : 1;



unsigned BasePathSize;
};

class BinaryOperatorBitfields {
friend class BinaryOperator;

unsigned : NumExprBits;

unsigned Opc : 6;




unsigned HasFPFeatures : 1;

SourceLocation OpLoc;
};

class InitListExprBitfields {
friend class InitListExpr;

unsigned : NumExprBits;



unsigned HadArrayRangeDesignator : 1;
};

class ParenListExprBitfields {
friend class ASTStmtReader;
friend class ParenListExpr;

unsigned : NumExprBits;


unsigned NumExprs;
};

class GenericSelectionExprBitfields {
friend class ASTStmtReader;
friend class GenericSelectionExpr;

unsigned : NumExprBits;


SourceLocation GenericLoc;
};

class PseudoObjectExprBitfields {
friend class ASTStmtReader;
friend class PseudoObjectExpr;

unsigned : NumExprBits;



unsigned NumSubExprs : 8;
unsigned ResultIndex : 32 - 8 - NumExprBits;
};

class SourceLocExprBitfields {
friend class ASTStmtReader;
friend class SourceLocExpr;

unsigned : NumExprBits;



unsigned Kind : 2;
};

class StmtExprBitfields {
friend class ASTStmtReader;
friend class StmtExpr;

unsigned : NumExprBits;




unsigned TemplateDepth;
};



class CXXOperatorCallExprBitfields {
friend class ASTStmtReader;
friend class CXXOperatorCallExpr;

unsigned : NumCallExprBits;



unsigned OperatorKind : 6;
};

class CXXRewrittenBinaryOperatorBitfields {
friend class ASTStmtReader;
friend class CXXRewrittenBinaryOperator;

unsigned : NumCallExprBits;

unsigned IsReversed : 1;
};

class CXXBoolLiteralExprBitfields {
friend class CXXBoolLiteralExpr;

unsigned : NumExprBits;


unsigned Value : 1;


SourceLocation Loc;
};

class CXXNullPtrLiteralExprBitfields {
friend class CXXNullPtrLiteralExpr;

unsigned : NumExprBits;


SourceLocation Loc;
};

class CXXThisExprBitfields {
friend class CXXThisExpr;

unsigned : NumExprBits;


unsigned IsImplicit : 1;


SourceLocation Loc;
};

class CXXThrowExprBitfields {
friend class ASTStmtReader;
friend class CXXThrowExpr;

unsigned : NumExprBits;


unsigned IsThrownVariableInScope : 1;


SourceLocation ThrowLoc;
};

class CXXDefaultArgExprBitfields {
friend class ASTStmtReader;
friend class CXXDefaultArgExpr;

unsigned : NumExprBits;


SourceLocation Loc;
};

class CXXDefaultInitExprBitfields {
friend class ASTStmtReader;
friend class CXXDefaultInitExpr;

unsigned : NumExprBits;


SourceLocation Loc;
};

class CXXScalarValueInitExprBitfields {
friend class ASTStmtReader;
friend class CXXScalarValueInitExpr;

unsigned : NumExprBits;

SourceLocation RParenLoc;
};

class CXXNewExprBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class CXXNewExpr;

unsigned : NumExprBits;


unsigned IsGlobalNew : 1;



unsigned IsArray : 1;


unsigned ShouldPassAlignment : 1;



unsigned UsualArrayDeleteWantsSize : 1;




unsigned StoredInitializationStyle : 2;


unsigned IsParenTypeId : 1;


unsigned NumPlacementArgs;
};

class CXXDeleteExprBitfields {
friend class ASTStmtReader;
friend class CXXDeleteExpr;

unsigned : NumExprBits;


unsigned GlobalDelete : 1;


unsigned ArrayForm : 1;




unsigned ArrayFormAsWritten : 1;



unsigned UsualArrayDeleteWantsSize : 1;


SourceLocation Loc;
};

class TypeTraitExprBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class TypeTraitExpr;

unsigned : NumExprBits;


unsigned Kind : 8;



unsigned Value : 1;




unsigned NumArgs;
};

class DependentScopeDeclRefExprBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class DependentScopeDeclRefExpr;

unsigned : NumExprBits;



unsigned HasTemplateKWAndArgsInfo : 1;
};

class CXXConstructExprBitfields {
friend class ASTStmtReader;
friend class CXXConstructExpr;

unsigned : NumExprBits;

unsigned Elidable : 1;
unsigned HadMultipleCandidates : 1;
unsigned ListInitialization : 1;
unsigned StdInitListInitialization : 1;
unsigned ZeroInitialization : 1;
unsigned ConstructionKind : 3;

SourceLocation Loc;
};

class ExprWithCleanupsBitfields {
friend class ASTStmtReader;
friend class ExprWithCleanups;

unsigned : NumExprBits;


unsigned CleanupsHaveSideEffects : 1;

unsigned NumObjects : 32 - 1 - NumExprBits;
};

class CXXUnresolvedConstructExprBitfields {
friend class ASTStmtReader;
friend class CXXUnresolvedConstructExpr;

unsigned : NumExprBits;


unsigned NumArgs;
};

class CXXDependentScopeMemberExprBitfields {
friend class ASTStmtReader;
friend class CXXDependentScopeMemberExpr;

unsigned : NumExprBits;



unsigned IsArrow : 1;



unsigned HasTemplateKWAndArgsInfo : 1;



unsigned HasFirstQualifierFoundInScope : 1;


SourceLocation OperatorLoc;
};

class OverloadExprBitfields {
friend class ASTStmtReader;
friend class OverloadExpr;

unsigned : NumExprBits;



unsigned HasTemplateKWAndArgsInfo : 1;




unsigned : 32 - NumExprBits - 1;


unsigned NumResults;
};
enum { NumOverloadExprBits = NumExprBits + 1 };

class UnresolvedLookupExprBitfields {
friend class ASTStmtReader;
friend class UnresolvedLookupExpr;

unsigned : NumOverloadExprBits;



unsigned RequiresADL : 1;



unsigned Overloaded : 1;
};
static_assert(sizeof(UnresolvedLookupExprBitfields) <= 4,
"UnresolvedLookupExprBitfields must be <= than 4 bytes to"
"avoid trashing OverloadExprBitfields::NumResults!");

class UnresolvedMemberExprBitfields {
friend class ASTStmtReader;
friend class UnresolvedMemberExpr;

unsigned : NumOverloadExprBits;



unsigned IsArrow : 1;


unsigned HasUnresolvedUsing : 1;
};
static_assert(sizeof(UnresolvedMemberExprBitfields) <= 4,
"UnresolvedMemberExprBitfields must be <= than 4 bytes to"
"avoid trashing OverloadExprBitfields::NumResults!");

class CXXNoexceptExprBitfields {
friend class ASTStmtReader;
friend class CXXNoexceptExpr;

unsigned : NumExprBits;

unsigned Value : 1;
};

class SubstNonTypeTemplateParmExprBitfields {
friend class ASTStmtReader;
friend class SubstNonTypeTemplateParmExpr;

unsigned : NumExprBits;


SourceLocation NameLoc;
};

class LambdaExprBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class LambdaExpr;

unsigned : NumExprBits;



unsigned CaptureDefault : 2;



unsigned ExplicitParams : 1;


unsigned ExplicitResultType : 1;


unsigned NumCaptures : 16;
};

class RequiresExprBitfields {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class RequiresExpr;

unsigned : NumExprBits;

unsigned IsSatisfied : 1;
SourceLocation RequiresKWLoc;
};



class CoawaitExprBitfields {
friend class CoawaitExpr;

unsigned : NumExprBits;

unsigned IsImplicit : 1;
};



class ObjCIndirectCopyRestoreExprBitfields {
friend class ObjCIndirectCopyRestoreExpr;

unsigned : NumExprBits;

unsigned ShouldCopy : 1;
};



class OpaqueValueExprBitfields {
friend class ASTStmtReader;
friend class OpaqueValueExpr;

unsigned : NumExprBits;



unsigned IsUnique : 1;

SourceLocation Loc;
};

union {


StmtBitfields StmtBits;
NullStmtBitfields NullStmtBits;
CompoundStmtBitfields CompoundStmtBits;
LabelStmtBitfields LabelStmtBits;
AttributedStmtBitfields AttributedStmtBits;
IfStmtBitfields IfStmtBits;
SwitchStmtBitfields SwitchStmtBits;
WhileStmtBitfields WhileStmtBits;
DoStmtBitfields DoStmtBits;
ForStmtBitfields ForStmtBits;
GotoStmtBitfields GotoStmtBits;
ContinueStmtBitfields ContinueStmtBits;
BreakStmtBitfields BreakStmtBits;
ReturnStmtBitfields ReturnStmtBits;
SwitchCaseBitfields SwitchCaseBits;


ExprBitfields ExprBits;
ConstantExprBitfields ConstantExprBits;
PredefinedExprBitfields PredefinedExprBits;
DeclRefExprBitfields DeclRefExprBits;
FloatingLiteralBitfields FloatingLiteralBits;
StringLiteralBitfields StringLiteralBits;
CharacterLiteralBitfields CharacterLiteralBits;
UnaryOperatorBitfields UnaryOperatorBits;
UnaryExprOrTypeTraitExprBitfields UnaryExprOrTypeTraitExprBits;
ArrayOrMatrixSubscriptExprBitfields ArrayOrMatrixSubscriptExprBits;
CallExprBitfields CallExprBits;
MemberExprBitfields MemberExprBits;
CastExprBitfields CastExprBits;
BinaryOperatorBitfields BinaryOperatorBits;
InitListExprBitfields InitListExprBits;
ParenListExprBitfields ParenListExprBits;
GenericSelectionExprBitfields GenericSelectionExprBits;
PseudoObjectExprBitfields PseudoObjectExprBits;
SourceLocExprBitfields SourceLocExprBits;


StmtExprBitfields StmtExprBits;


CXXOperatorCallExprBitfields CXXOperatorCallExprBits;
CXXRewrittenBinaryOperatorBitfields CXXRewrittenBinaryOperatorBits;
CXXBoolLiteralExprBitfields CXXBoolLiteralExprBits;
CXXNullPtrLiteralExprBitfields CXXNullPtrLiteralExprBits;
CXXThisExprBitfields CXXThisExprBits;
CXXThrowExprBitfields CXXThrowExprBits;
CXXDefaultArgExprBitfields CXXDefaultArgExprBits;
CXXDefaultInitExprBitfields CXXDefaultInitExprBits;
CXXScalarValueInitExprBitfields CXXScalarValueInitExprBits;
CXXNewExprBitfields CXXNewExprBits;
CXXDeleteExprBitfields CXXDeleteExprBits;
TypeTraitExprBitfields TypeTraitExprBits;
DependentScopeDeclRefExprBitfields DependentScopeDeclRefExprBits;
CXXConstructExprBitfields CXXConstructExprBits;
ExprWithCleanupsBitfields ExprWithCleanupsBits;
CXXUnresolvedConstructExprBitfields CXXUnresolvedConstructExprBits;
CXXDependentScopeMemberExprBitfields CXXDependentScopeMemberExprBits;
OverloadExprBitfields OverloadExprBits;
UnresolvedLookupExprBitfields UnresolvedLookupExprBits;
UnresolvedMemberExprBitfields UnresolvedMemberExprBits;
CXXNoexceptExprBitfields CXXNoexceptExprBits;
SubstNonTypeTemplateParmExprBitfields SubstNonTypeTemplateParmExprBits;
LambdaExprBitfields LambdaExprBits;
RequiresExprBitfields RequiresExprBits;


CoawaitExprBitfields CoawaitBits;


ObjCIndirectCopyRestoreExprBitfields ObjCIndirectCopyRestoreExprBits;


OpaqueValueExprBitfields OpaqueValueExprBits;
};

public:


void* operator new(size_t bytes, const ASTContext& C,
unsigned alignment = 8);

void* operator new(size_t bytes, const ASTContext* C,
unsigned alignment = 8) {
return operator new(bytes, *C, alignment);
}

void *operator new(size_t bytes, void *mem) noexcept { return mem; }

void operator delete(void *, const ASTContext &, unsigned) noexcept {}
void operator delete(void *, const ASTContext *, unsigned) noexcept {}
void operator delete(void *, size_t) noexcept {}
void operator delete(void *, void *) noexcept {}

public:



struct EmptyShell {};


enum Likelihood {
LH_Unlikely = -1,
LH_None,

LH_Likely
};

protected:




template<typename T, typename TPtr = T *, typename StmtPtr = Stmt *>
struct CastIterator
: llvm::iterator_adaptor_base<CastIterator<T, TPtr, StmtPtr>, StmtPtr *,
std::random_access_iterator_tag, TPtr> {
using Base = typename CastIterator::iterator_adaptor_base;

CastIterator() : Base(nullptr) {}
CastIterator(StmtPtr *I) : Base(I) {}

typename Base::value_type operator*() const {
return cast_or_null<T>(*this->I);
}
};


template <typename T>
using ConstCastIterator = CastIterator<T, const T *const, const Stmt *const>;

using ExprIterator = CastIterator<Expr>;
using ConstExprIterator = ConstCastIterator<Expr>;

private:

static bool StatisticsEnabled;

protected:

explicit Stmt(StmtClass SC, EmptyShell) : Stmt(SC) {}

public:
Stmt() = delete;
Stmt(const Stmt &) = delete;
Stmt(Stmt &&) = delete;
Stmt &operator=(const Stmt &) = delete;
Stmt &operator=(Stmt &&) = delete;

Stmt(StmtClass SC) {
static_assert(sizeof(*this) <= 8,
"changing bitfields changed sizeof(Stmt)");
static_assert(sizeof(*this) % alignof(void *) == 0,
"Insufficient alignment!");
StmtBits.sClass = SC;
if (StatisticsEnabled) Stmt::addStmtClass(SC);
}

StmtClass getStmtClass() const {
return static_cast<StmtClass>(StmtBits.sClass);
}

const char *getStmtClassName() const;




SourceRange getSourceRange() const LLVM_READONLY;
SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;


static void addStmtClass(const StmtClass s);
static void EnableStatistics();
static void PrintStats();


static Likelihood getLikelihood(ArrayRef<const Attr *> Attrs);


static Likelihood getLikelihood(const Stmt *S);


static const Attr *getLikelihoodAttr(const Stmt *S);




static Likelihood getLikelihood(const Stmt *Then, const Stmt *Else);




static std::tuple<bool, const Attr *, const Attr *>
determineLikelihoodConflict(const Stmt *Then, const Stmt *Else);



void dump() const;
void dump(raw_ostream &OS, const ASTContext &Context) const;


int64_t getID(const ASTContext &Context) const;


void dumpColor() const;



void dumpPretty(const ASTContext &Context) const;
void printPretty(raw_ostream &OS, PrinterHelper *Helper,
const PrintingPolicy &Policy, unsigned Indentation = 0,
StringRef NewlineSymbol = "\n",
const ASTContext *Context = nullptr) const;


void printJson(raw_ostream &Out, PrinterHelper *Helper,
const PrintingPolicy &Policy, bool AddQuotes) const;



void viewAST() const;



Stmt *IgnoreContainers(bool IgnoreCaptured = false);
const Stmt *IgnoreContainers(bool IgnoreCaptured = false) const {
return const_cast<Stmt *>(this)->IgnoreContainers(IgnoreCaptured);
}

const Stmt *stripLabelLikeStatements() const;
Stmt *stripLabelLikeStatements() {
return const_cast<Stmt*>(
const_cast<const Stmt*>(this)->stripLabelLikeStatements());
}




using child_iterator = StmtIterator;
using const_child_iterator = ConstStmtIterator;

using child_range = llvm::iterator_range<child_iterator>;
using const_child_range = llvm::iterator_range<const_child_iterator>;

child_range children();

const_child_range children() const {
auto Children = const_cast<Stmt *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_iterator child_begin() { return children().begin(); }
child_iterator child_end() { return children().end(); }

const_child_iterator child_begin() const { return children().begin(); }
const_child_iterator child_end() const { return children().end(); }













void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
bool Canonical) const;








void ProcessODRHash(llvm::FoldingSetNodeID &ID, ODRHash& Hash) const;
};





class DeclStmt : public Stmt {
DeclGroupRef DG;
SourceLocation StartLoc, EndLoc;

public:
DeclStmt(DeclGroupRef dg, SourceLocation startLoc, SourceLocation endLoc)
: Stmt(DeclStmtClass), DG(dg), StartLoc(startLoc), EndLoc(endLoc) {}


explicit DeclStmt(EmptyShell Empty) : Stmt(DeclStmtClass, Empty) {}



bool isSingleDecl() const { return DG.isSingleDecl(); }

const Decl *getSingleDecl() const { return DG.getSingleDecl(); }
Decl *getSingleDecl() { return DG.getSingleDecl(); }

const DeclGroupRef getDeclGroup() const { return DG; }
DeclGroupRef getDeclGroup() { return DG; }
void setDeclGroup(DeclGroupRef DGR) { DG = DGR; }

void setStartLoc(SourceLocation L) { StartLoc = L; }
SourceLocation getEndLoc() const { return EndLoc; }
void setEndLoc(SourceLocation L) { EndLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return StartLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == DeclStmtClass;
}


child_range children() {
return child_range(child_iterator(DG.begin(), DG.end()),
child_iterator(DG.end(), DG.end()));
}

const_child_range children() const {
auto Children = const_cast<DeclStmt *>(this)->children();
return const_child_range(Children);
}

using decl_iterator = DeclGroupRef::iterator;
using const_decl_iterator = DeclGroupRef::const_iterator;
using decl_range = llvm::iterator_range<decl_iterator>;
using decl_const_range = llvm::iterator_range<const_decl_iterator>;

decl_range decls() { return decl_range(decl_begin(), decl_end()); }

decl_const_range decls() const {
return decl_const_range(decl_begin(), decl_end());
}

decl_iterator decl_begin() { return DG.begin(); }
decl_iterator decl_end() { return DG.end(); }
const_decl_iterator decl_begin() const { return DG.begin(); }
const_decl_iterator decl_end() const { return DG.end(); }

using reverse_decl_iterator = std::reverse_iterator<decl_iterator>;

reverse_decl_iterator decl_rbegin() {
return reverse_decl_iterator(decl_end());
}

reverse_decl_iterator decl_rend() {
return reverse_decl_iterator(decl_begin());
}
};



class NullStmt : public Stmt {
public:
NullStmt(SourceLocation L, bool hasLeadingEmptyMacro = false)
: Stmt(NullStmtClass) {
NullStmtBits.HasLeadingEmptyMacro = hasLeadingEmptyMacro;
setSemiLoc(L);
}


explicit NullStmt(EmptyShell Empty) : Stmt(NullStmtClass, Empty) {}

SourceLocation getSemiLoc() const { return NullStmtBits.SemiLoc; }
void setSemiLoc(SourceLocation L) { NullStmtBits.SemiLoc = L; }

bool hasLeadingEmptyMacro() const {
return NullStmtBits.HasLeadingEmptyMacro;
}

SourceLocation getBeginLoc() const { return getSemiLoc(); }
SourceLocation getEndLoc() const { return getSemiLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == NullStmtClass;
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};


class CompoundStmt final : public Stmt,
private llvm::TrailingObjects<CompoundStmt, Stmt *> {
friend class ASTStmtReader;
friend TrailingObjects;


SourceLocation RBraceLoc;

CompoundStmt(ArrayRef<Stmt *> Stmts, SourceLocation LB, SourceLocation RB);
explicit CompoundStmt(EmptyShell Empty) : Stmt(CompoundStmtClass, Empty) {}

void setStmts(ArrayRef<Stmt *> Stmts);

public:
static CompoundStmt *Create(const ASTContext &C, ArrayRef<Stmt *> Stmts,
SourceLocation LB, SourceLocation RB);


explicit CompoundStmt(SourceLocation Loc)
: Stmt(CompoundStmtClass), RBraceLoc(Loc) {
CompoundStmtBits.NumStmts = 0;
CompoundStmtBits.LBraceLoc = Loc;
}


static CompoundStmt *CreateEmpty(const ASTContext &C, unsigned NumStmts);

bool body_empty() const { return CompoundStmtBits.NumStmts == 0; }
unsigned size() const { return CompoundStmtBits.NumStmts; }

using body_iterator = Stmt **;
using body_range = llvm::iterator_range<body_iterator>;

body_range body() { return body_range(body_begin(), body_end()); }
body_iterator body_begin() { return getTrailingObjects<Stmt *>(); }
body_iterator body_end() { return body_begin() + size(); }
Stmt *body_front() { return !body_empty() ? body_begin()[0] : nullptr; }

Stmt *body_back() {
return !body_empty() ? body_begin()[size() - 1] : nullptr;
}

using const_body_iterator = Stmt *const *;
using body_const_range = llvm::iterator_range<const_body_iterator>;

body_const_range body() const {
return body_const_range(body_begin(), body_end());
}

const_body_iterator body_begin() const {
return getTrailingObjects<Stmt *>();
}

const_body_iterator body_end() const { return body_begin() + size(); }

const Stmt *body_front() const {
return !body_empty() ? body_begin()[0] : nullptr;
}

const Stmt *body_back() const {
return !body_empty() ? body_begin()[size() - 1] : nullptr;
}

using reverse_body_iterator = std::reverse_iterator<body_iterator>;

reverse_body_iterator body_rbegin() {
return reverse_body_iterator(body_end());
}

reverse_body_iterator body_rend() {
return reverse_body_iterator(body_begin());
}

using const_reverse_body_iterator =
std::reverse_iterator<const_body_iterator>;

const_reverse_body_iterator body_rbegin() const {
return const_reverse_body_iterator(body_end());
}

const_reverse_body_iterator body_rend() const {
return const_reverse_body_iterator(body_begin());
}









Stmt *getStmtExprResult() {
for (auto *B : llvm::reverse(body())) {
if (!isa<NullStmt>(B))
return B;
}
return body_back();
}

const Stmt *getStmtExprResult() const {
return const_cast<CompoundStmt *>(this)->getStmtExprResult();
}

SourceLocation getBeginLoc() const { return CompoundStmtBits.LBraceLoc; }
SourceLocation getEndLoc() const { return RBraceLoc; }

SourceLocation getLBracLoc() const { return CompoundStmtBits.LBraceLoc; }
SourceLocation getRBracLoc() const { return RBraceLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CompoundStmtClass;
}


child_range children() { return child_range(body_begin(), body_end()); }

const_child_range children() const {
return const_child_range(body_begin(), body_end());
}
};


class SwitchCase : public Stmt {
protected:

SourceLocation ColonLoc;






SwitchCase *NextSwitchCase = nullptr;

SwitchCase(StmtClass SC, SourceLocation KWLoc, SourceLocation ColonLoc)
: Stmt(SC), ColonLoc(ColonLoc) {
setKeywordLoc(KWLoc);
}

SwitchCase(StmtClass SC, EmptyShell) : Stmt(SC) {}

public:
const SwitchCase *getNextSwitchCase() const { return NextSwitchCase; }
SwitchCase *getNextSwitchCase() { return NextSwitchCase; }
void setNextSwitchCase(SwitchCase *SC) { NextSwitchCase = SC; }

SourceLocation getKeywordLoc() const { return SwitchCaseBits.KeywordLoc; }
void setKeywordLoc(SourceLocation L) { SwitchCaseBits.KeywordLoc = L; }
SourceLocation getColonLoc() const { return ColonLoc; }
void setColonLoc(SourceLocation L) { ColonLoc = L; }

inline Stmt *getSubStmt();
const Stmt *getSubStmt() const {
return const_cast<SwitchCase *>(this)->getSubStmt();
}

SourceLocation getBeginLoc() const { return getKeywordLoc(); }
inline SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == CaseStmtClass ||
T->getStmtClass() == DefaultStmtClass;
}
};



class CaseStmt final
: public SwitchCase,
private llvm::TrailingObjects<CaseStmt, Stmt *, SourceLocation> {
friend TrailingObjects;
















enum { LhsOffset = 0, SubStmtOffsetFromRhs = 1 };
enum { NumMandatoryStmtPtr = 2 };

unsigned numTrailingObjects(OverloadToken<Stmt *>) const {
return NumMandatoryStmtPtr + caseStmtIsGNURange();
}

unsigned numTrailingObjects(OverloadToken<SourceLocation>) const {
return caseStmtIsGNURange();
}

unsigned lhsOffset() const { return LhsOffset; }
unsigned rhsOffset() const { return LhsOffset + caseStmtIsGNURange(); }
unsigned subStmtOffset() const { return rhsOffset() + SubStmtOffsetFromRhs; }



CaseStmt(Expr *lhs, Expr *rhs, SourceLocation caseLoc,
SourceLocation ellipsisLoc, SourceLocation colonLoc)
: SwitchCase(CaseStmtClass, caseLoc, colonLoc) {

bool IsGNURange = rhs != nullptr;
SwitchCaseBits.CaseStmtIsGNURange = IsGNURange;
setLHS(lhs);
setSubStmt(nullptr);
if (IsGNURange) {
setRHS(rhs);
setEllipsisLoc(ellipsisLoc);
}
}


explicit CaseStmt(EmptyShell Empty, bool CaseStmtIsGNURange)
: SwitchCase(CaseStmtClass, Empty) {
SwitchCaseBits.CaseStmtIsGNURange = CaseStmtIsGNURange;
}

public:

static CaseStmt *Create(const ASTContext &Ctx, Expr *lhs, Expr *rhs,
SourceLocation caseLoc, SourceLocation ellipsisLoc,
SourceLocation colonLoc);


static CaseStmt *CreateEmpty(const ASTContext &Ctx, bool CaseStmtIsGNURange);




bool caseStmtIsGNURange() const { return SwitchCaseBits.CaseStmtIsGNURange; }

SourceLocation getCaseLoc() const { return getKeywordLoc(); }
void setCaseLoc(SourceLocation L) { setKeywordLoc(L); }


SourceLocation getEllipsisLoc() const {
return caseStmtIsGNURange() ? *getTrailingObjects<SourceLocation>()
: SourceLocation();
}



void setEllipsisLoc(SourceLocation L) {
assert(
caseStmtIsGNURange() &&
"setEllipsisLoc but this is not a case stmt of the form LHS ... RHS!");
*getTrailingObjects<SourceLocation>() = L;
}

Expr *getLHS() {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[lhsOffset()]);
}

const Expr *getLHS() const {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[lhsOffset()]);
}

void setLHS(Expr *Val) {
getTrailingObjects<Stmt *>()[lhsOffset()] = reinterpret_cast<Stmt *>(Val);
}

Expr *getRHS() {
return caseStmtIsGNURange() ? reinterpret_cast<Expr *>(
getTrailingObjects<Stmt *>()[rhsOffset()])
: nullptr;
}

const Expr *getRHS() const {
return caseStmtIsGNURange() ? reinterpret_cast<Expr *>(
getTrailingObjects<Stmt *>()[rhsOffset()])
: nullptr;
}

void setRHS(Expr *Val) {
assert(caseStmtIsGNURange() &&
"setRHS but this is not a case stmt of the form LHS ... RHS!");
getTrailingObjects<Stmt *>()[rhsOffset()] = reinterpret_cast<Stmt *>(Val);
}

Stmt *getSubStmt() { return getTrailingObjects<Stmt *>()[subStmtOffset()]; }
const Stmt *getSubStmt() const {
return getTrailingObjects<Stmt *>()[subStmtOffset()];
}

void setSubStmt(Stmt *S) {
getTrailingObjects<Stmt *>()[subStmtOffset()] = S;
}

SourceLocation getBeginLoc() const { return getKeywordLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY {

const CaseStmt *CS = this;
while (const auto *CS2 = dyn_cast<CaseStmt>(CS->getSubStmt()))
CS = CS2;

return CS->getSubStmt()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CaseStmtClass;
}


child_range children() {
return child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}

const_child_range children() const {
return const_child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}
};

class DefaultStmt : public SwitchCase {
Stmt *SubStmt;

public:
DefaultStmt(SourceLocation DL, SourceLocation CL, Stmt *substmt)
: SwitchCase(DefaultStmtClass, DL, CL), SubStmt(substmt) {}


explicit DefaultStmt(EmptyShell Empty)
: SwitchCase(DefaultStmtClass, Empty) {}

Stmt *getSubStmt() { return SubStmt; }
const Stmt *getSubStmt() const { return SubStmt; }
void setSubStmt(Stmt *S) { SubStmt = S; }

SourceLocation getDefaultLoc() const { return getKeywordLoc(); }
void setDefaultLoc(SourceLocation L) { setKeywordLoc(L); }

SourceLocation getBeginLoc() const { return getKeywordLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY {
return SubStmt->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == DefaultStmtClass;
}


child_range children() { return child_range(&SubStmt, &SubStmt + 1); }

const_child_range children() const {
return const_child_range(&SubStmt, &SubStmt + 1);
}
};

SourceLocation SwitchCase::getEndLoc() const {
if (const auto *CS = dyn_cast<CaseStmt>(this))
return CS->getEndLoc();
else if (const auto *DS = dyn_cast<DefaultStmt>(this))
return DS->getEndLoc();
llvm_unreachable("SwitchCase is neither a CaseStmt nor a DefaultStmt!");
}

Stmt *SwitchCase::getSubStmt() {
if (auto *CS = dyn_cast<CaseStmt>(this))
return CS->getSubStmt();
else if (auto *DS = dyn_cast<DefaultStmt>(this))
return DS->getSubStmt();
llvm_unreachable("SwitchCase is neither a CaseStmt nor a DefaultStmt!");
}







class ValueStmt : public Stmt {
protected:
using Stmt::Stmt;

public:
const Expr *getExprStmt() const;
Expr *getExprStmt() {
const ValueStmt *ConstThis = this;
return const_cast<Expr*>(ConstThis->getExprStmt());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() >= firstValueStmtConstant &&
T->getStmtClass() <= lastValueStmtConstant;
}
};



class LabelStmt : public ValueStmt {
LabelDecl *TheDecl;
Stmt *SubStmt;
bool SideEntry = false;

public:

LabelStmt(SourceLocation IL, LabelDecl *D, Stmt *substmt)
: ValueStmt(LabelStmtClass), TheDecl(D), SubStmt(substmt) {
setIdentLoc(IL);
}


explicit LabelStmt(EmptyShell Empty) : ValueStmt(LabelStmtClass, Empty) {}

SourceLocation getIdentLoc() const { return LabelStmtBits.IdentLoc; }
void setIdentLoc(SourceLocation L) { LabelStmtBits.IdentLoc = L; }

LabelDecl *getDecl() const { return TheDecl; }
void setDecl(LabelDecl *D) { TheDecl = D; }

const char *getName() const;
Stmt *getSubStmt() { return SubStmt; }

const Stmt *getSubStmt() const { return SubStmt; }
void setSubStmt(Stmt *SS) { SubStmt = SS; }

SourceLocation getBeginLoc() const { return getIdentLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY { return SubStmt->getEndLoc();}

child_range children() { return child_range(&SubStmt, &SubStmt + 1); }

const_child_range children() const {
return const_child_range(&SubStmt, &SubStmt + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == LabelStmtClass;
}
bool isSideEntry() const { return SideEntry; }
void setSideEntry(bool SE) { SideEntry = SE; }
};





class AttributedStmt final
: public ValueStmt,
private llvm::TrailingObjects<AttributedStmt, const Attr *> {
friend class ASTStmtReader;
friend TrailingObjects;

Stmt *SubStmt;

AttributedStmt(SourceLocation Loc, ArrayRef<const Attr *> Attrs,
Stmt *SubStmt)
: ValueStmt(AttributedStmtClass), SubStmt(SubStmt) {
AttributedStmtBits.NumAttrs = Attrs.size();
AttributedStmtBits.AttrLoc = Loc;
std::copy(Attrs.begin(), Attrs.end(), getAttrArrayPtr());
}

explicit AttributedStmt(EmptyShell Empty, unsigned NumAttrs)
: ValueStmt(AttributedStmtClass, Empty) {
AttributedStmtBits.NumAttrs = NumAttrs;
AttributedStmtBits.AttrLoc = SourceLocation{};
std::fill_n(getAttrArrayPtr(), NumAttrs, nullptr);
}

const Attr *const *getAttrArrayPtr() const {
return getTrailingObjects<const Attr *>();
}
const Attr **getAttrArrayPtr() { return getTrailingObjects<const Attr *>(); }

public:
static AttributedStmt *Create(const ASTContext &C, SourceLocation Loc,
ArrayRef<const Attr *> Attrs, Stmt *SubStmt);


static AttributedStmt *CreateEmpty(const ASTContext &C, unsigned NumAttrs);

SourceLocation getAttrLoc() const { return AttributedStmtBits.AttrLoc; }
ArrayRef<const Attr *> getAttrs() const {
return llvm::makeArrayRef(getAttrArrayPtr(), AttributedStmtBits.NumAttrs);
}

Stmt *getSubStmt() { return SubStmt; }
const Stmt *getSubStmt() const { return SubStmt; }

SourceLocation getBeginLoc() const { return getAttrLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY { return SubStmt->getEndLoc();}

child_range children() { return child_range(&SubStmt, &SubStmt + 1); }

const_child_range children() const {
return const_child_range(&SubStmt, &SubStmt + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == AttributedStmtClass;
}
};


class IfStmt final
: public Stmt,
private llvm::TrailingObjects<IfStmt, Stmt *, SourceLocation> {
friend TrailingObjects;























enum { InitOffset = 0, ThenOffsetFromCond = 1, ElseOffsetFromCond = 2 };
enum { NumMandatoryStmtPtr = 2 };
SourceLocation LParenLoc;
SourceLocation RParenLoc;

unsigned numTrailingObjects(OverloadToken<Stmt *>) const {
return NumMandatoryStmtPtr + hasElseStorage() + hasVarStorage() +
hasInitStorage();
}

unsigned numTrailingObjects(OverloadToken<SourceLocation>) const {
return hasElseStorage();
}

unsigned initOffset() const { return InitOffset; }
unsigned varOffset() const { return InitOffset + hasInitStorage(); }
unsigned condOffset() const {
return InitOffset + hasInitStorage() + hasVarStorage();
}
unsigned thenOffset() const { return condOffset() + ThenOffsetFromCond; }
unsigned elseOffset() const { return condOffset() + ElseOffsetFromCond; }


IfStmt(const ASTContext &Ctx, SourceLocation IL, bool IsConstexpr, Stmt *Init,
VarDecl *Var, Expr *Cond, SourceLocation LParenLoc,
SourceLocation RParenLoc, Stmt *Then, SourceLocation EL, Stmt *Else);


explicit IfStmt(EmptyShell Empty, bool HasElse, bool HasVar, bool HasInit);

public:

static IfStmt *Create(const ASTContext &Ctx, SourceLocation IL,
bool IsConstexpr, Stmt *Init, VarDecl *Var, Expr *Cond,
SourceLocation LPL, SourceLocation RPL, Stmt *Then,
SourceLocation EL = SourceLocation(),
Stmt *Else = nullptr);



static IfStmt *CreateEmpty(const ASTContext &Ctx, bool HasElse, bool HasVar,
bool HasInit);


bool hasInitStorage() const { return IfStmtBits.HasInit; }


bool hasVarStorage() const { return IfStmtBits.HasVar; }


bool hasElseStorage() const { return IfStmtBits.HasElse; }

Expr *getCond() {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[condOffset()]);
}

const Expr *getCond() const {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[condOffset()]);
}

void setCond(Expr *Cond) {
getTrailingObjects<Stmt *>()[condOffset()] = reinterpret_cast<Stmt *>(Cond);
}

Stmt *getThen() { return getTrailingObjects<Stmt *>()[thenOffset()]; }
const Stmt *getThen() const {
return getTrailingObjects<Stmt *>()[thenOffset()];
}

void setThen(Stmt *Then) {
getTrailingObjects<Stmt *>()[thenOffset()] = Then;
}

Stmt *getElse() {
return hasElseStorage() ? getTrailingObjects<Stmt *>()[elseOffset()]
: nullptr;
}

const Stmt *getElse() const {
return hasElseStorage() ? getTrailingObjects<Stmt *>()[elseOffset()]
: nullptr;
}

void setElse(Stmt *Else) {
assert(hasElseStorage() &&
"This if statement has no storage for an else statement!");
getTrailingObjects<Stmt *>()[elseOffset()] = Else;
}









VarDecl *getConditionVariable();
const VarDecl *getConditionVariable() const {
return const_cast<IfStmt *>(this)->getConditionVariable();
}



void setConditionVariable(const ASTContext &Ctx, VarDecl *V);



DeclStmt *getConditionVariableDeclStmt() {
return hasVarStorage() ? static_cast<DeclStmt *>(
getTrailingObjects<Stmt *>()[varOffset()])
: nullptr;
}

const DeclStmt *getConditionVariableDeclStmt() const {
return hasVarStorage() ? static_cast<DeclStmt *>(
getTrailingObjects<Stmt *>()[varOffset()])
: nullptr;
}

Stmt *getInit() {
return hasInitStorage() ? getTrailingObjects<Stmt *>()[initOffset()]
: nullptr;
}

const Stmt *getInit() const {
return hasInitStorage() ? getTrailingObjects<Stmt *>()[initOffset()]
: nullptr;
}

void setInit(Stmt *Init) {
assert(hasInitStorage() &&
"This if statement has no storage for an init statement!");
getTrailingObjects<Stmt *>()[initOffset()] = Init;
}

SourceLocation getIfLoc() const { return IfStmtBits.IfLoc; }
void setIfLoc(SourceLocation IfLoc) { IfStmtBits.IfLoc = IfLoc; }

SourceLocation getElseLoc() const {
return hasElseStorage() ? *getTrailingObjects<SourceLocation>()
: SourceLocation();
}

void setElseLoc(SourceLocation ElseLoc) {
assert(hasElseStorage() &&
"This if statement has no storage for an else statement!");
*getTrailingObjects<SourceLocation>() = ElseLoc;
}

bool isConstexpr() const { return IfStmtBits.IsConstexpr; }
void setConstexpr(bool C) { IfStmtBits.IsConstexpr = C; }



Optional<const Stmt*> getNondiscardedCase(const ASTContext &Ctx) const;
Optional<Stmt *> getNondiscardedCase(const ASTContext &Ctx);

bool isObjCAvailabilityCheck() const;

SourceLocation getBeginLoc() const { return getIfLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY {
if (getElse())
return getElse()->getEndLoc();
return getThen()->getEndLoc();
}
SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation Loc) { RParenLoc = Loc; }



child_range children() {
return child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}

const_child_range children() const {
return const_child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == IfStmtClass;
}
};


class SwitchStmt final : public Stmt,
private llvm::TrailingObjects<SwitchStmt, Stmt *> {
friend TrailingObjects;


SwitchCase *FirstCase = nullptr;


















enum { InitOffset = 0, BodyOffsetFromCond = 1 };
enum { NumMandatoryStmtPtr = 2 };
SourceLocation LParenLoc;
SourceLocation RParenLoc;

unsigned numTrailingObjects(OverloadToken<Stmt *>) const {
return NumMandatoryStmtPtr + hasInitStorage() + hasVarStorage();
}

unsigned initOffset() const { return InitOffset; }
unsigned varOffset() const { return InitOffset + hasInitStorage(); }
unsigned condOffset() const {
return InitOffset + hasInitStorage() + hasVarStorage();
}
unsigned bodyOffset() const { return condOffset() + BodyOffsetFromCond; }


SwitchStmt(const ASTContext &Ctx, Stmt *Init, VarDecl *Var, Expr *Cond,
SourceLocation LParenLoc, SourceLocation RParenLoc);


explicit SwitchStmt(EmptyShell Empty, bool HasInit, bool HasVar);

public:

static SwitchStmt *Create(const ASTContext &Ctx, Stmt *Init, VarDecl *Var,
Expr *Cond, SourceLocation LParenLoc,
SourceLocation RParenLoc);



static SwitchStmt *CreateEmpty(const ASTContext &Ctx, bool HasInit,
bool HasVar);


bool hasInitStorage() const { return SwitchStmtBits.HasInit; }


bool hasVarStorage() const { return SwitchStmtBits.HasVar; }

Expr *getCond() {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[condOffset()]);
}

const Expr *getCond() const {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[condOffset()]);
}

void setCond(Expr *Cond) {
getTrailingObjects<Stmt *>()[condOffset()] = reinterpret_cast<Stmt *>(Cond);
}

Stmt *getBody() { return getTrailingObjects<Stmt *>()[bodyOffset()]; }
const Stmt *getBody() const {
return getTrailingObjects<Stmt *>()[bodyOffset()];
}

void setBody(Stmt *Body) {
getTrailingObjects<Stmt *>()[bodyOffset()] = Body;
}

Stmt *getInit() {
return hasInitStorage() ? getTrailingObjects<Stmt *>()[initOffset()]
: nullptr;
}

const Stmt *getInit() const {
return hasInitStorage() ? getTrailingObjects<Stmt *>()[initOffset()]
: nullptr;
}

void setInit(Stmt *Init) {
assert(hasInitStorage() &&
"This switch statement has no storage for an init statement!");
getTrailingObjects<Stmt *>()[initOffset()] = Init;
}










VarDecl *getConditionVariable();
const VarDecl *getConditionVariable() const {
return const_cast<SwitchStmt *>(this)->getConditionVariable();
}



void setConditionVariable(const ASTContext &Ctx, VarDecl *VD);



DeclStmt *getConditionVariableDeclStmt() {
return hasVarStorage() ? static_cast<DeclStmt *>(
getTrailingObjects<Stmt *>()[varOffset()])
: nullptr;
}

const DeclStmt *getConditionVariableDeclStmt() const {
return hasVarStorage() ? static_cast<DeclStmt *>(
getTrailingObjects<Stmt *>()[varOffset()])
: nullptr;
}

SwitchCase *getSwitchCaseList() { return FirstCase; }
const SwitchCase *getSwitchCaseList() const { return FirstCase; }
void setSwitchCaseList(SwitchCase *SC) { FirstCase = SC; }

SourceLocation getSwitchLoc() const { return SwitchStmtBits.SwitchLoc; }
void setSwitchLoc(SourceLocation L) { SwitchStmtBits.SwitchLoc = L; }
SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation Loc) { RParenLoc = Loc; }

void setBody(Stmt *S, SourceLocation SL) {
setBody(S);
setSwitchLoc(SL);
}

void addSwitchCase(SwitchCase *SC) {
assert(!SC->getNextSwitchCase() &&
"case/default already added to a switch");
SC->setNextSwitchCase(FirstCase);
FirstCase = SC;
}



void setAllEnumCasesCovered() { SwitchStmtBits.AllEnumCasesCovered = true; }



bool isAllEnumCasesCovered() const {
return SwitchStmtBits.AllEnumCasesCovered;
}

SourceLocation getBeginLoc() const { return getSwitchLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY {
return getBody() ? getBody()->getEndLoc()
: reinterpret_cast<const Stmt *>(getCond())->getEndLoc();
}


child_range children() {
return child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}

const_child_range children() const {
return const_child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == SwitchStmtClass;
}
};


class WhileStmt final : public Stmt,
private llvm::TrailingObjects<WhileStmt, Stmt *> {
friend TrailingObjects;
















enum { VarOffset = 0, BodyOffsetFromCond = 1 };
enum { NumMandatoryStmtPtr = 2 };

SourceLocation LParenLoc, RParenLoc;

unsigned varOffset() const { return VarOffset; }
unsigned condOffset() const { return VarOffset + hasVarStorage(); }
unsigned bodyOffset() const { return condOffset() + BodyOffsetFromCond; }

unsigned numTrailingObjects(OverloadToken<Stmt *>) const {
return NumMandatoryStmtPtr + hasVarStorage();
}


WhileStmt(const ASTContext &Ctx, VarDecl *Var, Expr *Cond, Stmt *Body,
SourceLocation WL, SourceLocation LParenLoc,
SourceLocation RParenLoc);


explicit WhileStmt(EmptyShell Empty, bool HasVar);

public:

static WhileStmt *Create(const ASTContext &Ctx, VarDecl *Var, Expr *Cond,
Stmt *Body, SourceLocation WL,
SourceLocation LParenLoc, SourceLocation RParenLoc);



static WhileStmt *CreateEmpty(const ASTContext &Ctx, bool HasVar);


bool hasVarStorage() const { return WhileStmtBits.HasVar; }

Expr *getCond() {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[condOffset()]);
}

const Expr *getCond() const {
return reinterpret_cast<Expr *>(getTrailingObjects<Stmt *>()[condOffset()]);
}

void setCond(Expr *Cond) {
getTrailingObjects<Stmt *>()[condOffset()] = reinterpret_cast<Stmt *>(Cond);
}

Stmt *getBody() { return getTrailingObjects<Stmt *>()[bodyOffset()]; }
const Stmt *getBody() const {
return getTrailingObjects<Stmt *>()[bodyOffset()];
}

void setBody(Stmt *Body) {
getTrailingObjects<Stmt *>()[bodyOffset()] = Body;
}









VarDecl *getConditionVariable();
const VarDecl *getConditionVariable() const {
return const_cast<WhileStmt *>(this)->getConditionVariable();
}



void setConditionVariable(const ASTContext &Ctx, VarDecl *V);



DeclStmt *getConditionVariableDeclStmt() {
return hasVarStorage() ? static_cast<DeclStmt *>(
getTrailingObjects<Stmt *>()[varOffset()])
: nullptr;
}

const DeclStmt *getConditionVariableDeclStmt() const {
return hasVarStorage() ? static_cast<DeclStmt *>(
getTrailingObjects<Stmt *>()[varOffset()])
: nullptr;
}

SourceLocation getWhileLoc() const { return WhileStmtBits.WhileLoc; }
void setWhileLoc(SourceLocation L) { WhileStmtBits.WhileLoc = L; }

SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation L) { LParenLoc = L; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const { return getWhileLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY {
return getBody()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == WhileStmtClass;
}


child_range children() {
return child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}

const_child_range children() const {
return const_child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}
};


class DoStmt : public Stmt {
enum { BODY, COND, END_EXPR };
Stmt *SubExprs[END_EXPR];
SourceLocation WhileLoc;
SourceLocation RParenLoc;

public:
DoStmt(Stmt *Body, Expr *Cond, SourceLocation DL, SourceLocation WL,
SourceLocation RP)
: Stmt(DoStmtClass), WhileLoc(WL), RParenLoc(RP) {
setCond(Cond);
setBody(Body);
setDoLoc(DL);
}


explicit DoStmt(EmptyShell Empty) : Stmt(DoStmtClass, Empty) {}

Expr *getCond() { return reinterpret_cast<Expr *>(SubExprs[COND]); }
const Expr *getCond() const {
return reinterpret_cast<Expr *>(SubExprs[COND]);
}

void setCond(Expr *Cond) { SubExprs[COND] = reinterpret_cast<Stmt *>(Cond); }

Stmt *getBody() { return SubExprs[BODY]; }
const Stmt *getBody() const { return SubExprs[BODY]; }
void setBody(Stmt *Body) { SubExprs[BODY] = Body; }

SourceLocation getDoLoc() const { return DoStmtBits.DoLoc; }
void setDoLoc(SourceLocation L) { DoStmtBits.DoLoc = L; }
SourceLocation getWhileLoc() const { return WhileLoc; }
void setWhileLoc(SourceLocation L) { WhileLoc = L; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const { return getDoLoc(); }
SourceLocation getEndLoc() const { return getRParenLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == DoStmtClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}

const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}
};




class ForStmt : public Stmt {
enum { INIT, CONDVAR, COND, INC, BODY, END_EXPR };
Stmt* SubExprs[END_EXPR];
SourceLocation LParenLoc, RParenLoc;

public:
ForStmt(const ASTContext &C, Stmt *Init, Expr *Cond, VarDecl *condVar,
Expr *Inc, Stmt *Body, SourceLocation FL, SourceLocation LP,
SourceLocation RP);


explicit ForStmt(EmptyShell Empty) : Stmt(ForStmtClass, Empty) {}

Stmt *getInit() { return SubExprs[INIT]; }









VarDecl *getConditionVariable() const;
void setConditionVariable(const ASTContext &C, VarDecl *V);



const DeclStmt *getConditionVariableDeclStmt() const {
return reinterpret_cast<DeclStmt*>(SubExprs[CONDVAR]);
}

Expr *getCond() { return reinterpret_cast<Expr*>(SubExprs[COND]); }
Expr *getInc() { return reinterpret_cast<Expr*>(SubExprs[INC]); }
Stmt *getBody() { return SubExprs[BODY]; }

const Stmt *getInit() const { return SubExprs[INIT]; }
const Expr *getCond() const { return reinterpret_cast<Expr*>(SubExprs[COND]);}
const Expr *getInc() const { return reinterpret_cast<Expr*>(SubExprs[INC]); }
const Stmt *getBody() const { return SubExprs[BODY]; }

void setInit(Stmt *S) { SubExprs[INIT] = S; }
void setCond(Expr *E) { SubExprs[COND] = reinterpret_cast<Stmt*>(E); }
void setInc(Expr *E) { SubExprs[INC] = reinterpret_cast<Stmt*>(E); }
void setBody(Stmt *S) { SubExprs[BODY] = S; }

SourceLocation getForLoc() const { return ForStmtBits.ForLoc; }
void setForLoc(SourceLocation L) { ForStmtBits.ForLoc = L; }
SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation L) { LParenLoc = L; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const { return getForLoc(); }
SourceLocation getEndLoc() const { return getBody()->getEndLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ForStmtClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0]+END_EXPR);
}

const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}
};


class GotoStmt : public Stmt {
LabelDecl *Label;
SourceLocation LabelLoc;

public:
GotoStmt(LabelDecl *label, SourceLocation GL, SourceLocation LL)
: Stmt(GotoStmtClass), Label(label), LabelLoc(LL) {
setGotoLoc(GL);
}


explicit GotoStmt(EmptyShell Empty) : Stmt(GotoStmtClass, Empty) {}

LabelDecl *getLabel() const { return Label; }
void setLabel(LabelDecl *D) { Label = D; }

SourceLocation getGotoLoc() const { return GotoStmtBits.GotoLoc; }
void setGotoLoc(SourceLocation L) { GotoStmtBits.GotoLoc = L; }
SourceLocation getLabelLoc() const { return LabelLoc; }
void setLabelLoc(SourceLocation L) { LabelLoc = L; }

SourceLocation getBeginLoc() const { return getGotoLoc(); }
SourceLocation getEndLoc() const { return getLabelLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == GotoStmtClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};


class IndirectGotoStmt : public Stmt {
SourceLocation StarLoc;
Stmt *Target;

public:
IndirectGotoStmt(SourceLocation gotoLoc, SourceLocation starLoc, Expr *target)
: Stmt(IndirectGotoStmtClass), StarLoc(starLoc) {
setTarget(target);
setGotoLoc(gotoLoc);
}


explicit IndirectGotoStmt(EmptyShell Empty)
: Stmt(IndirectGotoStmtClass, Empty) {}

void setGotoLoc(SourceLocation L) { GotoStmtBits.GotoLoc = L; }
SourceLocation getGotoLoc() const { return GotoStmtBits.GotoLoc; }
void setStarLoc(SourceLocation L) { StarLoc = L; }
SourceLocation getStarLoc() const { return StarLoc; }

Expr *getTarget() { return reinterpret_cast<Expr *>(Target); }
const Expr *getTarget() const {
return reinterpret_cast<const Expr *>(Target);
}
void setTarget(Expr *E) { Target = reinterpret_cast<Stmt *>(E); }



LabelDecl *getConstantTarget();
const LabelDecl *getConstantTarget() const {
return const_cast<IndirectGotoStmt *>(this)->getConstantTarget();
}

SourceLocation getBeginLoc() const { return getGotoLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Target->getEndLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == IndirectGotoStmtClass;
}


child_range children() { return child_range(&Target, &Target + 1); }

const_child_range children() const {
return const_child_range(&Target, &Target + 1);
}
};


class ContinueStmt : public Stmt {
public:
ContinueStmt(SourceLocation CL) : Stmt(ContinueStmtClass) {
setContinueLoc(CL);
}


explicit ContinueStmt(EmptyShell Empty) : Stmt(ContinueStmtClass, Empty) {}

SourceLocation getContinueLoc() const { return ContinueStmtBits.ContinueLoc; }
void setContinueLoc(SourceLocation L) { ContinueStmtBits.ContinueLoc = L; }

SourceLocation getBeginLoc() const { return getContinueLoc(); }
SourceLocation getEndLoc() const { return getContinueLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ContinueStmtClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};


class BreakStmt : public Stmt {
public:
BreakStmt(SourceLocation BL) : Stmt(BreakStmtClass) {
setBreakLoc(BL);
}


explicit BreakStmt(EmptyShell Empty) : Stmt(BreakStmtClass, Empty) {}

SourceLocation getBreakLoc() const { return BreakStmtBits.BreakLoc; }
void setBreakLoc(SourceLocation L) { BreakStmtBits.BreakLoc = L; }

SourceLocation getBeginLoc() const { return getBreakLoc(); }
SourceLocation getEndLoc() const { return getBreakLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == BreakStmtClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};









class ReturnStmt final
: public Stmt,
private llvm::TrailingObjects<ReturnStmt, const VarDecl *> {
friend TrailingObjects;


Stmt *RetExpr;





bool hasNRVOCandidate() const { return ReturnStmtBits.HasNRVOCandidate; }

unsigned numTrailingObjects(OverloadToken<const VarDecl *>) const {
return hasNRVOCandidate();
}


ReturnStmt(SourceLocation RL, Expr *E, const VarDecl *NRVOCandidate);


explicit ReturnStmt(EmptyShell Empty, bool HasNRVOCandidate);

public:

static ReturnStmt *Create(const ASTContext &Ctx, SourceLocation RL, Expr *E,
const VarDecl *NRVOCandidate);



static ReturnStmt *CreateEmpty(const ASTContext &Ctx, bool HasNRVOCandidate);

Expr *getRetValue() { return reinterpret_cast<Expr *>(RetExpr); }
const Expr *getRetValue() const { return reinterpret_cast<Expr *>(RetExpr); }
void setRetValue(Expr *E) { RetExpr = reinterpret_cast<Stmt *>(E); }






const VarDecl *getNRVOCandidate() const {
return hasNRVOCandidate() ? *getTrailingObjects<const VarDecl *>()
: nullptr;
}




void setNRVOCandidate(const VarDecl *Var) {
assert(hasNRVOCandidate() &&
"This return statement has no storage for an NRVO candidate!");
*getTrailingObjects<const VarDecl *>() = Var;
}

SourceLocation getReturnLoc() const { return ReturnStmtBits.RetLoc; }
void setReturnLoc(SourceLocation L) { ReturnStmtBits.RetLoc = L; }

SourceLocation getBeginLoc() const { return getReturnLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY {
return RetExpr ? RetExpr->getEndLoc() : getReturnLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ReturnStmtClass;
}


child_range children() {
if (RetExpr)
return child_range(&RetExpr, &RetExpr + 1);
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
if (RetExpr)
return const_child_range(&RetExpr, &RetExpr + 1);
return const_child_range(const_child_iterator(), const_child_iterator());
}
};


class AsmStmt : public Stmt {
protected:
friend class ASTStmtReader;

SourceLocation AsmLoc;



bool IsSimple;



bool IsVolatile;

unsigned NumOutputs;
unsigned NumInputs;
unsigned NumClobbers;

Stmt **Exprs = nullptr;

AsmStmt(StmtClass SC, SourceLocation asmloc, bool issimple, bool isvolatile,
unsigned numoutputs, unsigned numinputs, unsigned numclobbers)
: Stmt (SC), AsmLoc(asmloc), IsSimple(issimple), IsVolatile(isvolatile),
NumOutputs(numoutputs), NumInputs(numinputs),
NumClobbers(numclobbers) {}

public:

explicit AsmStmt(StmtClass SC, EmptyShell Empty) : Stmt(SC, Empty) {}

SourceLocation getAsmLoc() const { return AsmLoc; }
void setAsmLoc(SourceLocation L) { AsmLoc = L; }

bool isSimple() const { return IsSimple; }
void setSimple(bool V) { IsSimple = V; }

bool isVolatile() const { return IsVolatile; }
void setVolatile(bool V) { IsVolatile = V; }

SourceLocation getBeginLoc() const LLVM_READONLY { return {}; }
SourceLocation getEndLoc() const LLVM_READONLY { return {}; }




std::string generateAsmString(const ASTContext &C) const;



unsigned getNumOutputs() const { return NumOutputs; }




StringRef getOutputConstraint(unsigned i) const;




bool isOutputPlusConstraint(unsigned i) const {
return getOutputConstraint(i)[0] == '+';
}

const Expr *getOutputExpr(unsigned i) const;



unsigned getNumPlusOperands() const;



unsigned getNumInputs() const { return NumInputs; }



StringRef getInputConstraint(unsigned i) const;

const Expr *getInputExpr(unsigned i) const;



unsigned getNumClobbers() const { return NumClobbers; }
StringRef getClobber(unsigned i) const;

static bool classof(const Stmt *T) {
return T->getStmtClass() == GCCAsmStmtClass ||
T->getStmtClass() == MSAsmStmtClass;
}



using inputs_iterator = ExprIterator;
using const_inputs_iterator = ConstExprIterator;
using inputs_range = llvm::iterator_range<inputs_iterator>;
using inputs_const_range = llvm::iterator_range<const_inputs_iterator>;

inputs_iterator begin_inputs() {
return &Exprs[0] + NumOutputs;
}

inputs_iterator end_inputs() {
return &Exprs[0] + NumOutputs + NumInputs;
}

inputs_range inputs() { return inputs_range(begin_inputs(), end_inputs()); }

const_inputs_iterator begin_inputs() const {
return &Exprs[0] + NumOutputs;
}

const_inputs_iterator end_inputs() const {
return &Exprs[0] + NumOutputs + NumInputs;
}

inputs_const_range inputs() const {
return inputs_const_range(begin_inputs(), end_inputs());
}



using outputs_iterator = ExprIterator;
using const_outputs_iterator = ConstExprIterator;
using outputs_range = llvm::iterator_range<outputs_iterator>;
using outputs_const_range = llvm::iterator_range<const_outputs_iterator>;

outputs_iterator begin_outputs() {
return &Exprs[0];
}

outputs_iterator end_outputs() {
return &Exprs[0] + NumOutputs;
}

outputs_range outputs() {
return outputs_range(begin_outputs(), end_outputs());
}

const_outputs_iterator begin_outputs() const {
return &Exprs[0];
}

const_outputs_iterator end_outputs() const {
return &Exprs[0] + NumOutputs;
}

outputs_const_range outputs() const {
return outputs_const_range(begin_outputs(), end_outputs());
}

child_range children() {
return child_range(&Exprs[0], &Exprs[0] + NumOutputs + NumInputs);
}

const_child_range children() const {
return const_child_range(&Exprs[0], &Exprs[0] + NumOutputs + NumInputs);
}
};


class GCCAsmStmt : public AsmStmt {
friend class ASTStmtReader;

SourceLocation RParenLoc;
StringLiteral *AsmStr;


StringLiteral **Constraints = nullptr;
StringLiteral **Clobbers = nullptr;
IdentifierInfo **Names = nullptr;
unsigned NumLabels = 0;

public:
GCCAsmStmt(const ASTContext &C, SourceLocation asmloc, bool issimple,
bool isvolatile, unsigned numoutputs, unsigned numinputs,
IdentifierInfo **names, StringLiteral **constraints, Expr **exprs,
StringLiteral *asmstr, unsigned numclobbers,
StringLiteral **clobbers, unsigned numlabels,
SourceLocation rparenloc);


explicit GCCAsmStmt(EmptyShell Empty) : AsmStmt(GCCAsmStmtClass, Empty) {}

SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }



const StringLiteral *getAsmString() const { return AsmStr; }
StringLiteral *getAsmString() { return AsmStr; }
void setAsmString(StringLiteral *E) { AsmStr = E; }




class AsmStringPiece {
public:
enum Kind {
String,
Operand
};

private:
Kind MyKind;
std::string Str;
unsigned OperandNo;


CharSourceRange Range;

public:
AsmStringPiece(const std::string &S) : MyKind(String), Str(S) {}
AsmStringPiece(unsigned OpNo, const std::string &S, SourceLocation Begin,
SourceLocation End)
: MyKind(Operand), Str(S), OperandNo(OpNo),
Range(CharSourceRange::getCharRange(Begin, End)) {}

bool isString() const { return MyKind == String; }
bool isOperand() const { return MyKind == Operand; }

const std::string &getString() const { return Str; }

unsigned getOperandNo() const {
assert(isOperand());
return OperandNo;
}

CharSourceRange getRange() const {
assert(isOperand() && "Range is currently used only for Operands.");
return Range;
}



char getModifier() const;
};






unsigned AnalyzeAsmString(SmallVectorImpl<AsmStringPiece> &Pieces,
const ASTContext &C, unsigned &DiagOffs) const;


std::string generateAsmString(const ASTContext &C) const;



IdentifierInfo *getOutputIdentifier(unsigned i) const { return Names[i]; }

StringRef getOutputName(unsigned i) const {
if (IdentifierInfo *II = getOutputIdentifier(i))
return II->getName();

return {};
}

StringRef getOutputConstraint(unsigned i) const;

const StringLiteral *getOutputConstraintLiteral(unsigned i) const {
return Constraints[i];
}
StringLiteral *getOutputConstraintLiteral(unsigned i) {
return Constraints[i];
}

Expr *getOutputExpr(unsigned i);

const Expr *getOutputExpr(unsigned i) const {
return const_cast<GCCAsmStmt*>(this)->getOutputExpr(i);
}



IdentifierInfo *getInputIdentifier(unsigned i) const {
return Names[i + NumOutputs];
}

StringRef getInputName(unsigned i) const {
if (IdentifierInfo *II = getInputIdentifier(i))
return II->getName();

return {};
}

StringRef getInputConstraint(unsigned i) const;

const StringLiteral *getInputConstraintLiteral(unsigned i) const {
return Constraints[i + NumOutputs];
}
StringLiteral *getInputConstraintLiteral(unsigned i) {
return Constraints[i + NumOutputs];
}

Expr *getInputExpr(unsigned i);
void setInputExpr(unsigned i, Expr *E);

const Expr *getInputExpr(unsigned i) const {
return const_cast<GCCAsmStmt*>(this)->getInputExpr(i);
}



bool isAsmGoto() const {
return NumLabels > 0;
}

unsigned getNumLabels() const {
return NumLabels;
}

IdentifierInfo *getLabelIdentifier(unsigned i) const {
return Names[i + NumOutputs + NumInputs];
}

AddrLabelExpr *getLabelExpr(unsigned i) const;
StringRef getLabelName(unsigned i) const;
using labels_iterator = CastIterator<AddrLabelExpr>;
using const_labels_iterator = ConstCastIterator<AddrLabelExpr>;
using labels_range = llvm::iterator_range<labels_iterator>;
using labels_const_range = llvm::iterator_range<const_labels_iterator>;

labels_iterator begin_labels() {
return &Exprs[0] + NumOutputs + NumInputs;
}

labels_iterator end_labels() {
return &Exprs[0] + NumOutputs + NumInputs + NumLabels;
}

labels_range labels() {
return labels_range(begin_labels(), end_labels());
}

const_labels_iterator begin_labels() const {
return &Exprs[0] + NumOutputs + NumInputs;
}

const_labels_iterator end_labels() const {
return &Exprs[0] + NumOutputs + NumInputs + NumLabels;
}

labels_const_range labels() const {
return labels_const_range(begin_labels(), end_labels());
}

private:
void setOutputsAndInputsAndClobbers(const ASTContext &C,
IdentifierInfo **Names,
StringLiteral **Constraints,
Stmt **Exprs,
unsigned NumOutputs,
unsigned NumInputs,
unsigned NumLabels,
StringLiteral **Clobbers,
unsigned NumClobbers);

public:





int getNamedOperand(StringRef SymbolicName) const;

StringRef getClobber(unsigned i) const;

StringLiteral *getClobberStringLiteral(unsigned i) { return Clobbers[i]; }
const StringLiteral *getClobberStringLiteral(unsigned i) const {
return Clobbers[i];
}

SourceLocation getBeginLoc() const LLVM_READONLY { return AsmLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == GCCAsmStmtClass;
}
};


class MSAsmStmt : public AsmStmt {
friend class ASTStmtReader;

SourceLocation LBraceLoc, EndLoc;
StringRef AsmStr;

unsigned NumAsmToks = 0;

Token *AsmToks = nullptr;
StringRef *Constraints = nullptr;
StringRef *Clobbers = nullptr;

public:
MSAsmStmt(const ASTContext &C, SourceLocation asmloc,
SourceLocation lbraceloc, bool issimple, bool isvolatile,
ArrayRef<Token> asmtoks, unsigned numoutputs, unsigned numinputs,
ArrayRef<StringRef> constraints,
ArrayRef<Expr*> exprs, StringRef asmstr,
ArrayRef<StringRef> clobbers, SourceLocation endloc);


explicit MSAsmStmt(EmptyShell Empty) : AsmStmt(MSAsmStmtClass, Empty) {}

SourceLocation getLBraceLoc() const { return LBraceLoc; }
void setLBraceLoc(SourceLocation L) { LBraceLoc = L; }
SourceLocation getEndLoc() const { return EndLoc; }
void setEndLoc(SourceLocation L) { EndLoc = L; }

bool hasBraces() const { return LBraceLoc.isValid(); }

unsigned getNumAsmToks() { return NumAsmToks; }
Token *getAsmToks() { return AsmToks; }


StringRef getAsmString() const { return AsmStr; }


std::string generateAsmString(const ASTContext &C) const;



StringRef getOutputConstraint(unsigned i) const {
assert(i < NumOutputs);
return Constraints[i];
}

Expr *getOutputExpr(unsigned i);

const Expr *getOutputExpr(unsigned i) const {
return const_cast<MSAsmStmt*>(this)->getOutputExpr(i);
}



StringRef getInputConstraint(unsigned i) const {
assert(i < NumInputs);
return Constraints[i + NumOutputs];
}

Expr *getInputExpr(unsigned i);
void setInputExpr(unsigned i, Expr *E);

const Expr *getInputExpr(unsigned i) const {
return const_cast<MSAsmStmt*>(this)->getInputExpr(i);
}



ArrayRef<StringRef> getAllConstraints() const {
return llvm::makeArrayRef(Constraints, NumInputs + NumOutputs);
}

ArrayRef<StringRef> getClobbers() const {
return llvm::makeArrayRef(Clobbers, NumClobbers);
}

ArrayRef<Expr*> getAllExprs() const {
return llvm::makeArrayRef(reinterpret_cast<Expr**>(Exprs),
NumInputs + NumOutputs);
}

StringRef getClobber(unsigned i) const { return getClobbers()[i]; }

private:
void initialize(const ASTContext &C, StringRef AsmString,
ArrayRef<Token> AsmToks, ArrayRef<StringRef> Constraints,
ArrayRef<Expr*> Exprs, ArrayRef<StringRef> Clobbers);

public:
SourceLocation getBeginLoc() const LLVM_READONLY { return AsmLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == MSAsmStmtClass;
}

child_range children() {
return child_range(&Exprs[0], &Exprs[NumInputs + NumOutputs]);
}

const_child_range children() const {
return const_child_range(&Exprs[0], &Exprs[NumInputs + NumOutputs]);
}
};

class SEHExceptStmt : public Stmt {
friend class ASTReader;
friend class ASTStmtReader;

SourceLocation Loc;
Stmt *Children[2];

enum { FILTER_EXPR, BLOCK };

SEHExceptStmt(SourceLocation Loc, Expr *FilterExpr, Stmt *Block);
explicit SEHExceptStmt(EmptyShell E) : Stmt(SEHExceptStmtClass, E) {}

public:
static SEHExceptStmt* Create(const ASTContext &C,
SourceLocation ExceptLoc,
Expr *FilterExpr,
Stmt *Block);

SourceLocation getBeginLoc() const LLVM_READONLY { return getExceptLoc(); }

SourceLocation getExceptLoc() const { return Loc; }
SourceLocation getEndLoc() const { return getBlock()->getEndLoc(); }

Expr *getFilterExpr() const {
return reinterpret_cast<Expr*>(Children[FILTER_EXPR]);
}

CompoundStmt *getBlock() const {
return cast<CompoundStmt>(Children[BLOCK]);
}

child_range children() {
return child_range(Children, Children+2);
}

const_child_range children() const {
return const_child_range(Children, Children + 2);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == SEHExceptStmtClass;
}
};

class SEHFinallyStmt : public Stmt {
friend class ASTReader;
friend class ASTStmtReader;

SourceLocation Loc;
Stmt *Block;

SEHFinallyStmt(SourceLocation Loc, Stmt *Block);
explicit SEHFinallyStmt(EmptyShell E) : Stmt(SEHFinallyStmtClass, E) {}

public:
static SEHFinallyStmt* Create(const ASTContext &C,
SourceLocation FinallyLoc,
Stmt *Block);

SourceLocation getBeginLoc() const LLVM_READONLY { return getFinallyLoc(); }

SourceLocation getFinallyLoc() const { return Loc; }
SourceLocation getEndLoc() const { return Block->getEndLoc(); }

CompoundStmt *getBlock() const { return cast<CompoundStmt>(Block); }

child_range children() {
return child_range(&Block,&Block+1);
}

const_child_range children() const {
return const_child_range(&Block, &Block + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == SEHFinallyStmtClass;
}
};

class SEHTryStmt : public Stmt {
friend class ASTReader;
friend class ASTStmtReader;

bool IsCXXTry;
SourceLocation TryLoc;
Stmt *Children[2];

enum { TRY = 0, HANDLER = 1 };

SEHTryStmt(bool isCXXTry,
SourceLocation TryLoc,
Stmt *TryBlock,
Stmt *Handler);

explicit SEHTryStmt(EmptyShell E) : Stmt(SEHTryStmtClass, E) {}

public:
static SEHTryStmt* Create(const ASTContext &C, bool isCXXTry,
SourceLocation TryLoc, Stmt *TryBlock,
Stmt *Handler);

SourceLocation getBeginLoc() const LLVM_READONLY { return getTryLoc(); }

SourceLocation getTryLoc() const { return TryLoc; }
SourceLocation getEndLoc() const { return Children[HANDLER]->getEndLoc(); }

bool getIsCXXTry() const { return IsCXXTry; }

CompoundStmt* getTryBlock() const {
return cast<CompoundStmt>(Children[TRY]);
}

Stmt *getHandler() const { return Children[HANDLER]; }


SEHExceptStmt *getExceptHandler() const;
SEHFinallyStmt *getFinallyHandler() const;

child_range children() {
return child_range(Children, Children+2);
}

const_child_range children() const {
return const_child_range(Children, Children + 2);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == SEHTryStmtClass;
}
};


class SEHLeaveStmt : public Stmt {
SourceLocation LeaveLoc;

public:
explicit SEHLeaveStmt(SourceLocation LL)
: Stmt(SEHLeaveStmtClass), LeaveLoc(LL) {}


explicit SEHLeaveStmt(EmptyShell Empty) : Stmt(SEHLeaveStmtClass, Empty) {}

SourceLocation getLeaveLoc() const { return LeaveLoc; }
void setLeaveLoc(SourceLocation L) { LeaveLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return LeaveLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return LeaveLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == SEHLeaveStmtClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};










class CapturedStmt : public Stmt {
public:


enum VariableCaptureKind {
VCK_This,
VCK_ByRef,
VCK_ByCopy,
VCK_VLAType,
};



class Capture {
llvm::PointerIntPair<VarDecl *, 2, VariableCaptureKind> VarAndKind;
SourceLocation Loc;

public:
friend class ASTStmtReader;








Capture(SourceLocation Loc, VariableCaptureKind Kind,
VarDecl *Var = nullptr);


VariableCaptureKind getCaptureKind() const;



SourceLocation getLocation() const { return Loc; }


bool capturesThis() const { return getCaptureKind() == VCK_This; }


bool capturesVariable() const { return getCaptureKind() == VCK_ByRef; }


bool capturesVariableByCopy() const {
return getCaptureKind() == VCK_ByCopy;
}



bool capturesVariableArrayType() const {
return getCaptureKind() == VCK_VLAType;
}




VarDecl *getCapturedVar() const;
};

private:

unsigned NumCaptures;



llvm::PointerIntPair<CapturedDecl *, 2, CapturedRegionKind> CapDeclAndKind;


RecordDecl *TheRecordDecl = nullptr;


CapturedStmt(Stmt *S, CapturedRegionKind Kind, ArrayRef<Capture> Captures,
ArrayRef<Expr *> CaptureInits, CapturedDecl *CD, RecordDecl *RD);


CapturedStmt(EmptyShell Empty, unsigned NumCaptures);

Stmt **getStoredStmts() { return reinterpret_cast<Stmt **>(this + 1); }

Stmt *const *getStoredStmts() const {
return reinterpret_cast<Stmt *const *>(this + 1);
}

Capture *getStoredCaptures() const;

void setCapturedStmt(Stmt *S) { getStoredStmts()[NumCaptures] = S; }

public:
friend class ASTStmtReader;

static CapturedStmt *Create(const ASTContext &Context, Stmt *S,
CapturedRegionKind Kind,
ArrayRef<Capture> Captures,
ArrayRef<Expr *> CaptureInits,
CapturedDecl *CD, RecordDecl *RD);

static CapturedStmt *CreateDeserialized(const ASTContext &Context,
unsigned NumCaptures);


Stmt *getCapturedStmt() { return getStoredStmts()[NumCaptures]; }
const Stmt *getCapturedStmt() const { return getStoredStmts()[NumCaptures]; }


CapturedDecl *getCapturedDecl();
const CapturedDecl *getCapturedDecl() const;


void setCapturedDecl(CapturedDecl *D);


CapturedRegionKind getCapturedRegionKind() const;


void setCapturedRegionKind(CapturedRegionKind Kind);


const RecordDecl *getCapturedRecordDecl() const { return TheRecordDecl; }


void setCapturedRecordDecl(RecordDecl *D) {
assert(D && "null RecordDecl");
TheRecordDecl = D;
}


bool capturesVariable(const VarDecl *Var) const;


using capture_iterator = Capture *;
using const_capture_iterator = const Capture *;
using capture_range = llvm::iterator_range<capture_iterator>;
using capture_const_range = llvm::iterator_range<const_capture_iterator>;

capture_range captures() {
return capture_range(capture_begin(), capture_end());
}
capture_const_range captures() const {
return capture_const_range(capture_begin(), capture_end());
}


capture_iterator capture_begin() { return getStoredCaptures(); }
const_capture_iterator capture_begin() const { return getStoredCaptures(); }



capture_iterator capture_end() const {
return getStoredCaptures() + NumCaptures;
}


unsigned capture_size() const { return NumCaptures; }


using capture_init_iterator = Expr **;
using capture_init_range = llvm::iterator_range<capture_init_iterator>;



using const_capture_init_iterator = Expr *const *;
using const_capture_init_range =
llvm::iterator_range<const_capture_init_iterator>;

capture_init_range capture_inits() {
return capture_init_range(capture_init_begin(), capture_init_end());
}

const_capture_init_range capture_inits() const {
return const_capture_init_range(capture_init_begin(), capture_init_end());
}


capture_init_iterator capture_init_begin() {
return reinterpret_cast<Expr **>(getStoredStmts());
}

const_capture_init_iterator capture_init_begin() const {
return reinterpret_cast<Expr *const *>(getStoredStmts());
}



capture_init_iterator capture_init_end() {
return capture_init_begin() + NumCaptures;
}

const_capture_init_iterator capture_init_end() const {
return capture_init_begin() + NumCaptures;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return getCapturedStmt()->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
return getCapturedStmt()->getEndLoc();
}

SourceRange getSourceRange() const LLVM_READONLY {
return getCapturedStmt()->getSourceRange();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CapturedStmtClass;
}

child_range children();

const_child_range children() const;
};

}

#endif
