














#if !defined(__CLANG_HIP_RUNTIME_WRAPPER_H__)
#define __CLANG_HIP_RUNTIME_WRAPPER_H__

#if __HIP__

#define __host__ __attribute__((host))
#define __device__ __attribute__((device))
#define __global__ __attribute__((global))
#define __shared__ __attribute__((shared))
#define __constant__ __attribute__((constant))
#define __managed__ __attribute__((managed))

#if !defined(__cplusplus) || __cplusplus < 201103L
#define nullptr NULL;
#endif

#if defined(__cplusplus)
extern "C" {
__attribute__((__visibility__("default")))
__attribute__((weak))
__attribute__((noreturn))
__device__ void __cxa_pure_virtual(void) {
__builtin_trap();
}
__attribute__((__visibility__("default")))
__attribute__((weak))
__attribute__((noreturn))
__device__ void __cxa_deleted_virtual(void) {
__builtin_trap();
}
}
#endif

#if !defined(__HIPCC_RTC__)
#include <cmath>
#include <cstdlib>
#include <stdlib.h>
#else
typedef __SIZE_TYPE__ size_t;




#pragma push_macro("NULL")
#pragma push_macro("uint32_t")
#pragma push_macro("uint64_t")
#pragma push_macro("CHAR_BIT")
#pragma push_macro("INT_MAX")
#define NULL (void *)0
#define uint32_t __UINT32_TYPE__
#define uint64_t __UINT64_TYPE__
#define CHAR_BIT __CHAR_BIT__
#define INT_MAX __INTMAX_MAX__
#endif

typedef __SIZE_TYPE__ __hip_size_t;

#if defined(__cplusplus)
extern "C" {
#endif

#if __HIP_ENABLE_DEVICE_MALLOC__
__device__ void *__hip_malloc(__hip_size_t __size);
__device__ void *__hip_free(void *__ptr);
__attribute__((weak)) inline __device__ void *malloc(__hip_size_t __size) {
return __hip_malloc(__size);
}
__attribute__((weak)) inline __device__ void *free(void *__ptr) {
return __hip_free(__ptr);
}
#else
__attribute__((weak)) inline __device__ void *malloc(__hip_size_t __size) {
__builtin_trap();
return (void *)0;
}
__attribute__((weak)) inline __device__ void *free(void *__ptr) {
__builtin_trap();
return (void *)0;
}
#endif

#if defined(__cplusplus)
}
#endif

#include <__clang_hip_libdevice_declares.h>
#include <__clang_hip_math.h>

#if defined(__HIPCC_RTC__)
#include <__clang_hip_cmath.h>
#else
#include <__clang_cuda_math_forward_declares.h>
#include <__clang_hip_cmath.h>
#include <__clang_cuda_complex_builtins.h>
#include <algorithm>
#include <complex>
#include <new>
#endif

#define __CLANG_HIP_RUNTIME_WRAPPER_INCLUDED__ 1
#if defined(__HIPCC_RTC__)
#pragma pop_macro("NULL")
#pragma pop_macro("uint32_t")
#pragma pop_macro("uint64_t")
#pragma pop_macro("CHAR_BIT")
#pragma pop_macro("INT_MAX")
#endif
#endif
#endif
