











#if !defined(LLVM_CLANG_AST_EXPROPENMP_H)
#define LLVM_CLANG_AST_EXPROPENMP_H

#include "clang/AST/ComputeDependence.h"
#include "clang/AST/Expr.h"

namespace clang {




































class OMPArraySectionExpr : public Expr {
enum { BASE, LOWER_BOUND, LENGTH, STRIDE, END_EXPR };
Stmt *SubExprs[END_EXPR];
SourceLocation ColonLocFirst;
SourceLocation ColonLocSecond;
SourceLocation RBracketLoc;

public:
OMPArraySectionExpr(Expr *Base, Expr *LowerBound, Expr *Length, Expr *Stride,
QualType Type, ExprValueKind VK, ExprObjectKind OK,
SourceLocation ColonLocFirst,
SourceLocation ColonLocSecond, SourceLocation RBracketLoc)
: Expr(OMPArraySectionExprClass, Type, VK, OK),
ColonLocFirst(ColonLocFirst), ColonLocSecond(ColonLocSecond),
RBracketLoc(RBracketLoc) {
SubExprs[BASE] = Base;
SubExprs[LOWER_BOUND] = LowerBound;
SubExprs[LENGTH] = Length;
SubExprs[STRIDE] = Stride;
setDependence(computeDependence(this));
}


explicit OMPArraySectionExpr(EmptyShell Shell)
: Expr(OMPArraySectionExprClass, Shell) {}




Expr *getBase() { return cast<Expr>(SubExprs[BASE]); }
const Expr *getBase() const { return cast<Expr>(SubExprs[BASE]); }

void setBase(Expr *E) { SubExprs[BASE] = E; }


static QualType getBaseOriginalType(const Expr *Base);


Expr *getLowerBound() { return cast_or_null<Expr>(SubExprs[LOWER_BOUND]); }
const Expr *getLowerBound() const {
return cast_or_null<Expr>(SubExprs[LOWER_BOUND]);
}

void setLowerBound(Expr *E) { SubExprs[LOWER_BOUND] = E; }


Expr *getLength() { return cast_or_null<Expr>(SubExprs[LENGTH]); }
const Expr *getLength() const { return cast_or_null<Expr>(SubExprs[LENGTH]); }

void setLength(Expr *E) { SubExprs[LENGTH] = E; }


Expr *getStride() { return cast_or_null<Expr>(SubExprs[STRIDE]); }
const Expr *getStride() const { return cast_or_null<Expr>(SubExprs[STRIDE]); }

void setStride(Expr *E) { SubExprs[STRIDE] = E; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return getBase()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY { return RBracketLoc; }

SourceLocation getColonLocFirst() const { return ColonLocFirst; }
void setColonLocFirst(SourceLocation L) { ColonLocFirst = L; }

SourceLocation getColonLocSecond() const { return ColonLocSecond; }
void setColonLocSecond(SourceLocation L) { ColonLocSecond = L; }

SourceLocation getRBracketLoc() const { return RBracketLoc; }
void setRBracketLoc(SourceLocation L) { RBracketLoc = L; }

SourceLocation getExprLoc() const LLVM_READONLY {
return getBase()->getExprLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPArraySectionExprClass;
}

child_range children() {
return child_range(&SubExprs[BASE], &SubExprs[END_EXPR]);
}

const_child_range children() const {
return const_child_range(&SubExprs[BASE], &SubExprs[END_EXPR]);
}
};



class OMPArrayShapingExpr final
: public Expr,
private llvm::TrailingObjects<OMPArrayShapingExpr, Expr *, SourceRange> {
friend TrailingObjects;
friend class ASTStmtReader;
friend class ASTStmtWriter;

SourceLocation LPLoc;
SourceLocation RPLoc;
unsigned NumDims = 0;


OMPArrayShapingExpr(QualType ExprTy, Expr *Op, SourceLocation L,
SourceLocation R, ArrayRef<Expr *> Dims);


explicit OMPArrayShapingExpr(EmptyShell Shell, unsigned NumDims)
: Expr(OMPArrayShapingExprClass, Shell), NumDims(NumDims) {}


void setDimensions(ArrayRef<Expr *> Dims);


void setBase(Expr *Op) { getTrailingObjects<Expr *>()[NumDims] = Op; }


void setBracketsRanges(ArrayRef<SourceRange> BR);

unsigned numTrailingObjects(OverloadToken<Expr *>) const {

return NumDims + 1;
}

unsigned numTrailingObjects(OverloadToken<SourceRange>) const {
return NumDims;
}

public:
static OMPArrayShapingExpr *Create(const ASTContext &Context, QualType T,
Expr *Op, SourceLocation L,
SourceLocation R, ArrayRef<Expr *> Dims,
ArrayRef<SourceRange> BracketRanges);

static OMPArrayShapingExpr *CreateEmpty(const ASTContext &Context,
unsigned NumDims);

SourceLocation getLParenLoc() const { return LPLoc; }
void setLParenLoc(SourceLocation L) { LPLoc = L; }

SourceLocation getRParenLoc() const { return RPLoc; }
void setRParenLoc(SourceLocation L) { RPLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return LPLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return getBase()->getEndLoc();
}


ArrayRef<Expr *> getDimensions() const {
return llvm::makeArrayRef(getTrailingObjects<Expr *>(), NumDims);
}


ArrayRef<SourceRange> getBracketsRanges() const {
return llvm::makeArrayRef(getTrailingObjects<SourceRange>(), NumDims);
}


Expr *getBase() { return getTrailingObjects<Expr *>()[NumDims]; }
const Expr *getBase() const { return getTrailingObjects<Expr *>()[NumDims]; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPArrayShapingExprClass;
}


child_range children() {
Stmt **Begin = reinterpret_cast<Stmt **>(getTrailingObjects<Expr *>());
return child_range(Begin, Begin + NumDims + 1);
}
const_child_range children() const {
Stmt *const *Begin =
reinterpret_cast<Stmt *const *>(getTrailingObjects<Expr *>());
return const_child_range(Begin, Begin + NumDims + 1);
}
};



struct OMPIteratorHelperData {

VarDecl *CounterVD = nullptr;


Expr *Upper = nullptr;


Expr *Update = nullptr;

Expr *CounterUpdate = nullptr;
};

























class OMPIteratorExpr final
: public Expr,
private llvm::TrailingObjects<OMPIteratorExpr, Decl *, Expr *,
SourceLocation, OMPIteratorHelperData> {
public:

struct IteratorRange {
Expr *Begin = nullptr;
Expr *End = nullptr;
Expr *Step = nullptr;
};

struct IteratorDefinition {
Decl *IteratorDecl = nullptr;
IteratorRange Range;
SourceLocation AssignmentLoc;
SourceLocation ColonLoc, SecondColonLoc;
};

private:
friend TrailingObjects;
friend class ASTStmtReader;
friend class ASTStmtWriter;


enum class RangeExprOffset {
Begin = 0,
End = 1,
Step = 2,
Total = 3,
};


enum class RangeLocOffset {
AssignLoc = 0,
FirstColonLoc = 1,
SecondColonLoc = 2,
Total = 3,
};

SourceLocation IteratorKwLoc;

SourceLocation LPLoc;

SourceLocation RPLoc;

unsigned NumIterators = 0;

OMPIteratorExpr(QualType ExprTy, SourceLocation IteratorKwLoc,
SourceLocation L, SourceLocation R,
ArrayRef<IteratorDefinition> Data,
ArrayRef<OMPIteratorHelperData> Helpers);


explicit OMPIteratorExpr(EmptyShell Shell, unsigned NumIterators)
: Expr(OMPIteratorExprClass, Shell), NumIterators(NumIterators) {}


void setIteratorDeclaration(unsigned I, Decl *D);



void setAssignmentLoc(unsigned I, SourceLocation Loc);



void setIteratorRange(unsigned I, Expr *Begin, SourceLocation ColonLoc,
Expr *End, SourceLocation SecondColonLoc, Expr *Step);


void setHelper(unsigned I, const OMPIteratorHelperData &D);

unsigned numTrailingObjects(OverloadToken<Decl *>) const {
return NumIterators;
}

unsigned numTrailingObjects(OverloadToken<Expr *>) const {
return NumIterators * static_cast<int>(RangeExprOffset::Total);
}

unsigned numTrailingObjects(OverloadToken<SourceLocation>) const {
return NumIterators * static_cast<int>(RangeLocOffset::Total);
}

public:
static OMPIteratorExpr *Create(const ASTContext &Context, QualType T,
SourceLocation IteratorKwLoc, SourceLocation L,
SourceLocation R,
ArrayRef<IteratorDefinition> Data,
ArrayRef<OMPIteratorHelperData> Helpers);

static OMPIteratorExpr *CreateEmpty(const ASTContext &Context,
unsigned NumIterators);

SourceLocation getLParenLoc() const { return LPLoc; }
void setLParenLoc(SourceLocation L) { LPLoc = L; }

SourceLocation getRParenLoc() const { return RPLoc; }
void setRParenLoc(SourceLocation L) { RPLoc = L; }

SourceLocation getIteratorKwLoc() const { return IteratorKwLoc; }
void setIteratorKwLoc(SourceLocation L) { IteratorKwLoc = L; }
SourceLocation getBeginLoc() const LLVM_READONLY { return IteratorKwLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RPLoc; }


Decl *getIteratorDecl(unsigned I);
const Decl *getIteratorDecl(unsigned I) const {
return const_cast<OMPIteratorExpr *>(this)->getIteratorDecl(I);
}


IteratorRange getIteratorRange(unsigned I);
const IteratorRange getIteratorRange(unsigned I) const {
return const_cast<OMPIteratorExpr *>(this)->getIteratorRange(I);
}


SourceLocation getAssignLoc(unsigned I) const;


SourceLocation getColonLoc(unsigned I) const;


SourceLocation getSecondColonLoc(unsigned I) const;


unsigned numOfIterators() const { return NumIterators; }


OMPIteratorHelperData &getHelper(unsigned I);
const OMPIteratorHelperData &getHelper(unsigned I) const;

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPIteratorExprClass;
}


child_range children() {
Stmt **Begin = reinterpret_cast<Stmt **>(getTrailingObjects<Expr *>());
return child_range(
Begin, Begin + NumIterators * static_cast<int>(RangeExprOffset::Total));
}
const_child_range children() const {
Stmt *const *Begin =
reinterpret_cast<Stmt *const *>(getTrailingObjects<Expr *>());
return const_child_range(
Begin, Begin + NumIterators * static_cast<int>(RangeExprOffset::Total));
}
};

}

#endif
