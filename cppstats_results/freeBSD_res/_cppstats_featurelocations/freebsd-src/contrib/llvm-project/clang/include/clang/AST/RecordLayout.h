











#if !defined(LLVM_CLANG_AST_RECORDLAYOUT_H)
#define LLVM_CLANG_AST_RECORDLAYOUT_H

#include "clang/AST/ASTVector.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/DeclCXX.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include <cassert>
#include <cstdint>

namespace clang {

class ASTContext;
class CXXRecordDecl;








class ASTRecordLayout {
public:
struct VBaseInfo {


CharUnits VBaseOffset;

private:



bool HasVtorDisp = false;

public:
VBaseInfo() = default;
VBaseInfo(CharUnits VBaseOffset, bool hasVtorDisp)
: VBaseOffset(VBaseOffset), HasVtorDisp(hasVtorDisp) {}

bool hasVtorDisp() const { return HasVtorDisp; }
};

using VBaseOffsetsMapTy = llvm::DenseMap<const CXXRecordDecl *, VBaseInfo>;

private:
friend class ASTContext;


CharUnits Size;


CharUnits DataSize;


CharUnits Alignment;




CharUnits PreferredAlignment;



CharUnits UnadjustedAlignment;



CharUnits RequiredAlignment;


ASTVector<uint64_t> FieldOffsets;


struct CXXRecordLayoutInfo {


CharUnits NonVirtualSize;



CharUnits NonVirtualAlignment;




CharUnits PreferredNVAlignment;




CharUnits SizeOfLargestEmptySubobject;


CharUnits VBPtrOffset;




bool HasOwnVFPtr : 1;




bool HasExtendableVFPtr : 1;




bool EndsWithZeroSizedObject : 1;



bool LeadsWithZeroSizedBase : 1;


llvm::PointerIntPair<const CXXRecordDecl *, 1, bool> PrimaryBase;


const CXXRecordDecl *BaseSharingVBPtr;


using BaseOffsetsMapTy = llvm::DenseMap<const CXXRecordDecl *, CharUnits>;


BaseOffsetsMapTy BaseOffsets;


VBaseOffsetsMapTy VBaseOffsets;
};



CXXRecordLayoutInfo *CXXInfo = nullptr;

ASTRecordLayout(const ASTContext &Ctx, CharUnits size, CharUnits alignment,
CharUnits preferredAlignment, CharUnits unadjustedAlignment,
CharUnits requiredAlignment, CharUnits datasize,
ArrayRef<uint64_t> fieldoffsets);

using BaseOffsetsMapTy = CXXRecordLayoutInfo::BaseOffsetsMapTy;


ASTRecordLayout(const ASTContext &Ctx, CharUnits size, CharUnits alignment,
CharUnits preferredAlignment, CharUnits unadjustedAlignment,
CharUnits requiredAlignment, bool hasOwnVFPtr,
bool hasExtendableVFPtr, CharUnits vbptroffset,
CharUnits datasize, ArrayRef<uint64_t> fieldoffsets,
CharUnits nonvirtualsize, CharUnits nonvirtualalignment,
CharUnits preferrednvalignment,
CharUnits SizeOfLargestEmptySubobject,
const CXXRecordDecl *PrimaryBase, bool IsPrimaryBaseVirtual,
const CXXRecordDecl *BaseSharingVBPtr,
bool EndsWithZeroSizedObject, bool LeadsWithZeroSizedBase,
const BaseOffsetsMapTy &BaseOffsets,
const VBaseOffsetsMapTy &VBaseOffsets);

~ASTRecordLayout() = default;

void Destroy(ASTContext &Ctx);

public:
ASTRecordLayout(const ASTRecordLayout &) = delete;
ASTRecordLayout &operator=(const ASTRecordLayout &) = delete;


CharUnits getAlignment() const { return Alignment; }



CharUnits getPreferredAlignment() const { return PreferredAlignment; }



CharUnits getUnadjustedAlignment() const { return UnadjustedAlignment; }


CharUnits getSize() const { return Size; }


unsigned getFieldCount() const { return FieldOffsets.size(); }



uint64_t getFieldOffset(unsigned FieldNo) const {
return FieldOffsets[FieldNo];
}



CharUnits getDataSize() const { return DataSize; }



CharUnits getNonVirtualSize() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");

return CXXInfo->NonVirtualSize;
}



CharUnits getNonVirtualAlignment() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");

return CXXInfo->NonVirtualAlignment;
}




CharUnits getPreferredNVAlignment() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");

return CXXInfo->PreferredNVAlignment;
}


const CXXRecordDecl *getPrimaryBase() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");

return CXXInfo->PrimaryBase.getPointer();
}



bool isPrimaryBaseVirtual() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");

return CXXInfo->PrimaryBase.getInt();
}


CharUnits getBaseClassOffset(const CXXRecordDecl *Base) const {
assert(CXXInfo && "Record layout does not have C++ specific info!");

Base = Base->getDefinition();
assert(CXXInfo->BaseOffsets.count(Base) && "Did not find base!");

return CXXInfo->BaseOffsets[Base];
}


CharUnits getVBaseClassOffset(const CXXRecordDecl *VBase) const {
assert(CXXInfo && "Record layout does not have C++ specific info!");

VBase = VBase->getDefinition();
assert(CXXInfo->VBaseOffsets.count(VBase) && "Did not find base!");

return CXXInfo->VBaseOffsets[VBase].VBaseOffset;
}

CharUnits getSizeOfLargestEmptySubobject() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return CXXInfo->SizeOfLargestEmptySubobject;
}








bool hasOwnVFPtr() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return CXXInfo->HasOwnVFPtr;
}




bool hasExtendableVFPtr() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return CXXInfo->HasExtendableVFPtr;
}








bool hasOwnVBPtr() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return hasVBPtr() && !CXXInfo->BaseSharingVBPtr;
}


bool hasVBPtr() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return !CXXInfo->VBPtrOffset.isNegative();
}

CharUnits getRequiredAlignment() const { return RequiredAlignment; }

bool endsWithZeroSizedObject() const {
return CXXInfo && CXXInfo->EndsWithZeroSizedObject;
}

bool leadsWithZeroSizedBase() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return CXXInfo->LeadsWithZeroSizedBase;
}



CharUnits getVBPtrOffset() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return CXXInfo->VBPtrOffset;
}

const CXXRecordDecl *getBaseSharingVBPtr() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return CXXInfo->BaseSharingVBPtr;
}

const VBaseOffsetsMapTy &getVBaseOffsetsMap() const {
assert(CXXInfo && "Record layout does not have C++ specific info!");
return CXXInfo->VBaseOffsets;
}
};

}

#endif
