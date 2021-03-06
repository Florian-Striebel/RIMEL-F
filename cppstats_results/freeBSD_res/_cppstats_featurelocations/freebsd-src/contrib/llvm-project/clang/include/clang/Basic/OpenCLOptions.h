












#if !defined(LLVM_CLANG_BASIC_OPENCLOPTIONS_H)
#define LLVM_CLANG_BASIC_OPENCLOPTIONS_H

#include "clang/Basic/LangOptions.h"
#include "llvm/ADT/StringMap.h"

namespace clang {

class DiagnosticsEngine;
class TargetInfo;

namespace {



enum OpenCLVersionID : unsigned int {
OCL_C_10 = 0x1,
OCL_C_11 = 0x2,
OCL_C_12 = 0x4,
OCL_C_20 = 0x8,
OCL_C_30 = 0x10,
OCL_C_ALL = 0x1f,
OCL_C_11P = OCL_C_ALL ^ OCL_C_10,
OCL_C_12P = OCL_C_ALL ^ (OCL_C_10 | OCL_C_11),
};

static inline OpenCLVersionID encodeOpenCLVersion(unsigned OpenCLVersion) {
switch (OpenCLVersion) {
default:
llvm_unreachable("Unknown OpenCL version code");
case 100:
return OCL_C_10;
case 110:
return OCL_C_11;
case 120:
return OCL_C_12;
case 200:
return OCL_C_20;
case 300:
return OCL_C_30;
}
}



static inline bool isOpenCLVersionContainedInMask(const LangOptions &LO,
unsigned Mask) {
auto CLVer = LO.OpenCLCPlusPlus ? 200 : LO.OpenCLVersion;
OpenCLVersionID Code = encodeOpenCLVersion(CLVer);
return Mask & Code;
}

}


class OpenCLOptions {

public:









bool areProgramScopeVariablesSupported(const LangOptions &Opts) const {
return Opts.OpenCLCPlusPlus || Opts.OpenCLVersion == 200 ||
(Opts.OpenCLVersion == 300 &&
isSupported("__opencl_c_program_scope_global_variables", Opts));
}

struct OpenCLOptionInfo {

bool WithPragma = false;


unsigned Avail = 100U;


unsigned Core = 0U;


unsigned Opt = 0U;


bool Supported = false;


bool Enabled = false;

OpenCLOptionInfo() = default;
OpenCLOptionInfo(bool Pragma, unsigned AvailV, unsigned CoreV,
unsigned OptV)
: WithPragma(Pragma), Avail(AvailV), Core(CoreV), Opt(OptV) {}

bool isCore() const { return Core != 0U; }

bool isOptionalCore() const { return Opt != 0U; }


bool isAvailableIn(const LangOptions &LO) const {

auto CLVer = LO.OpenCLCPlusPlus ? 200 : LO.OpenCLVersion;
return CLVer >= Avail;
}


bool isCoreIn(const LangOptions &LO) const {
return isAvailableIn(LO) && isOpenCLVersionContainedInMask(LO, Core);
}


bool isOptionalCoreIn(const LangOptions &LO) const {
return isAvailableIn(LO) && isOpenCLVersionContainedInMask(LO, Opt);
}
};

bool isKnown(llvm::StringRef Ext) const;




bool isAvailableOption(llvm::StringRef Ext, const LangOptions &LO) const;

bool isWithPragma(llvm::StringRef Ext) const;



bool isSupported(llvm::StringRef Ext, const LangOptions &LO) const;



bool isSupportedCore(llvm::StringRef Ext, const LangOptions &LO) const;



bool isSupportedOptionalCore(llvm::StringRef Ext,
const LangOptions &LO) const;



bool isSupportedCoreOrOptionalCore(llvm::StringRef Ext,
const LangOptions &LO) const;



bool isSupportedExtension(llvm::StringRef Ext, const LangOptions &LO) const;




void acceptsPragma(llvm::StringRef Ext, bool V = true);

void enable(llvm::StringRef Ext, bool V = true);




void support(llvm::StringRef Ext, bool V = true);

OpenCLOptions();


void addSupport(const llvm::StringMap<bool> &FeaturesMap,
const LangOptions &Opts);


void disableAll();

friend class ASTWriter;
friend class ASTReader;

using OpenCLOptionInfoMap = llvm::StringMap<OpenCLOptionInfo>;

template <typename... Args>
static bool isOpenCLOptionCoreIn(const LangOptions &LO, Args &&... args) {
return OpenCLOptionInfo(std::forward<Args>(args)...).isCoreIn(LO);
}

template <typename... Args>
static bool isOpenCLOptionAvailableIn(const LangOptions &LO,
Args &&... args) {
return OpenCLOptionInfo(std::forward<Args>(args)...).isAvailableIn(LO);
}



static bool diagnoseUnsupportedFeatureDependencies(const TargetInfo &TI,
DiagnosticsEngine &Diags);



static bool diagnoseFeatureExtensionDifferences(const TargetInfo &TI,
DiagnosticsEngine &Diags);

private:

bool isEnabled(llvm::StringRef Ext) const;

OpenCLOptionInfoMap OptMap;
};

}

#endif
