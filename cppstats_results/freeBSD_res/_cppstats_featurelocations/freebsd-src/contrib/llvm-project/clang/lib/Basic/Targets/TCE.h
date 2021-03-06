











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_TCE_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_TCE_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {









static const unsigned TCEOpenCLAddrSpaceMap[] = {
0,
3,
4,
5,
0,
1,
1,

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
};

class LLVM_LIBRARY_VISIBILITY TCETargetInfo : public TargetInfo {
public:
TCETargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple) {
TLSSupported = false;
IntWidth = 32;
LongWidth = LongLongWidth = 32;
PointerWidth = 32;
IntAlign = 32;
LongAlign = LongLongAlign = 32;
PointerAlign = 32;
SuitableAlign = 32;
SizeType = UnsignedInt;
IntMaxType = SignedLong;
IntPtrType = SignedInt;
PtrDiffType = SignedInt;
FloatWidth = 32;
FloatAlign = 32;
DoubleWidth = 32;
DoubleAlign = 32;
LongDoubleWidth = 32;
LongDoubleAlign = 32;
FloatFormat = &llvm::APFloat::IEEEsingle();
DoubleFormat = &llvm::APFloat::IEEEsingle();
LongDoubleFormat = &llvm::APFloat::IEEEsingle();
resetDataLayout("E-p:32:32:32-i1:8:8-i8:8:32-"
"i16:16:32-i32:32:32-i64:32:32-"
"f32:32:32-f64:32:32-v64:32:32-"
"v128:32:32-v256:32:32-v512:32:32-"
"v1024:32:32-a0:0:32-n32");
AddrSpaceMap = &TCEOpenCLAddrSpaceMap;
UseAddrSpaceMapMangling = true;
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

bool hasFeature(StringRef Feature) const override { return Feature == "tce"; }

ArrayRef<Builtin::Info> getTargetBuiltins() const override { return None; }

const char *getClobbers() const override { return ""; }

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::VoidPtrBuiltinVaList;
}

ArrayRef<const char *> getGCCRegNames() const override { return None; }

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &info) const override {
return true;
}

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
return None;
}
};

class LLVM_LIBRARY_VISIBILITY TCELETargetInfo : public TCETargetInfo {
public:
TCELETargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: TCETargetInfo(Triple, Opts) {
BigEndian = false;

resetDataLayout("e-p:32:32:32-i1:8:8-i8:8:32-"
"i16:16:32-i32:32:32-i64:32:32-"
"f32:32:32-f64:32:32-v64:32:32-"
"v128:32:32-v256:32:32-v512:32:32-"
"v1024:32:32-a0:0:32-n32");
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;
};
}
}
#endif
