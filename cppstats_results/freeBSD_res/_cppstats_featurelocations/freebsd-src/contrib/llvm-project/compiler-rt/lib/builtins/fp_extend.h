












#if !defined(FP_EXTEND_HEADER)
#define FP_EXTEND_HEADER

#include "int_lib.h"

#if defined SRC_SINGLE
typedef float src_t;
typedef uint32_t src_rep_t;
#define SRC_REP_C UINT32_C
static const int srcSigBits = 23;
#define src_rep_t_clz clzsi

#elif defined SRC_DOUBLE
typedef double src_t;
typedef uint64_t src_rep_t;
#define SRC_REP_C UINT64_C
static const int srcSigBits = 52;
static __inline int src_rep_t_clz(src_rep_t a) {
#if defined __LP64__
return __builtin_clzl(a);
#else
if (a & REP_C(0xffffffff00000000))
return __builtin_clz(a >> 32);
else
return 32 + __builtin_clz(a & REP_C(0xffffffff));
#endif
}

#elif defined SRC_HALF
#if defined(COMPILER_RT_HAS_FLOAT16)
typedef _Float16 src_t;
#else
typedef uint16_t src_t;
#endif
typedef uint16_t src_rep_t;
#define SRC_REP_C UINT16_C
static const int srcSigBits = 10;
#define src_rep_t_clz __builtin_clz

#else
#error Source should be half, single, or double precision!
#endif

#if defined DST_SINGLE
typedef float dst_t;
typedef uint32_t dst_rep_t;
#define DST_REP_C UINT32_C
static const int dstSigBits = 23;

#elif defined DST_DOUBLE
typedef double dst_t;
typedef uint64_t dst_rep_t;
#define DST_REP_C UINT64_C
static const int dstSigBits = 52;

#elif defined DST_QUAD
typedef long double dst_t;
typedef __uint128_t dst_rep_t;
#define DST_REP_C (__uint128_t)
static const int dstSigBits = 112;

#else
#error Destination should be single, double, or quad precision!
#endif




static __inline src_rep_t srcToRep(src_t x) {
const union {
src_t f;
src_rep_t i;
} rep = {.f = x};
return rep.i;
}

static __inline dst_t dstFromRep(dst_rep_t x) {
const union {
dst_t f;
dst_rep_t i;
} rep = {.i = x};
return rep.f;
}


#endif
