






















#if !defined(__IMMINTRIN_H)
#error "Never use <keylockerintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(_KEYLOCKERINTRIN_H)
#define _KEYLOCKERINTRIN_H

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) || defined(__KL__)



#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("kl"),__min_vector_width__(128)))



























































static __inline__ void __DEFAULT_FN_ATTRS
_mm_loadiwkey (unsigned int __ctl, __m128i __intkey,
__m128i __enkey_lo, __m128i __enkey_hi) {
__builtin_ia32_loadiwkey (__intkey, __enkey_lo, __enkey_hi, __ctl);
}

































static __inline__ unsigned int __DEFAULT_FN_ATTRS
_mm_encodekey128_u32(unsigned int __htype, __m128i __key, void *__h) {
return __builtin_ia32_encodekey128_u32(__htype, (__v2di)__key, __h);
}




































static __inline__ unsigned int __DEFAULT_FN_ATTRS
_mm_encodekey256_u32(unsigned int __htype, __m128i __key_lo, __m128i __key_hi,
void *__h) {
return __builtin_ia32_encodekey256_u32(__htype, (__v2di)__key_lo,
(__v2di)__key_hi, __h);
}

































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesenc128kl_u8(__m128i* __odata, __m128i __idata, const void *__h) {
return __builtin_ia32_aesenc128kl_u8((__v2di *)__odata, (__v2di)__idata, __h);
}



































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesenc256kl_u8(__m128i* __odata, __m128i __idata, const void *__h) {
return __builtin_ia32_aesenc256kl_u8((__v2di *)__odata, (__v2di)__idata, __h);
}



































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesdec128kl_u8(__m128i* __odata, __m128i __idata, const void *__h) {
return __builtin_ia32_aesdec128kl_u8((__v2di *)__odata, (__v2di)__idata, __h);
}



































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesdec256kl_u8(__m128i* __odata, __m128i __idata, const void *__h) {
return __builtin_ia32_aesdec256kl_u8((__v2di *)__odata, (__v2di)__idata, __h);
}

#undef __DEFAULT_FN_ATTRS

#endif


#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) || defined(__WIDEKL__)



#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("kl,widekl"),__min_vector_width__(128)))











































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesencwide128kl_u8(__m128i __odata[8], const __m128i __idata[8], const void* __h) {
return __builtin_ia32_aesencwide128kl_u8((__v2di *)__odata,
(const __v2di *)__idata, __h);
}









































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesencwide256kl_u8(__m128i __odata[8], const __m128i __idata[8], const void* __h) {
return __builtin_ia32_aesencwide256kl_u8((__v2di *)__odata,
(const __v2di *)__idata, __h);
}









































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesdecwide128kl_u8(__m128i __odata[8], const __m128i __idata[8], const void* __h) {
return __builtin_ia32_aesdecwide128kl_u8((__v2di *)__odata,
(const __v2di *)__idata, __h);
}









































static __inline__ unsigned char __DEFAULT_FN_ATTRS
_mm_aesdecwide256kl_u8(__m128i __odata[8], const __m128i __idata[8], const void* __h) {
return __builtin_ia32_aesdecwide256kl_u8((__v2di *)__odata,
(const __v2di *)__idata, __h);
}

#undef __DEFAULT_FN_ATTRS

#endif


#endif
