







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CONTIKI_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CONTIKI_H

#include "Gnu.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Contiki : public Generic_ELF {
public:
Contiki(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);


void addLibCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override {}
void addLibStdCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override {}

SanitizerMask getSupportedSanitizers() const override;
};

}
}
}

#endif
