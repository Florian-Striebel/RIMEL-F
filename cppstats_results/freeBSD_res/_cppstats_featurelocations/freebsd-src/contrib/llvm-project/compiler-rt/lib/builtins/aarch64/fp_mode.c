







#include <stdint.h>

#include "../fp_mode.h"

#define AARCH64_TONEAREST 0x0
#define AARCH64_UPWARD 0x1
#define AARCH64_DOWNWARD 0x2
#define AARCH64_TOWARDZERO 0x3
#define AARCH64_RMODE_MASK (AARCH64_TONEAREST | AARCH64_UPWARD | AARCH64_DOWNWARD | AARCH64_TOWARDZERO)

#define AARCH64_RMODE_SHIFT 22

#define AARCH64_INEXACT 0x10

#if !defined(__ARM_FP)


CRT_FE_ROUND_MODE __attribute__((weak)) __aarch64_fe_default_rmode =
CRT_FE_TONEAREST;
#endif

CRT_FE_ROUND_MODE __fe_getround() {
#if defined(__ARM_FP)
uint64_t fpcr;
__asm__ __volatile__("mrs %0, fpcr" : "=r" (fpcr));
fpcr = fpcr >> AARCH64_RMODE_SHIFT & AARCH64_RMODE_MASK;
switch (fpcr) {
case AARCH64_UPWARD:
return CRT_FE_UPWARD;
case AARCH64_DOWNWARD:
return CRT_FE_DOWNWARD;
case AARCH64_TOWARDZERO:
return CRT_FE_TOWARDZERO;
case AARCH64_TONEAREST:
default:
return CRT_FE_TONEAREST;
}
#else
return __aarch64_fe_default_rmode;
#endif
}

int __fe_raise_inexact() {
#if defined(__ARM_FP)
uint64_t fpsr;
__asm__ __volatile__("mrs %0, fpsr" : "=r" (fpsr));
__asm__ __volatile__("msr fpsr, %0" : : "ri" (fpsr | AARCH64_INEXACT));
return 0;
#else
return 0;
#endif
}
