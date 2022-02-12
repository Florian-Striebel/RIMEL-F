























#include "inner.h"


void
br_ssl_engine_set_default_rsavrfy(br_ssl_engine_context *cc)
{
br_ssl_engine_set_rsavrfy(cc, br_rsa_pkcs1_vrfy_get_default());
}
