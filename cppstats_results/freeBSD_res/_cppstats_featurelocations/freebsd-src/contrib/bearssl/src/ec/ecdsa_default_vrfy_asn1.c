























#include "inner.h"


br_ecdsa_vrfy
br_ecdsa_vrfy_asn1_get_default(void)
{
#if BR_LOMUL
return &br_ecdsa_i15_vrfy_asn1;
#else
return &br_ecdsa_i31_vrfy_asn1;
#endif
}
