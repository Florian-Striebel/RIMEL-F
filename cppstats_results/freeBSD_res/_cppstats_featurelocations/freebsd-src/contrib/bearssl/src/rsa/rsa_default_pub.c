























#include "inner.h"


br_rsa_public
br_rsa_public_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_public;
#elif BR_LOMUL
return &br_rsa_i15_public;
#else
return &br_rsa_i31_public;
#endif
}
