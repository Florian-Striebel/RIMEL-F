







#if !defined(LLVM_CLANG_FRONTEND_FRONTENDACTIONS_H)
#define LLVM_CLANG_FRONTEND_FRONTENDACTIONS_H

#include "clang/Frontend/FrontendAction.h"
#include <string>
#include <vector>

namespace clang {

class Module;
class FileEntry;





class InitOnlyAction : public FrontendAction {
void ExecuteAction() override;

std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

public:


bool usesPreprocessorOnly() const override { return false; }
};


class ReadPCHAndPreprocessAction : public FrontendAction {
void ExecuteAction() override;

std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

public:
bool usesPreprocessorOnly() const override { return false; }
};

class DumpCompilerOptionsAction : public FrontendAction {
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override {
return nullptr;
}

void ExecuteAction() override;

public:
bool usesPreprocessorOnly() const override { return true; }
};





class ASTPrintAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
};

class ASTDumpAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
};

class ASTDeclListAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
};

class ASTViewAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
};

class GeneratePCHAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

TranslationUnitKind getTranslationUnitKind() override {
return TU_Prefix;
}

bool hasASTFileSupport() const override { return false; }

bool shouldEraseOutputFiles() override;

public:




static bool ComputeASTConsumerArguments(CompilerInstance &CI,
std::string &Sysroot);



static std::unique_ptr<llvm::raw_pwrite_stream>
CreateOutputFile(CompilerInstance &CI, StringRef InFile,
std::string &OutputFile);

bool BeginSourceFileAction(CompilerInstance &CI) override;
};

class GenerateModuleAction : public ASTFrontendAction {
virtual std::unique_ptr<raw_pwrite_stream>
CreateOutputFile(CompilerInstance &CI, StringRef InFile) = 0;

protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

TranslationUnitKind getTranslationUnitKind() override {
return TU_Module;
}

bool hasASTFileSupport() const override { return false; }

bool shouldEraseOutputFiles() override;
};

class GenerateInterfaceStubsAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

TranslationUnitKind getTranslationUnitKind() override { return TU_Module; }
bool hasASTFileSupport() const override { return false; }
};

class GenerateModuleFromModuleMapAction : public GenerateModuleAction {
private:
bool BeginSourceFileAction(CompilerInstance &CI) override;

std::unique_ptr<raw_pwrite_stream>
CreateOutputFile(CompilerInstance &CI, StringRef InFile) override;
};

class GenerateModuleInterfaceAction : public GenerateModuleAction {
private:
bool BeginSourceFileAction(CompilerInstance &CI) override;

std::unique_ptr<raw_pwrite_stream>
CreateOutputFile(CompilerInstance &CI, StringRef InFile) override;
};

class GenerateHeaderModuleAction : public GenerateModuleAction {

std::unique_ptr<llvm::MemoryBuffer> Buffer;
std::vector<std::string> ModuleHeaders;

private:
bool PrepareToExecuteAction(CompilerInstance &CI) override;
bool BeginSourceFileAction(CompilerInstance &CI) override;

std::unique_ptr<raw_pwrite_stream>
CreateOutputFile(CompilerInstance &CI, StringRef InFile) override;
};

class SyntaxOnlyAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

public:
~SyntaxOnlyAction() override;
bool hasCodeCompletionSupport() const override { return true; }
};



class DumpModuleInfoAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
bool BeginInvocation(CompilerInstance &CI) override;
void ExecuteAction() override;

public:
bool hasPCHSupport() const override { return false; }
bool hasASTFileSupport() const override { return true; }
bool hasIRSupport() const override { return false; }
bool hasCodeCompletionSupport() const override { return false; }
};

class VerifyPCHAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

void ExecuteAction() override;

public:
bool hasCodeCompletionSupport() const override { return false; }
};

class TemplightDumpAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

void ExecuteAction() override;
};









class ASTMergeAction : public FrontendAction {

std::unique_ptr<FrontendAction> AdaptedAction;


std::vector<std::string> ASTFiles;

protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

bool BeginSourceFileAction(CompilerInstance &CI) override;

void ExecuteAction() override;
void EndSourceFileAction() override;

public:
ASTMergeAction(std::unique_ptr<FrontendAction> AdaptedAction,
ArrayRef<std::string> ASTFiles);
~ASTMergeAction() override;

bool usesPreprocessorOnly() const override;
TranslationUnitKind getTranslationUnitKind() override;
bool hasPCHSupport() const override;
bool hasASTFileSupport() const override;
bool hasCodeCompletionSupport() const override;
};

class PrintPreambleAction : public FrontendAction {
protected:
void ExecuteAction() override;
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &,
StringRef) override {
return nullptr;
}

bool usesPreprocessorOnly() const override { return true; }
};

class PrintDependencyDirectivesSourceMinimizerAction : public FrontendAction {
protected:
void ExecuteAction() override;
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &,
StringRef) override {
return nullptr;
}

bool usesPreprocessorOnly() const override { return true; }
};





class DumpRawTokensAction : public PreprocessorFrontendAction {
protected:
void ExecuteAction() override;
};

class DumpTokensAction : public PreprocessorFrontendAction {
protected:
void ExecuteAction() override;
};

class PreprocessOnlyAction : public PreprocessorFrontendAction {
protected:
void ExecuteAction() override;
};

class PrintPreprocessedAction : public PreprocessorFrontendAction {
protected:
void ExecuteAction() override;

bool hasPCHSupport() const override { return true; }
};

}

#endif
