







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AVR_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AVR_H

#include "Gnu.h"
#include "clang/Driver/InputInfo.h"
#include "clang/Driver/ToolChain.h"
#include "clang/Driver/Tool.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY AVRToolChain : public Generic_ELF {
public:
AVRToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

protected:
Tool *buildLinker() const override;

private:




bool LinkStdlib;

llvm::Optional<std::string> findAVRLibcInstallation() const;
};

}

namespace tools {
namespace AVR {
class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const llvm::Triple &Triple, const ToolChain &TC, bool LinkStdlib)
: Tool("AVR::Linker", "avr-ld", TC), Triple(Triple),
LinkStdlib(LinkStdlib) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;

protected:
const llvm::Triple &Triple;
bool LinkStdlib;
};
}
}
}
}

#endif
