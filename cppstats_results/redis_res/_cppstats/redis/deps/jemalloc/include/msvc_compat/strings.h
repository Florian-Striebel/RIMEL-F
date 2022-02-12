#if defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
static __forceinline int ffsl(long x) {
unsigned long i;
if (_BitScanForward(&i, x)) {
return i + 1;
}
return 0;
}
static __forceinline int ffs(int x) {
return ffsl(x);
}
#if defined(_M_X64)
#pragma intrinsic(_BitScanForward64)
#endif
static __forceinline int ffsll(unsigned __int64 x) {
unsigned long i;
#if defined(_M_X64)
if (_BitScanForward64(&i, x)) {
return i + 1;
}
return 0;
#else
union {
unsigned __int64 ll;
unsigned long l[2];
} s;
s.ll = x;
if (_BitScanForward(&i, s.l[0])) {
return i + 1;
} else if(_BitScanForward(&i, s.l[1])) {
return i + 33;
}
return 0;
#endif
}
#else
#define ffsll(x) __builtin_ffsll(x)
#define ffsl(x) __builtin_ffsl(x)
#define ffs(x) __builtin_ffs(x)
#endif
