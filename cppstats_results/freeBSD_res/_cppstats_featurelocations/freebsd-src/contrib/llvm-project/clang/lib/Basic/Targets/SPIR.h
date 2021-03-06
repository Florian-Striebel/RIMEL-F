











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_SPIR_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_SPIR_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

static const unsigned SPIRDefIsPrivMap[] = {
0,
1,
3,
2,
0,
4,
5,
6,
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
0
};

static const unsigned SPIRDefIsGenMap[] = {
4,

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
1,
5,
6,
3,
0,
0,
0,
0
};

class LLVM_LIBRARY_VISIBILITY SPIRTargetInfo : public TargetInfo {
public:
SPIRTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple) {
assert(getTriple().getOS() == llvm::Triple::UnknownOS &&
"SPIR target must use unknown OS");
assert(getTriple().getEnvironment() == llvm::Triple::UnknownEnvironment &&
"SPIR target must use unknown environment type");
TLSSupported = false;
VLASupported = false;
LongWidth = LongAlign = 64;
AddrSpaceMap = &SPIRDefIsPrivMap;
UseAddrSpaceMapMangling = true;
HasLegalHalfType = true;
HasFloat16 = true;


NoAsmVariants = true;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

bool hasFeature(StringRef Feature) const override {
return Feature == "spir";
}



bool useFP16ConversionIntrinsics() const override { return false; }

ArrayRef<Builtin::Info> getTargetBuiltins() const override { return None; }

const char *getClobbers() const override { return ""; }

ArrayRef<const char *> getGCCRegNames() const override { return None; }

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &info) const override {
return true;
}

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
return None;
}

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::VoidPtrBuiltinVaList;
}

Optional<unsigned>
getDWARFAddressSpace(unsigned AddressSpace) const override {
return AddressSpace;
}

CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {
return (CC == CC_SpirFunction || CC == CC_OpenCLKernel) ? CCCR_OK
: CCCR_Warning;
}

CallingConv getDefaultCallingConv() const override {
return CC_SpirFunction;
}

void setAddressSpaceMap(bool DefaultIsGeneric) {
AddrSpaceMap = DefaultIsGeneric ? &SPIRDefIsGenMap : &SPIRDefIsPrivMap;
}

void adjust(DiagnosticsEngine &Diags, LangOptions &Opts) override {
TargetInfo::adjust(Diags, Opts);







setAddressSpaceMap(Opts.SYCLIsDevice);
}

void setSupportedOpenCLOpts() override {


supportAllOpenCLOpts();
}

bool hasExtIntType() const override { return true; }

bool hasInt128Type() const override { return false; }
};

class LLVM_LIBRARY_VISIBILITY SPIR32TargetInfo : public SPIRTargetInfo {
public:
SPIR32TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: SPIRTargetInfo(Triple, Opts) {
PointerWidth = PointerAlign = 32;
SizeType = TargetInfo::UnsignedInt;
PtrDiffType = IntPtrType = TargetInfo::SignedInt;
resetDataLayout("e-p:32:32-i64:64-v16:16-v24:32-v32:32-v48:64-"
"v96:128-v192:256-v256:256-v512:512-v1024:1024");
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;
};

class LLVM_LIBRARY_VISIBILITY SPIR64TargetInfo : public SPIRTargetInfo {
public:
SPIR64TargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: SPIRTargetInfo(Triple, Opts) {
PointerWidth = PointerAlign = 64;
SizeType = TargetInfo::UnsignedLong;
PtrDiffType = IntPtrType = TargetInfo::SignedLong;
resetDataLayout("e-i64:64-v16:16-v24:32-v32:32-v48:64-"
"v96:128-v192:256-v256:256-v512:512-v1024:1024");
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;
};
}
}
#endif
