







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_PPC_LINUX_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_PPC_LINUX_H

#include "Linux.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY PPCLinuxToolChain : public Linux {
public:
PPCLinuxToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args)
: Linux(D, Triple, Args) {}

void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
};

}
}
}

#endif
