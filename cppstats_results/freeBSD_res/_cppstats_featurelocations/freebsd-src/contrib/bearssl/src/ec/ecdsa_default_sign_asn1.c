























#include "inner.h"


br_ecdsa_sign
br_ecdsa_sign_asn1_get_default(void)
{
#if BR_LOMUL
return &br_ecdsa_i15_sign_asn1;
#else
return &br_ecdsa_i31_sign_asn1;
#endif
}
