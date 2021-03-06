












#if !defined(LLVM_CLANG_AST_OPTIONALDIAGNOSTIC_H)
#define LLVM_CLANG_AST_OPTIONALDIAGNOSTIC_H

#include "clang/AST/APValue.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace clang {



class OptionalDiagnostic {
PartialDiagnostic *Diag;

public:
explicit OptionalDiagnostic(PartialDiagnostic *Diag = nullptr) : Diag(Diag) {}

template <typename T> OptionalDiagnostic &operator<<(const T &v) {
if (Diag)
*Diag << v;
return *this;
}

OptionalDiagnostic &operator<<(const llvm::APSInt &I) {
if (Diag) {
SmallVector<char, 32> Buffer;
I.toString(Buffer);
*Diag << StringRef(Buffer.data(), Buffer.size());
}
return *this;
}

OptionalDiagnostic &operator<<(const llvm::APFloat &F) {
if (Diag) {






unsigned precision = llvm::APFloat::semanticsPrecision(F.getSemantics());
precision = (precision * 59 + 195) / 196;
SmallVector<char, 32> Buffer;
F.toString(Buffer, precision);
*Diag << StringRef(Buffer.data(), Buffer.size());
}
return *this;
}

OptionalDiagnostic &operator<<(const llvm::APFixedPoint &FX) {
if (Diag) {
SmallVector<char, 32> Buffer;
FX.toString(Buffer);
*Diag << StringRef(Buffer.data(), Buffer.size());
}
return *this;
}
};

}

#endif
