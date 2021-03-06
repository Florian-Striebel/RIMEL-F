












#if !defined(LLVM_CLANG_AST_CANONICALTYPE_H)
#define LLVM_CLANG_AST_CANONICALTYPE_H

#include "clang/AST/Type.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/iterator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include <cassert>
#include <iterator>
#include <type_traits>

namespace clang {

template<typename T> class CanProxy;
template<typename T> struct CanProxyAdaptor;
class CXXRecordDecl;
class EnumDecl;
class Expr;
class IdentifierInfo;
class ObjCInterfaceDecl;
class RecordDecl;
class TagDecl;
class TemplateTypeParmDecl;























template<typename T = Type>
class CanQual {

QualType Stored;

public:

CanQual() = default;



template <typename U>
CanQual(const CanQual<U> &Other,
std::enable_if_t<std::is_base_of<T, U>::value, int> = 0);





const T *getTypePtr() const { return cast<T>(Stored.getTypePtr()); }



const T *getTypePtrOrNull() const {
return cast_or_null<T>(Stored.getTypePtrOrNull());
}


operator QualType() const { return Stored; }


explicit operator bool() const { return !isNull(); }

bool isNull() const {
return Stored.isNull();
}

SplitQualType split() const { return Stored.split(); }


















template<typename U> CanProxy<U> getAs() const;

template<typename U> CanProxy<U> castAs() const;



CanProxy<T> operator->() const;


Qualifiers getQualifiers() const { return Stored.getLocalQualifiers(); }


unsigned getCVRQualifiers() const { return Stored.getLocalCVRQualifiers(); }


bool hasQualifiers() const { return Stored.hasLocalQualifiers(); }

bool isConstQualified() const {
return Stored.isLocalConstQualified();
}

bool isVolatileQualified() const {
return Stored.isLocalVolatileQualified();
}

bool isRestrictQualified() const {
return Stored.isLocalRestrictQualified();
}




bool isCanonicalAsParam() const {
return Stored.isCanonicalAsParam();
}


CanQual<T> getUnqualifiedType() const;



QualType withConst() const {
return Stored.withConst();
}



bool isMoreQualifiedThan(CanQual<T> Other) const {
return Stored.isMoreQualifiedThan(Other.Stored);
}



bool isAtLeastAsQualifiedAs(CanQual<T> Other) const {
return Stored.isAtLeastAsQualifiedAs(Other.Stored);
}



CanQual<Type> getNonReferenceType() const;


void *getAsOpaquePtr() const { return Stored.getAsOpaquePtr(); }


static CanQual<T> getFromOpaquePtr(void *Ptr);






static CanQual<T> CreateUnsafe(QualType Other);

void dump() const { Stored.dump(); }

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.AddPointer(getAsOpaquePtr());
}
};

template<typename T, typename U>
inline bool operator==(CanQual<T> x, CanQual<U> y) {
return x.getAsOpaquePtr() == y.getAsOpaquePtr();
}

template<typename T, typename U>
inline bool operator!=(CanQual<T> x, CanQual<U> y) {
return x.getAsOpaquePtr() != y.getAsOpaquePtr();
}


using CanQualType = CanQual<Type>;

inline CanQualType Type::getCanonicalTypeUnqualified() const {
return CanQualType::CreateUnsafe(getCanonicalTypeInternal());
}

inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
CanQualType T) {
DB << static_cast<QualType>(T);
return DB;
}





#define LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(Accessor) CanQualType Accessor() const { return CanQualType::CreateUnsafe(this->getTypePtr()->Accessor()); }




#define LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(Type, Accessor) Type Accessor() const { return this->getTypePtr()->Accessor(); }




template<typename T>
class CanProxyBase {
protected:
CanQual<T> Stored;

public:

const T *getTypePtr() const { return Stored.getTypePtr(); }








operator const T*() const { return this->Stored.getTypePtrOrNull(); }



template<typename U> CanProxy<U> getAs() const {
return this->Stored.template getAs<U>();
}

LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(Type::TypeClass, getTypeClass)


LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjectType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isIncompleteType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isSizelessType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isSizelessBuiltinType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isIncompleteOrObjectType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isVariablyModifiedType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isIntegerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isEnumeralType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isBooleanType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isCharType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isWideCharType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isIntegralType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isIntegralOrEnumerationType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isRealFloatingType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isComplexType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isAnyComplexType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isFloatingType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isRealType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isArithmeticType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isVoidType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isDerivedType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isScalarType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isAggregateType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isAnyPointerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isVoidPointerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isFunctionPointerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isMemberFunctionPointerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isClassType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isStructureType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isInterfaceType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isStructureOrClassType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isUnionType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isComplexIntegerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isNullPtrType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isDependentType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isOverloadableType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isArrayType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasPointerRepresentation)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasObjCPointerRepresentation)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasIntegerRepresentation)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasSignedIntegerRepresentation)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasUnsignedIntegerRepresentation)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasFloatingRepresentation)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isPromotableIntegerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isSignedIntegerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isUnsignedIntegerType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isSignedIntegerOrEnumerationType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isUnsignedIntegerOrEnumerationType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isConstantSizeType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isSpecifierType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(CXXRecordDecl*, getAsCXXRecordDecl)







const CanProxyAdaptor<T> *operator->() const {
return static_cast<const CanProxyAdaptor<T> *>(this);
}
};











template<typename T>
struct CanProxyAdaptor : CanProxyBase<T> {};








template<typename T>
class CanProxy : public CanProxyAdaptor<T> {
public:

CanProxy() = default;


CanProxy(CanQual<T> Stored) { this->Stored = Stored; }


operator CanQual<T>() const { return this->Stored; }
};

}

namespace llvm {




template<typename T>
struct simplify_type< ::clang::CanQual<T>> {
using SimpleType = const T *;

static SimpleType getSimplifiedValue(::clang::CanQual<T> Val) {
return Val.getTypePtr();
}
};


template<typename T>
struct PointerLikeTypeTraits<clang::CanQual<T>> {
static void *getAsVoidPointer(clang::CanQual<T> P) {
return P.getAsOpaquePtr();
}

static clang::CanQual<T> getFromVoidPointer(void *P) {
return clang::CanQual<T>::getFromOpaquePtr(P);
}


static constexpr int NumLowBitsAvailable = 0;
};

}

namespace clang {







template <typename InputIterator>
struct CanTypeIterator
: llvm::iterator_adaptor_base<
CanTypeIterator<InputIterator>, InputIterator,
typename std::iterator_traits<InputIterator>::iterator_category,
CanQualType,
typename std::iterator_traits<InputIterator>::difference_type,
CanProxy<Type>, CanQualType> {
CanTypeIterator() = default;
explicit CanTypeIterator(InputIterator Iter)
: CanTypeIterator::iterator_adaptor_base(std::move(Iter)) {}

CanQualType operator*() const { return CanQualType::CreateUnsafe(*this->I); }
CanProxy<Type> operator->() const;
};

template<>
struct CanProxyAdaptor<ComplexType> : public CanProxyBase<ComplexType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getElementType)
};

template<>
struct CanProxyAdaptor<PointerType> : public CanProxyBase<PointerType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getPointeeType)
};

template<>
struct CanProxyAdaptor<BlockPointerType>
: public CanProxyBase<BlockPointerType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getPointeeType)
};

template<>
struct CanProxyAdaptor<ReferenceType> : public CanProxyBase<ReferenceType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getPointeeType)
};

template<>
struct CanProxyAdaptor<LValueReferenceType>
: public CanProxyBase<LValueReferenceType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getPointeeType)
};

template<>
struct CanProxyAdaptor<RValueReferenceType>
: public CanProxyBase<RValueReferenceType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getPointeeType)
};

template<>
struct CanProxyAdaptor<MemberPointerType>
: public CanProxyBase<MemberPointerType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getPointeeType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(const Type *, getClass)
};



template<> struct CanProxyAdaptor<ArrayType>;
template<> struct CanProxyAdaptor<ConstantArrayType>;
template<> struct CanProxyAdaptor<IncompleteArrayType>;
template<> struct CanProxyAdaptor<VariableArrayType>;
template<> struct CanProxyAdaptor<DependentSizedArrayType>;

template<>
struct CanProxyAdaptor<DependentSizedExtVectorType>
: public CanProxyBase<DependentSizedExtVectorType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getElementType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(const Expr *, getSizeExpr)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(SourceLocation, getAttributeLoc)
};

template<>
struct CanProxyAdaptor<VectorType> : public CanProxyBase<VectorType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getElementType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(unsigned, getNumElements)
};

template<>
struct CanProxyAdaptor<ExtVectorType> : public CanProxyBase<ExtVectorType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getElementType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(unsigned, getNumElements)
};

template<>
struct CanProxyAdaptor<FunctionType> : public CanProxyBase<FunctionType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getReturnType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(FunctionType::ExtInfo, getExtInfo)
};

template<>
struct CanProxyAdaptor<FunctionNoProtoType>
: public CanProxyBase<FunctionNoProtoType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getReturnType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(FunctionType::ExtInfo, getExtInfo)
};

template<>
struct CanProxyAdaptor<FunctionProtoType>
: public CanProxyBase<FunctionProtoType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getReturnType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(FunctionType::ExtInfo, getExtInfo)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(unsigned, getNumParams)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasExtParameterInfos)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(
ArrayRef<FunctionProtoType::ExtParameterInfo>, getExtParameterInfos)

CanQualType getParamType(unsigned i) const {
return CanQualType::CreateUnsafe(this->getTypePtr()->getParamType(i));
}

LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isVariadic)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(Qualifiers, getMethodQuals)

using param_type_iterator =
CanTypeIterator<FunctionProtoType::param_type_iterator>;

param_type_iterator param_type_begin() const {
return param_type_iterator(this->getTypePtr()->param_type_begin());
}

param_type_iterator param_type_end() const {
return param_type_iterator(this->getTypePtr()->param_type_end());
}


};

template<>
struct CanProxyAdaptor<TypeOfType> : public CanProxyBase<TypeOfType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getUnderlyingType)
};

template<>
struct CanProxyAdaptor<DecltypeType> : public CanProxyBase<DecltypeType> {
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(Expr *, getUnderlyingExpr)
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getUnderlyingType)
};

template <>
struct CanProxyAdaptor<UnaryTransformType>
: public CanProxyBase<UnaryTransformType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getBaseType)
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getUnderlyingType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(UnaryTransformType::UTTKind, getUTTKind)
};

template<>
struct CanProxyAdaptor<TagType> : public CanProxyBase<TagType> {
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(TagDecl *, getDecl)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isBeingDefined)
};

template<>
struct CanProxyAdaptor<RecordType> : public CanProxyBase<RecordType> {
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(RecordDecl *, getDecl)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isBeingDefined)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, hasConstFields)
};

template<>
struct CanProxyAdaptor<EnumType> : public CanProxyBase<EnumType> {
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(EnumDecl *, getDecl)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isBeingDefined)
};

template<>
struct CanProxyAdaptor<TemplateTypeParmType>
: public CanProxyBase<TemplateTypeParmType> {
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(unsigned, getDepth)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(unsigned, getIndex)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isParameterPack)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(TemplateTypeParmDecl *, getDecl)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(IdentifierInfo *, getIdentifier)
};

template<>
struct CanProxyAdaptor<ObjCObjectType>
: public CanProxyBase<ObjCObjectType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getBaseType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(const ObjCInterfaceDecl *,
getInterface)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCUnqualifiedId)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCUnqualifiedClass)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCQualifiedId)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCQualifiedClass)

using qual_iterator = ObjCObjectPointerType::qual_iterator;

LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(qual_iterator, qual_begin)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(qual_iterator, qual_end)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, qual_empty)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(unsigned, getNumProtocols)
};

template<>
struct CanProxyAdaptor<ObjCObjectPointerType>
: public CanProxyBase<ObjCObjectPointerType> {
LLVM_CLANG_CANPROXY_TYPE_ACCESSOR(getPointeeType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(const ObjCInterfaceType *,
getInterfaceType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCIdType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCClassType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCQualifiedIdType)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, isObjCQualifiedClassType)

using qual_iterator = ObjCObjectPointerType::qual_iterator;

LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(qual_iterator, qual_begin)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(qual_iterator, qual_end)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(bool, qual_empty)
LLVM_CLANG_CANPROXY_SIMPLE_ACCESSOR(unsigned, getNumProtocols)
};




template<typename T>
inline CanQual<T> CanQual<T>::getUnqualifiedType() const {
return CanQual<T>::CreateUnsafe(Stored.getLocalUnqualifiedType());
}

template<typename T>
inline CanQual<Type> CanQual<T>::getNonReferenceType() const {
if (CanQual<ReferenceType> RefType = getAs<ReferenceType>())
return RefType->getPointeeType();
else
return *this;
}

template<typename T>
CanQual<T> CanQual<T>::getFromOpaquePtr(void *Ptr) {
CanQual<T> Result;
Result.Stored = QualType::getFromOpaquePtr(Ptr);
assert((!Result || Result.Stored.getAsOpaquePtr() == (void*)-1 ||
Result.Stored.isCanonical()) && "Type is not canonical!");
return Result;
}

template<typename T>
CanQual<T> CanQual<T>::CreateUnsafe(QualType Other) {
assert((Other.isNull() || Other.isCanonical()) && "Type is not canonical!");
assert((Other.isNull() || isa<T>(Other.getTypePtr())) &&
"Dynamic type does not meet the static type's requires");
CanQual<T> Result;
Result.Stored = Other;
return Result;
}

template<typename T>
template<typename U>
CanProxy<U> CanQual<T>::getAs() const {
static_assert(!TypeIsArrayType<T>::value,
"ArrayType cannot be used with getAs!");

if (Stored.isNull())
return CanProxy<U>();

if (isa<U>(Stored.getTypePtr()))
return CanQual<U>::CreateUnsafe(Stored);

return CanProxy<U>();
}

template<typename T>
template<typename U>
CanProxy<U> CanQual<T>::castAs() const {
static_assert(!TypeIsArrayType<U>::value,
"ArrayType cannot be used with castAs!");

assert(!Stored.isNull() && isa<U>(Stored.getTypePtr()));
return CanQual<U>::CreateUnsafe(Stored);
}

template<typename T>
CanProxy<T> CanQual<T>::operator->() const {
return CanProxy<T>(*this);
}

template <typename InputIterator>
CanProxy<Type> CanTypeIterator<InputIterator>::operator->() const {
return CanProxy<Type>(*this);
}

}

#endif
