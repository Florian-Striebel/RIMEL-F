











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SMTCONV_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SMTCONV_H

#include "clang/AST/Expr.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/APSIntType.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymbolManager.h"
#include "llvm/Support/SMTAPI.h"

namespace clang {
namespace ento {

class SMTConv {
public:

static inline llvm::SMTSortRef mkSort(llvm::SMTSolverRef &Solver,
const QualType &Ty, unsigned BitWidth) {
if (Ty->isBooleanType())
return Solver->getBoolSort();

if (Ty->isRealFloatingType())
return Solver->getFloatSort(BitWidth);

return Solver->getBitvectorSort(BitWidth);
}


static inline llvm::SMTExprRef fromUnOp(llvm::SMTSolverRef &Solver,
const UnaryOperator::Opcode Op,
const llvm::SMTExprRef &Exp) {
switch (Op) {
case UO_Minus:
return Solver->mkBVNeg(Exp);

case UO_Not:
return Solver->mkBVNot(Exp);

case UO_LNot:
return Solver->mkNot(Exp);

default:;
}
llvm_unreachable("Unimplemented opcode");
}


static inline llvm::SMTExprRef fromFloatUnOp(llvm::SMTSolverRef &Solver,
const UnaryOperator::Opcode Op,
const llvm::SMTExprRef &Exp) {
switch (Op) {
case UO_Minus:
return Solver->mkFPNeg(Exp);

case UO_LNot:
return fromUnOp(Solver, Op, Exp);

default:;
}
llvm_unreachable("Unimplemented opcode");
}


static inline llvm::SMTExprRef
fromNBinOp(llvm::SMTSolverRef &Solver, const BinaryOperator::Opcode Op,
const std::vector<llvm::SMTExprRef> &ASTs) {
assert(!ASTs.empty());

if (Op != BO_LAnd && Op != BO_LOr)
llvm_unreachable("Unimplemented opcode");

llvm::SMTExprRef res = ASTs.front();
for (std::size_t i = 1; i < ASTs.size(); ++i)
res = (Op == BO_LAnd) ? Solver->mkAnd(res, ASTs[i])
: Solver->mkOr(res, ASTs[i]);
return res;
}


static inline llvm::SMTExprRef fromBinOp(llvm::SMTSolverRef &Solver,
const llvm::SMTExprRef &LHS,
const BinaryOperator::Opcode Op,
const llvm::SMTExprRef &RHS,
bool isSigned) {
assert(*Solver->getSort(LHS) == *Solver->getSort(RHS) &&
"AST's must have the same sort!");

switch (Op) {

case BO_Mul:
return Solver->mkBVMul(LHS, RHS);

case BO_Div:
return isSigned ? Solver->mkBVSDiv(LHS, RHS) : Solver->mkBVUDiv(LHS, RHS);

case BO_Rem:
return isSigned ? Solver->mkBVSRem(LHS, RHS) : Solver->mkBVURem(LHS, RHS);


case BO_Add:
return Solver->mkBVAdd(LHS, RHS);

case BO_Sub:
return Solver->mkBVSub(LHS, RHS);


case BO_Shl:
return Solver->mkBVShl(LHS, RHS);

case BO_Shr:
return isSigned ? Solver->mkBVAshr(LHS, RHS) : Solver->mkBVLshr(LHS, RHS);


case BO_LT:
return isSigned ? Solver->mkBVSlt(LHS, RHS) : Solver->mkBVUlt(LHS, RHS);

case BO_GT:
return isSigned ? Solver->mkBVSgt(LHS, RHS) : Solver->mkBVUgt(LHS, RHS);

case BO_LE:
return isSigned ? Solver->mkBVSle(LHS, RHS) : Solver->mkBVUle(LHS, RHS);

case BO_GE:
return isSigned ? Solver->mkBVSge(LHS, RHS) : Solver->mkBVUge(LHS, RHS);


case BO_EQ:
return Solver->mkEqual(LHS, RHS);

case BO_NE:
return fromUnOp(Solver, UO_LNot,
fromBinOp(Solver, LHS, BO_EQ, RHS, isSigned));


case BO_And:
return Solver->mkBVAnd(LHS, RHS);

case BO_Xor:
return Solver->mkBVXor(LHS, RHS);

case BO_Or:
return Solver->mkBVOr(LHS, RHS);


case BO_LAnd:
return Solver->mkAnd(LHS, RHS);

case BO_LOr:
return Solver->mkOr(LHS, RHS);

default:;
}
llvm_unreachable("Unimplemented opcode");
}



static inline llvm::SMTExprRef
fromFloatSpecialBinOp(llvm::SMTSolverRef &Solver, const llvm::SMTExprRef &LHS,
const BinaryOperator::Opcode Op,
const llvm::APFloat::fltCategory &RHS) {
switch (Op) {

case BO_EQ:
switch (RHS) {
case llvm::APFloat::fcInfinity:
return Solver->mkFPIsInfinite(LHS);

case llvm::APFloat::fcNaN:
return Solver->mkFPIsNaN(LHS);

case llvm::APFloat::fcNormal:
return Solver->mkFPIsNormal(LHS);

case llvm::APFloat::fcZero:
return Solver->mkFPIsZero(LHS);
}
break;

case BO_NE:
return fromFloatUnOp(Solver, UO_LNot,
fromFloatSpecialBinOp(Solver, LHS, BO_EQ, RHS));

default:;
}

llvm_unreachable("Unimplemented opcode");
}


static inline llvm::SMTExprRef fromFloatBinOp(llvm::SMTSolverRef &Solver,
const llvm::SMTExprRef &LHS,
const BinaryOperator::Opcode Op,
const llvm::SMTExprRef &RHS) {
assert(*Solver->getSort(LHS) == *Solver->getSort(RHS) &&
"AST's must have the same sort!");

switch (Op) {

case BO_Mul:
return Solver->mkFPMul(LHS, RHS);

case BO_Div:
return Solver->mkFPDiv(LHS, RHS);

case BO_Rem:
return Solver->mkFPRem(LHS, RHS);


case BO_Add:
return Solver->mkFPAdd(LHS, RHS);

case BO_Sub:
return Solver->mkFPSub(LHS, RHS);


case BO_LT:
return Solver->mkFPLt(LHS, RHS);

case BO_GT:
return Solver->mkFPGt(LHS, RHS);

case BO_LE:
return Solver->mkFPLe(LHS, RHS);

case BO_GE:
return Solver->mkFPGe(LHS, RHS);


case BO_EQ:
return Solver->mkFPEqual(LHS, RHS);

case BO_NE:
return fromFloatUnOp(Solver, UO_LNot,
fromFloatBinOp(Solver, LHS, BO_EQ, RHS));


case BO_LAnd:
case BO_LOr:
return fromBinOp(Solver, LHS, Op, RHS, false);

default:;
}

llvm_unreachable("Unimplemented opcode");
}



static inline llvm::SMTExprRef fromCast(llvm::SMTSolverRef &Solver,
const llvm::SMTExprRef &Exp,
QualType ToTy, uint64_t ToBitWidth,
QualType FromTy,
uint64_t FromBitWidth) {
if ((FromTy->isIntegralOrEnumerationType() &&
ToTy->isIntegralOrEnumerationType()) ||
(FromTy->isAnyPointerType() ^ ToTy->isAnyPointerType()) ||
(FromTy->isBlockPointerType() ^ ToTy->isBlockPointerType()) ||
(FromTy->isReferenceType() ^ ToTy->isReferenceType())) {

if (FromTy->isBooleanType()) {
assert(ToBitWidth > 0 && "BitWidth must be positive!");
return Solver->mkIte(
Exp, Solver->mkBitvector(llvm::APSInt("1"), ToBitWidth),
Solver->mkBitvector(llvm::APSInt("0"), ToBitWidth));
}

if (ToBitWidth > FromBitWidth)
return FromTy->isSignedIntegerOrEnumerationType()
? Solver->mkBVSignExt(ToBitWidth - FromBitWidth, Exp)
: Solver->mkBVZeroExt(ToBitWidth - FromBitWidth, Exp);

if (ToBitWidth < FromBitWidth)
return Solver->mkBVExtract(ToBitWidth - 1, 0, Exp);


return Exp;
}

if (FromTy->isRealFloatingType() && ToTy->isRealFloatingType()) {
if (ToBitWidth != FromBitWidth)
return Solver->mkFPtoFP(Exp, Solver->getFloatSort(ToBitWidth));

return Exp;
}

if (FromTy->isIntegralOrEnumerationType() && ToTy->isRealFloatingType()) {
llvm::SMTSortRef Sort = Solver->getFloatSort(ToBitWidth);
return FromTy->isSignedIntegerOrEnumerationType()
? Solver->mkSBVtoFP(Exp, Sort)
: Solver->mkUBVtoFP(Exp, Sort);
}

if (FromTy->isRealFloatingType() && ToTy->isIntegralOrEnumerationType())
return ToTy->isSignedIntegerOrEnumerationType()
? Solver->mkFPtoSBV(Exp, ToBitWidth)
: Solver->mkFPtoUBV(Exp, ToBitWidth);

llvm_unreachable("Unsupported explicit type cast!");
}


static inline llvm::APSInt castAPSInt(llvm::SMTSolverRef &Solver,
const llvm::APSInt &V, QualType ToTy,
uint64_t ToWidth, QualType FromTy,
uint64_t FromWidth) {
APSIntType TargetType(ToWidth, !ToTy->isSignedIntegerOrEnumerationType());
return TargetType.convert(V);
}


static inline llvm::SMTExprRef
fromData(llvm::SMTSolverRef &Solver, ASTContext &Ctx, const SymbolData *Sym) {
const SymbolID ID = Sym->getSymbolID();
const QualType Ty = Sym->getType();
const uint64_t BitWidth = Ctx.getTypeSize(Ty);

llvm::SmallString<16> Str;
llvm::raw_svector_ostream OS(Str);
OS << Sym->getKindStr() << ID;
return Solver->mkSymbol(Str.c_str(), mkSort(Solver, Ty, BitWidth));
}


static inline llvm::SMTExprRef getCastExpr(llvm::SMTSolverRef &Solver,
ASTContext &Ctx,
const llvm::SMTExprRef &Exp,
QualType FromTy, QualType ToTy) {
return fromCast(Solver, Exp, ToTy, Ctx.getTypeSize(ToTy), FromTy,
Ctx.getTypeSize(FromTy));
}



static inline llvm::SMTExprRef
getBinExpr(llvm::SMTSolverRef &Solver, ASTContext &Ctx,
const llvm::SMTExprRef &LHS, QualType LTy,
BinaryOperator::Opcode Op, const llvm::SMTExprRef &RHS,
QualType RTy, QualType *RetTy) {
llvm::SMTExprRef NewLHS = LHS;
llvm::SMTExprRef NewRHS = RHS;
doTypeConversion(Solver, Ctx, NewLHS, NewRHS, LTy, RTy);


if (RetTy) {



if (BinaryOperator::isComparisonOp(Op) ||
BinaryOperator::isLogicalOp(Op)) {
*RetTy = Ctx.BoolTy;
} else {
*RetTy = LTy;
}



if (LTy->isAnyPointerType() && RTy->isAnyPointerType() && Op == BO_Sub) {
*RetTy = Ctx.getPointerDiffType();
}
}

return LTy->isRealFloatingType()
? fromFloatBinOp(Solver, NewLHS, Op, NewRHS)
: fromBinOp(Solver, NewLHS, Op, NewRHS,
LTy->isSignedIntegerOrEnumerationType());
}



static inline llvm::SMTExprRef getSymBinExpr(llvm::SMTSolverRef &Solver,
ASTContext &Ctx,
const BinarySymExpr *BSE,
bool *hasComparison,
QualType *RetTy) {
QualType LTy, RTy;
BinaryOperator::Opcode Op = BSE->getOpcode();

if (const SymIntExpr *SIE = dyn_cast<SymIntExpr>(BSE)) {
llvm::SMTExprRef LHS =
getSymExpr(Solver, Ctx, SIE->getLHS(), &LTy, hasComparison);
llvm::APSInt NewRInt;
std::tie(NewRInt, RTy) = fixAPSInt(Ctx, SIE->getRHS());
llvm::SMTExprRef RHS =
Solver->mkBitvector(NewRInt, NewRInt.getBitWidth());
return getBinExpr(Solver, Ctx, LHS, LTy, Op, RHS, RTy, RetTy);
}

if (const IntSymExpr *ISE = dyn_cast<IntSymExpr>(BSE)) {
llvm::APSInt NewLInt;
std::tie(NewLInt, LTy) = fixAPSInt(Ctx, ISE->getLHS());
llvm::SMTExprRef LHS =
Solver->mkBitvector(NewLInt, NewLInt.getBitWidth());
llvm::SMTExprRef RHS =
getSymExpr(Solver, Ctx, ISE->getRHS(), &RTy, hasComparison);
return getBinExpr(Solver, Ctx, LHS, LTy, Op, RHS, RTy, RetTy);
}

if (const SymSymExpr *SSM = dyn_cast<SymSymExpr>(BSE)) {
llvm::SMTExprRef LHS =
getSymExpr(Solver, Ctx, SSM->getLHS(), &LTy, hasComparison);
llvm::SMTExprRef RHS =
getSymExpr(Solver, Ctx, SSM->getRHS(), &RTy, hasComparison);
return getBinExpr(Solver, Ctx, LHS, LTy, Op, RHS, RTy, RetTy);
}

llvm_unreachable("Unsupported BinarySymExpr type!");
}



static inline llvm::SMTExprRef getSymExpr(llvm::SMTSolverRef &Solver,
ASTContext &Ctx, SymbolRef Sym,
QualType *RetTy,
bool *hasComparison) {
if (const SymbolData *SD = dyn_cast<SymbolData>(Sym)) {
if (RetTy)
*RetTy = Sym->getType();

return fromData(Solver, Ctx, SD);
}

if (const SymbolCast *SC = dyn_cast<SymbolCast>(Sym)) {
if (RetTy)
*RetTy = Sym->getType();

QualType FromTy;
llvm::SMTExprRef Exp =
getSymExpr(Solver, Ctx, SC->getOperand(), &FromTy, hasComparison);




if (hasComparison)
*hasComparison = false;
return getCastExpr(Solver, Ctx, Exp, FromTy, Sym->getType());
}

if (const BinarySymExpr *BSE = dyn_cast<BinarySymExpr>(Sym)) {
llvm::SMTExprRef Exp =
getSymBinExpr(Solver, Ctx, BSE, hasComparison, RetTy);

if (hasComparison)
*hasComparison = BinaryOperator::isComparisonOp(BSE->getOpcode());
return Exp;
}

llvm_unreachable("Unsupported SymbolRef type!");
}





static inline llvm::SMTExprRef getExpr(llvm::SMTSolverRef &Solver,
ASTContext &Ctx, SymbolRef Sym,
QualType *RetTy = nullptr,
bool *hasComparison = nullptr) {
if (hasComparison) {
*hasComparison = false;
}

return getSymExpr(Solver, Ctx, Sym, RetTy, hasComparison);
}


static inline llvm::SMTExprRef getZeroExpr(llvm::SMTSolverRef &Solver,
ASTContext &Ctx,
const llvm::SMTExprRef &Exp,
QualType Ty, bool Assumption) {
if (Ty->isRealFloatingType()) {
llvm::APFloat Zero =
llvm::APFloat::getZero(Ctx.getFloatTypeSemantics(Ty));
return fromFloatBinOp(Solver, Exp, Assumption ? BO_EQ : BO_NE,
Solver->mkFloat(Zero));
}

if (Ty->isIntegralOrEnumerationType() || Ty->isAnyPointerType() ||
Ty->isBlockPointerType() || Ty->isReferenceType()) {


bool isSigned = Ty->isSignedIntegerOrEnumerationType();
if (Ty->isBooleanType())
return Assumption ? fromUnOp(Solver, UO_LNot, Exp) : Exp;

return fromBinOp(
Solver, Exp, Assumption ? BO_EQ : BO_NE,
Solver->mkBitvector(llvm::APSInt("0"), Ctx.getTypeSize(Ty)),
isSigned);
}

llvm_unreachable("Unsupported type for zero value!");
}



static inline llvm::SMTExprRef
getRangeExpr(llvm::SMTSolverRef &Solver, ASTContext &Ctx, SymbolRef Sym,
const llvm::APSInt &From, const llvm::APSInt &To, bool InRange) {

QualType FromTy;
llvm::APSInt NewFromInt;
std::tie(NewFromInt, FromTy) = fixAPSInt(Ctx, From);
llvm::SMTExprRef FromExp =
Solver->mkBitvector(NewFromInt, NewFromInt.getBitWidth());


QualType SymTy;
llvm::SMTExprRef Exp = getExpr(Solver, Ctx, Sym, &SymTy);


if (From == To)
return getBinExpr(Solver, Ctx, Exp, SymTy, InRange ? BO_EQ : BO_NE,
FromExp, FromTy, nullptr);

QualType ToTy;
llvm::APSInt NewToInt;
std::tie(NewToInt, ToTy) = fixAPSInt(Ctx, To);
llvm::SMTExprRef ToExp =
Solver->mkBitvector(NewToInt, NewToInt.getBitWidth());
assert(FromTy == ToTy && "Range values have different types!");


llvm::SMTExprRef LHS =
getBinExpr(Solver, Ctx, Exp, SymTy, InRange ? BO_GE : BO_LT, FromExp,
FromTy, nullptr);
llvm::SMTExprRef RHS = getBinExpr(Solver, Ctx, Exp, SymTy,
InRange ? BO_LE : BO_GT, ToExp, ToTy,
nullptr);

return fromBinOp(Solver, LHS, InRange ? BO_LAnd : BO_LOr, RHS,
SymTy->isSignedIntegerOrEnumerationType());
}



static inline QualType getAPSIntType(ASTContext &Ctx,
const llvm::APSInt &Int) {
return Ctx.getIntTypeForBitwidth(Int.getBitWidth(), Int.isSigned());
}


static inline std::pair<llvm::APSInt, QualType>
fixAPSInt(ASTContext &Ctx, const llvm::APSInt &Int) {
llvm::APSInt NewInt;




if (Int.getBitWidth() == 1 && getAPSIntType(Ctx, Int).isNull()) {
NewInt = Int.extend(Ctx.getTypeSize(Ctx.BoolTy));
} else
NewInt = Int;

return std::make_pair(NewInt, getAPSIntType(Ctx, NewInt));
}




static inline void doTypeConversion(llvm::SMTSolverRef &Solver,
ASTContext &Ctx, llvm::SMTExprRef &LHS,
llvm::SMTExprRef &RHS, QualType &LTy,
QualType &RTy) {
assert(!LTy.isNull() && !RTy.isNull() && "Input type is null!");


if ((LTy->isIntegralOrEnumerationType() &&
RTy->isIntegralOrEnumerationType()) &&
(LTy->isArithmeticType() && RTy->isArithmeticType())) {
SMTConv::doIntTypeConversion<llvm::SMTExprRef, &fromCast>(
Solver, Ctx, LHS, LTy, RHS, RTy);
return;
}

if (LTy->isRealFloatingType() || RTy->isRealFloatingType()) {
SMTConv::doFloatTypeConversion<llvm::SMTExprRef, &fromCast>(
Solver, Ctx, LHS, LTy, RHS, RTy);
return;
}

if ((LTy->isAnyPointerType() || RTy->isAnyPointerType()) ||
(LTy->isBlockPointerType() || RTy->isBlockPointerType()) ||
(LTy->isReferenceType() || RTy->isReferenceType())) {



uint64_t LBitWidth = Ctx.getTypeSize(LTy);
uint64_t RBitWidth = Ctx.getTypeSize(RTy);



if ((LTy->isAnyPointerType() ^ RTy->isAnyPointerType()) ||
(LTy->isBlockPointerType() ^ RTy->isBlockPointerType()) ||
(LTy->isReferenceType() ^ RTy->isReferenceType())) {
if (LTy->isNullPtrType() || LTy->isBlockPointerType() ||
LTy->isReferenceType()) {
LHS = fromCast(Solver, LHS, RTy, RBitWidth, LTy, LBitWidth);
LTy = RTy;
} else {
RHS = fromCast(Solver, RHS, LTy, LBitWidth, RTy, RBitWidth);
RTy = LTy;
}
}





if (LTy->isVoidPointerType() ^ RTy->isVoidPointerType()) {
assert((Ctx.getTypeSize(LTy) == Ctx.getTypeSize(RTy)) &&
"Pointer types have different bitwidths!");
if (RTy->isVoidPointerType())
RTy = LTy;
else
LTy = RTy;
}

if (LTy == RTy)
return;
}


if ((LTy.getCanonicalType() == RTy.getCanonicalType()) ||
(LTy->isObjCObjectPointerType() && RTy->isObjCObjectPointerType())) {
LTy = RTy;
return;
}


}




template <typename T, T (*doCast)(llvm::SMTSolverRef &Solver, const T &,
QualType, uint64_t, QualType, uint64_t)>
static inline void doIntTypeConversion(llvm::SMTSolverRef &Solver,
ASTContext &Ctx, T &LHS, QualType &LTy,
T &RHS, QualType &RTy) {
uint64_t LBitWidth = Ctx.getTypeSize(LTy);
uint64_t RBitWidth = Ctx.getTypeSize(RTy);

assert(!LTy.isNull() && !RTy.isNull() && "Input type is null!");


if (LTy->isPromotableIntegerType()) {
QualType NewTy = Ctx.getPromotedIntegerType(LTy);
uint64_t NewBitWidth = Ctx.getTypeSize(NewTy);
LHS = (*doCast)(Solver, LHS, NewTy, NewBitWidth, LTy, LBitWidth);
LTy = NewTy;
LBitWidth = NewBitWidth;
}
if (RTy->isPromotableIntegerType()) {
QualType NewTy = Ctx.getPromotedIntegerType(RTy);
uint64_t NewBitWidth = Ctx.getTypeSize(NewTy);
RHS = (*doCast)(Solver, RHS, NewTy, NewBitWidth, RTy, RBitWidth);
RTy = NewTy;
RBitWidth = NewBitWidth;
}

if (LTy == RTy)
return;



bool isLSignedTy = LTy->isSignedIntegerOrEnumerationType();
bool isRSignedTy = RTy->isSignedIntegerOrEnumerationType();

int order = Ctx.getIntegerTypeOrder(LTy, RTy);
if (isLSignedTy == isRSignedTy) {

if (order == 1) {
RHS = (*doCast)(Solver, RHS, LTy, LBitWidth, RTy, RBitWidth);
RTy = LTy;
} else {
LHS = (*doCast)(Solver, LHS, RTy, RBitWidth, LTy, LBitWidth);
LTy = RTy;
}
} else if (order != (isLSignedTy ? 1 : -1)) {


if (isRSignedTy) {
RHS = (*doCast)(Solver, RHS, LTy, LBitWidth, RTy, RBitWidth);
RTy = LTy;
} else {
LHS = (*doCast)(Solver, LHS, RTy, RBitWidth, LTy, LBitWidth);
LTy = RTy;
}
} else if (LBitWidth != RBitWidth) {



if (isLSignedTy) {
RHS = (doCast)(Solver, RHS, LTy, LBitWidth, RTy, RBitWidth);
RTy = LTy;
} else {
LHS = (*doCast)(Solver, LHS, RTy, RBitWidth, LTy, LBitWidth);
LTy = RTy;
}
} else {




QualType NewTy =
Ctx.getCorrespondingUnsignedType(isLSignedTy ? LTy : RTy);
RHS = (*doCast)(Solver, RHS, LTy, LBitWidth, RTy, RBitWidth);
RTy = NewTy;
LHS = (doCast)(Solver, LHS, RTy, RBitWidth, LTy, LBitWidth);
LTy = NewTy;
}
}




template <typename T, T (*doCast)(llvm::SMTSolverRef &Solver, const T &,
QualType, uint64_t, QualType, uint64_t)>
static inline void
doFloatTypeConversion(llvm::SMTSolverRef &Solver, ASTContext &Ctx, T &LHS,
QualType &LTy, T &RHS, QualType &RTy) {
uint64_t LBitWidth = Ctx.getTypeSize(LTy);
uint64_t RBitWidth = Ctx.getTypeSize(RTy);


if (!LTy->isRealFloatingType()) {
LHS = (*doCast)(Solver, LHS, RTy, RBitWidth, LTy, LBitWidth);
LTy = RTy;
LBitWidth = RBitWidth;
}
if (!RTy->isRealFloatingType()) {
RHS = (*doCast)(Solver, RHS, LTy, LBitWidth, RTy, RBitWidth);
RTy = LTy;
RBitWidth = LBitWidth;
}

if (LTy == RTy)
return;




int order = Ctx.getFloatingTypeOrder(LTy, RTy);
if (order > 0) {
RHS = (*doCast)(Solver, RHS, LTy, LBitWidth, RTy, RBitWidth);
RTy = LTy;
} else if (order == 0) {
LHS = (*doCast)(Solver, LHS, RTy, RBitWidth, LTy, LBitWidth);
LTy = RTy;
} else {
llvm_unreachable("Unsupported floating-point type cast!");
}
}
};
}
}

#endif
