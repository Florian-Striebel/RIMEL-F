
















#if !defined(LLVM_CLANG_AST_TYPEORDERING_H)
#define LLVM_CLANG_AST_TYPEORDERING_H

#include "clang/AST/CanonicalType.h"
#include "clang/AST/Type.h"
#include <functional>

namespace clang {


struct QualTypeOrdering {
bool operator()(QualType T1, QualType T2) const {
return std::less<void*>()(T1.getAsOpaquePtr(), T2.getAsOpaquePtr());
}
};

}

namespace llvm {
template<class> struct DenseMapInfo;

template<> struct DenseMapInfo<clang::QualType> {
static inline clang::QualType getEmptyKey() { return clang::QualType(); }

static inline clang::QualType getTombstoneKey() {
using clang::QualType;
return QualType::getFromOpaquePtr(reinterpret_cast<clang::Type *>(-1));
}

static unsigned getHashValue(clang::QualType Val) {
return (unsigned)((uintptr_t)Val.getAsOpaquePtr()) ^
((unsigned)((uintptr_t)Val.getAsOpaquePtr() >> 9));
}

static bool isEqual(clang::QualType LHS, clang::QualType RHS) {
return LHS == RHS;
}
};

template<> struct DenseMapInfo<clang::CanQualType> {
static inline clang::CanQualType getEmptyKey() {
return clang::CanQualType();
}

static inline clang::CanQualType getTombstoneKey() {
using clang::CanQualType;
return CanQualType::getFromOpaquePtr(reinterpret_cast<clang::Type *>(-1));
}

static unsigned getHashValue(clang::CanQualType Val) {
return (unsigned)((uintptr_t)Val.getAsOpaquePtr()) ^
((unsigned)((uintptr_t)Val.getAsOpaquePtr() >> 9));
}

static bool isEqual(clang::CanQualType LHS, clang::CanQualType RHS) {
return LHS == RHS;
}
};
}

#endif
