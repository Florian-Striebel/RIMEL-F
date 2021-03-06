








#if !defined(__X86INTRIN_H)
#error "Never use <ia32intrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__IA32INTRIN_H)
#define __IA32INTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__))
#define __DEFAULT_FN_ATTRS_SSE42 __attribute__((__always_inline__, __nodebug__, __target__("sse4.2")))

#if defined(__cplusplus) && (__cplusplus >= 201103L)
#define __DEFAULT_FN_ATTRS_CAST __attribute__((__always_inline__)) constexpr
#define __DEFAULT_FN_ATTRS_CONSTEXPR __DEFAULT_FN_ATTRS constexpr
#else
#define __DEFAULT_FN_ATTRS_CAST __attribute__((__always_inline__))
#define __DEFAULT_FN_ATTRS_CONSTEXPR __DEFAULT_FN_ATTRS
#endif













static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
__bsfd(int __A) {
return __builtin_ctz(__A);
}













static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
__bsrd(int __A) {
return 31 - __builtin_clz(__A);
}












static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
__bswapd(int __A) {
return __builtin_bswap32(__A);
}

static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
_bswap(int __A) {
return __builtin_bswap32(__A);
}

#define _bit_scan_forward(A) __bsfd((A))
#define _bit_scan_reverse(A) __bsrd((A))

#if defined(__x86_64__)












static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
__bsfq(long long __A) {
return __builtin_ctzll(__A);
}













static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
__bsrq(long long __A) {
return 63 - __builtin_clzll(__A);
}












static __inline__ long long __DEFAULT_FN_ATTRS_CONSTEXPR
__bswapq(long long __A) {
return __builtin_bswap64(__A);
}

#define _bswap64(A) __bswapq((A))
#endif













static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
__popcntd(unsigned int __A)
{
return __builtin_popcount(__A);
}

#define _popcnt32(A) __popcntd((A))

#if defined(__x86_64__)












static __inline__ long long __DEFAULT_FN_ATTRS_CONSTEXPR
__popcntq(unsigned long long __A)
{
return __builtin_popcountll(__A);
}

#define _popcnt64(A) __popcntq((A))
#endif

#if defined(__x86_64__)
static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__readeflags(void)
{
return __builtin_ia32_readeflags_u64();
}

static __inline__ void __DEFAULT_FN_ATTRS
__writeeflags(unsigned long long __f)
{
__builtin_ia32_writeeflags_u64(__f);
}

#else
static __inline__ unsigned int __DEFAULT_FN_ATTRS
__readeflags(void)
{
return __builtin_ia32_readeflags_u32();
}

static __inline__ void __DEFAULT_FN_ATTRS
__writeeflags(unsigned int __f)
{
__builtin_ia32_writeeflags_u32(__f);
}
#endif











static __inline__ unsigned int __DEFAULT_FN_ATTRS_CAST
_castf32_u32(float __A) {
return __builtin_bit_cast(unsigned int, __A);
}











static __inline__ unsigned long long __DEFAULT_FN_ATTRS_CAST
_castf64_u64(double __A) {
return __builtin_bit_cast(unsigned long long, __A);
}











static __inline__ float __DEFAULT_FN_ATTRS_CAST
_castu32_f32(unsigned int __A) {
return __builtin_bit_cast(float, __A);
}











static __inline__ double __DEFAULT_FN_ATTRS_CAST
_castu64_f64(unsigned long long __A) {
return __builtin_bit_cast(double, __A);
}
















static __inline__ unsigned int __DEFAULT_FN_ATTRS_SSE42
__crc32b(unsigned int __C, unsigned char __D)
{
return __builtin_ia32_crc32qi(__C, __D);
}
















static __inline__ unsigned int __DEFAULT_FN_ATTRS_SSE42
__crc32w(unsigned int __C, unsigned short __D)
{
return __builtin_ia32_crc32hi(__C, __D);
}
















static __inline__ unsigned int __DEFAULT_FN_ATTRS_SSE42
__crc32d(unsigned int __C, unsigned int __D)
{
return __builtin_ia32_crc32si(__C, __D);
}

#if defined(__x86_64__)















static __inline__ unsigned long long __DEFAULT_FN_ATTRS_SSE42
__crc32q(unsigned long long __C, unsigned long long __D)
{
return __builtin_ia32_crc32di(__C, __D);
}
#endif

static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__rdpmc(int __A) {
return __builtin_ia32_rdpmc(__A);
}


static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__rdtscp(unsigned int *__A) {
return __builtin_ia32_rdtscp(__A);
}

#define _rdtsc() __rdtsc()

#define _rdpmc(A) __rdpmc(A)

static __inline__ void __DEFAULT_FN_ATTRS
_wbinvd(void) {
__builtin_ia32_wbinvd();
}

static __inline__ unsigned char __DEFAULT_FN_ATTRS_CONSTEXPR
__rolb(unsigned char __X, int __C) {
return __builtin_rotateleft8(__X, __C);
}

static __inline__ unsigned char __DEFAULT_FN_ATTRS_CONSTEXPR
__rorb(unsigned char __X, int __C) {
return __builtin_rotateright8(__X, __C);
}

static __inline__ unsigned short __DEFAULT_FN_ATTRS_CONSTEXPR
__rolw(unsigned short __X, int __C) {
return __builtin_rotateleft16(__X, __C);
}

static __inline__ unsigned short __DEFAULT_FN_ATTRS_CONSTEXPR
__rorw(unsigned short __X, int __C) {
return __builtin_rotateright16(__X, __C);
}

static __inline__ unsigned int __DEFAULT_FN_ATTRS_CONSTEXPR
__rold(unsigned int __X, int __C) {
return __builtin_rotateleft32(__X, __C);
}

static __inline__ unsigned int __DEFAULT_FN_ATTRS_CONSTEXPR
__rord(unsigned int __X, int __C) {
return __builtin_rotateright32(__X, __C);
}

#if defined(__x86_64__)
static __inline__ unsigned long long __DEFAULT_FN_ATTRS_CONSTEXPR
__rolq(unsigned long long __X, int __C) {
return __builtin_rotateleft64(__X, __C);
}

static __inline__ unsigned long long __DEFAULT_FN_ATTRS_CONSTEXPR
__rorq(unsigned long long __X, int __C) {
return __builtin_rotateright64(__X, __C);
}
#endif

#if !defined(_MSC_VER)


#if defined(__LP64__)
#define _lrotl(a,b) __rolq((a), (b))
#define _lrotr(a,b) __rorq((a), (b))
#else
#define _lrotl(a,b) __rold((a), (b))
#define _lrotr(a,b) __rord((a), (b))
#endif
#define _rotl(a,b) __rold((a), (b))
#define _rotr(a,b) __rord((a), (b))
#endif


#define _rotwl(a,b) __rolw((a), (b))
#define _rotwr(a,b) __rorw((a), (b))

#undef __DEFAULT_FN_ATTRS
#undef __DEFAULT_FN_ATTRS_CAST
#undef __DEFAULT_FN_ATTRS_SSE42
#undef __DEFAULT_FN_ATTRS_CONSTEXPR

#endif
