








#if !defined(__CLANG_OPENMP_TIME_H__)
#define __CLANG_OPENMP_TIME_H__

#if !defined(_OPENMP)
#error "This file is for OpenMP compilation only."
#endif

#if defined(__cplusplus)
#define __DEVICE__ static constexpr __attribute__((always_inline, nothrow))
#else
#define __DEVICE__ static __attribute__((always_inline, nothrow))
#endif

#include_next <time.h>

#pragma omp begin declare variant match( device = {arch(nvptx, nvptx64)}, implementation = {extension(match_any)})


__DEVICE__ clock_t clock() { return __nvvm_read_ptx_sreg_clock(); }

#pragma omp end declare variant

#endif
