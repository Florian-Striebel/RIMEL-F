























#include "inner.h"


br_rsa_keygen
br_rsa_keygen_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_keygen;
#elif BR_LOMUL
return &br_rsa_i15_keygen;
#else
return &br_rsa_i31_keygen;
#endif
}
