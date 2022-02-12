







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_SOLARIS_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_SOLARIS_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {


namespace solaris {
class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC)
: Tool("solaris::Assembler", "assembler", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("solaris::Linker", "linker", TC) {}

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

class LLVM_LIBRARY_VISIBILITY Solaris : public Generic_ELF {
public:
Solaris(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void
addLibStdCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

SanitizerMask getSupportedSanitizers() const override;
unsigned GetDefaultDwarfVersion() const override { return 2; }

const char *getDefaultLinker() const override {

return "/usr/bin/ld";
}

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;
};

}
}
}

#endif
