























#include "../int_lib.h"


extern unsigned char declaration;

#if defined(__ARM_EABI__)
AEABI_RTABI int __attribute__((weak)) __attribute__((visibility("hidden")))
__aeabi_idiv0(int return_value) {
return return_value;
}

AEABI_RTABI long long __attribute__((weak))
__attribute__((visibility("hidden"))) __aeabi_ldiv0(long long return_value) {
return return_value;
}
#endif
