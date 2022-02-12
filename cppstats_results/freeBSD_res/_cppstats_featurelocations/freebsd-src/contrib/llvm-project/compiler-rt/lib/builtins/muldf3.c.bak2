












#define DOUBLE_PRECISION
#include "fp_mul_impl.inc"

COMPILER_RT_ABI fp_t __muldf3(fp_t a, fp_t b) { return __mulXf3__(a, b); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_dmul(fp_t a, fp_t b) { return __muldf3(a, b); }
#else
COMPILER_RT_ALIAS(__muldf3, __aeabi_dmul)
#endif
#endif
