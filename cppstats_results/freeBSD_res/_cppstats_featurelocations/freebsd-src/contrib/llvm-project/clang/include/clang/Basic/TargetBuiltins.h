













#if !defined(LLVM_CLANG_BASIC_TARGETBUILTINS_H)
#define LLVM_CLANG_BASIC_TARGETBUILTINS_H

#include <algorithm>
#include <stdint.h>
#include "clang/Basic/Builtins.h"
#include "llvm/Support/MathExtras.h"
#undef PPC

namespace clang {

namespace NEON {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsNEON.def"
FirstTSBuiltin
};
}


namespace ARM {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
LastNEONBuiltin = NEON::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsARM.def"
LastTSBuiltin
};
}

namespace SVE {
enum {
LastNEONBuiltin = NEON::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsSVE.def"
FirstTSBuiltin,
};
}


namespace AArch64 {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin - 1,
LastNEONBuiltin = NEON::FirstTSBuiltin - 1,
FirstSVEBuiltin = NEON::FirstTSBuiltin,
LastSVEBuiltin = SVE::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsAArch64.def"
LastTSBuiltin
};
}


namespace BPF {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsBPF.def"
LastTSBuiltin
};
}


namespace PPC {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsPPC.def"
LastTSBuiltin
};
}


namespace NVPTX {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsNVPTX.def"
LastTSBuiltin
};
}


namespace AMDGPU {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsAMDGPU.def"
LastTSBuiltin
};
}


namespace X86 {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsX86.def"
FirstX86_64Builtin,
LastX86CommonBuiltin = FirstX86_64Builtin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsX86_64.def"
LastTSBuiltin
};
}


namespace VE {
enum { LastTIBuiltin = clang::Builtin::FirstTSBuiltin - 1, LastTSBuiltin };
}


namespace RISCV {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin - 1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsRISCV.def"
LastTSBuiltin
};
}




class NeonTypeFlags {
enum {
EltTypeMask = 0xf,
UnsignedFlag = 0x10,
QuadFlag = 0x20
};
uint32_t Flags;

public:
enum EltType {
Int8,
Int16,
Int32,
Int64,
Poly8,
Poly16,
Poly64,
Poly128,
Float16,
Float32,
Float64,
BFloat16
};

NeonTypeFlags(unsigned F) : Flags(F) {}
NeonTypeFlags(EltType ET, bool IsUnsigned, bool IsQuad) : Flags(ET) {
if (IsUnsigned)
Flags |= UnsignedFlag;
if (IsQuad)
Flags |= QuadFlag;
}

EltType getEltType() const { return (EltType)(Flags & EltTypeMask); }
bool isPoly() const {
EltType ET = getEltType();
return ET == Poly8 || ET == Poly16 || ET == Poly64;
}
bool isUnsigned() const { return (Flags & UnsignedFlag) != 0; }
bool isQuad() const { return (Flags & QuadFlag) != 0; }
};


class SVETypeFlags {
uint64_t Flags;
unsigned EltTypeShift;
unsigned MemEltTypeShift;
unsigned MergeTypeShift;
unsigned SplatOperandMaskShift;

public:
#define LLVM_GET_SVE_TYPEFLAGS
#include "clang/Basic/arm_sve_typeflags.inc"
#undef LLVM_GET_SVE_TYPEFLAGS

enum EltType {
#define LLVM_GET_SVE_ELTTYPES
#include "clang/Basic/arm_sve_typeflags.inc"
#undef LLVM_GET_SVE_ELTTYPES
};

enum MemEltType {
#define LLVM_GET_SVE_MEMELTTYPES
#include "clang/Basic/arm_sve_typeflags.inc"
#undef LLVM_GET_SVE_MEMELTTYPES
};

enum MergeType {
#define LLVM_GET_SVE_MERGETYPES
#include "clang/Basic/arm_sve_typeflags.inc"
#undef LLVM_GET_SVE_MERGETYPES
};

enum ImmCheckType {
#define LLVM_GET_SVE_IMMCHECKTYPES
#include "clang/Basic/arm_sve_typeflags.inc"
#undef LLVM_GET_SVE_IMMCHECKTYPES
};

SVETypeFlags(uint64_t F) : Flags(F) {
EltTypeShift = llvm::countTrailingZeros(EltTypeMask);
MemEltTypeShift = llvm::countTrailingZeros(MemEltTypeMask);
MergeTypeShift = llvm::countTrailingZeros(MergeTypeMask);
SplatOperandMaskShift = llvm::countTrailingZeros(SplatOperandMask);
}

EltType getEltType() const {
return (EltType)((Flags & EltTypeMask) >> EltTypeShift);
}

MemEltType getMemEltType() const {
return (MemEltType)((Flags & MemEltTypeMask) >> MemEltTypeShift);
}

MergeType getMergeType() const {
return (MergeType)((Flags & MergeTypeMask) >> MergeTypeShift);
}

unsigned getSplatOperand() const {
return ((Flags & SplatOperandMask) >> SplatOperandMaskShift) - 1;
}

bool hasSplatOperand() const {
return Flags & SplatOperandMask;
}

bool isLoad() const { return Flags & IsLoad; }
bool isStore() const { return Flags & IsStore; }
bool isGatherLoad() const { return Flags & IsGatherLoad; }
bool isScatterStore() const { return Flags & IsScatterStore; }
bool isStructLoad() const { return Flags & IsStructLoad; }
bool isStructStore() const { return Flags & IsStructStore; }
bool isZExtReturn() const { return Flags & IsZExtReturn; }
bool isByteIndexed() const { return Flags & IsByteIndexed; }
bool isOverloadNone() const { return Flags & IsOverloadNone; }
bool isOverloadWhile() const { return Flags & IsOverloadWhile; }
bool isOverloadDefault() const { return !(Flags & OverloadKindMask); }
bool isOverloadWhileRW() const { return Flags & IsOverloadWhileRW; }
bool isOverloadCvt() const { return Flags & IsOverloadCvt; }
bool isPrefetch() const { return Flags & IsPrefetch; }
bool isReverseCompare() const { return Flags & ReverseCompare; }
bool isAppendSVALL() const { return Flags & IsAppendSVALL; }
bool isInsertOp1SVALL() const { return Flags & IsInsertOp1SVALL; }
bool isGatherPrefetch() const { return Flags & IsGatherPrefetch; }
bool isReverseUSDOT() const { return Flags & ReverseUSDOT; }
bool isUndef() const { return Flags & IsUndef; }
bool isTupleCreate() const { return Flags & IsTupleCreate; }
bool isTupleGet() const { return Flags & IsTupleGet; }
bool isTupleSet() const { return Flags & IsTupleSet; }

uint64_t getBits() const { return Flags; }
bool isFlagSet(uint64_t Flag) const { return Flags & Flag; }
};


namespace Hexagon {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsHexagon.def"
LastTSBuiltin
};
}


namespace Mips {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsMips.def"
LastTSBuiltin
};
}


namespace XCore {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsXCore.def"
LastTSBuiltin
};
}


namespace SystemZ {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsSystemZ.def"
LastTSBuiltin
};
}


namespace WebAssembly {
enum {
LastTIBuiltin = clang::Builtin::FirstTSBuiltin-1,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/BuiltinsWebAssembly.def"
LastTSBuiltin
};
}

static constexpr uint64_t LargestBuiltinID = std::max<uint64_t>(
{ARM::LastTSBuiltin, AArch64::LastTSBuiltin, BPF::LastTSBuiltin,
PPC::LastTSBuiltin, NVPTX::LastTSBuiltin, AMDGPU::LastTSBuiltin,
X86::LastTSBuiltin, VE::LastTSBuiltin, RISCV::LastTSBuiltin,
Hexagon::LastTSBuiltin, Mips::LastTSBuiltin, XCore::LastTSBuiltin,
SystemZ::LastTSBuiltin, WebAssembly::LastTSBuiltin});

}

#endif
