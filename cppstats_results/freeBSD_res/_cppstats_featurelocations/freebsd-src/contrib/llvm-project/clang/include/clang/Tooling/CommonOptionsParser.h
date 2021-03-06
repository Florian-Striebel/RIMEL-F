
























#if !defined(LLVM_CLANG_TOOLING_COMMONOPTIONSPARSER_H)
#define LLVM_CLANG_TOOLING_COMMONOPTIONSPARSER_H

#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace tooling {





























class CommonOptionsParser {

protected:








CommonOptionsParser(
int &argc, const char **argv, llvm::cl::OptionCategory &Category,
llvm::cl::NumOccurrencesFlag OccurrencesFlag = llvm::cl::OneOrMore,
const char *Overview = nullptr);

public:


static llvm::Expected<CommonOptionsParser>
create(int &argc, const char **argv, llvm::cl::OptionCategory &Category,
llvm::cl::NumOccurrencesFlag OccurrencesFlag = llvm::cl::OneOrMore,
const char *Overview = nullptr);


CompilationDatabase &getCompilations() {
return *Compilations;
}


const std::vector<std::string> &getSourcePathList() const {
return SourcePathList;
}



ArgumentsAdjuster getArgumentsAdjuster() { return Adjuster; }

static const char *const HelpMessage;

private:
CommonOptionsParser() = default;

llvm::Error init(int &argc, const char **argv,
llvm::cl::OptionCategory &Category,
llvm::cl::NumOccurrencesFlag OccurrencesFlag,
const char *Overview);

std::unique_ptr<CompilationDatabase> Compilations;
std::vector<std::string> SourcePathList;
ArgumentsAdjuster Adjuster;
};

class ArgumentsAdjustingCompilations : public CompilationDatabase {
public:
ArgumentsAdjustingCompilations(
std::unique_ptr<CompilationDatabase> Compilations)
: Compilations(std::move(Compilations)) {}

void appendArgumentsAdjuster(ArgumentsAdjuster Adjuster);

std::vector<CompileCommand>
getCompileCommands(StringRef FilePath) const override;

std::vector<std::string> getAllFiles() const override;

std::vector<CompileCommand> getAllCompileCommands() const override;

private:
std::unique_ptr<CompilationDatabase> Compilations;
std::vector<ArgumentsAdjuster> Adjusters;

std::vector<CompileCommand>
adjustCommands(std::vector<CompileCommand> Commands) const;
};

}
}

#endif
