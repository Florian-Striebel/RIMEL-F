







#if !defined(__CLANG__CUDA_MATH_FORWARD_DECLARES_H__)
#define __CLANG__CUDA_MATH_FORWARD_DECLARES_H__
#if !defined(__CUDA__) && !__HIP__
#error "This file is for CUDA/HIP compilation only."
#endif








#pragma push_macro("__DEVICE__")
#define __DEVICE__ static __inline__ __attribute__((always_inline)) __attribute__((device))


__DEVICE__ long abs(long);
__DEVICE__ long long abs(long long);
__DEVICE__ double abs(double);
__DEVICE__ float abs(float);
__DEVICE__ int abs(int);
__DEVICE__ double acos(double);
__DEVICE__ float acos(float);
__DEVICE__ double acosh(double);
__DEVICE__ float acosh(float);
__DEVICE__ double asin(double);
__DEVICE__ float asin(float);
__DEVICE__ double asinh(double);
__DEVICE__ float asinh(float);
__DEVICE__ double atan2(double, double);
__DEVICE__ float atan2(float, float);
__DEVICE__ double atan(double);
__DEVICE__ float atan(float);
__DEVICE__ double atanh(double);
__DEVICE__ float atanh(float);
__DEVICE__ double cbrt(double);
__DEVICE__ float cbrt(float);
__DEVICE__ double ceil(double);
__DEVICE__ float ceil(float);
__DEVICE__ double copysign(double, double);
__DEVICE__ float copysign(float, float);
__DEVICE__ double cos(double);
__DEVICE__ float cos(float);
__DEVICE__ double cosh(double);
__DEVICE__ float cosh(float);
__DEVICE__ double erfc(double);
__DEVICE__ float erfc(float);
__DEVICE__ double erf(double);
__DEVICE__ float erf(float);
__DEVICE__ double exp2(double);
__DEVICE__ float exp2(float);
__DEVICE__ double exp(double);
__DEVICE__ float exp(float);
__DEVICE__ double expm1(double);
__DEVICE__ float expm1(float);
__DEVICE__ double fabs(double);
__DEVICE__ float fabs(float);
__DEVICE__ double fdim(double, double);
__DEVICE__ float fdim(float, float);
__DEVICE__ double floor(double);
__DEVICE__ float floor(float);
__DEVICE__ double fma(double, double, double);
__DEVICE__ float fma(float, float, float);
__DEVICE__ double fmax(double, double);
__DEVICE__ float fmax(float, float);
__DEVICE__ double fmin(double, double);
__DEVICE__ float fmin(float, float);
__DEVICE__ double fmod(double, double);
__DEVICE__ float fmod(float, float);
__DEVICE__ int fpclassify(double);
__DEVICE__ int fpclassify(float);
__DEVICE__ double frexp(double, int *);
__DEVICE__ float frexp(float, int *);
__DEVICE__ double hypot(double, double);
__DEVICE__ float hypot(float, float);
__DEVICE__ int ilogb(double);
__DEVICE__ int ilogb(float);
#if defined(_MSC_VER)
__DEVICE__ bool isfinite(long double);
#endif
__DEVICE__ bool isfinite(double);
__DEVICE__ bool isfinite(float);
__DEVICE__ bool isgreater(double, double);
__DEVICE__ bool isgreaterequal(double, double);
__DEVICE__ bool isgreaterequal(float, float);
__DEVICE__ bool isgreater(float, float);
#if defined(_MSC_VER)
__DEVICE__ bool isinf(long double);
#endif
__DEVICE__ bool isinf(double);
__DEVICE__ bool isinf(float);
__DEVICE__ bool isless(double, double);
__DEVICE__ bool islessequal(double, double);
__DEVICE__ bool islessequal(float, float);
__DEVICE__ bool isless(float, float);
__DEVICE__ bool islessgreater(double, double);
__DEVICE__ bool islessgreater(float, float);
#if defined(_MSC_VER)
__DEVICE__ bool isnan(long double);
#endif
__DEVICE__ bool isnan(double);
__DEVICE__ bool isnan(float);
__DEVICE__ bool isnormal(double);
__DEVICE__ bool isnormal(float);
__DEVICE__ bool isunordered(double, double);
__DEVICE__ bool isunordered(float, float);
__DEVICE__ long labs(long);
__DEVICE__ double ldexp(double, int);
__DEVICE__ float ldexp(float, int);
__DEVICE__ double lgamma(double);
__DEVICE__ float lgamma(float);
__DEVICE__ long long llabs(long long);
__DEVICE__ long long llrint(double);
__DEVICE__ long long llrint(float);
__DEVICE__ double log10(double);
__DEVICE__ float log10(float);
__DEVICE__ double log1p(double);
__DEVICE__ float log1p(float);
__DEVICE__ double log2(double);
__DEVICE__ float log2(float);
__DEVICE__ double logb(double);
__DEVICE__ float logb(float);
__DEVICE__ double log(double);
__DEVICE__ float log(float);
__DEVICE__ long lrint(double);
__DEVICE__ long lrint(float);
__DEVICE__ long lround(double);
__DEVICE__ long lround(float);
__DEVICE__ long long llround(float);
__DEVICE__ double modf(double, double *);
__DEVICE__ float modf(float, float *);
__DEVICE__ double nan(const char *);
__DEVICE__ float nanf(const char *);
__DEVICE__ double nearbyint(double);
__DEVICE__ float nearbyint(float);
__DEVICE__ double nextafter(double, double);
__DEVICE__ float nextafter(float, float);
__DEVICE__ double pow(double, double);
__DEVICE__ double pow(double, int);
__DEVICE__ float pow(float, float);
__DEVICE__ float pow(float, int);
__DEVICE__ double remainder(double, double);
__DEVICE__ float remainder(float, float);
__DEVICE__ double remquo(double, double, int *);
__DEVICE__ float remquo(float, float, int *);
__DEVICE__ double rint(double);
__DEVICE__ float rint(float);
__DEVICE__ double round(double);
__DEVICE__ float round(float);
__DEVICE__ double scalbln(double, long);
__DEVICE__ float scalbln(float, long);
__DEVICE__ double scalbn(double, int);
__DEVICE__ float scalbn(float, int);
#if defined(_MSC_VER)
__DEVICE__ bool signbit(long double);
#endif
__DEVICE__ bool signbit(double);
__DEVICE__ bool signbit(float);
__DEVICE__ double sin(double);
__DEVICE__ float sin(float);
__DEVICE__ double sinh(double);
__DEVICE__ float sinh(float);
__DEVICE__ double sqrt(double);
__DEVICE__ float sqrt(float);
__DEVICE__ double tan(double);
__DEVICE__ float tan(float);
__DEVICE__ double tanh(double);
__DEVICE__ float tanh(float);
__DEVICE__ double tgamma(double);
__DEVICE__ float tgamma(float);
__DEVICE__ double trunc(double);
__DEVICE__ float trunc(float);









#if defined(_LIBCPP_BEGIN_NAMESPACE_STD)
_LIBCPP_BEGIN_NAMESPACE_STD
#else
namespace std {
#if defined(_GLIBCXX_BEGIN_NAMESPACE_VERSION)
_GLIBCXX_BEGIN_NAMESPACE_VERSION
#endif
#endif

using ::abs;
using ::acos;
using ::acosh;
using ::asin;
using ::asinh;
using ::atan;
using ::atan2;
using ::atanh;
using ::cbrt;
using ::ceil;
using ::copysign;
using ::cos;
using ::cosh;
using ::erf;
using ::erfc;
using ::exp;
using ::exp2;
using ::expm1;
using ::fabs;
using ::fdim;
using ::floor;
using ::fma;
using ::fmax;
using ::fmin;
using ::fmod;
using ::fpclassify;
using ::frexp;
using ::hypot;
using ::ilogb;
using ::isfinite;
using ::isgreater;
using ::isgreaterequal;
using ::isinf;
using ::isless;
using ::islessequal;
using ::islessgreater;
using ::isnan;
using ::isnormal;
using ::isunordered;
using ::labs;
using ::ldexp;
using ::lgamma;
using ::llabs;
using ::llrint;
using ::log;
using ::log10;
using ::log1p;
using ::log2;
using ::logb;
using ::lrint;
using ::lround;
using ::llround;
using ::modf;
using ::nan;
using ::nanf;
using ::nearbyint;
using ::nextafter;
using ::pow;
using ::remainder;
using ::remquo;
using ::rint;
using ::round;
using ::scalbln;
using ::scalbn;
using ::signbit;
using ::sin;
using ::sinh;
using ::sqrt;
using ::tan;
using ::tanh;
using ::tgamma;
using ::trunc;

#if defined(_LIBCPP_END_NAMESPACE_STD)
_LIBCPP_END_NAMESPACE_STD
#else
#if defined(_GLIBCXX_BEGIN_NAMESPACE_VERSION)
_GLIBCXX_END_NAMESPACE_VERSION
#endif
}
#endif

#pragma pop_macro("__DEVICE__")

#endif
