








#if !defined(__CLANG_OPENMP_COMPLEX_H__)
#define __CLANG_OPENMP_COMPLEX_H__

#if !defined(_OPENMP)
#error "This file is for OpenMP compilation only."
#endif


#include <math.h>

#define __OPENMP_NVPTX__
#include <__clang_cuda_complex_builtins.h>
#undef __OPENMP_NVPTX__
#endif


#include_next <complex.h>
