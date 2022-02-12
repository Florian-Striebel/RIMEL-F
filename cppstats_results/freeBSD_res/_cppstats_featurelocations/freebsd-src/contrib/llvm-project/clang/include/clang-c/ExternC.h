












#if !defined(LLVM_CLANG_C_EXTERN_C_H)
#define LLVM_CLANG_C_EXTERN_C_H

#if defined(__clang__)
#define LLVM_CLANG_C_STRICT_PROTOTYPES_BEGIN _Pragma("clang diagnostic push") _Pragma("clang diagnostic error \"-Wstrict-prototypes\"")


#define LLVM_CLANG_C_STRICT_PROTOTYPES_END _Pragma("clang diagnostic pop")
#else
#define LLVM_CLANG_C_STRICT_PROTOTYPES_BEGIN
#define LLVM_CLANG_C_STRICT_PROTOTYPES_END
#endif

#if defined(__cplusplus)
#define LLVM_CLANG_C_EXTERN_C_BEGIN extern "C" { LLVM_CLANG_C_STRICT_PROTOTYPES_BEGIN


#define LLVM_CLANG_C_EXTERN_C_END LLVM_CLANG_C_STRICT_PROTOTYPES_END }


#else
#define LLVM_CLANG_C_EXTERN_C_BEGIN LLVM_CLANG_C_STRICT_PROTOTYPES_BEGIN
#define LLVM_CLANG_C_EXTERN_C_END LLVM_CLANG_C_STRICT_PROTOTYPES_END
#endif

#endif
