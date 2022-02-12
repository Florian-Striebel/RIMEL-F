








#if !defined(__POPCNTINTRIN_H)
#define __POPCNTINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("popcnt")))

#if defined(__cplusplus) && (__cplusplus >= 201103L)
#define __DEFAULT_FN_ATTRS_CONSTEXPR __DEFAULT_FN_ATTRS constexpr
#else
#define __DEFAULT_FN_ATTRS_CONSTEXPR __DEFAULT_FN_ATTRS
#endif











static __inline__ int __DEFAULT_FN_ATTRS_CONSTEXPR
_mm_popcnt_u32(unsigned int __A)
{
return __builtin_popcount(__A);
}

#if defined(__x86_64__)










static __inline__ long long __DEFAULT_FN_ATTRS_CONSTEXPR
_mm_popcnt_u64(unsigned long long __A)
{
return __builtin_popcountll(__A);
}
#endif

#undef __DEFAULT_FN_ATTRS
#undef __DEFAULT_FN_ATTRS_CONSTEXPR

#endif
