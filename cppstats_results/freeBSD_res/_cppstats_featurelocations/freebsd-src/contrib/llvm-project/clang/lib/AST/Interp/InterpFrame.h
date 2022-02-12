











#if !defined(LLVM_CLANG_AST_INTERP_INTERPFRAME_H)
#define LLVM_CLANG_AST_INTERP_INTERPFRAME_H

#include "Frame.h"
#include "Pointer.h"
#include "Program.h"
#include "State.h"
#include <cstdint>
#include <vector>

namespace clang {
namespace interp {
class Function;
class InterpState;


class InterpFrame final : public Frame {
public:

InterpFrame *Caller;


InterpFrame(InterpState &S, Function *Func, InterpFrame *Caller,
CodePtr RetPC, Pointer &&This);


~InterpFrame();


void destroy(unsigned Idx);


void popArgs();


void describe(llvm::raw_ostream &OS) override;


Frame *getCaller() const override;


SourceLocation getCallLocation() const override;


const FunctionDecl *getCallee() const override;


Function *getFunction() const { return Func; }


size_t getFrameOffset() const { return FrameOffset; }


template <typename T> const T &getLocal(unsigned Offset) {
return localRef<T>(Offset);
}


template <typename T> void setLocal(unsigned Offset, const T &Value) {
localRef<T>(Offset) = Value;
}


Pointer getLocalPointer(unsigned Offset);


template <typename T> const T &getParam(unsigned Offset) {
auto Pt = Params.find(Offset);
if (Pt == Params.end()) {
return stackRef<T>(Offset);
} else {
return Pointer(reinterpret_cast<Block *>(Pt->second.get())).deref<T>();
}
}


template <typename T> void setParam(unsigned Offset, const T &Value) {
getParamPointer(Offset).deref<T>() = Value;
}


Pointer getParamPointer(unsigned Offset);


const Pointer &getThis() const { return This; }


bool isRoot() const { return !Func; }


CodePtr getPC() const { return Func->getCodeBegin(); }


CodePtr getRetPC() const { return RetPC; }


virtual SourceInfo getSource(CodePtr PC) const;
const Expr *getExpr(CodePtr PC) const;
SourceLocation getLocation(CodePtr PC) const;

private:

template <typename T> const T &stackRef(unsigned Offset) {
return *reinterpret_cast<const T *>(Args - ArgSize + Offset);
}


template <typename T> T &localRef(unsigned Offset) {
return *reinterpret_cast<T *>(Locals.get() + Offset);
}


void *localBlock(unsigned Offset) {
return Locals.get() + Offset - sizeof(Block);
}

private:

InterpState &S;

Function *Func;

Pointer This;

CodePtr RetPC;

const unsigned ArgSize;

char *Args = nullptr;

std::unique_ptr<char[]> Locals;

const size_t FrameOffset;

llvm::DenseMap<unsigned, std::unique_ptr<char[]>> Params;
};

}
}

#endif
