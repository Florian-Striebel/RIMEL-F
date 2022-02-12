







#if !defined(LLVM_CLANG_APINOTES_TYPES_H)
#define LLVM_CLANG_APINOTES_TYPES_H

#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include <climits>
#include <vector>

namespace clang {
namespace api_notes {
enum class RetainCountConventionKind {
None,
CFReturnsRetained,
CFReturnsNotRetained,
NSReturnsRetained,
NSReturnsNotRetained,
};




enum class EnumExtensibilityKind {
None,
Open,
Closed,
};


enum class SwiftNewTypeKind {
None,
Struct,
Enum,
};




class CommonEntityInfo {
public:

std::string UnavailableMsg;


unsigned Unavailable : 1;


unsigned UnavailableInSwift : 1;

private:

unsigned SwiftPrivateSpecified : 1;


unsigned SwiftPrivate : 1;

public:

std::string SwiftName;

CommonEntityInfo()
: Unavailable(0), UnavailableInSwift(0), SwiftPrivateSpecified(0),
SwiftPrivate(0) {}

llvm::Optional<bool> isSwiftPrivate() const {
return SwiftPrivateSpecified ? llvm::Optional<bool>(SwiftPrivate)
: llvm::None;
}

void setSwiftPrivate(llvm::Optional<bool> Private) {
SwiftPrivateSpecified = Private.hasValue();
SwiftPrivate = Private.hasValue() ? *Private : 0;
}

friend bool operator==(const CommonEntityInfo &, const CommonEntityInfo &);

CommonEntityInfo &operator|=(const CommonEntityInfo &RHS) {

if (RHS.Unavailable) {
Unavailable = true;
if (UnavailableMsg.empty())
UnavailableMsg = RHS.UnavailableMsg;
}

if (RHS.UnavailableInSwift) {
UnavailableInSwift = true;
if (UnavailableMsg.empty())
UnavailableMsg = RHS.UnavailableMsg;
}

if (!SwiftPrivateSpecified)
setSwiftPrivate(RHS.isSwiftPrivate());

if (SwiftName.empty())
SwiftName = RHS.SwiftName;

return *this;
}

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS) const;
};

inline bool operator==(const CommonEntityInfo &LHS,
const CommonEntityInfo &RHS) {
return LHS.UnavailableMsg == RHS.UnavailableMsg &&
LHS.Unavailable == RHS.Unavailable &&
LHS.UnavailableInSwift == RHS.UnavailableInSwift &&
LHS.SwiftPrivateSpecified == RHS.SwiftPrivateSpecified &&
LHS.SwiftPrivate == RHS.SwiftPrivate && LHS.SwiftName == RHS.SwiftName;
}

inline bool operator!=(const CommonEntityInfo &LHS,
const CommonEntityInfo &RHS) {
return !(LHS == RHS);
}


class CommonTypeInfo : public CommonEntityInfo {



llvm::Optional<std::string> SwiftBridge;


llvm::Optional<std::string> NSErrorDomain;

public:
CommonTypeInfo() : CommonEntityInfo() {}

const llvm::Optional<std::string> &getSwiftBridge() const {
return SwiftBridge;
}

void setSwiftBridge(const llvm::Optional<std::string> &SwiftType) {
SwiftBridge = SwiftType;
}

void setSwiftBridge(const llvm::Optional<llvm::StringRef> &SwiftType) {
SwiftBridge = SwiftType
? llvm::Optional<std::string>(std::string(*SwiftType))
: llvm::None;
}

const llvm::Optional<std::string> &getNSErrorDomain() const {
return NSErrorDomain;
}

void setNSErrorDomain(const llvm::Optional<std::string> &Domain) {
NSErrorDomain = Domain;
}

void setNSErrorDomain(const llvm::Optional<llvm::StringRef> &Domain) {
NSErrorDomain =
Domain ? llvm::Optional<std::string>(std::string(*Domain)) : llvm::None;
}

friend bool operator==(const CommonTypeInfo &, const CommonTypeInfo &);

CommonTypeInfo &operator|=(const CommonTypeInfo &RHS) {

static_cast<CommonEntityInfo &>(*this) |= RHS;

if (!SwiftBridge)
setSwiftBridge(RHS.getSwiftBridge());
if (!NSErrorDomain)
setNSErrorDomain(RHS.getNSErrorDomain());

return *this;
}

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS) const;
};

inline bool operator==(const CommonTypeInfo &LHS, const CommonTypeInfo &RHS) {
return static_cast<const CommonEntityInfo &>(LHS) == RHS &&
LHS.SwiftBridge == RHS.SwiftBridge &&
LHS.NSErrorDomain == RHS.NSErrorDomain;
}

inline bool operator!=(const CommonTypeInfo &LHS, const CommonTypeInfo &RHS) {
return !(LHS == RHS);
}


class ObjCContextInfo : public CommonTypeInfo {

unsigned HasDefaultNullability : 1;


unsigned DefaultNullability : 2;


unsigned HasDesignatedInits : 1;

unsigned SwiftImportAsNonGenericSpecified : 1;
unsigned SwiftImportAsNonGeneric : 1;

unsigned SwiftObjCMembersSpecified : 1;
unsigned SwiftObjCMembers : 1;

public:
ObjCContextInfo()
: CommonTypeInfo(), HasDefaultNullability(0), DefaultNullability(0),
HasDesignatedInits(0), SwiftImportAsNonGenericSpecified(false),
SwiftImportAsNonGeneric(false), SwiftObjCMembersSpecified(false),
SwiftObjCMembers(false) {}





llvm::Optional<NullabilityKind> getDefaultNullability() const {
return HasDefaultNullability
? llvm::Optional<NullabilityKind>(
static_cast<NullabilityKind>(DefaultNullability))
: llvm::None;
}


void setDefaultNullability(NullabilityKind Kind) {
HasDefaultNullability = true;
DefaultNullability = static_cast<unsigned>(Kind);
}

bool hasDesignatedInits() const { return HasDesignatedInits; }
void setHasDesignatedInits(bool Value) { HasDesignatedInits = Value; }

llvm::Optional<bool> getSwiftImportAsNonGeneric() const {
return SwiftImportAsNonGenericSpecified
? llvm::Optional<bool>(SwiftImportAsNonGeneric)
: llvm::None;
}
void setSwiftImportAsNonGeneric(llvm::Optional<bool> Value) {
SwiftImportAsNonGenericSpecified = Value.hasValue();
SwiftImportAsNonGeneric = Value.hasValue() ? *Value : false;
}

llvm::Optional<bool> getSwiftObjCMembers() const {
return SwiftObjCMembersSpecified ? llvm::Optional<bool>(SwiftObjCMembers)
: llvm::None;
}
void setSwiftObjCMembers(llvm::Optional<bool> Value) {
SwiftObjCMembersSpecified = Value.hasValue();
SwiftObjCMembers = Value.hasValue() ? *Value : false;
}



void stripModuleLocalInfo() {
HasDefaultNullability = false;
DefaultNullability = 0;
}

friend bool operator==(const ObjCContextInfo &, const ObjCContextInfo &);

ObjCContextInfo &operator|=(const ObjCContextInfo &RHS) {

static_cast<CommonTypeInfo &>(*this) |= RHS;


if (!getDefaultNullability())
if (auto Nullability = RHS.getDefaultNullability())
setDefaultNullability(*Nullability);

if (!SwiftImportAsNonGenericSpecified)
setSwiftImportAsNonGeneric(RHS.getSwiftImportAsNonGeneric());

if (!SwiftObjCMembersSpecified)
setSwiftObjCMembers(RHS.getSwiftObjCMembers());

HasDesignatedInits |= RHS.HasDesignatedInits;

return *this;
}

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS);
};

inline bool operator==(const ObjCContextInfo &LHS, const ObjCContextInfo &RHS) {
return static_cast<const CommonTypeInfo &>(LHS) == RHS &&
LHS.getDefaultNullability() == RHS.getDefaultNullability() &&
LHS.HasDesignatedInits == RHS.HasDesignatedInits &&
LHS.getSwiftImportAsNonGeneric() == RHS.getSwiftImportAsNonGeneric() &&
LHS.getSwiftObjCMembers() == RHS.getSwiftObjCMembers();
}

inline bool operator!=(const ObjCContextInfo &LHS, const ObjCContextInfo &RHS) {
return !(LHS == RHS);
}


class VariableInfo : public CommonEntityInfo {

unsigned NullabilityAudited : 1;



unsigned Nullable : 2;


std::string Type;

public:
VariableInfo() : CommonEntityInfo(), NullabilityAudited(false), Nullable(0) {}

llvm::Optional<NullabilityKind> getNullability() const {
return NullabilityAudited ? llvm::Optional<NullabilityKind>(
static_cast<NullabilityKind>(Nullable))
: llvm::None;
}

void setNullabilityAudited(NullabilityKind kind) {
NullabilityAudited = true;
Nullable = static_cast<unsigned>(kind);
}

const std::string &getType() const { return Type; }
void setType(const std::string &type) { Type = type; }

friend bool operator==(const VariableInfo &, const VariableInfo &);

VariableInfo &operator|=(const VariableInfo &RHS) {
static_cast<CommonEntityInfo &>(*this) |= RHS;

if (!NullabilityAudited && RHS.NullabilityAudited)
setNullabilityAudited(*RHS.getNullability());
if (Type.empty())
Type = RHS.Type;

return *this;
}

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS) const;
};

inline bool operator==(const VariableInfo &LHS, const VariableInfo &RHS) {
return static_cast<const CommonEntityInfo &>(LHS) == RHS &&
LHS.NullabilityAudited == RHS.NullabilityAudited &&
LHS.Nullable == RHS.Nullable && LHS.Type == RHS.Type;
}

inline bool operator!=(const VariableInfo &LHS, const VariableInfo &RHS) {
return !(LHS == RHS);
}


class ObjCPropertyInfo : public VariableInfo {
unsigned SwiftImportAsAccessorsSpecified : 1;
unsigned SwiftImportAsAccessors : 1;

public:
ObjCPropertyInfo()
: VariableInfo(), SwiftImportAsAccessorsSpecified(false),
SwiftImportAsAccessors(false) {}

llvm::Optional<bool> getSwiftImportAsAccessors() const {
return SwiftImportAsAccessorsSpecified
? llvm::Optional<bool>(SwiftImportAsAccessors)
: llvm::None;
}
void setSwiftImportAsAccessors(llvm::Optional<bool> Value) {
SwiftImportAsAccessorsSpecified = Value.hasValue();
SwiftImportAsAccessors = Value.hasValue() ? *Value : false;
}

friend bool operator==(const ObjCPropertyInfo &, const ObjCPropertyInfo &);


ObjCPropertyInfo &operator|=(const ObjCContextInfo &RHS) {
static_cast<CommonEntityInfo &>(*this) |= RHS;


if (!getNullability())
if (auto Nullable = RHS.getDefaultNullability())
setNullabilityAudited(*Nullable);

return *this;
}

ObjCPropertyInfo &operator|=(const ObjCPropertyInfo &RHS) {
static_cast<VariableInfo &>(*this) |= RHS;

if (!SwiftImportAsAccessorsSpecified)
setSwiftImportAsAccessors(RHS.getSwiftImportAsAccessors());

return *this;
}

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS) const;
};

inline bool operator==(const ObjCPropertyInfo &LHS,
const ObjCPropertyInfo &RHS) {
return static_cast<const VariableInfo &>(LHS) == RHS &&
LHS.getSwiftImportAsAccessors() == RHS.getSwiftImportAsAccessors();
}

inline bool operator!=(const ObjCPropertyInfo &LHS,
const ObjCPropertyInfo &RHS) {
return !(LHS == RHS);
}


class ParamInfo : public VariableInfo {

unsigned NoEscapeSpecified : 1;


unsigned NoEscape : 1;




unsigned RawRetainCountConvention : 3;

public:
ParamInfo()
: VariableInfo(), NoEscapeSpecified(false), NoEscape(false),
RawRetainCountConvention() {}

llvm::Optional<bool> isNoEscape() const {
if (!NoEscapeSpecified)
return llvm::None;
return NoEscape;
}
void setNoEscape(llvm::Optional<bool> Value) {
NoEscapeSpecified = Value.hasValue();
NoEscape = Value.hasValue() ? *Value : false;
}

llvm::Optional<RetainCountConventionKind> getRetainCountConvention() const {
if (!RawRetainCountConvention)
return llvm::None;
return static_cast<RetainCountConventionKind>(RawRetainCountConvention - 1);
}
void
setRetainCountConvention(llvm::Optional<RetainCountConventionKind> Value) {
RawRetainCountConvention =
Value.hasValue() ? static_cast<unsigned>(Value.getValue()) + 1 : 0;
assert(getRetainCountConvention() == Value && "bitfield too small");
}

ParamInfo &operator|=(const ParamInfo &RHS) {
static_cast<VariableInfo &>(*this) |= RHS;

if (!NoEscapeSpecified && RHS.NoEscapeSpecified) {
NoEscapeSpecified = true;
NoEscape = RHS.NoEscape;
}

if (!RawRetainCountConvention)
RawRetainCountConvention = RHS.RawRetainCountConvention;

return *this;
}

friend bool operator==(const ParamInfo &, const ParamInfo &);

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS) const;
};

inline bool operator==(const ParamInfo &LHS, const ParamInfo &RHS) {
return static_cast<const VariableInfo &>(LHS) == RHS &&
LHS.NoEscapeSpecified == RHS.NoEscapeSpecified &&
LHS.NoEscape == RHS.NoEscape &&
LHS.RawRetainCountConvention == RHS.RawRetainCountConvention;
}

inline bool operator!=(const ParamInfo &LHS, const ParamInfo &RHS) {
return !(LHS == RHS);
}


class FunctionInfo : public CommonEntityInfo {
private:
static constexpr const unsigned NullabilityKindMask = 0x3;
static constexpr const unsigned NullabilityKindSize = 2;

static constexpr const unsigned ReturnInfoIndex = 0;

public:





unsigned NullabilityAudited : 1;


unsigned NumAdjustedNullable : 8;


unsigned RawRetainCountConvention : 3;






uint64_t NullabilityPayload = 0;


std::string ResultType;


std::vector<ParamInfo> Params;

FunctionInfo()
: CommonEntityInfo(), NullabilityAudited(false), NumAdjustedNullable(0),
RawRetainCountConvention() {}

static unsigned getMaxNullabilityIndex() {
return ((sizeof(NullabilityPayload) * CHAR_BIT) / NullabilityKindSize);
}

void addTypeInfo(unsigned index, NullabilityKind kind) {
assert(index <= getMaxNullabilityIndex());
assert(static_cast<unsigned>(kind) < NullabilityKindMask);

NullabilityAudited = true;
if (NumAdjustedNullable < index + 1)
NumAdjustedNullable = index + 1;


NullabilityPayload &=
~(NullabilityKindMask << (index * NullabilityKindSize));


unsigned kindValue = (static_cast<unsigned>(kind))
<< (index * NullabilityKindSize);
NullabilityPayload |= kindValue;
}


void addReturnTypeInfo(NullabilityKind kind) {
addTypeInfo(ReturnInfoIndex, kind);
}


void addParamTypeInfo(unsigned index, NullabilityKind kind) {
addTypeInfo(index + 1, kind);
}

NullabilityKind getParamTypeInfo(unsigned index) const {
return getTypeInfo(index + 1);
}

NullabilityKind getReturnTypeInfo() const { return getTypeInfo(0); }

llvm::Optional<RetainCountConventionKind> getRetainCountConvention() const {
if (!RawRetainCountConvention)
return llvm::None;
return static_cast<RetainCountConventionKind>(RawRetainCountConvention - 1);
}
void
setRetainCountConvention(llvm::Optional<RetainCountConventionKind> Value) {
RawRetainCountConvention =
Value.hasValue() ? static_cast<unsigned>(Value.getValue()) + 1 : 0;
assert(getRetainCountConvention() == Value && "bitfield too small");
}

friend bool operator==(const FunctionInfo &, const FunctionInfo &);

private:
NullabilityKind getTypeInfo(unsigned index) const {
assert(NullabilityAudited &&
"Checking the type adjustment on non-audited method.");


if (index > NumAdjustedNullable)
return NullabilityKind::NonNull;
auto nullability = NullabilityPayload >> (index * NullabilityKindSize);
return static_cast<NullabilityKind>(nullability & NullabilityKindMask);
}

public:
LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS) const;
};

inline bool operator==(const FunctionInfo &LHS, const FunctionInfo &RHS) {
return static_cast<const CommonEntityInfo &>(LHS) == RHS &&
LHS.NullabilityAudited == RHS.NullabilityAudited &&
LHS.NumAdjustedNullable == RHS.NumAdjustedNullable &&
LHS.NullabilityPayload == RHS.NullabilityPayload &&
LHS.ResultType == RHS.ResultType && LHS.Params == RHS.Params &&
LHS.RawRetainCountConvention == RHS.RawRetainCountConvention;
}

inline bool operator!=(const FunctionInfo &LHS, const FunctionInfo &RHS) {
return !(LHS == RHS);
}


class ObjCMethodInfo : public FunctionInfo {
public:

unsigned DesignatedInit : 1;


unsigned RequiredInit : 1;

ObjCMethodInfo()
: FunctionInfo(), DesignatedInit(false), RequiredInit(false) {}

friend bool operator==(const ObjCMethodInfo &, const ObjCMethodInfo &);

ObjCMethodInfo &operator|=(const ObjCContextInfo &RHS) {

if (!NullabilityAudited) {
if (auto Nullable = RHS.getDefaultNullability()) {
NullabilityAudited = true;
addTypeInfo(0, *Nullable);
}
}
return *this;
}

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS);
};

inline bool operator==(const ObjCMethodInfo &LHS, const ObjCMethodInfo &RHS) {
return static_cast<const FunctionInfo &>(LHS) == RHS &&
LHS.DesignatedInit == RHS.DesignatedInit &&
LHS.RequiredInit == RHS.RequiredInit;
}

inline bool operator!=(const ObjCMethodInfo &LHS, const ObjCMethodInfo &RHS) {
return !(LHS == RHS);
}


class GlobalVariableInfo : public VariableInfo {
public:
GlobalVariableInfo() : VariableInfo() {}
};


class GlobalFunctionInfo : public FunctionInfo {
public:
GlobalFunctionInfo() : FunctionInfo() {}
};


class EnumConstantInfo : public CommonEntityInfo {
public:
EnumConstantInfo() : CommonEntityInfo() {}
};


class TagInfo : public CommonTypeInfo {
unsigned HasFlagEnum : 1;
unsigned IsFlagEnum : 1;

public:
llvm::Optional<EnumExtensibilityKind> EnumExtensibility;

TagInfo() : CommonTypeInfo(), HasFlagEnum(0), IsFlagEnum(0) {}

llvm::Optional<bool> isFlagEnum() const {
if (HasFlagEnum)
return IsFlagEnum;
return llvm::None;
}
void setFlagEnum(llvm::Optional<bool> Value) {
HasFlagEnum = Value.hasValue();
IsFlagEnum = Value.hasValue() ? *Value : false;
}

TagInfo &operator|=(const TagInfo &RHS) {
static_cast<CommonTypeInfo &>(*this) |= RHS;

if (!HasFlagEnum && HasFlagEnum)
setFlagEnum(RHS.isFlagEnum());

if (!EnumExtensibility.hasValue())
EnumExtensibility = RHS.EnumExtensibility;

return *this;
}

friend bool operator==(const TagInfo &, const TagInfo &);

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS);
};

inline bool operator==(const TagInfo &LHS, const TagInfo &RHS) {
return static_cast<const CommonTypeInfo &>(LHS) == RHS &&
LHS.isFlagEnum() == RHS.isFlagEnum() &&
LHS.EnumExtensibility == RHS.EnumExtensibility;
}

inline bool operator!=(const TagInfo &LHS, const TagInfo &RHS) {
return !(LHS == RHS);
}


class TypedefInfo : public CommonTypeInfo {
public:
llvm::Optional<SwiftNewTypeKind> SwiftWrapper;

TypedefInfo() : CommonTypeInfo() {}

TypedefInfo &operator|=(const TypedefInfo &RHS) {
static_cast<CommonTypeInfo &>(*this) |= RHS;
if (!SwiftWrapper.hasValue())
SwiftWrapper = RHS.SwiftWrapper;
return *this;
}

friend bool operator==(const TypedefInfo &, const TypedefInfo &);

LLVM_DUMP_METHOD void dump(llvm::raw_ostream &OS) const;
};

inline bool operator==(const TypedefInfo &LHS, const TypedefInfo &RHS) {
return static_cast<const CommonTypeInfo &>(LHS) == RHS &&
LHS.SwiftWrapper == RHS.SwiftWrapper;
}

inline bool operator!=(const TypedefInfo &LHS, const TypedefInfo &RHS) {
return !(LHS == RHS);
}
}
}

#endif
