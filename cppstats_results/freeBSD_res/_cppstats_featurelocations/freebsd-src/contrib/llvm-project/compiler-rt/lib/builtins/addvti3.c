











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI ti_int __addvti3(ti_int a, ti_int b) {
ti_int s = (tu_int)a + (tu_int)b;
if (b >= 0) {
if (s < a)
compilerrt_abort();
} else {
if (s >= a)
compilerrt_abort();
}
return s;
}

#endif
