







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_HAIKU_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_HAIKU_H

#include "Gnu.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Haiku : public Generic_ELF {
public:
Haiku(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

bool isPIEDefault() const override {
return getTriple().getArch() == llvm::Triple::x86_64;
}

void addLibCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void addLibStdCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
};

}
}
}

#endif
