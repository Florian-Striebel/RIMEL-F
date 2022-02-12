















#if !defined(LLVM_CLANG_AST_TYPE_H)
#define LLVM_CLANG_AST_TYPE_H

#include "clang/AST/DependenceFlags.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/TemplateName.h"
#include "clang/Basic/AddressSpaces.h"
#include "clang/Basic/AttrKinds.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/ExceptionSpecificationType.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/Linkage.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Basic/Visibility.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/type_traits.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

namespace clang {

class ExtQuals;
class QualType;
class ConceptDecl;
class TagDecl;
class TemplateParameterList;
class Type;

enum {
TypeAlignmentInBits = 4,
TypeAlignment = 1 << TypeAlignmentInBits
};

namespace serialization {
template <class T> class AbstractTypeReader;
template <class T> class AbstractTypeWriter;
}

}

namespace llvm {

template <typename T>
struct PointerLikeTypeTraits;
template<>
struct PointerLikeTypeTraits< ::clang::Type*> {
static inline void *getAsVoidPointer(::clang::Type *P) { return P; }

static inline ::clang::Type *getFromVoidPointer(void *P) {
return static_cast< ::clang::Type*>(P);
}

static constexpr int NumLowBitsAvailable = clang::TypeAlignmentInBits;
};

template<>
struct PointerLikeTypeTraits< ::clang::ExtQuals*> {
static inline void *getAsVoidPointer(::clang::ExtQuals *P) { return P; }

static inline ::clang::ExtQuals *getFromVoidPointer(void *P) {
return static_cast< ::clang::ExtQuals*>(P);
}

static constexpr int NumLowBitsAvailable = clang::TypeAlignmentInBits;
};

}

namespace clang {

class ASTContext;
template <typename> class CanQual;
class CXXRecordDecl;
class DeclContext;
class EnumDecl;
class Expr;
class ExtQualsTypeCommonBase;
class FunctionDecl;
class IdentifierInfo;
class NamedDecl;
class ObjCInterfaceDecl;
class ObjCProtocolDecl;
class ObjCTypeParamDecl;
struct PrintingPolicy;
class RecordDecl;
class Stmt;
class TagDecl;
class TemplateArgument;
class TemplateArgumentListInfo;
class TemplateArgumentLoc;
class TemplateTypeParmDecl;
class TypedefNameDecl;
class UnresolvedUsingTypenameDecl;

using CanQualType = CanQual<Type>;


#define TYPE(Class, Base) class Class##Type;
#include "clang/AST/TypeNodes.inc"







class Qualifiers {
public:
enum TQ {
Const = 0x1,
Restrict = 0x2,
Volatile = 0x4,
CVRMask = Const | Volatile | Restrict
};

enum GC {
GCNone = 0,
Weak,
Strong
};

enum ObjCLifetime {

OCL_None,



OCL_ExplicitNone,






OCL_Strong,


OCL_Weak,


OCL_Autoreleasing
};

enum {


MaxAddressSpace = 0x7fffffu,


FastWidth = 3,


FastMask = (1 << FastWidth) - 1
};



static Qualifiers removeCommonQualifiers(Qualifiers &L, Qualifiers &R) {

if (!(L.Mask & ~CVRMask) && !(R.Mask & ~CVRMask)) {
Qualifiers Q;
Q.Mask = L.Mask & R.Mask;
L.Mask &= ~Q.Mask;
R.Mask &= ~Q.Mask;
return Q;
}

Qualifiers Q;
unsigned CommonCRV = L.getCVRQualifiers() & R.getCVRQualifiers();
Q.addCVRQualifiers(CommonCRV);
L.removeCVRQualifiers(CommonCRV);
R.removeCVRQualifiers(CommonCRV);

if (L.getObjCGCAttr() == R.getObjCGCAttr()) {
Q.setObjCGCAttr(L.getObjCGCAttr());
L.removeObjCGCAttr();
R.removeObjCGCAttr();
}

if (L.getObjCLifetime() == R.getObjCLifetime()) {
Q.setObjCLifetime(L.getObjCLifetime());
L.removeObjCLifetime();
R.removeObjCLifetime();
}

if (L.getAddressSpace() == R.getAddressSpace()) {
Q.setAddressSpace(L.getAddressSpace());
L.removeAddressSpace();
R.removeAddressSpace();
}
return Q;
}

static Qualifiers fromFastMask(unsigned Mask) {
Qualifiers Qs;
Qs.addFastQualifiers(Mask);
return Qs;
}

static Qualifiers fromCVRMask(unsigned CVR) {
Qualifiers Qs;
Qs.addCVRQualifiers(CVR);
return Qs;
}

static Qualifiers fromCVRUMask(unsigned CVRU) {
Qualifiers Qs;
Qs.addCVRUQualifiers(CVRU);
return Qs;
}


static Qualifiers fromOpaqueValue(unsigned opaque) {
Qualifiers Qs;
Qs.Mask = opaque;
return Qs;
}


unsigned getAsOpaqueValue() const {
return Mask;
}

bool hasConst() const { return Mask & Const; }
bool hasOnlyConst() const { return Mask == Const; }
void removeConst() { Mask &= ~Const; }
void addConst() { Mask |= Const; }

bool hasVolatile() const { return Mask & Volatile; }
bool hasOnlyVolatile() const { return Mask == Volatile; }
void removeVolatile() { Mask &= ~Volatile; }
void addVolatile() { Mask |= Volatile; }

bool hasRestrict() const { return Mask & Restrict; }
bool hasOnlyRestrict() const { return Mask == Restrict; }
void removeRestrict() { Mask &= ~Restrict; }
void addRestrict() { Mask |= Restrict; }

bool hasCVRQualifiers() const { return getCVRQualifiers(); }
unsigned getCVRQualifiers() const { return Mask & CVRMask; }
unsigned getCVRUQualifiers() const { return Mask & (CVRMask | UMask); }

void setCVRQualifiers(unsigned mask) {
assert(!(mask & ~CVRMask) && "bitmask contains non-CVR bits");
Mask = (Mask & ~CVRMask) | mask;
}
void removeCVRQualifiers(unsigned mask) {
assert(!(mask & ~CVRMask) && "bitmask contains non-CVR bits");
Mask &= ~mask;
}
void removeCVRQualifiers() {
removeCVRQualifiers(CVRMask);
}
void addCVRQualifiers(unsigned mask) {
assert(!(mask & ~CVRMask) && "bitmask contains non-CVR bits");
Mask |= mask;
}
void addCVRUQualifiers(unsigned mask) {
assert(!(mask & ~CVRMask & ~UMask) && "bitmask contains non-CVRU bits");
Mask |= mask;
}

bool hasUnaligned() const { return Mask & UMask; }
void setUnaligned(bool flag) {
Mask = (Mask & ~UMask) | (flag ? UMask : 0);
}
void removeUnaligned() { Mask &= ~UMask; }
void addUnaligned() { Mask |= UMask; }

bool hasObjCGCAttr() const { return Mask & GCAttrMask; }
GC getObjCGCAttr() const { return GC((Mask & GCAttrMask) >> GCAttrShift); }
void setObjCGCAttr(GC type) {
Mask = (Mask & ~GCAttrMask) | (type << GCAttrShift);
}
void removeObjCGCAttr() { setObjCGCAttr(GCNone); }
void addObjCGCAttr(GC type) {
assert(type);
setObjCGCAttr(type);
}
Qualifiers withoutObjCGCAttr() const {
Qualifiers qs = *this;
qs.removeObjCGCAttr();
return qs;
}
Qualifiers withoutObjCLifetime() const {
Qualifiers qs = *this;
qs.removeObjCLifetime();
return qs;
}
Qualifiers withoutAddressSpace() const {
Qualifiers qs = *this;
qs.removeAddressSpace();
return qs;
}

bool hasObjCLifetime() const { return Mask & LifetimeMask; }
ObjCLifetime getObjCLifetime() const {
return ObjCLifetime((Mask & LifetimeMask) >> LifetimeShift);
}
void setObjCLifetime(ObjCLifetime type) {
Mask = (Mask & ~LifetimeMask) | (type << LifetimeShift);
}
void removeObjCLifetime() { setObjCLifetime(OCL_None); }
void addObjCLifetime(ObjCLifetime type) {
assert(type);
assert(!hasObjCLifetime());
Mask |= (type << LifetimeShift);
}


bool hasNonTrivialObjCLifetime() const {
ObjCLifetime lifetime = getObjCLifetime();
return (lifetime > OCL_ExplicitNone);
}


bool hasStrongOrWeakObjCLifetime() const {
ObjCLifetime lifetime = getObjCLifetime();
return (lifetime == OCL_Strong || lifetime == OCL_Weak);
}

bool hasAddressSpace() const { return Mask & AddressSpaceMask; }
LangAS getAddressSpace() const {
return static_cast<LangAS>(Mask >> AddressSpaceShift);
}
bool hasTargetSpecificAddressSpace() const {
return isTargetAddressSpace(getAddressSpace());
}

unsigned getAddressSpaceAttributePrintValue() const {
auto Addr = getAddressSpace();



assert(Addr == LangAS::Default || hasTargetSpecificAddressSpace());
if (Addr != LangAS::Default)
return toTargetAddressSpace(Addr);



return 0;
}
void setAddressSpace(LangAS space) {
assert((unsigned)space <= MaxAddressSpace);
Mask = (Mask & ~AddressSpaceMask)
| (((uint32_t) space) << AddressSpaceShift);
}
void removeAddressSpace() { setAddressSpace(LangAS::Default); }
void addAddressSpace(LangAS space) {
assert(space != LangAS::Default);
setAddressSpace(space);
}



bool hasFastQualifiers() const { return getFastQualifiers(); }
unsigned getFastQualifiers() const { return Mask & FastMask; }
void setFastQualifiers(unsigned mask) {
assert(!(mask & ~FastMask) && "bitmask contains non-fast qualifier bits");
Mask = (Mask & ~FastMask) | mask;
}
void removeFastQualifiers(unsigned mask) {
assert(!(mask & ~FastMask) && "bitmask contains non-fast qualifier bits");
Mask &= ~mask;
}
void removeFastQualifiers() {
removeFastQualifiers(FastMask);
}
void addFastQualifiers(unsigned mask) {
assert(!(mask & ~FastMask) && "bitmask contains non-fast qualifier bits");
Mask |= mask;
}



bool hasNonFastQualifiers() const { return Mask & ~FastMask; }
Qualifiers getNonFastQualifiers() const {
Qualifiers Quals = *this;
Quals.setFastQualifiers(0);
return Quals;
}


bool hasQualifiers() const { return Mask; }
bool empty() const { return !Mask; }


void addQualifiers(Qualifiers Q) {


if (!(Q.Mask & ~CVRMask))
Mask |= Q.Mask;
else {
Mask |= (Q.Mask & CVRMask);
if (Q.hasAddressSpace())
addAddressSpace(Q.getAddressSpace());
if (Q.hasObjCGCAttr())
addObjCGCAttr(Q.getObjCGCAttr());
if (Q.hasObjCLifetime())
addObjCLifetime(Q.getObjCLifetime());
}
}


void removeQualifiers(Qualifiers Q) {


if (!(Q.Mask & ~CVRMask))
Mask &= ~Q.Mask;
else {
Mask &= ~(Q.Mask & CVRMask);
if (getObjCGCAttr() == Q.getObjCGCAttr())
removeObjCGCAttr();
if (getObjCLifetime() == Q.getObjCLifetime())
removeObjCLifetime();
if (getAddressSpace() == Q.getAddressSpace())
removeAddressSpace();
}
}



void addConsistentQualifiers(Qualifiers qs) {
assert(getAddressSpace() == qs.getAddressSpace() ||
!hasAddressSpace() || !qs.hasAddressSpace());
assert(getObjCGCAttr() == qs.getObjCGCAttr() ||
!hasObjCGCAttr() || !qs.hasObjCGCAttr());
assert(getObjCLifetime() == qs.getObjCLifetime() ||
!hasObjCLifetime() || !qs.hasObjCLifetime());
Mask |= qs.Mask;
}








static bool isAddressSpaceSupersetOf(LangAS A, LangAS B) {

return A == B ||


(A == LangAS::opencl_generic && B != LangAS::opencl_constant) ||



(A == LangAS::opencl_global && (B == LangAS::opencl_global_device ||
B == LangAS::opencl_global_host)) ||
(A == LangAS::sycl_global && (B == LangAS::sycl_global_device ||
B == LangAS::sycl_global_host)) ||

((isPtrSizeAddressSpace(A) || A == LangAS::Default) &&
(isPtrSizeAddressSpace(B) || B == LangAS::Default)) ||

(A == LangAS::Default &&
(B == LangAS::sycl_private || B == LangAS::sycl_local ||
B == LangAS::sycl_global || B == LangAS::sycl_global_device ||
B == LangAS::sycl_global_host));
}



bool isAddressSpaceSupersetOf(Qualifiers other) const {
return isAddressSpaceSupersetOf(getAddressSpace(), other.getAddressSpace());
}




bool compatiblyIncludes(Qualifiers other) const {
return isAddressSpaceSupersetOf(other) &&


(getObjCGCAttr() == other.getObjCGCAttr() || !hasObjCGCAttr() ||
!other.hasObjCGCAttr()) &&

getObjCLifetime() == other.getObjCLifetime() &&

(((Mask & CVRMask) | (other.Mask & CVRMask)) == (Mask & CVRMask)) &&

(!other.hasUnaligned() || hasUnaligned());
}








bool compatiblyIncludesObjCLifetime(Qualifiers other) const {
if (getObjCLifetime() == other.getObjCLifetime())
return true;

if (getObjCLifetime() == OCL_Weak || other.getObjCLifetime() == OCL_Weak)
return false;

if (getObjCLifetime() == OCL_None || other.getObjCLifetime() == OCL_None)
return true;

return hasConst();
}



bool isStrictSupersetOf(Qualifiers Other) const;

bool operator==(Qualifiers Other) const { return Mask == Other.Mask; }
bool operator!=(Qualifiers Other) const { return Mask != Other.Mask; }

explicit operator bool() const { return hasQualifiers(); }

Qualifiers &operator+=(Qualifiers R) {
addQualifiers(R);
return *this;
}



friend Qualifiers operator+(Qualifiers L, Qualifiers R) {
L += R;
return L;
}

Qualifiers &operator-=(Qualifiers R) {
removeQualifiers(R);
return *this;
}


friend Qualifiers operator-(Qualifiers L, Qualifiers R) {
L -= R;
return L;
}

std::string getAsString() const;
std::string getAsString(const PrintingPolicy &Policy) const;

static std::string getAddrSpaceAsString(LangAS AS);

bool isEmptyWhenPrinted(const PrintingPolicy &Policy) const;
void print(raw_ostream &OS, const PrintingPolicy &Policy,
bool appendSpaceIfNonEmpty = false) const;

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.AddInteger(Mask);
}

private:


uint32_t Mask = 0;

static const uint32_t UMask = 0x8;
static const uint32_t UShift = 3;
static const uint32_t GCAttrMask = 0x30;
static const uint32_t GCAttrShift = 4;
static const uint32_t LifetimeMask = 0x1C0;
static const uint32_t LifetimeShift = 6;
static const uint32_t AddressSpaceMask =
~(CVRMask | UMask | GCAttrMask | LifetimeMask);
static const uint32_t AddressSpaceShift = 9;
};



struct SplitQualType {

const Type *Ty = nullptr;


Qualifiers Quals;

SplitQualType() = default;
SplitQualType(const Type *ty, Qualifiers qs) : Ty(ty), Quals(qs) {}

SplitQualType getSingleStepDesugaredType() const;


std::pair<const Type *,Qualifiers> asPair() const {
return std::pair<const Type *, Qualifiers>(Ty, Quals);
}

friend bool operator==(SplitQualType a, SplitQualType b) {
return a.Ty == b.Ty && a.Quals == b.Quals;
}
friend bool operator!=(SplitQualType a, SplitQualType b) {
return a.Ty != b.Ty || a.Quals != b.Quals;
}
};






enum class ObjCSubstitutionContext {

Ordinary,


Result,


Parameter,


Property,


Superclass,
};















class QualType {
friend class QualifierCollector;


llvm::PointerIntPair<llvm::PointerUnion<const Type *, const ExtQuals *>,
Qualifiers::FastWidth> Value;

const ExtQuals *getExtQualsUnsafe() const {
return Value.getPointer().get<const ExtQuals*>();
}

const Type *getTypePtrUnsafe() const {
return Value.getPointer().get<const Type*>();
}

const ExtQualsTypeCommonBase *getCommonPtr() const {
assert(!isNull() && "Cannot retrieve a NULL type pointer");
auto CommonPtrVal = reinterpret_cast<uintptr_t>(Value.getOpaqueValue());
CommonPtrVal &= ~(uintptr_t)((1 << TypeAlignmentInBits) - 1);
return reinterpret_cast<ExtQualsTypeCommonBase*>(CommonPtrVal);
}

public:
QualType() = default;
QualType(const Type *Ptr, unsigned Quals) : Value(Ptr, Quals) {}
QualType(const ExtQuals *Ptr, unsigned Quals) : Value(Ptr, Quals) {}

unsigned getLocalFastQualifiers() const { return Value.getInt(); }
void setLocalFastQualifiers(unsigned Quals) { Value.setInt(Quals); }





const Type *getTypePtr() const;

const Type *getTypePtrOrNull() const;


const IdentifierInfo *getBaseTypeIdentifier() const;



SplitQualType split() const;

void *getAsOpaquePtr() const { return Value.getOpaqueValue(); }

static QualType getFromOpaquePtr(const void *Ptr) {
QualType T;
T.Value.setFromOpaqueValue(const_cast<void*>(Ptr));
return T;
}

const Type &operator*() const {
return *getTypePtr();
}

const Type *operator->() const {
return getTypePtr();
}

bool isCanonical() const;
bool isCanonicalAsParam() const;


bool isNull() const {
return Value.getPointer().isNull();
}




bool isLocalConstQualified() const {
return (getLocalFastQualifiers() & Qualifiers::Const);
}


bool isConstQualified() const;




bool isLocalRestrictQualified() const {
return (getLocalFastQualifiers() & Qualifiers::Restrict);
}


bool isRestrictQualified() const;




bool isLocalVolatileQualified() const {
return (getLocalFastQualifiers() & Qualifiers::Volatile);
}


bool isVolatileQualified() const;




bool hasLocalQualifiers() const {
return getLocalFastQualifiers() || hasLocalNonFastQualifiers();
}


bool hasQualifiers() const;




bool hasLocalNonFastQualifiers() const {
return Value.getPointer().is<const ExtQuals*>();
}




Qualifiers getLocalQualifiers() const;


Qualifiers getQualifiers() const;




unsigned getLocalCVRQualifiers() const {
return getLocalFastQualifiers();
}



unsigned getCVRQualifiers() const;

bool isConstant(const ASTContext& Ctx) const {
return QualType::isConstant(*this, Ctx);
}


bool isPODType(const ASTContext &Context) const;



bool isCXX98PODType(const ASTContext &Context) const;





bool isCXX11PODType(const ASTContext &Context) const;


bool isTrivialType(const ASTContext &Context) const;


bool isTriviallyCopyableType(const ASTContext &Context) const;



bool mayBeDynamicClass() const;


bool mayBeNotDynamicClass() const;





void addConst() {
addFastQualifiers(Qualifiers::Const);
}
QualType withConst() const {
return withFastQualifiers(Qualifiers::Const);
}


void addVolatile() {
addFastQualifiers(Qualifiers::Volatile);
}
QualType withVolatile() const {
return withFastQualifiers(Qualifiers::Volatile);
}


void addRestrict() {
addFastQualifiers(Qualifiers::Restrict);
}
QualType withRestrict() const {
return withFastQualifiers(Qualifiers::Restrict);
}

QualType withCVRQualifiers(unsigned CVR) const {
return withFastQualifiers(CVR);
}

void addFastQualifiers(unsigned TQs) {
assert(!(TQs & ~Qualifiers::FastMask)
&& "non-fast qualifier bits set in mask!");
Value.setInt(Value.getInt() | TQs);
}

void removeLocalConst();
void removeLocalVolatile();
void removeLocalRestrict();
void removeLocalCVRQualifiers(unsigned Mask);

void removeLocalFastQualifiers() { Value.setInt(0); }
void removeLocalFastQualifiers(unsigned Mask) {
assert(!(Mask & ~Qualifiers::FastMask) && "mask has non-fast qualifiers");
Value.setInt(Value.getInt() & ~Mask);
}



QualType withFastQualifiers(unsigned TQs) const {
QualType T = *this;
T.addFastQualifiers(TQs);
return T;
}



QualType withExactLocalFastQualifiers(unsigned TQs) const {
return withoutLocalFastQualifiers().withFastQualifiers(TQs);
}


QualType withoutLocalFastQualifiers() const {
QualType T = *this;
T.removeLocalFastQualifiers();
return T;
}

QualType getCanonicalType() const;




QualType getLocalUnqualifiedType() const { return QualType(getTypePtr(), 0); }



















inline QualType getUnqualifiedType() const;










inline SplitQualType getSplitUnqualifiedType() const;



bool isMoreQualifiedThan(QualType Other) const;



bool isAtLeastAsQualifiedAs(QualType Other) const;

QualType getNonReferenceType() const;









QualType getNonLValueExprType(const ASTContext &Context) const;





QualType getNonPackExpansionType() const;









QualType getDesugaredType(const ASTContext &Context) const {
return getDesugaredType(*this, Context);
}

SplitQualType getSplitDesugaredType() const {
return getSplitDesugaredType(*this);
}






QualType getSingleStepDesugaredType(const ASTContext &Context) const {
return getSingleStepDesugaredTypeImpl(*this, Context);
}



QualType IgnoreParens() const {
if (isa<ParenType>(*this))
return QualType::IgnoreParens(*this);
return *this;
}


friend bool operator==(const QualType &LHS, const QualType &RHS) {
return LHS.Value == RHS.Value;
}
friend bool operator!=(const QualType &LHS, const QualType &RHS) {
return LHS.Value != RHS.Value;
}
friend bool operator<(const QualType &LHS, const QualType &RHS) {
return LHS.Value < RHS.Value;
}

static std::string getAsString(SplitQualType split,
const PrintingPolicy &Policy) {
return getAsString(split.Ty, split.Quals, Policy);
}
static std::string getAsString(const Type *ty, Qualifiers qs,
const PrintingPolicy &Policy);

std::string getAsString() const;
std::string getAsString(const PrintingPolicy &Policy) const;

void print(raw_ostream &OS, const PrintingPolicy &Policy,
const Twine &PlaceHolder = Twine(),
unsigned Indentation = 0) const;

static void print(SplitQualType split, raw_ostream &OS,
const PrintingPolicy &policy, const Twine &PlaceHolder,
unsigned Indentation = 0) {
return print(split.Ty, split.Quals, OS, policy, PlaceHolder, Indentation);
}

static void print(const Type *ty, Qualifiers qs,
raw_ostream &OS, const PrintingPolicy &policy,
const Twine &PlaceHolder,
unsigned Indentation = 0);

void getAsStringInternal(std::string &Str,
const PrintingPolicy &Policy) const;

static void getAsStringInternal(SplitQualType split, std::string &out,
const PrintingPolicy &policy) {
return getAsStringInternal(split.Ty, split.Quals, out, policy);
}

static void getAsStringInternal(const Type *ty, Qualifiers qs,
std::string &out,
const PrintingPolicy &policy);

class StreamedQualTypeHelper {
const QualType &T;
const PrintingPolicy &Policy;
const Twine &PlaceHolder;
unsigned Indentation;

public:
StreamedQualTypeHelper(const QualType &T, const PrintingPolicy &Policy,
const Twine &PlaceHolder, unsigned Indentation)
: T(T), Policy(Policy), PlaceHolder(PlaceHolder),
Indentation(Indentation) {}

friend raw_ostream &operator<<(raw_ostream &OS,
const StreamedQualTypeHelper &SQT) {
SQT.T.print(OS, SQT.Policy, SQT.PlaceHolder, SQT.Indentation);
return OS;
}
};

StreamedQualTypeHelper stream(const PrintingPolicy &Policy,
const Twine &PlaceHolder = Twine(),
unsigned Indentation = 0) const {
return StreamedQualTypeHelper(*this, Policy, PlaceHolder, Indentation);
}

void dump(const char *s) const;
void dump() const;
void dump(llvm::raw_ostream &OS, const ASTContext &Context) const;

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.AddPointer(getAsOpaquePtr());
}


inline bool hasAddressSpace() const;


inline LangAS getAddressSpace() const;









bool isAddressSpaceOverlapping(QualType T) const {
Qualifiers Q = getQualifiers();
Qualifiers TQ = T.getQualifiers();

return Q.isAddressSpaceSupersetOf(TQ) || TQ.isAddressSpaceSupersetOf(Q);
}


inline Qualifiers::GC getObjCGCAttr() const;


bool isObjCGCWeak() const {
return getObjCGCAttr() == Qualifiers::Weak;
}


bool isObjCGCStrong() const {
return getObjCGCAttr() == Qualifiers::Strong;
}


Qualifiers::ObjCLifetime getObjCLifetime() const {
return getQualifiers().getObjCLifetime();
}

bool hasNonTrivialObjCLifetime() const {
return getQualifiers().hasNonTrivialObjCLifetime();
}

bool hasStrongOrWeakObjCLifetime() const {
return getQualifiers().hasStrongOrWeakObjCLifetime();
}


bool isNonWeakInMRRWithObjCWeak(const ASTContext &Context) const;

enum PrimitiveDefaultInitializeKind {



PDIK_Trivial,



PDIK_ARCStrong,



PDIK_ARCWeak,


PDIK_Struct
};






PrimitiveDefaultInitializeKind
isNonTrivialToPrimitiveDefaultInitialize() const;

enum PrimitiveCopyKind {



PCK_Trivial,




PCK_VolatileTrivial,



PCK_ARCStrong,



PCK_ARCWeak,







PCK_Struct
};




PrimitiveCopyKind isNonTrivialToPrimitiveCopy() const;







PrimitiveCopyKind isNonTrivialToPrimitiveDestructiveMove() const;

enum DestructionKind {
DK_none,
DK_cxx_destructor,
DK_objc_strong_lifetime,
DK_objc_weak_lifetime,
DK_nontrivial_c_struct
};





DestructionKind isDestructedType() const {
return isDestructedTypeImpl(*this);
}





bool hasNonTrivialToPrimitiveDefaultInitializeCUnion() const;




bool hasNonTrivialToPrimitiveDestructCUnion() const;




bool hasNonTrivialToPrimitiveCopyCUnion() const;











bool isCForbiddenLValueType() const;















QualType substObjCTypeArgs(ASTContext &ctx,
ArrayRef<QualType> typeArgs,
ObjCSubstitutionContext context) const;





















QualType substObjCMemberType(QualType objectType,
const DeclContext *dc,
ObjCSubstitutionContext context) const;


QualType stripObjCKindOfType(const ASTContext &ctx) const;


QualType getAtomicUnqualifiedType() const;

private:



static bool isConstant(QualType T, const ASTContext& Ctx);
static QualType getDesugaredType(QualType T, const ASTContext &Context);
static SplitQualType getSplitDesugaredType(QualType T);
static SplitQualType getSplitUnqualifiedTypeImpl(QualType type);
static QualType getSingleStepDesugaredTypeImpl(QualType type,
const ASTContext &C);
static QualType IgnoreParens(QualType T);
static DestructionKind isDestructedTypeImpl(QualType type);


static bool hasNonTrivialToPrimitiveDefaultInitializeCUnion(const RecordDecl *RD);
static bool hasNonTrivialToPrimitiveDestructCUnion(const RecordDecl *RD);
static bool hasNonTrivialToPrimitiveCopyCUnion(const RecordDecl *RD);
};

}

namespace llvm {



template<> struct simplify_type< ::clang::QualType> {
using SimpleType = const ::clang::Type *;

static SimpleType getSimplifiedValue(::clang::QualType Val) {
return Val.getTypePtr();
}
};


template<>
struct PointerLikeTypeTraits<clang::QualType> {
static inline void *getAsVoidPointer(clang::QualType P) {
return P.getAsOpaquePtr();
}

static inline clang::QualType getFromVoidPointer(void *P) {
return clang::QualType::getFromOpaquePtr(P);
}


static constexpr int NumLowBitsAvailable = 0;
};

}

namespace clang {




class ExtQualsTypeCommonBase {
friend class ExtQuals;
friend class QualType;
friend class Type;






const Type *const BaseType;


QualType CanonicalType;

ExtQualsTypeCommonBase(const Type *baseType, QualType canon)
: BaseType(baseType), CanonicalType(canon) {}
};











class ExtQuals : public ExtQualsTypeCommonBase, public llvm::FoldingSetNode {















Qualifiers Quals;

ExtQuals *this_() { return this; }

public:
ExtQuals(const Type *baseType, QualType canon, Qualifiers quals)
: ExtQualsTypeCommonBase(baseType,
canon.isNull() ? QualType(this_(), 0) : canon),
Quals(quals) {
assert(Quals.hasNonFastQualifiers()
&& "ExtQuals created with no fast qualifiers");
assert(!Quals.hasFastQualifiers()
&& "ExtQuals created with fast qualifiers");
}

Qualifiers getQualifiers() const { return Quals; }

bool hasObjCGCAttr() const { return Quals.hasObjCGCAttr(); }
Qualifiers::GC getObjCGCAttr() const { return Quals.getObjCGCAttr(); }

bool hasObjCLifetime() const { return Quals.hasObjCLifetime(); }
Qualifiers::ObjCLifetime getObjCLifetime() const {
return Quals.getObjCLifetime();
}

bool hasAddressSpace() const { return Quals.hasAddressSpace(); }
LangAS getAddressSpace() const { return Quals.getAddressSpace(); }

const Type *getBaseType() const { return BaseType; }

public:
void Profile(llvm::FoldingSetNodeID &ID) const {
Profile(ID, getBaseType(), Quals);
}

static void Profile(llvm::FoldingSetNodeID &ID,
const Type *BaseType,
Qualifiers Quals) {
assert(!Quals.hasFastQualifiers() && "fast qualifiers in ExtQuals hash!");
ID.AddPointer(BaseType);
Quals.Profile(ID);
}
};




enum RefQualifierKind {

RQ_None = 0,


RQ_LValue,


RQ_RValue
};


enum class AutoTypeKeyword {

Auto,


DecltypeAuto,


GNUAutoType
};



























class alignas(8) Type : public ExtQualsTypeCommonBase {
public:
enum TypeClass {
#define TYPE(Class, Base) Class,
#define LAST_TYPE(Class) TypeLast = Class
#define ABSTRACT_TYPE(Class, Base)
#include "clang/AST/TypeNodes.inc"
};

private:

class TypeBitfields {
friend class Type;
template <class T> friend class TypePropertyCache;


unsigned TC : 8;


unsigned Dependence : llvm::BitWidth<TypeDependence>;



mutable unsigned CacheValid : 1;


mutable unsigned CachedLinkage : 3;


mutable unsigned CachedLocalOrUnnamed : 1;


mutable unsigned FromAST : 1;

bool isCacheValid() const {
return CacheValid;
}

Linkage getLinkage() const {
assert(isCacheValid() && "getting linkage from invalid cache");
return static_cast<Linkage>(CachedLinkage);
}

bool hasLocalOrUnnamedType() const {
assert(isCacheValid() && "getting linkage from invalid cache");
return CachedLocalOrUnnamed;
}
};
enum { NumTypeBits = 8 + llvm::BitWidth<TypeDependence> + 6 };

protected:



class ArrayTypeBitfields {
friend class ArrayType;

unsigned : NumTypeBits;



unsigned IndexTypeQuals : 3;




unsigned SizeModifier : 3;
};

class ConstantArrayTypeBitfields {
friend class ConstantArrayType;

unsigned : NumTypeBits + 3 + 3;


unsigned HasStoredSizeExpr : 1;
};

class BuiltinTypeBitfields {
friend class BuiltinType;

unsigned : NumTypeBits;


unsigned Kind : 8;
};




class FunctionTypeBitfields {
friend class FunctionProtoType;
friend class FunctionType;

unsigned : NumTypeBits;



unsigned ExtInfo : 13;




unsigned RefQualifier : 2;







unsigned FastTypeQuals : Qualifiers::FastWidth;

unsigned HasExtQuals : 1;





unsigned NumParams : 16;


unsigned ExceptionSpecType : 4;


unsigned HasExtParameterInfos : 1;


unsigned Variadic : 1;


unsigned HasTrailingReturn : 1;
};

class ObjCObjectTypeBitfields {
friend class ObjCObjectType;

unsigned : NumTypeBits;


unsigned NumTypeArgs : 7;


unsigned NumProtocols : 6;


unsigned IsKindOf : 1;
};

class ReferenceTypeBitfields {
friend class ReferenceType;

unsigned : NumTypeBits;












unsigned SpelledAsLValue : 1;



unsigned InnerRef : 1;
};

class TypeWithKeywordBitfields {
friend class TypeWithKeyword;

unsigned : NumTypeBits;


unsigned Keyword : 8;
};

enum { NumTypeWithKeywordBits = 8 };

class ElaboratedTypeBitfields {
friend class ElaboratedType;

unsigned : NumTypeBits;
unsigned : NumTypeWithKeywordBits;


unsigned HasOwnedTagDecl : 1;
};

class VectorTypeBitfields {
friend class VectorType;
friend class DependentVectorType;

unsigned : NumTypeBits;



unsigned VecKind : 3;

uint32_t NumElements;
};

class AttributedTypeBitfields {
friend class AttributedType;

unsigned : NumTypeBits;


unsigned AttrKind : 32 - NumTypeBits;
};

class AutoTypeBitfields {
friend class AutoType;

unsigned : NumTypeBits;



unsigned Keyword : 2;








unsigned NumArgs;
};

class SubstTemplateTypeParmPackTypeBitfields {
friend class SubstTemplateTypeParmPackType;

unsigned : NumTypeBits;








unsigned NumArgs;
};

class TemplateSpecializationTypeBitfields {
friend class TemplateSpecializationType;

unsigned : NumTypeBits;


unsigned TypeAlias : 1;








unsigned NumArgs;
};

class DependentTemplateSpecializationTypeBitfields {
friend class DependentTemplateSpecializationType;

unsigned : NumTypeBits;
unsigned : NumTypeWithKeywordBits;








unsigned NumArgs;
};

class PackExpansionTypeBitfields {
friend class PackExpansionType;

unsigned : NumTypeBits;












unsigned NumExpansions;
};

union {
TypeBitfields TypeBits;
ArrayTypeBitfields ArrayTypeBits;
ConstantArrayTypeBitfields ConstantArrayTypeBits;
AttributedTypeBitfields AttributedTypeBits;
AutoTypeBitfields AutoTypeBits;
BuiltinTypeBitfields BuiltinTypeBits;
FunctionTypeBitfields FunctionTypeBits;
ObjCObjectTypeBitfields ObjCObjectTypeBits;
ReferenceTypeBitfields ReferenceTypeBits;
TypeWithKeywordBitfields TypeWithKeywordBits;
ElaboratedTypeBitfields ElaboratedTypeBits;
VectorTypeBitfields VectorTypeBits;
SubstTemplateTypeParmPackTypeBitfields SubstTemplateTypeParmPackTypeBits;
TemplateSpecializationTypeBitfields TemplateSpecializationTypeBits;
DependentTemplateSpecializationTypeBitfields
DependentTemplateSpecializationTypeBits;
PackExpansionTypeBitfields PackExpansionTypeBits;
};

private:
template <class T> friend class TypePropertyCache;


void setFromAST(bool V = true) const {
TypeBits.FromAST = V;
}

protected:
friend class ASTContext;

Type(TypeClass tc, QualType canon, TypeDependence Dependence)
: ExtQualsTypeCommonBase(this,
canon.isNull() ? QualType(this_(), 0) : canon) {
static_assert(sizeof(*this) <= 8 + sizeof(ExtQualsTypeCommonBase),
"changing bitfields changed sizeof(Type)!");
static_assert(alignof(decltype(*this)) % sizeof(void *) == 0,
"Insufficient alignment!");
TypeBits.TC = tc;
TypeBits.Dependence = static_cast<unsigned>(Dependence);
TypeBits.CacheValid = false;
TypeBits.CachedLocalOrUnnamed = false;
TypeBits.CachedLinkage = NoLinkage;
TypeBits.FromAST = false;
}


Type *this_() { return this; }

void setDependence(TypeDependence D) {
TypeBits.Dependence = static_cast<unsigned>(D);
}

void addDependence(TypeDependence D) { setDependence(getDependence() | D); }

public:
friend class ASTReader;
friend class ASTWriter;
template <class T> friend class serialization::AbstractTypeReader;
template <class T> friend class serialization::AbstractTypeWriter;

Type(const Type &) = delete;
Type(Type &&) = delete;
Type &operator=(const Type &) = delete;
Type &operator=(Type &&) = delete;

TypeClass getTypeClass() const { return static_cast<TypeClass>(TypeBits.TC); }


bool isFromAST() const { return TypeBits.FromAST; }
















bool containsUnexpandedParameterPack() const {
return getDependence() & TypeDependence::UnexpandedPack;
}



bool isCanonicalUnqualified() const {
return CanonicalType == QualType(this, 0);
}




QualType getLocallyUnqualifiedSingleStepDesugaredType() const;







bool isSizelessType() const;
bool isSizelessBuiltinType() const;




bool isVLSTBuiltinType() const;




QualType getSveEltType(const ASTContext &Ctx) const;












bool isIncompleteType(NamedDecl **Def = nullptr) const;



bool isIncompleteOrObjectType() const {
return !isFunctionType();
}


bool isObjectType() const {



return !isReferenceType() && !isFunctionType() && !isVoidType();
}



bool isLiteralType(const ASTContext &Ctx) const;


bool isStructuralType() const;



bool isStandardLayoutType() const;





bool isBuiltinType() const;


bool isSpecificBuiltinType(unsigned K) const;




bool isPlaceholderType() const;
const BuiltinType *getAsPlaceholderType() const;


bool isSpecificPlaceholderType(unsigned K) const;



bool isNonOverloadPlaceholderType() const;



bool isIntegerType() const;
bool isEnumeralType() const;


bool isScopedEnumeralType() const;
bool isBooleanType() const;
bool isCharType() const;
bool isWideCharType() const;
bool isChar8Type() const;
bool isChar16Type() const;
bool isChar32Type() const;
bool isAnyCharacterType() const;
bool isIntegralType(const ASTContext &Ctx) const;


bool isIntegralOrEnumerationType() const;


bool isIntegralOrUnscopedEnumerationType() const;
bool isUnscopedEnumerationType() const;


bool isRealFloatingType() const;


bool isComplexType() const;
bool isAnyComplexType() const;
bool isFloatingType() const;
bool isHalfType() const;
bool isFloat16Type() const;
bool isBFloat16Type() const;
bool isFloat128Type() const;
bool isRealType() const;
bool isArithmeticType() const;
bool isVoidType() const;
bool isScalarType() const;
bool isAggregateType() const;
bool isFundamentalType() const;
bool isCompoundType() const;



bool isFunctionType() const;
bool isFunctionNoProtoType() const { return getAs<FunctionNoProtoType>(); }
bool isFunctionProtoType() const { return getAs<FunctionProtoType>(); }
bool isPointerType() const;
bool isAnyPointerType() const;
bool isBlockPointerType() const;
bool isVoidPointerType() const;
bool isReferenceType() const;
bool isLValueReferenceType() const;
bool isRValueReferenceType() const;
bool isObjectPointerType() const;
bool isFunctionPointerType() const;
bool isFunctionReferenceType() const;
bool isMemberPointerType() const;
bool isMemberFunctionPointerType() const;
bool isMemberDataPointerType() const;
bool isArrayType() const;
bool isConstantArrayType() const;
bool isIncompleteArrayType() const;
bool isVariableArrayType() const;
bool isDependentSizedArrayType() const;
bool isRecordType() const;
bool isClassType() const;
bool isStructureType() const;
bool isObjCBoxableRecordType() const;
bool isInterfaceType() const;
bool isStructureOrClassType() const;
bool isUnionType() const;
bool isComplexIntegerType() const;
bool isVectorType() const;
bool isExtVectorType() const;
bool isMatrixType() const;
bool isConstantMatrixType() const;
bool isDependentAddressSpaceType() const;
bool isObjCObjectPointerType() const;
bool isObjCRetainableType() const;
bool isObjCLifetimeType() const;
bool isObjCIndirectLifetimeType() const;
bool isObjCNSObjectType() const;
bool isObjCIndependentClassType() const;


bool isObjCObjectType() const;
bool isObjCQualifiedInterfaceType() const;
bool isObjCQualifiedIdType() const;
bool isObjCQualifiedClassType() const;
bool isObjCObjectOrInterfaceType() const;
bool isObjCIdType() const;
bool isDecltypeType() const;






bool isObjCInertUnsafeUnretainedType() const {
return hasAttr(attr::ObjCInertUnsafeUnretained);
}








bool isObjCIdOrObjectKindOfType(const ASTContext &ctx,
const ObjCObjectType *&bound) const;

bool isObjCClassType() const;







bool isObjCClassOrClassKindOfType() const;

bool isBlockCompatibleObjCPointerType(ASTContext &ctx) const;
bool isObjCSelType() const;
bool isObjCBuiltinType() const;
bool isObjCARCBridgableType() const;
bool isCARCBridgableType() const;
bool isTemplateTypeParmType() const;
bool isNullPtrType() const;
bool isNothrowT() const;
bool isAlignValT() const;
bool isStdByteType() const;
bool isAtomicType() const;
bool isUndeducedAutoType() const;

bool isTypedefNameType() const;

#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) bool is##Id##Type() const;

#include "clang/Basic/OpenCLImageTypes.def"

bool isImageType() const;

bool isSamplerT() const;
bool isEventT() const;
bool isClkEventT() const;
bool isQueueT() const;
bool isReserveIDT() const;

#define EXT_OPAQUE_TYPE(ExtType, Id, Ext) bool is##Id##Type() const;

#include "clang/Basic/OpenCLExtensionTypes.def"

bool isOCLIntelSubgroupAVCType() const;
bool isOCLExtOpaqueType() const;

bool isPipeType() const;
bool isExtIntType() const;
bool isOpenCLSpecificType() const;




bool isObjCARCImplicitlyUnretainedType() const;


bool isCUDADeviceBuiltinSurfaceType() const;

bool isCUDADeviceBuiltinTextureType() const;


Qualifiers::ObjCLifetime getObjCARCImplicitLifetime() const;

enum ScalarTypeKind {
STK_CPointer,
STK_BlockPointer,
STK_ObjCObjectPointer,
STK_MemberPointer,
STK_Bool,
STK_Integral,
STK_Floating,
STK_IntegralComplex,
STK_FloatingComplex,
STK_FixedPoint
};


ScalarTypeKind getScalarTypeKind() const;

TypeDependence getDependence() const {
return static_cast<TypeDependence>(TypeBits.Dependence);
}


bool containsErrors() const {
return getDependence() & TypeDependence::Error;
}



bool isDependentType() const {
return getDependence() & TypeDependence::Dependent;
}





bool isInstantiationDependentType() const {
return getDependence() & TypeDependence::Instantiation;
}




bool isUndeducedType() const;


bool isVariablyModifiedType() const {
return getDependence() & TypeDependence::VariablyModified;
}



bool hasSizedVLAType() const;


bool hasUnnamedOrLocalType() const;

bool isOverloadableType() const;


bool isElaboratedTypeSpecifier() const;

bool canDecayToPointerType() const;




bool hasPointerRepresentation() const;



bool hasObjCPointerRepresentation() const;



bool hasIntegerRepresentation() const;



bool hasSignedIntegerRepresentation() const;



bool hasUnsignedIntegerRepresentation() const;



bool hasFloatingRepresentation() const;




const RecordType *getAsStructureType() const;

const RecordType *getAsUnionType() const;
const ComplexType *getAsComplexIntegerType() const;
const ObjCObjectType *getAsObjCInterfaceType() const;



const ObjCObjectPointerType *getAsObjCInterfacePointerType() const;
const ObjCObjectPointerType *getAsObjCQualifiedIdType() const;
const ObjCObjectPointerType *getAsObjCQualifiedClassType() const;
const ObjCObjectType *getAsObjCQualifiedInterfaceType() const;




CXXRecordDecl *getAsCXXRecordDecl() const;


RecordDecl *getAsRecordDecl() const;




TagDecl *getAsTagDecl() const;






const CXXRecordDecl *getPointeeCXXRecordDecl() const;




DeducedType *getContainedDeducedType() const;




AutoType *getContainedAutoType() const {
return dyn_cast_or_null<AutoType>(getContainedDeducedType());
}




bool hasAutoForTrailingReturnType() const;







template <typename T> const T *getAs() const;






template <typename T> const T *getAsAdjusted() const;



const ArrayType *getAsArrayTypeUnsafe() const;







template <typename T> const T *castAs() const;



const ArrayType *castAsArrayTypeUnsafe() const;



bool hasAttr(attr::Kind AK) const;




const Type *getBaseElementTypeUnsafe() const;




const Type *getArrayElementTypeNoTypeQual() const;




const Type *getPointeeOrArrayElementType() const;



QualType getPointeeType() const;



const Type *getUnqualifiedDesugaredType() const;


bool isPromotableIntegerType() const;




bool isSignedIntegerType() const;




bool isUnsignedIntegerType() const;



bool isSignedIntegerOrEnumerationType() const;



bool isUnsignedIntegerOrEnumerationType() const;



bool isFixedPointType() const;


bool isFixedPointOrIntegerType() const;



bool isSaturatedFixedPointType() const;



bool isUnsaturatedFixedPointType() const;



bool isSignedFixedPointType() const;



bool isUnsignedFixedPointType() const;




bool isConstantSizeType() const;



bool isSpecifierType() const;


Linkage getLinkage() const;


Visibility getVisibility() const {
return getLinkageAndVisibility().getVisibility();
}


bool isVisibilityExplicit() const {
return getLinkageAndVisibility().isVisibilityExplicit();
}


LinkageInfo getLinkageAndVisibility() const;



bool isLinkageValid() const;






Optional<NullabilityKind> getNullability(const ASTContext &context) const;






bool canHaveNullability(bool ResultIfUnknown = true) const;
















Optional<ArrayRef<QualType>>
getObjCSubstitutions(const DeclContext *dc) const;



bool acceptsObjCTypeParams() const;

const char *getTypeClassName() const;

QualType getCanonicalTypeInternal() const {
return CanonicalType;
}

CanQualType getCanonicalTypeUnqualified() const;
void dump() const;
void dump(llvm::raw_ostream &OS, const ASTContext &Context) const;
};



template <> const TypedefType *Type::getAs() const;




template <> const TemplateSpecializationType *Type::getAs() const;



template <> const AttributedType *Type::getAs() const;



#define TYPE(Class, Base)
#define LEAF_TYPE(Class) template <> inline const Class##Type *Type::getAs() const { return dyn_cast<Class##Type>(CanonicalType); } template <> inline const Class##Type *Type::castAs() const { return cast<Class##Type>(CanonicalType); }






#include "clang/AST/TypeNodes.inc"



class BuiltinType : public Type {
public:
enum Kind {

#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) Id,
#include "clang/Basic/OpenCLImageTypes.def"

#define EXT_OPAQUE_TYPE(ExtType, Id, Ext) Id,
#include "clang/Basic/OpenCLExtensionTypes.def"

#define SVE_TYPE(Name, Id, SingletonId) Id,
#include "clang/Basic/AArch64SVEACLETypes.def"

#define PPC_VECTOR_TYPE(Name, Id, Size) Id,
#include "clang/Basic/PPCTypes.def"

#define RVV_TYPE(Name, Id, SingletonId) Id,
#include "clang/Basic/RISCVVTypes.def"

#define BUILTIN_TYPE(Id, SingletonId) Id,
#define LAST_BUILTIN_TYPE(Id) LastKind = Id
#include "clang/AST/BuiltinTypes.def"
};

private:
friend class ASTContext;

BuiltinType(Kind K)
: Type(Builtin, QualType(),
K == Dependent ? TypeDependence::DependentInstantiation
: TypeDependence::None) {
BuiltinTypeBits.Kind = K;
}

public:
Kind getKind() const { return static_cast<Kind>(BuiltinTypeBits.Kind); }
StringRef getName(const PrintingPolicy &Policy) const;

const char *getNameAsCString(const PrintingPolicy &Policy) const {

StringRef str = getName(Policy);
assert(!str.empty() && str.data()[str.size()] == '\0');
return str.data();
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

bool isInteger() const {
return getKind() >= Bool && getKind() <= Int128;
}

bool isSignedInteger() const {
return getKind() >= Char_S && getKind() <= Int128;
}

bool isUnsignedInteger() const {
return getKind() >= Bool && getKind() <= UInt128;
}

bool isFloatingPoint() const {
return getKind() >= Half && getKind() <= Float128;
}


static bool isPlaceholderTypeKind(Kind K) {
return K >= Overload;
}




bool isPlaceholderType() const {
return isPlaceholderTypeKind(getKind());
}










bool isNonOverloadPlaceholderType() const {
return getKind() > Overload;
}

static bool classof(const Type *T) { return T->getTypeClass() == Builtin; }
};



class ComplexType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

QualType ElementType;

ComplexType(QualType Element, QualType CanonicalPtr)
: Type(Complex, CanonicalPtr, Element->getDependence()),
ElementType(Element) {}

public:
QualType getElementType() const { return ElementType; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getElementType());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType Element) {
ID.AddPointer(Element.getAsOpaquePtr());
}

static bool classof(const Type *T) { return T->getTypeClass() == Complex; }
};


class ParenType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

QualType Inner;

ParenType(QualType InnerType, QualType CanonType)
: Type(Paren, CanonType, InnerType->getDependence()), Inner(InnerType) {}

public:
QualType getInnerType() const { return Inner; }

bool isSugared() const { return true; }
QualType desugar() const { return getInnerType(); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getInnerType());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType Inner) {
Inner.Profile(ID);
}

static bool classof(const Type *T) { return T->getTypeClass() == Paren; }
};


class PointerType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

QualType PointeeType;

PointerType(QualType Pointee, QualType CanonicalPtr)
: Type(Pointer, CanonicalPtr, Pointee->getDependence()),
PointeeType(Pointee) {}

public:
QualType getPointeeType() const { return PointeeType; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getPointeeType());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType Pointee) {
ID.AddPointer(Pointee.getAsOpaquePtr());
}

static bool classof(const Type *T) { return T->getTypeClass() == Pointer; }
};




class AdjustedType : public Type, public llvm::FoldingSetNode {
QualType OriginalTy;
QualType AdjustedTy;

protected:
friend class ASTContext;

AdjustedType(TypeClass TC, QualType OriginalTy, QualType AdjustedTy,
QualType CanonicalPtr)
: Type(TC, CanonicalPtr, OriginalTy->getDependence()),
OriginalTy(OriginalTy), AdjustedTy(AdjustedTy) {}

public:
QualType getOriginalType() const { return OriginalTy; }
QualType getAdjustedType() const { return AdjustedTy; }

bool isSugared() const { return true; }
QualType desugar() const { return AdjustedTy; }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, OriginalTy, AdjustedTy);
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType Orig, QualType New) {
ID.AddPointer(Orig.getAsOpaquePtr());
ID.AddPointer(New.getAsOpaquePtr());
}

static bool classof(const Type *T) {
return T->getTypeClass() == Adjusted || T->getTypeClass() == Decayed;
}
};


class DecayedType : public AdjustedType {
friend class ASTContext;

inline
DecayedType(QualType OriginalType, QualType Decayed, QualType Canonical);

public:
QualType getDecayedType() const { return getAdjustedType(); }

inline QualType getPointeeType() const;

static bool classof(const Type *T) { return T->getTypeClass() == Decayed; }
};




class BlockPointerType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;


QualType PointeeType;

BlockPointerType(QualType Pointee, QualType CanonicalCls)
: Type(BlockPointer, CanonicalCls, Pointee->getDependence()),
PointeeType(Pointee) {}

public:

QualType getPointeeType() const { return PointeeType; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getPointeeType());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType Pointee) {
ID.AddPointer(Pointee.getAsOpaquePtr());
}

static bool classof(const Type *T) {
return T->getTypeClass() == BlockPointer;
}
};


class ReferenceType : public Type, public llvm::FoldingSetNode {
QualType PointeeType;

protected:
ReferenceType(TypeClass tc, QualType Referencee, QualType CanonicalRef,
bool SpelledAsLValue)
: Type(tc, CanonicalRef, Referencee->getDependence()),
PointeeType(Referencee) {
ReferenceTypeBits.SpelledAsLValue = SpelledAsLValue;
ReferenceTypeBits.InnerRef = Referencee->isReferenceType();
}

public:
bool isSpelledAsLValue() const { return ReferenceTypeBits.SpelledAsLValue; }
bool isInnerRef() const { return ReferenceTypeBits.InnerRef; }

QualType getPointeeTypeAsWritten() const { return PointeeType; }

QualType getPointeeType() const {

const ReferenceType *T = this;
while (T->isInnerRef())
T = T->PointeeType->castAs<ReferenceType>();
return T->PointeeType;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, PointeeType, isSpelledAsLValue());
}

static void Profile(llvm::FoldingSetNodeID &ID,
QualType Referencee,
bool SpelledAsLValue) {
ID.AddPointer(Referencee.getAsOpaquePtr());
ID.AddBoolean(SpelledAsLValue);
}

static bool classof(const Type *T) {
return T->getTypeClass() == LValueReference ||
T->getTypeClass() == RValueReference;
}
};


class LValueReferenceType : public ReferenceType {
friend class ASTContext;

LValueReferenceType(QualType Referencee, QualType CanonicalRef,
bool SpelledAsLValue)
: ReferenceType(LValueReference, Referencee, CanonicalRef,
SpelledAsLValue) {}

public:
bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == LValueReference;
}
};


class RValueReferenceType : public ReferenceType {
friend class ASTContext;

RValueReferenceType(QualType Referencee, QualType CanonicalRef)
: ReferenceType(RValueReference, Referencee, CanonicalRef, false) {}

public:
bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == RValueReference;
}
};




class MemberPointerType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

QualType PointeeType;



const Type *Class;

MemberPointerType(QualType Pointee, const Type *Cls, QualType CanonicalPtr)
: Type(MemberPointer, CanonicalPtr,
(Cls->getDependence() & ~TypeDependence::VariablyModified) |
Pointee->getDependence()),
PointeeType(Pointee), Class(Cls) {}

public:
QualType getPointeeType() const { return PointeeType; }



bool isMemberFunctionPointer() const {
return PointeeType->isFunctionProtoType();
}



bool isMemberDataPointer() const {
return !PointeeType->isFunctionProtoType();
}

const Type *getClass() const { return Class; }
CXXRecordDecl *getMostRecentCXXRecordDecl() const;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getPointeeType(), getClass());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType Pointee,
const Type *Class) {
ID.AddPointer(Pointee.getAsOpaquePtr());
ID.AddPointer(Class);
}

static bool classof(const Type *T) {
return T->getTypeClass() == MemberPointer;
}
};


class ArrayType : public Type, public llvm::FoldingSetNode {
public:




enum ArraySizeModifier {
Normal, Static, Star
};

private:

QualType ElementType;

protected:
friend class ASTContext;

ArrayType(TypeClass tc, QualType et, QualType can, ArraySizeModifier sm,
unsigned tq, const Expr *sz = nullptr);

public:
QualType getElementType() const { return ElementType; }

ArraySizeModifier getSizeModifier() const {
return ArraySizeModifier(ArrayTypeBits.SizeModifier);
}

Qualifiers getIndexTypeQualifiers() const {
return Qualifiers::fromCVRMask(getIndexTypeCVRQualifiers());
}

unsigned getIndexTypeCVRQualifiers() const {
return ArrayTypeBits.IndexTypeQuals;
}

static bool classof(const Type *T) {
return T->getTypeClass() == ConstantArray ||
T->getTypeClass() == VariableArray ||
T->getTypeClass() == IncompleteArray ||
T->getTypeClass() == DependentSizedArray;
}
};




class ConstantArrayType final
: public ArrayType,
private llvm::TrailingObjects<ConstantArrayType, const Expr *> {
friend class ASTContext;
friend TrailingObjects;

llvm::APInt Size;

ConstantArrayType(QualType et, QualType can, const llvm::APInt &size,
const Expr *sz, ArraySizeModifier sm, unsigned tq)
: ArrayType(ConstantArray, et, can, sm, tq, sz), Size(size) {
ConstantArrayTypeBits.HasStoredSizeExpr = sz != nullptr;
if (ConstantArrayTypeBits.HasStoredSizeExpr) {
assert(!can.isNull() && "canonical constant array should not have size");
*getTrailingObjects<const Expr*>() = sz;
}
}

unsigned numTrailingObjects(OverloadToken<const Expr*>) const {
return ConstantArrayTypeBits.HasStoredSizeExpr;
}

public:
const llvm::APInt &getSize() const { return Size; }
const Expr *getSizeExpr() const {
return ConstantArrayTypeBits.HasStoredSizeExpr
? *getTrailingObjects<const Expr *>()
: nullptr;
}
bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }



static unsigned getNumAddressingBits(const ASTContext &Context,
QualType ElementType,
const llvm::APInt &NumElements);



static unsigned getMaxSizeBits(const ASTContext &Context);

void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Ctx) {
Profile(ID, Ctx, getElementType(), getSize(), getSizeExpr(),
getSizeModifier(), getIndexTypeCVRQualifiers());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Ctx,
QualType ET, const llvm::APInt &ArraySize,
const Expr *SizeExpr, ArraySizeModifier SizeMod,
unsigned TypeQuals);

static bool classof(const Type *T) {
return T->getTypeClass() == ConstantArray;
}
};




class IncompleteArrayType : public ArrayType {
friend class ASTContext;

IncompleteArrayType(QualType et, QualType can,
ArraySizeModifier sm, unsigned tq)
: ArrayType(IncompleteArray, et, can, sm, tq) {}

public:
friend class StmtIteratorBase;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == IncompleteArray;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getElementType(), getSizeModifier(),
getIndexTypeCVRQualifiers());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType ET,
ArraySizeModifier SizeMod, unsigned TypeQuals) {
ID.AddPointer(ET.getAsOpaquePtr());
ID.AddInteger(SizeMod);
ID.AddInteger(TypeQuals);
}
};















class VariableArrayType : public ArrayType {
friend class ASTContext;



Stmt *SizeExpr;


SourceRange Brackets;

VariableArrayType(QualType et, QualType can, Expr *e,
ArraySizeModifier sm, unsigned tq,
SourceRange brackets)
: ArrayType(VariableArray, et, can, sm, tq, e),
SizeExpr((Stmt*) e), Brackets(brackets) {}

public:
friend class StmtIteratorBase;

Expr *getSizeExpr() const {


return (Expr*) SizeExpr;
}

SourceRange getBracketsRange() const { return Brackets; }
SourceLocation getLBracketLoc() const { return Brackets.getBegin(); }
SourceLocation getRBracketLoc() const { return Brackets.getEnd(); }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == VariableArray;
}

void Profile(llvm::FoldingSetNodeID &ID) {
llvm_unreachable("Cannot unique VariableArrayTypes.");
}
};














class DependentSizedArrayType : public ArrayType {
friend class ASTContext;

const ASTContext &Context;






Stmt *SizeExpr;


SourceRange Brackets;

DependentSizedArrayType(const ASTContext &Context, QualType et, QualType can,
Expr *e, ArraySizeModifier sm, unsigned tq,
SourceRange brackets);

public:
friend class StmtIteratorBase;

Expr *getSizeExpr() const {


return (Expr*) SizeExpr;
}

SourceRange getBracketsRange() const { return Brackets; }
SourceLocation getLBracketLoc() const { return Brackets.getBegin(); }
SourceLocation getRBracketLoc() const { return Brackets.getEnd(); }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == DependentSizedArray;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, getElementType(),
getSizeModifier(), getIndexTypeCVRQualifiers(), getSizeExpr());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
QualType ET, ArraySizeModifier SizeMod,
unsigned TypeQuals, Expr *E);
};












class DependentAddressSpaceType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

const ASTContext &Context;
Expr *AddrSpaceExpr;
QualType PointeeType;
SourceLocation loc;

DependentAddressSpaceType(const ASTContext &Context, QualType PointeeType,
QualType can, Expr *AddrSpaceExpr,
SourceLocation loc);

public:
Expr *getAddrSpaceExpr() const { return AddrSpaceExpr; }
QualType getPointeeType() const { return PointeeType; }
SourceLocation getAttributeLoc() const { return loc; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == DependentAddressSpace;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, getPointeeType(), getAddrSpaceExpr());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
QualType PointeeType, Expr *AddrSpaceExpr);
};











class DependentSizedExtVectorType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

const ASTContext &Context;
Expr *SizeExpr;


QualType ElementType;

SourceLocation loc;

DependentSizedExtVectorType(const ASTContext &Context, QualType ElementType,
QualType can, Expr *SizeExpr, SourceLocation loc);

public:
Expr *getSizeExpr() const { return SizeExpr; }
QualType getElementType() const { return ElementType; }
SourceLocation getAttributeLoc() const { return loc; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == DependentSizedExtVector;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, getElementType(), getSizeExpr());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
QualType ElementType, Expr *SizeExpr);
};







class VectorType : public Type, public llvm::FoldingSetNode {
public:
enum VectorKind {

GenericVector,


AltiVecVector,


AltiVecPixel,


AltiVecBool,


NeonVector,


NeonPolyVector,


SveFixedLengthDataVector,


SveFixedLengthPredicateVector
};

protected:
friend class ASTContext;


QualType ElementType;

VectorType(QualType vecType, unsigned nElements, QualType canonType,
VectorKind vecKind);

VectorType(TypeClass tc, QualType vecType, unsigned nElements,
QualType canonType, VectorKind vecKind);

public:
QualType getElementType() const { return ElementType; }
unsigned getNumElements() const { return VectorTypeBits.NumElements; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

VectorKind getVectorKind() const {
return VectorKind(VectorTypeBits.VecKind);
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getElementType(), getNumElements(),
getTypeClass(), getVectorKind());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType ElementType,
unsigned NumElements, TypeClass TypeClass,
VectorKind VecKind) {
ID.AddPointer(ElementType.getAsOpaquePtr());
ID.AddInteger(NumElements);
ID.AddInteger(TypeClass);
ID.AddInteger(VecKind);
}

static bool classof(const Type *T) {
return T->getTypeClass() == Vector || T->getTypeClass() == ExtVector;
}
};










class DependentVectorType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

const ASTContext &Context;
QualType ElementType;
Expr *SizeExpr;
SourceLocation Loc;

DependentVectorType(const ASTContext &Context, QualType ElementType,
QualType CanonType, Expr *SizeExpr,
SourceLocation Loc, VectorType::VectorKind vecKind);

public:
Expr *getSizeExpr() const { return SizeExpr; }
QualType getElementType() const { return ElementType; }
SourceLocation getAttributeLoc() const { return Loc; }
VectorType::VectorKind getVectorKind() const {
return VectorType::VectorKind(VectorTypeBits.VecKind);
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == DependentVector;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, getElementType(), getSizeExpr(), getVectorKind());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
QualType ElementType, const Expr *SizeExpr,
VectorType::VectorKind VecKind);
};







class ExtVectorType : public VectorType {
friend class ASTContext;

ExtVectorType(QualType vecType, unsigned nElements, QualType canonType)
: VectorType(ExtVector, vecType, nElements, canonType, GenericVector) {}

public:
static int getPointAccessorIdx(char c) {
switch (c) {
default: return -1;
case 'x': case 'r': return 0;
case 'y': case 'g': return 1;
case 'z': case 'b': return 2;
case 'w': case 'a': return 3;
}
}

static int getNumericAccessorIdx(char c) {
switch (c) {
default: return -1;
case '0': return 0;
case '1': return 1;
case '2': return 2;
case '3': return 3;
case '4': return 4;
case '5': return 5;
case '6': return 6;
case '7': return 7;
case '8': return 8;
case '9': return 9;
case 'A':
case 'a': return 10;
case 'B':
case 'b': return 11;
case 'C':
case 'c': return 12;
case 'D':
case 'd': return 13;
case 'E':
case 'e': return 14;
case 'F':
case 'f': return 15;
}
}

static int getAccessorIdx(char c, bool isNumericAccessor) {
if (isNumericAccessor)
return getNumericAccessorIdx(c);
else
return getPointAccessorIdx(c);
}

bool isAccessorWithinNumElements(char c, bool isNumericAccessor) const {
if (int idx = getAccessorIdx(c, isNumericAccessor)+1)
return unsigned(idx-1) < getNumElements();
return false;
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == ExtVector;
}
};




class MatrixType : public Type, public llvm::FoldingSetNode {
protected:
friend class ASTContext;


QualType ElementType;

MatrixType(QualType ElementTy, QualType CanonElementTy);

MatrixType(TypeClass TypeClass, QualType ElementTy, QualType CanonElementTy,
const Expr *RowExpr = nullptr, const Expr *ColumnExpr = nullptr);

public:

QualType getElementType() const { return ElementType; }






static bool isValidElementType(QualType T) {
return T->isDependentType() ||
(T->isRealType() && !T->isBooleanType() && !T->isEnumeralType());
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == ConstantMatrix ||
T->getTypeClass() == DependentSizedMatrix;
}
};


class ConstantMatrixType final : public MatrixType {
protected:
friend class ASTContext;



QualType ElementType;


unsigned NumRows;
unsigned NumColumns;

static constexpr unsigned MaxElementsPerDimension = (1 << 20) - 1;

ConstantMatrixType(QualType MatrixElementType, unsigned NRows,
unsigned NColumns, QualType CanonElementType);

ConstantMatrixType(TypeClass typeClass, QualType MatrixType, unsigned NRows,
unsigned NColumns, QualType CanonElementType);

public:

unsigned getNumRows() const { return NumRows; }


unsigned getNumColumns() const { return NumColumns; }


unsigned getNumElementsFlattened() const {
return getNumRows() * getNumColumns();
}


static constexpr bool isDimensionValid(size_t NumElements) {
return NumElements > 0 && NumElements <= MaxElementsPerDimension;
}


static constexpr unsigned getMaxElementsPerDimension() {
return MaxElementsPerDimension;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getElementType(), getNumRows(), getNumColumns(),
getTypeClass());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType ElementType,
unsigned NumRows, unsigned NumColumns,
TypeClass TypeClass) {
ID.AddPointer(ElementType.getAsOpaquePtr());
ID.AddInteger(NumRows);
ID.AddInteger(NumColumns);
ID.AddInteger(TypeClass);
}

static bool classof(const Type *T) {
return T->getTypeClass() == ConstantMatrix;
}
};



class DependentSizedMatrixType final : public MatrixType {
friend class ASTContext;

const ASTContext &Context;
Expr *RowExpr;
Expr *ColumnExpr;

SourceLocation loc;

DependentSizedMatrixType(const ASTContext &Context, QualType ElementType,
QualType CanonicalType, Expr *RowExpr,
Expr *ColumnExpr, SourceLocation loc);

public:
QualType getElementType() const { return ElementType; }
Expr *getRowExpr() const { return RowExpr; }
Expr *getColumnExpr() const { return ColumnExpr; }
SourceLocation getAttributeLoc() const { return loc; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == DependentSizedMatrix;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, getElementType(), getRowExpr(), getColumnExpr());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
QualType ElementType, Expr *RowExpr, Expr *ColumnExpr);
};



class FunctionType : public Type {

QualType ResultType;

public:





















class ExtParameterInfo {
enum {
ABIMask = 0x0F,
IsConsumed = 0x10,
HasPassObjSize = 0x20,
IsNoEscape = 0x40,
};
unsigned char Data = 0;

public:
ExtParameterInfo() = default;


ParameterABI getABI() const { return ParameterABI(Data & ABIMask); }
ExtParameterInfo withABI(ParameterABI kind) const {
ExtParameterInfo copy = *this;
copy.Data = (copy.Data & ~ABIMask) | unsigned(kind);
return copy;
}



bool isConsumed() const { return (Data & IsConsumed); }
ExtParameterInfo withIsConsumed(bool consumed) const {
ExtParameterInfo copy = *this;
if (consumed)
copy.Data |= IsConsumed;
else
copy.Data &= ~IsConsumed;
return copy;
}

bool hasPassObjectSize() const { return Data & HasPassObjSize; }
ExtParameterInfo withHasPassObjectSize() const {
ExtParameterInfo Copy = *this;
Copy.Data |= HasPassObjSize;
return Copy;
}

bool isNoEscape() const { return Data & IsNoEscape; }
ExtParameterInfo withIsNoEscape(bool NoEscape) const {
ExtParameterInfo Copy = *this;
if (NoEscape)
Copy.Data |= IsNoEscape;
else
Copy.Data &= ~IsNoEscape;
return Copy;
}

unsigned char getOpaqueValue() const { return Data; }
static ExtParameterInfo getFromOpaqueValue(unsigned char data) {
ExtParameterInfo result;
result.Data = data;
return result;
}

friend bool operator==(ExtParameterInfo lhs, ExtParameterInfo rhs) {
return lhs.Data == rhs.Data;
}

friend bool operator!=(ExtParameterInfo lhs, ExtParameterInfo rhs) {
return lhs.Data != rhs.Data;
}
};





















class ExtInfo {
friend class FunctionType;









enum { CallConvMask = 0x1F };
enum { NoReturnMask = 0x20 };
enum { ProducesResultMask = 0x40 };
enum { NoCallerSavedRegsMask = 0x80 };
enum {
RegParmMask = 0x700,
RegParmOffset = 8
};
enum { NoCfCheckMask = 0x800 };
enum { CmseNSCallMask = 0x1000 };
uint16_t Bits = CC_C;

ExtInfo(unsigned Bits) : Bits(static_cast<uint16_t>(Bits)) {}

public:


ExtInfo(bool noReturn, bool hasRegParm, unsigned regParm, CallingConv cc,
bool producesResult, bool noCallerSavedRegs, bool NoCfCheck,
bool cmseNSCall) {
assert((!hasRegParm || regParm < 7) && "Invalid regparm value");
Bits = ((unsigned)cc) | (noReturn ? NoReturnMask : 0) |
(producesResult ? ProducesResultMask : 0) |
(noCallerSavedRegs ? NoCallerSavedRegsMask : 0) |
(hasRegParm ? ((regParm + 1) << RegParmOffset) : 0) |
(NoCfCheck ? NoCfCheckMask : 0) |
(cmseNSCall ? CmseNSCallMask : 0);
}



ExtInfo() = default;



ExtInfo(CallingConv CC) : Bits(CC) {}

bool getNoReturn() const { return Bits & NoReturnMask; }
bool getProducesResult() const { return Bits & ProducesResultMask; }
bool getCmseNSCall() const { return Bits & CmseNSCallMask; }
bool getNoCallerSavedRegs() const { return Bits & NoCallerSavedRegsMask; }
bool getNoCfCheck() const { return Bits & NoCfCheckMask; }
bool getHasRegParm() const { return ((Bits & RegParmMask) >> RegParmOffset) != 0; }

unsigned getRegParm() const {
unsigned RegParm = (Bits & RegParmMask) >> RegParmOffset;
if (RegParm > 0)
--RegParm;
return RegParm;
}

CallingConv getCC() const { return CallingConv(Bits & CallConvMask); }

bool operator==(ExtInfo Other) const {
return Bits == Other.Bits;
}
bool operator!=(ExtInfo Other) const {
return Bits != Other.Bits;
}




ExtInfo withNoReturn(bool noReturn) const {
if (noReturn)
return ExtInfo(Bits | NoReturnMask);
else
return ExtInfo(Bits & ~NoReturnMask);
}

ExtInfo withProducesResult(bool producesResult) const {
if (producesResult)
return ExtInfo(Bits | ProducesResultMask);
else
return ExtInfo(Bits & ~ProducesResultMask);
}

ExtInfo withCmseNSCall(bool cmseNSCall) const {
if (cmseNSCall)
return ExtInfo(Bits | CmseNSCallMask);
else
return ExtInfo(Bits & ~CmseNSCallMask);
}

ExtInfo withNoCallerSavedRegs(bool noCallerSavedRegs) const {
if (noCallerSavedRegs)
return ExtInfo(Bits | NoCallerSavedRegsMask);
else
return ExtInfo(Bits & ~NoCallerSavedRegsMask);
}

ExtInfo withNoCfCheck(bool noCfCheck) const {
if (noCfCheck)
return ExtInfo(Bits | NoCfCheckMask);
else
return ExtInfo(Bits & ~NoCfCheckMask);
}

ExtInfo withRegParm(unsigned RegParm) const {
assert(RegParm < 7 && "Invalid regparm value");
return ExtInfo((Bits & ~RegParmMask) |
((RegParm + 1) << RegParmOffset));
}

ExtInfo withCallingConv(CallingConv cc) const {
return ExtInfo((Bits & ~CallConvMask) | (unsigned) cc);
}

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.AddInteger(Bits);
}
};




struct ExceptionType { QualType Type; };





struct alignas(void *) FunctionTypeExtraBitfields {



unsigned NumExceptionType;
};

protected:
FunctionType(TypeClass tc, QualType res, QualType Canonical,
TypeDependence Dependence, ExtInfo Info)
: Type(tc, Canonical, Dependence), ResultType(res) {
FunctionTypeBits.ExtInfo = Info.Bits;
}

Qualifiers getFastTypeQuals() const {
return Qualifiers::fromFastMask(FunctionTypeBits.FastTypeQuals);
}

public:
QualType getReturnType() const { return ResultType; }

bool getHasRegParm() const { return getExtInfo().getHasRegParm(); }
unsigned getRegParmType() const { return getExtInfo().getRegParm(); }




bool getNoReturnAttr() const { return getExtInfo().getNoReturn(); }

bool getCmseNSCallAttr() const { return getExtInfo().getCmseNSCall(); }
CallingConv getCallConv() const { return getExtInfo().getCC(); }
ExtInfo getExtInfo() const { return ExtInfo(FunctionTypeBits.ExtInfo); }

static_assert((~Qualifiers::FastMask & Qualifiers::CVRMask) == 0,
"Const, volatile and restrict are assumed to be a subset of "
"the fast qualifiers.");

bool isConst() const { return getFastTypeQuals().hasConst(); }
bool isVolatile() const { return getFastTypeQuals().hasVolatile(); }
bool isRestrict() const { return getFastTypeQuals().hasRestrict(); }



QualType getCallResultType(const ASTContext &Context) const {
return getReturnType().getNonLValueExprType(Context);
}

static StringRef getNameForCallConv(CallingConv CC);

static bool classof(const Type *T) {
return T->getTypeClass() == FunctionNoProto ||
T->getTypeClass() == FunctionProto;
}
};



class FunctionNoProtoType : public FunctionType, public llvm::FoldingSetNode {
friend class ASTContext;

FunctionNoProtoType(QualType Result, QualType Canonical, ExtInfo Info)
: FunctionType(FunctionNoProto, Result, Canonical,
Result->getDependence() &
~(TypeDependence::DependentInstantiation |
TypeDependence::UnexpandedPack),
Info) {}

public:


bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getReturnType(), getExtInfo());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType ResultType,
ExtInfo Info) {
Info.Profile(ID);
ID.AddPointer(ResultType.getAsOpaquePtr());
}

static bool classof(const Type *T) {
return T->getTypeClass() == FunctionNoProto;
}
};








class FunctionProtoType final
: public FunctionType,
public llvm::FoldingSetNode,
private llvm::TrailingObjects<
FunctionProtoType, QualType, SourceLocation,
FunctionType::FunctionTypeExtraBitfields, FunctionType::ExceptionType,
Expr *, FunctionDecl *, FunctionType::ExtParameterInfo, Qualifiers> {
friend class ASTContext;
friend TrailingObjects;











































public:




struct ExceptionSpecInfo {

ExceptionSpecificationType Type = EST_None;


ArrayRef<QualType> Exceptions;


Expr *NoexceptExpr = nullptr;



FunctionDecl *SourceDecl = nullptr;



FunctionDecl *SourceTemplate = nullptr;

ExceptionSpecInfo() = default;

ExceptionSpecInfo(ExceptionSpecificationType EST) : Type(EST) {}
};




struct ExtProtoInfo {
FunctionType::ExtInfo ExtInfo;
bool Variadic : 1;
bool HasTrailingReturn : 1;
Qualifiers TypeQuals;
RefQualifierKind RefQualifier = RQ_None;
ExceptionSpecInfo ExceptionSpec;
const ExtParameterInfo *ExtParameterInfos = nullptr;
SourceLocation EllipsisLoc;

ExtProtoInfo() : Variadic(false), HasTrailingReturn(false) {}

ExtProtoInfo(CallingConv CC)
: ExtInfo(CC), Variadic(false), HasTrailingReturn(false) {}

ExtProtoInfo withExceptionSpec(const ExceptionSpecInfo &ESI) {
ExtProtoInfo Result(*this);
Result.ExceptionSpec = ESI;
return Result;
}
};

private:
unsigned numTrailingObjects(OverloadToken<QualType>) const {
return getNumParams();
}

unsigned numTrailingObjects(OverloadToken<SourceLocation>) const {
return isVariadic();
}

unsigned numTrailingObjects(OverloadToken<FunctionTypeExtraBitfields>) const {
return hasExtraBitfields();
}

unsigned numTrailingObjects(OverloadToken<ExceptionType>) const {
return getExceptionSpecSize().NumExceptionType;
}

unsigned numTrailingObjects(OverloadToken<Expr *>) const {
return getExceptionSpecSize().NumExprPtr;
}

unsigned numTrailingObjects(OverloadToken<FunctionDecl *>) const {
return getExceptionSpecSize().NumFunctionDeclPtr;
}

unsigned numTrailingObjects(OverloadToken<ExtParameterInfo>) const {
return hasExtParameterInfos() ? getNumParams() : 0;
}



static bool containsAnyUnexpandedParameterPack(const QualType *ArgArray,
unsigned numArgs) {
for (unsigned Idx = 0; Idx < numArgs; ++Idx)
if (ArgArray[Idx]->containsUnexpandedParameterPack())
return true;

return false;
}

FunctionProtoType(QualType result, ArrayRef<QualType> params,
QualType canonical, const ExtProtoInfo &epi);




struct ExceptionSpecSizeHolder {
unsigned NumExceptionType;
unsigned NumExprPtr;
unsigned NumFunctionDeclPtr;
};



static ExceptionSpecSizeHolder
getExceptionSpecSize(ExceptionSpecificationType EST, unsigned NumExceptions) {
switch (EST) {
case EST_None:
case EST_DynamicNone:
case EST_MSAny:
case EST_BasicNoexcept:
case EST_Unparsed:
case EST_NoThrow:
return {0, 0, 0};

case EST_Dynamic:
return {NumExceptions, 0, 0};

case EST_DependentNoexcept:
case EST_NoexceptFalse:
case EST_NoexceptTrue:
return {0, 1, 0};

case EST_Uninstantiated:
return {0, 0, 2};

case EST_Unevaluated:
return {0, 0, 1};
}
llvm_unreachable("bad exception specification kind");
}



ExceptionSpecSizeHolder getExceptionSpecSize() const {
return getExceptionSpecSize(getExceptionSpecType(), getNumExceptions());
}


static bool hasExtraBitfields(ExceptionSpecificationType EST) {


return EST == EST_Dynamic;
}


bool hasExtraBitfields() const {
return hasExtraBitfields(getExceptionSpecType());
}

bool hasExtQualifiers() const {
return FunctionTypeBits.HasExtQuals;
}

public:
unsigned getNumParams() const { return FunctionTypeBits.NumParams; }

QualType getParamType(unsigned i) const {
assert(i < getNumParams() && "invalid parameter index");
return param_type_begin()[i];
}

ArrayRef<QualType> getParamTypes() const {
return llvm::makeArrayRef(param_type_begin(), param_type_end());
}

ExtProtoInfo getExtProtoInfo() const {
ExtProtoInfo EPI;
EPI.ExtInfo = getExtInfo();
EPI.Variadic = isVariadic();
EPI.EllipsisLoc = getEllipsisLoc();
EPI.HasTrailingReturn = hasTrailingReturn();
EPI.ExceptionSpec = getExceptionSpecInfo();
EPI.TypeQuals = getMethodQuals();
EPI.RefQualifier = getRefQualifier();
EPI.ExtParameterInfos = getExtParameterInfosOrNull();
return EPI;
}


ExceptionSpecificationType getExceptionSpecType() const {
return static_cast<ExceptionSpecificationType>(
FunctionTypeBits.ExceptionSpecType);
}


bool hasExceptionSpec() const { return getExceptionSpecType() != EST_None; }


bool hasDynamicExceptionSpec() const {
return isDynamicExceptionSpec(getExceptionSpecType());
}


bool hasNoexceptExceptionSpec() const {
return isNoexceptExceptionSpec(getExceptionSpecType());
}


bool hasDependentExceptionSpec() const;



bool hasInstantiationDependentExceptionSpec() const;


ExceptionSpecInfo getExceptionSpecInfo() const {
ExceptionSpecInfo Result;
Result.Type = getExceptionSpecType();
if (Result.Type == EST_Dynamic) {
Result.Exceptions = exceptions();
} else if (isComputedNoexcept(Result.Type)) {
Result.NoexceptExpr = getNoexceptExpr();
} else if (Result.Type == EST_Uninstantiated) {
Result.SourceDecl = getExceptionSpecDecl();
Result.SourceTemplate = getExceptionSpecTemplate();
} else if (Result.Type == EST_Unevaluated) {
Result.SourceDecl = getExceptionSpecDecl();
}
return Result;
}


unsigned getNumExceptions() const {
return getExceptionSpecType() == EST_Dynamic
? getTrailingObjects<FunctionTypeExtraBitfields>()
->NumExceptionType
: 0;
}


QualType getExceptionType(unsigned i) const {
assert(i < getNumExceptions() && "Invalid exception number!");
return exception_begin()[i];
}



Expr *getNoexceptExpr() const {
if (!isComputedNoexcept(getExceptionSpecType()))
return nullptr;
return *getTrailingObjects<Expr *>();
}





FunctionDecl *getExceptionSpecDecl() const {
if (getExceptionSpecType() != EST_Uninstantiated &&
getExceptionSpecType() != EST_Unevaluated)
return nullptr;
return getTrailingObjects<FunctionDecl *>()[0];
}





FunctionDecl *getExceptionSpecTemplate() const {
if (getExceptionSpecType() != EST_Uninstantiated)
return nullptr;
return getTrailingObjects<FunctionDecl *>()[1];
}



CanThrowResult canThrow() const;




bool isNothrow(bool ResultIfDependent = false) const {
return ResultIfDependent ? canThrow() != CT_Can : canThrow() == CT_Cannot;
}


bool isVariadic() const { return FunctionTypeBits.Variadic; }

SourceLocation getEllipsisLoc() const {
return isVariadic() ? *getTrailingObjects<SourceLocation>()
: SourceLocation();
}







bool isTemplateVariadic() const;


bool hasTrailingReturn() const { return FunctionTypeBits.HasTrailingReturn; }

Qualifiers getMethodQuals() const {
if (hasExtQualifiers())
return *getTrailingObjects<Qualifiers>();
else
return getFastTypeQuals();
}


RefQualifierKind getRefQualifier() const {
return static_cast<RefQualifierKind>(FunctionTypeBits.RefQualifier);
}

using param_type_iterator = const QualType *;
using param_type_range = llvm::iterator_range<param_type_iterator>;

param_type_range param_types() const {
return param_type_range(param_type_begin(), param_type_end());
}

param_type_iterator param_type_begin() const {
return getTrailingObjects<QualType>();
}

param_type_iterator param_type_end() const {
return param_type_begin() + getNumParams();
}

using exception_iterator = const QualType *;

ArrayRef<QualType> exceptions() const {
return llvm::makeArrayRef(exception_begin(), exception_end());
}

exception_iterator exception_begin() const {
return reinterpret_cast<exception_iterator>(
getTrailingObjects<ExceptionType>());
}

exception_iterator exception_end() const {
return exception_begin() + getNumExceptions();
}



bool hasExtParameterInfos() const {
return FunctionTypeBits.HasExtParameterInfos;
}

ArrayRef<ExtParameterInfo> getExtParameterInfos() const {
assert(hasExtParameterInfos());
return ArrayRef<ExtParameterInfo>(getTrailingObjects<ExtParameterInfo>(),
getNumParams());
}




const ExtParameterInfo *getExtParameterInfosOrNull() const {
if (!hasExtParameterInfos())
return nullptr;
return getTrailingObjects<ExtParameterInfo>();
}

ExtParameterInfo getExtParameterInfo(unsigned I) const {
assert(I < getNumParams() && "parameter index out of range");
if (hasExtParameterInfos())
return getTrailingObjects<ExtParameterInfo>()[I];
return ExtParameterInfo();
}

ParameterABI getParameterABI(unsigned I) const {
assert(I < getNumParams() && "parameter index out of range");
if (hasExtParameterInfos())
return getTrailingObjects<ExtParameterInfo>()[I].getABI();
return ParameterABI::Ordinary;
}

bool isParamConsumed(unsigned I) const {
assert(I < getNumParams() && "parameter index out of range");
if (hasExtParameterInfos())
return getTrailingObjects<ExtParameterInfo>()[I].isConsumed();
return false;
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void printExceptionSpecification(raw_ostream &OS,
const PrintingPolicy &Policy) const;

static bool classof(const Type *T) {
return T->getTypeClass() == FunctionProto;
}

void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Ctx);
static void Profile(llvm::FoldingSetNodeID &ID, QualType Result,
param_type_iterator ArgTys, unsigned NumArgs,
const ExtProtoInfo &EPI, const ASTContext &Context,
bool Canonical);
};






class UnresolvedUsingType : public Type {
friend class ASTContext;

UnresolvedUsingTypenameDecl *Decl;

UnresolvedUsingType(const UnresolvedUsingTypenameDecl *D)
: Type(UnresolvedUsing, QualType(),
TypeDependence::DependentInstantiation),
Decl(const_cast<UnresolvedUsingTypenameDecl *>(D)) {}

public:
UnresolvedUsingTypenameDecl *getDecl() const { return Decl; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == UnresolvedUsing;
}

void Profile(llvm::FoldingSetNodeID &ID) {
return Profile(ID, Decl);
}

static void Profile(llvm::FoldingSetNodeID &ID,
UnresolvedUsingTypenameDecl *D) {
ID.AddPointer(D);
}
};

class TypedefType : public Type {
TypedefNameDecl *Decl;

private:
friend class ASTContext;

TypedefType(TypeClass tc, const TypedefNameDecl *D, QualType underlying,
QualType can);

public:
TypedefNameDecl *getDecl() const { return Decl; }

bool isSugared() const { return true; }
QualType desugar() const;

static bool classof(const Type *T) { return T->getTypeClass() == Typedef; }
};



class MacroQualifiedType : public Type {
friend class ASTContext;

QualType UnderlyingTy;
const IdentifierInfo *MacroII;

MacroQualifiedType(QualType UnderlyingTy, QualType CanonTy,
const IdentifierInfo *MacroII)
: Type(MacroQualified, CanonTy, UnderlyingTy->getDependence()),
UnderlyingTy(UnderlyingTy), MacroII(MacroII) {
assert(isa<AttributedType>(UnderlyingTy) &&
"Expected a macro qualified type to only wrap attributed types.");
}

public:
const IdentifierInfo *getMacroIdentifier() const { return MacroII; }
QualType getUnderlyingType() const { return UnderlyingTy; }



QualType getModifiedType() const;

bool isSugared() const { return true; }
QualType desugar() const;

static bool classof(const Type *T) {
return T->getTypeClass() == MacroQualified;
}
};


class TypeOfExprType : public Type {
Expr *TOExpr;

protected:
friend class ASTContext;

TypeOfExprType(Expr *E, QualType can = QualType());

public:
Expr *getUnderlyingExpr() const { return TOExpr; }


QualType desugar() const;


bool isSugared() const;

static bool classof(const Type *T) { return T->getTypeClass() == TypeOfExpr; }
};







class DependentTypeOfExprType
: public TypeOfExprType, public llvm::FoldingSetNode {
const ASTContext &Context;

public:
DependentTypeOfExprType(const ASTContext &Context, Expr *E)
: TypeOfExprType(E), Context(Context) {}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, getUnderlyingExpr());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
Expr *E);
};


class TypeOfType : public Type {
friend class ASTContext;

QualType TOType;

TypeOfType(QualType T, QualType can)
: Type(TypeOf, can, T->getDependence()), TOType(T) {
assert(!isa<TypedefType>(can) && "Invalid canonical type");
}

public:
QualType getUnderlyingType() const { return TOType; }


QualType desugar() const { return getUnderlyingType(); }


bool isSugared() const { return true; }

static bool classof(const Type *T) { return T->getTypeClass() == TypeOf; }
};


class DecltypeType : public Type {
Expr *E;
QualType UnderlyingType;

protected:
friend class ASTContext;

DecltypeType(Expr *E, QualType underlyingType, QualType can = QualType());

public:
Expr *getUnderlyingExpr() const { return E; }
QualType getUnderlyingType() const { return UnderlyingType; }


QualType desugar() const;


bool isSugared() const;

static bool classof(const Type *T) { return T->getTypeClass() == Decltype; }
};







class DependentDecltypeType : public DecltypeType, public llvm::FoldingSetNode {
const ASTContext &Context;

public:
DependentDecltypeType(const ASTContext &Context, Expr *E);

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, getUnderlyingExpr());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
Expr *E);
};


class UnaryTransformType : public Type {
public:
enum UTTKind {
EnumUnderlyingType
};

private:

QualType BaseType;


QualType UnderlyingType;

UTTKind UKind;

protected:
friend class ASTContext;

UnaryTransformType(QualType BaseTy, QualType UnderlyingTy, UTTKind UKind,
QualType CanonicalTy);

public:
bool isSugared() const { return !isDependentType(); }
QualType desugar() const { return UnderlyingType; }

QualType getUnderlyingType() const { return UnderlyingType; }
QualType getBaseType() const { return BaseType; }

UTTKind getUTTKind() const { return UKind; }

static bool classof(const Type *T) {
return T->getTypeClass() == UnaryTransform;
}
};







class DependentUnaryTransformType : public UnaryTransformType,
public llvm::FoldingSetNode {
public:
DependentUnaryTransformType(const ASTContext &C, QualType BaseType,
UTTKind UKind);

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getBaseType(), getUTTKind());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType BaseType,
UTTKind UKind) {
ID.AddPointer(BaseType.getAsOpaquePtr());
ID.AddInteger((unsigned)UKind);
}
};

class TagType : public Type {
friend class ASTReader;
template <class T> friend class serialization::AbstractTypeReader;



TagDecl *decl;

protected:
TagType(TypeClass TC, const TagDecl *D, QualType can);

public:
TagDecl *getDecl() const;


bool isBeingDefined() const;

static bool classof(const Type *T) {
return T->getTypeClass() == Enum || T->getTypeClass() == Record;
}
};



class RecordType : public TagType {
protected:
friend class ASTContext;

explicit RecordType(const RecordDecl *D)
: TagType(Record, reinterpret_cast<const TagDecl*>(D), QualType()) {}
explicit RecordType(TypeClass TC, RecordDecl *D)
: TagType(TC, reinterpret_cast<const TagDecl*>(D), QualType()) {}

public:
RecordDecl *getDecl() const {
return reinterpret_cast<RecordDecl*>(TagType::getDecl());
}



bool hasConstFields() const;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) { return T->getTypeClass() == Record; }
};



class EnumType : public TagType {
friend class ASTContext;

explicit EnumType(const EnumDecl *D)
: TagType(Enum, reinterpret_cast<const TagDecl*>(D), QualType()) {}

public:
EnumDecl *getDecl() const {
return reinterpret_cast<EnumDecl*>(TagType::getDecl());
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) { return T->getTypeClass() == Enum; }
};













class AttributedType : public Type, public llvm::FoldingSetNode {
public:
using Kind = attr::Kind;

private:
friend class ASTContext;

QualType ModifiedType;
QualType EquivalentType;

AttributedType(QualType canon, attr::Kind attrKind, QualType modified,
QualType equivalent)
: Type(Attributed, canon, equivalent->getDependence()),
ModifiedType(modified), EquivalentType(equivalent) {
AttributedTypeBits.AttrKind = attrKind;
}

public:
Kind getAttrKind() const {
return static_cast<Kind>(AttributedTypeBits.AttrKind);
}

QualType getModifiedType() const { return ModifiedType; }
QualType getEquivalentType() const { return EquivalentType; }

bool isSugared() const { return true; }
QualType desugar() const { return getEquivalentType(); }
















bool isQualifier() const;

bool isMSTypeSpec() const;

bool isCallingConv() const;

llvm::Optional<NullabilityKind> getImmediateNullability() const;



static Kind getNullabilityAttrKind(NullabilityKind kind) {
switch (kind) {
case NullabilityKind::NonNull:
return attr::TypeNonNull;

case NullabilityKind::Nullable:
return attr::TypeNullable;

case NullabilityKind::NullableResult:
return attr::TypeNullableResult;

case NullabilityKind::Unspecified:
return attr::TypeNullUnspecified;
}
llvm_unreachable("Unknown nullability kind.");
}










static Optional<NullabilityKind> stripOuterNullability(QualType &T);

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getAttrKind(), ModifiedType, EquivalentType);
}

static void Profile(llvm::FoldingSetNodeID &ID, Kind attrKind,
QualType modified, QualType equivalent) {
ID.AddInteger(attrKind);
ID.AddPointer(modified.getAsOpaquePtr());
ID.AddPointer(equivalent.getAsOpaquePtr());
}

static bool classof(const Type *T) {
return T->getTypeClass() == Attributed;
}
};

class TemplateTypeParmType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;


struct CanonicalTTPTInfo {
unsigned Depth : 15;
unsigned ParameterPack : 1;
unsigned Index : 16;
};

union {

CanonicalTTPTInfo CanTTPTInfo;


TemplateTypeParmDecl *TTPDecl;
};


TemplateTypeParmType(TemplateTypeParmDecl *TTPDecl, QualType Canon)
: Type(TemplateTypeParm, Canon,
TypeDependence::DependentInstantiation |
(Canon->getDependence() & TypeDependence::UnexpandedPack)),
TTPDecl(TTPDecl) {}


TemplateTypeParmType(unsigned D, unsigned I, bool PP)
: Type(TemplateTypeParm, QualType(this, 0),
TypeDependence::DependentInstantiation |
(PP ? TypeDependence::UnexpandedPack : TypeDependence::None)) {
CanTTPTInfo.Depth = D;
CanTTPTInfo.Index = I;
CanTTPTInfo.ParameterPack = PP;
}

const CanonicalTTPTInfo& getCanTTPTInfo() const {
QualType Can = getCanonicalTypeInternal();
return Can->castAs<TemplateTypeParmType>()->CanTTPTInfo;
}

public:
unsigned getDepth() const { return getCanTTPTInfo().Depth; }
unsigned getIndex() const { return getCanTTPTInfo().Index; }
bool isParameterPack() const { return getCanTTPTInfo().ParameterPack; }

TemplateTypeParmDecl *getDecl() const {
return isCanonicalUnqualified() ? nullptr : TTPDecl;
}

IdentifierInfo *getIdentifier() const;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getDepth(), getIndex(), isParameterPack(), getDecl());
}

static void Profile(llvm::FoldingSetNodeID &ID, unsigned Depth,
unsigned Index, bool ParameterPack,
TemplateTypeParmDecl *TTPDecl) {
ID.AddInteger(Depth);
ID.AddInteger(Index);
ID.AddBoolean(ParameterPack);
ID.AddPointer(TTPDecl);
}

static bool classof(const Type *T) {
return T->getTypeClass() == TemplateTypeParm;
}
};








class SubstTemplateTypeParmType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;


const TemplateTypeParmType *Replaced;

SubstTemplateTypeParmType(const TemplateTypeParmType *Param, QualType Canon)
: Type(SubstTemplateTypeParm, Canon, Canon->getDependence()),
Replaced(Param) {}

public:

const TemplateTypeParmType *getReplacedParameter() const {
return Replaced;
}



QualType getReplacementType() const {
return getCanonicalTypeInternal();
}

bool isSugared() const { return true; }
QualType desugar() const { return getReplacementType(); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getReplacedParameter(), getReplacementType());
}

static void Profile(llvm::FoldingSetNodeID &ID,
const TemplateTypeParmType *Replaced,
QualType Replacement) {
ID.AddPointer(Replaced);
ID.AddPointer(Replacement.getAsOpaquePtr());
}

static bool classof(const Type *T) {
return T->getTypeClass() == SubstTemplateTypeParm;
}
};













class SubstTemplateTypeParmPackType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;


const TemplateTypeParmType *Replaced;



const TemplateArgument *Arguments;

SubstTemplateTypeParmPackType(const TemplateTypeParmType *Param,
QualType Canon,
const TemplateArgument &ArgPack);

public:
IdentifierInfo *getIdentifier() const { return Replaced->getIdentifier(); }


const TemplateTypeParmType *getReplacedParameter() const {
return Replaced;
}

unsigned getNumArgs() const {
return SubstTemplateTypeParmPackTypeBits.NumArgs;
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

TemplateArgument getArgumentPack() const;

void Profile(llvm::FoldingSetNodeID &ID);
static void Profile(llvm::FoldingSetNodeID &ID,
const TemplateTypeParmType *Replaced,
const TemplateArgument &ArgPack);

static bool classof(const Type *T) {
return T->getTypeClass() == SubstTemplateTypeParmPack;
}
};









class DeducedType : public Type {
protected:
DeducedType(TypeClass TC, QualType DeducedAsType,
TypeDependence ExtraDependence)
: Type(TC,

DeducedAsType.isNull() ? QualType(this, 0)
: DeducedAsType.getCanonicalType(),
ExtraDependence | (DeducedAsType.isNull()
? TypeDependence::None
: DeducedAsType->getDependence() &
~TypeDependence::VariablyModified)) {}

public:
bool isSugared() const { return !isCanonicalUnqualified(); }
QualType desugar() const { return getCanonicalTypeInternal(); }



QualType getDeducedType() const {
return !isCanonicalUnqualified() ? getCanonicalTypeInternal() : QualType();
}
bool isDeduced() const {
return !isCanonicalUnqualified() || isDependentType();
}

static bool classof(const Type *T) {
return T->getTypeClass() == Auto ||
T->getTypeClass() == DeducedTemplateSpecialization;
}
};



class alignas(8) AutoType : public DeducedType, public llvm::FoldingSetNode {
friend class ASTContext;

ConceptDecl *TypeConstraintConcept;

AutoType(QualType DeducedAsType, AutoTypeKeyword Keyword,
TypeDependence ExtraDependence, ConceptDecl *CD,
ArrayRef<TemplateArgument> TypeConstraintArgs);

const TemplateArgument *getArgBuffer() const {
return reinterpret_cast<const TemplateArgument*>(this+1);
}

TemplateArgument *getArgBuffer() {
return reinterpret_cast<TemplateArgument*>(this+1);
}

public:

const TemplateArgument *getArgs() const {
return getArgBuffer();
}


unsigned getNumArgs() const {
return AutoTypeBits.NumArgs;
}

const TemplateArgument &getArg(unsigned Idx) const;

ArrayRef<TemplateArgument> getTypeConstraintArguments() const {
return {getArgs(), getNumArgs()};
}

ConceptDecl *getTypeConstraintConcept() const {
return TypeConstraintConcept;
}

bool isConstrained() const {
return TypeConstraintConcept != nullptr;
}

bool isDecltypeAuto() const {
return getKeyword() == AutoTypeKeyword::DecltypeAuto;
}

AutoTypeKeyword getKeyword() const {
return (AutoTypeKeyword)AutoTypeBits.Keyword;
}

void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context) {
Profile(ID, Context, getDeducedType(), getKeyword(), isDependentType(),
getTypeConstraintConcept(), getTypeConstraintArguments());
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
QualType Deduced, AutoTypeKeyword Keyword,
bool IsDependent, ConceptDecl *CD,
ArrayRef<TemplateArgument> Arguments);

static bool classof(const Type *T) {
return T->getTypeClass() == Auto;
}
};


class DeducedTemplateSpecializationType : public DeducedType,
public llvm::FoldingSetNode {
friend class ASTContext;


TemplateName Template;

DeducedTemplateSpecializationType(TemplateName Template,
QualType DeducedAsType,
bool IsDeducedAsDependent)
: DeducedType(DeducedTemplateSpecialization, DeducedAsType,
toTypeDependence(Template.getDependence()) |
(IsDeducedAsDependent
? TypeDependence::DependentInstantiation
: TypeDependence::None)),
Template(Template) {}

public:

TemplateName getTemplateName() const { return Template;}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getTemplateName(), getDeducedType(), isDependentType());
}

static void Profile(llvm::FoldingSetNodeID &ID, TemplateName Template,
QualType Deduced, bool IsDependent) {
Template.Profile(ID);
ID.AddPointer(Deduced.getAsOpaquePtr());
ID.AddBoolean(IsDependent);
}

static bool classof(const Type *T) {
return T->getTypeClass() == DeducedTemplateSpecialization;
}
};





















class alignas(8) TemplateSpecializationType
: public Type,
public llvm::FoldingSetNode {
friend class ASTContext;








TemplateName Template;

TemplateSpecializationType(TemplateName T,
ArrayRef<TemplateArgument> Args,
QualType Canon,
QualType Aliased);

public:











static bool
anyDependentTemplateArguments(ArrayRef<TemplateArgumentLoc> Args,
ArrayRef<TemplateArgument> Converted);
static bool
anyDependentTemplateArguments(const TemplateArgumentListInfo &,
ArrayRef<TemplateArgument> Converted);
static bool anyInstantiationDependentTemplateArguments(
ArrayRef<TemplateArgumentLoc> Args);



bool isCurrentInstantiation() const {
return isa<InjectedClassNameType>(getCanonicalTypeInternal());
}
















bool isTypeAlias() const { return TemplateSpecializationTypeBits.TypeAlias; }



QualType getAliasedType() const {
assert(isTypeAlias() && "not a type alias template specialization");
return *reinterpret_cast<const QualType*>(end());
}

using iterator = const TemplateArgument *;

iterator begin() const { return getArgs(); }
iterator end() const;


TemplateName getTemplateName() const { return Template; }


const TemplateArgument *getArgs() const {
return reinterpret_cast<const TemplateArgument *>(this + 1);
}


unsigned getNumArgs() const {
return TemplateSpecializationTypeBits.NumArgs;
}



const TemplateArgument &getArg(unsigned Idx) const;

ArrayRef<TemplateArgument> template_arguments() const {
return {getArgs(), getNumArgs()};
}

bool isSugared() const {
return !isDependentType() || isCurrentInstantiation() || isTypeAlias();
}

QualType desugar() const {
return isTypeAlias() ? getAliasedType() : getCanonicalTypeInternal();
}

void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Ctx) {
Profile(ID, Template, template_arguments(), Ctx);
if (isTypeAlias())
getAliasedType().Profile(ID);
}

static void Profile(llvm::FoldingSetNodeID &ID, TemplateName T,
ArrayRef<TemplateArgument> Args,
const ASTContext &Context);

static bool classof(const Type *T) {
return T->getTypeClass() == TemplateSpecialization;
}
};



void printTemplateArgumentList(raw_ostream &OS,
ArrayRef<TemplateArgument> Args,
const PrintingPolicy &Policy,
const TemplateParameterList *TPL = nullptr);

void printTemplateArgumentList(raw_ostream &OS,
ArrayRef<TemplateArgumentLoc> Args,
const PrintingPolicy &Policy,
const TemplateParameterList *TPL = nullptr);

void printTemplateArgumentList(raw_ostream &OS,
const TemplateArgumentListInfo &Args,
const PrintingPolicy &Policy,
const TemplateParameterList *TPL = nullptr);


















class InjectedClassNameType : public Type {
friend class ASTContext;
friend class ASTNodeImporter;
friend class ASTReader;


template <class T> friend class serialization::AbstractTypeReader;

CXXRecordDecl *Decl;










QualType InjectedType;

InjectedClassNameType(CXXRecordDecl *D, QualType TST)
: Type(InjectedClassName, QualType(),
TypeDependence::DependentInstantiation),
Decl(D), InjectedType(TST) {
assert(isa<TemplateSpecializationType>(TST));
assert(!TST.hasQualifiers());
assert(TST->isDependentType());
}

public:
QualType getInjectedSpecializationType() const { return InjectedType; }

const TemplateSpecializationType *getInjectedTST() const {
return cast<TemplateSpecializationType>(InjectedType.getTypePtr());
}

TemplateName getTemplateName() const {
return getInjectedTST()->getTemplateName();
}

CXXRecordDecl *getDecl() const;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == InjectedClassName;
}
};


enum TagTypeKind {

TTK_Struct,


TTK_Interface,


TTK_Union,


TTK_Class,


TTK_Enum
};



enum ElaboratedTypeKeyword {

ETK_Struct,


ETK_Interface,


ETK_Union,


ETK_Class,


ETK_Enum,



ETK_Typename,


ETK_None
};





class TypeWithKeyword : public Type {
protected:
TypeWithKeyword(ElaboratedTypeKeyword Keyword, TypeClass tc,
QualType Canonical, TypeDependence Dependence)
: Type(tc, Canonical, Dependence) {
TypeWithKeywordBits.Keyword = Keyword;
}

public:
ElaboratedTypeKeyword getKeyword() const {
return static_cast<ElaboratedTypeKeyword>(TypeWithKeywordBits.Keyword);
}


static ElaboratedTypeKeyword getKeywordForTypeSpec(unsigned TypeSpec);



static TagTypeKind getTagTypeKindForTypeSpec(unsigned TypeSpec);


static ElaboratedTypeKeyword getKeywordForTagTypeKind(TagTypeKind Tag);




static TagTypeKind getTagTypeKindForKeyword(ElaboratedTypeKeyword Keyword);

static bool KeywordIsTagTypeKind(ElaboratedTypeKeyword Keyword);

static StringRef getKeywordName(ElaboratedTypeKeyword Keyword);

static StringRef getTagTypeKindName(TagTypeKind Kind) {
return getKeywordName(getKeywordForTagTypeKind(Kind));
}

class CannotCastToThisType {};
static CannotCastToThisType classof(const Type *);
};









class ElaboratedType final
: public TypeWithKeyword,
public llvm::FoldingSetNode,
private llvm::TrailingObjects<ElaboratedType, TagDecl *> {
friend class ASTContext;
friend TrailingObjects;


NestedNameSpecifier *NNS;


QualType NamedType;





ElaboratedType(ElaboratedTypeKeyword Keyword, NestedNameSpecifier *NNS,
QualType NamedType, QualType CanonType, TagDecl *OwnedTagDecl)
: TypeWithKeyword(Keyword, Elaborated, CanonType,




NamedType->getDependence() |
(NNS ? toSyntacticDependence(
toTypeDependence(NNS->getDependence()))
: TypeDependence::None)),
NNS(NNS), NamedType(NamedType) {
ElaboratedTypeBits.HasOwnedTagDecl = false;
if (OwnedTagDecl) {
ElaboratedTypeBits.HasOwnedTagDecl = true;
*getTrailingObjects<TagDecl *>() = OwnedTagDecl;
}
assert(!(Keyword == ETK_None && NNS == nullptr) &&
"ElaboratedType cannot have elaborated type keyword "
"and name qualifier both null.");
}

public:

NestedNameSpecifier *getQualifier() const { return NNS; }


QualType getNamedType() const { return NamedType; }


QualType desugar() const { return getNamedType(); }


bool isSugared() const { return true; }



TagDecl *getOwnedTagDecl() const {
return ElaboratedTypeBits.HasOwnedTagDecl ? *getTrailingObjects<TagDecl *>()
: nullptr;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getKeyword(), NNS, NamedType, getOwnedTagDecl());
}

static void Profile(llvm::FoldingSetNodeID &ID, ElaboratedTypeKeyword Keyword,
NestedNameSpecifier *NNS, QualType NamedType,
TagDecl *OwnedTagDecl) {
ID.AddInteger(Keyword);
ID.AddPointer(NNS);
NamedType.Profile(ID);
ID.AddPointer(OwnedTagDecl);
}

static bool classof(const Type *T) { return T->getTypeClass() == Elaborated; }
};













class DependentNameType : public TypeWithKeyword, public llvm::FoldingSetNode {
friend class ASTContext;


NestedNameSpecifier *NNS;


const IdentifierInfo *Name;

DependentNameType(ElaboratedTypeKeyword Keyword, NestedNameSpecifier *NNS,
const IdentifierInfo *Name, QualType CanonType)
: TypeWithKeyword(Keyword, DependentName, CanonType,
TypeDependence::DependentInstantiation |
toTypeDependence(NNS->getDependence())),
NNS(NNS), Name(Name) {}

public:

NestedNameSpecifier *getQualifier() const { return NNS; }






const IdentifierInfo *getIdentifier() const {
return Name;
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getKeyword(), NNS, Name);
}

static void Profile(llvm::FoldingSetNodeID &ID, ElaboratedTypeKeyword Keyword,
NestedNameSpecifier *NNS, const IdentifierInfo *Name) {
ID.AddInteger(Keyword);
ID.AddPointer(NNS);
ID.AddPointer(Name);
}

static bool classof(const Type *T) {
return T->getTypeClass() == DependentName;
}
};




class alignas(8) DependentTemplateSpecializationType
: public TypeWithKeyword,
public llvm::FoldingSetNode {
friend class ASTContext;


NestedNameSpecifier *NNS;


const IdentifierInfo *Name;

DependentTemplateSpecializationType(ElaboratedTypeKeyword Keyword,
NestedNameSpecifier *NNS,
const IdentifierInfo *Name,
ArrayRef<TemplateArgument> Args,
QualType Canon);

const TemplateArgument *getArgBuffer() const {
return reinterpret_cast<const TemplateArgument*>(this+1);
}

TemplateArgument *getArgBuffer() {
return reinterpret_cast<TemplateArgument*>(this+1);
}

public:
NestedNameSpecifier *getQualifier() const { return NNS; }
const IdentifierInfo *getIdentifier() const { return Name; }


const TemplateArgument *getArgs() const {
return getArgBuffer();
}


unsigned getNumArgs() const {
return DependentTemplateSpecializationTypeBits.NumArgs;
}

const TemplateArgument &getArg(unsigned Idx) const;

ArrayRef<TemplateArgument> template_arguments() const {
return {getArgs(), getNumArgs()};
}

using iterator = const TemplateArgument *;

iterator begin() const { return getArgs(); }
iterator end() const;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context) {
Profile(ID, Context, getKeyword(), NNS, Name, {getArgs(), getNumArgs()});
}

static void Profile(llvm::FoldingSetNodeID &ID,
const ASTContext &Context,
ElaboratedTypeKeyword Keyword,
NestedNameSpecifier *Qualifier,
const IdentifierInfo *Name,
ArrayRef<TemplateArgument> Args);

static bool classof(const Type *T) {
return T->getTypeClass() == DependentTemplateSpecialization;
}
};























class PackExpansionType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;


QualType Pattern;

PackExpansionType(QualType Pattern, QualType Canon,
Optional<unsigned> NumExpansions)
: Type(PackExpansion, Canon,
(Pattern->getDependence() | TypeDependence::Dependent |
TypeDependence::Instantiation) &
~TypeDependence::UnexpandedPack),
Pattern(Pattern) {
PackExpansionTypeBits.NumExpansions =
NumExpansions ? *NumExpansions + 1 : 0;
}

public:



QualType getPattern() const { return Pattern; }



Optional<unsigned> getNumExpansions() const {
if (PackExpansionTypeBits.NumExpansions)
return PackExpansionTypeBits.NumExpansions - 1;
return None;
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getPattern(), getNumExpansions());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType Pattern,
Optional<unsigned> NumExpansions) {
ID.AddPointer(Pattern.getAsOpaquePtr());
ID.AddBoolean(NumExpansions.hasValue());
if (NumExpansions)
ID.AddInteger(*NumExpansions);
}

static bool classof(const Type *T) {
return T->getTypeClass() == PackExpansion;
}
};



template <class T>
class ObjCProtocolQualifiers {
protected:
ObjCProtocolQualifiers() = default;

ObjCProtocolDecl * const *getProtocolStorage() const {
return const_cast<ObjCProtocolQualifiers*>(this)->getProtocolStorage();
}

ObjCProtocolDecl **getProtocolStorage() {
return static_cast<T*>(this)->getProtocolStorageImpl();
}

void setNumProtocols(unsigned N) {
static_cast<T*>(this)->setNumProtocolsImpl(N);
}

void initialize(ArrayRef<ObjCProtocolDecl *> protocols) {
setNumProtocols(protocols.size());
assert(getNumProtocols() == protocols.size() &&
"bitfield overflow in protocol count");
if (!protocols.empty())
memcpy(getProtocolStorage(), protocols.data(),
protocols.size() * sizeof(ObjCProtocolDecl*));
}

public:
using qual_iterator = ObjCProtocolDecl * const *;
using qual_range = llvm::iterator_range<qual_iterator>;

qual_range quals() const { return qual_range(qual_begin(), qual_end()); }
qual_iterator qual_begin() const { return getProtocolStorage(); }
qual_iterator qual_end() const { return qual_begin() + getNumProtocols(); }

bool qual_empty() const { return getNumProtocols() == 0; }



unsigned getNumProtocols() const {
return static_cast<const T*>(this)->getNumProtocolsImpl();
}


ObjCProtocolDecl *getProtocol(unsigned I) const {
assert(I < getNumProtocols() && "Out-of-range protocol access");
return qual_begin()[I];
}


ArrayRef<ObjCProtocolDecl *> getProtocols() const {
return ArrayRef<ObjCProtocolDecl *>(qual_begin(), getNumProtocols());
}
};



class ObjCTypeParamType : public Type,
public ObjCProtocolQualifiers<ObjCTypeParamType>,
public llvm::FoldingSetNode {
friend class ASTContext;
friend class ObjCProtocolQualifiers<ObjCTypeParamType>;


unsigned NumProtocols : 6;

ObjCTypeParamDecl *OTPDecl;




ObjCProtocolDecl **getProtocolStorageImpl();



unsigned getNumProtocolsImpl() const {
return NumProtocols;
}

void setNumProtocolsImpl(unsigned N) {
NumProtocols = N;
}

ObjCTypeParamType(const ObjCTypeParamDecl *D,
QualType can,
ArrayRef<ObjCProtocolDecl *> protocols);

public:
bool isSugared() const { return true; }
QualType desugar() const { return getCanonicalTypeInternal(); }

static bool classof(const Type *T) {
return T->getTypeClass() == ObjCTypeParam;
}

void Profile(llvm::FoldingSetNodeID &ID);
static void Profile(llvm::FoldingSetNodeID &ID,
const ObjCTypeParamDecl *OTPDecl,
QualType CanonicalType,
ArrayRef<ObjCProtocolDecl *> protocols);

ObjCTypeParamDecl *getDecl() const { return OTPDecl; }
};





























class ObjCObjectType : public Type,
public ObjCProtocolQualifiers<ObjCObjectType> {
friend class ObjCProtocolQualifiers<ObjCObjectType>;














QualType BaseType;


mutable llvm::PointerIntPair<const ObjCObjectType *, 1, bool>
CachedSuperClassType;

QualType *getTypeArgStorage();
const QualType *getTypeArgStorage() const {
return const_cast<ObjCObjectType *>(this)->getTypeArgStorage();
}

ObjCProtocolDecl **getProtocolStorageImpl();


unsigned getNumProtocolsImpl() const {
return ObjCObjectTypeBits.NumProtocols;
}
void setNumProtocolsImpl(unsigned N) {
ObjCObjectTypeBits.NumProtocols = N;
}

protected:
enum Nonce_ObjCInterface { Nonce_ObjCInterface };

ObjCObjectType(QualType Canonical, QualType Base,
ArrayRef<QualType> typeArgs,
ArrayRef<ObjCProtocolDecl *> protocols,
bool isKindOf);

ObjCObjectType(enum Nonce_ObjCInterface)
: Type(ObjCInterface, QualType(), TypeDependence::None),
BaseType(QualType(this_(), 0)) {
ObjCObjectTypeBits.NumProtocols = 0;
ObjCObjectTypeBits.NumTypeArgs = 0;
ObjCObjectTypeBits.IsKindOf = 0;
}

void computeSuperClassTypeSlow() const;

public:






QualType getBaseType() const { return BaseType; }

bool isObjCId() const {
return getBaseType()->isSpecificBuiltinType(BuiltinType::ObjCId);
}

bool isObjCClass() const {
return getBaseType()->isSpecificBuiltinType(BuiltinType::ObjCClass);
}

bool isObjCUnqualifiedId() const { return qual_empty() && isObjCId(); }
bool isObjCUnqualifiedClass() const { return qual_empty() && isObjCClass(); }
bool isObjCUnqualifiedIdOrClass() const {
if (!qual_empty()) return false;
if (const BuiltinType *T = getBaseType()->getAs<BuiltinType>())
return T->getKind() == BuiltinType::ObjCId ||
T->getKind() == BuiltinType::ObjCClass;
return false;
}
bool isObjCQualifiedId() const { return !qual_empty() && isObjCId(); }
bool isObjCQualifiedClass() const { return !qual_empty() && isObjCClass(); }



ObjCInterfaceDecl *getInterface() const;



bool isSpecialized() const;


bool isSpecializedAsWritten() const {
return ObjCObjectTypeBits.NumTypeArgs > 0;
}



bool isUnspecialized() const { return !isSpecialized(); }



bool isUnspecializedAsWritten() const { return !isSpecializedAsWritten(); }


ArrayRef<QualType> getTypeArgs() const;



ArrayRef<QualType> getTypeArgsAsWritten() const {
return llvm::makeArrayRef(getTypeArgStorage(),
ObjCObjectTypeBits.NumTypeArgs);
}


bool isKindOfTypeAsWritten() const { return ObjCObjectTypeBits.IsKindOf; }


bool isKindOfType() const;







QualType getSuperClassType() const {
if (!CachedSuperClassType.getInt())
computeSuperClassTypeSlow();

assert(CachedSuperClassType.getInt() && "Superclass not set?");
return QualType(CachedSuperClassType.getPointer(), 0);
}



QualType stripObjCKindOfTypeAndQuals(const ASTContext &ctx) const;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == ObjCObject ||
T->getTypeClass() == ObjCInterface;
}
};





class ObjCObjectTypeImpl : public ObjCObjectType, public llvm::FoldingSetNode {
friend class ASTContext;




ObjCObjectTypeImpl(QualType Canonical, QualType Base,
ArrayRef<QualType> typeArgs,
ArrayRef<ObjCProtocolDecl *> protocols,
bool isKindOf)
: ObjCObjectType(Canonical, Base, typeArgs, protocols, isKindOf) {}

public:
void Profile(llvm::FoldingSetNodeID &ID);
static void Profile(llvm::FoldingSetNodeID &ID,
QualType Base,
ArrayRef<QualType> typeArgs,
ArrayRef<ObjCProtocolDecl *> protocols,
bool isKindOf);
};

inline QualType *ObjCObjectType::getTypeArgStorage() {
return reinterpret_cast<QualType *>(static_cast<ObjCObjectTypeImpl*>(this)+1);
}

inline ObjCProtocolDecl **ObjCObjectType::getProtocolStorageImpl() {
return reinterpret_cast<ObjCProtocolDecl**>(
getTypeArgStorage() + ObjCObjectTypeBits.NumTypeArgs);
}

inline ObjCProtocolDecl **ObjCTypeParamType::getProtocolStorageImpl() {
return reinterpret_cast<ObjCProtocolDecl**>(
static_cast<ObjCTypeParamType*>(this)+1);
}













class ObjCInterfaceType : public ObjCObjectType {
friend class ASTContext;
friend class ASTReader;
friend class ObjCInterfaceDecl;
template <class T> friend class serialization::AbstractTypeReader;

mutable ObjCInterfaceDecl *Decl;

ObjCInterfaceType(const ObjCInterfaceDecl *D)
: ObjCObjectType(Nonce_ObjCInterface),
Decl(const_cast<ObjCInterfaceDecl*>(D)) {}

public:

ObjCInterfaceDecl *getDecl() const { return Decl; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

static bool classof(const Type *T) {
return T->getTypeClass() == ObjCInterface;
}





enum {
qual_iterator,
qual_begin,
qual_end,
getNumProtocols,
getProtocol
};
};

inline ObjCInterfaceDecl *ObjCObjectType::getInterface() const {
QualType baseType = getBaseType();
while (const auto *ObjT = baseType->getAs<ObjCObjectType>()) {
if (const auto *T = dyn_cast<ObjCInterfaceType>(ObjT))
return T->getDecl();

baseType = ObjT->getBaseType();
}

return nullptr;
}










class ObjCObjectPointerType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

QualType PointeeType;

ObjCObjectPointerType(QualType Canonical, QualType Pointee)
: Type(ObjCObjectPointer, Canonical, Pointee->getDependence()),
PointeeType(Pointee) {}

public:


QualType getPointeeType() const { return PointeeType; }
























const ObjCObjectType *getObjectType() const {
return PointeeType->castAs<ObjCObjectType>();
}






const ObjCInterfaceType *getInterfaceType() const;





ObjCInterfaceDecl *getInterfaceDecl() const {
return getObjectType()->getInterface();
}



bool isObjCIdType() const {
return getObjectType()->isObjCUnqualifiedId();
}



bool isObjCClassType() const {
return getObjectType()->isObjCUnqualifiedClass();
}


bool isObjCIdOrClassType() const {
return getObjectType()->isObjCUnqualifiedIdOrClass();
}



bool isObjCQualifiedIdType() const {
return getObjectType()->isObjCQualifiedId();
}



bool isObjCQualifiedClassType() const {
return getObjectType()->isObjCQualifiedClass();
}


bool isKindOfType() const { return getObjectType()->isKindOfType(); }


bool isSpecialized() const { return getObjectType()->isSpecialized(); }


bool isSpecializedAsWritten() const {
return getObjectType()->isSpecializedAsWritten();
}


bool isUnspecialized() const { return getObjectType()->isUnspecialized(); }



bool isUnspecializedAsWritten() const { return !isSpecializedAsWritten(); }


ArrayRef<QualType> getTypeArgs() const {
return getObjectType()->getTypeArgs();
}


ArrayRef<QualType> getTypeArgsAsWritten() const {
return getObjectType()->getTypeArgsAsWritten();
}




using qual_iterator = ObjCObjectType::qual_iterator;
using qual_range = llvm::iterator_range<qual_iterator>;

qual_range quals() const { return qual_range(qual_begin(), qual_end()); }

qual_iterator qual_begin() const {
return getObjectType()->qual_begin();
}

qual_iterator qual_end() const {
return getObjectType()->qual_end();
}

bool qual_empty() const { return getObjectType()->qual_empty(); }


unsigned getNumProtocols() const {
return getObjectType()->getNumProtocols();
}


ObjCProtocolDecl *getProtocol(unsigned I) const {
return getObjectType()->getProtocol(I);
}

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }







QualType getSuperClassType() const;



const ObjCObjectPointerType *stripObjCKindOfTypeAndQuals(
const ASTContext &ctx) const;

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getPointeeType());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType T) {
ID.AddPointer(T.getAsOpaquePtr());
}

static bool classof(const Type *T) {
return T->getTypeClass() == ObjCObjectPointer;
}
};

class AtomicType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

QualType ValueType;

AtomicType(QualType ValTy, QualType Canonical)
: Type(Atomic, Canonical, ValTy->getDependence()), ValueType(ValTy) {}

public:


QualType getValueType() const { return ValueType; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getValueType());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType T) {
ID.AddPointer(T.getAsOpaquePtr());
}

static bool classof(const Type *T) {
return T->getTypeClass() == Atomic;
}
};


class PipeType : public Type, public llvm::FoldingSetNode {
friend class ASTContext;

QualType ElementType;
bool isRead;

PipeType(QualType elemType, QualType CanonicalPtr, bool isRead)
: Type(Pipe, CanonicalPtr, elemType->getDependence()),
ElementType(elemType), isRead(isRead) {}

public:
QualType getElementType() const { return ElementType; }

bool isSugared() const { return false; }

QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getElementType(), isReadOnly());
}

static void Profile(llvm::FoldingSetNodeID &ID, QualType T, bool isRead) {
ID.AddPointer(T.getAsOpaquePtr());
ID.AddBoolean(isRead);
}

static bool classof(const Type *T) {
return T->getTypeClass() == Pipe;
}

bool isReadOnly() const { return isRead; }
};


class ExtIntType final : public Type, public llvm::FoldingSetNode {
friend class ASTContext;
unsigned IsUnsigned : 1;
unsigned NumBits : 24;

protected:
ExtIntType(bool isUnsigned, unsigned NumBits);

public:
bool isUnsigned() const { return IsUnsigned; }
bool isSigned() const { return !IsUnsigned; }
unsigned getNumBits() const { return NumBits; }

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, isUnsigned(), getNumBits());
}

static void Profile(llvm::FoldingSetNodeID &ID, bool IsUnsigned,
unsigned NumBits) {
ID.AddBoolean(IsUnsigned);
ID.AddInteger(NumBits);
}

static bool classof(const Type *T) { return T->getTypeClass() == ExtInt; }
};

class DependentExtIntType final : public Type, public llvm::FoldingSetNode {
friend class ASTContext;
const ASTContext &Context;
llvm::PointerIntPair<Expr*, 1, bool> ExprAndUnsigned;

protected:
DependentExtIntType(const ASTContext &Context, bool IsUnsigned,
Expr *NumBits);

public:
bool isUnsigned() const;
bool isSigned() const { return !isUnsigned(); }
Expr *getNumBitsExpr() const;

bool isSugared() const { return false; }
QualType desugar() const { return QualType(this, 0); }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, Context, isUnsigned(), getNumBitsExpr());
}
static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context,
bool IsUnsigned, Expr *NumBitsExpr);

static bool classof(const Type *T) {
return T->getTypeClass() == DependentExtInt;
}
};


class QualifierCollector : public Qualifiers {
public:
QualifierCollector(Qualifiers Qs = Qualifiers()) : Qualifiers(Qs) {}




const Type *strip(QualType type) {
addFastQualifiers(type.getLocalFastQualifiers());
if (!type.hasLocalNonFastQualifiers())
return type.getTypePtrUnsafe();

const ExtQuals *extQuals = type.getExtQualsUnsafe();
addConsistentQualifiers(extQuals->getQualifiers());
return extQuals->getBaseType();
}


QualType apply(const ASTContext &Context, QualType QT) const;


QualType apply(const ASTContext &Context, const Type* T) const;
};








class alignas(8) TypeSourceInfo {


friend class ASTContext;

QualType Ty;

TypeSourceInfo(QualType ty) : Ty(ty) {}

public:

QualType getType() const { return Ty; }


TypeLoc getTypeLoc() const;


void overrideType(QualType T) { Ty = T; }
};



inline SplitQualType SplitQualType::getSingleStepDesugaredType() const {
SplitQualType desugar =
Ty->getLocallyUnqualifiedSingleStepDesugaredType().split();
desugar.Quals.addConsistentQualifiers(Quals);
return desugar;
}

inline const Type *QualType::getTypePtr() const {
return getCommonPtr()->BaseType;
}

inline const Type *QualType::getTypePtrOrNull() const {
return (isNull() ? nullptr : getCommonPtr()->BaseType);
}

inline SplitQualType QualType::split() const {
if (!hasLocalNonFastQualifiers())
return SplitQualType(getTypePtrUnsafe(),
Qualifiers::fromFastMask(getLocalFastQualifiers()));

const ExtQuals *eq = getExtQualsUnsafe();
Qualifiers qs = eq->getQualifiers();
qs.addFastQualifiers(getLocalFastQualifiers());
return SplitQualType(eq->getBaseType(), qs);
}

inline Qualifiers QualType::getLocalQualifiers() const {
Qualifiers Quals;
if (hasLocalNonFastQualifiers())
Quals = getExtQualsUnsafe()->getQualifiers();
Quals.addFastQualifiers(getLocalFastQualifiers());
return Quals;
}

inline Qualifiers QualType::getQualifiers() const {
Qualifiers quals = getCommonPtr()->CanonicalType.getLocalQualifiers();
quals.addFastQualifiers(getLocalFastQualifiers());
return quals;
}

inline unsigned QualType::getCVRQualifiers() const {
unsigned cvr = getCommonPtr()->CanonicalType.getLocalCVRQualifiers();
cvr |= getLocalCVRQualifiers();
return cvr;
}

inline QualType QualType::getCanonicalType() const {
QualType canon = getCommonPtr()->CanonicalType;
return canon.withFastQualifiers(getLocalFastQualifiers());
}

inline bool QualType::isCanonical() const {
return getTypePtr()->isCanonicalUnqualified();
}

inline bool QualType::isCanonicalAsParam() const {
if (!isCanonical()) return false;
if (hasLocalQualifiers()) return false;

const Type *T = getTypePtr();
if (T->isVariablyModifiedType() && T->hasSizedVLAType())
return false;

return !isa<FunctionType>(T) && !isa<ArrayType>(T);
}

inline bool QualType::isConstQualified() const {
return isLocalConstQualified() ||
getCommonPtr()->CanonicalType.isLocalConstQualified();
}

inline bool QualType::isRestrictQualified() const {
return isLocalRestrictQualified() ||
getCommonPtr()->CanonicalType.isLocalRestrictQualified();
}


inline bool QualType::isVolatileQualified() const {
return isLocalVolatileQualified() ||
getCommonPtr()->CanonicalType.isLocalVolatileQualified();
}

inline bool QualType::hasQualifiers() const {
return hasLocalQualifiers() ||
getCommonPtr()->CanonicalType.hasLocalQualifiers();
}

inline QualType QualType::getUnqualifiedType() const {
if (!getTypePtr()->getCanonicalTypeInternal().hasLocalQualifiers())
return QualType(getTypePtr(), 0);

return QualType(getSplitUnqualifiedTypeImpl(*this).Ty, 0);
}

inline SplitQualType QualType::getSplitUnqualifiedType() const {
if (!getTypePtr()->getCanonicalTypeInternal().hasLocalQualifiers())
return split();

return getSplitUnqualifiedTypeImpl(*this);
}

inline void QualType::removeLocalConst() {
removeLocalFastQualifiers(Qualifiers::Const);
}

inline void QualType::removeLocalRestrict() {
removeLocalFastQualifiers(Qualifiers::Restrict);
}

inline void QualType::removeLocalVolatile() {
removeLocalFastQualifiers(Qualifiers::Volatile);
}

inline void QualType::removeLocalCVRQualifiers(unsigned Mask) {
assert(!(Mask & ~Qualifiers::CVRMask) && "mask has non-CVR bits");
static_assert((int)Qualifiers::CVRMask == (int)Qualifiers::FastMask,
"Fast bits differ from CVR bits!");


removeLocalFastQualifiers(Mask);
}


inline bool QualType::hasAddressSpace() const {
return getQualifiers().hasAddressSpace();
}


inline LangAS QualType::getAddressSpace() const {
return getQualifiers().getAddressSpace();
}


inline Qualifiers::GC QualType::getObjCGCAttr() const {
return getQualifiers().getObjCGCAttr();
}

inline bool QualType::hasNonTrivialToPrimitiveDefaultInitializeCUnion() const {
if (auto *RD = getTypePtr()->getBaseElementTypeUnsafe()->getAsRecordDecl())
return hasNonTrivialToPrimitiveDefaultInitializeCUnion(RD);
return false;
}

inline bool QualType::hasNonTrivialToPrimitiveDestructCUnion() const {
if (auto *RD = getTypePtr()->getBaseElementTypeUnsafe()->getAsRecordDecl())
return hasNonTrivialToPrimitiveDestructCUnion(RD);
return false;
}

inline bool QualType::hasNonTrivialToPrimitiveCopyCUnion() const {
if (auto *RD = getTypePtr()->getBaseElementTypeUnsafe()->getAsRecordDecl())
return hasNonTrivialToPrimitiveCopyCUnion(RD);
return false;
}

inline FunctionType::ExtInfo getFunctionExtInfo(const Type &t) {
if (const auto *PT = t.getAs<PointerType>()) {
if (const auto *FT = PT->getPointeeType()->getAs<FunctionType>())
return FT->getExtInfo();
} else if (const auto *FT = t.getAs<FunctionType>())
return FT->getExtInfo();

return FunctionType::ExtInfo();
}

inline FunctionType::ExtInfo getFunctionExtInfo(QualType t) {
return getFunctionExtInfo(*t);
}






inline bool QualType::isMoreQualifiedThan(QualType other) const {
Qualifiers MyQuals = getQualifiers();
Qualifiers OtherQuals = other.getQualifiers();
return (MyQuals != OtherQuals && MyQuals.compatiblyIncludes(OtherQuals));
}





inline bool QualType::isAtLeastAsQualifiedAs(QualType other) const {
Qualifiers OtherQuals = other.getQualifiers();


if (getUnqualifiedType()->isVoidType())
OtherQuals.removeUnaligned();

return getQualifiers().compatiblyIncludes(OtherQuals);
}










inline QualType QualType::getNonReferenceType() const {
if (const auto *RefType = (*this)->getAs<ReferenceType>())
return RefType->getPointeeType();
else
return *this;
}

inline bool QualType::isCForbiddenLValueType() const {
return ((getTypePtr()->isVoidType() && !hasQualifiers()) ||
getTypePtr()->isFunctionType());
}




inline bool Type::isFundamentalType() const {
return isVoidType() ||
isNullPtrType() ||


(isArithmeticType() && !isEnumeralType());
}




inline bool Type::isCompoundType() const {



return isArrayType() ||

isFunctionType() ||

isPointerType() ||

isReferenceType() ||

isRecordType() ||


isUnionType() ||

isEnumeralType() ||

isMemberPointerType();
}

inline bool Type::isFunctionType() const {
return isa<FunctionType>(CanonicalType);
}

inline bool Type::isPointerType() const {
return isa<PointerType>(CanonicalType);
}

inline bool Type::isAnyPointerType() const {
return isPointerType() || isObjCObjectPointerType();
}

inline bool Type::isBlockPointerType() const {
return isa<BlockPointerType>(CanonicalType);
}

inline bool Type::isReferenceType() const {
return isa<ReferenceType>(CanonicalType);
}

inline bool Type::isLValueReferenceType() const {
return isa<LValueReferenceType>(CanonicalType);
}

inline bool Type::isRValueReferenceType() const {
return isa<RValueReferenceType>(CanonicalType);
}

inline bool Type::isObjectPointerType() const {



if (const auto *T = getAs<PointerType>())
return !T->getPointeeType()->isFunctionType();
else
return false;
}

inline bool Type::isFunctionPointerType() const {
if (const auto *T = getAs<PointerType>())
return T->getPointeeType()->isFunctionType();
else
return false;
}

inline bool Type::isFunctionReferenceType() const {
if (const auto *T = getAs<ReferenceType>())
return T->getPointeeType()->isFunctionType();
else
return false;
}

inline bool Type::isMemberPointerType() const {
return isa<MemberPointerType>(CanonicalType);
}

inline bool Type::isMemberFunctionPointerType() const {
if (const auto *T = getAs<MemberPointerType>())
return T->isMemberFunctionPointer();
else
return false;
}

inline bool Type::isMemberDataPointerType() const {
if (const auto *T = getAs<MemberPointerType>())
return T->isMemberDataPointer();
else
return false;
}

inline bool Type::isArrayType() const {
return isa<ArrayType>(CanonicalType);
}

inline bool Type::isConstantArrayType() const {
return isa<ConstantArrayType>(CanonicalType);
}

inline bool Type::isIncompleteArrayType() const {
return isa<IncompleteArrayType>(CanonicalType);
}

inline bool Type::isVariableArrayType() const {
return isa<VariableArrayType>(CanonicalType);
}

inline bool Type::isDependentSizedArrayType() const {
return isa<DependentSizedArrayType>(CanonicalType);
}

inline bool Type::isBuiltinType() const {
return isa<BuiltinType>(CanonicalType);
}

inline bool Type::isRecordType() const {
return isa<RecordType>(CanonicalType);
}

inline bool Type::isEnumeralType() const {
return isa<EnumType>(CanonicalType);
}

inline bool Type::isAnyComplexType() const {
return isa<ComplexType>(CanonicalType);
}

inline bool Type::isVectorType() const {
return isa<VectorType>(CanonicalType);
}

inline bool Type::isExtVectorType() const {
return isa<ExtVectorType>(CanonicalType);
}

inline bool Type::isMatrixType() const {
return isa<MatrixType>(CanonicalType);
}

inline bool Type::isConstantMatrixType() const {
return isa<ConstantMatrixType>(CanonicalType);
}

inline bool Type::isDependentAddressSpaceType() const {
return isa<DependentAddressSpaceType>(CanonicalType);
}

inline bool Type::isObjCObjectPointerType() const {
return isa<ObjCObjectPointerType>(CanonicalType);
}

inline bool Type::isObjCObjectType() const {
return isa<ObjCObjectType>(CanonicalType);
}

inline bool Type::isObjCObjectOrInterfaceType() const {
return isa<ObjCInterfaceType>(CanonicalType) ||
isa<ObjCObjectType>(CanonicalType);
}

inline bool Type::isAtomicType() const {
return isa<AtomicType>(CanonicalType);
}

inline bool Type::isUndeducedAutoType() const {
return isa<AutoType>(CanonicalType);
}

inline bool Type::isObjCQualifiedIdType() const {
if (const auto *OPT = getAs<ObjCObjectPointerType>())
return OPT->isObjCQualifiedIdType();
return false;
}

inline bool Type::isObjCQualifiedClassType() const {
if (const auto *OPT = getAs<ObjCObjectPointerType>())
return OPT->isObjCQualifiedClassType();
return false;
}

inline bool Type::isObjCIdType() const {
if (const auto *OPT = getAs<ObjCObjectPointerType>())
return OPT->isObjCIdType();
return false;
}

inline bool Type::isObjCClassType() const {
if (const auto *OPT = getAs<ObjCObjectPointerType>())
return OPT->isObjCClassType();
return false;
}

inline bool Type::isObjCSelType() const {
if (const auto *OPT = getAs<PointerType>())
return OPT->getPointeeType()->isSpecificBuiltinType(BuiltinType::ObjCSel);
return false;
}

inline bool Type::isObjCBuiltinType() const {
return isObjCIdType() || isObjCClassType() || isObjCSelType();
}

inline bool Type::isDecltypeType() const {
return isa<DecltypeType>(this);
}

#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) inline bool Type::is##Id##Type() const { return isSpecificBuiltinType(BuiltinType::Id); }



#include "clang/Basic/OpenCLImageTypes.def"

inline bool Type::isSamplerT() const {
return isSpecificBuiltinType(BuiltinType::OCLSampler);
}

inline bool Type::isEventT() const {
return isSpecificBuiltinType(BuiltinType::OCLEvent);
}

inline bool Type::isClkEventT() const {
return isSpecificBuiltinType(BuiltinType::OCLClkEvent);
}

inline bool Type::isQueueT() const {
return isSpecificBuiltinType(BuiltinType::OCLQueue);
}

inline bool Type::isReserveIDT() const {
return isSpecificBuiltinType(BuiltinType::OCLReserveID);
}

inline bool Type::isImageType() const {
#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) is##Id##Type() ||
return
#include "clang/Basic/OpenCLImageTypes.def"
false;
}

inline bool Type::isPipeType() const {
return isa<PipeType>(CanonicalType);
}

inline bool Type::isExtIntType() const {
return isa<ExtIntType>(CanonicalType);
}

#define EXT_OPAQUE_TYPE(ExtType, Id, Ext) inline bool Type::is##Id##Type() const { return isSpecificBuiltinType(BuiltinType::Id); }



#include "clang/Basic/OpenCLExtensionTypes.def"

inline bool Type::isOCLIntelSubgroupAVCType() const {
#define INTEL_SUBGROUP_AVC_TYPE(ExtType, Id) isOCLIntelSubgroupAVC##Id##Type() ||

return
#include "clang/Basic/OpenCLExtensionTypes.def"
false;
}

inline bool Type::isOCLExtOpaqueType() const {
#define EXT_OPAQUE_TYPE(ExtType, Id, Ext) is##Id##Type() ||
return
#include "clang/Basic/OpenCLExtensionTypes.def"
false;
}

inline bool Type::isOpenCLSpecificType() const {
return isSamplerT() || isEventT() || isImageType() || isClkEventT() ||
isQueueT() || isReserveIDT() || isPipeType() || isOCLExtOpaqueType();
}

inline bool Type::isTemplateTypeParmType() const {
return isa<TemplateTypeParmType>(CanonicalType);
}

inline bool Type::isSpecificBuiltinType(unsigned K) const {
if (const BuiltinType *BT = getAs<BuiltinType>()) {
return BT->getKind() == static_cast<BuiltinType::Kind>(K);
}
return false;
}

inline bool Type::isPlaceholderType() const {
if (const auto *BT = dyn_cast<BuiltinType>(this))
return BT->isPlaceholderType();
return false;
}

inline const BuiltinType *Type::getAsPlaceholderType() const {
if (const auto *BT = dyn_cast<BuiltinType>(this))
if (BT->isPlaceholderType())
return BT;
return nullptr;
}

inline bool Type::isSpecificPlaceholderType(unsigned K) const {
assert(BuiltinType::isPlaceholderTypeKind((BuiltinType::Kind) K));
return isSpecificBuiltinType(K);
}

inline bool Type::isNonOverloadPlaceholderType() const {
if (const auto *BT = dyn_cast<BuiltinType>(this))
return BT->isNonOverloadPlaceholderType();
return false;
}

inline bool Type::isVoidType() const {
return isSpecificBuiltinType(BuiltinType::Void);
}

inline bool Type::isHalfType() const {

return isSpecificBuiltinType(BuiltinType::Half);
}

inline bool Type::isFloat16Type() const {
return isSpecificBuiltinType(BuiltinType::Float16);
}

inline bool Type::isBFloat16Type() const {
return isSpecificBuiltinType(BuiltinType::BFloat16);
}

inline bool Type::isFloat128Type() const {
return isSpecificBuiltinType(BuiltinType::Float128);
}

inline bool Type::isNullPtrType() const {
return isSpecificBuiltinType(BuiltinType::NullPtr);
}

bool IsEnumDeclComplete(EnumDecl *);
bool IsEnumDeclScoped(EnumDecl *);

inline bool Type::isIntegerType() const {
if (const auto *BT = dyn_cast<BuiltinType>(CanonicalType))
return BT->getKind() >= BuiltinType::Bool &&
BT->getKind() <= BuiltinType::Int128;
if (const EnumType *ET = dyn_cast<EnumType>(CanonicalType)) {


return IsEnumDeclComplete(ET->getDecl()) &&
!IsEnumDeclScoped(ET->getDecl());
}
return isExtIntType();
}

inline bool Type::isFixedPointType() const {
if (const auto *BT = dyn_cast<BuiltinType>(CanonicalType)) {
return BT->getKind() >= BuiltinType::ShortAccum &&
BT->getKind() <= BuiltinType::SatULongFract;
}
return false;
}

inline bool Type::isFixedPointOrIntegerType() const {
return isFixedPointType() || isIntegerType();
}

inline bool Type::isSaturatedFixedPointType() const {
if (const auto *BT = dyn_cast<BuiltinType>(CanonicalType)) {
return BT->getKind() >= BuiltinType::SatShortAccum &&
BT->getKind() <= BuiltinType::SatULongFract;
}
return false;
}

inline bool Type::isUnsaturatedFixedPointType() const {
return isFixedPointType() && !isSaturatedFixedPointType();
}

inline bool Type::isSignedFixedPointType() const {
if (const auto *BT = dyn_cast<BuiltinType>(CanonicalType)) {
return ((BT->getKind() >= BuiltinType::ShortAccum &&
BT->getKind() <= BuiltinType::LongAccum) ||
(BT->getKind() >= BuiltinType::ShortFract &&
BT->getKind() <= BuiltinType::LongFract) ||
(BT->getKind() >= BuiltinType::SatShortAccum &&
BT->getKind() <= BuiltinType::SatLongAccum) ||
(BT->getKind() >= BuiltinType::SatShortFract &&
BT->getKind() <= BuiltinType::SatLongFract));
}
return false;
}

inline bool Type::isUnsignedFixedPointType() const {
return isFixedPointType() && !isSignedFixedPointType();
}

inline bool Type::isScalarType() const {
if (const auto *BT = dyn_cast<BuiltinType>(CanonicalType))
return BT->getKind() > BuiltinType::Void &&
BT->getKind() <= BuiltinType::NullPtr;
if (const EnumType *ET = dyn_cast<EnumType>(CanonicalType))


return IsEnumDeclComplete(ET->getDecl());
return isa<PointerType>(CanonicalType) ||
isa<BlockPointerType>(CanonicalType) ||
isa<MemberPointerType>(CanonicalType) ||
isa<ComplexType>(CanonicalType) ||
isa<ObjCObjectPointerType>(CanonicalType) ||
isExtIntType();
}

inline bool Type::isIntegralOrEnumerationType() const {
if (const auto *BT = dyn_cast<BuiltinType>(CanonicalType))
return BT->getKind() >= BuiltinType::Bool &&
BT->getKind() <= BuiltinType::Int128;



if (const auto *ET = dyn_cast<EnumType>(CanonicalType))
return IsEnumDeclComplete(ET->getDecl());

return isExtIntType();
}

inline bool Type::isBooleanType() const {
if (const auto *BT = dyn_cast<BuiltinType>(CanonicalType))
return BT->getKind() == BuiltinType::Bool;
return false;
}

inline bool Type::isUndeducedType() const {
auto *DT = getContainedDeducedType();
return DT && !DT->isDeduced();
}



inline bool Type::isOverloadableType() const {
return isDependentType() || isRecordType() || isEnumeralType();
}


inline bool Type::isTypedefNameType() const {
if (getAs<TypedefType>())
return true;
if (auto *TST = getAs<TemplateSpecializationType>())
return TST->isTypeAlias();
return false;
}


inline bool Type::canDecayToPointerType() const {
return isFunctionType() || isArrayType();
}

inline bool Type::hasPointerRepresentation() const {
return (isPointerType() || isReferenceType() || isBlockPointerType() ||
isObjCObjectPointerType() || isNullPtrType());
}

inline bool Type::hasObjCPointerRepresentation() const {
return isObjCObjectPointerType();
}

inline const Type *Type::getBaseElementTypeUnsafe() const {
const Type *type = this;
while (const ArrayType *arrayType = type->getAsArrayTypeUnsafe())
type = arrayType->getElementType().getTypePtr();
return type;
}

inline const Type *Type::getPointeeOrArrayElementType() const {
const Type *type = this;
if (type->isAnyPointerType())
return type->getPointeeType().getTypePtr();
else if (type->isArrayType())
return type->getBaseElementTypeUnsafe();
return type;
}


inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &PD,
LangAS AS) {
PD.AddTaggedVal(static_cast<std::underlying_type_t<LangAS>>(AS),
DiagnosticsEngine::ArgumentKind::ak_addrspace);
return PD;
}



inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &PD,
Qualifiers Q) {
PD.AddTaggedVal(Q.getAsOpaqueValue(),
DiagnosticsEngine::ArgumentKind::ak_qual);
return PD;
}



inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &PD,
QualType T) {
PD.AddTaggedVal(reinterpret_cast<intptr_t>(T.getAsOpaquePtr()),
DiagnosticsEngine::ak_qualtype);
return PD;
}



template <typename T>
using TypeIsArrayType =
std::integral_constant<bool, std::is_same<T, ArrayType>::value ||
std::is_base_of<ArrayType, T>::value>;


template <typename T> const T *Type::getAs() const {
static_assert(!TypeIsArrayType<T>::value,
"ArrayType cannot be used with getAs!");


if (const auto *Ty = dyn_cast<T>(this))
return Ty;


if (!isa<T>(CanonicalType))
return nullptr;



return cast<T>(getUnqualifiedDesugaredType());
}

template <typename T> const T *Type::getAsAdjusted() const {
static_assert(!TypeIsArrayType<T>::value, "ArrayType cannot be used with getAsAdjusted!");


if (const auto *Ty = dyn_cast<T>(this))
return Ty;


if (!isa<T>(CanonicalType))
return nullptr;



const Type *Ty = this;
while (Ty) {
if (const auto *A = dyn_cast<AttributedType>(Ty))
Ty = A->getModifiedType().getTypePtr();
else if (const auto *E = dyn_cast<ElaboratedType>(Ty))
Ty = E->desugar().getTypePtr();
else if (const auto *P = dyn_cast<ParenType>(Ty))
Ty = P->desugar().getTypePtr();
else if (const auto *A = dyn_cast<AdjustedType>(Ty))
Ty = A->desugar().getTypePtr();
else if (const auto *M = dyn_cast<MacroQualifiedType>(Ty))
Ty = M->desugar().getTypePtr();
else
break;
}



return dyn_cast<T>(Ty);
}

inline const ArrayType *Type::getAsArrayTypeUnsafe() const {

if (const auto *arr = dyn_cast<ArrayType>(this))
return arr;


if (!isa<ArrayType>(CanonicalType))
return nullptr;



return cast<ArrayType>(getUnqualifiedDesugaredType());
}

template <typename T> const T *Type::castAs() const {
static_assert(!TypeIsArrayType<T>::value,
"ArrayType cannot be used with castAs!");

if (const auto *ty = dyn_cast<T>(this)) return ty;
assert(isa<T>(CanonicalType));
return cast<T>(getUnqualifiedDesugaredType());
}

inline const ArrayType *Type::castAsArrayTypeUnsafe() const {
assert(isa<ArrayType>(CanonicalType));
if (const auto *arr = dyn_cast<ArrayType>(this)) return arr;
return cast<ArrayType>(getUnqualifiedDesugaredType());
}

DecayedType::DecayedType(QualType OriginalType, QualType DecayedPtr,
QualType CanonicalPtr)
: AdjustedType(Decayed, OriginalType, DecayedPtr, CanonicalPtr) {
#if !defined(NDEBUG)
QualType Adjusted = getAdjustedType();
(void)AttributedType::stripOuterNullability(Adjusted);
assert(isa<PointerType>(Adjusted));
#endif
}

QualType DecayedType::getPointeeType() const {
QualType Decayed = getDecayedType();
(void)AttributedType::stripOuterNullability(Decayed);
return cast<PointerType>(Decayed)->getPointeeType();
}





void FixedPointValueToString(SmallVectorImpl<char> &Str, llvm::APSInt Val,
unsigned Scale);

}

#endif
