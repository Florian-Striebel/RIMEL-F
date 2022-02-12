











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



COMPILER_RT_ABI ti_int __negti2(ti_int a) {


return -a;
}

#endif
