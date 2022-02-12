























#include "inner.h"


br_rsa_pss_sign
br_rsa_pss_sign_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_pss_sign;
#elif BR_LOMUL
return &br_rsa_i15_pss_sign;
#else
return &br_rsa_i31_pss_sign;
#endif
}
