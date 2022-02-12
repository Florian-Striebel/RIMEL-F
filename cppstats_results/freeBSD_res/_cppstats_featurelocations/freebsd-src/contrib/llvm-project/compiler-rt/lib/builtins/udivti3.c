











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



COMPILER_RT_ABI tu_int __udivti3(tu_int a, tu_int b) {
return __udivmodti4(a, b, 0);
}

#endif
