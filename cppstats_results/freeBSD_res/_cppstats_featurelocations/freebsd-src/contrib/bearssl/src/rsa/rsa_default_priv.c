























#include "inner.h"


br_rsa_private
br_rsa_private_get_default(void)
{
#if BR_INT128 || BR_UMUL128
return &br_rsa_i62_private;
#elif BR_LOMUL
return &br_rsa_i15_private;
#else
return &br_rsa_i31_private;
#endif
}
