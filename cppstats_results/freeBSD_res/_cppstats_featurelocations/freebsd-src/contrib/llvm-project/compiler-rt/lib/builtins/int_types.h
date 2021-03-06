














#if !defined(INT_TYPES_H)
#define INT_TYPES_H

#include "int_endianness.h"


#if defined(si_int)
#undef si_int
#endif
typedef int32_t si_int;
typedef uint32_t su_int;
#if UINT_MAX == 0xFFFFFFFF
#define clzsi __builtin_clz
#define ctzsi __builtin_ctz
#elif ULONG_MAX == 0xFFFFFFFF
#define clzsi __builtin_clzl
#define ctzsi __builtin_ctzl
#else
#error could not determine appropriate clzsi macro for this system
#endif

typedef int64_t di_int;
typedef uint64_t du_int;

typedef union {
di_int all;
struct {
#if _YUGA_LITTLE_ENDIAN
su_int low;
si_int high;
#else
si_int high;
su_int low;
#endif
} s;
} dwords;

typedef union {
du_int all;
struct {
#if _YUGA_LITTLE_ENDIAN
su_int low;
su_int high;
#else
su_int high;
su_int low;
#endif
} s;
} udwords;

#if defined(__LP64__) || defined(__wasm__) || defined(__mips64) || defined(__riscv) || defined(_WIN64)

#define CRT_HAS_128BIT
#endif




#if defined(_MSC_VER) && !defined(__clang__)
#undef CRT_HAS_128BIT
#endif

#if defined(CRT_HAS_128BIT)
typedef int ti_int __attribute__((mode(TI)));
typedef unsigned tu_int __attribute__((mode(TI)));

typedef union {
ti_int all;
struct {
#if _YUGA_LITTLE_ENDIAN
du_int low;
di_int high;
#else
di_int high;
du_int low;
#endif
} s;
} twords;

typedef union {
tu_int all;
struct {
#if _YUGA_LITTLE_ENDIAN
du_int low;
du_int high;
#else
du_int high;
du_int low;
#endif
} s;
} utwords;

static __inline ti_int make_ti(di_int h, di_int l) {
twords r;
r.s.high = h;
r.s.low = l;
return r.all;
}

static __inline tu_int make_tu(du_int h, du_int l) {
utwords r;
r.s.high = h;
r.s.low = l;
return r.all;
}

#endif



#if defined(__FreeBSD__) && defined(_STANDALONE)
#define CRT_HAS_FLOATING_POINT 0
#else
#define CRT_HAS_FLOATING_POINT 1
#endif

#if CRT_HAS_FLOATING_POINT
typedef union {
su_int u;
float f;
} float_bits;

typedef union {
udwords u;
double f;
} double_bits;
#endif

typedef struct {
#if _YUGA_LITTLE_ENDIAN
udwords low;
udwords high;
#else
udwords high;
udwords low;
#endif
} uqwords;






#if (defined(__i386__) || defined(__x86_64__)) && !(defined(_MSC_VER) || defined(__ANDROID__))

#define HAS_80_BIT_LONG_DOUBLE 1
#elif defined(__m68k__) || defined(__ia64__)
#define HAS_80_BIT_LONG_DOUBLE 1
#else
#define HAS_80_BIT_LONG_DOUBLE 0
#endif

#if CRT_HAS_FLOATING_POINT
typedef union {
uqwords u;
long double f;
} long_double_bits;

#if __STDC_VERSION__ >= 199901L
typedef float _Complex Fcomplex;
typedef double _Complex Dcomplex;
typedef long double _Complex Lcomplex;

#define COMPLEX_REAL(x) __real__(x)
#define COMPLEX_IMAGINARY(x) __imag__(x)
#else
typedef struct {
float real, imaginary;
} Fcomplex;

typedef struct {
double real, imaginary;
} Dcomplex;

typedef struct {
long double real, imaginary;
} Lcomplex;

#define COMPLEX_REAL(x) (x).real
#define COMPLEX_IMAGINARY(x) (x).imaginary
#endif
#endif
#endif
