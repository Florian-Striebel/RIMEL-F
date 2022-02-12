























#include "inner.h"


br_rsa_oaep_encrypt
br_rsa_oaep_encrypt_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_oaep_encrypt;
#elif BR_LOMUL
return &br_rsa_i15_oaep_encrypt;
#else
return &br_rsa_i31_oaep_encrypt;
#endif
}
