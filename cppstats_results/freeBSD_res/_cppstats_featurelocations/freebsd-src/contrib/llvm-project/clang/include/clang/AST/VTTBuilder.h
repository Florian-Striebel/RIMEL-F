












#if !defined(LLVM_CLANG_AST_VTTBUILDER_H)
#define LLVM_CLANG_AST_VTTBUILDER_H

#include "clang/AST/BaseSubobject.h"
#include "clang/AST/CharUnits.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include <cstdint>

namespace clang {

class ASTContext;
class ASTRecordLayout;
class CXXRecordDecl;

class VTTVTable {
llvm::PointerIntPair<const CXXRecordDecl *, 1, bool> BaseAndIsVirtual;
CharUnits BaseOffset;

public:
VTTVTable() = default;
VTTVTable(const CXXRecordDecl *Base, CharUnits BaseOffset, bool BaseIsVirtual)
: BaseAndIsVirtual(Base, BaseIsVirtual), BaseOffset(BaseOffset) {}
VTTVTable(BaseSubobject Base, bool BaseIsVirtual)
: BaseAndIsVirtual(Base.getBase(), BaseIsVirtual),
BaseOffset(Base.getBaseOffset()) {}

const CXXRecordDecl *getBase() const {
return BaseAndIsVirtual.getPointer();
}

CharUnits getBaseOffset() const {
return BaseOffset;
}

bool isVirtual() const {
return BaseAndIsVirtual.getInt();
}

BaseSubobject getBaseSubobject() const {
return BaseSubobject(getBase(), getBaseOffset());
}
};

struct VTTComponent {
uint64_t VTableIndex;
BaseSubobject VTableBase;

VTTComponent() = default;
VTTComponent(uint64_t VTableIndex, BaseSubobject VTableBase)
: VTableIndex(VTableIndex), VTableBase(VTableBase) {}
};


class VTTBuilder {
ASTContext &Ctx;


const CXXRecordDecl *MostDerivedClass;

using VTTVTablesVectorTy = SmallVector<VTTVTable, 64>;


VTTVTablesVectorTy VTTVTables;

using VTTComponentsVectorTy = SmallVector<VTTComponent, 64>;


VTTComponentsVectorTy VTTComponents;


const ASTRecordLayout &MostDerivedClassLayout;

using VisitedVirtualBasesSetTy = llvm::SmallPtrSet<const CXXRecordDecl *, 4>;

using AddressPointsMapTy = llvm::DenseMap<BaseSubobject, uint64_t>;


llvm::DenseMap<BaseSubobject, uint64_t> SubVTTIndicies;



llvm::DenseMap<BaseSubobject, uint64_t> SecondaryVirtualPointerIndices;


bool GenerateDefinition;


void AddVTablePointer(BaseSubobject Base, uint64_t VTableIndex,
const CXXRecordDecl *VTableClass);


void LayoutSecondaryVTTs(BaseSubobject Base);






void LayoutSecondaryVirtualPointers(BaseSubobject Base,
bool BaseIsMorallyVirtual,
uint64_t VTableIndex,
const CXXRecordDecl *VTableClass,
VisitedVirtualBasesSetTy &VBases);



void LayoutSecondaryVirtualPointers(BaseSubobject Base,
uint64_t VTableIndex);



void LayoutVirtualVTTs(const CXXRecordDecl *RD,
VisitedVirtualBasesSetTy &VBases);



void LayoutVTT(BaseSubobject Base, bool BaseIsVirtual);

public:
VTTBuilder(ASTContext &Ctx, const CXXRecordDecl *MostDerivedClass,
bool GenerateDefinition);


const VTTComponentsVectorTy &getVTTComponents() const {
return VTTComponents;
}


const VTTVTablesVectorTy &getVTTVTables() const {
return VTTVTables;
}


const llvm::DenseMap<BaseSubobject, uint64_t> &getSubVTTIndicies() const {
return SubVTTIndicies;
}


const llvm::DenseMap<BaseSubobject, uint64_t> &
getSecondaryVirtualPointerIndices() const {
return SecondaryVirtualPointerIndices;
}
};

}

#endif
