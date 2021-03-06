







#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_APSINTTYPE_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_APSINTTYPE_H

#include "llvm/ADT/APSInt.h"
#include <tuple>

namespace clang {
namespace ento {


class APSIntType {
uint32_t BitWidth;
bool IsUnsigned;

public:
APSIntType(uint32_t Width, bool Unsigned)
: BitWidth(Width), IsUnsigned(Unsigned) {}

APSIntType(const llvm::APSInt &Value)
: BitWidth(Value.getBitWidth()), IsUnsigned(Value.isUnsigned()) {}

uint32_t getBitWidth() const { return BitWidth; }
bool isUnsigned() const { return IsUnsigned; }





void apply(llvm::APSInt &Value) const {


Value = Value.extOrTrunc(BitWidth);
Value.setIsUnsigned(IsUnsigned);
}





llvm::APSInt convert(const llvm::APSInt &Value) const LLVM_READONLY {
llvm::APSInt Result(Value, Value.isUnsigned());
apply(Result);
return Result;
}


llvm::APSInt getZeroValue() const LLVM_READONLY {
return llvm::APSInt(BitWidth, IsUnsigned);
}


llvm::APSInt getMinValue() const LLVM_READONLY {
return llvm::APSInt::getMinValue(BitWidth, IsUnsigned);
}


llvm::APSInt getMaxValue() const LLVM_READONLY {
return llvm::APSInt::getMaxValue(BitWidth, IsUnsigned);
}

llvm::APSInt getValue(uint64_t RawValue) const LLVM_READONLY {
return (llvm::APSInt(BitWidth, IsUnsigned) = RawValue);
}




enum RangeTestResultKind {
RTR_Below = -1,
RTR_Within = 0,
RTR_Above = 1
};







RangeTestResultKind testInRange(const llvm::APSInt &Val,
bool AllowMixedSign) const LLVM_READONLY;

bool operator==(const APSIntType &Other) const {
return BitWidth == Other.BitWidth && IsUnsigned == Other.IsUnsigned;
}





bool operator<(const APSIntType &Other) const {
return std::tie(BitWidth, IsUnsigned) <
std::tie(Other.BitWidth, Other.IsUnsigned);
}
};

}
}

#endif
