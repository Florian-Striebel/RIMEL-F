












#if !defined(LLVM_CLANG_AST_TYPELOC_H)
#define LLVM_CLANG_AST_TYPELOC_H

#include "clang/AST/DeclarationName.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/MathExtras.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>

namespace clang {

class Attr;
class ASTContext;
class CXXRecordDecl;
class ConceptDecl;
class Expr;
class ObjCInterfaceDecl;
class ObjCProtocolDecl;
class ObjCTypeParamDecl;
class ParmVarDecl;
class TemplateTypeParmDecl;
class UnqualTypeLoc;
class UnresolvedUsingTypenameDecl;


#define ABSTRACT_TYPELOC(Class, Base)
#define TYPELOC(Class, Base) class Class##TypeLoc;

#include "clang/AST/TypeLocNodes.def"





class TypeLoc {
protected:


const void *Ty = nullptr;
void *Data = nullptr;

public:
TypeLoc() = default;
TypeLoc(QualType ty, void *opaqueData)
: Ty(ty.getAsOpaquePtr()), Data(opaqueData) {}
TypeLoc(const Type *ty, void *opaqueData)
: Ty(ty), Data(opaqueData) {}





template<typename T>
T castAs() const {
assert(T::isKind(*this));
T t;
TypeLoc& tl = t;
tl = *this;
return t;
}



template<typename T>
T getAs() const {
if (!T::isKind(*this))
return {};
T t;
TypeLoc& tl = t;
tl = *this;
return t;
}





template <typename T>
T getAsAdjusted() const;




enum TypeLocClass {
#define ABSTRACT_TYPE(Class, Base)
#define TYPE(Class, Base) Class = Type::Class,

#include "clang/AST/TypeNodes.inc"
Qualified
};

TypeLocClass getTypeLocClass() const {
if (getType().hasLocalQualifiers()) return Qualified;
return (TypeLocClass) getType()->getTypeClass();
}

bool isNull() const { return !Ty; }
explicit operator bool() const { return Ty; }


static unsigned getFullDataSizeForType(QualType Ty);



static unsigned getLocalAlignmentForType(QualType Ty);



QualType getType() const {
return QualType::getFromOpaquePtr(Ty);
}

const Type *getTypePtr() const {
return QualType::getFromOpaquePtr(Ty).getTypePtr();
}


void *getOpaqueData() const {
return Data;
}


SourceLocation getBeginLoc() const;


SourceLocation getEndLoc() const;


SourceRange getSourceRange() const LLVM_READONLY {
return SourceRange(getBeginLoc(), getEndLoc());
}



SourceRange getLocalSourceRange() const {
return getLocalSourceRangeImpl(*this);
}


unsigned getFullDataSize() const {
return getFullDataSizeForType(getType());
}



TypeLoc getNextTypeLoc() const {
return getNextTypeLocImpl(*this);
}


UnqualTypeLoc getUnqualifiedLoc() const;

TypeLoc IgnoreParens() const;







TypeLoc findExplicitQualifierLoc() const;




AutoTypeLoc getContainedAutoTypeLoc() const;






void initialize(ASTContext &Context, SourceLocation Loc) const {
initializeImpl(Context, *this, Loc);
}



void initializeFullCopy(TypeLoc Other) {
assert(getType() == Other.getType());
copy(Other);
}




void initializeFullCopy(TypeLoc Other, unsigned Size) {
assert(getType() == Other.getType());
assert(getFullDataSize() == Size);
copy(Other);
}


void copy(TypeLoc other);

friend bool operator==(const TypeLoc &LHS, const TypeLoc &RHS) {
return LHS.Ty == RHS.Ty && LHS.Data == RHS.Data;
}

friend bool operator!=(const TypeLoc &LHS, const TypeLoc &RHS) {
return !(LHS == RHS);
}



SourceLocation findNullabilityLoc() const;

private:
static bool isKind(const TypeLoc&) {
return true;
}

static void initializeImpl(ASTContext &Context, TypeLoc TL,
SourceLocation Loc);
static TypeLoc getNextTypeLocImpl(TypeLoc TL);
static TypeLoc IgnoreParensImpl(TypeLoc TL);
static SourceRange getLocalSourceRangeImpl(TypeLoc TL);
};


inline TypeLoc TypeSourceInfo::getTypeLoc() const {

return TypeLoc(Ty, const_cast<void*>(static_cast<const void*>(this + 1)));
}



class UnqualTypeLoc : public TypeLoc {
public:
UnqualTypeLoc() = default;
UnqualTypeLoc(const Type *Ty, void *Data) : TypeLoc(Ty, Data) {}

const Type *getTypePtr() const {
return reinterpret_cast<const Type*>(Ty);
}

TypeLocClass getTypeLocClass() const {
return (TypeLocClass) getTypePtr()->getTypeClass();
}

private:
friend class TypeLoc;

static bool isKind(const TypeLoc &TL) {
return !TL.getType().hasLocalQualifiers();
}
};






class QualifiedTypeLoc : public TypeLoc {
public:
SourceRange getLocalSourceRange() const { return {}; }

UnqualTypeLoc getUnqualifiedLoc() const {
unsigned align =
TypeLoc::getLocalAlignmentForType(QualType(getTypePtr(), 0));
auto dataInt = reinterpret_cast<uintptr_t>(Data);
dataInt = llvm::alignTo(dataInt, align);
return UnqualTypeLoc(getTypePtr(), reinterpret_cast<void*>(dataInt));
}



void initializeLocal(ASTContext &Context, SourceLocation Loc) {

}

void copyLocal(TypeLoc other) {

}

TypeLoc getNextTypeLoc() const {
return getUnqualifiedLoc();
}



unsigned getLocalDataSize() const {


return 0;
}



unsigned getLocalDataAlignment() const {

return 1;
}

private:
friend class TypeLoc;

static bool isKind(const TypeLoc &TL) {
return TL.getType().hasLocalQualifiers();
}
};

inline UnqualTypeLoc TypeLoc::getUnqualifiedLoc() const {
if (QualifiedTypeLoc Loc = getAs<QualifiedTypeLoc>())
return Loc.getUnqualifiedLoc();
return castAs<UnqualTypeLoc>();
}





























template <class Base, class Derived, class TypeClass, class LocalData>
class ConcreteTypeLoc : public Base {
friend class TypeLoc;

const Derived *asDerived() const {
return static_cast<const Derived*>(this);
}

static bool isKind(const TypeLoc &TL) {
return !TL.getType().hasLocalQualifiers() &&
Derived::classofType(TL.getTypePtr());
}

static bool classofType(const Type *Ty) {
return TypeClass::classof(Ty);
}

public:
unsigned getLocalDataAlignment() const {
return std::max(unsigned(alignof(LocalData)),
asDerived()->getExtraLocalDataAlignment());
}

unsigned getLocalDataSize() const {
unsigned size = sizeof(LocalData);
unsigned extraAlign = asDerived()->getExtraLocalDataAlignment();
size = llvm::alignTo(size, extraAlign);
size += asDerived()->getExtraLocalDataSize();
return size;
}

void copyLocal(Derived other) {

if (asDerived()->getLocalDataSize() == 0) return;


memcpy(getLocalData(), other.getLocalData(), sizeof(LocalData));




memcpy(getExtraLocalData(), other.getExtraLocalData(),
asDerived()->getExtraLocalDataSize());
}

TypeLoc getNextTypeLoc() const {
return getNextTypeLoc(asDerived()->getInnerType());
}

const TypeClass *getTypePtr() const {
return cast<TypeClass>(Base::getTypePtr());
}

protected:
unsigned getExtraLocalDataSize() const {
return 0;
}

unsigned getExtraLocalDataAlignment() const {
return 1;
}

LocalData *getLocalData() const {
return static_cast<LocalData*>(Base::Data);
}




void *getExtraLocalData() const {
unsigned size = sizeof(LocalData);
unsigned extraAlign = asDerived()->getExtraLocalDataAlignment();
size = llvm::alignTo(size, extraAlign);
return reinterpret_cast<char*>(Base::Data) + size;
}

void *getNonLocalData() const {
auto data = reinterpret_cast<uintptr_t>(Base::Data);
data += asDerived()->getLocalDataSize();
data = llvm::alignTo(data, getNextTypeAlign());
return reinterpret_cast<void*>(data);
}

struct HasNoInnerType {};
HasNoInnerType getInnerType() const { return HasNoInnerType(); }

TypeLoc getInnerTypeLoc() const {
return TypeLoc(asDerived()->getInnerType(), getNonLocalData());
}

private:
unsigned getInnerTypeSize() const {
return getInnerTypeSize(asDerived()->getInnerType());
}

unsigned getInnerTypeSize(HasNoInnerType _) const {
return 0;
}

unsigned getInnerTypeSize(QualType _) const {
return getInnerTypeLoc().getFullDataSize();
}

unsigned getNextTypeAlign() const {
return getNextTypeAlign(asDerived()->getInnerType());
}

unsigned getNextTypeAlign(HasNoInnerType _) const {
return 1;
}

unsigned getNextTypeAlign(QualType T) const {
return TypeLoc::getLocalAlignmentForType(T);
}

TypeLoc getNextTypeLoc(HasNoInnerType _) const { return {}; }

TypeLoc getNextTypeLoc(QualType T) const {
return TypeLoc(T, getNonLocalData());
}
};




template <class Base, class Derived, class TypeClass>
class InheritingConcreteTypeLoc : public Base {
friend class TypeLoc;

static bool classofType(const Type *Ty) {
return TypeClass::classof(Ty);
}

static bool isKind(const TypeLoc &TL) {
return !TL.getType().hasLocalQualifiers() &&
Derived::classofType(TL.getTypePtr());
}
static bool isKind(const UnqualTypeLoc &TL) {
return Derived::classofType(TL.getTypePtr());
}

public:
const TypeClass *getTypePtr() const {
return cast<TypeClass>(Base::getTypePtr());
}
};

struct TypeSpecLocInfo {
SourceLocation NameLoc;
};



class TypeSpecTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
TypeSpecTypeLoc,
Type,
TypeSpecLocInfo> {
public:
enum {
LocalDataSize = sizeof(TypeSpecLocInfo),
LocalDataAlignment = alignof(TypeSpecLocInfo)
};

SourceLocation getNameLoc() const {
return this->getLocalData()->NameLoc;
}

void setNameLoc(SourceLocation Loc) {
this->getLocalData()->NameLoc = Loc;
}

SourceRange getLocalSourceRange() const {
return SourceRange(getNameLoc(), getNameLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setNameLoc(Loc);
}

private:
friend class TypeLoc;

static bool isKind(const TypeLoc &TL);
};

struct BuiltinLocInfo {
SourceRange BuiltinRange;
};


class BuiltinTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
BuiltinTypeLoc,
BuiltinType,
BuiltinLocInfo> {
public:
SourceLocation getBuiltinLoc() const {
return getLocalData()->BuiltinRange.getBegin();
}

void setBuiltinLoc(SourceLocation Loc) {
getLocalData()->BuiltinRange = Loc;
}

void expandBuiltinRange(SourceRange Range) {
SourceRange &BuiltinRange = getLocalData()->BuiltinRange;
if (!BuiltinRange.getBegin().isValid()) {
BuiltinRange = Range;
} else {
BuiltinRange.setBegin(std::min(Range.getBegin(), BuiltinRange.getBegin()));
BuiltinRange.setEnd(std::max(Range.getEnd(), BuiltinRange.getEnd()));
}
}

SourceLocation getNameLoc() const { return getBuiltinLoc(); }

WrittenBuiltinSpecs& getWrittenBuiltinSpecs() {
return *(static_cast<WrittenBuiltinSpecs*>(getExtraLocalData()));
}
const WrittenBuiltinSpecs& getWrittenBuiltinSpecs() const {
return *(static_cast<WrittenBuiltinSpecs*>(getExtraLocalData()));
}

bool needsExtraLocalData() const {
BuiltinType::Kind bk = getTypePtr()->getKind();
return (bk >= BuiltinType::UShort && bk <= BuiltinType::UInt128)
|| (bk >= BuiltinType::Short && bk <= BuiltinType::Float128)
|| bk == BuiltinType::UChar
|| bk == BuiltinType::SChar;
}

unsigned getExtraLocalDataSize() const {
return needsExtraLocalData() ? sizeof(WrittenBuiltinSpecs) : 0;
}

unsigned getExtraLocalDataAlignment() const {
return needsExtraLocalData() ? alignof(WrittenBuiltinSpecs) : 1;
}

SourceRange getLocalSourceRange() const {
return getLocalData()->BuiltinRange;
}

TypeSpecifierSign getWrittenSignSpec() const {
if (needsExtraLocalData())
return static_cast<TypeSpecifierSign>(getWrittenBuiltinSpecs().Sign);
else
return TypeSpecifierSign::Unspecified;
}

bool hasWrittenSignSpec() const {
return getWrittenSignSpec() != TypeSpecifierSign::Unspecified;
}

void setWrittenSignSpec(TypeSpecifierSign written) {
if (needsExtraLocalData())
getWrittenBuiltinSpecs().Sign = static_cast<unsigned>(written);
}

TypeSpecifierWidth getWrittenWidthSpec() const {
if (needsExtraLocalData())
return static_cast<TypeSpecifierWidth>(getWrittenBuiltinSpecs().Width);
else
return TypeSpecifierWidth::Unspecified;
}

bool hasWrittenWidthSpec() const {
return getWrittenWidthSpec() != TypeSpecifierWidth::Unspecified;
}

void setWrittenWidthSpec(TypeSpecifierWidth written) {
if (needsExtraLocalData())
getWrittenBuiltinSpecs().Width = static_cast<unsigned>(written);
}

TypeSpecifierType getWrittenTypeSpec() const;

bool hasWrittenTypeSpec() const {
return getWrittenTypeSpec() != TST_unspecified;
}

void setWrittenTypeSpec(TypeSpecifierType written) {
if (needsExtraLocalData())
getWrittenBuiltinSpecs().Type = written;
}

bool hasModeAttr() const {
if (needsExtraLocalData())
return getWrittenBuiltinSpecs().ModeAttr;
else
return false;
}

void setModeAttr(bool written) {
if (needsExtraLocalData())
getWrittenBuiltinSpecs().ModeAttr = written;
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setBuiltinLoc(Loc);
if (needsExtraLocalData()) {
WrittenBuiltinSpecs &wbs = getWrittenBuiltinSpecs();
wbs.Sign = static_cast<unsigned>(TypeSpecifierSign::Unspecified);
wbs.Width = static_cast<unsigned>(TypeSpecifierWidth::Unspecified);
wbs.Type = TST_unspecified;
wbs.ModeAttr = false;
}
}
};


class TypedefTypeLoc : public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
TypedefTypeLoc,
TypedefType> {
public:
TypedefNameDecl *getTypedefNameDecl() const {
return getTypePtr()->getDecl();
}
};



class InjectedClassNameTypeLoc :
public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
InjectedClassNameTypeLoc,
InjectedClassNameType> {
public:
CXXRecordDecl *getDecl() const {
return getTypePtr()->getDecl();
}
};


class UnresolvedUsingTypeLoc :
public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
UnresolvedUsingTypeLoc,
UnresolvedUsingType> {
public:
UnresolvedUsingTypenameDecl *getDecl() const {
return getTypePtr()->getDecl();
}
};






class TagTypeLoc : public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
TagTypeLoc,
TagType> {
public:
TagDecl *getDecl() const { return getTypePtr()->getDecl(); }


bool isDefinition() const;
};


class RecordTypeLoc : public InheritingConcreteTypeLoc<TagTypeLoc,
RecordTypeLoc,
RecordType> {
public:
RecordDecl *getDecl() const { return getTypePtr()->getDecl(); }
};


class EnumTypeLoc : public InheritingConcreteTypeLoc<TagTypeLoc,
EnumTypeLoc,
EnumType> {
public:
EnumDecl *getDecl() const { return getTypePtr()->getDecl(); }
};


class TemplateTypeParmTypeLoc :
public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
TemplateTypeParmTypeLoc,
TemplateTypeParmType> {
public:
TemplateTypeParmDecl *getDecl() const { return getTypePtr()->getDecl(); }
};

struct ObjCTypeParamTypeLocInfo {
SourceLocation NameLoc;
};



class ObjCTypeParamTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
ObjCTypeParamTypeLoc,
ObjCTypeParamType,
ObjCTypeParamTypeLocInfo> {

SourceLocation *getProtocolLocArray() const {
return (SourceLocation*)this->getExtraLocalData() + 2;
}

public:
ObjCTypeParamDecl *getDecl() const { return getTypePtr()->getDecl(); }

SourceLocation getNameLoc() const {
return this->getLocalData()->NameLoc;
}

void setNameLoc(SourceLocation Loc) {
this->getLocalData()->NameLoc = Loc;
}

SourceLocation getProtocolLAngleLoc() const {
return getNumProtocols() ?
*((SourceLocation*)this->getExtraLocalData()) :
SourceLocation();
}

void setProtocolLAngleLoc(SourceLocation Loc) {
*((SourceLocation*)this->getExtraLocalData()) = Loc;
}

SourceLocation getProtocolRAngleLoc() const {
return getNumProtocols() ?
*((SourceLocation*)this->getExtraLocalData() + 1) :
SourceLocation();
}

void setProtocolRAngleLoc(SourceLocation Loc) {
*((SourceLocation*)this->getExtraLocalData() + 1) = Loc;
}

unsigned getNumProtocols() const {
return this->getTypePtr()->getNumProtocols();
}

SourceLocation getProtocolLoc(unsigned i) const {
assert(i < getNumProtocols() && "Index is out of bounds!");
return getProtocolLocArray()[i];
}

void setProtocolLoc(unsigned i, SourceLocation Loc) {
assert(i < getNumProtocols() && "Index is out of bounds!");
getProtocolLocArray()[i] = Loc;
}

ObjCProtocolDecl *getProtocol(unsigned i) const {
assert(i < getNumProtocols() && "Index is out of bounds!");
return *(this->getTypePtr()->qual_begin() + i);
}

ArrayRef<SourceLocation> getProtocolLocs() const {
return llvm::makeArrayRef(getProtocolLocArray(), getNumProtocols());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);

unsigned getExtraLocalDataSize() const {
if (!this->getNumProtocols()) return 0;


return (this->getNumProtocols() + 2) * sizeof(SourceLocation) ;
}

unsigned getExtraLocalDataAlignment() const {
return alignof(SourceLocation);
}

SourceRange getLocalSourceRange() const {
SourceLocation start = getNameLoc();
SourceLocation end = getProtocolRAngleLoc();
if (end.isInvalid()) return SourceRange(start, start);
return SourceRange(start, end);
}
};


class SubstTemplateTypeParmTypeLoc :
public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
SubstTemplateTypeParmTypeLoc,
SubstTemplateTypeParmType> {
};


class SubstTemplateTypeParmPackTypeLoc :
public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
SubstTemplateTypeParmPackTypeLoc,
SubstTemplateTypeParmPackType> {
};

struct AttributedLocInfo {
const Attr *TypeAttr;
};


class AttributedTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
AttributedTypeLoc,
AttributedType,
AttributedLocInfo> {
public:
attr::Kind getAttrKind() const {
return getTypePtr()->getAttrKind();
}

bool isQualifier() const {
return getTypePtr()->isQualifier();
}





TypeLoc getModifiedLoc() const {
return getInnerTypeLoc();
}


const Attr *getAttr() const {
return getLocalData()->TypeAttr;
}
void setAttr(const Attr *A) {
getLocalData()->TypeAttr = A;
}

template<typename T> const T *getAttrAs() {
return dyn_cast_or_null<T>(getAttr());
}

SourceRange getLocalSourceRange() const;

void initializeLocal(ASTContext &Context, SourceLocation loc) {
setAttr(nullptr);
}

QualType getInnerType() const {
return getTypePtr()->getModifiedType();
}
};

struct ObjCObjectTypeLocInfo {
SourceLocation TypeArgsLAngleLoc;
SourceLocation TypeArgsRAngleLoc;
SourceLocation ProtocolLAngleLoc;
SourceLocation ProtocolRAngleLoc;
bool HasBaseTypeAsWritten;
};






class ObjCObjectTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
ObjCObjectTypeLoc,
ObjCObjectType,
ObjCObjectTypeLocInfo> {

TypeSourceInfo **getTypeArgLocArray() const {
return (TypeSourceInfo**)this->getExtraLocalData();
}



SourceLocation *getProtocolLocArray() const {
return (SourceLocation*)(getTypeArgLocArray() + getNumTypeArgs());
}

public:
SourceLocation getTypeArgsLAngleLoc() const {
return this->getLocalData()->TypeArgsLAngleLoc;
}

void setTypeArgsLAngleLoc(SourceLocation Loc) {
this->getLocalData()->TypeArgsLAngleLoc = Loc;
}

SourceLocation getTypeArgsRAngleLoc() const {
return this->getLocalData()->TypeArgsRAngleLoc;
}

void setTypeArgsRAngleLoc(SourceLocation Loc) {
this->getLocalData()->TypeArgsRAngleLoc = Loc;
}

unsigned getNumTypeArgs() const {
return this->getTypePtr()->getTypeArgsAsWritten().size();
}

TypeSourceInfo *getTypeArgTInfo(unsigned i) const {
assert(i < getNumTypeArgs() && "Index is out of bounds!");
return getTypeArgLocArray()[i];
}

void setTypeArgTInfo(unsigned i, TypeSourceInfo *TInfo) {
assert(i < getNumTypeArgs() && "Index is out of bounds!");
getTypeArgLocArray()[i] = TInfo;
}

SourceLocation getProtocolLAngleLoc() const {
return this->getLocalData()->ProtocolLAngleLoc;
}

void setProtocolLAngleLoc(SourceLocation Loc) {
this->getLocalData()->ProtocolLAngleLoc = Loc;
}

SourceLocation getProtocolRAngleLoc() const {
return this->getLocalData()->ProtocolRAngleLoc;
}

void setProtocolRAngleLoc(SourceLocation Loc) {
this->getLocalData()->ProtocolRAngleLoc = Loc;
}

unsigned getNumProtocols() const {
return this->getTypePtr()->getNumProtocols();
}

SourceLocation getProtocolLoc(unsigned i) const {
assert(i < getNumProtocols() && "Index is out of bounds!");
return getProtocolLocArray()[i];
}

void setProtocolLoc(unsigned i, SourceLocation Loc) {
assert(i < getNumProtocols() && "Index is out of bounds!");
getProtocolLocArray()[i] = Loc;
}

ObjCProtocolDecl *getProtocol(unsigned i) const {
assert(i < getNumProtocols() && "Index is out of bounds!");
return *(this->getTypePtr()->qual_begin() + i);
}


ArrayRef<SourceLocation> getProtocolLocs() const {
return llvm::makeArrayRef(getProtocolLocArray(), getNumProtocols());
}

bool hasBaseTypeAsWritten() const {
return getLocalData()->HasBaseTypeAsWritten;
}

void setHasBaseTypeAsWritten(bool HasBaseType) {
getLocalData()->HasBaseTypeAsWritten = HasBaseType;
}

TypeLoc getBaseLoc() const {
return getInnerTypeLoc();
}

SourceRange getLocalSourceRange() const {
SourceLocation start = getTypeArgsLAngleLoc();
if (start.isInvalid())
start = getProtocolLAngleLoc();
SourceLocation end = getProtocolRAngleLoc();
if (end.isInvalid())
end = getTypeArgsRAngleLoc();
return SourceRange(start, end);
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);

unsigned getExtraLocalDataSize() const {
return this->getNumTypeArgs() * sizeof(TypeSourceInfo *)
+ this->getNumProtocols() * sizeof(SourceLocation);
}

unsigned getExtraLocalDataAlignment() const {
static_assert(alignof(ObjCObjectTypeLoc) >= alignof(TypeSourceInfo *),
"not enough alignment for tail-allocated data");
return alignof(TypeSourceInfo *);
}

QualType getInnerType() const {
return getTypePtr()->getBaseType();
}
};

struct ObjCInterfaceLocInfo {
SourceLocation NameLoc;
SourceLocation NameEndLoc;
};


class ObjCInterfaceTypeLoc : public ConcreteTypeLoc<ObjCObjectTypeLoc,
ObjCInterfaceTypeLoc,
ObjCInterfaceType,
ObjCInterfaceLocInfo> {
public:
ObjCInterfaceDecl *getIFaceDecl() const {
return getTypePtr()->getDecl();
}

SourceLocation getNameLoc() const {
return getLocalData()->NameLoc;
}

void setNameLoc(SourceLocation Loc) {
getLocalData()->NameLoc = Loc;
}

SourceRange getLocalSourceRange() const {
return SourceRange(getNameLoc(), getNameEndLoc());
}

SourceLocation getNameEndLoc() const {
return getLocalData()->NameEndLoc;
}

void setNameEndLoc(SourceLocation Loc) {
getLocalData()->NameEndLoc = Loc;
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setNameLoc(Loc);
setNameEndLoc(Loc);
}
};

struct MacroQualifiedLocInfo {
SourceLocation ExpansionLoc;
};

class MacroQualifiedTypeLoc
: public ConcreteTypeLoc<UnqualTypeLoc, MacroQualifiedTypeLoc,
MacroQualifiedType, MacroQualifiedLocInfo> {
public:
void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setExpansionLoc(Loc);
}

TypeLoc getInnerLoc() const { return getInnerTypeLoc(); }

const IdentifierInfo *getMacroIdentifier() const {
return getTypePtr()->getMacroIdentifier();
}

SourceLocation getExpansionLoc() const {
return this->getLocalData()->ExpansionLoc;
}

void setExpansionLoc(SourceLocation Loc) {
this->getLocalData()->ExpansionLoc = Loc;
}

QualType getInnerType() const { return getTypePtr()->getUnderlyingType(); }

SourceRange getLocalSourceRange() const {
return getInnerLoc().getLocalSourceRange();
}
};

struct ParenLocInfo {
SourceLocation LParenLoc;
SourceLocation RParenLoc;
};

class ParenTypeLoc
: public ConcreteTypeLoc<UnqualTypeLoc, ParenTypeLoc, ParenType,
ParenLocInfo> {
public:
SourceLocation getLParenLoc() const {
return this->getLocalData()->LParenLoc;
}

SourceLocation getRParenLoc() const {
return this->getLocalData()->RParenLoc;
}

void setLParenLoc(SourceLocation Loc) {
this->getLocalData()->LParenLoc = Loc;
}

void setRParenLoc(SourceLocation Loc) {
this->getLocalData()->RParenLoc = Loc;
}

SourceRange getLocalSourceRange() const {
return SourceRange(getLParenLoc(), getRParenLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setLParenLoc(Loc);
setRParenLoc(Loc);
}

TypeLoc getInnerLoc() const {
return getInnerTypeLoc();
}

QualType getInnerType() const {
return this->getTypePtr()->getInnerType();
}
};

inline TypeLoc TypeLoc::IgnoreParens() const {
if (ParenTypeLoc::isKind(*this))
return IgnoreParensImpl(*this);
return *this;
}

struct AdjustedLocInfo {};

class AdjustedTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc, AdjustedTypeLoc,
AdjustedType, AdjustedLocInfo> {
public:
TypeLoc getOriginalLoc() const {
return getInnerTypeLoc();
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {

}

QualType getInnerType() const {


return getTypePtr()->getOriginalType();
}

SourceRange getLocalSourceRange() const { return {}; }

unsigned getLocalDataSize() const {


return 0;
}
};



class DecayedTypeLoc : public InheritingConcreteTypeLoc<
AdjustedTypeLoc, DecayedTypeLoc, DecayedType> {
};

struct PointerLikeLocInfo {
SourceLocation StarLoc;
};


template <class Derived, class TypeClass, class LocalData = PointerLikeLocInfo>
class PointerLikeTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc, Derived,
TypeClass, LocalData> {
public:
SourceLocation getSigilLoc() const {
return this->getLocalData()->StarLoc;
}

void setSigilLoc(SourceLocation Loc) {
this->getLocalData()->StarLoc = Loc;
}

TypeLoc getPointeeLoc() const {
return this->getInnerTypeLoc();
}

SourceRange getLocalSourceRange() const {
return SourceRange(getSigilLoc(), getSigilLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setSigilLoc(Loc);
}

QualType getInnerType() const {
return this->getTypePtr()->getPointeeType();
}
};


class PointerTypeLoc : public PointerLikeTypeLoc<PointerTypeLoc,
PointerType> {
public:
SourceLocation getStarLoc() const {
return getSigilLoc();
}

void setStarLoc(SourceLocation Loc) {
setSigilLoc(Loc);
}
};


class BlockPointerTypeLoc : public PointerLikeTypeLoc<BlockPointerTypeLoc,
BlockPointerType> {
public:
SourceLocation getCaretLoc() const {
return getSigilLoc();
}

void setCaretLoc(SourceLocation Loc) {
setSigilLoc(Loc);
}
};

struct MemberPointerLocInfo : public PointerLikeLocInfo {
TypeSourceInfo *ClassTInfo;
};


class MemberPointerTypeLoc : public PointerLikeTypeLoc<MemberPointerTypeLoc,
MemberPointerType,
MemberPointerLocInfo> {
public:
SourceLocation getStarLoc() const {
return getSigilLoc();
}

void setStarLoc(SourceLocation Loc) {
setSigilLoc(Loc);
}

const Type *getClass() const {
return getTypePtr()->getClass();
}

TypeSourceInfo *getClassTInfo() const {
return getLocalData()->ClassTInfo;
}

void setClassTInfo(TypeSourceInfo* TI) {
getLocalData()->ClassTInfo = TI;
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setSigilLoc(Loc);
setClassTInfo(nullptr);
}

SourceRange getLocalSourceRange() const {
if (TypeSourceInfo *TI = getClassTInfo())
return SourceRange(TI->getTypeLoc().getBeginLoc(), getStarLoc());
else
return SourceRange(getStarLoc());
}
};


class ObjCObjectPointerTypeLoc :
public PointerLikeTypeLoc<ObjCObjectPointerTypeLoc,
ObjCObjectPointerType> {
public:
SourceLocation getStarLoc() const {
return getSigilLoc();
}

void setStarLoc(SourceLocation Loc) {
setSigilLoc(Loc);
}
};

class ReferenceTypeLoc : public PointerLikeTypeLoc<ReferenceTypeLoc,
ReferenceType> {
public:
QualType getInnerType() const {
return getTypePtr()->getPointeeTypeAsWritten();
}
};

class LValueReferenceTypeLoc :
public InheritingConcreteTypeLoc<ReferenceTypeLoc,
LValueReferenceTypeLoc,
LValueReferenceType> {
public:
SourceLocation getAmpLoc() const {
return getSigilLoc();
}

void setAmpLoc(SourceLocation Loc) {
setSigilLoc(Loc);
}
};

class RValueReferenceTypeLoc :
public InheritingConcreteTypeLoc<ReferenceTypeLoc,
RValueReferenceTypeLoc,
RValueReferenceType> {
public:
SourceLocation getAmpAmpLoc() const {
return getSigilLoc();
}

void setAmpAmpLoc(SourceLocation Loc) {
setSigilLoc(Loc);
}
};

struct FunctionLocInfo {
SourceLocation LocalRangeBegin;
SourceLocation LParenLoc;
SourceLocation RParenLoc;
SourceLocation LocalRangeEnd;
};


class FunctionTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
FunctionTypeLoc,
FunctionType,
FunctionLocInfo> {
bool hasExceptionSpec() const {
if (auto *FPT = dyn_cast<FunctionProtoType>(getTypePtr())) {
return FPT->hasExceptionSpec();
}
return false;
}

SourceRange *getExceptionSpecRangePtr() const {
assert(hasExceptionSpec() && "No exception spec range");


return (SourceRange *)(getParmArray() + getNumParams());
}

public:
SourceLocation getLocalRangeBegin() const {
return getLocalData()->LocalRangeBegin;
}

void setLocalRangeBegin(SourceLocation L) {
getLocalData()->LocalRangeBegin = L;
}

SourceLocation getLocalRangeEnd() const {
return getLocalData()->LocalRangeEnd;
}

void setLocalRangeEnd(SourceLocation L) {
getLocalData()->LocalRangeEnd = L;
}

SourceLocation getLParenLoc() const {
return this->getLocalData()->LParenLoc;
}

void setLParenLoc(SourceLocation Loc) {
this->getLocalData()->LParenLoc = Loc;
}

SourceLocation getRParenLoc() const {
return this->getLocalData()->RParenLoc;
}

void setRParenLoc(SourceLocation Loc) {
this->getLocalData()->RParenLoc = Loc;
}

SourceRange getParensRange() const {
return SourceRange(getLParenLoc(), getRParenLoc());
}

SourceRange getExceptionSpecRange() const {
if (hasExceptionSpec())
return *getExceptionSpecRangePtr();
return {};
}

void setExceptionSpecRange(SourceRange R) {
if (hasExceptionSpec())
*getExceptionSpecRangePtr() = R;
}

ArrayRef<ParmVarDecl *> getParams() const {
return llvm::makeArrayRef(getParmArray(), getNumParams());
}


ParmVarDecl **getParmArray() const {
return (ParmVarDecl**) getExtraLocalData();
}

unsigned getNumParams() const {
if (isa<FunctionNoProtoType>(getTypePtr()))
return 0;
return cast<FunctionProtoType>(getTypePtr())->getNumParams();
}

ParmVarDecl *getParam(unsigned i) const { return getParmArray()[i]; }
void setParam(unsigned i, ParmVarDecl *VD) { getParmArray()[i] = VD; }

TypeLoc getReturnLoc() const {
return getInnerTypeLoc();
}

SourceRange getLocalSourceRange() const {
return SourceRange(getLocalRangeBegin(), getLocalRangeEnd());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setLocalRangeBegin(Loc);
setLParenLoc(Loc);
setRParenLoc(Loc);
setLocalRangeEnd(Loc);
for (unsigned i = 0, e = getNumParams(); i != e; ++i)
setParam(i, nullptr);
if (hasExceptionSpec())
setExceptionSpecRange(Loc);
}



unsigned getExtraLocalDataSize() const {
unsigned ExceptSpecSize = hasExceptionSpec() ? sizeof(SourceRange) : 0;
return (getNumParams() * sizeof(ParmVarDecl *)) + ExceptSpecSize;
}

unsigned getExtraLocalDataAlignment() const { return alignof(ParmVarDecl *); }

QualType getInnerType() const { return getTypePtr()->getReturnType(); }
};

class FunctionProtoTypeLoc :
public InheritingConcreteTypeLoc<FunctionTypeLoc,
FunctionProtoTypeLoc,
FunctionProtoType> {
};

class FunctionNoProtoTypeLoc :
public InheritingConcreteTypeLoc<FunctionTypeLoc,
FunctionNoProtoTypeLoc,
FunctionNoProtoType> {
};

struct ArrayLocInfo {
SourceLocation LBracketLoc, RBracketLoc;
Expr *Size;
};


class ArrayTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
ArrayTypeLoc,
ArrayType,
ArrayLocInfo> {
public:
SourceLocation getLBracketLoc() const {
return getLocalData()->LBracketLoc;
}

void setLBracketLoc(SourceLocation Loc) {
getLocalData()->LBracketLoc = Loc;
}

SourceLocation getRBracketLoc() const {
return getLocalData()->RBracketLoc;
}

void setRBracketLoc(SourceLocation Loc) {
getLocalData()->RBracketLoc = Loc;
}

SourceRange getBracketsRange() const {
return SourceRange(getLBracketLoc(), getRBracketLoc());
}

Expr *getSizeExpr() const {
return getLocalData()->Size;
}

void setSizeExpr(Expr *Size) {
getLocalData()->Size = Size;
}

TypeLoc getElementLoc() const {
return getInnerTypeLoc();
}

SourceRange getLocalSourceRange() const {
return SourceRange(getLBracketLoc(), getRBracketLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setLBracketLoc(Loc);
setRBracketLoc(Loc);
setSizeExpr(nullptr);
}

QualType getInnerType() const { return getTypePtr()->getElementType(); }
};

class ConstantArrayTypeLoc :
public InheritingConcreteTypeLoc<ArrayTypeLoc,
ConstantArrayTypeLoc,
ConstantArrayType> {
};

class IncompleteArrayTypeLoc :
public InheritingConcreteTypeLoc<ArrayTypeLoc,
IncompleteArrayTypeLoc,
IncompleteArrayType> {
};

class DependentSizedArrayTypeLoc :
public InheritingConcreteTypeLoc<ArrayTypeLoc,
DependentSizedArrayTypeLoc,
DependentSizedArrayType> {
public:
void initializeLocal(ASTContext &Context, SourceLocation Loc) {
ArrayTypeLoc::initializeLocal(Context, Loc);
setSizeExpr(getTypePtr()->getSizeExpr());
}
};

class VariableArrayTypeLoc :
public InheritingConcreteTypeLoc<ArrayTypeLoc,
VariableArrayTypeLoc,
VariableArrayType> {
};


struct TemplateNameLocInfo {
SourceLocation NameLoc;
};

struct TemplateSpecializationLocInfo : TemplateNameLocInfo {
SourceLocation TemplateKWLoc;
SourceLocation LAngleLoc;
SourceLocation RAngleLoc;
};

class TemplateSpecializationTypeLoc :
public ConcreteTypeLoc<UnqualTypeLoc,
TemplateSpecializationTypeLoc,
TemplateSpecializationType,
TemplateSpecializationLocInfo> {
public:
SourceLocation getTemplateKeywordLoc() const {
return getLocalData()->TemplateKWLoc;
}

void setTemplateKeywordLoc(SourceLocation Loc) {
getLocalData()->TemplateKWLoc = Loc;
}

SourceLocation getLAngleLoc() const {
return getLocalData()->LAngleLoc;
}

void setLAngleLoc(SourceLocation Loc) {
getLocalData()->LAngleLoc = Loc;
}

SourceLocation getRAngleLoc() const {
return getLocalData()->RAngleLoc;
}

void setRAngleLoc(SourceLocation Loc) {
getLocalData()->RAngleLoc = Loc;
}

unsigned getNumArgs() const {
return getTypePtr()->getNumArgs();
}

void setArgLocInfo(unsigned i, TemplateArgumentLocInfo AI) {
getArgInfos()[i] = AI;
}

TemplateArgumentLocInfo getArgLocInfo(unsigned i) const {
return getArgInfos()[i];
}

TemplateArgumentLoc getArgLoc(unsigned i) const {
return TemplateArgumentLoc(getTypePtr()->getArg(i), getArgLocInfo(i));
}

SourceLocation getTemplateNameLoc() const {
return getLocalData()->NameLoc;
}

void setTemplateNameLoc(SourceLocation Loc) {
getLocalData()->NameLoc = Loc;
}


void copy(TemplateSpecializationTypeLoc Loc) {
unsigned size = getFullDataSize();
assert(size == Loc.getFullDataSize());





memcpy(Data, Loc.Data, size);
}

SourceRange getLocalSourceRange() const {
if (getTemplateKeywordLoc().isValid())
return SourceRange(getTemplateKeywordLoc(), getRAngleLoc());
else
return SourceRange(getTemplateNameLoc(), getRAngleLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setTemplateKeywordLoc(Loc);
setTemplateNameLoc(Loc);
setLAngleLoc(Loc);
setRAngleLoc(Loc);
initializeArgLocs(Context, getNumArgs(), getTypePtr()->getArgs(),
getArgInfos(), Loc);
}

static void initializeArgLocs(ASTContext &Context, unsigned NumArgs,
const TemplateArgument *Args,
TemplateArgumentLocInfo *ArgInfos,
SourceLocation Loc);

unsigned getExtraLocalDataSize() const {
return getNumArgs() * sizeof(TemplateArgumentLocInfo);
}

unsigned getExtraLocalDataAlignment() const {
return alignof(TemplateArgumentLocInfo);
}

private:
TemplateArgumentLocInfo *getArgInfos() const {
return static_cast<TemplateArgumentLocInfo*>(getExtraLocalData());
}
};

struct DependentAddressSpaceLocInfo {
Expr *ExprOperand;
SourceRange OperandParens;
SourceLocation AttrLoc;
};

class DependentAddressSpaceTypeLoc
: public ConcreteTypeLoc<UnqualTypeLoc,
DependentAddressSpaceTypeLoc,
DependentAddressSpaceType,
DependentAddressSpaceLocInfo> {
public:



SourceLocation getAttrNameLoc() const {
return getLocalData()->AttrLoc;
}
void setAttrNameLoc(SourceLocation loc) {
getLocalData()->AttrLoc = loc;
}




Expr *getAttrExprOperand() const {
return getLocalData()->ExprOperand;
}
void setAttrExprOperand(Expr *e) {
getLocalData()->ExprOperand = e;
}





SourceRange getAttrOperandParensRange() const {
return getLocalData()->OperandParens;
}
void setAttrOperandParensRange(SourceRange range) {
getLocalData()->OperandParens = range;
}

SourceRange getLocalSourceRange() const {
SourceRange range(getAttrNameLoc());
range.setEnd(getAttrOperandParensRange().getEnd());
return range;
}





QualType getInnerType() const {
return this->getTypePtr()->getPointeeType();
}

TypeLoc getPointeeTypeLoc() const {
return this->getInnerTypeLoc();
}

void initializeLocal(ASTContext &Context, SourceLocation loc) {
setAttrNameLoc(loc);
setAttrOperandParensRange(loc);
setAttrOperandParensRange(SourceRange(loc));
setAttrExprOperand(getTypePtr()->getAddrSpaceExpr());
}
};









struct VectorTypeLocInfo {
SourceLocation NameLoc;
};

class VectorTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc, VectorTypeLoc,
VectorType, VectorTypeLocInfo> {
public:
SourceLocation getNameLoc() const { return this->getLocalData()->NameLoc; }

void setNameLoc(SourceLocation Loc) { this->getLocalData()->NameLoc = Loc; }

SourceRange getLocalSourceRange() const {
return SourceRange(getNameLoc(), getNameLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setNameLoc(Loc);
}

TypeLoc getElementLoc() const { return getInnerTypeLoc(); }

QualType getInnerType() const { return this->getTypePtr()->getElementType(); }
};



class DependentVectorTypeLoc
: public ConcreteTypeLoc<UnqualTypeLoc, DependentVectorTypeLoc,
DependentVectorType, VectorTypeLocInfo> {
public:
SourceLocation getNameLoc() const { return this->getLocalData()->NameLoc; }

void setNameLoc(SourceLocation Loc) { this->getLocalData()->NameLoc = Loc; }

SourceRange getLocalSourceRange() const {
return SourceRange(getNameLoc(), getNameLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setNameLoc(Loc);
}

TypeLoc getElementLoc() const { return getInnerTypeLoc(); }

QualType getInnerType() const { return this->getTypePtr()->getElementType(); }
};


class ExtVectorTypeLoc
: public InheritingConcreteTypeLoc<VectorTypeLoc, ExtVectorTypeLoc,
ExtVectorType> {};



class DependentSizedExtVectorTypeLoc
: public ConcreteTypeLoc<UnqualTypeLoc, DependentSizedExtVectorTypeLoc,
DependentSizedExtVectorType, VectorTypeLocInfo> {
public:
SourceLocation getNameLoc() const { return this->getLocalData()->NameLoc; }

void setNameLoc(SourceLocation Loc) { this->getLocalData()->NameLoc = Loc; }

SourceRange getLocalSourceRange() const {
return SourceRange(getNameLoc(), getNameLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setNameLoc(Loc);
}

TypeLoc getElementLoc() const { return getInnerTypeLoc(); }

QualType getInnerType() const { return this->getTypePtr()->getElementType(); }
};

struct MatrixTypeLocInfo {
SourceLocation AttrLoc;
SourceRange OperandParens;
Expr *RowOperand;
Expr *ColumnOperand;
};

class MatrixTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc, MatrixTypeLoc,
MatrixType, MatrixTypeLocInfo> {
public:



SourceLocation getAttrNameLoc() const { return getLocalData()->AttrLoc; }
void setAttrNameLoc(SourceLocation loc) { getLocalData()->AttrLoc = loc; }




Expr *getAttrRowOperand() const { return getLocalData()->RowOperand; }
void setAttrRowOperand(Expr *e) { getLocalData()->RowOperand = e; }




Expr *getAttrColumnOperand() const { return getLocalData()->ColumnOperand; }
void setAttrColumnOperand(Expr *e) { getLocalData()->ColumnOperand = e; }





SourceRange getAttrOperandParensRange() const {
return getLocalData()->OperandParens;
}
void setAttrOperandParensRange(SourceRange range) {
getLocalData()->OperandParens = range;
}

SourceRange getLocalSourceRange() const {
SourceRange range(getAttrNameLoc());
range.setEnd(getAttrOperandParensRange().getEnd());
return range;
}

void initializeLocal(ASTContext &Context, SourceLocation loc) {
setAttrNameLoc(loc);
setAttrOperandParensRange(loc);
setAttrRowOperand(nullptr);
setAttrColumnOperand(nullptr);
}
};

class ConstantMatrixTypeLoc
: public InheritingConcreteTypeLoc<MatrixTypeLoc, ConstantMatrixTypeLoc,
ConstantMatrixType> {};

class DependentSizedMatrixTypeLoc
: public InheritingConcreteTypeLoc<MatrixTypeLoc,
DependentSizedMatrixTypeLoc,
DependentSizedMatrixType> {};


class ComplexTypeLoc : public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
ComplexTypeLoc,
ComplexType> {
};

struct TypeofLocInfo {
SourceLocation TypeofLoc;
SourceLocation LParenLoc;
SourceLocation RParenLoc;
};

struct TypeOfExprTypeLocInfo : public TypeofLocInfo {
};

struct TypeOfTypeLocInfo : public TypeofLocInfo {
TypeSourceInfo* UnderlyingTInfo;
};

template <class Derived, class TypeClass, class LocalData = TypeofLocInfo>
class TypeofLikeTypeLoc
: public ConcreteTypeLoc<UnqualTypeLoc, Derived, TypeClass, LocalData> {
public:
SourceLocation getTypeofLoc() const {
return this->getLocalData()->TypeofLoc;
}

void setTypeofLoc(SourceLocation Loc) {
this->getLocalData()->TypeofLoc = Loc;
}

SourceLocation getLParenLoc() const {
return this->getLocalData()->LParenLoc;
}

void setLParenLoc(SourceLocation Loc) {
this->getLocalData()->LParenLoc = Loc;
}

SourceLocation getRParenLoc() const {
return this->getLocalData()->RParenLoc;
}

void setRParenLoc(SourceLocation Loc) {
this->getLocalData()->RParenLoc = Loc;
}

SourceRange getParensRange() const {
return SourceRange(getLParenLoc(), getRParenLoc());
}

void setParensRange(SourceRange range) {
setLParenLoc(range.getBegin());
setRParenLoc(range.getEnd());
}

SourceRange getLocalSourceRange() const {
return SourceRange(getTypeofLoc(), getRParenLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setTypeofLoc(Loc);
setLParenLoc(Loc);
setRParenLoc(Loc);
}
};

class TypeOfExprTypeLoc : public TypeofLikeTypeLoc<TypeOfExprTypeLoc,
TypeOfExprType,
TypeOfExprTypeLocInfo> {
public:
Expr* getUnderlyingExpr() const {
return getTypePtr()->getUnderlyingExpr();
}




SourceRange getLocalSourceRange() const;
};

class TypeOfTypeLoc
: public TypeofLikeTypeLoc<TypeOfTypeLoc, TypeOfType, TypeOfTypeLocInfo> {
public:
QualType getUnderlyingType() const {
return this->getTypePtr()->getUnderlyingType();
}

TypeSourceInfo* getUnderlyingTInfo() const {
return this->getLocalData()->UnderlyingTInfo;
}

void setUnderlyingTInfo(TypeSourceInfo* TI) const {
this->getLocalData()->UnderlyingTInfo = TI;
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);
};


class DecltypeTypeLoc : public InheritingConcreteTypeLoc<TypeSpecTypeLoc,
DecltypeTypeLoc,
DecltypeType> {
public:
Expr *getUnderlyingExpr() const { return getTypePtr()->getUnderlyingExpr(); }
};

struct UnaryTransformTypeLocInfo {


SourceLocation KWLoc, LParenLoc, RParenLoc;
TypeSourceInfo *UnderlyingTInfo;
};

class UnaryTransformTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
UnaryTransformTypeLoc,
UnaryTransformType,
UnaryTransformTypeLocInfo> {
public:
SourceLocation getKWLoc() const { return getLocalData()->KWLoc; }
void setKWLoc(SourceLocation Loc) { getLocalData()->KWLoc = Loc; }

SourceLocation getLParenLoc() const { return getLocalData()->LParenLoc; }
void setLParenLoc(SourceLocation Loc) { getLocalData()->LParenLoc = Loc; }

SourceLocation getRParenLoc() const { return getLocalData()->RParenLoc; }
void setRParenLoc(SourceLocation Loc) { getLocalData()->RParenLoc = Loc; }

TypeSourceInfo* getUnderlyingTInfo() const {
return getLocalData()->UnderlyingTInfo;
}

void setUnderlyingTInfo(TypeSourceInfo *TInfo) {
getLocalData()->UnderlyingTInfo = TInfo;
}

SourceRange getLocalSourceRange() const {
return SourceRange(getKWLoc(), getRParenLoc());
}

SourceRange getParensRange() const {
return SourceRange(getLParenLoc(), getRParenLoc());
}

void setParensRange(SourceRange Range) {
setLParenLoc(Range.getBegin());
setRParenLoc(Range.getEnd());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);
};

class DeducedTypeLoc
: public InheritingConcreteTypeLoc<TypeSpecTypeLoc, DeducedTypeLoc,
DeducedType> {};

struct AutoTypeLocInfo : TypeSpecLocInfo {
NestedNameSpecifierLoc NestedNameSpec;
SourceLocation TemplateKWLoc;
SourceLocation ConceptNameLoc;
NamedDecl *FoundDecl;
SourceLocation LAngleLoc;
SourceLocation RAngleLoc;
};

class AutoTypeLoc
: public ConcreteTypeLoc<DeducedTypeLoc,
AutoTypeLoc,
AutoType,
AutoTypeLocInfo> {
public:
AutoTypeKeyword getAutoKeyword() const {
return getTypePtr()->getKeyword();
}

bool isConstrained() const {
return getTypePtr()->isConstrained();
}

const NestedNameSpecifierLoc &getNestedNameSpecifierLoc() const {
return getLocalData()->NestedNameSpec;
}

void setNestedNameSpecifierLoc(NestedNameSpecifierLoc NNS) {
getLocalData()->NestedNameSpec = NNS;
}

SourceLocation getTemplateKWLoc() const {
return getLocalData()->TemplateKWLoc;
}

void setTemplateKWLoc(SourceLocation Loc) {
getLocalData()->TemplateKWLoc = Loc;
}

SourceLocation getConceptNameLoc() const {
return getLocalData()->ConceptNameLoc;
}

void setConceptNameLoc(SourceLocation Loc) {
getLocalData()->ConceptNameLoc = Loc;
}

NamedDecl *getFoundDecl() const {
return getLocalData()->FoundDecl;
}

void setFoundDecl(NamedDecl *D) {
getLocalData()->FoundDecl = D;
}

ConceptDecl *getNamedConcept() const {
return getTypePtr()->getTypeConstraintConcept();
}

DeclarationNameInfo getConceptNameInfo() const;

bool hasExplicitTemplateArgs() const {
return getLocalData()->LAngleLoc.isValid();
}

SourceLocation getLAngleLoc() const {
return this->getLocalData()->LAngleLoc;
}

void setLAngleLoc(SourceLocation Loc) {
this->getLocalData()->LAngleLoc = Loc;
}

SourceLocation getRAngleLoc() const {
return this->getLocalData()->RAngleLoc;
}

void setRAngleLoc(SourceLocation Loc) {
this->getLocalData()->RAngleLoc = Loc;
}

unsigned getNumArgs() const {
return getTypePtr()->getNumArgs();
}

void setArgLocInfo(unsigned i, TemplateArgumentLocInfo AI) {
getArgInfos()[i] = AI;
}

TemplateArgumentLocInfo getArgLocInfo(unsigned i) const {
return getArgInfos()[i];
}

TemplateArgumentLoc getArgLoc(unsigned i) const {
return TemplateArgumentLoc(getTypePtr()->getTypeConstraintArguments()[i],
getArgLocInfo(i));
}

SourceRange getLocalSourceRange() const {
return{
isConstrained()
? (getNestedNameSpecifierLoc()
? getNestedNameSpecifierLoc().getBeginLoc()
: (getTemplateKWLoc().isValid()
? getTemplateKWLoc()
: getConceptNameLoc()))
: getNameLoc(),
getNameLoc()
};
}

void copy(AutoTypeLoc Loc) {
unsigned size = getFullDataSize();
assert(size == Loc.getFullDataSize());
memcpy(Data, Loc.Data, size);
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);

unsigned getExtraLocalDataSize() const {
return getNumArgs() * sizeof(TemplateArgumentLocInfo);
}

unsigned getExtraLocalDataAlignment() const {
return alignof(TemplateArgumentLocInfo);
}

private:
TemplateArgumentLocInfo *getArgInfos() const {
return static_cast<TemplateArgumentLocInfo*>(getExtraLocalData());
}
};

class DeducedTemplateSpecializationTypeLoc
: public InheritingConcreteTypeLoc<DeducedTypeLoc,
DeducedTemplateSpecializationTypeLoc,
DeducedTemplateSpecializationType> {
public:
SourceLocation getTemplateNameLoc() const {
return getNameLoc();
}

void setTemplateNameLoc(SourceLocation Loc) {
setNameLoc(Loc);
}
};

struct ElaboratedLocInfo {
SourceLocation ElaboratedKWLoc;


void *QualifierData;
};

class ElaboratedTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
ElaboratedTypeLoc,
ElaboratedType,
ElaboratedLocInfo> {
public:
SourceLocation getElaboratedKeywordLoc() const {
return this->getLocalData()->ElaboratedKWLoc;
}

void setElaboratedKeywordLoc(SourceLocation Loc) {
this->getLocalData()->ElaboratedKWLoc = Loc;
}

NestedNameSpecifierLoc getQualifierLoc() const {
return NestedNameSpecifierLoc(getTypePtr()->getQualifier(),
getLocalData()->QualifierData);
}

void setQualifierLoc(NestedNameSpecifierLoc QualifierLoc) {
assert(QualifierLoc.getNestedNameSpecifier()
== getTypePtr()->getQualifier() &&
"Inconsistent nested-name-specifier pointer");
getLocalData()->QualifierData = QualifierLoc.getOpaqueData();
}

SourceRange getLocalSourceRange() const {
if (getElaboratedKeywordLoc().isValid())
if (getQualifierLoc())
return SourceRange(getElaboratedKeywordLoc(),
getQualifierLoc().getEndLoc());
else
return SourceRange(getElaboratedKeywordLoc());
else
return getQualifierLoc().getSourceRange();
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);

TypeLoc getNamedTypeLoc() const {
return getInnerTypeLoc();
}

QualType getInnerType() const {
return getTypePtr()->getNamedType();
}

void copy(ElaboratedTypeLoc Loc) {
unsigned size = getFullDataSize();
assert(size == Loc.getFullDataSize());
memcpy(Data, Loc.Data, size);
}
};



struct DependentNameLocInfo : ElaboratedLocInfo {
SourceLocation NameLoc;
};

class DependentNameTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc,
DependentNameTypeLoc,
DependentNameType,
DependentNameLocInfo> {
public:
SourceLocation getElaboratedKeywordLoc() const {
return this->getLocalData()->ElaboratedKWLoc;
}

void setElaboratedKeywordLoc(SourceLocation Loc) {
this->getLocalData()->ElaboratedKWLoc = Loc;
}

NestedNameSpecifierLoc getQualifierLoc() const {
return NestedNameSpecifierLoc(getTypePtr()->getQualifier(),
getLocalData()->QualifierData);
}

void setQualifierLoc(NestedNameSpecifierLoc QualifierLoc) {
assert(QualifierLoc.getNestedNameSpecifier()
== getTypePtr()->getQualifier() &&
"Inconsistent nested-name-specifier pointer");
getLocalData()->QualifierData = QualifierLoc.getOpaqueData();
}

SourceLocation getNameLoc() const {
return this->getLocalData()->NameLoc;
}

void setNameLoc(SourceLocation Loc) {
this->getLocalData()->NameLoc = Loc;
}

SourceRange getLocalSourceRange() const {
if (getElaboratedKeywordLoc().isValid())
return SourceRange(getElaboratedKeywordLoc(), getNameLoc());
else
return SourceRange(getQualifierLoc().getBeginLoc(), getNameLoc());
}

void copy(DependentNameTypeLoc Loc) {
unsigned size = getFullDataSize();
assert(size == Loc.getFullDataSize());
memcpy(Data, Loc.Data, size);
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);
};

struct DependentTemplateSpecializationLocInfo : DependentNameLocInfo {
SourceLocation TemplateKWLoc;
SourceLocation LAngleLoc;
SourceLocation RAngleLoc;

};

class DependentTemplateSpecializationTypeLoc :
public ConcreteTypeLoc<UnqualTypeLoc,
DependentTemplateSpecializationTypeLoc,
DependentTemplateSpecializationType,
DependentTemplateSpecializationLocInfo> {
public:
SourceLocation getElaboratedKeywordLoc() const {
return this->getLocalData()->ElaboratedKWLoc;
}

void setElaboratedKeywordLoc(SourceLocation Loc) {
this->getLocalData()->ElaboratedKWLoc = Loc;
}

NestedNameSpecifierLoc getQualifierLoc() const {
if (!getLocalData()->QualifierData)
return NestedNameSpecifierLoc();

return NestedNameSpecifierLoc(getTypePtr()->getQualifier(),
getLocalData()->QualifierData);
}

void setQualifierLoc(NestedNameSpecifierLoc QualifierLoc) {
if (!QualifierLoc) {




getLocalData()->QualifierData = nullptr;
return;
}

assert(QualifierLoc.getNestedNameSpecifier()
== getTypePtr()->getQualifier() &&
"Inconsistent nested-name-specifier pointer");
getLocalData()->QualifierData = QualifierLoc.getOpaqueData();
}

SourceLocation getTemplateKeywordLoc() const {
return getLocalData()->TemplateKWLoc;
}

void setTemplateKeywordLoc(SourceLocation Loc) {
getLocalData()->TemplateKWLoc = Loc;
}

SourceLocation getTemplateNameLoc() const {
return this->getLocalData()->NameLoc;
}

void setTemplateNameLoc(SourceLocation Loc) {
this->getLocalData()->NameLoc = Loc;
}

SourceLocation getLAngleLoc() const {
return this->getLocalData()->LAngleLoc;
}

void setLAngleLoc(SourceLocation Loc) {
this->getLocalData()->LAngleLoc = Loc;
}

SourceLocation getRAngleLoc() const {
return this->getLocalData()->RAngleLoc;
}

void setRAngleLoc(SourceLocation Loc) {
this->getLocalData()->RAngleLoc = Loc;
}

unsigned getNumArgs() const {
return getTypePtr()->getNumArgs();
}

void setArgLocInfo(unsigned i, TemplateArgumentLocInfo AI) {
getArgInfos()[i] = AI;
}

TemplateArgumentLocInfo getArgLocInfo(unsigned i) const {
return getArgInfos()[i];
}

TemplateArgumentLoc getArgLoc(unsigned i) const {
return TemplateArgumentLoc(getTypePtr()->getArg(i), getArgLocInfo(i));
}

SourceRange getLocalSourceRange() const {
if (getElaboratedKeywordLoc().isValid())
return SourceRange(getElaboratedKeywordLoc(), getRAngleLoc());
else if (getQualifierLoc())
return SourceRange(getQualifierLoc().getBeginLoc(), getRAngleLoc());
else if (getTemplateKeywordLoc().isValid())
return SourceRange(getTemplateKeywordLoc(), getRAngleLoc());
else
return SourceRange(getTemplateNameLoc(), getRAngleLoc());
}

void copy(DependentTemplateSpecializationTypeLoc Loc) {
unsigned size = getFullDataSize();
assert(size == Loc.getFullDataSize());
memcpy(Data, Loc.Data, size);
}

void initializeLocal(ASTContext &Context, SourceLocation Loc);

unsigned getExtraLocalDataSize() const {
return getNumArgs() * sizeof(TemplateArgumentLocInfo);
}

unsigned getExtraLocalDataAlignment() const {
return alignof(TemplateArgumentLocInfo);
}

private:
TemplateArgumentLocInfo *getArgInfos() const {
return static_cast<TemplateArgumentLocInfo*>(getExtraLocalData());
}
};

struct PackExpansionTypeLocInfo {
SourceLocation EllipsisLoc;
};

class PackExpansionTypeLoc
: public ConcreteTypeLoc<UnqualTypeLoc, PackExpansionTypeLoc,
PackExpansionType, PackExpansionTypeLocInfo> {
public:
SourceLocation getEllipsisLoc() const {
return this->getLocalData()->EllipsisLoc;
}

void setEllipsisLoc(SourceLocation Loc) {
this->getLocalData()->EllipsisLoc = Loc;
}

SourceRange getLocalSourceRange() const {
return SourceRange(getEllipsisLoc(), getEllipsisLoc());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setEllipsisLoc(Loc);
}

TypeLoc getPatternLoc() const {
return getInnerTypeLoc();
}

QualType getInnerType() const {
return this->getTypePtr()->getPattern();
}
};

struct AtomicTypeLocInfo {
SourceLocation KWLoc, LParenLoc, RParenLoc;
};

class AtomicTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc, AtomicTypeLoc,
AtomicType, AtomicTypeLocInfo> {
public:
TypeLoc getValueLoc() const {
return this->getInnerTypeLoc();
}

SourceRange getLocalSourceRange() const {
return SourceRange(getKWLoc(), getRParenLoc());
}

SourceLocation getKWLoc() const {
return this->getLocalData()->KWLoc;
}

void setKWLoc(SourceLocation Loc) {
this->getLocalData()->KWLoc = Loc;
}

SourceLocation getLParenLoc() const {
return this->getLocalData()->LParenLoc;
}

void setLParenLoc(SourceLocation Loc) {
this->getLocalData()->LParenLoc = Loc;
}

SourceLocation getRParenLoc() const {
return this->getLocalData()->RParenLoc;
}

void setRParenLoc(SourceLocation Loc) {
this->getLocalData()->RParenLoc = Loc;
}

SourceRange getParensRange() const {
return SourceRange(getLParenLoc(), getRParenLoc());
}

void setParensRange(SourceRange Range) {
setLParenLoc(Range.getBegin());
setRParenLoc(Range.getEnd());
}

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setKWLoc(Loc);
setLParenLoc(Loc);
setRParenLoc(Loc);
}

QualType getInnerType() const {
return this->getTypePtr()->getValueType();
}
};

struct PipeTypeLocInfo {
SourceLocation KWLoc;
};

class PipeTypeLoc : public ConcreteTypeLoc<UnqualTypeLoc, PipeTypeLoc, PipeType,
PipeTypeLocInfo> {
public:
TypeLoc getValueLoc() const { return this->getInnerTypeLoc(); }

SourceRange getLocalSourceRange() const { return SourceRange(getKWLoc()); }

SourceLocation getKWLoc() const { return this->getLocalData()->KWLoc; }
void setKWLoc(SourceLocation Loc) { this->getLocalData()->KWLoc = Loc; }

void initializeLocal(ASTContext &Context, SourceLocation Loc) {
setKWLoc(Loc);
}

QualType getInnerType() const { return this->getTypePtr()->getElementType(); }
};

template <typename T>
inline T TypeLoc::getAsAdjusted() const {
TypeLoc Cur = *this;
while (!T::isKind(Cur)) {
if (auto PTL = Cur.getAs<ParenTypeLoc>())
Cur = PTL.getInnerLoc();
else if (auto ATL = Cur.getAs<AttributedTypeLoc>())
Cur = ATL.getModifiedLoc();
else if (auto ETL = Cur.getAs<ElaboratedTypeLoc>())
Cur = ETL.getNamedTypeLoc();
else if (auto ATL = Cur.getAs<AdjustedTypeLoc>())
Cur = ATL.getOriginalLoc();
else if (auto MQL = Cur.getAs<MacroQualifiedTypeLoc>())
Cur = MQL.getInnerLoc();
else
break;
}
return Cur.getAs<T>();
}
class ExtIntTypeLoc final
: public InheritingConcreteTypeLoc<TypeSpecTypeLoc, ExtIntTypeLoc,
ExtIntType> {};
class DependentExtIntTypeLoc final
: public InheritingConcreteTypeLoc<TypeSpecTypeLoc, DependentExtIntTypeLoc,
DependentExtIntType> {};

}

#endif
