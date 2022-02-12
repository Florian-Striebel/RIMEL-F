























#include "inner.h"


void
br_ssl_engine_set_default_ecdsa(br_ssl_engine_context *cc)
{
#if BR_LOMUL
br_ssl_engine_set_ec(cc, &br_ec_all_m15);
br_ssl_engine_set_ecdsa(cc, &br_ecdsa_i15_vrfy_asn1);
#else
br_ssl_engine_set_ec(cc, &br_ec_all_m31);
br_ssl_engine_set_ecdsa(cc, &br_ecdsa_i31_vrfy_asn1);
#endif
}
