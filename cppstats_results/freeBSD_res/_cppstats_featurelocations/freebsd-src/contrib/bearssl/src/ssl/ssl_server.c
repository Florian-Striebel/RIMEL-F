























#include "inner.h"


void
br_ssl_server_zero(br_ssl_server_context *cc)
{






memset(cc, 0, sizeof *cc);
}


int
br_ssl_server_reset(br_ssl_server_context *cc)
{
br_ssl_engine_set_buffer(&cc->eng, NULL, 0, 0);
if (!br_ssl_engine_init_rand(&cc->eng)) {
return 0;
}
cc->eng.reneg = 0;
br_ssl_engine_hs_reset(&cc->eng,
br_ssl_hs_server_init_main, br_ssl_hs_server_run);
return br_ssl_engine_last_error(&cc->eng) == BR_ERR_OK;
}
