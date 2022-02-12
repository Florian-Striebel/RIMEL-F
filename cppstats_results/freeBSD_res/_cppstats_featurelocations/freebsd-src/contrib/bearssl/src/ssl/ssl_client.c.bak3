























#include "inner.h"


void
br_ssl_client_zero(br_ssl_client_context *cc)
{






memset(cc, 0, sizeof *cc);
}


int
br_ssl_client_reset(br_ssl_client_context *cc,
const char *server_name, int resume_session)
{
size_t n;

br_ssl_engine_set_buffer(&cc->eng, NULL, 0, 0);
cc->eng.version_out = cc->eng.version_min;
if (!resume_session) {
br_ssl_client_forget_session(cc);
}
if (!br_ssl_engine_init_rand(&cc->eng)) {
return 0;
}







cc->eng.reneg = 0;

if (server_name == NULL) {
cc->eng.server_name[0] = 0;
} else {
n = strlen(server_name) + 1;
if (n > sizeof cc->eng.server_name) {
br_ssl_engine_fail(&cc->eng, BR_ERR_BAD_PARAM);
return 0;
}
memcpy(cc->eng.server_name, server_name, n);
}

br_ssl_engine_hs_reset(&cc->eng,
br_ssl_hs_client_init_main, br_ssl_hs_client_run);
return br_ssl_engine_last_error(&cc->eng) == BR_ERR_OK;
}
