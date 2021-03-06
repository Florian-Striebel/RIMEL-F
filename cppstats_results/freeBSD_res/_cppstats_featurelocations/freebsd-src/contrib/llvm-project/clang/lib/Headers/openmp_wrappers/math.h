

















#if defined(__cplusplus)
#include <cmath>
#endif

#if !defined(__CLANG_OPENMP_MATH_H__)
#define __CLANG_OPENMP_MATH_H__

#if !defined(_OPENMP)
#error "This file is for OpenMP compilation only."
#endif

#include_next <math.h>



#include <limits.h>



#include <stdlib.h>

#pragma omp begin declare variant match( device = {arch(nvptx, nvptx64)}, implementation = {extension(match_any)})


#define __CUDA__
#define __OPENMP_NVPTX__
#include <__clang_cuda_math.h>
#undef __OPENMP_NVPTX__
#undef __CUDA__

#pragma omp end declare variant

#if defined(__AMDGCN__)
#pragma omp begin declare variant match(device = {arch(amdgcn)})

#define __OPENMP_AMDGCN__
#include <__clang_hip_math.h>
#undef __OPENMP_AMDGCN__

#pragma omp end declare variant
#endif

#endif
