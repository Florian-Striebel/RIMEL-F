







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_TCE_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_TCE_H

#include "clang/Driver/Driver.h"
#include "clang/Driver/ToolChain.h"
#include <set>

namespace clang {
namespace driver {
namespace toolchains {



class LLVM_LIBRARY_VISIBILITY TCEToolChain : public ToolChain {
public:
TCEToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
~TCEToolChain() override;

bool IsMathErrnoDefault() const override;
bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;
};


class LLVM_LIBRARY_VISIBILITY TCELEToolChain : public TCEToolChain {
public:
TCELEToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
~TCELEToolChain() override;
};

}
}
}

#endif
