











#if !defined(LLVM_CLANG_AST_INTERP_INTERP_H)
#define LLVM_CLANG_AST_INTERP_INTERP_H

#include <limits>
#include <vector>
#include "Function.h"
#include "InterpFrame.h"
#include "InterpStack.h"
#include "InterpState.h"
#include "Opcode.h"
#include "PrimType.h"
#include "Program.h"
#include "State.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTDiagnostic.h"
#include "clang/AST/CXXInheritance.h"
#include "clang/AST/Expr.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/Support/Endian.h"

namespace clang {
namespace interp {

using APInt = llvm::APInt;
using APSInt = llvm::APSInt;


template <typename T> bool ReturnValue(const T &V, APValue &R) {
R = V.toAPValue();
return true;
}


bool CheckExtern(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckArray(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckLive(InterpState &S, CodePtr OpPC, const Pointer &Ptr,
AccessKinds AK);

bool CheckNull(InterpState &S, CodePtr OpPC, const Pointer &Ptr,
CheckSubobjectKind CSK);


bool CheckRange(InterpState &S, CodePtr OpPC, const Pointer &Ptr,
AccessKinds AK);


bool CheckRange(InterpState &S, CodePtr OpPC, const Pointer &Ptr,
CheckSubobjectKind CSK);


bool CheckConst(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckMutable(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckLoad(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckStore(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckInvoke(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckInit(InterpState &S, CodePtr OpPC, const Pointer &Ptr);


bool CheckCallable(InterpState &S, CodePtr OpPC, Function *F);


bool CheckThis(InterpState &S, CodePtr OpPC, const Pointer &This);


bool CheckPure(InterpState &S, CodePtr OpPC, const CXXMethodDecl *MD);

template <typename T> inline bool IsTrue(const T &V) { return !V.isZero(); }





template <typename T, bool (*OpFW)(T, T, unsigned, T *),
template <typename U> class OpAP>
bool AddSubMulHelper(InterpState &S, CodePtr OpPC, unsigned Bits, const T &LHS,
const T &RHS) {

T Result;
if (!OpFW(LHS, RHS, Bits, &Result)) {
S.Stk.push<T>(Result);
return true;
}


S.Stk.push<T>(Result);


APSInt Value = OpAP<APSInt>()(LHS.toAPSInt(Bits), RHS.toAPSInt(Bits));


const Expr *E = S.Current->getExpr(OpPC);
QualType Type = E->getType();
if (S.checkingForUndefinedBehavior()) {
SmallString<32> Trunc;
Value.trunc(Result.bitWidth()).toString(Trunc, 10);
auto Loc = E->getExprLoc();
S.report(Loc, diag::warn_integer_constant_overflow) << Trunc << Type;
return true;
} else {
S.CCEDiag(E, diag::note_constexpr_overflow) << Value << Type;
return S.noteUndefinedBehavior();
}
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Add(InterpState &S, CodePtr OpPC) {
const T &RHS = S.Stk.pop<T>();
const T &LHS = S.Stk.pop<T>();
const unsigned Bits = RHS.bitWidth() + 1;
return AddSubMulHelper<T, T::add, std::plus>(S, OpPC, Bits, LHS, RHS);
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Sub(InterpState &S, CodePtr OpPC) {
const T &RHS = S.Stk.pop<T>();
const T &LHS = S.Stk.pop<T>();
const unsigned Bits = RHS.bitWidth() + 1;
return AddSubMulHelper<T, T::sub, std::minus>(S, OpPC, Bits, LHS, RHS);
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Mul(InterpState &S, CodePtr OpPC) {
const T &RHS = S.Stk.pop<T>();
const T &LHS = S.Stk.pop<T>();
const unsigned Bits = RHS.bitWidth() * 2;
return AddSubMulHelper<T, T::mul, std::multiplies>(S, OpPC, Bits, LHS, RHS);
}





using CompareFn = llvm::function_ref<bool(ComparisonCategoryResult)>;

template <typename T>
bool CmpHelper(InterpState &S, CodePtr OpPC, CompareFn Fn) {
using BoolT = PrimConv<PT_Bool>::T;
const T &RHS = S.Stk.pop<T>();
const T &LHS = S.Stk.pop<T>();
S.Stk.push<BoolT>(BoolT::from(Fn(LHS.compare(RHS))));
return true;
}

template <typename T>
bool CmpHelperEQ(InterpState &S, CodePtr OpPC, CompareFn Fn) {
return CmpHelper<T>(S, OpPC, Fn);
}

template <>
inline bool CmpHelper<Pointer>(InterpState &S, CodePtr OpPC, CompareFn Fn) {
using BoolT = PrimConv<PT_Bool>::T;
const Pointer &RHS = S.Stk.pop<Pointer>();
const Pointer &LHS = S.Stk.pop<Pointer>();

if (!Pointer::hasSameBase(LHS, RHS)) {
const SourceInfo &Loc = S.Current->getSource(OpPC);
S.FFDiag(Loc, diag::note_invalid_subexpr_in_const_expr);
return false;
} else {
unsigned VL = LHS.getByteOffset();
unsigned VR = RHS.getByteOffset();
S.Stk.push<BoolT>(BoolT::from(Fn(Compare(VL, VR))));
return true;
}
}

template <>
inline bool CmpHelperEQ<Pointer>(InterpState &S, CodePtr OpPC, CompareFn Fn) {
using BoolT = PrimConv<PT_Bool>::T;
const Pointer &RHS = S.Stk.pop<Pointer>();
const Pointer &LHS = S.Stk.pop<Pointer>();

if (LHS.isZero() && RHS.isZero()) {
S.Stk.push<BoolT>(BoolT::from(Fn(ComparisonCategoryResult::Equal)));
return true;
}

if (!Pointer::hasSameBase(LHS, RHS)) {
S.Stk.push<BoolT>(BoolT::from(Fn(ComparisonCategoryResult::Unordered)));
return true;
} else {
unsigned VL = LHS.getByteOffset();
unsigned VR = RHS.getByteOffset();
S.Stk.push<BoolT>(BoolT::from(Fn(Compare(VL, VR))));
return true;
}
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool EQ(InterpState &S, CodePtr OpPC) {
return CmpHelperEQ<T>(S, OpPC, [](ComparisonCategoryResult R) {
return R == ComparisonCategoryResult::Equal;
});
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool NE(InterpState &S, CodePtr OpPC) {
return CmpHelperEQ<T>(S, OpPC, [](ComparisonCategoryResult R) {
return R != ComparisonCategoryResult::Equal;
});
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool LT(InterpState &S, CodePtr OpPC) {
return CmpHelper<T>(S, OpPC, [](ComparisonCategoryResult R) {
return R == ComparisonCategoryResult::Less;
});
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool LE(InterpState &S, CodePtr OpPC) {
return CmpHelper<T>(S, OpPC, [](ComparisonCategoryResult R) {
return R == ComparisonCategoryResult::Less ||
R == ComparisonCategoryResult::Equal;
});
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GT(InterpState &S, CodePtr OpPC) {
return CmpHelper<T>(S, OpPC, [](ComparisonCategoryResult R) {
return R == ComparisonCategoryResult::Greater;
});
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GE(InterpState &S, CodePtr OpPC) {
return CmpHelper<T>(S, OpPC, [](ComparisonCategoryResult R) {
return R == ComparisonCategoryResult::Greater ||
R == ComparisonCategoryResult::Equal;
});
}





template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InRange(InterpState &S, CodePtr OpPC) {
const T RHS = S.Stk.pop<T>();
const T LHS = S.Stk.pop<T>();
const T Value = S.Stk.pop<T>();

S.Stk.push<bool>(LHS <= Value && Value <= RHS);
return true;
}





template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Dup(InterpState &S, CodePtr OpPC) {
S.Stk.push<T>(S.Stk.peek<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Pop(InterpState &S, CodePtr OpPC) {
S.Stk.pop<T>();
return true;
}





template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Const(InterpState &S, CodePtr OpPC, const T &Arg) {
S.Stk.push<T>(Arg);
return true;
}





template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GetLocal(InterpState &S, CodePtr OpPC, uint32_t I) {
S.Stk.push<T>(S.Current->getLocal<T>(I));
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool SetLocal(InterpState &S, CodePtr OpPC, uint32_t I) {
S.Current->setLocal<T>(I, S.Stk.pop<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GetParam(InterpState &S, CodePtr OpPC, uint32_t I) {
if (S.checkingPotentialConstantExpression()) {
return false;
}
S.Stk.push<T>(S.Current->getParam<T>(I));
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool SetParam(InterpState &S, CodePtr OpPC, uint32_t I) {
S.Current->setParam<T>(I, S.Stk.pop<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GetField(InterpState &S, CodePtr OpPC, uint32_t I) {
const Pointer &Obj = S.Stk.peek<Pointer>();
if (!CheckNull(S, OpPC, Obj, CSK_Field))
return false;
if (!CheckRange(S, OpPC, Obj, CSK_Field))
return false;
const Pointer &Field = Obj.atField(I);
if (!CheckLoad(S, OpPC, Field))
return false;
S.Stk.push<T>(Field.deref<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool SetField(InterpState &S, CodePtr OpPC, uint32_t I) {
const T &Value = S.Stk.pop<T>();
const Pointer &Obj = S.Stk.peek<Pointer>();
if (!CheckNull(S, OpPC, Obj, CSK_Field))
return false;
if (!CheckRange(S, OpPC, Obj, CSK_Field))
return false;
const Pointer &Field = Obj.atField(I);
if (!CheckStore(S, OpPC, Field))
return false;
Field.deref<T>() = Value;
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GetFieldPop(InterpState &S, CodePtr OpPC, uint32_t I) {
const Pointer &Obj = S.Stk.pop<Pointer>();
if (!CheckNull(S, OpPC, Obj, CSK_Field))
return false;
if (!CheckRange(S, OpPC, Obj, CSK_Field))
return false;
const Pointer &Field = Obj.atField(I);
if (!CheckLoad(S, OpPC, Field))
return false;
S.Stk.push<T>(Field.deref<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GetThisField(InterpState &S, CodePtr OpPC, uint32_t I) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
const Pointer &Field = This.atField(I);
if (!CheckLoad(S, OpPC, Field))
return false;
S.Stk.push<T>(Field.deref<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool SetThisField(InterpState &S, CodePtr OpPC, uint32_t I) {
if (S.checkingPotentialConstantExpression())
return false;
const T &Value = S.Stk.pop<T>();
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
const Pointer &Field = This.atField(I);
if (!CheckStore(S, OpPC, Field))
return false;
Field.deref<T>() = Value;
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool GetGlobal(InterpState &S, CodePtr OpPC, uint32_t I) {
auto *B = S.P.getGlobal(I);
if (B->isExtern())
return false;
S.Stk.push<T>(B->deref<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool SetGlobal(InterpState &S, CodePtr OpPC, uint32_t I) {

return false;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitGlobal(InterpState &S, CodePtr OpPC, uint32_t I) {
S.P.getGlobal(I)->deref<T>() = S.Stk.pop<T>();
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitThisField(InterpState &S, CodePtr OpPC, uint32_t I) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
const Pointer &Field = This.atField(I);
Field.deref<T>() = S.Stk.pop<T>();
Field.initialize();
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitThisBitField(InterpState &S, CodePtr OpPC, const Record::Field *F) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
const Pointer &Field = This.atField(F->Offset);
const auto &Value = S.Stk.pop<T>();
Field.deref<T>() = Value.truncate(F->Decl->getBitWidthValue(S.getCtx()));
Field.initialize();
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitThisFieldActive(InterpState &S, CodePtr OpPC, uint32_t I) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
const Pointer &Field = This.atField(I);
Field.deref<T>() = S.Stk.pop<T>();
Field.activate();
Field.initialize();
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitField(InterpState &S, CodePtr OpPC, uint32_t I) {
const T &Value = S.Stk.pop<T>();
const Pointer &Field = S.Stk.pop<Pointer>().atField(I);
Field.deref<T>() = Value;
Field.activate();
Field.initialize();
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitBitField(InterpState &S, CodePtr OpPC, const Record::Field *F) {
const T &Value = S.Stk.pop<T>();
const Pointer &Field = S.Stk.pop<Pointer>().atField(F->Offset);
Field.deref<T>() = Value.truncate(F->Decl->getBitWidthValue(S.getCtx()));
Field.activate();
Field.initialize();
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitFieldActive(InterpState &S, CodePtr OpPC, uint32_t I) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.pop<Pointer>();
const Pointer &Field = Ptr.atField(I);
Field.deref<T>() = Value;
Field.activate();
Field.initialize();
return true;
}





inline bool GetPtrLocal(InterpState &S, CodePtr OpPC, uint32_t I) {
S.Stk.push<Pointer>(S.Current->getLocalPointer(I));
return true;
}

inline bool GetPtrParam(InterpState &S, CodePtr OpPC, uint32_t I) {
if (S.checkingPotentialConstantExpression()) {
return false;
}
S.Stk.push<Pointer>(S.Current->getParamPointer(I));
return true;
}

inline bool GetPtrGlobal(InterpState &S, CodePtr OpPC, uint32_t I) {
S.Stk.push<Pointer>(S.P.getPtrGlobal(I));
return true;
}

inline bool GetPtrField(InterpState &S, CodePtr OpPC, uint32_t Off) {
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckNull(S, OpPC, Ptr, CSK_Field))
return false;
if (!CheckExtern(S, OpPC, Ptr))
return false;
if (!CheckRange(S, OpPC, Ptr, CSK_Field))
return false;
S.Stk.push<Pointer>(Ptr.atField(Off));
return true;
}

inline bool GetPtrThisField(InterpState &S, CodePtr OpPC, uint32_t Off) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
S.Stk.push<Pointer>(This.atField(Off));
return true;
}

inline bool GetPtrActiveField(InterpState &S, CodePtr OpPC, uint32_t Off) {
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckNull(S, OpPC, Ptr, CSK_Field))
return false;
if (!CheckRange(S, OpPC, Ptr, CSK_Field))
return false;
Pointer Field = Ptr.atField(Off);
Ptr.deactivate();
Field.activate();
S.Stk.push<Pointer>(std::move(Field));
return true;
}

inline bool GetPtrActiveThisField(InterpState &S, CodePtr OpPC, uint32_t Off) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
Pointer Field = This.atField(Off);
This.deactivate();
Field.activate();
S.Stk.push<Pointer>(std::move(Field));
return true;
}

inline bool GetPtrBase(InterpState &S, CodePtr OpPC, uint32_t Off) {
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckNull(S, OpPC, Ptr, CSK_Base))
return false;
S.Stk.push<Pointer>(Ptr.atField(Off));
return true;
}

inline bool GetPtrThisBase(InterpState &S, CodePtr OpPC, uint32_t Off) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
S.Stk.push<Pointer>(This.atField(Off));
return true;
}

inline bool VirtBaseHelper(InterpState &S, CodePtr OpPC, const RecordDecl *Decl,
const Pointer &Ptr) {
Pointer Base = Ptr;
while (Base.isBaseClass())
Base = Base.getBase();

auto *Field = Base.getRecord()->getVirtualBase(Decl);
S.Stk.push<Pointer>(Base.atField(Field->Offset));
return true;
}

inline bool GetPtrVirtBase(InterpState &S, CodePtr OpPC, const RecordDecl *D) {
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckNull(S, OpPC, Ptr, CSK_Base))
return false;
return VirtBaseHelper(S, OpPC, D, Ptr);
}

inline bool GetPtrThisVirtBase(InterpState &S, CodePtr OpPC,
const RecordDecl *D) {
if (S.checkingPotentialConstantExpression())
return false;
const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;
return VirtBaseHelper(S, OpPC, D, S.Current->getThis());
}





template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Load(InterpState &S, CodePtr OpPC) {
const Pointer &Ptr = S.Stk.peek<Pointer>();
if (!CheckLoad(S, OpPC, Ptr))
return false;
S.Stk.push<T>(Ptr.deref<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool LoadPop(InterpState &S, CodePtr OpPC) {
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckLoad(S, OpPC, Ptr))
return false;
S.Stk.push<T>(Ptr.deref<T>());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Store(InterpState &S, CodePtr OpPC) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.peek<Pointer>();
if (!CheckStore(S, OpPC, Ptr))
return false;
Ptr.deref<T>() = Value;
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool StorePop(InterpState &S, CodePtr OpPC) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckStore(S, OpPC, Ptr))
return false;
Ptr.deref<T>() = Value;
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool StoreBitField(InterpState &S, CodePtr OpPC) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.peek<Pointer>();
if (!CheckStore(S, OpPC, Ptr))
return false;
if (auto *FD = Ptr.getField()) {
Ptr.deref<T>() = Value.truncate(FD->getBitWidthValue(S.getCtx()));
} else {
Ptr.deref<T>() = Value;
}
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool StoreBitFieldPop(InterpState &S, CodePtr OpPC) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckStore(S, OpPC, Ptr))
return false;
if (auto *FD = Ptr.getField()) {
Ptr.deref<T>() = Value.truncate(FD->getBitWidthValue(S.getCtx()));
} else {
Ptr.deref<T>() = Value;
}
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitPop(InterpState &S, CodePtr OpPC) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckInit(S, OpPC, Ptr))
return false;
Ptr.initialize();
new (&Ptr.deref<T>()) T(Value);
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitElem(InterpState &S, CodePtr OpPC, uint32_t Idx) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.peek<Pointer>().atIndex(Idx);
if (!CheckInit(S, OpPC, Ptr))
return false;
Ptr.initialize();
new (&Ptr.deref<T>()) T(Value);
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool InitElemPop(InterpState &S, CodePtr OpPC, uint32_t Idx) {
const T &Value = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.pop<Pointer>().atIndex(Idx);
if (!CheckInit(S, OpPC, Ptr))
return false;
Ptr.initialize();
new (&Ptr.deref<T>()) T(Value);
return true;
}





template <class T, bool Add> bool OffsetHelper(InterpState &S, CodePtr OpPC) {

const T &Offset = S.Stk.pop<T>();
const Pointer &Ptr = S.Stk.pop<Pointer>();
if (!CheckNull(S, OpPC, Ptr, CSK_ArrayIndex))
return false;
if (!CheckRange(S, OpPC, Ptr, CSK_ArrayToPointer))
return false;


T Index = T::from(Ptr.getIndex(), Offset.bitWidth());


if (Offset.isZero()) {
S.Stk.push<Pointer>(Index.isZero() ? Ptr.atIndex(0) : Ptr);
return true;
}

if (!CheckArray(S, OpPC, Ptr))
return false;


unsigned MaxIndex = Ptr.getNumElems();


auto InvalidOffset = [&]() {
const unsigned Bits = Offset.bitWidth();
APSInt APOffset(Offset.toAPSInt().extend(Bits + 2), false);
APSInt APIndex(Index.toAPSInt().extend(Bits + 2), false);
APSInt NewIndex = Add ? (APIndex + APOffset) : (APIndex - APOffset);
S.CCEDiag(S.Current->getSource(OpPC), diag::note_constexpr_array_index)
<< NewIndex
<< static_cast<int>(!Ptr.inArray())
<< static_cast<unsigned>(MaxIndex);
return false;
};


if (Add && Offset.isNegative() && (Offset.isMin() || -Offset > Index))
return InvalidOffset();
if (!Add && Offset.isPositive() && Index < Offset)
return InvalidOffset();


unsigned MaxOffset = MaxIndex - Ptr.getIndex();
if (Add && Offset.isPositive() && Offset > MaxOffset)
return InvalidOffset();
if (!Add && Offset.isNegative() && (Offset.isMin() || -Offset > MaxOffset))
return InvalidOffset();


int64_t WideIndex = static_cast<int64_t>(Index);
int64_t WideOffset = static_cast<int64_t>(Offset);
int64_t Result = Add ? (WideIndex + WideOffset) : (WideIndex - WideOffset);
S.Stk.push<Pointer>(Ptr.atIndex(static_cast<unsigned>(Result)));
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool AddOffset(InterpState &S, CodePtr OpPC) {
return OffsetHelper<T, true>(S, OpPC);
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
bool SubOffset(InterpState &S, CodePtr OpPC) {
return OffsetHelper<T, false>(S, OpPC);
}






inline bool Destroy(InterpState &S, CodePtr OpPC, uint32_t I) {
S.Current->destroy(I);
return true;
}





template <PrimType TIn, PrimType TOut> bool Cast(InterpState &S, CodePtr OpPC) {
using T = typename PrimConv<TIn>::T;
using U = typename PrimConv<TOut>::T;
S.Stk.push<U>(U::from(S.Stk.pop<T>()));
return true;
}





template <PrimType Name, class T = typename PrimConv<Name>::T>
bool Zero(InterpState &S, CodePtr OpPC) {
S.Stk.push<T>(T::zero());
return true;
}

template <PrimType Name, class T = typename PrimConv<Name>::T>
inline bool Null(InterpState &S, CodePtr OpPC) {
S.Stk.push<T>();
return true;
}





inline bool This(InterpState &S, CodePtr OpPC) {

if (S.checkingPotentialConstantExpression()) {
return false;
}

const Pointer &This = S.Current->getThis();
if (!CheckThis(S, OpPC, This))
return false;

S.Stk.push<Pointer>(This);
return true;
}





template <PrimType TR, PrimType TL, class T = typename PrimConv<TR>::T>
unsigned Trunc(InterpState &S, CodePtr OpPC, unsigned Bits, const T &V) {


if (Bits > 1 && V >= T::from(Bits, V.bitWidth())) {
const Expr *E = S.Current->getExpr(OpPC);
const APSInt Val = V.toAPSInt();
QualType Ty = E->getType();
S.CCEDiag(E, diag::note_constexpr_large_shift) << Val << Ty << Bits;
return Bits;
} else {
return static_cast<unsigned>(V);
}
}

template <PrimType TL, PrimType TR, typename T = typename PrimConv<TL>::T>
inline bool ShiftRight(InterpState &S, CodePtr OpPC, const T &V, unsigned RHS) {
if (RHS >= V.bitWidth()) {
S.Stk.push<T>(T::from(0, V.bitWidth()));
} else {
S.Stk.push<T>(T::from(V >> RHS, V.bitWidth()));
}
return true;
}

template <PrimType TL, PrimType TR, typename T = typename PrimConv<TL>::T>
inline bool ShiftLeft(InterpState &S, CodePtr OpPC, const T &V, unsigned RHS) {
if (V.isSigned() && !S.getLangOpts().CPlusPlus20) {




if (V.isNegative()) {
const Expr *E = S.Current->getExpr(OpPC);
S.CCEDiag(E, diag::note_constexpr_lshift_of_negative) << V.toAPSInt();
} else if (V.countLeadingZeros() < RHS) {
S.CCEDiag(S.Current->getExpr(OpPC), diag::note_constexpr_lshift_discards);
}
}

if (V.bitWidth() == 1) {
S.Stk.push<T>(V);
} else if (RHS >= V.bitWidth()) {
S.Stk.push<T>(T::from(0, V.bitWidth()));
} else {
S.Stk.push<T>(T::from(V.toUnsigned() << RHS, V.bitWidth()));
}
return true;
}

template <PrimType TL, PrimType TR>
inline bool Shr(InterpState &S, CodePtr OpPC) {
const auto &RHS = S.Stk.pop<typename PrimConv<TR>::T>();
const auto &LHS = S.Stk.pop<typename PrimConv<TL>::T>();
const unsigned Bits = LHS.bitWidth();

if (RHS.isSigned() && RHS.isNegative()) {
const SourceInfo &Loc = S.Current->getSource(OpPC);
S.CCEDiag(Loc, diag::note_constexpr_negative_shift) << RHS.toAPSInt();
return ShiftLeft<TL, TR>(S, OpPC, LHS, Trunc<TR, TL>(S, OpPC, Bits, -RHS));
} else {
return ShiftRight<TL, TR>(S, OpPC, LHS, Trunc<TR, TL>(S, OpPC, Bits, RHS));
}
}

template <PrimType TL, PrimType TR>
inline bool Shl(InterpState &S, CodePtr OpPC) {
const auto &RHS = S.Stk.pop<typename PrimConv<TR>::T>();
const auto &LHS = S.Stk.pop<typename PrimConv<TL>::T>();
const unsigned Bits = LHS.bitWidth();

if (RHS.isSigned() && RHS.isNegative()) {
const SourceInfo &Loc = S.Current->getSource(OpPC);
S.CCEDiag(Loc, diag::note_constexpr_negative_shift) << RHS.toAPSInt();
return ShiftRight<TL, TR>(S, OpPC, LHS, Trunc<TR, TL>(S, OpPC, Bits, -RHS));
} else {
return ShiftLeft<TL, TR>(S, OpPC, LHS, Trunc<TR, TL>(S, OpPC, Bits, RHS));
}
}





inline bool NoRet(InterpState &S, CodePtr OpPC) {
SourceLocation EndLoc = S.Current->getCallee()->getEndLoc();
S.FFDiag(EndLoc, diag::note_constexpr_no_return);
return false;
}





inline bool NarrowPtr(InterpState &S, CodePtr OpPC) {
const Pointer &Ptr = S.Stk.pop<Pointer>();
S.Stk.push<Pointer>(Ptr.narrow());
return true;
}

inline bool ExpandPtr(InterpState &S, CodePtr OpPC) {
const Pointer &Ptr = S.Stk.pop<Pointer>();
S.Stk.push<Pointer>(Ptr.expand());
return true;
}


bool Interpret(InterpState &S, APValue &Result);

}
}

#endif
