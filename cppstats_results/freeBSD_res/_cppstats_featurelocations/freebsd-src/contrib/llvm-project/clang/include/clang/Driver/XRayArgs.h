






#if !defined(LLVM_CLANG_DRIVER_XRAYARGS_H)
#define LLVM_CLANG_DRIVER_XRAYARGS_H

#include "clang/Basic/XRayInstr.h"
#include "clang/Driver/Types.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace clang {
namespace driver {

class ToolChain;

class XRayArgs {
std::vector<std::string> AlwaysInstrumentFiles;
std::vector<std::string> NeverInstrumentFiles;
std::vector<std::string> AttrListFiles;
std::vector<std::string> ExtraDeps;
std::vector<std::string> Modes;
XRayInstrSet InstrumentationBundle;
bool XRayInstrument = false;
int InstructionThreshold = 200;
bool XRayAlwaysEmitCustomEvents = false;
bool XRayAlwaysEmitTypedEvents = false;
bool XRayRT = true;
bool XRayIgnoreLoops = false;
bool XRayFunctionIndex;
int XRayFunctionGroups = 1;
int XRaySelectedFunctionGroup = 0;

public:

XRayArgs(const ToolChain &TC, const llvm::opt::ArgList &Args);
void addArgs(const ToolChain &TC, const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs, types::ID InputType) const;

bool needsXRayRt() const { return XRayInstrument && XRayRT; }
llvm::ArrayRef<std::string> modeList() const { return Modes; }
XRayInstrSet instrumentationBundle() const { return InstrumentationBundle; }
};

}
}

#endif
