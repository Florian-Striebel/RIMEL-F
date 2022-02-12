













#if !defined(LLVM_CLANG_CODEGEN_CGFUNCTIONINFO_H)
#define LLVM_CLANG_CODEGEN_CGFUNCTIONINFO_H

#include "clang/AST/CanonicalType.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>

namespace clang {
namespace CodeGen {



class ABIArgInfo {
public:
enum Kind : uint8_t {






Direct,



Extend,



Indirect,












IndirectAliased,



Ignore,





Expand,





CoerceAndExpand,






InAlloca,
KindFirst = Direct,
KindLast = InAlloca
};

private:
llvm::Type *TypeData;
union {
llvm::Type *PaddingType;
llvm::Type *UnpaddedCoerceAndExpandType;
};
struct DirectAttrInfo {
unsigned Offset;
unsigned Align;
};
struct IndirectAttrInfo {
unsigned Align;
unsigned AddrSpace;
};
union {
DirectAttrInfo DirectAttr;
IndirectAttrInfo IndirectAttr;
unsigned AllocaFieldIndex;
};
Kind TheKind;
bool PaddingInReg : 1;
bool InAllocaSRet : 1;
bool InAllocaIndirect : 1;
bool IndirectByVal : 1;
bool IndirectRealign : 1;
bool SRetAfterThis : 1;
bool InReg : 1;
bool CanBeFlattened: 1;
bool SignExt : 1;

bool canHavePaddingType() const {
return isDirect() || isExtend() || isIndirect() || isIndirectAliased() ||
isExpand();
}
void setPaddingType(llvm::Type *T) {
assert(canHavePaddingType());
PaddingType = T;
}

void setUnpaddedCoerceToType(llvm::Type *T) {
assert(isCoerceAndExpand());
UnpaddedCoerceAndExpandType = T;
}

public:
ABIArgInfo(Kind K = Direct)
: TypeData(nullptr), PaddingType(nullptr), DirectAttr{0, 0}, TheKind(K),
PaddingInReg(false), InAllocaSRet(false),
InAllocaIndirect(false), IndirectByVal(false), IndirectRealign(false),
SRetAfterThis(false), InReg(false), CanBeFlattened(false),
SignExt(false) {}

static ABIArgInfo getDirect(llvm::Type *T = nullptr, unsigned Offset = 0,
llvm::Type *Padding = nullptr,
bool CanBeFlattened = true, unsigned Align = 0) {
auto AI = ABIArgInfo(Direct);
AI.setCoerceToType(T);
AI.setPaddingType(Padding);
AI.setDirectOffset(Offset);
AI.setDirectAlign(Align);
AI.setCanBeFlattened(CanBeFlattened);
return AI;
}
static ABIArgInfo getDirectInReg(llvm::Type *T = nullptr) {
auto AI = getDirect(T);
AI.setInReg(true);
return AI;
}

static ABIArgInfo getSignExtend(QualType Ty, llvm::Type *T = nullptr) {
assert(Ty->isIntegralOrEnumerationType() && "Unexpected QualType");
auto AI = ABIArgInfo(Extend);
AI.setCoerceToType(T);
AI.setPaddingType(nullptr);
AI.setDirectOffset(0);
AI.setDirectAlign(0);
AI.setSignExt(true);
return AI;
}

static ABIArgInfo getZeroExtend(QualType Ty, llvm::Type *T = nullptr) {
assert(Ty->isIntegralOrEnumerationType() && "Unexpected QualType");
auto AI = ABIArgInfo(Extend);
AI.setCoerceToType(T);
AI.setPaddingType(nullptr);
AI.setDirectOffset(0);
AI.setDirectAlign(0);
AI.setSignExt(false);
return AI;
}



static ABIArgInfo getExtend(QualType Ty, llvm::Type *T = nullptr) {
assert(Ty->isIntegralOrEnumerationType() && "Unexpected QualType");
if (Ty->hasSignedIntegerRepresentation())
return getSignExtend(Ty, T);
return getZeroExtend(Ty, T);
}

static ABIArgInfo getExtendInReg(QualType Ty, llvm::Type *T = nullptr) {
auto AI = getExtend(Ty, T);
AI.setInReg(true);
return AI;
}
static ABIArgInfo getIgnore() {
return ABIArgInfo(Ignore);
}
static ABIArgInfo getIndirect(CharUnits Alignment, bool ByVal = true,
bool Realign = false,
llvm::Type *Padding = nullptr) {
auto AI = ABIArgInfo(Indirect);
AI.setIndirectAlign(Alignment);
AI.setIndirectByVal(ByVal);
AI.setIndirectRealign(Realign);
AI.setSRetAfterThis(false);
AI.setPaddingType(Padding);
return AI;
}


static ABIArgInfo getIndirectAliased(CharUnits Alignment, unsigned AddrSpace,
bool Realign = false,
llvm::Type *Padding = nullptr) {
auto AI = ABIArgInfo(IndirectAliased);
AI.setIndirectAlign(Alignment);
AI.setIndirectRealign(Realign);
AI.setPaddingType(Padding);
AI.setIndirectAddrSpace(AddrSpace);
return AI;
}

static ABIArgInfo getIndirectInReg(CharUnits Alignment, bool ByVal = true,
bool Realign = false) {
auto AI = getIndirect(Alignment, ByVal, Realign);
AI.setInReg(true);
return AI;
}
static ABIArgInfo getInAlloca(unsigned FieldIndex, bool Indirect = false) {
auto AI = ABIArgInfo(InAlloca);
AI.setInAllocaFieldIndex(FieldIndex);
AI.setInAllocaIndirect(Indirect);
return AI;
}
static ABIArgInfo getExpand() {
auto AI = ABIArgInfo(Expand);
AI.setPaddingType(nullptr);
return AI;
}
static ABIArgInfo getExpandWithPadding(bool PaddingInReg,
llvm::Type *Padding) {
auto AI = getExpand();
AI.setPaddingInReg(PaddingInReg);
AI.setPaddingType(Padding);
return AI;
}




static ABIArgInfo getCoerceAndExpand(llvm::StructType *coerceToType,
llvm::Type *unpaddedCoerceToType) {
#if !defined(NDEBUG)



auto unpaddedStruct = dyn_cast<llvm::StructType>(unpaddedCoerceToType);
assert(!unpaddedStruct || unpaddedStruct->getNumElements() != 1);



unsigned unpaddedIndex = 0;
for (auto eltType : coerceToType->elements()) {
if (isPaddingForCoerceAndExpand(eltType)) continue;
if (unpaddedStruct) {
assert(unpaddedStruct->getElementType(unpaddedIndex) == eltType);
} else {
assert(unpaddedIndex == 0 && unpaddedCoerceToType == eltType);
}
unpaddedIndex++;
}


if (unpaddedStruct) {
assert(unpaddedStruct->getNumElements() == unpaddedIndex);
} else {
assert(unpaddedIndex == 1);
}
#endif

auto AI = ABIArgInfo(CoerceAndExpand);
AI.setCoerceToType(coerceToType);
AI.setUnpaddedCoerceToType(unpaddedCoerceToType);
return AI;
}

static bool isPaddingForCoerceAndExpand(llvm::Type *eltType) {
if (eltType->isArrayTy()) {
assert(eltType->getArrayElementType()->isIntegerTy(8));
return true;
} else {
return false;
}
}

Kind getKind() const { return TheKind; }
bool isDirect() const { return TheKind == Direct; }
bool isInAlloca() const { return TheKind == InAlloca; }
bool isExtend() const { return TheKind == Extend; }
bool isIgnore() const { return TheKind == Ignore; }
bool isIndirect() const { return TheKind == Indirect; }
bool isIndirectAliased() const { return TheKind == IndirectAliased; }
bool isExpand() const { return TheKind == Expand; }
bool isCoerceAndExpand() const { return TheKind == CoerceAndExpand; }

bool canHaveCoerceToType() const {
return isDirect() || isExtend() || isCoerceAndExpand();
}


unsigned getDirectOffset() const {
assert((isDirect() || isExtend()) && "Not a direct or extend kind");
return DirectAttr.Offset;
}
void setDirectOffset(unsigned Offset) {
assert((isDirect() || isExtend()) && "Not a direct or extend kind");
DirectAttr.Offset = Offset;
}

unsigned getDirectAlign() const {
assert((isDirect() || isExtend()) && "Not a direct or extend kind");
return DirectAttr.Align;
}
void setDirectAlign(unsigned Align) {
assert((isDirect() || isExtend()) && "Not a direct or extend kind");
DirectAttr.Align = Align;
}

bool isSignExt() const {
assert(isExtend() && "Invalid kind!");
return SignExt;
}
void setSignExt(bool SExt) {
assert(isExtend() && "Invalid kind!");
SignExt = SExt;
}

llvm::Type *getPaddingType() const {
return (canHavePaddingType() ? PaddingType : nullptr);
}

bool getPaddingInReg() const {
return PaddingInReg;
}
void setPaddingInReg(bool PIR) {
PaddingInReg = PIR;
}

llvm::Type *getCoerceToType() const {
assert(canHaveCoerceToType() && "Invalid kind!");
return TypeData;
}

void setCoerceToType(llvm::Type *T) {
assert(canHaveCoerceToType() && "Invalid kind!");
TypeData = T;
}

llvm::StructType *getCoerceAndExpandType() const {
assert(isCoerceAndExpand());
return cast<llvm::StructType>(TypeData);
}

llvm::Type *getUnpaddedCoerceAndExpandType() const {
assert(isCoerceAndExpand());
return UnpaddedCoerceAndExpandType;
}

ArrayRef<llvm::Type *>getCoerceAndExpandTypeSequence() const {
assert(isCoerceAndExpand());
if (auto structTy =
dyn_cast<llvm::StructType>(UnpaddedCoerceAndExpandType)) {
return structTy->elements();
} else {
return llvm::makeArrayRef(&UnpaddedCoerceAndExpandType, 1);
}
}

bool getInReg() const {
assert((isDirect() || isExtend() || isIndirect()) && "Invalid kind!");
return InReg;
}

void setInReg(bool IR) {
assert((isDirect() || isExtend() || isIndirect()) && "Invalid kind!");
InReg = IR;
}


CharUnits getIndirectAlign() const {
assert((isIndirect() || isIndirectAliased()) && "Invalid kind!");
return CharUnits::fromQuantity(IndirectAttr.Align);
}
void setIndirectAlign(CharUnits IA) {
assert((isIndirect() || isIndirectAliased()) && "Invalid kind!");
IndirectAttr.Align = IA.getQuantity();
}

bool getIndirectByVal() const {
assert(isIndirect() && "Invalid kind!");
return IndirectByVal;
}
void setIndirectByVal(bool IBV) {
assert(isIndirect() && "Invalid kind!");
IndirectByVal = IBV;
}

unsigned getIndirectAddrSpace() const {
assert(isIndirectAliased() && "Invalid kind!");
return IndirectAttr.AddrSpace;
}

void setIndirectAddrSpace(unsigned AddrSpace) {
assert(isIndirectAliased() && "Invalid kind!");
IndirectAttr.AddrSpace = AddrSpace;
}

bool getIndirectRealign() const {
assert((isIndirect() || isIndirectAliased()) && "Invalid kind!");
return IndirectRealign;
}
void setIndirectRealign(bool IR) {
assert((isIndirect() || isIndirectAliased()) && "Invalid kind!");
IndirectRealign = IR;
}

bool isSRetAfterThis() const {
assert(isIndirect() && "Invalid kind!");
return SRetAfterThis;
}
void setSRetAfterThis(bool AfterThis) {
assert(isIndirect() && "Invalid kind!");
SRetAfterThis = AfterThis;
}

unsigned getInAllocaFieldIndex() const {
assert(isInAlloca() && "Invalid kind!");
return AllocaFieldIndex;
}
void setInAllocaFieldIndex(unsigned FieldIndex) {
assert(isInAlloca() && "Invalid kind!");
AllocaFieldIndex = FieldIndex;
}

unsigned getInAllocaIndirect() const {
assert(isInAlloca() && "Invalid kind!");
return InAllocaIndirect;
}
void setInAllocaIndirect(bool Indirect) {
assert(isInAlloca() && "Invalid kind!");
InAllocaIndirect = Indirect;
}



bool getInAllocaSRet() const {
assert(isInAlloca() && "Invalid kind!");
return InAllocaSRet;
}

void setInAllocaSRet(bool SRet) {
assert(isInAlloca() && "Invalid kind!");
InAllocaSRet = SRet;
}

bool getCanBeFlattened() const {
assert(isDirect() && "Invalid kind!");
return CanBeFlattened;
}

void setCanBeFlattened(bool Flatten) {
assert(isDirect() && "Invalid kind!");
CanBeFlattened = Flatten;
}

void dump() const;
};



class RequiredArgs {


unsigned NumRequired;
public:
enum All_t { All };

RequiredArgs(All_t _) : NumRequired(~0U) {}
explicit RequiredArgs(unsigned n) : NumRequired(n) {
assert(n != ~0U);
}






static RequiredArgs forPrototypePlus(const FunctionProtoType *prototype,
unsigned additional) {
if (!prototype->isVariadic()) return All;

if (prototype->hasExtParameterInfos())
additional += llvm::count_if(
prototype->getExtParameterInfos(),
[](const FunctionProtoType::ExtParameterInfo &ExtInfo) {
return ExtInfo.hasPassObjectSize();
});

return RequiredArgs(prototype->getNumParams() + additional);
}

static RequiredArgs forPrototypePlus(CanQual<FunctionProtoType> prototype,
unsigned additional) {
return forPrototypePlus(prototype.getTypePtr(), additional);
}

static RequiredArgs forPrototype(const FunctionProtoType *prototype) {
return forPrototypePlus(prototype, 0);
}

static RequiredArgs forPrototype(CanQual<FunctionProtoType> prototype) {
return forPrototypePlus(prototype.getTypePtr(), 0);
}

bool allowsOptionalArgs() const { return NumRequired != ~0U; }
unsigned getNumRequiredArgs() const {
assert(allowsOptionalArgs());
return NumRequired;
}

unsigned getOpaqueData() const { return NumRequired; }
static RequiredArgs getFromOpaqueData(unsigned value) {
if (value == ~0U) return All;
return RequiredArgs(value);
}
};



struct CGFunctionInfoArgInfo {
CanQualType type;
ABIArgInfo info;
};



class CGFunctionInfo final
: public llvm::FoldingSetNode,
private llvm::TrailingObjects<CGFunctionInfo, CGFunctionInfoArgInfo,
FunctionProtoType::ExtParameterInfo> {
typedef CGFunctionInfoArgInfo ArgInfo;
typedef FunctionProtoType::ExtParameterInfo ExtParameterInfo;



unsigned CallingConvention : 8;



unsigned EffectiveCallingConvention : 8;


unsigned ASTCallingConvention : 6;


unsigned InstanceMethod : 1;


unsigned ChainCall : 1;


unsigned CmseNSCall : 1;


unsigned NoReturn : 1;


unsigned ReturnsRetained : 1;


unsigned NoCallerSavedRegs : 1;


unsigned HasRegParm : 1;
unsigned RegParm : 3;


unsigned NoCfCheck : 1;

RequiredArgs Required;



llvm::StructType *ArgStruct;
unsigned ArgStructAlign : 31;
unsigned HasExtParameterInfos : 1;

unsigned NumArgs;

ArgInfo *getArgsBuffer() {
return getTrailingObjects<ArgInfo>();
}
const ArgInfo *getArgsBuffer() const {
return getTrailingObjects<ArgInfo>();
}

ExtParameterInfo *getExtParameterInfosBuffer() {
return getTrailingObjects<ExtParameterInfo>();
}
const ExtParameterInfo *getExtParameterInfosBuffer() const{
return getTrailingObjects<ExtParameterInfo>();
}

CGFunctionInfo() : Required(RequiredArgs::All) {}

public:
static CGFunctionInfo *create(unsigned llvmCC,
bool instanceMethod,
bool chainCall,
const FunctionType::ExtInfo &extInfo,
ArrayRef<ExtParameterInfo> paramInfos,
CanQualType resultType,
ArrayRef<CanQualType> argTypes,
RequiredArgs required);
void operator delete(void *p) { ::operator delete(p); }



friend class TrailingObjects;
size_t numTrailingObjects(OverloadToken<ArgInfo>) const {
return NumArgs + 1;
}
size_t numTrailingObjects(OverloadToken<ExtParameterInfo>) const {
return (HasExtParameterInfos ? NumArgs : 0);
}

typedef const ArgInfo *const_arg_iterator;
typedef ArgInfo *arg_iterator;

MutableArrayRef<ArgInfo> arguments() {
return MutableArrayRef<ArgInfo>(arg_begin(), NumArgs);
}
ArrayRef<ArgInfo> arguments() const {
return ArrayRef<ArgInfo>(arg_begin(), NumArgs);
}

const_arg_iterator arg_begin() const { return getArgsBuffer() + 1; }
const_arg_iterator arg_end() const { return getArgsBuffer() + 1 + NumArgs; }
arg_iterator arg_begin() { return getArgsBuffer() + 1; }
arg_iterator arg_end() { return getArgsBuffer() + 1 + NumArgs; }

unsigned arg_size() const { return NumArgs; }

bool isVariadic() const { return Required.allowsOptionalArgs(); }
RequiredArgs getRequiredArgs() const { return Required; }
unsigned getNumRequiredArgs() const {
return isVariadic() ? getRequiredArgs().getNumRequiredArgs() : arg_size();
}

bool isInstanceMethod() const { return InstanceMethod; }

bool isChainCall() const { return ChainCall; }

bool isCmseNSCall() const { return CmseNSCall; }

bool isNoReturn() const { return NoReturn; }



bool isReturnsRetained() const { return ReturnsRetained; }


bool isNoCallerSavedRegs() const { return NoCallerSavedRegs; }


bool isNoCfCheck() const { return NoCfCheck; }



CallingConv getASTCallingConvention() const {
return CallingConv(ASTCallingConvention);
}



unsigned getCallingConvention() const { return CallingConvention; }



unsigned getEffectiveCallingConvention() const {
return EffectiveCallingConvention;
}
void setEffectiveCallingConvention(unsigned Value) {
EffectiveCallingConvention = Value;
}

bool getHasRegParm() const { return HasRegParm; }
unsigned getRegParm() const { return RegParm; }

FunctionType::ExtInfo getExtInfo() const {
return FunctionType::ExtInfo(isNoReturn(), getHasRegParm(), getRegParm(),
getASTCallingConvention(), isReturnsRetained(),
isNoCallerSavedRegs(), isNoCfCheck(),
isCmseNSCall());
}

CanQualType getReturnType() const { return getArgsBuffer()[0].type; }

ABIArgInfo &getReturnInfo() { return getArgsBuffer()[0].info; }
const ABIArgInfo &getReturnInfo() const { return getArgsBuffer()[0].info; }

ArrayRef<ExtParameterInfo> getExtParameterInfos() const {
if (!HasExtParameterInfos) return {};
return llvm::makeArrayRef(getExtParameterInfosBuffer(), NumArgs);
}
ExtParameterInfo getExtParameterInfo(unsigned argIndex) const {
assert(argIndex <= NumArgs);
if (!HasExtParameterInfos) return ExtParameterInfo();
return getExtParameterInfos()[argIndex];
}


bool usesInAlloca() const { return ArgStruct; }


llvm::StructType *getArgStruct() const { return ArgStruct; }
CharUnits getArgStructAlignment() const {
return CharUnits::fromQuantity(ArgStructAlign);
}
void setArgStruct(llvm::StructType *Ty, CharUnits Align) {
ArgStruct = Ty;
ArgStructAlign = Align.getQuantity();
}

void Profile(llvm::FoldingSetNodeID &ID) {
ID.AddInteger(getASTCallingConvention());
ID.AddBoolean(InstanceMethod);
ID.AddBoolean(ChainCall);
ID.AddBoolean(NoReturn);
ID.AddBoolean(ReturnsRetained);
ID.AddBoolean(NoCallerSavedRegs);
ID.AddBoolean(HasRegParm);
ID.AddInteger(RegParm);
ID.AddBoolean(NoCfCheck);
ID.AddBoolean(CmseNSCall);
ID.AddInteger(Required.getOpaqueData());
ID.AddBoolean(HasExtParameterInfos);
if (HasExtParameterInfos) {
for (auto paramInfo : getExtParameterInfos())
ID.AddInteger(paramInfo.getOpaqueValue());
}
getReturnType().Profile(ID);
for (const auto &I : arguments())
I.type.Profile(ID);
}
static void Profile(llvm::FoldingSetNodeID &ID,
bool InstanceMethod,
bool ChainCall,
const FunctionType::ExtInfo &info,
ArrayRef<ExtParameterInfo> paramInfos,
RequiredArgs required,
CanQualType resultType,
ArrayRef<CanQualType> argTypes) {
ID.AddInteger(info.getCC());
ID.AddBoolean(InstanceMethod);
ID.AddBoolean(ChainCall);
ID.AddBoolean(info.getNoReturn());
ID.AddBoolean(info.getProducesResult());
ID.AddBoolean(info.getNoCallerSavedRegs());
ID.AddBoolean(info.getHasRegParm());
ID.AddInteger(info.getRegParm());
ID.AddBoolean(info.getNoCfCheck());
ID.AddBoolean(info.getCmseNSCall());
ID.AddInteger(required.getOpaqueData());
ID.AddBoolean(!paramInfos.empty());
if (!paramInfos.empty()) {
for (auto paramInfo : paramInfos)
ID.AddInteger(paramInfo.getOpaqueValue());
}
resultType.Profile(ID);
for (ArrayRef<CanQualType>::iterator
i = argTypes.begin(), e = argTypes.end(); i != e; ++i) {
i->Profile(ID);
}
}
};

}
}

#endif
