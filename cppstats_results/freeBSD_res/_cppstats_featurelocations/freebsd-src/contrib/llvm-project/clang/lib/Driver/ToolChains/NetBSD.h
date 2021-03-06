







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_NETBSD_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_NETBSD_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {


namespace netbsd {
class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC) : Tool("netbsd::Assembler", "assembler", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("netbsd::Linker", "linker", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};
}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY NetBSD : public Generic_ELF {
public:
NetBSD(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

bool IsMathErrnoDefault() const override { return false; }
bool IsObjCNonFragileABIDefault() const override { return true; }

CXXStdlibType GetDefaultCXXStdlibType() const override;

void addLibCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void addLibStdCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

bool IsUnwindTablesDefault(const llvm::opt::ArgList &Args) const override {
return true;
}

llvm::ExceptionHandling GetExceptionModel(
const llvm::opt::ArgList &Args) const override;

SanitizerMask getSupportedSanitizers() const override;

void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;
};

}
}
}

#endif
