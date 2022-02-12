







#include "int_lib.h"

extern void __clear_cache(void *start, void *end);







#if __ppc__ && !defined(__powerpc64__)
COMPILER_RT_ABI void __trampoline_setup(uint32_t *trampOnStack,
int trampSizeAllocated,
const void *realFunc, void *localsPtr) {


if (trampSizeAllocated < 40)
compilerrt_abort();


trampOnStack[0] = 0x7c0802a6;
trampOnStack[1] = 0x4800000d;
trampOnStack[2] = (uint32_t)realFunc;
trampOnStack[3] = (uint32_t)localsPtr;
trampOnStack[4] = 0x7d6802a6;
trampOnStack[5] = 0x818b0000;
trampOnStack[6] = 0x7c0803a6;
trampOnStack[7] = 0x7d8903a6;
trampOnStack[8] = 0x816b0004;
trampOnStack[9] = 0x4e800420;


__clear_cache(trampOnStack, &trampOnStack[10]);
}
#endif
