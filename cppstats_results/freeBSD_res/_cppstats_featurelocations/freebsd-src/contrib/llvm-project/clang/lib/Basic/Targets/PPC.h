











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_PPC_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_PPC_H

#include "OSTargets.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {


class LLVM_LIBRARY_VISIBILITY PPCTargetInfo : public TargetInfo {


typedef enum {
ArchDefineNone = 0,
ArchDefineName = 1 << 0,
ArchDefinePpcgr = 1 << 1,
ArchDefinePpcsq = 1 << 2,
ArchDefine440 = 1 << 3,
ArchDefine603 = 1 << 4,
ArchDefine604 = 1 << 5,
ArchDefinePwr4 = 1 << 6,
ArchDefinePwr5 = 1 << 7,
ArchDefinePwr5x = 1 << 8,
ArchDefinePwr6 = 1 << 9,
ArchDefinePwr6x = 1 << 10,
ArchDefinePwr7 = 1 << 11,
ArchDefinePwr8 = 1 << 12,
ArchDefinePwr9 = 1 << 13,
ArchDefinePwr10 = 1 << 14,
ArchDefineFuture = 1 << 15,
ArchDefineA2 = 1 << 16,
ArchDefineE500 = 1 << 18
} ArchDefineTypes;

ArchDefineTypes ArchDefs = ArchDefineNone;
static const Builtin::Info BuiltinInfo[];
static const char *const GCCRegNames[];
static const TargetInfo::GCCRegAlias GCCRegAliases[];
std::string CPU;
enum PPCFloatABI { HardFloat, SoftFloat } FloatABI;


bool HasAltivec = false;
bool HasMMA = false;
bool HasROPProtect = false;
bool HasPrivileged = false;
bool HasVSX = false;
bool HasP8Vector = false;
bool HasP8Crypto = false;
bool HasDirectMove = false;
bool HasHTM = false;
bool HasBPERMD = false;
bool HasExtDiv = false;
bool HasP9Vector = false;
bool HasSPE = false;
bool PairedVectorMemops = false;
bool HasP10Vector = false;
bool HasPCRelativeMemops = false;
bool HasPrefixInstrs = false;
bool IsISA2_07 = false;
bool IsISA3_0 = false;
bool IsISA3_1 = false;

protected:
std::string ABI;

public:
PPCTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple) {
SuitableAlign = 128;
SimdDefaultAlign = 128;
LongDoubleWidth = LongDoubleAlign = 128;
LongDoubleFormat = &llvm::APFloat::PPCDoubleDouble();
HasStrictFP = true;
}


void adjust(DiagnosticsEngine &Diags, LangOptions &Opts) override;




bool isValidCPUName(StringRef Name) const override;
void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;

bool setCPU(const std::string &Name) override {
bool CPUKnown = isValidCPUName(Name);
if (CPUKnown) {
CPU = Name;


ArchDefs =
(ArchDefineTypes)llvm::StringSwitch<int>(CPU)
.Case("440", ArchDefineName)
.Case("450", ArchDefineName | ArchDefine440)
.Case("601", ArchDefineName)
.Case("602", ArchDefineName | ArchDefinePpcgr)
.Case("603", ArchDefineName | ArchDefinePpcgr)
.Case("603e", ArchDefineName | ArchDefine603 | ArchDefinePpcgr)
.Case("603ev", ArchDefineName | ArchDefine603 | ArchDefinePpcgr)
.Case("604", ArchDefineName | ArchDefinePpcgr)
.Case("604e", ArchDefineName | ArchDefine604 | ArchDefinePpcgr)
.Case("620", ArchDefineName | ArchDefinePpcgr)
.Case("630", ArchDefineName | ArchDefinePpcgr)
.Case("7400", ArchDefineName | ArchDefinePpcgr)
.Case("7450", ArchDefineName | ArchDefinePpcgr)
.Case("750", ArchDefineName | ArchDefinePpcgr)
.Case("970", ArchDefineName | ArchDefinePwr4 | ArchDefinePpcgr |
ArchDefinePpcsq)
.Case("a2", ArchDefineA2)
.Cases("power3", "pwr3", ArchDefinePpcgr)
.Cases("power4", "pwr4",
ArchDefinePwr4 | ArchDefinePpcgr | ArchDefinePpcsq)
.Cases("power5", "pwr5",
ArchDefinePwr5 | ArchDefinePwr4 | ArchDefinePpcgr |
ArchDefinePpcsq)
.Cases("power5x", "pwr5x",
ArchDefinePwr5x | ArchDefinePwr5 | ArchDefinePwr4 |
ArchDefinePpcgr | ArchDefinePpcsq)
.Cases("power6", "pwr6",
ArchDefinePwr6 | ArchDefinePwr5x | ArchDefinePwr5 |
ArchDefinePwr4 | ArchDefinePpcgr | ArchDefinePpcsq)
.Cases("power6x", "pwr6x",
ArchDefinePwr6x | ArchDefinePwr6 | ArchDefinePwr5x |
ArchDefinePwr5 | ArchDefinePwr4 | ArchDefinePpcgr |
ArchDefinePpcsq)
.Cases("power7", "pwr7",
ArchDefinePwr7 | ArchDefinePwr6 | ArchDefinePwr5x |
ArchDefinePwr5 | ArchDefinePwr4 | ArchDefinePpcgr |
ArchDefinePpcsq)

.Cases("power8", "pwr8", "ppc64le",
ArchDefinePwr8 | ArchDefinePwr7 | ArchDefinePwr6 |
ArchDefinePwr5x | ArchDefinePwr5 | ArchDefinePwr4 |
ArchDefinePpcgr | ArchDefinePpcsq)
.Cases("power9", "pwr9",
ArchDefinePwr9 | ArchDefinePwr8 | ArchDefinePwr7 |
ArchDefinePwr6 | ArchDefinePwr5x | ArchDefinePwr5 |
ArchDefinePwr4 | ArchDefinePpcgr | ArchDefinePpcsq)
.Cases("power10", "pwr10",
ArchDefinePwr10 | ArchDefinePwr9 | ArchDefinePwr8 |
ArchDefinePwr7 | ArchDefinePwr6 | ArchDefinePwr5x |
ArchDefinePwr5 | ArchDefinePwr4 | ArchDefinePpcgr |
ArchDefinePpcsq)
.Case("future",
ArchDefineFuture | ArchDefinePwr10 | ArchDefinePwr9 |
ArchDefinePwr8 | ArchDefinePwr7 | ArchDefinePwr6 |
ArchDefinePwr5x | ArchDefinePwr5 | ArchDefinePwr4 |
ArchDefinePpcgr | ArchDefinePpcsq)
.Cases("8548", "e500", ArchDefineE500)
.Default(ArchDefineNone);
}
return CPUKnown;
}

StringRef getABI() const override { return ABI; }

ArrayRef<Builtin::Info> getTargetBuiltins() const override;

bool isCLZForZeroUndef() const override { return false; }

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

bool
initFeatureMap(llvm::StringMap<bool> &Features, DiagnosticsEngine &Diags,
StringRef CPU,
const std::vector<std::string> &FeaturesVec) const override;

void addP10SpecificFeatures(llvm::StringMap<bool> &Features) const;
void addFutureSpecificFeatures(llvm::StringMap<bool> &Features) const;

bool handleTargetFeatures(std::vector<std::string> &Features,
DiagnosticsEngine &Diags) override;

bool hasFeature(StringRef Feature) const override;

void setFeatureEnabled(llvm::StringMap<bool> &Features, StringRef Name,
bool Enabled) const override;

ArrayRef<const char *> getGCCRegNames() const override;

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override;

ArrayRef<TargetInfo::AddlRegName> getGCCAddlRegNames() const override;

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &Info) const override {
switch (*Name) {
default:
return false;
case 'O':
break;
case 'f':

if (FloatABI == SoftFloat)
return false;
LLVM_FALLTHROUGH;
case 'b':
Info.setAllowsRegister();
break;



case 'd':
case 'v':


if (FloatABI == SoftFloat)
return false;
Info.setAllowsRegister();
break;
case 'w':
switch (Name[1]) {
case 'd':
case 'f':
case 's':
case 'w':
case 'a':
case 'c':
case 'i':
break;
default:
return false;
}
Info.setAllowsRegister();
Name++;
break;
case 'h':
case 'q':
case 'c':
case 'l':
case 'x':
case 'y':
case 'z':
Info.setAllowsRegister();
break;
case 'I':
case 'J':

case 'K':
case 'L':
case 'M':
case 'N':
case 'P':
case 'G':

case 'H':

break;
case 'm':











case 'e':
if (Name[1] != 's')
return false;





Info.setAllowsMemory();
Name++;
break;
case 'Q':

Info.setAllowsRegister();
LLVM_FALLTHROUGH;
case 'Z':


Info.setAllowsMemory();
break;
case 'R':
case 'a':

case 'S':
case 'T':
case 'U':
case 't':

case 'W':
case 'j':
break;

}
return true;
}

std::string convertConstraint(const char *&Constraint) const override {
std::string R;
switch (*Constraint) {
case 'e':
case 'w':

R = std::string("^") + std::string(Constraint, 2);
Constraint++;
break;
default:
return TargetInfo::convertConstraint(Constraint);
}
return R;
}

const char *getClobbers() const override { return ""; }
int getEHDataRegisterNumber(unsigned RegNo) const override {
if (RegNo == 0)
return 3;
if (RegNo == 1)
return 4;
return -1;
}

bool hasSjLjLowering() const override { return true; }

const char *getLongDoubleMangling() const override {
if (LongDoubleWidth == 64)
return "e";
return LongDoubleFormat == &llvm::APFloat::PPCDoubleDouble()
? "g"
: "u9__ieee128";
}
const char *getFloat128Mangling() const override { return "u9__ieee128"; }

bool hasExtIntType() const override { return true; }

bool isSPRegName(StringRef RegName) const override {
return RegName.equals("r1") || RegName.equals("x1");
}
};

class LLVM_LIBRARY_VISIBILITY PPC32TargetInfo : public PPCTargetInfo {
public:
PPC32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: PPCTargetInfo(Triple, Opts) {
if (Triple.isOSAIX())
resetDataLayout("E-m:a-p:32:32-i64:64-n32");
else if (Triple.getArch() == llvm::Triple::ppcle)
resetDataLayout("e-m:e-p:32:32-i64:64-n32");
else
resetDataLayout("E-m:e-p:32:32-i64:64-n32");

switch (getTriple().getOS()) {
case llvm::Triple::Linux:
case llvm::Triple::FreeBSD:
case llvm::Triple::NetBSD:
SizeType = UnsignedInt;
PtrDiffType = SignedInt;
IntPtrType = SignedInt;
break;
case llvm::Triple::AIX:
SizeType = UnsignedLong;
PtrDiffType = SignedLong;
IntPtrType = SignedLong;
LongDoubleWidth = 64;
LongDoubleAlign = DoubleAlign = 32;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
break;
default:
break;
}

if (Triple.isOSFreeBSD() || Triple.isOSNetBSD() || Triple.isOSOpenBSD() ||
Triple.isMusl()) {
LongDoubleWidth = LongDoubleAlign = 64;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
}


MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 32;
}

BuiltinVaListKind getBuiltinVaListKind() const override {

return TargetInfo::PowerABIBuiltinVaList;
}
};



class LLVM_LIBRARY_VISIBILITY PPC64TargetInfo : public PPCTargetInfo {
public:
PPC64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: PPCTargetInfo(Triple, Opts) {
LongWidth = LongAlign = PointerWidth = PointerAlign = 64;
IntMaxType = SignedLong;
Int64Type = SignedLong;
std::string DataLayout = "";

if (Triple.isOSAIX()) {

DataLayout = "E-m:a-i64:64-n32:64";
LongDoubleWidth = 64;
LongDoubleAlign = DoubleAlign = 32;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
} else if ((Triple.getArch() == llvm::Triple::ppc64le)) {
DataLayout = "e-m:e-i64:64-n32:64";
ABI = "elfv2";
} else {
DataLayout = "E-m:e-i64:64-n32:64";
ABI = "elfv1";
}

if (Triple.isOSFreeBSD() || Triple.isOSOpenBSD() || Triple.isMusl()) {
LongDoubleWidth = LongDoubleAlign = 64;
LongDoubleFormat = &llvm::APFloat::IEEEdouble();
}

if (Triple.isOSAIX() || Triple.isOSLinux())
DataLayout += "-S128-v256:256:256-v512:512:512";
resetDataLayout(DataLayout);


MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 64;
}

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::CharPtrBuiltinVaList;
}


bool setABI(const std::string &Name) override {
if (Name == "elfv1" || Name == "elfv2") {
ABI = Name;
return true;
}
return false;
}

CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {
switch (CC) {
case CC_Swift:
return CCCR_OK;
case CC_SwiftAsync:
return CCCR_Error;
default:
return CCCR_Warning;
}
}
};

class LLVM_LIBRARY_VISIBILITY DarwinPPC32TargetInfo
: public DarwinTargetInfo<PPC32TargetInfo> {
public:
DarwinPPC32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: DarwinTargetInfo<PPC32TargetInfo>(Triple, Opts) {
HasAlignMac68kSupport = true;
BoolWidth = BoolAlign = 32;
PtrDiffType = SignedInt;
LongLongAlign = 32;
resetDataLayout("E-m:o-p:32:32-f64:32:64-n32", "_");
}

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::CharPtrBuiltinVaList;
}
};

class LLVM_LIBRARY_VISIBILITY DarwinPPC64TargetInfo
: public DarwinTargetInfo<PPC64TargetInfo> {
public:
DarwinPPC64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: DarwinTargetInfo<PPC64TargetInfo>(Triple, Opts) {
HasAlignMac68kSupport = true;
resetDataLayout("E-m:o-i64:64-n32:64", "_");
}
};

class LLVM_LIBRARY_VISIBILITY AIXPPC32TargetInfo :
public AIXTargetInfo<PPC32TargetInfo> {
public:
using AIXTargetInfo::AIXTargetInfo;
BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::CharPtrBuiltinVaList;
}
};

class LLVM_LIBRARY_VISIBILITY AIXPPC64TargetInfo :
public AIXTargetInfo<PPC64TargetInfo> {
public:
using AIXTargetInfo::AIXTargetInfo;
};

}
}
#endif
