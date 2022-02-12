























#include "inner.h"


br_rsa_pkcs1_sign
br_rsa_pkcs1_sign_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_pkcs1_sign;
#elif BR_LOMUL
return &br_rsa_i15_pkcs1_sign;
#else
return &br_rsa_i31_pkcs1_sign;
#endif
}
