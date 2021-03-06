











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_SPARC_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_SPARC_H
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"
namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY SparcTargetInfo : public TargetInfo {
static const TargetInfo::GCCRegAlias GCCRegAliases[];
static const char *const GCCRegNames[];
bool SoftFloat;

public:
SparcTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple), SoftFloat(false) {}

int getEHDataRegisterNumber(unsigned RegNo) const override {
if (RegNo == 0)
return 24;
if (RegNo == 1)
return 25;
return -1;
}

bool handleTargetFeatures(std::vector<std::string> &Features,
DiagnosticsEngine &Diags) override {

auto Feature = llvm::find(Features, "+soft-float");
if (Feature != Features.end()) {
SoftFloat = true;
}
return true;
}
void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

bool hasFeature(StringRef Feature) const override;

bool hasSjLjLowering() const override { return true; }

ArrayRef<Builtin::Info> getTargetBuiltins() const override {

return None;
}
BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::VoidPtrBuiltinVaList;
}
ArrayRef<const char *> getGCCRegNames() const override;
ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override;
bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &info) const override {

switch (*Name) {
case 'I':
case 'J':
case 'K':
case 'L':
case 'M':
case 'N':
case 'O':
return true;

case 'f':
case 'e':
info.setAllowsRegister();
return true;
}
return false;
}
const char *getClobbers() const override {

return "";
}


enum CPUKind {
CK_GENERIC,
CK_V8,
CK_SUPERSPARC,
CK_SPARCLITE,
CK_F934,
CK_HYPERSPARC,
CK_SPARCLITE86X,
CK_SPARCLET,
CK_TSC701,
CK_V9,
CK_ULTRASPARC,
CK_ULTRASPARC3,
CK_NIAGARA,
CK_NIAGARA2,
CK_NIAGARA3,
CK_NIAGARA4,
CK_MYRIAD2100,
CK_MYRIAD2150,
CK_MYRIAD2155,
CK_MYRIAD2450,
CK_MYRIAD2455,
CK_MYRIAD2x5x,
CK_MYRIAD2080,
CK_MYRIAD2085,
CK_MYRIAD2480,
CK_MYRIAD2485,
CK_MYRIAD2x8x,
CK_LEON2,
CK_LEON2_AT697E,
CK_LEON2_AT697F,
CK_LEON3,
CK_LEON3_UT699,
CK_LEON3_GR712RC,
CK_LEON4,
CK_LEON4_GR740
} CPU = CK_GENERIC;

enum CPUGeneration {
CG_V8,
CG_V9,
};

CPUGeneration getCPUGeneration(CPUKind Kind) const;

CPUKind getCPUKind(StringRef Name) const;

bool isValidCPUName(StringRef Name) const override {
return getCPUKind(Name) != CK_GENERIC;
}

void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;

bool setCPU(const std::string &Name) override {
CPU = getCPUKind(Name);
return CPU != CK_GENERIC;
}
};


class LLVM_LIBRARY_VISIBILITY SparcV8TargetInfo : public SparcTargetInfo {
public:
SparcV8TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: SparcTargetInfo(Triple, Opts) {
resetDataLayout("E-m:e-p:32:32-i64:64-f128:64-n32-S64");

switch (getTriple().getOS()) {
default:
SizeType = UnsignedInt;
IntPtrType = SignedInt;
PtrDiffType = SignedInt;
break;
case llvm::Triple::NetBSD:
case llvm::Triple::OpenBSD:
SizeType = UnsignedLong;
IntPtrType = SignedLong;
PtrDiffType = SignedLong;
break;
}


MaxAtomicPromoteWidth = 64;
if (getCPUGeneration(CPU) == CG_V9)
MaxAtomicInlineWidth = 64;
else


MaxAtomicInlineWidth = 32;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

bool hasSjLjLowering() const override { return true; }
bool hasExtIntType() const override { return true; }
};


class LLVM_LIBRARY_VISIBILITY SparcV8elTargetInfo : public SparcV8TargetInfo {
public:
SparcV8elTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: SparcV8TargetInfo(Triple, Opts) {
resetDataLayout("e-m:e-p:32:32-i64:64-f128:64-n32-S64");
}
};


class LLVM_LIBRARY_VISIBILITY SparcV9TargetInfo : public SparcTargetInfo {
public:
SparcV9TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: SparcTargetInfo(Triple, Opts) {

resetDataLayout("E-m:e-i64:64-n32:64-S128");

LongWidth = LongAlign = PointerWidth = PointerAlign = 64;


if (getTriple().isOSOpenBSD())
IntMaxType = SignedLongLong;
else
IntMaxType = SignedLong;
Int64Type = IntMaxType;



LongDoubleWidth = 128;
LongDoubleAlign = 128;
SuitableAlign = 128;
LongDoubleFormat = &llvm::APFloat::IEEEquad();
MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 64;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

bool isValidCPUName(StringRef Name) const override {
return getCPUGeneration(SparcTargetInfo::getCPUKind(Name)) == CG_V9;
}

void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;

bool setCPU(const std::string &Name) override {
if (!SparcTargetInfo::setCPU(Name))
return false;
return getCPUGeneration(CPU) == CG_V9;
}

bool hasExtIntType() const override { return true; }
};
}
}
#endif
