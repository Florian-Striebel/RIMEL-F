













#if !defined(LLVM_CLANG_BASIC_ABI_H)
#define LLVM_CLANG_BASIC_ABI_H

#include "llvm/Support/DataTypes.h"
#include <cstring>

namespace clang {


enum CXXCtorType {
Ctor_Complete,
Ctor_Base,
Ctor_Comdat,
Ctor_CopyingClosure,
Ctor_DefaultClosure,
};


enum CXXDtorType {
Dtor_Deleting,
Dtor_Complete,
Dtor_Base,
Dtor_Comdat
};

}

#endif
