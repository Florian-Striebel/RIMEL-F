












#if !defined(LLVM_CLANG_LIB_CODEGEN_CGVALUE_H)
#define LLVM_CLANG_LIB_CODEGEN_CGVALUE_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "Address.h"
#include "CodeGenTBAA.h"

namespace llvm {
class Constant;
class MDNode;
}

namespace clang {
namespace CodeGen {
class AggValueSlot;
class CodeGenFunction;
struct CGBitFieldInfo;





class RValue {
enum Flavor { Scalar, Complex, Aggregate };



enum { AggAlignShift = 4 };


llvm::PointerIntPair<llvm::Value *, 2, Flavor> V1;

llvm::PointerIntPair<llvm::Value *, 1, bool> V2;

public:
bool isScalar() const { return V1.getInt() == Scalar; }
bool isComplex() const { return V1.getInt() == Complex; }
bool isAggregate() const { return V1.getInt() == Aggregate; }

bool isVolatileQualified() const { return V2.getInt(); }


llvm::Value *getScalarVal() const {
assert(isScalar() && "Not a scalar!");
return V1.getPointer();
}



std::pair<llvm::Value *, llvm::Value *> getComplexVal() const {
return std::make_pair(V1.getPointer(), V2.getPointer());
}


Address getAggregateAddress() const {
assert(isAggregate() && "Not an aggregate!");
auto align = reinterpret_cast<uintptr_t>(V2.getPointer()) >> AggAlignShift;
return Address(V1.getPointer(), CharUnits::fromQuantity(align));
}
llvm::Value *getAggregatePointer() const {
assert(isAggregate() && "Not an aggregate!");
return V1.getPointer();
}

static RValue getIgnored() {

return get(nullptr);
}

static RValue get(llvm::Value *V) {
RValue ER;
ER.V1.setPointer(V);
ER.V1.setInt(Scalar);
ER.V2.setInt(false);
return ER;
}
static RValue getComplex(llvm::Value *V1, llvm::Value *V2) {
RValue ER;
ER.V1.setPointer(V1);
ER.V2.setPointer(V2);
ER.V1.setInt(Complex);
ER.V2.setInt(false);
return ER;
}
static RValue getComplex(const std::pair<llvm::Value *, llvm::Value *> &C) {
return getComplex(C.first, C.second);
}



static RValue getAggregate(Address addr, bool isVolatile = false) {
RValue ER;
ER.V1.setPointer(addr.getPointer());
ER.V1.setInt(Aggregate);

auto align = static_cast<uintptr_t>(addr.getAlignment().getQuantity());
ER.V2.setPointer(reinterpret_cast<llvm::Value*>(align << AggAlignShift));
ER.V2.setInt(isVolatile);
return ER;
}
};


enum ARCPreciseLifetime_t {
ARCImpreciseLifetime, ARCPreciseLifetime
};



enum class AlignmentSource {



Decl,




AttributedType,



Type
};



static inline AlignmentSource getFieldAlignmentSource(AlignmentSource Source) {


return AlignmentSource::Decl;
}

class LValueBaseInfo {
AlignmentSource AlignSource;

public:
explicit LValueBaseInfo(AlignmentSource Source = AlignmentSource::Type)
: AlignSource(Source) {}
AlignmentSource getAlignmentSource() const { return AlignSource; }
void setAlignmentSource(AlignmentSource Source) { AlignSource = Source; }

void mergeForCast(const LValueBaseInfo &Info) {
setAlignmentSource(Info.getAlignmentSource());
}
};




class LValue {
enum {
Simple,
VectorElt,
BitField,
ExtVectorElt,
GlobalReg,
MatrixElt
} LVType;

llvm::Value *V;

union {

llvm::Value *VectorIdx;


llvm::Constant *VectorElts;


const CGBitFieldInfo *BitFieldInfo;
};

QualType Type;


Qualifiers Quals;



unsigned Alignment;


bool Ivar:1;


bool ObjIsArray:1;



bool NonGC: 1;


bool GlobalObjCRef : 1;


bool ThreadLocalRef : 1;



bool ImpreciseLifetime : 1;



bool Nontemporal : 1;

LValueBaseInfo BaseInfo;
TBAAAccessInfo TBAAInfo;

Expr *BaseIvarExp;

private:
void Initialize(QualType Type, Qualifiers Quals, CharUnits Alignment,
LValueBaseInfo BaseInfo, TBAAAccessInfo TBAAInfo) {
assert((!Alignment.isZero() || Type->isIncompleteType()) &&
"initializing l-value with zero alignment!");
this->Type = Type;
this->Quals = Quals;
const unsigned MaxAlign = 1U << 31;
this->Alignment = Alignment.getQuantity() <= MaxAlign
? Alignment.getQuantity()
: MaxAlign;
assert(this->Alignment == Alignment.getQuantity() &&
"Alignment exceeds allowed max!");
this->BaseInfo = BaseInfo;
this->TBAAInfo = TBAAInfo;


this->Ivar = this->ObjIsArray = this->NonGC = this->GlobalObjCRef = false;
this->ImpreciseLifetime = false;
this->Nontemporal = false;
this->ThreadLocalRef = false;
this->BaseIvarExp = nullptr;
}

public:
bool isSimple() const { return LVType == Simple; }
bool isVectorElt() const { return LVType == VectorElt; }
bool isBitField() const { return LVType == BitField; }
bool isExtVectorElt() const { return LVType == ExtVectorElt; }
bool isGlobalReg() const { return LVType == GlobalReg; }
bool isMatrixElt() const { return LVType == MatrixElt; }

bool isVolatileQualified() const { return Quals.hasVolatile(); }
bool isRestrictQualified() const { return Quals.hasRestrict(); }
unsigned getVRQualifiers() const {
return Quals.getCVRQualifiers() & ~Qualifiers::Const;
}

QualType getType() const { return Type; }

Qualifiers::ObjCLifetime getObjCLifetime() const {
return Quals.getObjCLifetime();
}

bool isObjCIvar() const { return Ivar; }
void setObjCIvar(bool Value) { Ivar = Value; }

bool isObjCArray() const { return ObjIsArray; }
void setObjCArray(bool Value) { ObjIsArray = Value; }

bool isNonGC () const { return NonGC; }
void setNonGC(bool Value) { NonGC = Value; }

bool isGlobalObjCRef() const { return GlobalObjCRef; }
void setGlobalObjCRef(bool Value) { GlobalObjCRef = Value; }

bool isThreadLocalRef() const { return ThreadLocalRef; }
void setThreadLocalRef(bool Value) { ThreadLocalRef = Value;}

ARCPreciseLifetime_t isARCPreciseLifetime() const {
return ARCPreciseLifetime_t(!ImpreciseLifetime);
}
void setARCPreciseLifetime(ARCPreciseLifetime_t value) {
ImpreciseLifetime = (value == ARCImpreciseLifetime);
}
bool isNontemporal() const { return Nontemporal; }
void setNontemporal(bool Value) { Nontemporal = Value; }

bool isObjCWeak() const {
return Quals.getObjCGCAttr() == Qualifiers::Weak;
}
bool isObjCStrong() const {
return Quals.getObjCGCAttr() == Qualifiers::Strong;
}

bool isVolatile() const {
return Quals.hasVolatile();
}

Expr *getBaseIvarExp() const { return BaseIvarExp; }
void setBaseIvarExp(Expr *V) { BaseIvarExp = V; }

TBAAAccessInfo getTBAAInfo() const { return TBAAInfo; }
void setTBAAInfo(TBAAAccessInfo Info) { TBAAInfo = Info; }

const Qualifiers &getQuals() const { return Quals; }
Qualifiers &getQuals() { return Quals; }

LangAS getAddressSpace() const { return Quals.getAddressSpace(); }

CharUnits getAlignment() const { return CharUnits::fromQuantity(Alignment); }
void setAlignment(CharUnits A) { Alignment = A.getQuantity(); }

LValueBaseInfo getBaseInfo() const { return BaseInfo; }
void setBaseInfo(LValueBaseInfo Info) { BaseInfo = Info; }


llvm::Value *getPointer(CodeGenFunction &CGF) const {
assert(isSimple());
return V;
}
Address getAddress(CodeGenFunction &CGF) const {
return Address(getPointer(CGF), getAlignment());
}
void setAddress(Address address) {
assert(isSimple());
V = address.getPointer();
Alignment = address.getAlignment().getQuantity();
}


Address getVectorAddress() const {
return Address(getVectorPointer(), getAlignment());
}
llvm::Value *getVectorPointer() const {
assert(isVectorElt());
return V;
}
llvm::Value *getVectorIdx() const {
assert(isVectorElt());
return VectorIdx;
}

Address getMatrixAddress() const {
return Address(getMatrixPointer(), getAlignment());
}
llvm::Value *getMatrixPointer() const {
assert(isMatrixElt());
return V;
}
llvm::Value *getMatrixIdx() const {
assert(isMatrixElt());
return VectorIdx;
}


Address getExtVectorAddress() const {
return Address(getExtVectorPointer(), getAlignment());
}
llvm::Value *getExtVectorPointer() const {
assert(isExtVectorElt());
return V;
}
llvm::Constant *getExtVectorElts() const {
assert(isExtVectorElt());
return VectorElts;
}


Address getBitFieldAddress() const {
return Address(getBitFieldPointer(), getAlignment());
}
llvm::Value *getBitFieldPointer() const { assert(isBitField()); return V; }
const CGBitFieldInfo &getBitFieldInfo() const {
assert(isBitField());
return *BitFieldInfo;
}


llvm::Value *getGlobalReg() const { assert(isGlobalReg()); return V; }

static LValue MakeAddr(Address address, QualType type, ASTContext &Context,
LValueBaseInfo BaseInfo, TBAAAccessInfo TBAAInfo) {
Qualifiers qs = type.getQualifiers();
qs.setObjCGCAttr(Context.getObjCGCAttrKind(type));

LValue R;
R.LVType = Simple;
assert(address.getPointer()->getType()->isPointerTy());
R.V = address.getPointer();
R.Initialize(type, qs, address.getAlignment(), BaseInfo, TBAAInfo);
return R;
}

static LValue MakeVectorElt(Address vecAddress, llvm::Value *Idx,
QualType type, LValueBaseInfo BaseInfo,
TBAAAccessInfo TBAAInfo) {
LValue R;
R.LVType = VectorElt;
R.V = vecAddress.getPointer();
R.VectorIdx = Idx;
R.Initialize(type, type.getQualifiers(), vecAddress.getAlignment(),
BaseInfo, TBAAInfo);
return R;
}

static LValue MakeExtVectorElt(Address vecAddress, llvm::Constant *Elts,
QualType type, LValueBaseInfo BaseInfo,
TBAAAccessInfo TBAAInfo) {
LValue R;
R.LVType = ExtVectorElt;
R.V = vecAddress.getPointer();
R.VectorElts = Elts;
R.Initialize(type, type.getQualifiers(), vecAddress.getAlignment(),
BaseInfo, TBAAInfo);
return R;
}







static LValue MakeBitfield(Address Addr, const CGBitFieldInfo &Info,
QualType type, LValueBaseInfo BaseInfo,
TBAAAccessInfo TBAAInfo) {
LValue R;
R.LVType = BitField;
R.V = Addr.getPointer();
R.BitFieldInfo = &Info;
R.Initialize(type, type.getQualifiers(), Addr.getAlignment(), BaseInfo,
TBAAInfo);
return R;
}

static LValue MakeGlobalReg(Address Reg, QualType type) {
LValue R;
R.LVType = GlobalReg;
R.V = Reg.getPointer();
R.Initialize(type, type.getQualifiers(), Reg.getAlignment(),
LValueBaseInfo(AlignmentSource::Decl), TBAAAccessInfo());
return R;
}

static LValue MakeMatrixElt(Address matAddress, llvm::Value *Idx,
QualType type, LValueBaseInfo BaseInfo,
TBAAAccessInfo TBAAInfo) {
LValue R;
R.LVType = MatrixElt;
R.V = matAddress.getPointer();
R.VectorIdx = Idx;
R.Initialize(type, type.getQualifiers(), matAddress.getAlignment(),
BaseInfo, TBAAInfo);
return R;
}

RValue asAggregateRValue(CodeGenFunction &CGF) const {
return RValue::getAggregate(getAddress(CGF), isVolatileQualified());
}
};


class AggValueSlot {

llvm::Value *Addr;


Qualifiers Quals;

unsigned Alignment;




bool DestructedFlag : 1;




bool ObjCGCFlag : 1;




bool ZeroedFlag : 1;













bool AliasedFlag : 1;






bool OverlapFlag : 1;






bool SanitizerCheckedFlag : 1;

public:
enum IsAliased_t { IsNotAliased, IsAliased };
enum IsDestructed_t { IsNotDestructed, IsDestructed };
enum IsZeroed_t { IsNotZeroed, IsZeroed };
enum Overlap_t { DoesNotOverlap, MayOverlap };
enum NeedsGCBarriers_t { DoesNotNeedGCBarriers, NeedsGCBarriers };
enum IsSanitizerChecked_t { IsNotSanitizerChecked, IsSanitizerChecked };



static AggValueSlot ignored() {
return forAddr(Address::invalid(), Qualifiers(), IsNotDestructed,
DoesNotNeedGCBarriers, IsNotAliased, DoesNotOverlap);
}











static AggValueSlot forAddr(Address addr,
Qualifiers quals,
IsDestructed_t isDestructed,
NeedsGCBarriers_t needsGC,
IsAliased_t isAliased,
Overlap_t mayOverlap,
IsZeroed_t isZeroed = IsNotZeroed,
IsSanitizerChecked_t isChecked = IsNotSanitizerChecked) {
AggValueSlot AV;
if (addr.isValid()) {
AV.Addr = addr.getPointer();
AV.Alignment = addr.getAlignment().getQuantity();
} else {
AV.Addr = nullptr;
AV.Alignment = 0;
}
AV.Quals = quals;
AV.DestructedFlag = isDestructed;
AV.ObjCGCFlag = needsGC;
AV.ZeroedFlag = isZeroed;
AV.AliasedFlag = isAliased;
AV.OverlapFlag = mayOverlap;
AV.SanitizerCheckedFlag = isChecked;
return AV;
}

static AggValueSlot
forLValue(const LValue &LV, CodeGenFunction &CGF, IsDestructed_t isDestructed,
NeedsGCBarriers_t needsGC, IsAliased_t isAliased,
Overlap_t mayOverlap, IsZeroed_t isZeroed = IsNotZeroed,
IsSanitizerChecked_t isChecked = IsNotSanitizerChecked) {
return forAddr(LV.getAddress(CGF), LV.getQuals(), isDestructed, needsGC,
isAliased, mayOverlap, isZeroed, isChecked);
}

IsDestructed_t isExternallyDestructed() const {
return IsDestructed_t(DestructedFlag);
}
void setExternallyDestructed(bool destructed = true) {
DestructedFlag = destructed;
}

Qualifiers getQualifiers() const { return Quals; }

bool isVolatile() const {
return Quals.hasVolatile();
}

void setVolatile(bool flag) {
if (flag)
Quals.addVolatile();
else
Quals.removeVolatile();
}

Qualifiers::ObjCLifetime getObjCLifetime() const {
return Quals.getObjCLifetime();
}

NeedsGCBarriers_t requiresGCollection() const {
return NeedsGCBarriers_t(ObjCGCFlag);
}

llvm::Value *getPointer() const {
return Addr;
}

Address getAddress() const {
return Address(Addr, getAlignment());
}

bool isIgnored() const {
return Addr == nullptr;
}

CharUnits getAlignment() const {
return CharUnits::fromQuantity(Alignment);
}

IsAliased_t isPotentiallyAliased() const {
return IsAliased_t(AliasedFlag);
}

Overlap_t mayOverlap() const {
return Overlap_t(OverlapFlag);
}

bool isSanitizerChecked() const {
return SanitizerCheckedFlag;
}

RValue asRValue() const {
if (isIgnored()) {
return RValue::getIgnored();
} else {
return RValue::getAggregate(getAddress(), isVolatile());
}
}

void setZeroed(bool V = true) { ZeroedFlag = V; }
IsZeroed_t isZeroed() const {
return IsZeroed_t(ZeroedFlag);
}




CharUnits getPreferredSize(ASTContext &Ctx, QualType Type) const {
return mayOverlap() ? Ctx.getTypeInfoDataSizeInChars(Type).Width
: Ctx.getTypeSizeInChars(Type);
}
};

}
}

#endif
