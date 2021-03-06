











#if !defined(LLVM_CLANG_AST_VTABLEBUILDER_H)
#define LLVM_CLANG_AST_VTABLEBUILDER_H

#include "clang/AST/BaseSubobject.h"
#include "clang/AST/CXXInheritance.h"
#include "clang/AST/GlobalDecl.h"
#include "clang/AST/RecordLayout.h"
#include "clang/Basic/ABI.h"
#include "clang/Basic/Thunk.h"
#include "llvm/ADT/DenseMap.h"
#include <memory>
#include <utility>

namespace clang {
class CXXRecordDecl;


class VTableComponent {
public:
enum Kind {
CK_VCallOffset,
CK_VBaseOffset,
CK_OffsetToTop,
CK_RTTI,
CK_FunctionPointer,


CK_CompleteDtorPointer,


CK_DeletingDtorPointer,






CK_UnusedFunctionPointer
};

VTableComponent() = default;

static VTableComponent MakeVCallOffset(CharUnits Offset) {
return VTableComponent(CK_VCallOffset, Offset);
}

static VTableComponent MakeVBaseOffset(CharUnits Offset) {
return VTableComponent(CK_VBaseOffset, Offset);
}

static VTableComponent MakeOffsetToTop(CharUnits Offset) {
return VTableComponent(CK_OffsetToTop, Offset);
}

static VTableComponent MakeRTTI(const CXXRecordDecl *RD) {
return VTableComponent(CK_RTTI, reinterpret_cast<uintptr_t>(RD));
}

static VTableComponent MakeFunction(const CXXMethodDecl *MD) {
assert(!isa<CXXDestructorDecl>(MD) &&
"Don't use MakeFunction with destructors!");

return VTableComponent(CK_FunctionPointer,
reinterpret_cast<uintptr_t>(MD));
}

static VTableComponent MakeCompleteDtor(const CXXDestructorDecl *DD) {
return VTableComponent(CK_CompleteDtorPointer,
reinterpret_cast<uintptr_t>(DD));
}

static VTableComponent MakeDeletingDtor(const CXXDestructorDecl *DD) {
return VTableComponent(CK_DeletingDtorPointer,
reinterpret_cast<uintptr_t>(DD));
}

static VTableComponent MakeUnusedFunction(const CXXMethodDecl *MD) {
assert(!isa<CXXDestructorDecl>(MD) &&
"Don't use MakeUnusedFunction with destructors!");
return VTableComponent(CK_UnusedFunctionPointer,
reinterpret_cast<uintptr_t>(MD));
}


Kind getKind() const {
return (Kind)(Value & 0x7);
}

CharUnits getVCallOffset() const {
assert(getKind() == CK_VCallOffset && "Invalid component kind!");

return getOffset();
}

CharUnits getVBaseOffset() const {
assert(getKind() == CK_VBaseOffset && "Invalid component kind!");

return getOffset();
}

CharUnits getOffsetToTop() const {
assert(getKind() == CK_OffsetToTop && "Invalid component kind!");

return getOffset();
}

const CXXRecordDecl *getRTTIDecl() const {
assert(isRTTIKind() && "Invalid component kind!");
return reinterpret_cast<CXXRecordDecl *>(getPointer());
}

const CXXMethodDecl *getFunctionDecl() const {
assert(isFunctionPointerKind() && "Invalid component kind!");
if (isDestructorKind())
return getDestructorDecl();
return reinterpret_cast<CXXMethodDecl *>(getPointer());
}

const CXXDestructorDecl *getDestructorDecl() const {
assert(isDestructorKind() && "Invalid component kind!");
return reinterpret_cast<CXXDestructorDecl *>(getPointer());
}

const CXXMethodDecl *getUnusedFunctionDecl() const {
assert(getKind() == CK_UnusedFunctionPointer && "Invalid component kind!");
return reinterpret_cast<CXXMethodDecl *>(getPointer());
}

bool isDestructorKind() const { return isDestructorKind(getKind()); }

bool isUsedFunctionPointerKind() const {
return isUsedFunctionPointerKind(getKind());
}

bool isFunctionPointerKind() const {
return isFunctionPointerKind(getKind());
}

bool isRTTIKind() const { return isRTTIKind(getKind()); }

GlobalDecl getGlobalDecl() const {
assert(isUsedFunctionPointerKind() &&
"GlobalDecl can be created only from virtual function");

auto *DtorDecl = dyn_cast<CXXDestructorDecl>(getFunctionDecl());
switch (getKind()) {
case CK_FunctionPointer:
return GlobalDecl(getFunctionDecl());
case CK_CompleteDtorPointer:
return GlobalDecl(DtorDecl, CXXDtorType::Dtor_Complete);
case CK_DeletingDtorPointer:
return GlobalDecl(DtorDecl, CXXDtorType::Dtor_Deleting);
case CK_VCallOffset:
case CK_VBaseOffset:
case CK_OffsetToTop:
case CK_RTTI:
case CK_UnusedFunctionPointer:
llvm_unreachable("Only function pointers kinds");
}
llvm_unreachable("Should already return");
}

private:
static bool isFunctionPointerKind(Kind ComponentKind) {
return isUsedFunctionPointerKind(ComponentKind) ||
ComponentKind == CK_UnusedFunctionPointer;
}
static bool isUsedFunctionPointerKind(Kind ComponentKind) {
return ComponentKind == CK_FunctionPointer ||
isDestructorKind(ComponentKind);
}
static bool isDestructorKind(Kind ComponentKind) {
return ComponentKind == CK_CompleteDtorPointer ||
ComponentKind == CK_DeletingDtorPointer;
}
static bool isRTTIKind(Kind ComponentKind) {
return ComponentKind == CK_RTTI;
}

VTableComponent(Kind ComponentKind, CharUnits Offset) {
assert((ComponentKind == CK_VCallOffset ||
ComponentKind == CK_VBaseOffset ||
ComponentKind == CK_OffsetToTop) && "Invalid component kind!");
assert(Offset.getQuantity() < (1LL << 56) && "Offset is too big!");
assert(Offset.getQuantity() >= -(1LL << 56) && "Offset is too small!");

Value = (uint64_t(Offset.getQuantity()) << 3) | ComponentKind;
}

VTableComponent(Kind ComponentKind, uintptr_t Ptr) {
assert((isRTTIKind(ComponentKind) || isFunctionPointerKind(ComponentKind)) &&
"Invalid component kind!");

assert((Ptr & 7) == 0 && "Pointer not sufficiently aligned!");

Value = Ptr | ComponentKind;
}

CharUnits getOffset() const {
assert((getKind() == CK_VCallOffset || getKind() == CK_VBaseOffset ||
getKind() == CK_OffsetToTop) && "Invalid component kind!");

return CharUnits::fromQuantity(Value >> 3);
}

uintptr_t getPointer() const {
assert((getKind() == CK_RTTI || isFunctionPointerKind()) &&
"Invalid component kind!");

return static_cast<uintptr_t>(Value & ~7ULL);
}






int64_t Value;
};

class VTableLayout {
public:
typedef std::pair<uint64_t, ThunkInfo> VTableThunkTy;
struct AddressPointLocation {
unsigned VTableIndex, AddressPointIndex;
};
typedef llvm::DenseMap<BaseSubobject, AddressPointLocation>
AddressPointsMapTy;




typedef llvm::SmallVector<unsigned, 4> AddressPointsIndexMapTy;

private:




OwningArrayRef<size_t> VTableIndices;

OwningArrayRef<VTableComponent> VTableComponents;


OwningArrayRef<VTableThunkTy> VTableThunks;


AddressPointsMapTy AddressPoints;


AddressPointsIndexMapTy AddressPointIndices;

public:
VTableLayout(ArrayRef<size_t> VTableIndices,
ArrayRef<VTableComponent> VTableComponents,
ArrayRef<VTableThunkTy> VTableThunks,
const AddressPointsMapTy &AddressPoints);
~VTableLayout();

ArrayRef<VTableComponent> vtable_components() const {
return VTableComponents;
}

ArrayRef<VTableThunkTy> vtable_thunks() const {
return VTableThunks;
}

AddressPointLocation getAddressPoint(BaseSubobject Base) const {
assert(AddressPoints.count(Base) && "Did not find address point!");
return AddressPoints.find(Base)->second;
}

const AddressPointsMapTy &getAddressPoints() const {
return AddressPoints;
}

const AddressPointsIndexMapTy &getAddressPointIndices() const {
return AddressPointIndices;
}

size_t getNumVTables() const {
if (VTableIndices.empty())
return 1;
return VTableIndices.size();
}

size_t getVTableOffset(size_t i) const {
if (VTableIndices.empty()) {
assert(i == 0);
return 0;
}
return VTableIndices[i];
}

size_t getVTableSize(size_t i) const {
if (VTableIndices.empty()) {
assert(i == 0);
return vtable_components().size();
}

size_t thisIndex = VTableIndices[i];
size_t nextIndex = (i + 1 == VTableIndices.size())
? vtable_components().size()
: VTableIndices[i + 1];
return nextIndex - thisIndex;
}
};

class VTableContextBase {
public:
typedef SmallVector<ThunkInfo, 1> ThunkInfoVectorTy;

bool isMicrosoft() const { return IsMicrosoftABI; }

virtual ~VTableContextBase() {}

protected:
typedef llvm::DenseMap<const CXXMethodDecl *, ThunkInfoVectorTy> ThunksMapTy;


ThunksMapTy Thunks;



virtual void computeVTableRelatedInformation(const CXXRecordDecl *RD) = 0;

VTableContextBase(bool MS) : IsMicrosoftABI(MS) {}

public:
virtual const ThunkInfoVectorTy *getThunkInfo(GlobalDecl GD) {
const CXXMethodDecl *MD = cast<CXXMethodDecl>(GD.getDecl()->getCanonicalDecl());
computeVTableRelatedInformation(MD->getParent());



ThunksMapTy::const_iterator I = Thunks.find(MD);
if (I == Thunks.end()) {

return nullptr;
}

return &I->second;
}

bool IsMicrosoftABI;


static bool hasVtableSlot(const CXXMethodDecl *MD);
};

class ItaniumVTableContext : public VTableContextBase {
private:



typedef llvm::DenseMap<GlobalDecl, int64_t> MethodVTableIndicesTy;
MethodVTableIndicesTy MethodVTableIndices;

typedef llvm::DenseMap<const CXXRecordDecl *,
std::unique_ptr<const VTableLayout>>
VTableLayoutMapTy;
VTableLayoutMapTy VTableLayouts;

typedef std::pair<const CXXRecordDecl *,
const CXXRecordDecl *> ClassPairTy;





typedef llvm::DenseMap<ClassPairTy, CharUnits>
VirtualBaseClassOffsetOffsetsMapTy;
VirtualBaseClassOffsetOffsetsMapTy VirtualBaseClassOffsetOffsets;

void computeVTableRelatedInformation(const CXXRecordDecl *RD) override;

public:
enum VTableComponentLayout {

Pointer,



Relative,
};

ItaniumVTableContext(ASTContext &Context,
VTableComponentLayout ComponentLayout = Pointer);
~ItaniumVTableContext() override;

const VTableLayout &getVTableLayout(const CXXRecordDecl *RD) {
computeVTableRelatedInformation(RD);
assert(VTableLayouts.count(RD) && "No layout for this record decl!");

return *VTableLayouts[RD];
}

std::unique_ptr<VTableLayout> createConstructionVTableLayout(
const CXXRecordDecl *MostDerivedClass, CharUnits MostDerivedClassOffset,
bool MostDerivedClassIsVirtual, const CXXRecordDecl *LayoutClass);





uint64_t getMethodVTableIndex(GlobalDecl GD);






CharUnits getVirtualBaseOffsetOffset(const CXXRecordDecl *RD,
const CXXRecordDecl *VBase);

static bool classof(const VTableContextBase *VT) {
return !VT->isMicrosoft();
}

VTableComponentLayout getVTableComponentLayout() const {
return ComponentLayout;
}

bool isPointerLayout() const { return ComponentLayout == Pointer; }
bool isRelativeLayout() const { return ComponentLayout == Relative; }

private:
VTableComponentLayout ComponentLayout;
};




struct VPtrInfo {
typedef SmallVector<const CXXRecordDecl *, 1> BasePath;

VPtrInfo(const CXXRecordDecl *RD)
: ObjectWithVPtr(RD), IntroducingObject(RD), NextBaseToMangle(RD) {}




const CXXRecordDecl *ObjectWithVPtr;



const CXXRecordDecl *IntroducingObject;



CharUnits NonVirtualOffset;





BasePath MangledPath;



const CXXRecordDecl *NextBaseToMangle;




BasePath ContainingVBases;




BasePath PathToIntroducingObject;



CharUnits FullOffsetInMDC;


const CXXRecordDecl *getVBaseWithVPtr() const {
return ContainingVBases.empty() ? nullptr : ContainingVBases.front();
}
};

typedef SmallVector<std::unique_ptr<VPtrInfo>, 2> VPtrInfoVector;




struct VirtualBaseInfo {


llvm::DenseMap<const CXXRecordDecl *, unsigned> VBTableIndices;



VPtrInfoVector VBPtrPaths;
};

struct MethodVFTableLocation {

uint64_t VBTableIndex;



const CXXRecordDecl *VBase;



CharUnits VFPtrOffset;


uint64_t Index;

MethodVFTableLocation()
: VBTableIndex(0), VBase(nullptr), VFPtrOffset(CharUnits::Zero()),
Index(0) {}

MethodVFTableLocation(uint64_t VBTableIndex, const CXXRecordDecl *VBase,
CharUnits VFPtrOffset, uint64_t Index)
: VBTableIndex(VBTableIndex), VBase(VBase), VFPtrOffset(VFPtrOffset),
Index(Index) {}

bool operator<(const MethodVFTableLocation &other) const {
if (VBTableIndex != other.VBTableIndex) {
assert(VBase != other.VBase);
return VBTableIndex < other.VBTableIndex;
}
return std::tie(VFPtrOffset, Index) <
std::tie(other.VFPtrOffset, other.Index);
}
};

class MicrosoftVTableContext : public VTableContextBase {
public:

private:
ASTContext &Context;

typedef llvm::DenseMap<GlobalDecl, MethodVFTableLocation>
MethodVFTableLocationsTy;
MethodVFTableLocationsTy MethodVFTableLocations;

typedef llvm::DenseMap<const CXXRecordDecl *, std::unique_ptr<VPtrInfoVector>>
VFPtrLocationsMapTy;
VFPtrLocationsMapTy VFPtrLocations;

typedef std::pair<const CXXRecordDecl *, CharUnits> VFTableIdTy;
typedef llvm::DenseMap<VFTableIdTy, std::unique_ptr<const VTableLayout>>
VFTableLayoutMapTy;
VFTableLayoutMapTy VFTableLayouts;

llvm::DenseMap<const CXXRecordDecl *, std::unique_ptr<VirtualBaseInfo>>
VBaseInfo;

void enumerateVFPtrs(const CXXRecordDecl *ForClass, VPtrInfoVector &Result);

void computeVTableRelatedInformation(const CXXRecordDecl *RD) override;

void dumpMethodLocations(const CXXRecordDecl *RD,
const MethodVFTableLocationsTy &NewMethods,
raw_ostream &);

const VirtualBaseInfo &
computeVBTableRelatedInformation(const CXXRecordDecl *RD);

void computeVTablePaths(bool ForVBTables, const CXXRecordDecl *RD,
VPtrInfoVector &Paths);

public:
MicrosoftVTableContext(ASTContext &Context)
: VTableContextBase(true), Context(Context) {}

~MicrosoftVTableContext() override;

const VPtrInfoVector &getVFPtrOffsets(const CXXRecordDecl *RD);

const VTableLayout &getVFTableLayout(const CXXRecordDecl *RD,
CharUnits VFPtrOffset);

MethodVFTableLocation getMethodVFTableLocation(GlobalDecl GD);

const ThunkInfoVectorTy *getThunkInfo(GlobalDecl GD) override {

if (isa<CXXDestructorDecl>(GD.getDecl()) &&
GD.getDtorType() == Dtor_Complete)
return nullptr;
return VTableContextBase::getThunkInfo(GD);
}





unsigned getVBTableIndex(const CXXRecordDecl *Derived,
const CXXRecordDecl *VBase);

const VPtrInfoVector &enumerateVBTables(const CXXRecordDecl *RD);

static bool classof(const VTableContextBase *VT) { return VT->isMicrosoft(); }
};

}

#endif
