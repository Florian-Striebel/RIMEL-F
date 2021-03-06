












#if !defined(LLVM_CLANG_FRONTEND_MULTIPLEXCONSUMER_H)
#define LLVM_CLANG_FRONTEND_MULTIPLEXCONSUMER_H

#include "clang/Basic/LLVM.h"
#include "clang/Sema/SemaConsumer.h"
#include "clang/Serialization/ASTDeserializationListener.h"
#include <memory>
#include <vector>

namespace clang {

class MultiplexASTMutationListener;



class MultiplexASTDeserializationListener : public ASTDeserializationListener {
public:

MultiplexASTDeserializationListener(
const std::vector<ASTDeserializationListener *> &L);
void ReaderInitialized(ASTReader *Reader) override;
void IdentifierRead(serialization::IdentID ID, IdentifierInfo *II) override;
void MacroRead(serialization::MacroID ID, MacroInfo *MI) override;
void TypeRead(serialization::TypeIdx Idx, QualType T) override;
void DeclRead(serialization::DeclID ID, const Decl *D) override;
void SelectorRead(serialization::SelectorID iD, Selector Sel) override;
void MacroDefinitionRead(serialization::PreprocessedEntityID,
MacroDefinitionRecord *MD) override;
void ModuleRead(serialization::SubmoduleID ID, Module *Mod) override;

private:
std::vector<ASTDeserializationListener *> Listeners;
};


class MultiplexConsumer : public SemaConsumer {
public:

MultiplexConsumer(std::vector<std::unique_ptr<ASTConsumer>> C);
~MultiplexConsumer() override;


void Initialize(ASTContext &Context) override;
void HandleCXXStaticMemberVarInstantiation(VarDecl *VD) override;
bool HandleTopLevelDecl(DeclGroupRef D) override;
void HandleInlineFunctionDefinition(FunctionDecl *D) override;
void HandleInterestingDecl(DeclGroupRef D) override;
void HandleTranslationUnit(ASTContext &Ctx) override;
void HandleTagDeclDefinition(TagDecl *D) override;
void HandleTagDeclRequiredDefinition(const TagDecl *D) override;
void HandleCXXImplicitFunctionInstantiation(FunctionDecl *D) override;
void HandleTopLevelDeclInObjCContainer(DeclGroupRef D) override;
void HandleImplicitImportDecl(ImportDecl *D) override;
void CompleteTentativeDefinition(VarDecl *D) override;
void CompleteExternalDeclaration(VarDecl *D) override;
void AssignInheritanceModel(CXXRecordDecl *RD) override;
void HandleVTable(CXXRecordDecl *RD) override;
ASTMutationListener *GetASTMutationListener() override;
ASTDeserializationListener *GetASTDeserializationListener() override;
void PrintStats() override;
bool shouldSkipFunctionBody(Decl *D) override;


void InitializeSema(Sema &S) override;
void ForgetSema() override;

private:
std::vector<std::unique_ptr<ASTConsumer>> Consumers;
std::unique_ptr<MultiplexASTMutationListener> MutationListener;
std::unique_ptr<MultiplexASTDeserializationListener> DeserializationListener;
};

}

#endif
