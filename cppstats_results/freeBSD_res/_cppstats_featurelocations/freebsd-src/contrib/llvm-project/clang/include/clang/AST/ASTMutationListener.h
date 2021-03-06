










#if !defined(LLVM_CLANG_AST_ASTMUTATIONLISTENER_H)
#define LLVM_CLANG_AST_ASTMUTATIONLISTENER_H

namespace clang {
class Attr;
class ClassTemplateDecl;
class ClassTemplateSpecializationDecl;
class ConstructorUsingShadowDecl;
class CXXDestructorDecl;
class CXXRecordDecl;
class Decl;
class DeclContext;
class Expr;
class FieldDecl;
class FunctionDecl;
class FunctionTemplateDecl;
class Module;
class NamedDecl;
class ObjCCategoryDecl;
class ObjCContainerDecl;
class ObjCInterfaceDecl;
class ObjCPropertyDecl;
class ParmVarDecl;
class QualType;
class RecordDecl;
class TagDecl;
class ValueDecl;
class VarDecl;
class VarTemplateDecl;
class VarTemplateSpecializationDecl;




class ASTMutationListener {
public:
virtual ~ASTMutationListener();


virtual void CompletedTagDefinition(const TagDecl *D) { }


virtual void AddedVisibleDecl(const DeclContext *DC, const Decl *D) {}


virtual void AddedCXXImplicitMember(const CXXRecordDecl *RD, const Decl *D) {}



virtual void AddedCXXTemplateSpecialization(const ClassTemplateDecl *TD,
const ClassTemplateSpecializationDecl *D) {}



virtual void
AddedCXXTemplateSpecialization(const VarTemplateDecl *TD,
const VarTemplateSpecializationDecl *D) {}



virtual void AddedCXXTemplateSpecialization(const FunctionTemplateDecl *TD,
const FunctionDecl *D) {}



virtual void ResolvedExceptionSpec(const FunctionDecl *FD) {}


virtual void DeducedReturnType(const FunctionDecl *FD, QualType ReturnType);


virtual void ResolvedOperatorDelete(const CXXDestructorDecl *DD,
const FunctionDecl *Delete,
Expr *ThisArg) {}


virtual void CompletedImplicitDefinition(const FunctionDecl *D) {}




virtual void InstantiationRequested(const ValueDecl *D) {}


virtual void VariableDefinitionInstantiated(const VarDecl *D) {}


virtual void FunctionDefinitionInstantiated(const FunctionDecl *D) {}


virtual void DefaultArgumentInstantiated(const ParmVarDecl *D) {}


virtual void DefaultMemberInitializerInstantiated(const FieldDecl *D) {}


virtual void AddedObjCCategoryToInterface(const ObjCCategoryDecl *CatD,
const ObjCInterfaceDecl *IFD) {}




virtual void DeclarationMarkedUsed(const Decl *D) {}





virtual void DeclarationMarkedOpenMPThreadPrivate(const Decl *D) {}






virtual void DeclarationMarkedOpenMPDeclareTarget(const Decl *D,
const Attr *Attr) {}




virtual void DeclarationMarkedOpenMPAllocate(const Decl *D, const Attr *A) {}






virtual void RedefinedHiddenDefinition(const NamedDecl *D, Module *M) {}






virtual void AddedAttributeToRecord(const Attr *Attr,
const RecordDecl *Record) {}



};

}

#endif
