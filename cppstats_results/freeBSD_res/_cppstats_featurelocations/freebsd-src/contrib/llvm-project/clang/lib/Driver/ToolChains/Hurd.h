







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_Hurd_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_Hurd_H

#include "Gnu.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Hurd : public Generic_ELF {
public:
Hurd(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

bool HasNativeLLVMSupport() const override;

void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void
addLibStdCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

std::string getDynamicLinker(const llvm::opt::ArgList &Args) const override;

void addExtraOpts(llvm::opt::ArgStringList &CmdArgs) const override;

std::vector<std::string> ExtraOpts;

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;

std::string getMultiarchTriple(const Driver &D,
const llvm::Triple &TargetTriple,
StringRef SysRoot) const override;
};

}
}
}

#endif
