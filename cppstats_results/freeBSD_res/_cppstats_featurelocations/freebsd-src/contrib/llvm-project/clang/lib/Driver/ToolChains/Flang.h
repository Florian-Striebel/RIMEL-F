







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_FLANG_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_FLANG_H

#include "clang/Driver/Tool.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/ToolChain.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace driver {

namespace tools {


class LLVM_LIBRARY_VISIBILITY Flang : public Tool {
private:





void AddFortranDialectOptions(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;






void AddPreprocessingOptions(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;





void AddOtherOptions(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;

public:
Flang(const ToolChain &TC);
~Flang() override;

bool hasGoodDiagnostics() const override { return true; }
bool hasIntegratedAssembler() const override { return true; }
bool hasIntegratedCPP() const override { return true; }
bool canEmitIR() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

}

}
}

#endif
