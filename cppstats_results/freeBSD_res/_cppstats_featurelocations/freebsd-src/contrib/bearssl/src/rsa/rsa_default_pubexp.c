























#include "inner.h"


br_rsa_compute_pubexp
br_rsa_compute_pubexp_get_default(void)
{
#if BR_LOMUL
return &br_rsa_i15_compute_pubexp;
#else
return &br_rsa_i31_compute_pubexp;
#endif
}
