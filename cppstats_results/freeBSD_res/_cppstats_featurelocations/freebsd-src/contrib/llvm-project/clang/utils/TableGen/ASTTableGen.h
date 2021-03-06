







#if !defined(CLANG_AST_TABLEGEN_H)
#define CLANG_AST_TABLEGEN_H

#include "llvm/TableGen/Record.h"
#include "llvm/ADT/STLExtras.h"



#define HasPropertiesClassName "HasProperties"



#define ASTNodeClassName "ASTNode"
#define BaseFieldName "Base"
#define AbstractFieldName "Abstract"


#define CommentNodeClassName "CommentNode"


#define DeclNodeClassName "DeclNode"
#define DeclContextNodeClassName "DeclContext"


#define StmtNodeClassName "StmtNode"


#define TypeNodeClassName "TypeNode"
#define AlwaysDependentClassName "AlwaysDependent"
#define NeverCanonicalClassName "NeverCanonical"
#define NeverCanonicalUnlessDependentClassName "NeverCanonicalUnlessDependent"
#define LeafTypeClassName "LeafType"


#define TypeKindClassName "PropertyTypeKind"
#define KindTypeFieldName "KindType"
#define KindPropertyNameFieldName "KindPropertyName"
#define TypeCaseClassName "PropertyTypeCase"


#define PropertyClassName "Property"
#define ClassFieldName "Class"
#define NameFieldName "Name"
#define TypeFieldName "Type"
#define ReadFieldName "Read"


#define PropertyTypeClassName "PropertyType"
#define CXXTypeNameFieldName "CXXName"
#define PassByReferenceFieldName "PassByReference"
#define ConstWhenWritingFieldName "ConstWhenWriting"
#define ConditionalCodeFieldName "Conditional"
#define PackOptionalCodeFieldName "PackOptional"
#define UnpackOptionalCodeFieldName "UnpackOptional"
#define BufferElementTypesFieldName "BufferElementTypes"
#define ArrayTypeClassName "Array"
#define ArrayElementTypeFieldName "Element"
#define OptionalTypeClassName "Optional"
#define OptionalElementTypeFieldName "Element"
#define SubclassPropertyTypeClassName "SubclassPropertyType"
#define SubclassBaseTypeFieldName "Base"
#define SubclassClassNameFieldName "SubclassName"
#define EnumPropertyTypeClassName "EnumPropertyType"


#define ReadHelperRuleClassName "ReadHelper"
#define HelperCodeFieldName "Code"


#define CreationRuleClassName "Creator"
#define CreateFieldName "Create"


#define OverrideRuleClassName "Override"
#define IgnoredPropertiesFieldName "IgnoredProperties"

namespace clang {
namespace tblgen {

class WrappedRecord {
llvm::Record *Record;

protected:
WrappedRecord(llvm::Record *record = nullptr) : Record(record) {}

llvm::Record *get() const {
assert(Record && "accessing null record");
return Record;
}

public:
llvm::Record *getRecord() const { return Record; }

explicit operator bool() const { return Record != nullptr; }

llvm::ArrayRef<llvm::SMLoc> getLoc() const {
return get()->getLoc();
}


bool isSubClassOf(llvm::StringRef className) const {
return get()->isSubClassOf(className);
}

template <class NodeClass>
NodeClass getAs() const {
return (isSubClassOf(NodeClass::getTableGenNodeClassName())
? NodeClass(get()) : NodeClass());
}

friend bool operator<(WrappedRecord lhs, WrappedRecord rhs) {
assert(lhs && rhs && "sorting null nodes");
return lhs.get()->getName() < rhs.get()->getName();
}
friend bool operator>(WrappedRecord lhs, WrappedRecord rhs) {
return rhs < lhs;
}
friend bool operator<=(WrappedRecord lhs, WrappedRecord rhs) {
return !(rhs < lhs);
}
friend bool operator>=(WrappedRecord lhs, WrappedRecord rhs) {
return !(lhs < rhs);
}
friend bool operator==(WrappedRecord lhs, WrappedRecord rhs) {

return lhs.getRecord() == rhs.getRecord();
}
friend bool operator!=(WrappedRecord lhs, WrappedRecord rhs) {
return !(lhs == rhs);
}
};


class HasProperties : public WrappedRecord {
public:
static constexpr llvm::StringRef ClassName = HasPropertiesClassName;

HasProperties(llvm::Record *record = nullptr) : WrappedRecord(record) {}

llvm::StringRef getName() const;

static llvm::StringRef getTableGenNodeClassName() {
return HasPropertiesClassName;
}
};



class ASTNode : public HasProperties {
public:
ASTNode(llvm::Record *record = nullptr) : HasProperties(record) {}

llvm::StringRef getName() const {
return get()->getName();
}


ASTNode getBase() const {
return get()->getValueAsOptionalDef(BaseFieldName);
}


bool isAbstract() const {
return get()->getValueAsBit(AbstractFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return ASTNodeClassName;
}
};

class DeclNode : public ASTNode {
public:
DeclNode(llvm::Record *record = nullptr) : ASTNode(record) {}

llvm::StringRef getId() const;
std::string getClassName() const;
DeclNode getBase() const { return DeclNode(ASTNode::getBase().getRecord()); }

static llvm::StringRef getASTHierarchyName() {
return "Decl";
}
static llvm::StringRef getASTIdTypeName() {
return "Decl::Kind";
}
static llvm::StringRef getASTIdAccessorName() {
return "getKind";
}
static llvm::StringRef getTableGenNodeClassName() {
return DeclNodeClassName;
}
};

class TypeNode : public ASTNode {
public:
TypeNode(llvm::Record *record = nullptr) : ASTNode(record) {}

llvm::StringRef getId() const;
llvm::StringRef getClassName() const;
TypeNode getBase() const { return TypeNode(ASTNode::getBase().getRecord()); }

static llvm::StringRef getASTHierarchyName() {
return "Type";
}
static llvm::StringRef getASTIdTypeName() {
return "Type::TypeClass";
}
static llvm::StringRef getASTIdAccessorName() {
return "getTypeClass";
}
static llvm::StringRef getTableGenNodeClassName() {
return TypeNodeClassName;
}
};

class StmtNode : public ASTNode {
public:
StmtNode(llvm::Record *record = nullptr) : ASTNode(record) {}

std::string getId() const;
llvm::StringRef getClassName() const;
StmtNode getBase() const { return StmtNode(ASTNode::getBase().getRecord()); }

static llvm::StringRef getASTHierarchyName() {
return "Stmt";
}
static llvm::StringRef getASTIdTypeName() {
return "Stmt::StmtClass";
}
static llvm::StringRef getASTIdAccessorName() {
return "getStmtClass";
}
static llvm::StringRef getTableGenNodeClassName() {
return StmtNodeClassName;
}
};


class PropertyType : public WrappedRecord {
public:
PropertyType(llvm::Record *record = nullptr) : WrappedRecord(record) {}


bool isGenericSpecialization() const {
return get()->isAnonymous();
}



llvm::StringRef getAbstractTypeName() const {
return get()->getName();
}



llvm::StringRef getCXXTypeName() const {
return get()->getValueAsString(CXXTypeNameFieldName);
}
void emitCXXValueTypeName(bool forRead, llvm::raw_ostream &out) const;


bool shouldPassByReference() const {
return get()->getValueAsBit(PassByReferenceFieldName);
}



bool isConstWhenWriting() const {
return get()->getValueAsBit(ConstWhenWritingFieldName);
}


PropertyType getArrayElementType() const {
if (isSubClassOf(ArrayTypeClassName))
return get()->getValueAsDef(ArrayElementTypeFieldName);
return nullptr;
}


PropertyType getOptionalElementType() const {
if (isSubClassOf(OptionalTypeClassName))
return get()->getValueAsDef(OptionalElementTypeFieldName);
return nullptr;
}


PropertyType getSuperclassType() const {
if (isSubClassOf(SubclassPropertyTypeClassName))
return get()->getValueAsDef(SubclassBaseTypeFieldName);
return nullptr;
}




llvm::StringRef getSubclassClassName() const {
return get()->getValueAsString(SubclassClassNameFieldName);
}


bool isEnum() const {
return isSubClassOf(EnumPropertyTypeClassName);
}

llvm::StringRef getPackOptionalCode() const {
return get()->getValueAsString(PackOptionalCodeFieldName);
}

llvm::StringRef getUnpackOptionalCode() const {
return get()->getValueAsString(UnpackOptionalCodeFieldName);
}

std::vector<llvm::Record*> getBufferElementTypes() const {
return get()->getValueAsListOfDefs(BufferElementTypesFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return PropertyTypeClassName;
}
};


class TypeKindRule : public WrappedRecord {
public:
TypeKindRule(llvm::Record *record = nullptr) : WrappedRecord(record) {}


PropertyType getParentType() const {
return get()->getValueAsDef(TypeFieldName);
}


PropertyType getKindType() const {
return get()->getValueAsDef(KindTypeFieldName);
}


llvm::StringRef getKindPropertyName() const {
return get()->getValueAsString(KindPropertyNameFieldName);
}


llvm::StringRef getReadCode() const {
return get()->getValueAsString(ReadFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return TypeKindClassName;
}
};


class TypeCase : public HasProperties {
public:
TypeCase(llvm::Record *record = nullptr) : HasProperties(record) {}


llvm::StringRef getCaseName() const {
return get()->getValueAsString(NameFieldName);
}


PropertyType getParentType() const {
return get()->getValueAsDef(TypeFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return TypeCaseClassName;
}
};


class Property : public WrappedRecord {
public:
Property(llvm::Record *record = nullptr) : WrappedRecord(record) {}


llvm::StringRef getName() const {
return get()->getValueAsString(NameFieldName);
}


PropertyType getType() const {
return get()->getValueAsDef(TypeFieldName);
}


HasProperties getClass() const {
return get()->getValueAsDef(ClassFieldName);
}


llvm::StringRef getReadCode() const {
return get()->getValueAsString(ReadFieldName);
}


llvm::StringRef getCondition() const {
return get()->getValueAsString(ConditionalCodeFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return PropertyClassName;
}
};



class ReadHelperRule : public WrappedRecord {
public:
ReadHelperRule(llvm::Record *record = nullptr) : WrappedRecord(record) {}



HasProperties getClass() const {
return get()->getValueAsDef(ClassFieldName);
}

llvm::StringRef getHelperCode() const {
return get()->getValueAsString(HelperCodeFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return ReadHelperRuleClassName;
}
};


class CreationRule : public WrappedRecord {
public:
CreationRule(llvm::Record *record = nullptr) : WrappedRecord(record) {}



HasProperties getClass() const {
return get()->getValueAsDef(ClassFieldName);
}

llvm::StringRef getCreationCode() const {
return get()->getValueAsString(CreateFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return CreationRuleClassName;
}
};


class OverrideRule : public WrappedRecord {
public:
OverrideRule(llvm::Record *record = nullptr) : WrappedRecord(record) {}



HasProperties getClass() const {
return get()->getValueAsDef(ClassFieldName);
}




std::vector<llvm::StringRef> getIgnoredProperties() const {
return get()->getValueAsListOfStrings(IgnoredPropertiesFieldName);
}

static llvm::StringRef getTableGenNodeClassName() {
return OverrideRuleClassName;
}
};



template <class NodeClass>
using ASTNodeHierarchyVisitor =
llvm::function_ref<void(NodeClass node, NodeClass base)>;

void visitASTNodeHierarchyImpl(llvm::RecordKeeper &records,
llvm::StringRef nodeClassName,
ASTNodeHierarchyVisitor<ASTNode> visit);

template <class NodeClass>
void visitASTNodeHierarchy(llvm::RecordKeeper &records,
ASTNodeHierarchyVisitor<NodeClass> visit) {
visitASTNodeHierarchyImpl(records, NodeClass::getTableGenNodeClassName(),
[visit](ASTNode node, ASTNode base) {
visit(NodeClass(node.getRecord()),
NodeClass(base.getRecord()));
});
}

}
}

#endif
