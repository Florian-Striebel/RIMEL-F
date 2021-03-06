







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AIX_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AIX_H

#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {


namespace aix {

class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC) : Tool("aix::Assembler", "assembler", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("aix::Linker", "linker", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

}

}
}
}

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY AIX : public ToolChain {
public:
AIX(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

bool parseInlineAsmUsingAsmParser() const override {
return ParseInlineAsmUsingAsmParser;
}
bool isPICDefault() const override { return true; }
bool isPIEDefault() const override { return false; }
bool isPICDefaultForced() const override { return true; }

void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

CXXStdlibType GetDefaultCXXStdlibType() const override;

RuntimeLibType GetDefaultRuntimeLibType() const override;


unsigned GetDefaultDwarfVersion() const override { return 3; }

llvm::DebuggerKind getDefaultDebuggerTuning() const override {
return llvm::DebuggerKind::DBX;
}

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;

private:
llvm::StringRef GetHeaderSysroot(const llvm::opt::ArgList &DriverArgs) const;
bool ParseInlineAsmUsingAsmParser;
};

}
}
}

#endif
