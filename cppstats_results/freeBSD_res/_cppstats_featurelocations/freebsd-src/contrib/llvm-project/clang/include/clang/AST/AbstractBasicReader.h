







#if !defined(CLANG_AST_ABSTRACTBASICREADER_H)
#define CLANG_AST_ABSTRACTBASICREADER_H

#include "clang/AST/DeclTemplate.h"

namespace clang {
namespace serialization {

template <class T>
inline T makeNullableFromOptional(const Optional<T> &value) {
return (value ? *value : T());
}

template <class T>
inline T *makePointerFromOptional(Optional<T *> value) {
return (value ? *value : nullptr);
}











































































#include "clang/AST/AbstractBasicReader.inc"




















template <class Impl>
class DataStreamBasicReader : public BasicReaderBase<Impl> {
protected:
using BasicReaderBase<Impl>::asImpl;
DataStreamBasicReader(ASTContext &ctx) : BasicReaderBase<Impl>(ctx) {}

public:
using BasicReaderBase<Impl>::getASTContext;



Impl &find(const char *propertyName) {
return asImpl();
}

template <class T>
T readEnum() {
return T(asImpl().readUInt32());
}



Impl &readObject() { return asImpl(); }

template <class T>
llvm::ArrayRef<T> readArray(llvm::SmallVectorImpl<T> &buffer) {
assert(buffer.empty());

uint32_t size = asImpl().readUInt32();
buffer.reserve(size);

for (uint32_t i = 0; i != size; ++i) {
buffer.push_back(ReadDispatcher<T>::read(asImpl()));
}
return buffer;
}

template <class T, class... Args>
llvm::Optional<T> readOptional(Args &&...args) {
return UnpackOptionalValue<T>::unpack(
ReadDispatcher<T>::read(asImpl(), std::forward<Args>(args)...));
}

llvm::APSInt readAPSInt() {
bool isUnsigned = asImpl().readBool();
llvm::APInt value = asImpl().readAPInt();
return llvm::APSInt(std::move(value), isUnsigned);
}

llvm::APInt readAPInt() {
unsigned bitWidth = asImpl().readUInt32();
unsigned numWords = llvm::APInt::getNumWords(bitWidth);
llvm::SmallVector<uint64_t, 4> data;
for (uint32_t i = 0; i != numWords; ++i)
data.push_back(asImpl().readUInt64());
return llvm::APInt(bitWidth, numWords, &data[0]);
}

llvm::FixedPointSemantics readFixedPointSemantics() {
unsigned width = asImpl().readUInt32();
unsigned scale = asImpl().readUInt32();
unsigned tmp = asImpl().readUInt32();
bool isSigned = tmp & 0x1;
bool isSaturated = tmp & 0x2;
bool hasUnsignedPadding = tmp & 0x4;
return llvm::FixedPointSemantics(width, scale, isSigned, isSaturated,
hasUnsignedPadding);
}

APValue::LValuePathSerializationHelper readLValuePathSerializationHelper(
SmallVectorImpl<APValue::LValuePathEntry> &path) {
auto elemTy = asImpl().readQualType();
unsigned pathLength = asImpl().readUInt32();
for (unsigned i = 0; i < pathLength; ++i) {
if (elemTy->template getAs<RecordType>()) {
unsigned int_ = asImpl().readUInt32();
Decl *decl = asImpl().template readDeclAs<Decl>();
if (auto *recordDecl = dyn_cast<CXXRecordDecl>(decl))
elemTy = getASTContext().getRecordType(recordDecl);
else
elemTy = cast<ValueDecl>(decl)->getType();
path.push_back(
APValue::LValuePathEntry(APValue::BaseOrMemberType(decl, int_)));
} else {
elemTy = getASTContext().getAsArrayType(elemTy)->getElementType();
path.push_back(
APValue::LValuePathEntry::ArrayIndex(asImpl().readUInt32()));
}
}
return APValue::LValuePathSerializationHelper(path, elemTy);
}

Qualifiers readQualifiers() {
static_assert(sizeof(Qualifiers().getAsOpaqueValue()) <= sizeof(uint32_t),
"update this if the value size changes");
uint32_t value = asImpl().readUInt32();
return Qualifiers::fromOpaqueValue(value);
}

FunctionProtoType::ExceptionSpecInfo
readExceptionSpecInfo(llvm::SmallVectorImpl<QualType> &buffer) {
FunctionProtoType::ExceptionSpecInfo esi;
esi.Type = ExceptionSpecificationType(asImpl().readUInt32());
if (esi.Type == EST_Dynamic) {
esi.Exceptions = asImpl().template readArray<QualType>(buffer);
} else if (isComputedNoexcept(esi.Type)) {
esi.NoexceptExpr = asImpl().readExprRef();
} else if (esi.Type == EST_Uninstantiated) {
esi.SourceDecl = asImpl().readFunctionDeclRef();
esi.SourceTemplate = asImpl().readFunctionDeclRef();
} else if (esi.Type == EST_Unevaluated) {
esi.SourceDecl = asImpl().readFunctionDeclRef();
}
return esi;
}

FunctionProtoType::ExtParameterInfo readExtParameterInfo() {
static_assert(sizeof(FunctionProtoType::ExtParameterInfo().getOpaqueValue())
<= sizeof(uint32_t),
"opaque value doesn't fit into uint32_t");
uint32_t value = asImpl().readUInt32();
return FunctionProtoType::ExtParameterInfo::getFromOpaqueValue(value);
}

NestedNameSpecifier *readNestedNameSpecifier() {
auto &ctx = getASTContext();


NestedNameSpecifier *cur = nullptr;

uint32_t depth = asImpl().readUInt32();
for (uint32_t i = 0; i != depth; ++i) {
auto kind = asImpl().readNestedNameSpecifierKind();
switch (kind) {
case NestedNameSpecifier::Identifier:
cur = NestedNameSpecifier::Create(ctx, cur,
asImpl().readIdentifier());
continue;

case NestedNameSpecifier::Namespace:
cur = NestedNameSpecifier::Create(ctx, cur,
asImpl().readNamespaceDeclRef());
continue;

case NestedNameSpecifier::NamespaceAlias:
cur = NestedNameSpecifier::Create(ctx, cur,
asImpl().readNamespaceAliasDeclRef());
continue;

case NestedNameSpecifier::TypeSpec:
case NestedNameSpecifier::TypeSpecWithTemplate:
cur = NestedNameSpecifier::Create(ctx, cur,
kind == NestedNameSpecifier::TypeSpecWithTemplate,
asImpl().readQualType().getTypePtr());
continue;

case NestedNameSpecifier::Global:
cur = NestedNameSpecifier::GlobalSpecifier(ctx);
continue;

case NestedNameSpecifier::Super:
cur = NestedNameSpecifier::SuperSpecifier(ctx,
asImpl().readCXXRecordDeclRef());
continue;
}
llvm_unreachable("bad nested name specifier kind");
}

return cur;
}
};

}
}

#endif
