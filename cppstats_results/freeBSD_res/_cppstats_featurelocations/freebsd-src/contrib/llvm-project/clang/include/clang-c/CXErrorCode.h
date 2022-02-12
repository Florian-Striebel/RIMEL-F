












#if !defined(LLVM_CLANG_C_CXERRORCODE_H)
#define LLVM_CLANG_C_CXERRORCODE_H

#include "clang-c/ExternC.h"
#include "clang-c/Platform.h"

LLVM_CLANG_C_EXTERN_C_BEGIN







enum CXErrorCode {



CXError_Success = 0,







CXError_Failure = 1,




CXError_Crashed = 2,





CXError_InvalidArguments = 3,




CXError_ASTReadError = 4
};

LLVM_CLANG_C_EXTERN_C_END

#endif

