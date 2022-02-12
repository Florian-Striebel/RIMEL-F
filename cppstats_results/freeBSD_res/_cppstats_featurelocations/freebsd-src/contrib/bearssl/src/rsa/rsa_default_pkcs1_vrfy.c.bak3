























#include "inner.h"


br_rsa_pkcs1_vrfy
br_rsa_pkcs1_vrfy_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_pkcs1_vrfy;
#elif BR_LOMUL
return &br_rsa_i15_pkcs1_vrfy;
#else
return &br_rsa_i31_pkcs1_vrfy;
#endif
}
