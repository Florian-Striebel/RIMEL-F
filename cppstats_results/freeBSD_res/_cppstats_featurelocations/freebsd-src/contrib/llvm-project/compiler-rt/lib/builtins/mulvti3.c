











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





#define fixint_t ti_int
#include "int_mulv_impl.inc"

COMPILER_RT_ABI ti_int __mulvti3(ti_int a, ti_int b) { return __mulvXi3(a, b); }

#endif
