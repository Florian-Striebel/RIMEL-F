























#include "inner.h"


br_rsa_oaep_decrypt
br_rsa_oaep_decrypt_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_oaep_decrypt;
#elif BR_LOMUL
return &br_rsa_i15_oaep_decrypt;
#else
return &br_rsa_i31_oaep_decrypt;
#endif
}
