























#include "inner.h"


br_rsa_compute_modulus
br_rsa_compute_modulus_get_default(void)
{
#if BR_LOMUL
return &br_rsa_i15_compute_modulus;
#else
return &br_rsa_i31_compute_modulus;
#endif
}
