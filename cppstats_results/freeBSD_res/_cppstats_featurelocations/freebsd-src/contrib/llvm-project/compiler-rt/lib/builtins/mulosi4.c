











#define fixint_t si_int
#include "int_mulo_impl.inc"





COMPILER_RT_ABI si_int __mulosi4(si_int a, si_int b, int *overflow) {
return __muloXi4(a, b, overflow);
}
