







#if !defined(CLANG_AST_ABSTRACTBASICWRITER_H)
#define CLANG_AST_ABSTRACTBASICWRITER_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclTemplate.h"

namespace clang {
namespace serialization {

template <class T>
inline llvm::Optional<T> makeOptionalFromNullable(const T &value) {
return (value.isNull()
? llvm::Optional<T>()
: llvm::Optional<T>(value));
}

template <class T>
inline llvm::Optional<T*> makeOptionalFromPointer(T *value) {
return (value ? llvm::Optional<T*>(value) : llvm::Optional<T*>());
}







































































#include "clang/AST/AbstractBasicWriter.inc"




















template <class Impl>
class DataStreamBasicWriter : public BasicWriterBase<Impl> {
protected:
using BasicWriterBase<Impl>::asImpl;
DataStreamBasicWriter(ASTContext &ctx) : BasicWriterBase<Impl>(ctx) {}

public:


Impl &find(const char *propertyName) {
return asImpl();
}



Impl &writeObject() { return asImpl(); }

template <class T>
void writeEnum(T value) {
asImpl().writeUInt32(uint32_t(value));
}

template <class T>
void writeArray(llvm::ArrayRef<T> array) {
asImpl().writeUInt32(array.size());
for (const T &elt : array) {
WriteDispatcher<T>::write(asImpl(), elt);
}
}

template <class T>
void writeOptional(llvm::Optional<T> value) {
WriteDispatcher<T>::write(asImpl(), PackOptionalValue<T>::pack(value));
}

void writeAPSInt(const llvm::APSInt &value) {
asImpl().writeBool(value.isUnsigned());
asImpl().writeAPInt(value);
}

void writeAPInt(const llvm::APInt &value) {
asImpl().writeUInt32(value.getBitWidth());
const uint64_t *words = value.getRawData();
for (size_t i = 0, e = value.getNumWords(); i != e; ++i)
asImpl().writeUInt64(words[i]);
}

void writeFixedPointSemantics(const llvm::FixedPointSemantics &sema) {
asImpl().writeUInt32(sema.getWidth());
asImpl().writeUInt32(sema.getScale());
asImpl().writeUInt32(sema.isSigned() | sema.isSaturated() << 1 |
sema.hasUnsignedPadding() << 2);
}

void writeLValuePathSerializationHelper(
APValue::LValuePathSerializationHelper lvaluePath) {
ArrayRef<APValue::LValuePathEntry> path = lvaluePath.Path;
QualType elemTy = lvaluePath.getType();
asImpl().writeQualType(elemTy);
asImpl().writeUInt32(path.size());
auto &ctx = ((BasicWriterBase<Impl> *)this)->getASTContext();
for (auto elem : path) {
if (elemTy->getAs<RecordType>()) {
asImpl().writeUInt32(elem.getAsBaseOrMember().getInt());
const Decl *baseOrMember = elem.getAsBaseOrMember().getPointer();
if (const auto *recordDecl = dyn_cast<CXXRecordDecl>(baseOrMember)) {
asImpl().writeDeclRef(recordDecl);
elemTy = ctx.getRecordType(recordDecl);
} else {
const auto *valueDecl = cast<ValueDecl>(baseOrMember);
asImpl().writeDeclRef(valueDecl);
elemTy = valueDecl->getType();
}
} else {
asImpl().writeUInt32(elem.getAsArrayIndex());
elemTy = ctx.getAsArrayType(elemTy)->getElementType();
}
}
}

void writeQualifiers(Qualifiers value) {
static_assert(sizeof(value.getAsOpaqueValue()) <= sizeof(uint32_t),
"update this if the value size changes");
asImpl().writeUInt32(value.getAsOpaqueValue());
}

void writeExceptionSpecInfo(
const FunctionProtoType::ExceptionSpecInfo &esi) {
asImpl().writeUInt32(uint32_t(esi.Type));
if (esi.Type == EST_Dynamic) {
asImpl().writeArray(esi.Exceptions);
} else if (isComputedNoexcept(esi.Type)) {
asImpl().writeExprRef(esi.NoexceptExpr);
} else if (esi.Type == EST_Uninstantiated) {
asImpl().writeDeclRef(esi.SourceDecl);
asImpl().writeDeclRef(esi.SourceTemplate);
} else if (esi.Type == EST_Unevaluated) {
asImpl().writeDeclRef(esi.SourceDecl);
}
}

void writeExtParameterInfo(FunctionProtoType::ExtParameterInfo epi) {
static_assert(sizeof(epi.getOpaqueValue()) <= sizeof(uint32_t),
"opaque value doesn't fit into uint32_t");
asImpl().writeUInt32(epi.getOpaqueValue());
}

void writeNestedNameSpecifier(NestedNameSpecifier *NNS) {


SmallVector<NestedNameSpecifier *, 8> nestedNames;


while (NNS) {
nestedNames.push_back(NNS);
NNS = NNS->getPrefix();
}

asImpl().writeUInt32(nestedNames.size());
while (!nestedNames.empty()) {
NNS = nestedNames.pop_back_val();
NestedNameSpecifier::SpecifierKind kind = NNS->getKind();
asImpl().writeNestedNameSpecifierKind(kind);
switch (kind) {
case NestedNameSpecifier::Identifier:
asImpl().writeIdentifier(NNS->getAsIdentifier());
continue;

case NestedNameSpecifier::Namespace:
asImpl().writeNamespaceDeclRef(NNS->getAsNamespace());
continue;

case NestedNameSpecifier::NamespaceAlias:
asImpl().writeNamespaceAliasDeclRef(NNS->getAsNamespaceAlias());
continue;

case NestedNameSpecifier::TypeSpec:
case NestedNameSpecifier::TypeSpecWithTemplate:
asImpl().writeQualType(QualType(NNS->getAsType(), 0));
continue;

case NestedNameSpecifier::Global:

continue;

case NestedNameSpecifier::Super:
asImpl().writeDeclRef(NNS->getAsRecordDecl());
continue;
}
llvm_unreachable("bad nested name specifier kind");
}
}
};

}
}

#endif
