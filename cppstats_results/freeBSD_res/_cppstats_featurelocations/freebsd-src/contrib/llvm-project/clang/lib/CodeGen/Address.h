












#if !defined(LLVM_CLANG_LIB_CODEGEN_ADDRESS_H)
#define LLVM_CLANG_LIB_CODEGEN_ADDRESS_H

#include "llvm/IR/Constants.h"
#include "clang/AST/CharUnits.h"

namespace clang {
namespace CodeGen {


class Address {
llvm::Value *Pointer;
CharUnits Alignment;
public:
Address(llvm::Value *pointer, CharUnits alignment)
: Pointer(pointer), Alignment(alignment) {
assert((!alignment.isZero() || pointer == nullptr) &&
"creating valid address with invalid alignment");
}

static Address invalid() { return Address(nullptr, CharUnits()); }
bool isValid() const { return Pointer != nullptr; }

llvm::Value *getPointer() const {
assert(isValid());
return Pointer;
}


llvm::PointerType *getType() const {
return llvm::cast<llvm::PointerType>(getPointer()->getType());
}





llvm::Type *getElementType() const {
return getType()->getElementType();
}


unsigned getAddressSpace() const {
return getType()->getAddressSpace();
}


llvm::StringRef getName() const {
return getPointer()->getName();
}


CharUnits getAlignment() const {
assert(isValid());
return Alignment;
}
};



class ConstantAddress : public Address {
public:
ConstantAddress(llvm::Constant *pointer, CharUnits alignment)
: Address(pointer, alignment) {}

static ConstantAddress invalid() {
return ConstantAddress(nullptr, CharUnits());
}

llvm::Constant *getPointer() const {
return llvm::cast<llvm::Constant>(Address::getPointer());
}

ConstantAddress getBitCast(llvm::Type *ty) const {
return ConstantAddress(llvm::ConstantExpr::getBitCast(getPointer(), ty),
getAlignment());
}

ConstantAddress getElementBitCast(llvm::Type *ty) const {
return getBitCast(ty->getPointerTo(getAddressSpace()));
}

static bool isaImpl(Address addr) {
return llvm::isa<llvm::Constant>(addr.getPointer());
}
static ConstantAddress castImpl(Address addr) {
return ConstantAddress(llvm::cast<llvm::Constant>(addr.getPointer()),
addr.getAlignment());
}
};

}


template <class U> inline U cast(CodeGen::Address addr) {
return U::castImpl(addr);
}
template <class U> inline bool isa(CodeGen::Address addr) {
return U::isaImpl(addr);
}

}

#endif
