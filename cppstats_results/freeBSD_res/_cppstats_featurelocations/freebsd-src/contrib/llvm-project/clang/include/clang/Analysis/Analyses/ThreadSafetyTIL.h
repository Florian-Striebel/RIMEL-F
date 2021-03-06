












































#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETYTIL_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETYTIL_H

#include "clang/AST/Decl.h"
#include "clang/Analysis/Analyses/ThreadSafetyUtil.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <utility>

namespace clang {

class CallExpr;
class Expr;
class Stmt;

namespace threadSafety {
namespace til {

class BasicBlock;


enum TIL_Opcode {
#define TIL_OPCODE_DEF(X) COP_##X,
#include "ThreadSafetyOps.def"
#undef TIL_OPCODE_DEF
};


enum TIL_UnaryOpcode : unsigned char {
UOP_Minus,
UOP_BitNot,
UOP_LogicNot
};


enum TIL_BinaryOpcode : unsigned char {
BOP_Add,
BOP_Sub,
BOP_Mul,
BOP_Div,
BOP_Rem,
BOP_Shl,
BOP_Shr,
BOP_BitAnd,
BOP_BitXor,
BOP_BitOr,
BOP_Eq,
BOP_Neq,
BOP_Lt,
BOP_Leq,
BOP_Cmp,
BOP_LogicAnd,
BOP_LogicOr
};


enum TIL_CastOpcode : unsigned char {
CAST_none = 0,


CAST_extendNum,


CAST_truncNum,


CAST_toFloat,


CAST_toInt,


CAST_objToPtr
};

const TIL_Opcode COP_Min = COP_Future;
const TIL_Opcode COP_Max = COP_Branch;
const TIL_UnaryOpcode UOP_Min = UOP_Minus;
const TIL_UnaryOpcode UOP_Max = UOP_LogicNot;
const TIL_BinaryOpcode BOP_Min = BOP_Add;
const TIL_BinaryOpcode BOP_Max = BOP_LogicOr;
const TIL_CastOpcode CAST_Min = CAST_none;
const TIL_CastOpcode CAST_Max = CAST_toInt;


StringRef getUnaryOpcodeString(TIL_UnaryOpcode Op);


StringRef getBinaryOpcodeString(TIL_BinaryOpcode Op);







struct ValueType {
enum BaseType : unsigned char {
BT_Void = 0,
BT_Bool,
BT_Int,
BT_Float,
BT_String,
BT_Pointer,
BT_ValueRef
};

enum SizeType : unsigned char {
ST_0 = 0,
ST_1,
ST_8,
ST_16,
ST_32,
ST_64,
ST_128
};

ValueType(BaseType B, SizeType Sz, bool S, unsigned char VS)
: Base(B), Size(Sz), Signed(S), VectSize(VS) {}

inline static SizeType getSizeType(unsigned nbytes);

template <class T>
inline static ValueType getValueType();

BaseType Base;
SizeType Size;
bool Signed;


unsigned char VectSize;
};

inline ValueType::SizeType ValueType::getSizeType(unsigned nbytes) {
switch (nbytes) {
case 1: return ST_8;
case 2: return ST_16;
case 4: return ST_32;
case 8: return ST_64;
case 16: return ST_128;
default: return ST_0;
}
}

template<>
inline ValueType ValueType::getValueType<void>() {
return ValueType(BT_Void, ST_0, false, 0);
}

template<>
inline ValueType ValueType::getValueType<bool>() {
return ValueType(BT_Bool, ST_1, false, 0);
}

template<>
inline ValueType ValueType::getValueType<int8_t>() {
return ValueType(BT_Int, ST_8, true, 0);
}

template<>
inline ValueType ValueType::getValueType<uint8_t>() {
return ValueType(BT_Int, ST_8, false, 0);
}

template<>
inline ValueType ValueType::getValueType<int16_t>() {
return ValueType(BT_Int, ST_16, true, 0);
}

template<>
inline ValueType ValueType::getValueType<uint16_t>() {
return ValueType(BT_Int, ST_16, false, 0);
}

template<>
inline ValueType ValueType::getValueType<int32_t>() {
return ValueType(BT_Int, ST_32, true, 0);
}

template<>
inline ValueType ValueType::getValueType<uint32_t>() {
return ValueType(BT_Int, ST_32, false, 0);
}

template<>
inline ValueType ValueType::getValueType<int64_t>() {
return ValueType(BT_Int, ST_64, true, 0);
}

template<>
inline ValueType ValueType::getValueType<uint64_t>() {
return ValueType(BT_Int, ST_64, false, 0);
}

template<>
inline ValueType ValueType::getValueType<float>() {
return ValueType(BT_Float, ST_32, true, 0);
}

template<>
inline ValueType ValueType::getValueType<double>() {
return ValueType(BT_Float, ST_64, true, 0);
}

template<>
inline ValueType ValueType::getValueType<long double>() {
return ValueType(BT_Float, ST_128, true, 0);
}

template<>
inline ValueType ValueType::getValueType<StringRef>() {
return ValueType(BT_String, getSizeType(sizeof(StringRef)), false, 0);
}

template<>
inline ValueType ValueType::getValueType<void*>() {
return ValueType(BT_Pointer, getSizeType(sizeof(void*)), false, 0);
}


class SExpr {
public:
SExpr() = delete;

TIL_Opcode opcode() const { return static_cast<TIL_Opcode>(Opcode); }















void *operator new(size_t S, MemRegionRef &R) {
return ::operator new(S, R);
}


void *operator new(size_t) = delete;




void operator delete(void *) = delete;



unsigned id() const { return SExprID; }



BasicBlock *block() const { return Block; }


void setID(BasicBlock *B, unsigned id) { Block = B; SExprID = id; }

protected:
SExpr(TIL_Opcode Op) : Opcode(Op) {}
SExpr(const SExpr &E) : Opcode(E.Opcode), Flags(E.Flags) {}

const unsigned char Opcode;
unsigned char Reserved = 0;
unsigned short Flags = 0;
unsigned SExprID = 0;
BasicBlock *Block = nullptr;
};


namespace ThreadSafetyTIL {

inline bool isTrivial(const SExpr *E) {
unsigned Op = E->opcode();
return Op == COP_Variable || Op == COP_Literal || Op == COP_LiteralPtr;
}

}















class Variable : public SExpr {
public:
enum VariableKind {

VK_Let,


VK_Fun,


VK_SFun
};

Variable(StringRef s, SExpr *D = nullptr)
: SExpr(COP_Variable), Name(s), Definition(D) {
Flags = VK_Let;
}

Variable(SExpr *D, const ValueDecl *Cvd = nullptr)
: SExpr(COP_Variable), Name(Cvd ? Cvd->getName() : "_x"),
Definition(D), Cvdecl(Cvd) {
Flags = VK_Let;
}

Variable(const Variable &Vd, SExpr *D)
: SExpr(Vd), Name(Vd.Name), Definition(D), Cvdecl(Vd.Cvdecl) {
Flags = Vd.kind();
}

static bool classof(const SExpr *E) { return E->opcode() == COP_Variable; }


VariableKind kind() const { return static_cast<VariableKind>(Flags); }


StringRef name() const { return Name; }


const ValueDecl *clangDecl() const { return Cvdecl; }




SExpr *definition() { return Definition; }
const SExpr *definition() const { return Definition; }

void setName(StringRef S) { Name = S; }
void setKind(VariableKind K) { Flags = K; }
void setDefinition(SExpr *E) { Definition = E; }
void setClangDecl(const ValueDecl *VD) { Cvdecl = VD; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {

return Vs.reduceVariableRef(this);
}

template <class C>
typename C::CType compare(const Variable* E, C& Cmp) const {
return Cmp.compareVariableRefs(this, E);
}

private:
friend class BasicBlock;
friend class Function;
friend class Let;
friend class SFunction;


StringRef Name;


SExpr *Definition;


const ValueDecl *Cvdecl = nullptr;
};



class Future : public SExpr {
public:
enum FutureStatus {
FS_pending,
FS_evaluating,
FS_done
};

Future() : SExpr(COP_Future) {}
virtual ~Future() = delete;

static bool classof(const SExpr *E) { return E->opcode() == COP_Future; }


virtual SExpr *compute() { return nullptr; }


SExpr *maybeGetResult() const { return Result; }


SExpr *result() {
switch (Status) {
case FS_pending:
return force();
case FS_evaluating:
return nullptr;
case FS_done:
return Result;
}
}

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
assert(Result && "Cannot traverse Future that has not been forced.");
return Vs.traverse(Result, Ctx);
}

template <class C>
typename C::CType compare(const Future* E, C& Cmp) const {
if (!Result || !E->Result)
return Cmp.comparePointers(this, E);
return Cmp.compare(Result, E->Result);
}

private:
SExpr* force();

FutureStatus Status = FS_pending;
SExpr *Result = nullptr;
};


class Undefined : public SExpr {
public:
Undefined(const Stmt *S = nullptr) : SExpr(COP_Undefined), Cstmt(S) {}
Undefined(const Undefined &U) : SExpr(U), Cstmt(U.Cstmt) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Undefined; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
return Vs.reduceUndefined(*this);
}

template <class C>
typename C::CType compare(const Undefined* E, C& Cmp) const {
return Cmp.trueResult();
}

private:
const Stmt *Cstmt;
};


class Wildcard : public SExpr {
public:
Wildcard() : SExpr(COP_Wildcard) {}
Wildcard(const Wildcard &) = default;

static bool classof(const SExpr *E) { return E->opcode() == COP_Wildcard; }

template <class V> typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
return Vs.reduceWildcard(*this);
}

template <class C>
typename C::CType compare(const Wildcard* E, C& Cmp) const {
return Cmp.trueResult();
}
};

template <class T> class LiteralT;


class Literal : public SExpr {
public:
Literal(const Expr *C)
: SExpr(COP_Literal), ValType(ValueType::getValueType<void>()), Cexpr(C) {}
Literal(ValueType VT) : SExpr(COP_Literal), ValType(VT) {}
Literal(const Literal &) = default;

static bool classof(const SExpr *E) { return E->opcode() == COP_Literal; }


const Expr *clangExpr() const { return Cexpr; }

ValueType valueType() const { return ValType; }

template<class T> const LiteralT<T>& as() const {
return *static_cast<const LiteralT<T>*>(this);
}
template<class T> LiteralT<T>& as() {
return *static_cast<LiteralT<T>*>(this);
}

template <class V> typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx);

template <class C>
typename C::CType compare(const Literal* E, C& Cmp) const {

return Cmp.trueResult();
}

private:
const ValueType ValType;
const Expr *Cexpr = nullptr;
};


template<class T>
class LiteralT : public Literal {
public:
LiteralT(T Dat) : Literal(ValueType::getValueType<T>()), Val(Dat) {}
LiteralT(const LiteralT<T> &L) : Literal(L), Val(L.Val) {}

T value() const { return Val;}
T& value() { return Val; }

private:
T Val;
};

template <class V>
typename V::R_SExpr Literal::traverse(V &Vs, typename V::R_Ctx Ctx) {
if (Cexpr)
return Vs.reduceLiteral(*this);

switch (ValType.Base) {
case ValueType::BT_Void:
break;
case ValueType::BT_Bool:
return Vs.reduceLiteralT(as<bool>());
case ValueType::BT_Int: {
switch (ValType.Size) {
case ValueType::ST_8:
if (ValType.Signed)
return Vs.reduceLiteralT(as<int8_t>());
else
return Vs.reduceLiteralT(as<uint8_t>());
case ValueType::ST_16:
if (ValType.Signed)
return Vs.reduceLiteralT(as<int16_t>());
else
return Vs.reduceLiteralT(as<uint16_t>());
case ValueType::ST_32:
if (ValType.Signed)
return Vs.reduceLiteralT(as<int32_t>());
else
return Vs.reduceLiteralT(as<uint32_t>());
case ValueType::ST_64:
if (ValType.Signed)
return Vs.reduceLiteralT(as<int64_t>());
else
return Vs.reduceLiteralT(as<uint64_t>());
default:
break;
}
}
case ValueType::BT_Float: {
switch (ValType.Size) {
case ValueType::ST_32:
return Vs.reduceLiteralT(as<float>());
case ValueType::ST_64:
return Vs.reduceLiteralT(as<double>());
default:
break;
}
}
case ValueType::BT_String:
return Vs.reduceLiteralT(as<StringRef>());
case ValueType::BT_Pointer:
return Vs.reduceLiteralT(as<void*>());
case ValueType::BT_ValueRef:
break;
}
return Vs.reduceLiteral(*this);
}



class LiteralPtr : public SExpr {
public:
LiteralPtr(const ValueDecl *D) : SExpr(COP_LiteralPtr), Cvdecl(D) {
assert(D && "ValueDecl must not be null");
}
LiteralPtr(const LiteralPtr &) = default;

static bool classof(const SExpr *E) { return E->opcode() == COP_LiteralPtr; }


const ValueDecl *clangDecl() const { return Cvdecl; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
return Vs.reduceLiteralPtr(*this);
}

template <class C>
typename C::CType compare(const LiteralPtr* E, C& Cmp) const {
return Cmp.comparePointers(Cvdecl, E->Cvdecl);
}

private:
const ValueDecl *Cvdecl;
};




class Function : public SExpr {
public:
Function(Variable *Vd, SExpr *Bd)
: SExpr(COP_Function), VarDecl(Vd), Body(Bd) {
Vd->setKind(Variable::VK_Fun);
}

Function(const Function &F, Variable *Vd, SExpr *Bd)
: SExpr(F), VarDecl(Vd), Body(Bd) {
Vd->setKind(Variable::VK_Fun);
}

static bool classof(const SExpr *E) { return E->opcode() == COP_Function; }

Variable *variableDecl() { return VarDecl; }
const Variable *variableDecl() const { return VarDecl; }

SExpr *body() { return Body; }
const SExpr *body() const { return Body; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {

auto E0 = Vs.traverse(VarDecl->Definition, Vs.typeCtx(Ctx));

Variable *Nvd = Vs.enterScope(*VarDecl, E0);
auto E1 = Vs.traverse(Body, Vs.declCtx(Ctx));
Vs.exitScope(*VarDecl);
return Vs.reduceFunction(*this, Nvd, E1);
}

template <class C>
typename C::CType compare(const Function* E, C& Cmp) const {
typename C::CType Ct =
Cmp.compare(VarDecl->definition(), E->VarDecl->definition());
if (Cmp.notTrue(Ct))
return Ct;
Cmp.enterScope(variableDecl(), E->variableDecl());
Ct = Cmp.compare(body(), E->body());
Cmp.leaveScope();
return Ct;
}

private:
Variable *VarDecl;
SExpr* Body;
};




class SFunction : public SExpr {
public:
SFunction(Variable *Vd, SExpr *B)
: SExpr(COP_SFunction), VarDecl(Vd), Body(B) {
assert(Vd->Definition == nullptr);
Vd->setKind(Variable::VK_SFun);
Vd->Definition = this;
}

SFunction(const SFunction &F, Variable *Vd, SExpr *B)
: SExpr(F), VarDecl(Vd), Body(B) {
assert(Vd->Definition == nullptr);
Vd->setKind(Variable::VK_SFun);
Vd->Definition = this;
}

static bool classof(const SExpr *E) { return E->opcode() == COP_SFunction; }

Variable *variableDecl() { return VarDecl; }
const Variable *variableDecl() const { return VarDecl; }

SExpr *body() { return Body; }
const SExpr *body() const { return Body; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {



Variable *Nvd = Vs.enterScope(*VarDecl, nullptr);
auto E1 = Vs.traverse(Body, Vs.declCtx(Ctx));
Vs.exitScope(*VarDecl);

return Vs.reduceSFunction(*this, Nvd, E1);
}

template <class C>
typename C::CType compare(const SFunction* E, C& Cmp) const {
Cmp.enterScope(variableDecl(), E->variableDecl());
typename C::CType Ct = Cmp.compare(body(), E->body());
Cmp.leaveScope();
return Ct;
}

private:
Variable *VarDecl;
SExpr* Body;
};


class Code : public SExpr {
public:
Code(SExpr *T, SExpr *B) : SExpr(COP_Code), ReturnType(T), Body(B) {}
Code(const Code &C, SExpr *T, SExpr *B)
: SExpr(C), ReturnType(T), Body(B) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Code; }

SExpr *returnType() { return ReturnType; }
const SExpr *returnType() const { return ReturnType; }

SExpr *body() { return Body; }
const SExpr *body() const { return Body; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nt = Vs.traverse(ReturnType, Vs.typeCtx(Ctx));
auto Nb = Vs.traverse(Body, Vs.lazyCtx(Ctx));
return Vs.reduceCode(*this, Nt, Nb);
}

template <class C>
typename C::CType compare(const Code* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(returnType(), E->returnType());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(body(), E->body());
}

private:
SExpr* ReturnType;
SExpr* Body;
};


class Field : public SExpr {
public:
Field(SExpr *R, SExpr *B) : SExpr(COP_Field), Range(R), Body(B) {}
Field(const Field &C, SExpr *R, SExpr *B)
: SExpr(C), Range(R), Body(B) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Field; }

SExpr *range() { return Range; }
const SExpr *range() const { return Range; }

SExpr *body() { return Body; }
const SExpr *body() const { return Body; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nr = Vs.traverse(Range, Vs.typeCtx(Ctx));
auto Nb = Vs.traverse(Body, Vs.lazyCtx(Ctx));
return Vs.reduceField(*this, Nr, Nb);
}

template <class C>
typename C::CType compare(const Field* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(range(), E->range());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(body(), E->body());
}

private:
SExpr* Range;
SExpr* Body;
};






class Apply : public SExpr {
public:
Apply(SExpr *F, SExpr *A) : SExpr(COP_Apply), Fun(F), Arg(A) {}
Apply(const Apply &A, SExpr *F, SExpr *Ar)
: SExpr(A), Fun(F), Arg(Ar) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Apply; }

SExpr *fun() { return Fun; }
const SExpr *fun() const { return Fun; }

SExpr *arg() { return Arg; }
const SExpr *arg() const { return Arg; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nf = Vs.traverse(Fun, Vs.subExprCtx(Ctx));
auto Na = Vs.traverse(Arg, Vs.subExprCtx(Ctx));
return Vs.reduceApply(*this, Nf, Na);
}

template <class C>
typename C::CType compare(const Apply* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(fun(), E->fun());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(arg(), E->arg());
}

private:
SExpr* Fun;
SExpr* Arg;
};


class SApply : public SExpr {
public:
SApply(SExpr *Sf, SExpr *A = nullptr) : SExpr(COP_SApply), Sfun(Sf), Arg(A) {}
SApply(SApply &A, SExpr *Sf, SExpr *Ar = nullptr)
: SExpr(A), Sfun(Sf), Arg(Ar) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_SApply; }

SExpr *sfun() { return Sfun; }
const SExpr *sfun() const { return Sfun; }

SExpr *arg() { return Arg ? Arg : Sfun; }
const SExpr *arg() const { return Arg ? Arg : Sfun; }

bool isDelegation() const { return Arg != nullptr; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nf = Vs.traverse(Sfun, Vs.subExprCtx(Ctx));
typename V::R_SExpr Na = Arg ? Vs.traverse(Arg, Vs.subExprCtx(Ctx))
: nullptr;
return Vs.reduceSApply(*this, Nf, Na);
}

template <class C>
typename C::CType compare(const SApply* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(sfun(), E->sfun());
if (Cmp.notTrue(Ct) || (!arg() && !E->arg()))
return Ct;
return Cmp.compare(arg(), E->arg());
}

private:
SExpr* Sfun;
SExpr* Arg;
};


class Project : public SExpr {
public:
Project(SExpr *R, const ValueDecl *Cvd)
: SExpr(COP_Project), Rec(R), Cvdecl(Cvd) {
assert(Cvd && "ValueDecl must not be null");
}

static bool classof(const SExpr *E) { return E->opcode() == COP_Project; }

SExpr *record() { return Rec; }
const SExpr *record() const { return Rec; }

const ValueDecl *clangDecl() const { return Cvdecl; }

bool isArrow() const { return (Flags & 0x01) != 0; }

void setArrow(bool b) {
if (b) Flags |= 0x01;
else Flags &= 0xFFFE;
}

StringRef slotName() const {
if (Cvdecl->getDeclName().isIdentifier())
return Cvdecl->getName();
if (!SlotName) {
SlotName = "";
llvm::raw_string_ostream OS(*SlotName);
Cvdecl->printName(OS);
}
return *SlotName;
}

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nr = Vs.traverse(Rec, Vs.subExprCtx(Ctx));
return Vs.reduceProject(*this, Nr);
}

template <class C>
typename C::CType compare(const Project* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(record(), E->record());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.comparePointers(Cvdecl, E->Cvdecl);
}

private:
SExpr* Rec;
mutable llvm::Optional<std::string> SlotName;
const ValueDecl *Cvdecl;
};


class Call : public SExpr {
public:
Call(SExpr *T, const CallExpr *Ce = nullptr)
: SExpr(COP_Call), Target(T), Cexpr(Ce) {}
Call(const Call &C, SExpr *T) : SExpr(C), Target(T), Cexpr(C.Cexpr) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Call; }

SExpr *target() { return Target; }
const SExpr *target() const { return Target; }

const CallExpr *clangCallExpr() const { return Cexpr; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nt = Vs.traverse(Target, Vs.subExprCtx(Ctx));
return Vs.reduceCall(*this, Nt);
}

template <class C>
typename C::CType compare(const Call* E, C& Cmp) const {
return Cmp.compare(target(), E->target());
}

private:
SExpr* Target;
const CallExpr *Cexpr;
};


class Alloc : public SExpr {
public:
enum AllocKind {
AK_Stack,
AK_Heap
};

Alloc(SExpr *D, AllocKind K) : SExpr(COP_Alloc), Dtype(D) { Flags = K; }
Alloc(const Alloc &A, SExpr *Dt) : SExpr(A), Dtype(Dt) { Flags = A.kind(); }

static bool classof(const SExpr *E) { return E->opcode() == COP_Call; }

AllocKind kind() const { return static_cast<AllocKind>(Flags); }

SExpr *dataType() { return Dtype; }
const SExpr *dataType() const { return Dtype; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nd = Vs.traverse(Dtype, Vs.declCtx(Ctx));
return Vs.reduceAlloc(*this, Nd);
}

template <class C>
typename C::CType compare(const Alloc* E, C& Cmp) const {
typename C::CType Ct = Cmp.compareIntegers(kind(), E->kind());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(dataType(), E->dataType());
}

private:
SExpr* Dtype;
};


class Load : public SExpr {
public:
Load(SExpr *P) : SExpr(COP_Load), Ptr(P) {}
Load(const Load &L, SExpr *P) : SExpr(L), Ptr(P) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Load; }

SExpr *pointer() { return Ptr; }
const SExpr *pointer() const { return Ptr; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Np = Vs.traverse(Ptr, Vs.subExprCtx(Ctx));
return Vs.reduceLoad(*this, Np);
}

template <class C>
typename C::CType compare(const Load* E, C& Cmp) const {
return Cmp.compare(pointer(), E->pointer());
}

private:
SExpr* Ptr;
};



class Store : public SExpr {
public:
Store(SExpr *P, SExpr *V) : SExpr(COP_Store), Dest(P), Source(V) {}
Store(const Store &S, SExpr *P, SExpr *V) : SExpr(S), Dest(P), Source(V) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Store; }

SExpr *destination() { return Dest; }
const SExpr *destination() const { return Dest; }

SExpr *source() { return Source; }
const SExpr *source() const { return Source; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Np = Vs.traverse(Dest, Vs.subExprCtx(Ctx));
auto Nv = Vs.traverse(Source, Vs.subExprCtx(Ctx));
return Vs.reduceStore(*this, Np, Nv);
}

template <class C>
typename C::CType compare(const Store* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(destination(), E->destination());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(source(), E->source());
}

private:
SExpr* Dest;
SExpr* Source;
};



class ArrayIndex : public SExpr {
public:
ArrayIndex(SExpr *A, SExpr *N) : SExpr(COP_ArrayIndex), Array(A), Index(N) {}
ArrayIndex(const ArrayIndex &E, SExpr *A, SExpr *N)
: SExpr(E), Array(A), Index(N) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_ArrayIndex; }

SExpr *array() { return Array; }
const SExpr *array() const { return Array; }

SExpr *index() { return Index; }
const SExpr *index() const { return Index; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Na = Vs.traverse(Array, Vs.subExprCtx(Ctx));
auto Ni = Vs.traverse(Index, Vs.subExprCtx(Ctx));
return Vs.reduceArrayIndex(*this, Na, Ni);
}

template <class C>
typename C::CType compare(const ArrayIndex* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(array(), E->array());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(index(), E->index());
}

private:
SExpr* Array;
SExpr* Index;
};




class ArrayAdd : public SExpr {
public:
ArrayAdd(SExpr *A, SExpr *N) : SExpr(COP_ArrayAdd), Array(A), Index(N) {}
ArrayAdd(const ArrayAdd &E, SExpr *A, SExpr *N)
: SExpr(E), Array(A), Index(N) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_ArrayAdd; }

SExpr *array() { return Array; }
const SExpr *array() const { return Array; }

SExpr *index() { return Index; }
const SExpr *index() const { return Index; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Na = Vs.traverse(Array, Vs.subExprCtx(Ctx));
auto Ni = Vs.traverse(Index, Vs.subExprCtx(Ctx));
return Vs.reduceArrayAdd(*this, Na, Ni);
}

template <class C>
typename C::CType compare(const ArrayAdd* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(array(), E->array());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(index(), E->index());
}

private:
SExpr* Array;
SExpr* Index;
};



class UnaryOp : public SExpr {
public:
UnaryOp(TIL_UnaryOpcode Op, SExpr *E) : SExpr(COP_UnaryOp), Expr0(E) {
Flags = Op;
}

UnaryOp(const UnaryOp &U, SExpr *E) : SExpr(U), Expr0(E) { Flags = U.Flags; }

static bool classof(const SExpr *E) { return E->opcode() == COP_UnaryOp; }

TIL_UnaryOpcode unaryOpcode() const {
return static_cast<TIL_UnaryOpcode>(Flags);
}

SExpr *expr() { return Expr0; }
const SExpr *expr() const { return Expr0; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Ne = Vs.traverse(Expr0, Vs.subExprCtx(Ctx));
return Vs.reduceUnaryOp(*this, Ne);
}

template <class C>
typename C::CType compare(const UnaryOp* E, C& Cmp) const {
typename C::CType Ct =
Cmp.compareIntegers(unaryOpcode(), E->unaryOpcode());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(expr(), E->expr());
}

private:
SExpr* Expr0;
};



class BinaryOp : public SExpr {
public:
BinaryOp(TIL_BinaryOpcode Op, SExpr *E0, SExpr *E1)
: SExpr(COP_BinaryOp), Expr0(E0), Expr1(E1) {
Flags = Op;
}

BinaryOp(const BinaryOp &B, SExpr *E0, SExpr *E1)
: SExpr(B), Expr0(E0), Expr1(E1) {
Flags = B.Flags;
}

static bool classof(const SExpr *E) { return E->opcode() == COP_BinaryOp; }

TIL_BinaryOpcode binaryOpcode() const {
return static_cast<TIL_BinaryOpcode>(Flags);
}

SExpr *expr0() { return Expr0; }
const SExpr *expr0() const { return Expr0; }

SExpr *expr1() { return Expr1; }
const SExpr *expr1() const { return Expr1; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Ne0 = Vs.traverse(Expr0, Vs.subExprCtx(Ctx));
auto Ne1 = Vs.traverse(Expr1, Vs.subExprCtx(Ctx));
return Vs.reduceBinaryOp(*this, Ne0, Ne1);
}

template <class C>
typename C::CType compare(const BinaryOp* E, C& Cmp) const {
typename C::CType Ct =
Cmp.compareIntegers(binaryOpcode(), E->binaryOpcode());
if (Cmp.notTrue(Ct))
return Ct;
Ct = Cmp.compare(expr0(), E->expr0());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(expr1(), E->expr1());
}

private:
SExpr* Expr0;
SExpr* Expr1;
};




class Cast : public SExpr {
public:
Cast(TIL_CastOpcode Op, SExpr *E) : SExpr(COP_Cast), Expr0(E) { Flags = Op; }
Cast(const Cast &C, SExpr *E) : SExpr(C), Expr0(E) { Flags = C.Flags; }

static bool classof(const SExpr *E) { return E->opcode() == COP_Cast; }

TIL_CastOpcode castOpcode() const {
return static_cast<TIL_CastOpcode>(Flags);
}

SExpr *expr() { return Expr0; }
const SExpr *expr() const { return Expr0; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Ne = Vs.traverse(Expr0, Vs.subExprCtx(Ctx));
return Vs.reduceCast(*this, Ne);
}

template <class C>
typename C::CType compare(const Cast* E, C& Cmp) const {
typename C::CType Ct =
Cmp.compareIntegers(castOpcode(), E->castOpcode());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(expr(), E->expr());
}

private:
SExpr* Expr0;
};

class SCFG;




class Phi : public SExpr {
public:
using ValArray = SimpleArray<SExpr *>;




enum Status {
PH_MultiVal = 0,
PH_SingleVal,
PH_Incomplete
};

Phi() : SExpr(COP_Phi) {}
Phi(MemRegionRef A, unsigned Nvals) : SExpr(COP_Phi), Values(A, Nvals) {}
Phi(const Phi &P, ValArray &&Vs) : SExpr(P), Values(std::move(Vs)) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Phi; }

const ValArray &values() const { return Values; }
ValArray &values() { return Values; }

Status status() const { return static_cast<Status>(Flags); }
void setStatus(Status s) { Flags = s; }


const ValueDecl *clangDecl() const { return Cvdecl; }


void setClangDecl(const ValueDecl *Cvd) { Cvdecl = Cvd; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
typename V::template Container<typename V::R_SExpr>
Nvs(Vs, Values.size());

for (const auto *Val : Values)
Nvs.push_back( Vs.traverse(Val, Vs.subExprCtx(Ctx)) );
return Vs.reducePhi(*this, Nvs);
}

template <class C>
typename C::CType compare(const Phi *E, C &Cmp) const {

return Cmp.comparePointers(this, E);
}

private:
ValArray Values;
const ValueDecl* Cvdecl = nullptr;
};


class Terminator : public SExpr {
protected:
Terminator(TIL_Opcode Op) : SExpr(Op) {}
Terminator(const SExpr &E) : SExpr(E) {}

public:
static bool classof(const SExpr *E) {
return E->opcode() >= COP_Goto && E->opcode() <= COP_Return;
}


ArrayRef<BasicBlock *> successors();

ArrayRef<BasicBlock *> successors() const {
return const_cast<Terminator*>(this)->successors();
}
};






class Goto : public Terminator {
public:
Goto(BasicBlock *B, unsigned I)
: Terminator(COP_Goto), TargetBlock(B), Index(I) {}
Goto(const Goto &G, BasicBlock *B, unsigned I)
: Terminator(COP_Goto), TargetBlock(B), Index(I) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Goto; }

const BasicBlock *targetBlock() const { return TargetBlock; }
BasicBlock *targetBlock() { return TargetBlock; }


unsigned index() const { return Index; }


ArrayRef<BasicBlock *> successors() { return TargetBlock; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
BasicBlock *Ntb = Vs.reduceBasicBlockRef(TargetBlock);
return Vs.reduceGoto(*this, Ntb);
}

template <class C>
typename C::CType compare(const Goto *E, C &Cmp) const {

return Cmp.comparePointers(this, E);
}

private:
BasicBlock *TargetBlock;
unsigned Index;
};




class Branch : public Terminator {
public:
Branch(SExpr *C, BasicBlock *T, BasicBlock *E)
: Terminator(COP_Branch), Condition(C) {
Branches[0] = T;
Branches[1] = E;
}

Branch(const Branch &Br, SExpr *C, BasicBlock *T, BasicBlock *E)
: Terminator(Br), Condition(C) {
Branches[0] = T;
Branches[1] = E;
}

static bool classof(const SExpr *E) { return E->opcode() == COP_Branch; }

const SExpr *condition() const { return Condition; }
SExpr *condition() { return Condition; }

const BasicBlock *thenBlock() const { return Branches[0]; }
BasicBlock *thenBlock() { return Branches[0]; }

const BasicBlock *elseBlock() const { return Branches[1]; }
BasicBlock *elseBlock() { return Branches[1]; }


ArrayRef<BasicBlock*> successors() {
return llvm::makeArrayRef(Branches);
}

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nc = Vs.traverse(Condition, Vs.subExprCtx(Ctx));
BasicBlock *Ntb = Vs.reduceBasicBlockRef(Branches[0]);
BasicBlock *Nte = Vs.reduceBasicBlockRef(Branches[1]);
return Vs.reduceBranch(*this, Nc, Ntb, Nte);
}

template <class C>
typename C::CType compare(const Branch *E, C &Cmp) const {

return Cmp.comparePointers(this, E);
}

private:
SExpr *Condition;
BasicBlock *Branches[2];
};



class Return : public Terminator {
public:
Return(SExpr* Rval) : Terminator(COP_Return), Retval(Rval) {}
Return(const Return &R, SExpr* Rval) : Terminator(R), Retval(Rval) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_Return; }


ArrayRef<BasicBlock *> successors() { return None; }

SExpr *returnValue() { return Retval; }
const SExpr *returnValue() const { return Retval; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Ne = Vs.traverse(Retval, Vs.subExprCtx(Ctx));
return Vs.reduceReturn(*this, Ne);
}

template <class C>
typename C::CType compare(const Return *E, C &Cmp) const {
return Cmp.compare(Retval, E->Retval);
}

private:
SExpr* Retval;
};

inline ArrayRef<BasicBlock*> Terminator::successors() {
switch (opcode()) {
case COP_Goto: return cast<Goto>(this)->successors();
case COP_Branch: return cast<Branch>(this)->successors();
case COP_Return: return cast<Return>(this)->successors();
default:
return None;
}
}






class BasicBlock : public SExpr {
public:
using InstrArray = SimpleArray<SExpr *>;
using BlockArray = SimpleArray<BasicBlock *>;





struct TopologyNode {
int NodeID = 0;


int SizeOfSubTree = 0;


BasicBlock *Parent = nullptr;

TopologyNode() = default;

bool isParentOf(const TopologyNode& OtherNode) {
return OtherNode.NodeID > NodeID &&
OtherNode.NodeID < NodeID + SizeOfSubTree;
}

bool isParentOfOrEqual(const TopologyNode& OtherNode) {
return OtherNode.NodeID >= NodeID &&
OtherNode.NodeID < NodeID + SizeOfSubTree;
}
};

explicit BasicBlock(MemRegionRef A)
: SExpr(COP_BasicBlock), Arena(A), BlockID(0), Visited(false) {}
BasicBlock(BasicBlock &B, MemRegionRef A, InstrArray &&As, InstrArray &&Is,
Terminator *T)
: SExpr(COP_BasicBlock), Arena(A), BlockID(0), Visited(false),
Args(std::move(As)), Instrs(std::move(Is)), TermInstr(T) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_BasicBlock; }


int blockID() const { return BlockID; }


size_t numPredecessors() const { return Predecessors.size(); }
size_t numSuccessors() const { return successors().size(); }

const SCFG* cfg() const { return CFGPtr; }
SCFG* cfg() { return CFGPtr; }

const BasicBlock *parent() const { return DominatorNode.Parent; }
BasicBlock *parent() { return DominatorNode.Parent; }

const InstrArray &arguments() const { return Args; }
InstrArray &arguments() { return Args; }

InstrArray &instructions() { return Instrs; }
const InstrArray &instructions() const { return Instrs; }




BlockArray &predecessors() { return Predecessors; }
const BlockArray &predecessors() const { return Predecessors; }

ArrayRef<BasicBlock*> successors() { return TermInstr->successors(); }
ArrayRef<BasicBlock*> successors() const { return TermInstr->successors(); }

const Terminator *terminator() const { return TermInstr; }
Terminator *terminator() { return TermInstr; }

void setTerminator(Terminator *E) { TermInstr = E; }

bool Dominates(const BasicBlock &Other) {
return DominatorNode.isParentOfOrEqual(Other.DominatorNode);
}

bool PostDominates(const BasicBlock &Other) {
return PostDominatorNode.isParentOfOrEqual(Other.PostDominatorNode);
}


void addArgument(Phi *V) {
Args.reserveCheck(1, Arena);
Args.push_back(V);
}


void addInstruction(SExpr *V) {
Instrs.reserveCheck(1, Arena);
Instrs.push_back(V);
}



unsigned addPredecessor(BasicBlock *Pred);


void reserveArguments(unsigned Nargs) { Args.reserve(Nargs, Arena); }


void reserveInstructions(unsigned Nins) { Instrs.reserve(Nins, Arena); }


void reservePredecessors(unsigned NumPreds);


unsigned findPredecessorIndex(const BasicBlock *BB) const {
auto I = llvm::find(Predecessors, BB);
return std::distance(Predecessors.cbegin(), I);
}

template <class V>
typename V::R_BasicBlock traverse(V &Vs, typename V::R_Ctx Ctx) {
typename V::template Container<SExpr*> Nas(Vs, Args.size());
typename V::template Container<SExpr*> Nis(Vs, Instrs.size());


Vs.enterBasicBlock(*this);

for (const auto *E : Args) {
auto Ne = Vs.traverse(E, Vs.subExprCtx(Ctx));
Nas.push_back(Ne);
}
for (const auto *E : Instrs) {
auto Ne = Vs.traverse(E, Vs.subExprCtx(Ctx));
Nis.push_back(Ne);
}
auto Nt = Vs.traverse(TermInstr, Ctx);


Vs.exitBasicBlock(*this);

return Vs.reduceBasicBlock(*this, Nas, Nis, Nt);
}

template <class C>
typename C::CType compare(const BasicBlock *E, C &Cmp) const {

return Cmp.comparePointers(this, E);
}

private:
friend class SCFG;


unsigned renumberInstrs(unsigned id);

unsigned topologicalSort(SimpleArray<BasicBlock *> &Blocks, unsigned ID);
unsigned topologicalFinalSort(SimpleArray<BasicBlock *> &Blocks, unsigned ID);
void computeDominator();
void computePostDominator();


MemRegionRef Arena;


SCFG *CFGPtr = nullptr;


unsigned BlockID : 31;


bool Visited : 1;


BlockArray Predecessors;


InstrArray Args;


InstrArray Instrs;


Terminator *TermInstr = nullptr;


TopologyNode DominatorNode;


TopologyNode PostDominatorNode;
};




class SCFG : public SExpr {
public:
using BlockArray = SimpleArray<BasicBlock *>;
using iterator = BlockArray::iterator;
using const_iterator = BlockArray::const_iterator;

SCFG(MemRegionRef A, unsigned Nblocks)
: SExpr(COP_SCFG), Arena(A), Blocks(A, Nblocks) {
Entry = new (A) BasicBlock(A);
Exit = new (A) BasicBlock(A);
auto *V = new (A) Phi();
Exit->addArgument(V);
Exit->setTerminator(new (A) Return(V));
add(Entry);
add(Exit);
}

SCFG(const SCFG &Cfg, BlockArray &&Ba)
: SExpr(COP_SCFG), Arena(Cfg.Arena), Blocks(std::move(Ba)) {

}

static bool classof(const SExpr *E) { return E->opcode() == COP_SCFG; }


bool valid() const { return Entry && Exit && Blocks.size() > 0; }




bool normal() const { return Normal; }

iterator begin() { return Blocks.begin(); }
iterator end() { return Blocks.end(); }

const_iterator begin() const { return cbegin(); }
const_iterator end() const { return cend(); }

const_iterator cbegin() const { return Blocks.cbegin(); }
const_iterator cend() const { return Blocks.cend(); }

const BasicBlock *entry() const { return Entry; }
BasicBlock *entry() { return Entry; }
const BasicBlock *exit() const { return Exit; }
BasicBlock *exit() { return Exit; }



size_t numBlocks() const { return Blocks.size(); }




unsigned numInstructions() { return NumInstructions; }

inline void add(BasicBlock *BB) {
assert(BB->CFGPtr == nullptr);
BB->CFGPtr = this;
Blocks.reserveCheck(1, Arena);
Blocks.push_back(BB);
}

void setEntry(BasicBlock *BB) { Entry = BB; }
void setExit(BasicBlock *BB) { Exit = BB; }

void computeNormalForm();

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
Vs.enterCFG(*this);
typename V::template Container<BasicBlock *> Bbs(Vs, Blocks.size());

for (const auto *B : Blocks) {
Bbs.push_back( B->traverse(Vs, Vs.subExprCtx(Ctx)) );
}
Vs.exitCFG(*this);
return Vs.reduceSCFG(*this, Bbs);
}

template <class C>
typename C::CType compare(const SCFG *E, C &Cmp) const {

return Cmp.comparePointers(this, E);
}

private:

void renumberInstrs();

MemRegionRef Arena;
BlockArray Blocks;
BasicBlock *Entry = nullptr;
BasicBlock *Exit = nullptr;
unsigned NumInstructions = 0;
bool Normal = false;
};



class Identifier : public SExpr {
public:
Identifier(StringRef Id): SExpr(COP_Identifier), Name(Id) {}
Identifier(const Identifier &) = default;

static bool classof(const SExpr *E) { return E->opcode() == COP_Identifier; }

StringRef name() const { return Name; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
return Vs.reduceIdentifier(*this);
}

template <class C>
typename C::CType compare(const Identifier* E, C& Cmp) const {
return Cmp.compareStrings(name(), E->name());
}

private:
StringRef Name;
};



class IfThenElse : public SExpr {
public:
IfThenElse(SExpr *C, SExpr *T, SExpr *E)
: SExpr(COP_IfThenElse), Condition(C), ThenExpr(T), ElseExpr(E) {}
IfThenElse(const IfThenElse &I, SExpr *C, SExpr *T, SExpr *E)
: SExpr(I), Condition(C), ThenExpr(T), ElseExpr(E) {}

static bool classof(const SExpr *E) { return E->opcode() == COP_IfThenElse; }

SExpr *condition() { return Condition; }
const SExpr *condition() const { return Condition; }

SExpr *thenExpr() { return ThenExpr; }
const SExpr *thenExpr() const { return ThenExpr; }

SExpr *elseExpr() { return ElseExpr; }
const SExpr *elseExpr() const { return ElseExpr; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {
auto Nc = Vs.traverse(Condition, Vs.subExprCtx(Ctx));
auto Nt = Vs.traverse(ThenExpr, Vs.subExprCtx(Ctx));
auto Ne = Vs.traverse(ElseExpr, Vs.subExprCtx(Ctx));
return Vs.reduceIfThenElse(*this, Nc, Nt, Ne);
}

template <class C>
typename C::CType compare(const IfThenElse* E, C& Cmp) const {
typename C::CType Ct = Cmp.compare(condition(), E->condition());
if (Cmp.notTrue(Ct))
return Ct;
Ct = Cmp.compare(thenExpr(), E->thenExpr());
if (Cmp.notTrue(Ct))
return Ct;
return Cmp.compare(elseExpr(), E->elseExpr());
}

private:
SExpr* Condition;
SExpr* ThenExpr;
SExpr* ElseExpr;
};



class Let : public SExpr {
public:
Let(Variable *Vd, SExpr *Bd) : SExpr(COP_Let), VarDecl(Vd), Body(Bd) {
Vd->setKind(Variable::VK_Let);
}

Let(const Let &L, Variable *Vd, SExpr *Bd) : SExpr(L), VarDecl(Vd), Body(Bd) {
Vd->setKind(Variable::VK_Let);
}

static bool classof(const SExpr *E) { return E->opcode() == COP_Let; }

Variable *variableDecl() { return VarDecl; }
const Variable *variableDecl() const { return VarDecl; }

SExpr *body() { return Body; }
const SExpr *body() const { return Body; }

template <class V>
typename V::R_SExpr traverse(V &Vs, typename V::R_Ctx Ctx) {

auto E0 = Vs.traverse(VarDecl->Definition, Vs.subExprCtx(Ctx));

Variable *Nvd = Vs.enterScope(*VarDecl, E0);
auto E1 = Vs.traverse(Body, Ctx);
Vs.exitScope(*VarDecl);
return Vs.reduceLet(*this, Nvd, E1);
}

template <class C>
typename C::CType compare(const Let* E, C& Cmp) const {
typename C::CType Ct =
Cmp.compare(VarDecl->definition(), E->VarDecl->definition());
if (Cmp.notTrue(Ct))
return Ct;
Cmp.enterScope(variableDecl(), E->variableDecl());
Ct = Cmp.compare(body(), E->body());
Cmp.leaveScope();
return Ct;
}

private:
Variable *VarDecl;
SExpr* Body;
};

const SExpr *getCanonicalVal(const SExpr *E);
SExpr* simplifyToCanonicalVal(SExpr *E);
void simplifyIncompleteArg(til::Phi *Ph);

}
}

}

#endif
