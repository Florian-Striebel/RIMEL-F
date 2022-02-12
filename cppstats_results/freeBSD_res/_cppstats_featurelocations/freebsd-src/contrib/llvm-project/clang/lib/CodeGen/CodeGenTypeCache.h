











#if !defined(LLVM_CLANG_LIB_CODEGEN_CODEGENTYPECACHE_H)
#define LLVM_CLANG_LIB_CODEGEN_CODEGENTYPECACHE_H

#include "clang/AST/CharUnits.h"
#include "clang/Basic/AddressSpaces.h"
#include "llvm/IR/CallingConv.h"

namespace llvm {
class Type;
class IntegerType;
class PointerType;
}

namespace clang {
namespace CodeGen {




struct CodeGenTypeCache {

llvm::Type *VoidTy;


llvm::IntegerType *Int8Ty, *Int16Ty, *Int32Ty, *Int64Ty;

llvm::Type *HalfTy, *BFloatTy, *FloatTy, *DoubleTy;


llvm::IntegerType *IntTy;


llvm::IntegerType *CharTy;


union {
llvm::IntegerType *IntPtrTy;
llvm::IntegerType *SizeTy;
llvm::IntegerType *PtrDiffTy;
};


union {
llvm::PointerType *VoidPtrTy;
llvm::PointerType *Int8PtrTy;
};


union {
llvm::PointerType *VoidPtrPtrTy;
llvm::PointerType *Int8PtrPtrTy;
};


union {
llvm::PointerType *AllocaVoidPtrTy;
llvm::PointerType *AllocaInt8PtrTy;
};



union {
unsigned char IntSizeInBytes;
unsigned char IntAlignInBytes;
};
CharUnits getIntSize() const {
return CharUnits::fromQuantity(IntSizeInBytes);
}
CharUnits getIntAlign() const {
return CharUnits::fromQuantity(IntAlignInBytes);
}


unsigned char PointerWidthInBits;


union {
unsigned char PointerAlignInBytes;
unsigned char PointerSizeInBytes;
};


union {
unsigned char SizeSizeInBytes;
unsigned char SizeAlignInBytes;
};

LangAS ASTAllocaAddressSpace;

CharUnits getSizeSize() const {
return CharUnits::fromQuantity(SizeSizeInBytes);
}
CharUnits getSizeAlign() const {
return CharUnits::fromQuantity(SizeAlignInBytes);
}
CharUnits getPointerSize() const {
return CharUnits::fromQuantity(PointerSizeInBytes);
}
CharUnits getPointerAlign() const {
return CharUnits::fromQuantity(PointerAlignInBytes);
}

llvm::CallingConv::ID RuntimeCC;
llvm::CallingConv::ID getRuntimeCC() const { return RuntimeCC; }

LangAS getASTAllocaAddressSpace() const { return ASTAllocaAddressSpace; }
};

}
}

#endif
