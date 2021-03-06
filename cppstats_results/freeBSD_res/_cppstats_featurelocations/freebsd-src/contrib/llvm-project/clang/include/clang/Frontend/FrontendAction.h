















#if !defined(LLVM_CLANG_FRONTEND_FRONTENDACTION_H)
#define LLVM_CLANG_FRONTEND_FRONTENDACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/FrontendOptions.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <memory>
#include <string>
#include <vector>

namespace clang {
class ASTMergeAction;
class CompilerInstance;


class FrontendAction {
FrontendInputFile CurrentInput;
std::unique_ptr<ASTUnit> CurrentASTUnit;
CompilerInstance *Instance;
friend class ASTMergeAction;
friend class WrapperFrontendAction;

private:
std::unique_ptr<ASTConsumer> CreateWrappedASTConsumer(CompilerInstance &CI,
StringRef InFile);

protected:







virtual bool PrepareToExecuteAction(CompilerInstance &CI) { return true; }














virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) = 0;







virtual bool BeginInvocation(CompilerInstance &CI) { return true; }





virtual bool BeginSourceFileAction(CompilerInstance &CI) {
return true;
}






virtual void ExecuteAction() = 0;





virtual void EndSourceFileAction() {}







virtual bool shouldEraseOutputFiles();



public:
FrontendAction();
virtual ~FrontendAction();




CompilerInstance &getCompilerInstance() const {
assert(Instance && "Compiler instance not registered!");
return *Instance;
}

void setCompilerInstance(CompilerInstance *Value) { Instance = Value; }





bool isCurrentFileAST() const {
assert(!CurrentInput.isEmpty() && "No current file!");
return (bool)CurrentASTUnit;
}

const FrontendInputFile &getCurrentInput() const {
return CurrentInput;
}

StringRef getCurrentFile() const {
assert(!CurrentInput.isEmpty() && "No current file!");
return CurrentInput.getFile();
}

StringRef getCurrentFileOrBufferName() const {
assert(!CurrentInput.isEmpty() && "No current file!");
return CurrentInput.isFile()
? CurrentInput.getFile()
: CurrentInput.getBuffer().getBufferIdentifier();
}

InputKind getCurrentFileKind() const {
assert(!CurrentInput.isEmpty() && "No current file!");
return CurrentInput.getKind();
}

ASTUnit &getCurrentASTUnit() const {
assert(CurrentASTUnit && "No current AST unit!");
return *CurrentASTUnit;
}

Module *getCurrentModule() const;

std::unique_ptr<ASTUnit> takeCurrentASTUnit() {
return std::move(CurrentASTUnit);
}

void setCurrentInput(const FrontendInputFile &CurrentInput,
std::unique_ptr<ASTUnit> AST = nullptr);










virtual bool isModelParsingAction() const { return false; }





virtual bool usesPreprocessorOnly() const = 0;


virtual TranslationUnitKind getTranslationUnitKind() { return TU_Complete; }


virtual bool hasPCHSupport() const { return true; }


virtual bool hasASTFileSupport() const { return true; }


virtual bool hasIRSupport() const { return false; }


virtual bool hasCodeCompletionSupport() const { return false; }






bool PrepareToExecute(CompilerInstance &CI) {
return PrepareToExecuteAction(CI);
}




















bool BeginSourceFile(CompilerInstance &CI, const FrontendInputFile &Input);


llvm::Error Execute();



virtual void EndSourceFile();


};


class ASTFrontendAction : public FrontendAction {
protected:





void ExecuteAction() override;

public:
ASTFrontendAction() {}
bool usesPreprocessorOnly() const override { return false; }
};

class PluginASTAction : public ASTFrontendAction {
virtual void anchor();
public:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override = 0;







virtual bool ParseArgs(const CompilerInstance &CI,
const std::vector<std::string> &arg) = 0;

enum ActionType {
Cmdline,
ReplaceAction,
AddBeforeMainAction,
AddAfterMainAction
};





virtual ActionType getActionType() { return Cmdline; }
};


class PreprocessorFrontendAction : public FrontendAction {
protected:


std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

public:
bool usesPreprocessorOnly() const override { return true; }
};







class WrapperFrontendAction : public FrontendAction {
protected:
std::unique_ptr<FrontendAction> WrappedAction;

bool PrepareToExecuteAction(CompilerInstance &CI) override;
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
bool BeginInvocation(CompilerInstance &CI) override;
bool BeginSourceFileAction(CompilerInstance &CI) override;
void ExecuteAction() override;
void EndSourceFile() override;
void EndSourceFileAction() override;
bool shouldEraseOutputFiles() override;

public:


WrapperFrontendAction(std::unique_ptr<FrontendAction> WrappedAction);

bool usesPreprocessorOnly() const override;
TranslationUnitKind getTranslationUnitKind() override;
bool hasPCHSupport() const override;
bool hasASTFileSupport() const override;
bool hasIRSupport() const override;
bool hasCodeCompletionSupport() const override;
};

}

#endif
