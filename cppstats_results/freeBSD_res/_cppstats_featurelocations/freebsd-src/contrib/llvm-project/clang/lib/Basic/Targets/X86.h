











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_X86_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_X86_H

#include "OSTargets.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/X86TargetParser.h"

namespace clang {
namespace targets {

static const unsigned X86AddrSpaceMap[] = {
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
270,
271,
272
};



class LLVM_LIBRARY_VISIBILITY X86TargetInfo : public TargetInfo {

enum X86SSEEnum {
NoSSE,
SSE1,
SSE2,
SSE3,
SSSE3,
SSE41,
SSE42,
AVX,
AVX2,
AVX512F
} SSELevel = NoSSE;
enum MMX3DNowEnum {
NoMMX3DNow,
MMX,
AMD3DNow,
AMD3DNowAthlon
} MMX3DNowLevel = NoMMX3DNow;
enum XOPEnum { NoXOP, SSE4A, FMA4, XOP } XOPLevel = NoXOP;
enum AddrSpace { ptr32_sptr = 270, ptr32_uptr = 271, ptr64 = 272 };

bool HasAES = false;
bool HasVAES = false;
bool HasPCLMUL = false;
bool HasVPCLMULQDQ = false;
bool HasGFNI = false;
bool HasLZCNT = false;
bool HasRDRND = false;
bool HasFSGSBASE = false;
bool HasBMI = false;
bool HasBMI2 = false;
bool HasPOPCNT = false;
bool HasRTM = false;
bool HasPRFCHW = false;
bool HasRDSEED = false;
bool HasADX = false;
bool HasTBM = false;
bool HasLWP = false;
bool HasFMA = false;
bool HasF16C = false;
bool HasAVX512CD = false;
bool HasAVX512VPOPCNTDQ = false;
bool HasAVX512VNNI = false;
bool HasAVX512BF16 = false;
bool HasAVX512ER = false;
bool HasAVX512PF = false;
bool HasAVX512DQ = false;
bool HasAVX512BITALG = false;
bool HasAVX512BW = false;
bool HasAVX512VL = false;
bool HasAVX512VBMI = false;
bool HasAVX512VBMI2 = false;
bool HasAVX512IFMA = false;
bool HasAVX512VP2INTERSECT = false;
bool HasSHA = false;
bool HasSHSTK = false;
bool HasSGX = false;
bool HasCX8 = false;
bool HasCX16 = false;
bool HasFXSR = false;
bool HasXSAVE = false;
bool HasXSAVEOPT = false;
bool HasXSAVEC = false;
bool HasXSAVES = false;
bool HasMWAITX = false;
bool HasCLZERO = false;
bool HasCLDEMOTE = false;
bool HasPCONFIG = false;
bool HasPKU = false;
bool HasCLFLUSHOPT = false;
bool HasCLWB = false;
bool HasMOVBE = false;
bool HasPREFETCHWT1 = false;
bool HasRDPID = false;
bool HasRetpolineExternalThunk = false;
bool HasLAHFSAHF = false;
bool HasWBNOINVD = false;
bool HasWAITPKG = false;
bool HasMOVDIRI = false;
bool HasMOVDIR64B = false;
bool HasPTWRITE = false;
bool HasINVPCID = false;
bool HasENQCMD = false;
bool HasKL = false;
bool HasWIDEKL = false;
bool HasHRESET = false;
bool HasAVXVNNI = false;
bool HasAMXTILE = false;
bool HasAMXINT8 = false;
bool HasAMXBF16 = false;
bool HasSERIALIZE = false;
bool HasTSXLDTRK = false;
bool HasUINTR = false;

protected:
llvm::X86::CPUKind CPU = llvm::X86::CK_None;

enum FPMathKind { FP_Default, FP_SSE, FP_387 } FPMath = FP_Default;

public:
X86TargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple) {
LongDoubleFormat = &llvm::APFloat::x87DoubleExtended();
AddrSpaceMap = &X86AddrSpaceMap;
HasStrictFP = true;

bool IsWinCOFF =
getTriple().isOSWindows() && getTriple().isOSBinFormatCOFF();
if (IsWinCOFF)
MaxVectorAlign = MaxTLSAlign = 8192u * getCharWidth();
}

const char *getLongDoubleMangling() const override {
return LongDoubleFormat == &llvm::APFloat::IEEEquad() ? "g" : "e";
}

unsigned getFloatEvalMethod() const override {

return SSELevel == NoSSE ? 2 : 0;
}

ArrayRef<const char *> getGCCRegNames() const override;

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
return None;
}

ArrayRef<TargetInfo::AddlRegName> getGCCAddlRegNames() const override;

bool isSPRegName(StringRef RegName) const override {
return RegName.equals("esp") || RegName.equals("rsp");
}

bool validateCpuSupports(StringRef Name) const override;

bool validateCpuIs(StringRef Name) const override;

bool validateCPUSpecificCPUDispatch(StringRef Name) const override;

char CPUSpecificManglingCharacter(StringRef Name) const override;

void getCPUSpecificCPUDispatchFeatures(
StringRef Name,
llvm::SmallVectorImpl<StringRef> &Features) const override;

Optional<unsigned> getCPUCacheLineSize() const override;

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &info) const override;

bool validateGlobalRegisterVariable(StringRef RegName, unsigned RegSize,
bool &HasSizeMismatch) const override {


if (RegName.equals("esp") || RegName.equals("ebp")) {

HasSizeMismatch = RegSize != 32;
return true;
}

return false;
}

bool validateOutputSize(const llvm::StringMap<bool> &FeatureMap,
StringRef Constraint, unsigned Size) const override;

bool validateInputSize(const llvm::StringMap<bool> &FeatureMap,
StringRef Constraint, unsigned Size) const override;

virtual bool
checkCFProtectionReturnSupported(DiagnosticsEngine &Diags) const override {
return true;
};

virtual bool
checkCFProtectionBranchSupported(DiagnosticsEngine &Diags) const override {
return true;
};

virtual bool validateOperandSize(const llvm::StringMap<bool> &FeatureMap,
StringRef Constraint, unsigned Size) const;

std::string convertConstraint(const char *&Constraint) const override;
const char *getClobbers() const override {
return "~{dirflag},~{fpsr},~{flags}";
}

StringRef getConstraintRegister(StringRef Constraint,
StringRef Expression) const override {
StringRef::iterator I, E;
for (I = Constraint.begin(), E = Constraint.end(); I != E; ++I) {
if (isalpha(*I) || *I == '@')
break;
}
if (I == E)
return "";
switch (*I) {

case 'a':
return "ax";
case 'b':
return "bx";
case 'c':
return "cx";
case 'd':
return "dx";
case 'S':
return "si";
case 'D':
return "di";

case 'r':
return Expression;

case 'Y':
if ((++I != E) && ((*I == '0') || (*I == 'z')))
return "xmm0";
break;
default:
break;
}
return "";
}

bool useFP16ConversionIntrinsics() const override {
return false;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

void setFeatureEnabled(llvm::StringMap<bool> &Features, StringRef Name,
bool Enabled) const final;

bool
initFeatureMap(llvm::StringMap<bool> &Features, DiagnosticsEngine &Diags,
StringRef CPU,
const std::vector<std::string> &FeaturesVec) const override;

bool isValidFeatureName(StringRef Name) const override;

bool hasFeature(StringRef Feature) const final;

bool handleTargetFeatures(std::vector<std::string> &Features,
DiagnosticsEngine &Diags) override;

StringRef getABI() const override {
if (getTriple().getArch() == llvm::Triple::x86_64 && SSELevel >= AVX512F)
return "avx512";
if (getTriple().getArch() == llvm::Triple::x86_64 && SSELevel >= AVX)
return "avx";
if (getTriple().getArch() == llvm::Triple::x86 &&
MMX3DNowLevel == NoMMX3DNow)
return "no-mmx";
return "";
}

bool supportsTargetAttributeTune() const override {
return true;
}

bool isValidCPUName(StringRef Name) const override {
bool Only64Bit = getTriple().getArch() != llvm::Triple::x86;
return llvm::X86::parseArchX86(Name, Only64Bit) != llvm::X86::CK_None;
}

bool isValidTuneCPUName(StringRef Name) const override {
if (Name == "generic")
return true;




return llvm::X86::parseTuneCPU(Name) != llvm::X86::CK_None;
}

void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;
void fillValidTuneCPUList(SmallVectorImpl<StringRef> &Values) const override;

bool setCPU(const std::string &Name) override {
bool Only64Bit = getTriple().getArch() != llvm::Triple::x86;
CPU = llvm::X86::parseArchX86(Name, Only64Bit);
return CPU != llvm::X86::CK_None;
}

unsigned multiVersionSortPriority(StringRef Name) const override;

bool setFPMath(StringRef Name) override;

bool supportsExtendIntArgs() const override {
return getTriple().getArch() != llvm::Triple::x86;
}

CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {

switch (CC) {
case CC_X86ThisCall:
case CC_X86FastCall:
case CC_X86StdCall:
case CC_X86VectorCall:
case CC_X86RegCall:
case CC_C:
case CC_PreserveMost:
case CC_Swift:
case CC_X86Pascal:
case CC_IntelOclBicc:
case CC_OpenCLKernel:
return CCCR_OK;
case CC_SwiftAsync:
return CCCR_Error;
default:
return CCCR_Warning;
}
}

bool checkArithmeticFenceSupported() const override { return true; }

CallingConv getDefaultCallingConv() const override {
return CC_C;
}

bool hasSjLjLowering() const override { return true; }

void setSupportedOpenCLOpts() override { supportAllOpenCLOpts(); }

uint64_t getPointerWidthV(unsigned AddrSpace) const override {
if (AddrSpace == ptr32_sptr || AddrSpace == ptr32_uptr)
return 32;
if (AddrSpace == ptr64)
return 64;
return PointerWidth;
}

uint64_t getPointerAlignV(unsigned AddrSpace) const override {
return getPointerWidthV(AddrSpace);
}
};


class LLVM_LIBRARY_VISIBILITY X86_32TargetInfo : public X86TargetInfo {
public:
X86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: X86TargetInfo(Triple, Opts) {
DoubleAlign = LongLongAlign = 32;
LongDoubleWidth = 96;
LongDoubleAlign = 32;
SuitableAlign = 128;
resetDataLayout(
Triple.isOSBinFormatMachO()
? "e-m:o-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-"
"f80:32-n8:16:32-S128"
: "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-"
"f80:32-n8:16:32-S128",
Triple.isOSBinFormatMachO() ? "_" : "");
SizeType = UnsignedInt;
PtrDiffType = SignedInt;
IntPtrType = SignedInt;
RegParmMax = 3;


RealTypeUsesObjCFPRet =
((1 << TargetInfo::Float) | (1 << TargetInfo::Double) |
(1 << TargetInfo::LongDouble));


MaxAtomicPromoteWidth = 64;
MaxAtomicInlineWidth = 32;
}

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::CharPtrBuiltinVaList;
}

int getEHDataRegisterNumber(unsigned RegNo) const override {
if (RegNo == 0)
return 0;
if (RegNo == 1)
return 2;
return -1;
}

bool validateOperandSize(const llvm::StringMap<bool> &FeatureMap,
StringRef Constraint, unsigned Size) const override {
switch (Constraint[0]) {
default:
break;
case 'R':
case 'q':
case 'Q':
case 'a':
case 'b':
case 'c':
case 'd':
case 'S':
case 'D':
return Size <= 32;
case 'A':
return Size <= 64;
}

return X86TargetInfo::validateOperandSize(FeatureMap, Constraint, Size);
}

void setMaxAtomicWidth() override {
if (hasFeature("cx8"))
MaxAtomicInlineWidth = 64;
}

ArrayRef<Builtin::Info> getTargetBuiltins() const override;

bool hasExtIntType() const override { return true; }
};

class LLVM_LIBRARY_VISIBILITY NetBSDI386TargetInfo
: public NetBSDTargetInfo<X86_32TargetInfo> {
public:
NetBSDI386TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: NetBSDTargetInfo<X86_32TargetInfo>(Triple, Opts) {}

unsigned getFloatEvalMethod() const override {
unsigned Major, Minor, Micro;
getTriple().getOSVersion(Major, Minor, Micro);

if (Major >= 7 || (Major == 6 && Minor == 99 && Micro >= 26) || Major == 0)
return X86_32TargetInfo::getFloatEvalMethod();

return 1;
}
};

class LLVM_LIBRARY_VISIBILITY OpenBSDI386TargetInfo
: public OpenBSDTargetInfo<X86_32TargetInfo> {
public:
OpenBSDI386TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OpenBSDTargetInfo<X86_32TargetInfo>(Triple, Opts) {
SizeType = UnsignedLong;
IntPtrType = SignedLong;
PtrDiffType = SignedLong;
}
};

class LLVM_LIBRARY_VISIBILITY DarwinI386TargetInfo
: public DarwinTargetInfo<X86_32TargetInfo> {
public:
DarwinI386TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: DarwinTargetInfo<X86_32TargetInfo>(Triple, Opts) {
LongDoubleWidth = 128;
LongDoubleAlign = 128;
SuitableAlign = 128;
MaxVectorAlign = 256;

llvm::Triple T = llvm::Triple(Triple);
if (T.isWatchOS())
UseSignedCharForObjCBool = false;
SizeType = UnsignedLong;
IntPtrType = SignedLong;
resetDataLayout("e-m:o-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-"
"f80:128-n8:16:32-S128", "_");
HasAlignMac68kSupport = true;
}

bool handleTargetFeatures(std::vector<std::string> &Features,
DiagnosticsEngine &Diags) override {
if (!DarwinTargetInfo<X86_32TargetInfo>::handleTargetFeatures(Features,
Diags))
return false;

MaxVectorAlign =
hasFeature("avx512f") ? 512 : hasFeature("avx") ? 256 : 128;
return true;
}
};


class LLVM_LIBRARY_VISIBILITY WindowsX86_32TargetInfo
: public WindowsTargetInfo<X86_32TargetInfo> {
public:
WindowsX86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: WindowsTargetInfo<X86_32TargetInfo>(Triple, Opts) {
DoubleAlign = LongLongAlign = 64;
bool IsWinCOFF =
getTriple().isOSWindows() && getTriple().isOSBinFormatCOFF();
resetDataLayout(IsWinCOFF ? "e-m:x-p:32:32-p270:32:32-p271:32:32-p272:64:"
"64-i64:64-f80:32-n8:16:32-a:0:32-S32"
: "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:"
"64-i64:64-f80:32-n8:16:32-a:0:32-S32",
IsWinCOFF ? "_" : "");
}
};


class LLVM_LIBRARY_VISIBILITY MicrosoftX86_32TargetInfo
: public WindowsX86_32TargetInfo {
public:
MicrosoftX86_32TargetInfo(const llvm::Triple &Triple,
const TargetOptions &Opts)
: WindowsX86_32TargetInfo(Triple, Opts) {
LongDoubleWidth = LongDoubleAlign = 64;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
WindowsX86_32TargetInfo::getTargetDefines(Opts, Builder);



Builder.defineMacro("_M_IX86", "600");
}
};


class LLVM_LIBRARY_VISIBILITY MinGWX86_32TargetInfo
: public WindowsX86_32TargetInfo {
public:
MinGWX86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: WindowsX86_32TargetInfo(Triple, Opts) {
HasFloat128 = true;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
WindowsX86_32TargetInfo::getTargetDefines(Opts, Builder);
Builder.defineMacro("_X86_");
}
};


class LLVM_LIBRARY_VISIBILITY CygwinX86_32TargetInfo : public X86_32TargetInfo {
public:
CygwinX86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: X86_32TargetInfo(Triple, Opts) {
this->WCharType = TargetInfo::UnsignedShort;
DoubleAlign = LongLongAlign = 64;
resetDataLayout("e-m:x-p:32:32-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:"
"32-n8:16:32-a:0:32-S32",
"_");
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
X86_32TargetInfo::getTargetDefines(Opts, Builder);
Builder.defineMacro("_X86_");
Builder.defineMacro("__CYGWIN__");
Builder.defineMacro("__CYGWIN32__");
addCygMingDefines(Opts, Builder);
DefineStd(Builder, "unix", Opts);
if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");
}
};


class LLVM_LIBRARY_VISIBILITY HaikuX86_32TargetInfo
: public HaikuTargetInfo<X86_32TargetInfo> {
public:
HaikuX86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: HaikuTargetInfo<X86_32TargetInfo>(Triple, Opts) {}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
HaikuTargetInfo<X86_32TargetInfo>::getTargetDefines(Opts, Builder);
Builder.defineMacro("__INTEL__");
}
};


class LLVM_LIBRARY_VISIBILITY MCUX86_32TargetInfo : public X86_32TargetInfo {
public:
MCUX86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: X86_32TargetInfo(Triple, Opts) {
LongDoubleWidth = 64;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
resetDataLayout("e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-i64:32-f64:"
"32-f128:32-n8:16:32-a:0:32-S32");
WIntType = UnsignedInt;
}

CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {

return CC == CC_C ? CCCR_OK : CCCR_Warning;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
X86_32TargetInfo::getTargetDefines(Opts, Builder);
Builder.defineMacro("__iamcu");
Builder.defineMacro("__iamcu__");
}

bool allowsLargerPreferedTypeAlignment() const override { return false; }
};


class LLVM_LIBRARY_VISIBILITY RTEMSX86_32TargetInfo : public X86_32TargetInfo {
public:
RTEMSX86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: X86_32TargetInfo(Triple, Opts) {
SizeType = UnsignedLong;
IntPtrType = SignedLong;
PtrDiffType = SignedLong;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
X86_32TargetInfo::getTargetDefines(Opts, Builder);
Builder.defineMacro("__INTEL__");
Builder.defineMacro("__rtems__");
}
};


class LLVM_LIBRARY_VISIBILITY X86_64TargetInfo : public X86TargetInfo {
public:
X86_64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: X86TargetInfo(Triple, Opts) {
const bool IsX32 = getTriple().isX32();
bool IsWinCOFF =
getTriple().isOSWindows() && getTriple().isOSBinFormatCOFF();
LongWidth = LongAlign = PointerWidth = PointerAlign = IsX32 ? 32 : 64;
LongDoubleWidth = 128;
LongDoubleAlign = 128;
LargeArrayMinWidth = 128;
LargeArrayAlign = 128;
SuitableAlign = 128;
SizeType = IsX32 ? UnsignedInt : UnsignedLong;
PtrDiffType = IsX32 ? SignedInt : SignedLong;
IntPtrType = IsX32 ? SignedInt : SignedLong;
IntMaxType = IsX32 ? SignedLongLong : SignedLong;
Int64Type = IsX32 ? SignedLongLong : SignedLong;
RegParmMax = 6;


resetDataLayout(IsX32 ? "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-"
"i64:64-f80:128-n8:16:32:64-S128"
: IsWinCOFF ? "e-m:w-p270:32:32-p271:32:32-p272:64:"
"64-i64:64-f80:128-n8:16:32:64-S128"
: "e-m:e-p270:32:32-p271:32:32-p272:64:"
"64-i64:64-f80:128-n8:16:32:64-S128");


RealTypeUsesObjCFPRet = (1 << TargetInfo::LongDouble);


ComplexLongDoubleUsesFP2Ret = true;


HasBuiltinMSVaList = true;


MaxAtomicPromoteWidth = 128;
MaxAtomicInlineWidth = 64;
}

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::X86_64ABIBuiltinVaList;
}

int getEHDataRegisterNumber(unsigned RegNo) const override {
if (RegNo == 0)
return 0;
if (RegNo == 1)
return 1;
return -1;
}

CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {
switch (CC) {
case CC_C:
case CC_Swift:
case CC_SwiftAsync:
case CC_X86VectorCall:
case CC_IntelOclBicc:
case CC_Win64:
case CC_PreserveMost:
case CC_PreserveAll:
case CC_X86RegCall:
case CC_OpenCLKernel:
return CCCR_OK;
default:
return CCCR_Warning;
}
}

CallingConv getDefaultCallingConv() const override {
return CC_C;
}


bool hasInt128Type() const override { return true; }

unsigned getUnwindWordWidth() const override { return 64; }

unsigned getRegisterWidth() const override { return 64; }

bool validateGlobalRegisterVariable(StringRef RegName, unsigned RegSize,
bool &HasSizeMismatch) const override {


if (RegName.equals("rsp") || RegName.equals("rbp")) {

HasSizeMismatch = RegSize != 64;
return true;
}


return X86TargetInfo::validateGlobalRegisterVariable(RegName, RegSize,
HasSizeMismatch);
}

void setMaxAtomicWidth() override {
if (hasFeature("cx16"))
MaxAtomicInlineWidth = 128;
}

ArrayRef<Builtin::Info> getTargetBuiltins() const override;

bool hasExtIntType() const override { return true; }
};


class LLVM_LIBRARY_VISIBILITY WindowsX86_64TargetInfo
: public WindowsTargetInfo<X86_64TargetInfo> {
public:
WindowsX86_64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: WindowsTargetInfo<X86_64TargetInfo>(Triple, Opts) {
LongWidth = LongAlign = 32;
DoubleAlign = LongLongAlign = 64;
IntMaxType = SignedLongLong;
Int64Type = SignedLongLong;
SizeType = UnsignedLongLong;
PtrDiffType = SignedLongLong;
IntPtrType = SignedLongLong;
}

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::CharPtrBuiltinVaList;
}

CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {
switch (CC) {
case CC_X86StdCall:
case CC_X86ThisCall:
case CC_X86FastCall:
return CCCR_Ignore;
case CC_C:
case CC_X86VectorCall:
case CC_IntelOclBicc:
case CC_PreserveMost:
case CC_PreserveAll:
case CC_X86_64SysV:
case CC_Swift:
case CC_SwiftAsync:
case CC_X86RegCall:
case CC_OpenCLKernel:
return CCCR_OK;
default:
return CCCR_Warning;
}
}
};


class LLVM_LIBRARY_VISIBILITY MicrosoftX86_64TargetInfo
: public WindowsX86_64TargetInfo {
public:
MicrosoftX86_64TargetInfo(const llvm::Triple &Triple,
const TargetOptions &Opts)
: WindowsX86_64TargetInfo(Triple, Opts) {
LongDoubleWidth = LongDoubleAlign = 64;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
WindowsX86_64TargetInfo::getTargetDefines(Opts, Builder);
Builder.defineMacro("_M_X64", "100");
Builder.defineMacro("_M_AMD64", "100");
}

TargetInfo::CallingConvKind
getCallingConvKind(bool ClangABICompat4) const override {
return CCK_MicrosoftWin64;
}
};


class LLVM_LIBRARY_VISIBILITY MinGWX86_64TargetInfo
: public WindowsX86_64TargetInfo {
public:
MinGWX86_64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: WindowsX86_64TargetInfo(Triple, Opts) {


LongDoubleWidth = LongDoubleAlign = 128;
LongDoubleFormat = &llvm::APFloat::x87DoubleExtended();
HasFloat128 = true;
}
};


class LLVM_LIBRARY_VISIBILITY CygwinX86_64TargetInfo : public X86_64TargetInfo {
public:
CygwinX86_64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: X86_64TargetInfo(Triple, Opts) {
this->WCharType = TargetInfo::UnsignedShort;
TLSSupported = false;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
X86_64TargetInfo::getTargetDefines(Opts, Builder);
Builder.defineMacro("__x86_64__");
Builder.defineMacro("__CYGWIN__");
Builder.defineMacro("__CYGWIN64__");
addCygMingDefines(Opts, Builder);
DefineStd(Builder, "unix", Opts);
if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");
}
};

class LLVM_LIBRARY_VISIBILITY DarwinX86_64TargetInfo
: public DarwinTargetInfo<X86_64TargetInfo> {
public:
DarwinX86_64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: DarwinTargetInfo<X86_64TargetInfo>(Triple, Opts) {
Int64Type = SignedLongLong;

llvm::Triple T = llvm::Triple(Triple);
if (T.isiOS())
UseSignedCharForObjCBool = false;
resetDataLayout("e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:"
"16:32:64-S128", "_");
}

bool handleTargetFeatures(std::vector<std::string> &Features,
DiagnosticsEngine &Diags) override {
if (!DarwinTargetInfo<X86_64TargetInfo>::handleTargetFeatures(Features,
Diags))
return false;

MaxVectorAlign =
hasFeature("avx512f") ? 512 : hasFeature("avx") ? 256 : 128;
return true;
}
};

class LLVM_LIBRARY_VISIBILITY OpenBSDX86_64TargetInfo
: public OpenBSDTargetInfo<X86_64TargetInfo> {
public:
OpenBSDX86_64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OpenBSDTargetInfo<X86_64TargetInfo>(Triple, Opts) {
IntMaxType = SignedLongLong;
Int64Type = SignedLongLong;
}
};


class LLVM_LIBRARY_VISIBILITY AndroidX86_32TargetInfo
: public LinuxTargetInfo<X86_32TargetInfo> {
public:
AndroidX86_32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: LinuxTargetInfo<X86_32TargetInfo>(Triple, Opts) {
SuitableAlign = 32;
LongDoubleWidth = 64;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
}
};


class LLVM_LIBRARY_VISIBILITY AndroidX86_64TargetInfo
: public LinuxTargetInfo<X86_64TargetInfo> {
public:
AndroidX86_64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: LinuxTargetInfo<X86_64TargetInfo>(Triple, Opts) {
LongDoubleFormat = &llvm::APFloat::IEEEquad();
}
};
}
}
#endif
