












#if !defined(LLVM_CLANG_C_CXSTRING_H)
#define LLVM_CLANG_C_CXSTRING_H

#include "clang-c/ExternC.h"
#include "clang-c/Platform.h"

LLVM_CLANG_C_EXTERN_C_BEGIN
















typedef struct {
const void *data;
unsigned private_flags;
} CXString;

typedef struct {
CXString *Strings;
unsigned Count;
} CXStringSet;




CINDEX_LINKAGE const char *clang_getCString(CXString string);




CINDEX_LINKAGE void clang_disposeString(CXString string);




CINDEX_LINKAGE void clang_disposeStringSet(CXStringSet *set);





LLVM_CLANG_C_EXTERN_C_END

#endif

