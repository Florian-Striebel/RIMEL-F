























#include "inner.h"


void
br_ssl_engine_set_default_ec(br_ssl_engine_context *cc)
{
#if BR_LOMUL
br_ssl_engine_set_ec(cc, &br_ec_all_m15);
#else
br_ssl_engine_set_ec(cc, &br_ec_all_m31);
#endif
}
