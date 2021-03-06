








#if !defined(__CLANG_OPENMP_DEVICE_FUNCTIONS_H__)
#define __CLANG_OPENMP_DEVICE_FUNCTIONS_H__

#if !defined(_OPENMP)
#error "This file is for OpenMP compilation only."
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#pragma omp begin declare variant match( device = {arch(nvptx, nvptx64)}, implementation = {extension(match_any)})


#define __CUDA__
#define __OPENMP_NVPTX__


#include <__clang_cuda_libdevice_declares.h>


#include <__clang_cuda_device_functions.h>

#undef __OPENMP_NVPTX__
#undef __CUDA__

#pragma omp end declare variant

#if defined(__AMDGCN__)
#pragma omp begin declare variant match(device = {arch(amdgcn)})


#if !defined(__cplusplus)
#include <stdbool.h>
#include <stdint.h>
#endif

#define __OPENMP_AMDGCN__
#pragma push_macro("__device__")
#define __device__


#include <__clang_hip_libdevice_declares.h>

#pragma pop_macro("__device__")
#undef __OPENMP_AMDGCN__

#pragma omp end declare variant
#endif

#if defined(__cplusplus)
}
#endif



#if defined(__cplusplus)


#include <cstdlib>

#pragma push_macro("OPENMP_NOEXCEPT")
#if __cplusplus >= 201103L
#define OPENMP_NOEXCEPT noexcept
#else
#define OPENMP_NOEXCEPT
#endif


inline void *operator new(__SIZE_TYPE__ size) {
if (size == 0)
size = 1;
return ::malloc(size);
}

inline void *operator new[](__SIZE_TYPE__ size) { return ::operator new(size); }

inline void operator delete(void *ptr)OPENMP_NOEXCEPT { ::free(ptr); }

inline void operator delete[](void *ptr) OPENMP_NOEXCEPT {
::operator delete(ptr);
}


#if __cplusplus >= 201402L
inline void operator delete(void *ptr, __SIZE_TYPE__ size)OPENMP_NOEXCEPT {
::operator delete(ptr);
}
inline void operator delete[](void *ptr, __SIZE_TYPE__ size) OPENMP_NOEXCEPT {
::operator delete(ptr);
}
#endif

#pragma pop_macro("OPENMP_NOEXCEPT")
#endif

#endif
