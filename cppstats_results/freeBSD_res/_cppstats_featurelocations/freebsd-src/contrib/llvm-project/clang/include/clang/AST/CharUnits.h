











#if !defined(LLVM_CLANG_AST_CHARUNITS_H)
#define LLVM_CLANG_AST_CHARUNITS_H

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/MathExtras.h"

namespace clang {
















class CharUnits {
public:
typedef int64_t QuantityType;

private:
QuantityType Quantity = 0;

explicit CharUnits(QuantityType C) : Quantity(C) {}

public:


CharUnits() = default;


static CharUnits Zero() {
return CharUnits(0);
}


static CharUnits One() {
return CharUnits(1);
}


static CharUnits fromQuantity(QuantityType Quantity) {
return CharUnits(Quantity);
}


CharUnits& operator+= (const CharUnits &Other) {
Quantity += Other.Quantity;
return *this;
}
CharUnits& operator++ () {
++Quantity;
return *this;
}
CharUnits operator++ (int) {
return CharUnits(Quantity++);
}
CharUnits& operator-= (const CharUnits &Other) {
Quantity -= Other.Quantity;
return *this;
}
CharUnits& operator-- () {
--Quantity;
return *this;
}
CharUnits operator-- (int) {
return CharUnits(Quantity--);
}


bool operator== (const CharUnits &Other) const {
return Quantity == Other.Quantity;
}
bool operator!= (const CharUnits &Other) const {
return Quantity != Other.Quantity;
}


bool operator< (const CharUnits &Other) const {
return Quantity < Other.Quantity;
}
bool operator<= (const CharUnits &Other) const {
return Quantity <= Other.Quantity;
}
bool operator> (const CharUnits &Other) const {
return Quantity > Other.Quantity;
}
bool operator>= (const CharUnits &Other) const {
return Quantity >= Other.Quantity;
}




bool isZero() const { return Quantity == 0; }


bool isOne() const { return Quantity == 1; }


bool isPositive() const { return Quantity > 0; }


bool isNegative() const { return Quantity < 0; }



bool isPowerOfTwo() const {
return (Quantity & -Quantity) == Quantity;
}





bool isMultipleOf(CharUnits N) const {
return (*this % N) == 0;
}


CharUnits operator* (QuantityType N) const {
return CharUnits(Quantity * N);
}
CharUnits &operator*= (QuantityType N) {
Quantity *= N;
return *this;
}
CharUnits operator/ (QuantityType N) const {
return CharUnits(Quantity / N);
}
CharUnits &operator/= (QuantityType N) {
Quantity /= N;
return *this;
}
QuantityType operator/ (const CharUnits &Other) const {
return Quantity / Other.Quantity;
}
CharUnits operator% (QuantityType N) const {
return CharUnits(Quantity % N);
}
QuantityType operator% (const CharUnits &Other) const {
return Quantity % Other.Quantity;
}
CharUnits operator+ (const CharUnits &Other) const {
return CharUnits(Quantity + Other.Quantity);
}
CharUnits operator- (const CharUnits &Other) const {
return CharUnits(Quantity - Other.Quantity);
}
CharUnits operator- () const {
return CharUnits(-Quantity);
}





QuantityType getQuantity() const { return Quantity; }



llvm::Align getAsAlign() const { return llvm::Align(Quantity); }




CharUnits alignTo(const CharUnits &Align) const {
return CharUnits(llvm::alignTo(Quantity, Align.Quantity));
}



CharUnits alignmentAtOffset(CharUnits offset) const {
assert(Quantity != 0 && "offsetting from unknown alignment?");
return CharUnits(llvm::MinAlign(Quantity, offset.Quantity));
}



CharUnits alignmentOfArrayElement(CharUnits elementSize) const {



return alignmentAtOffset(elementSize);
}


};
}

inline clang::CharUnits operator* (clang::CharUnits::QuantityType Scale,
const clang::CharUnits &CU) {
return CU * Scale;
}

namespace llvm {

template<> struct DenseMapInfo<clang::CharUnits> {
static clang::CharUnits getEmptyKey() {
clang::CharUnits::QuantityType Quantity =
DenseMapInfo<clang::CharUnits::QuantityType>::getEmptyKey();

return clang::CharUnits::fromQuantity(Quantity);
}

static clang::CharUnits getTombstoneKey() {
clang::CharUnits::QuantityType Quantity =
DenseMapInfo<clang::CharUnits::QuantityType>::getTombstoneKey();

return clang::CharUnits::fromQuantity(Quantity);
}

static unsigned getHashValue(const clang::CharUnits &CU) {
clang::CharUnits::QuantityType Quantity = CU.getQuantity();
return DenseMapInfo<clang::CharUnits::QuantityType>::getHashValue(Quantity);
}

static bool isEqual(const clang::CharUnits &LHS,
const clang::CharUnits &RHS) {
return LHS == RHS;
}
};

}

#endif
