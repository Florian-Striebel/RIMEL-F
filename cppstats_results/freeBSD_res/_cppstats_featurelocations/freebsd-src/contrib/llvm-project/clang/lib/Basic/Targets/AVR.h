











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_AVR_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_AVR_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {


class LLVM_LIBRARY_VISIBILITY AVRTargetInfo : public TargetInfo {
public:
AVRTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
: TargetInfo(Triple) {
TLSSupported = false;
PointerWidth = 16;
PointerAlign = 8;
IntWidth = 16;
IntAlign = 8;
LongWidth = 32;
LongAlign = 8;
LongLongWidth = 64;
LongLongAlign = 8;
SuitableAlign = 8;
DefaultAlignForAttributeAligned = 8;
HalfWidth = 16;
HalfAlign = 8;
FloatWidth = 32;
FloatAlign = 8;
DoubleWidth = 32;
DoubleAlign = 8;
DoubleFormat = &llvm::APFloat::IEEEsingle();
LongDoubleWidth = 32;
LongDoubleAlign = 8;
LongDoubleFormat = &llvm::APFloat::IEEEsingle();
SizeType = UnsignedInt;
PtrDiffType = SignedInt;
IntPtrType = SignedInt;
Char16Type = UnsignedInt;
WIntType = SignedInt;
Int16Type = SignedInt;
Char32Type = UnsignedLong;
SigAtomicType = SignedChar;
resetDataLayout("e-P1-p:16:8-i8:8-i16:8-i32:8-i64:8-f32:8-f64:8-n8-a:8");
}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

ArrayRef<Builtin::Info> getTargetBuiltins() const override { return None; }

BuiltinVaListKind getBuiltinVaListKind() const override {
return TargetInfo::VoidPtrBuiltinVaList;
}

const char *getClobbers() const override { return ""; }

ArrayRef<const char *> getGCCRegNames() const override {
static const char *const GCCRegNames[] = {
"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9",
"r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19",
"r20", "r21", "r22", "r23", "r24", "r25", "X", "Y", "Z", "SP"
};
return llvm::makeArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
return None;
}

ArrayRef<TargetInfo::AddlRegName> getGCCAddlRegNames() const override {
static const TargetInfo::AddlRegName AddlRegNames[] = {
{{"r26", "r27"}, 26},
{{"r28", "r29"}, 27},
{{"r30", "r31"}, 28},
{{"SPL", "SPH"}, 29},
};
return llvm::makeArrayRef(AddlRegNames);
}

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &Info) const override {

if (StringRef(Name).size() > 1)
return false;

switch (*Name) {
default:
return false;
case 'a':
case 'b':
case 'd':
case 'l':
case 'e':
case 'q':
case 'r':
case 'w':
case 't':
case 'x':
case 'X':
case 'y':
case 'Y':
case 'z':
case 'Z':
Info.setAllowsRegister();
return true;
case 'I':
Info.setRequiresImmediate(0, 63);
return true;
case 'J':
Info.setRequiresImmediate(-63, 0);
return true;
case 'K':
Info.setRequiresImmediate(2);
return true;
case 'L':
Info.setRequiresImmediate(0);
return true;
case 'M':
Info.setRequiresImmediate(0, 0xff);
return true;
case 'N':
Info.setRequiresImmediate(-1);
return true;
case 'O':
Info.setRequiresImmediate({8, 16, 24});
return true;
case 'P':
Info.setRequiresImmediate(1);
return true;
case 'R':
Info.setRequiresImmediate(-6, 5);
return true;
case 'G':
case 'Q':
return true;
}

return false;
}

IntType getIntTypeByWidth(unsigned BitWidth, bool IsSigned) const final {

return BitWidth == 16 ? (IsSigned ? SignedInt : UnsignedInt)
: TargetInfo::getIntTypeByWidth(BitWidth, IsSigned);
}

IntType getLeastIntTypeByWidth(unsigned BitWidth, bool IsSigned) const final {

return BitWidth == 16
? (IsSigned ? SignedInt : UnsignedInt)
: TargetInfo::getLeastIntTypeByWidth(BitWidth, IsSigned);
}

bool isValidCPUName(StringRef Name) const override;
void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;
bool setCPU(const std::string &Name) override {
bool isValid = isValidCPUName(Name);
if (isValid)
CPU = Name;
return isValid;
}

protected:
std::string CPU;
};

}
}

#endif
