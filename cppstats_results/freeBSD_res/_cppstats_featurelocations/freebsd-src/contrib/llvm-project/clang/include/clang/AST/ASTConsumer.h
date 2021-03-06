











#if !defined(LLVM_CLANG_AST_ASTCONSUMER_H)
#define LLVM_CLANG_AST_ASTCONSUMER_H

namespace clang {
class ASTContext;
class CXXMethodDecl;
class CXXRecordDecl;
class Decl;
class DeclGroupRef;
class ASTMutationListener;
class ASTDeserializationListener;
class SemaConsumer;
class TagDecl;
class VarDecl;
class FunctionDecl;
class ImportDecl;




class ASTConsumer {


bool SemaConsumer;

friend class SemaConsumer;

public:
ASTConsumer() : SemaConsumer(false) { }

virtual ~ASTConsumer() {}



virtual void Initialize(ASTContext &Context) {}





virtual bool HandleTopLevelDecl(DeclGroupRef D);



virtual void HandleInlineFunctionDefinition(FunctionDecl *D) {}




virtual void HandleInterestingDecl(DeclGroupRef D);



virtual void HandleTranslationUnit(ASTContext &Ctx) {}





virtual void HandleTagDeclDefinition(TagDecl *D) {}



virtual void HandleTagDeclRequiredDefinition(const TagDecl *D) {}





virtual void HandleCXXImplicitFunctionInstantiation(FunctionDecl *D) {}




virtual void HandleTopLevelDeclInObjCContainer(DeclGroupRef D);




virtual void HandleImplicitImportDecl(ImportDecl *D);










virtual void CompleteTentativeDefinition(VarDecl *D) {}




virtual void CompleteExternalDeclaration(VarDecl *D) {}



virtual void AssignInheritanceModel(CXXRecordDecl *RD) {}



virtual void HandleCXXStaticMemberVarInstantiation(VarDecl *D) {}






virtual void HandleVTable(CXXRecordDecl *RD) {}




virtual ASTMutationListener *GetASTMutationListener() { return nullptr; }



virtual ASTDeserializationListener *GetASTDeserializationListener() {
return nullptr;
}


virtual void PrintStats() {}







virtual bool shouldSkipFunctionBody(Decl *D) { return true; }
};

}

#endif
