











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



COMPILER_RT_ABI tu_int __umodti3(tu_int a, tu_int b) {
tu_int r;
__udivmodti4(a, b, &r);
return r;
}

#endif
