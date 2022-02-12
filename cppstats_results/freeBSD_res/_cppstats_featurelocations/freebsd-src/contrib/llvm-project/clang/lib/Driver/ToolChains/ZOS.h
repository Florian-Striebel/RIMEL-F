







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ZOS_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ZOS_H

#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY ZOS : public ToolChain {
public:
ZOS(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
~ZOS() override;

bool isPICDefault() const override { return false; }
bool isPIEDefault() const override { return false; }
bool isPICDefaultForced() const override { return false; }

bool IsIntegratedAssemblerDefault() const override { return true; }

void addClangTargetOptions(
const llvm::opt::ArgList &DriverArgs, llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadingKind) const override;
};

}
}
}

#endif
