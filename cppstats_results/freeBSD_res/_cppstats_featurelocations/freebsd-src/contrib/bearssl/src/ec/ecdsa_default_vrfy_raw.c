























#include "inner.h"


br_ecdsa_vrfy
br_ecdsa_vrfy_raw_get_default(void)
{
#if BR_LOMUL
return &br_ecdsa_i15_vrfy_raw;
#else
return &br_ecdsa_i31_vrfy_raw;
#endif
}
