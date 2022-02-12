















#if !defined(LLVM_CLANG_CODEGEN_CONSTANTINITFUTURE_H)
#define LLVM_CLANG_CODEGEN_CONSTANTINITFUTURE_H

#include "llvm/ADT/PointerUnion.h"
#include "llvm/IR/Constant.h"




namespace clang {
namespace CodeGen {
class ConstantInitBuilderBase;
}
}
namespace llvm {
template <>
struct PointerLikeTypeTraits< ::clang::CodeGen::ConstantInitBuilderBase*> {
using T = ::clang::CodeGen::ConstantInitBuilderBase*;

static inline void *getAsVoidPointer(T p) { return p; }
static inline T getFromVoidPointer(void *p) {return static_cast<T>(p);}
static constexpr int NumLowBitsAvailable = 2;
};
}

namespace clang {
namespace CodeGen {



class ConstantInitFuture {
using PairTy = llvm::PointerUnion<ConstantInitBuilderBase*, llvm::Constant*>;

PairTy Data;

friend class ConstantInitBuilderBase;
explicit ConstantInitFuture(ConstantInitBuilderBase *builder);

public:
ConstantInitFuture() {}


explicit ConstantInitFuture(llvm::Constant *initializer) : Data(initializer) {
assert(initializer && "creating null future");
}


explicit operator bool() const { return bool(Data); }


llvm::Type *getType() const;


void abandon();



void installInGlobal(llvm::GlobalVariable *global);

void *getOpaqueValue() const { return Data.getOpaqueValue(); }
static ConstantInitFuture getFromOpaqueValue(void *value) {
ConstantInitFuture result;
result.Data = PairTy::getFromOpaqueValue(value);
return result;
}
static constexpr int NumLowBitsAvailable =
llvm::PointerLikeTypeTraits<PairTy>::NumLowBitsAvailable;
};

}
}

namespace llvm {

template <>
struct PointerLikeTypeTraits< ::clang::CodeGen::ConstantInitFuture> {
using T = ::clang::CodeGen::ConstantInitFuture;

static inline void *getAsVoidPointer(T future) {
return future.getOpaqueValue();
}
static inline T getFromVoidPointer(void *p) {
return T::getFromOpaqueValue(p);
}
static constexpr int NumLowBitsAvailable = T::NumLowBitsAvailable;
};

}

#endif
