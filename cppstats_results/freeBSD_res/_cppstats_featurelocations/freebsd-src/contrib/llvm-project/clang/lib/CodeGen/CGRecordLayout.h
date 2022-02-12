







#if !defined(LLVM_CLANG_LIB_CODEGEN_CGRECORDLAYOUT_H)
#define LLVM_CLANG_LIB_CODEGEN_CGRECORDLAYOUT_H

#include "clang/AST/CharUnits.h"
#include "clang/AST/DeclCXX.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/DerivedTypes.h"

namespace llvm {
class StructType;
}

namespace clang {
namespace CodeGen {









































struct CGBitFieldInfo {


unsigned Offset : 16;


unsigned Size : 15;


unsigned IsSigned : 1;



unsigned StorageSize;


CharUnits StorageOffset;




unsigned VolatileOffset : 16;



unsigned VolatileStorageSize;


CharUnits VolatileStorageOffset;

CGBitFieldInfo()
: Offset(), Size(), IsSigned(), StorageSize(), StorageOffset(),
VolatileOffset(), VolatileStorageSize(), VolatileStorageOffset() {}

CGBitFieldInfo(unsigned Offset, unsigned Size, bool IsSigned,
unsigned StorageSize, CharUnits StorageOffset)
: Offset(Offset), Size(Size), IsSigned(IsSigned),
StorageSize(StorageSize), StorageOffset(StorageOffset) {}

void print(raw_ostream &OS) const;
void dump() const;




static CGBitFieldInfo MakeInfo(class CodeGenTypes &Types,
const FieldDecl *FD,
uint64_t Offset, uint64_t Size,
uint64_t StorageSize,
CharUnits StorageOffset);
};





class CGRecordLayout {
friend class CodeGenTypes;

CGRecordLayout(const CGRecordLayout &) = delete;
void operator=(const CGRecordLayout &) = delete;

private:


llvm::StructType *CompleteObjectType;



llvm::StructType *BaseSubobjectType;



llvm::DenseMap<const FieldDecl *, unsigned> FieldInfo;



llvm::DenseMap<const FieldDecl *, CGBitFieldInfo> BitFields;



llvm::DenseMap<const CXXRecordDecl *, unsigned> NonVirtualBases;


llvm::DenseMap<const CXXRecordDecl *, unsigned> CompleteObjectVirtualBases;




bool IsZeroInitializable : 1;




bool IsZeroInitializableAsBase : 1;

public:
CGRecordLayout(llvm::StructType *CompleteObjectType,
llvm::StructType *BaseSubobjectType,
bool IsZeroInitializable,
bool IsZeroInitializableAsBase)
: CompleteObjectType(CompleteObjectType),
BaseSubobjectType(BaseSubobjectType),
IsZeroInitializable(IsZeroInitializable),
IsZeroInitializableAsBase(IsZeroInitializableAsBase) {}



llvm::StructType *getLLVMType() const {
return CompleteObjectType;
}



llvm::StructType *getBaseSubobjectLLVMType() const {
return BaseSubobjectType;
}



bool isZeroInitializable() const {
return IsZeroInitializable;
}



bool isZeroInitializableAsBase() const {
return IsZeroInitializableAsBase;
}



unsigned getLLVMFieldNo(const FieldDecl *FD) const {
FD = FD->getCanonicalDecl();
assert(FieldInfo.count(FD) && "Invalid field for record!");
return FieldInfo.lookup(FD);
}

unsigned getNonVirtualBaseLLVMFieldNo(const CXXRecordDecl *RD) const {
assert(NonVirtualBases.count(RD) && "Invalid non-virtual base!");
return NonVirtualBases.lookup(RD);
}



unsigned getVirtualBaseIndex(const CXXRecordDecl *base) const {
assert(CompleteObjectVirtualBases.count(base) && "Invalid virtual base!");
return CompleteObjectVirtualBases.lookup(base);
}


const CGBitFieldInfo &getBitFieldInfo(const FieldDecl *FD) const {
FD = FD->getCanonicalDecl();
assert(FD->isBitField() && "Invalid call for non-bit-field decl!");
llvm::DenseMap<const FieldDecl *, CGBitFieldInfo>::const_iterator
it = BitFields.find(FD);
assert(it != BitFields.end() && "Unable to find bitfield info");
return it->second;
}

void print(raw_ostream &OS) const;
void dump() const;
};

}
}

#endif
