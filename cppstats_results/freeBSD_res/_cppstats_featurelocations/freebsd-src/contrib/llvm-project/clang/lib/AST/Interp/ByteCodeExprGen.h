











#if !defined(LLVM_CLANG_AST_INTERP_BYTECODEEXPRGEN_H)
#define LLVM_CLANG_AST_INTERP_BYTECODEEXPRGEN_H

#include "ByteCodeEmitter.h"
#include "EvalEmitter.h"
#include "Pointer.h"
#include "PrimType.h"
#include "Record.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Basic/TargetInfo.h"
#include "llvm/ADT/Optional.h"

namespace clang {
class QualType;

namespace interp {
class Function;
class State;

template <class Emitter> class LocalScope;
template <class Emitter> class RecordScope;
template <class Emitter> class VariableScope;
template <class Emitter> class DeclScope;
template <class Emitter> class OptionScope;


template <class Emitter>
class ByteCodeExprGen : public ConstStmtVisitor<ByteCodeExprGen<Emitter>, bool>,
public Emitter {
protected:

using NullaryFn = bool (ByteCodeExprGen::*)(const SourceInfo &);
using UnaryFn = bool (ByteCodeExprGen::*)(PrimType, const SourceInfo &);
using BinaryFn = bool (ByteCodeExprGen::*)(PrimType, PrimType,
const SourceInfo &);


using LabelTy = typename Emitter::LabelTy;
using AddrTy = typename Emitter::AddrTy;


using InitFnRef = std::function<bool()>;


Context &Ctx;

Program &P;

public:

template <typename... Tys>
ByteCodeExprGen(Context &Ctx, Program &P, Tys &&... Args)
: Emitter(Ctx, P, Args...), Ctx(Ctx), P(P) {}


bool VisitCastExpr(const CastExpr *E);
bool VisitIntegerLiteral(const IntegerLiteral *E);
bool VisitParenExpr(const ParenExpr *E);
bool VisitBinaryOperator(const BinaryOperator *E);

protected:
bool visitExpr(const Expr *E) override;
bool visitDecl(const VarDecl *VD) override;

protected:

void emitCleanup();


const RecordType *getRecordTy(QualType Ty);


Record *getRecord(QualType Ty);
Record *getRecord(const RecordDecl *RD);


unsigned getIntWidth(QualType Ty) {
auto &ASTContext = Ctx.getASTContext();
return ASTContext.getIntWidth(Ty);
}


unsigned getCharBit() const {
auto &ASTContext = Ctx.getASTContext();
return ASTContext.getTargetInfo().getCharWidth();
}


llvm::Optional<PrimType> classify(const Expr *E) const {
return E->isGLValue() ? PT_Ptr : classify(E->getType());
}
llvm::Optional<PrimType> classify(QualType Ty) const {
return Ctx.classify(Ty);
}


bool needsAdjust(QualType Ty) const {
return true;
}


PrimType classifyPrim(QualType Ty) const {
if (auto T = classify(Ty)) {
return *T;
}
llvm_unreachable("not a primitive type");
}


bool discard(const Expr *E);

bool visit(const Expr *E);

bool visitInitializer(const Expr *E, InitFnRef GenPtr);


bool visitBool(const Expr *E);


bool visitLocalInitializer(const Expr *Init, unsigned I) {
return visitInitializer(Init, [this, I, Init] {
return this->emitGetPtrLocal(I, Init);
});
}


bool visitGlobalInitializer(const Expr *Init, unsigned I) {
return visitInitializer(Init, [this, I, Init] {
return this->emitGetPtrGlobal(I, Init);
});
}


bool visitThisInitializer(const Expr *I) {
return visitInitializer(I, [this, I] { return this->emitThis(I); });
}


unsigned allocateLocalPrimitive(DeclTy &&Decl, PrimType Ty, bool IsMutable,
bool IsExtended = false);


llvm::Optional<unsigned> allocateLocal(DeclTy &&Decl,
bool IsExtended = false);

private:
friend class VariableScope<Emitter>;
friend class LocalScope<Emitter>;
friend class RecordScope<Emitter>;
friend class DeclScope<Emitter>;
friend class OptionScope<Emitter>;


bool visitZeroInitializer(PrimType T, const Expr *E);

enum class DerefKind {

Read,

Write,

ReadWrite,
};




bool dereference(const Expr *LV, DerefKind AK,
llvm::function_ref<bool(PrimType)> Direct,
llvm::function_ref<bool(PrimType)> Indirect);
bool dereferenceParam(const Expr *LV, PrimType T, const ParmVarDecl *PD,
DerefKind AK,
llvm::function_ref<bool(PrimType)> Direct,
llvm::function_ref<bool(PrimType)> Indirect);
bool dereferenceVar(const Expr *LV, PrimType T, const VarDecl *PD,
DerefKind AK, llvm::function_ref<bool(PrimType)> Direct,
llvm::function_ref<bool(PrimType)> Indirect);


bool emitConst(PrimType T, unsigned NumBits, const llvm::APInt &Value,
const Expr *E);


template <typename T> bool emitConst(const Expr *E, T Value) {
QualType Ty = E->getType();
unsigned NumBits = getIntWidth(Ty);
APInt WrappedValue(NumBits, Value, std::is_signed<T>::value);
return emitConst(*Ctx.classify(Ty), NumBits, WrappedValue, E);
}


bool getPtrVarDecl(const VarDecl *VD, const Expr *E);


llvm::Optional<unsigned> getGlobalIdx(const VarDecl *VD);


bool emitInitFn() {
assert(InitFn && "missing initializer");
return (*InitFn)();
}

protected:

llvm::DenseMap<const ValueDecl *, Scope::Local> Locals;


llvm::DenseMap<const OpaqueValueExpr *, unsigned> OpaqueExprs;


VariableScope<Emitter> *VarScope = nullptr;


llvm::Optional<uint64_t> ArrayIndex;


bool DiscardResult = false;


llvm::Optional<InitFnRef> InitFn = {};
};

extern template class ByteCodeExprGen<ByteCodeEmitter>;
extern template class ByteCodeExprGen<EvalEmitter>;


template <class Emitter> class VariableScope {
public:
virtual ~VariableScope() { Ctx->VarScope = this->Parent; }

void add(const Scope::Local &Local, bool IsExtended) {
if (IsExtended)
this->addExtended(Local);
else
this->addLocal(Local);
}

virtual void addLocal(const Scope::Local &Local) {
if (this->Parent)
this->Parent->addLocal(Local);
}

virtual void addExtended(const Scope::Local &Local) {
if (this->Parent)
this->Parent->addExtended(Local);
}

virtual void emitDestruction() {}

VariableScope *getParent() { return Parent; }

protected:
VariableScope(ByteCodeExprGen<Emitter> *Ctx)
: Ctx(Ctx), Parent(Ctx->VarScope) {
Ctx->VarScope = this;
}


ByteCodeExprGen<Emitter> *Ctx;

VariableScope *Parent;
};





template <class Emitter> class LocalScope : public VariableScope<Emitter> {
public:
LocalScope(ByteCodeExprGen<Emitter> *Ctx) : VariableScope<Emitter>(Ctx) {}

~LocalScope() override { this->emitDestruction(); }

void addLocal(const Scope::Local &Local) override {
if (!Idx.hasValue()) {
Idx = this->Ctx->Descriptors.size();
this->Ctx->Descriptors.emplace_back();
}

this->Ctx->Descriptors[*Idx].emplace_back(Local);
}

void emitDestruction() override {
if (!Idx.hasValue())
return;
this->Ctx->emitDestroy(*Idx, SourceInfo{});
}

protected:

Optional<unsigned> Idx;
};


template <class Emitter> class BlockScope final : public LocalScope<Emitter> {
public:
BlockScope(ByteCodeExprGen<Emitter> *Ctx) : LocalScope<Emitter>(Ctx) {}

void addExtended(const Scope::Local &Local) override {
llvm_unreachable("Cannot create temporaries in full scopes");
}
};



template <class Emitter> class ExprScope final : public LocalScope<Emitter> {
public:
ExprScope(ByteCodeExprGen<Emitter> *Ctx) : LocalScope<Emitter>(Ctx) {}

void addExtended(const Scope::Local &Local) override {
this->Parent->addLocal(Local);
}
};

}
}

#endif
