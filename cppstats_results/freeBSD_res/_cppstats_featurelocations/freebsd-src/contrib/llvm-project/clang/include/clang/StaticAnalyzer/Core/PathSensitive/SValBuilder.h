












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SVALBUILDER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SVALBUILDER_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/BasicValueFactory.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/MemRegion.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymExpr.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymbolManager.h"
#include "llvm/ADT/ImmutableList.h"
#include "llvm/ADT/Optional.h"
#include <cstdint>

namespace clang {

class BlockDecl;
class CXXBoolLiteralExpr;
class CXXMethodDecl;
class CXXRecordDecl;
class DeclaratorDecl;
class FunctionDecl;
class LocationContext;
class StackFrameContext;
class Stmt;

namespace ento {

class ConditionTruthVal;
class ProgramStateManager;
class StoreRef;

class SValBuilder {
virtual void anchor();

protected:
ASTContext &Context;


BasicValueFactory BasicVals;


SymbolManager SymMgr;


MemRegionManager MemMgr;

ProgramStateManager &StateMgr;


const QualType ArrayIndexTy;


const unsigned ArrayIndexWidth;

SVal evalCastKind(UndefinedVal V, QualType CastTy, QualType OriginalTy);
SVal evalCastKind(UnknownVal V, QualType CastTy, QualType OriginalTy);
SVal evalCastKind(Loc V, QualType CastTy, QualType OriginalTy);
SVal evalCastKind(NonLoc V, QualType CastTy, QualType OriginalTy);
SVal evalCastSubKind(loc::ConcreteInt V, QualType CastTy,
QualType OriginalTy);
SVal evalCastSubKind(loc::GotoLabel V, QualType CastTy, QualType OriginalTy);
SVal evalCastSubKind(loc::MemRegionVal V, QualType CastTy,
QualType OriginalTy);
SVal evalCastSubKind(nonloc::CompoundVal V, QualType CastTy,
QualType OriginalTy);
SVal evalCastSubKind(nonloc::ConcreteInt V, QualType CastTy,
QualType OriginalTy);
SVal evalCastSubKind(nonloc::LazyCompoundVal V, QualType CastTy,
QualType OriginalTy);
SVal evalCastSubKind(nonloc::LocAsInteger V, QualType CastTy,
QualType OriginalTy);
SVal evalCastSubKind(nonloc::SymbolVal V, QualType CastTy,
QualType OriginalTy);
SVal evalCastSubKind(nonloc::PointerToMember V, QualType CastTy,
QualType OriginalTy);

public:
SValBuilder(llvm::BumpPtrAllocator &alloc, ASTContext &context,
ProgramStateManager &stateMgr)
: Context(context), BasicVals(context, alloc),
SymMgr(context, BasicVals, alloc), MemMgr(context, alloc),
StateMgr(stateMgr), ArrayIndexTy(context.LongLongTy),
ArrayIndexWidth(context.getTypeSize(ArrayIndexTy)) {}

virtual ~SValBuilder() = default;

bool haveSameType(const SymExpr *Sym1, const SymExpr *Sym2) {
return haveSameType(Sym1->getType(), Sym2->getType());
}

bool haveSameType(QualType Ty1, QualType Ty2) {


return (Context.getCanonicalType(Ty1) == Context.getCanonicalType(Ty2) ||
(Ty1->isIntegralOrEnumerationType() &&
Ty2->isIntegralOrEnumerationType()));
}

SVal evalCast(SVal V, QualType CastTy, QualType OriginalTy);


SVal evalIntegralCast(ProgramStateRef state, SVal val, QualType castTy,
QualType originalType);

virtual SVal evalMinus(NonLoc val) = 0;

virtual SVal evalComplement(NonLoc val) = 0;



virtual SVal evalBinOpNN(ProgramStateRef state, BinaryOperator::Opcode op,
NonLoc lhs, NonLoc rhs, QualType resultTy) = 0;



virtual SVal evalBinOpLL(ProgramStateRef state, BinaryOperator::Opcode op,
Loc lhs, Loc rhs, QualType resultTy) = 0;




virtual SVal evalBinOpLN(ProgramStateRef state, BinaryOperator::Opcode op,
Loc lhs, NonLoc rhs, QualType resultTy) = 0;



virtual const llvm::APSInt *getKnownValue(ProgramStateRef state, SVal val) = 0;




virtual SVal simplifySVal(ProgramStateRef State, SVal Val) = 0;


SVal makeSymExprValNN(BinaryOperator::Opcode op,
NonLoc lhs, NonLoc rhs, QualType resultTy);

SVal evalBinOp(ProgramStateRef state, BinaryOperator::Opcode op,
SVal lhs, SVal rhs, QualType type);


ConditionTruthVal areEqual(ProgramStateRef state, SVal lhs, SVal rhs);

SVal evalEQ(ProgramStateRef state, SVal lhs, SVal rhs);

DefinedOrUnknownSVal evalEQ(ProgramStateRef state, DefinedOrUnknownSVal lhs,
DefinedOrUnknownSVal rhs);

ASTContext &getContext() { return Context; }
const ASTContext &getContext() const { return Context; }

ProgramStateManager &getStateManager() { return StateMgr; }

QualType getConditionType() const {
return Context.getLangOpts().CPlusPlus ? Context.BoolTy : Context.IntTy;
}

QualType getArrayIndexType() const {
return ArrayIndexTy;
}

BasicValueFactory &getBasicValueFactory() { return BasicVals; }
const BasicValueFactory &getBasicValueFactory() const { return BasicVals; }

SymbolManager &getSymbolManager() { return SymMgr; }
const SymbolManager &getSymbolManager() const { return SymMgr; }

MemRegionManager &getRegionManager() { return MemMgr; }
const MemRegionManager &getRegionManager() const { return MemMgr; }



const SymbolConjured* conjureSymbol(const Stmt *stmt,
const LocationContext *LCtx,
QualType type,
unsigned visitCount,
const void *symbolTag = nullptr) {
return SymMgr.conjureSymbol(stmt, LCtx, type, visitCount, symbolTag);
}

const SymbolConjured* conjureSymbol(const Expr *expr,
const LocationContext *LCtx,
unsigned visitCount,
const void *symbolTag = nullptr) {
return SymMgr.conjureSymbol(expr, LCtx, visitCount, symbolTag);
}


DefinedOrUnknownSVal makeZeroVal(QualType type);


DefinedOrUnknownSVal getRegionValueSymbolVal(const TypedValueRegion *region);







DefinedOrUnknownSVal conjureSymbolVal(const void *symbolTag,
const Expr *expr,
const LocationContext *LCtx,
unsigned count);
DefinedOrUnknownSVal conjureSymbolVal(const void *symbolTag,
const Expr *expr,
const LocationContext *LCtx,
QualType type,
unsigned count);
DefinedOrUnknownSVal conjureSymbolVal(const Stmt *stmt,
const LocationContext *LCtx,
QualType type,
unsigned visitCount);




DefinedOrUnknownSVal getConjuredHeapSymbolVal(const Expr *E,
const LocationContext *LCtx,
unsigned Count);





DefinedOrUnknownSVal getConjuredHeapSymbolVal(const Expr *E,
const LocationContext *LCtx,
QualType type, unsigned Count);

DefinedOrUnknownSVal getDerivedRegionValueSymbolVal(
SymbolRef parentSymbol, const TypedValueRegion *region);

DefinedSVal getMetadataSymbolVal(const void *symbolTag,
const MemRegion *region,
const Expr *expr, QualType type,
const LocationContext *LCtx,
unsigned count);

DefinedSVal getMemberPointer(const NamedDecl *ND);

DefinedSVal getFunctionPointer(const FunctionDecl *func);

DefinedSVal getBlockPointer(const BlockDecl *block, CanQualType locTy,
const LocationContext *locContext,
unsigned blockCount);





Optional<SVal> getConstantVal(const Expr *E);

NonLoc makeCompoundVal(QualType type, llvm::ImmutableList<SVal> vals) {
return nonloc::CompoundVal(BasicVals.getCompoundValData(type, vals));
}

NonLoc makeLazyCompoundVal(const StoreRef &store,
const TypedValueRegion *region) {
return nonloc::LazyCompoundVal(
BasicVals.getLazyCompoundValData(store, region));
}

NonLoc makePointerToMember(const DeclaratorDecl *DD) {
return nonloc::PointerToMember(DD);
}

NonLoc makePointerToMember(const PointerToMemberData *PTMD) {
return nonloc::PointerToMember(PTMD);
}

NonLoc makeZeroArrayIndex() {
return nonloc::ConcreteInt(BasicVals.getValue(0, ArrayIndexTy));
}

NonLoc makeArrayIndex(uint64_t idx) {
return nonloc::ConcreteInt(BasicVals.getValue(idx, ArrayIndexTy));
}

SVal convertToArrayIndex(SVal val);

nonloc::ConcreteInt makeIntVal(const IntegerLiteral* integer) {
return nonloc::ConcreteInt(
BasicVals.getValue(integer->getValue(),
integer->getType()->isUnsignedIntegerOrEnumerationType()));
}

nonloc::ConcreteInt makeBoolVal(const ObjCBoolLiteralExpr *boolean) {
return makeTruthVal(boolean->getValue(), boolean->getType());
}

nonloc::ConcreteInt makeBoolVal(const CXXBoolLiteralExpr *boolean);

nonloc::ConcreteInt makeIntVal(const llvm::APSInt& integer) {
return nonloc::ConcreteInt(BasicVals.getValue(integer));
}

loc::ConcreteInt makeIntLocVal(const llvm::APSInt &integer) {
return loc::ConcreteInt(BasicVals.getValue(integer));
}

NonLoc makeIntVal(const llvm::APInt& integer, bool isUnsigned) {
return nonloc::ConcreteInt(BasicVals.getValue(integer, isUnsigned));
}

DefinedSVal makeIntVal(uint64_t integer, QualType type) {
if (Loc::isLocType(type))
return loc::ConcreteInt(BasicVals.getValue(integer, type));

return nonloc::ConcreteInt(BasicVals.getValue(integer, type));
}

NonLoc makeIntVal(uint64_t integer, bool isUnsigned) {
return nonloc::ConcreteInt(BasicVals.getIntValue(integer, isUnsigned));
}

NonLoc makeIntValWithPtrWidth(uint64_t integer, bool isUnsigned) {
return nonloc::ConcreteInt(
BasicVals.getIntWithPtrWidth(integer, isUnsigned));
}

NonLoc makeLocAsInteger(Loc loc, unsigned bits) {
return nonloc::LocAsInteger(BasicVals.getPersistentSValWithData(loc, bits));
}

NonLoc makeNonLoc(const SymExpr *lhs, BinaryOperator::Opcode op,
const llvm::APSInt& rhs, QualType type);

NonLoc makeNonLoc(const llvm::APSInt& rhs, BinaryOperator::Opcode op,
const SymExpr *lhs, QualType type);

NonLoc makeNonLoc(const SymExpr *lhs, BinaryOperator::Opcode op,
const SymExpr *rhs, QualType type);


NonLoc makeNonLoc(const SymExpr *operand, QualType fromTy, QualType toTy);

nonloc::ConcreteInt makeTruthVal(bool b, QualType type) {
return nonloc::ConcreteInt(BasicVals.getTruthValue(b, type));
}

nonloc::ConcreteInt makeTruthVal(bool b) {
return nonloc::ConcreteInt(BasicVals.getTruthValue(b));
}




Loc makeNullWithType(QualType type) {
return loc::ConcreteInt(BasicVals.getZeroWithTypeSize(type));
}

Loc makeNull() {
return loc::ConcreteInt(BasicVals.getZeroWithPtrWidth());
}

Loc makeLoc(SymbolRef sym) {
return loc::MemRegionVal(MemMgr.getSymbolicRegion(sym));
}

Loc makeLoc(const MemRegion* region) {
return loc::MemRegionVal(region);
}

Loc makeLoc(const AddrLabelExpr *expr) {
return loc::GotoLabel(expr->getLabel());
}

Loc makeLoc(const llvm::APSInt& integer) {
return loc::ConcreteInt(BasicVals.getValue(integer));
}


Optional<loc::MemRegionVal> getCastedMemRegionVal(const MemRegion *region,
QualType type);




SVal makeSymbolVal(SymbolRef Sym) {
if (Loc::isLocType(Sym->getType()))
return makeLoc(Sym);
return nonloc::SymbolVal(Sym);
}


loc::MemRegionVal getCXXThis(const CXXMethodDecl *D,
const StackFrameContext *SFC);


loc::MemRegionVal getCXXThis(const CXXRecordDecl *D,
const StackFrameContext *SFC);
};

SValBuilder* createSimpleSValBuilder(llvm::BumpPtrAllocator &alloc,
ASTContext &context,
ProgramStateManager &stateMgr);

}

}

#endif
