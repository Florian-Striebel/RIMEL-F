























#include "inner.h"


br_rsa_compute_privexp
br_rsa_compute_privexp_get_default(void)
{
#if BR_LOMUL
return &br_rsa_i15_compute_privexp;
#else
return &br_rsa_i31_compute_privexp;
#endif
}
