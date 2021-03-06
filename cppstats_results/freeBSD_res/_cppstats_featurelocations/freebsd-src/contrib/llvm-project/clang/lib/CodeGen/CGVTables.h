











#if !defined(LLVM_CLANG_LIB_CODEGEN_CGVTABLES_H)
#define LLVM_CLANG_LIB_CODEGEN_CGVTABLES_H

#include "clang/AST/BaseSubobject.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/GlobalDecl.h"
#include "clang/AST/VTableBuilder.h"
#include "clang/Basic/ABI.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/GlobalVariable.h"

namespace clang {
class CXXRecordDecl;

namespace CodeGen {
class CodeGenModule;
class ConstantArrayBuilder;
class ConstantStructBuilder;

class CodeGenVTables {
CodeGenModule &CGM;

VTableContextBase *VTContext;


typedef VTableLayout::AddressPointsMapTy VTableAddressPointsMapTy;

typedef std::pair<const CXXRecordDecl *, BaseSubobject> BaseSubobjectPairTy;
typedef llvm::DenseMap<BaseSubobjectPairTy, uint64_t> SubVTTIndiciesMapTy;


SubVTTIndiciesMapTy SubVTTIndicies;

typedef llvm::DenseMap<BaseSubobjectPairTy, uint64_t>
SecondaryVirtualPointerIndicesMapTy;



SecondaryVirtualPointerIndicesMapTy SecondaryVirtualPointerIndices;


llvm::Constant *PureVirtualFn = nullptr;


llvm::Constant *DeletedVirtualFn = nullptr;


llvm::Constant *maybeEmitThunk(GlobalDecl GD,
const ThunkInfo &ThunkAdjustments,
bool ForVTable);

void addVTableComponent(ConstantArrayBuilder &builder,
const VTableLayout &layout, unsigned componentIndex,
llvm::Constant *rtti, unsigned &nextVTableThunkIndex,
unsigned vtableAddressPoint,
bool vtableHasLocalLinkage);



void addRelativeComponent(ConstantArrayBuilder &builder,
llvm::Constant *component,
unsigned vtableAddressPoint,
bool vtableHasLocalLinkage,
bool isCompleteDtor) const;






llvm::Function *
getOrCreateRelativeStub(llvm::Function *func,
llvm::GlobalValue::LinkageTypes stubLinkage,
bool isCompleteDtor) const;

bool useRelativeLayout() const;

llvm::Type *getVTableComponentType() const;

public:


void createVTableInitializer(ConstantStructBuilder &builder,
const VTableLayout &layout, llvm::Constant *rtti,
bool vtableHasLocalLinkage);

CodeGenVTables(CodeGenModule &CGM);

ItaniumVTableContext &getItaniumVTableContext() {
return *cast<ItaniumVTableContext>(VTContext);
}

MicrosoftVTableContext &getMicrosoftVTableContext() {
return *cast<MicrosoftVTableContext>(VTContext);
}



uint64_t getSubVTTIndex(const CXXRecordDecl *RD, BaseSubobject Base);



uint64_t getSecondaryVirtualPointerIndex(const CXXRecordDecl *RD,
BaseSubobject Base);



llvm::GlobalVariable *
GenerateConstructionVTable(const CXXRecordDecl *RD, const BaseSubobject &Base,
bool BaseIsVirtual,
llvm::GlobalVariable::LinkageTypes Linkage,
VTableAddressPointsMapTy& AddressPoints);



llvm::GlobalVariable *GetAddrOfVTT(const CXXRecordDecl *RD);


void EmitVTTDefinition(llvm::GlobalVariable *VTT,
llvm::GlobalVariable::LinkageTypes Linkage,
const CXXRecordDecl *RD);


void EmitThunks(GlobalDecl GD);





void GenerateClassData(const CXXRecordDecl *RD);

bool isVTableExternal(const CXXRecordDecl *RD);




llvm::Type *getVTableType(const VTableLayout &layout);





void GenerateRelativeVTableAlias(llvm::GlobalVariable *VTable,
llvm::StringRef AliasNameRef);
};

}
}
#endif
