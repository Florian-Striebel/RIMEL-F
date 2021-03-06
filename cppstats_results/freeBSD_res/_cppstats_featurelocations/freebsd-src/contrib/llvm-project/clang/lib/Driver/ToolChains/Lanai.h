







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_LANAI_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_LANAI_H

#include "Gnu.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY LanaiToolChain : public Generic_ELF {
public:
LanaiToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args)
: Generic_ELF(D, Triple, Args) {}


void addLibCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override {}
void addLibStdCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override {}

bool IsIntegratedAssemblerDefault() const override { return true; }
};

}
}
}

#endif
