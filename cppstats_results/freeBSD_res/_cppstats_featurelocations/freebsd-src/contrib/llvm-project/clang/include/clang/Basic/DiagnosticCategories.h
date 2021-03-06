







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICCATEGORIES_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICCATEGORIES_H

namespace clang {
namespace diag {
enum {
#define GET_CATEGORY_TABLE
#define CATEGORY(X, ENUM) ENUM,
#include "clang/Basic/DiagnosticGroups.inc"
#undef CATEGORY
#undef GET_CATEGORY_TABLE
DiagCat_NUM_CATEGORIES
};
}
}

#endif
