

















#if !defined(INT_MATH_H)
#define INT_MATH_H

#if !defined(__has_builtin)
#define __has_builtin(x) 0
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#include <math.h>
#include <stdlib.h>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define CRT_INFINITY INFINITY
#else
#define CRT_INFINITY __builtin_huge_valf()
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define crt_isfinite(x) _finite((x))
#define crt_isinf(x) !_finite((x))
#define crt_isnan(x) _isnan((x))
#else



#if __has_builtin(__builtin_isfinite)
#define crt_isfinite(x) __builtin_isfinite((x))
#elif defined(__GNUC__)
#define crt_isfinite(x) __extension__(({ __typeof((x)) x_ = (x); !crt_isinf(x_) && !crt_isnan(x_); }))




#else
#error "Do not know how to check for infinity"
#endif
#define crt_isinf(x) __builtin_isinf((x))
#define crt_isnan(x) __builtin_isnan((x))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define crt_copysign(x, y) copysign((x), (y))
#define crt_copysignf(x, y) copysignf((x), (y))
#define crt_copysignl(x, y) copysignl((x), (y))
#else
#define crt_copysign(x, y) __builtin_copysign((x), (y))
#define crt_copysignf(x, y) __builtin_copysignf((x), (y))
#define crt_copysignl(x, y) __builtin_copysignl((x), (y))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define crt_fabs(x) fabs((x))
#define crt_fabsf(x) fabsf((x))
#define crt_fabsl(x) fabs((x))
#else
#define crt_fabs(x) __builtin_fabs((x))
#define crt_fabsf(x) __builtin_fabsf((x))
#define crt_fabsl(x) __builtin_fabsl((x))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define crt_fmaxl(x, y) __max((x), (y))
#else
#define crt_fmaxl(x, y) __builtin_fmaxl((x), (y))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define crt_logbl(x) logbl((x))
#else
#define crt_logbl(x) __builtin_logbl((x))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define crt_scalbnl(x, y) scalbnl((x), (y))
#else
#define crt_scalbnl(x, y) __builtin_scalbnl((x), (y))
#endif

#endif
