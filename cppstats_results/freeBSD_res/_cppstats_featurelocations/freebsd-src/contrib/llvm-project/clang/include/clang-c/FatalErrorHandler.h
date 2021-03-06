








#if !defined(LLVM_CLANG_C_FATAL_ERROR_HANDLER_H)
#define LLVM_CLANG_C_FATAL_ERROR_HANDLER_H

#include "clang-c/ExternC.h"

LLVM_CLANG_C_EXTERN_C_BEGIN





void clang_install_aborting_llvm_fatal_error_handler(void);






void clang_uninstall_llvm_fatal_error_handler(void);

LLVM_CLANG_C_EXTERN_C_END

#endif
