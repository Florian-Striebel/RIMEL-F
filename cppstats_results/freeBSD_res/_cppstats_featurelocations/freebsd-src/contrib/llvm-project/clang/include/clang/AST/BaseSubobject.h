











#if !defined(LLVM_CLANG_AST_BASESUBOBJECT_H)
#define LLVM_CLANG_AST_BASESUBOBJECT_H

#include "clang/AST/CharUnits.h"
#include "clang/AST/DeclCXX.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Support/type_traits.h"
#include <cstdint>
#include <utility>

namespace clang {

class CXXRecordDecl;




class BaseSubobject {

const CXXRecordDecl *Base;


CharUnits BaseOffset;

public:
BaseSubobject() = default;
BaseSubobject(const CXXRecordDecl *Base, CharUnits BaseOffset)
: Base(Base), BaseOffset(BaseOffset) {}


const CXXRecordDecl *getBase() const { return Base; }


CharUnits getBaseOffset() const { return BaseOffset; }

friend bool operator==(const BaseSubobject &LHS, const BaseSubobject &RHS) {
return LHS.Base == RHS.Base && LHS.BaseOffset == RHS.BaseOffset;
}
};

}

namespace llvm {

template<> struct DenseMapInfo<clang::BaseSubobject> {
static clang::BaseSubobject getEmptyKey() {
return clang::BaseSubobject(
DenseMapInfo<const clang::CXXRecordDecl *>::getEmptyKey(),
clang::CharUnits::fromQuantity(DenseMapInfo<int64_t>::getEmptyKey()));
}

static clang::BaseSubobject getTombstoneKey() {
return clang::BaseSubobject(
DenseMapInfo<const clang::CXXRecordDecl *>::getTombstoneKey(),
clang::CharUnits::fromQuantity(DenseMapInfo<int64_t>::getTombstoneKey()));
}

static unsigned getHashValue(const clang::BaseSubobject &Base) {
using PairTy = std::pair<const clang::CXXRecordDecl *, clang::CharUnits>;

return DenseMapInfo<PairTy>::getHashValue(PairTy(Base.getBase(),
Base.getBaseOffset()));
}

static bool isEqual(const clang::BaseSubobject &LHS,
const clang::BaseSubobject &RHS) {
return LHS == RHS;
}
};

}

#endif
