











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_MSP430_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_MSP430_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY MSP430TargetInfo : public TargetInfo {
static const char *const GCCRegNames[];

public:
MSP430TargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple) {
TLSSupported = false;
IntWidth = 16;
IntAlign = 16;
LongWidth = 32;
LongLongWidth = 64;
LongAlign = LongLongAlign = 16;
FloatWidth = 32;
FloatAlign = 16;
DoubleWidth = LongDoubleWidth = 64;
DoubleAlign = LongDoubleAlign = 16;
PointerWidth = 16;
PointerAlign = 16;
SuitableAlign = 16;
SizeType = UnsignedInt;
IntMaxType = SignedLongLong;
IntPtrType = SignedInt;
PtrDiffType = SignedInt;
SigAtomicType = SignedLong;
resetDataLayout("e-m:e-p:16:16-i32:16-i64:16-f32:16-f64:16-a:8-n8:16-S16");
}
void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

ArrayRef<Builtin::Info> getTargetBuiltins() const override {

return None;
}

bool allowsLargerPreferedTypeAlignment() const override { return false; }

bool hasFeature(StringRef Feature) const override {
return Feature == "msp430";
}

ArrayRef<const char *> getGCCRegNames() const override;

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {

static const TargetInfo::GCCRegAlias GCCRegAliases[] = {
{{"r0"}, "pc"},
{{"r1"}, "sp"},
{{"r2"}, "sr"},
{{"r3"}, "cg"},
};
return llvm::makeArrayRef(GCCRegAliases);
}

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &info) const override {

switch (*Name) {
case 'K':
case 'L':
case 'M':
return true;
}

return false;
}

const char *getClobbers() const override {

return "";
}

BuiltinVaListKind getBuiltinVaListKind() const override {

return TargetInfo::CharPtrBuiltinVaList;
}
};

}
}
#endif
