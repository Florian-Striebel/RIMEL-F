







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ROCM_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ROCM_H

#include "clang/Basic/Cuda.h"
#include "clang/Basic/LLVM.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Options.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/VersionTuple.h"

namespace clang {
namespace driver {



class RocmInstallationDetector {
private:
struct ConditionalLibrary {
SmallString<0> On;
SmallString<0> Off;

bool isValid() const { return !On.empty() && !Off.empty(); }

StringRef get(bool Enabled) const {
assert(isValid());
return Enabled ? On : Off;
}
};


struct Candidate {
llvm::SmallString<0> Path;
bool StrictChecking;



std::string SPACKReleaseStr;

bool isSPACK() const { return !SPACKReleaseStr.empty(); }
Candidate(std::string Path, bool StrictChecking = false,
StringRef SPACKReleaseStr = {})
: Path(Path), StrictChecking(StrictChecking),
SPACKReleaseStr(SPACKReleaseStr.str()) {}
};

const Driver &D;
bool HasHIPRuntime = false;
bool HasDeviceLibrary = false;


const unsigned DefaultVersionMajor = 3;
const unsigned DefaultVersionMinor = 5;
const char *DefaultVersionPatch = "0";


std::string DetectedVersion;

llvm::VersionTuple VersionMajorMinor;

std::string VersionPatch;


StringRef RocmPathArg;

std::vector<std::string> RocmDeviceLibPathArg;

StringRef HIPPathArg;

StringRef HIPVersionArg;

bool NoBuiltinLibs = false;


SmallString<0> InstallPath;
SmallString<0> BinPath;
SmallString<0> LibPath;
SmallString<0> LibDevicePath;
SmallString<0> IncludePath;
llvm::StringMap<std::string> LibDeviceMap;


SmallString<0> OCML;
SmallString<0> OCKL;


SmallString<0> OpenCL;
SmallString<0> HIP;


SmallString<0> AsanRTL;


ConditionalLibrary WavefrontSize64;
ConditionalLibrary FiniteOnly;
ConditionalLibrary UnsafeMath;
ConditionalLibrary DenormalsAreZero;
ConditionalLibrary CorrectlyRoundedSqrt;


SmallVector<Candidate, 4> ROCmSearchDirs;
bool PrintROCmSearchDirs;
bool Verbose;

bool allGenericLibsValid() const {
return !OCML.empty() && !OCKL.empty() && !OpenCL.empty() && !HIP.empty() &&
WavefrontSize64.isValid() && FiniteOnly.isValid() &&
UnsafeMath.isValid() && DenormalsAreZero.isValid() &&
CorrectlyRoundedSqrt.isValid();
}

void scanLibDevicePath(llvm::StringRef Path);
bool parseHIPVersionFile(llvm::StringRef V);
const SmallVectorImpl<Candidate> &getInstallationPathCandidates();





llvm::SmallString<0> findSPACKPackage(const Candidate &Cand,
StringRef PackageName);

public:
RocmInstallationDetector(const Driver &D, const llvm::Triple &HostTriple,
const llvm::opt::ArgList &Args,
bool DetectHIPRuntime = true,
bool DetectDeviceLib = false);



llvm::SmallVector<std::string, 12>
getCommonBitcodeLibs(const llvm::opt::ArgList &DriverArgs,
StringRef LibDeviceFile, bool Wave64, bool DAZ,
bool FiniteOnly, bool UnsafeMathOpt,
bool FastRelaxedMath, bool CorrectSqrt) const;


bool hasHIPRuntime() const { return HasHIPRuntime; }


bool hasDeviceLibrary() const { return HasDeviceLibrary; }


void print(raw_ostream &OS) const;





StringRef getInstallPath() const { return InstallPath; }





StringRef getIncludePath() const { return IncludePath; }


StringRef getLibPath() const { return LibPath; }


StringRef getLibDevicePath() const { return LibDevicePath; }

StringRef getOCMLPath() const {
assert(!OCML.empty());
return OCML;
}

StringRef getOCKLPath() const {
assert(!OCKL.empty());
return OCKL;
}

StringRef getOpenCLPath() const {
assert(!OpenCL.empty());
return OpenCL;
}

StringRef getHIPPath() const {
assert(!HIP.empty());
return HIP;
}


StringRef getAsanRTLPath() const { return AsanRTL; }

StringRef getWavefrontSize64Path(bool Enabled) const {
return WavefrontSize64.get(Enabled);
}

StringRef getFiniteOnlyPath(bool Enabled) const {
return FiniteOnly.get(Enabled);
}

StringRef getUnsafeMathPath(bool Enabled) const {
return UnsafeMath.get(Enabled);
}

StringRef getDenormalsAreZeroPath(bool Enabled) const {
return DenormalsAreZero.get(Enabled);
}

StringRef getCorrectlyRoundedSqrtPath(bool Enabled) const {
return CorrectlyRoundedSqrt.get(Enabled);
}


std::string getLibDeviceFile(StringRef Gpu) const {
return LibDeviceMap.lookup(Gpu);
}

void AddHIPIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;

void detectDeviceLibrary();
void detectHIPRuntime();


std::vector<std::string> getRocmDeviceLibPathArg() const {
return RocmDeviceLibPathArg;
}


StringRef getRocmPathArg() const { return RocmPathArg; }


StringRef getHIPVersionArg() const { return HIPVersionArg; }

std::string getHIPVersion() const { return DetectedVersion; }
};

}
}

#endif
