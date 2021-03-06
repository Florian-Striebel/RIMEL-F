












#if !defined(LLVM_CLANG_BASIC_TARGETOPTIONS_H)
#define LLVM_CLANG_BASIC_TARGETOPTIONS_H

#include "clang/Basic/OpenCLOptions.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Target/TargetOptions.h"
#include <string>
#include <vector>

namespace clang {


class TargetOptions {
public:

std::string Triple;



std::string HostTriple;


std::string CPU;


std::string TuneCPU;


std::string FPMath;


std::string ABI;


llvm::EABI EABIVersion;


std::string LinkerVersion;


std::vector<std::string> FeaturesAsWritten;



std::vector<std::string> Features;



llvm::StringMap<bool> FeatureMap;


llvm::StringMap<bool> OpenCLFeaturesMap;



std::vector<std::string> OpenCLExtensionsAsWritten;


bool ForceEnableInt128 = false;



bool NVPTXUseShortPointers = false;


bool AllowAMDGPUUnsafeFPAtomics = false;





std::string CodeModel;







llvm::VersionTuple SDKVersion;
};

}

#endif
