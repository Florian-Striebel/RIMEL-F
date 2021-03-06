







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_DRAGONFLY_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_DRAGONFLY_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

namespace dragonfly {
class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC)
: Tool("dragonfly::Assembler", "assembler", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("dragonfly::Linker", "linker", TC) {}

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

class LLVM_LIBRARY_VISIBILITY DragonFly : public Generic_ELF {
public:
DragonFly(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

bool IsMathErrnoDefault() const override { return false; }

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;
};

}
}
}

#endif
