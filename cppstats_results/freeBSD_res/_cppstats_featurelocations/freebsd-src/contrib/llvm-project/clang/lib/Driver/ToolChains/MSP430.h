







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_MSP430_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_MSP430_H

#include "Gnu.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/InputInfo.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"

#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY MSP430ToolChain : public Generic_ELF {
public:
MSP430ToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind) const override;

bool isPICDefault() const override { return false; }
bool isPIEDefault() const override { return false; }
bool isPICDefaultForced() const override { return true; }

UnwindLibType
GetUnwindLibType(const llvm::opt::ArgList &Args) const override {
return UNW_None;
}

protected:
Tool *buildLinker() const override;

private:
std::string computeSysRoot() const override;
};

}

namespace tools {
namespace msp430 {

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("MSP430::Linker", "msp430-elf-ld", TC) {}
bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;

private:
void AddStartFiles(bool UseExceptions, const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;
void AddDefaultLibs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;
void AddEndFiles(bool UseExceptions, const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;
};

void getMSP430TargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features);
}
}
}
}

#endif
