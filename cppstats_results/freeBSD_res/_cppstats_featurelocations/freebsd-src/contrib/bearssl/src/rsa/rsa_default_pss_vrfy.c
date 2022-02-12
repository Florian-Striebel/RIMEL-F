























#include "inner.h"


br_rsa_pss_vrfy
br_rsa_pss_vrfy_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_pss_vrfy;
#elif BR_LOMUL
return &br_rsa_i15_pss_vrfy;
#else
return &br_rsa_i31_pss_vrfy;
#endif
}
