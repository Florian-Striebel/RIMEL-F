











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





#define fixint_t ti_int
#include "int_mulo_impl.inc"

COMPILER_RT_ABI ti_int __muloti4(ti_int a, ti_int b, int *overflow) {
return __muloXi4(a, b, overflow);
}

#endif
