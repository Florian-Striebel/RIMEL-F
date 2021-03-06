







#if !defined(LLVM_CLANG_CODEGEN_CODEGENACTION_H)
#define LLVM_CLANG_CODEGEN_CODEGENACTION_H

#include "clang/Frontend/FrontendAction.h"
#include <memory>

namespace llvm {
class LLVMContext;
class Module;
}

namespace clang {
class BackendConsumer;
class CodeGenerator;

class CodeGenAction : public ASTFrontendAction {
private:

friend class BackendConsumer;


struct LinkModule {

std::unique_ptr<llvm::Module> Module;




bool PropagateAttrs;


bool Internalize;


unsigned LinkFlags;
};

unsigned Act;
std::unique_ptr<llvm::Module> TheModule;


SmallVector<LinkModule, 4> LinkModules;
llvm::LLVMContext *VMContext;
bool OwnsVMContext;

std::unique_ptr<llvm::Module> loadModule(llvm::MemoryBufferRef MBRef);

protected:



CodeGenAction(unsigned _Act, llvm::LLVMContext *_VMContext = nullptr);

bool hasIRSupport() const override;

std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

void ExecuteAction() override;

void EndSourceFileAction() override;

public:
~CodeGenAction() override;



std::unique_ptr<llvm::Module> takeModule();


llvm::LLVMContext *takeLLVMContext();

CodeGenerator *getCodeGenerator() const;

BackendConsumer *BEConsumer;
};

class EmitAssemblyAction : public CodeGenAction {
virtual void anchor();
public:
EmitAssemblyAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitBCAction : public CodeGenAction {
virtual void anchor();
public:
EmitBCAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitLLVMAction : public CodeGenAction {
virtual void anchor();
public:
EmitLLVMAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitLLVMOnlyAction : public CodeGenAction {
virtual void anchor();
public:
EmitLLVMOnlyAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitCodeGenOnlyAction : public CodeGenAction {
virtual void anchor();
public:
EmitCodeGenOnlyAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitObjAction : public CodeGenAction {
virtual void anchor();
public:
EmitObjAction(llvm::LLVMContext *_VMContext = nullptr);
};

}

#endif
