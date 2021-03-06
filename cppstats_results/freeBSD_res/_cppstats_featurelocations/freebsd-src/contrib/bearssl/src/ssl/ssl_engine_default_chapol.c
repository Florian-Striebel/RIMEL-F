























#include "inner.h"


void
br_ssl_engine_set_default_chapol(br_ssl_engine_context *cc)
{
#if BR_INT128 || BR_UMUL128
br_poly1305_run bp;
#endif
#if BR_SSE2
br_chacha20_run bc;
#endif

br_ssl_engine_set_chapol(cc,
&br_sslrec_in_chapol_vtable,
&br_sslrec_out_chapol_vtable);
#if BR_SSE2
bc = br_chacha20_sse2_get();
if (bc) {
br_ssl_engine_set_chacha20(cc, bc);
} else {
#endif
br_ssl_engine_set_chacha20(cc, &br_chacha20_ct_run);
#if BR_SSE2
}
#endif
#if BR_INT128 || BR_UMUL128
bp = br_poly1305_ctmulq_get();
if (bp) {
br_ssl_engine_set_poly1305(cc, bp);
} else {
#endif
#if BR_LOMUL
br_ssl_engine_set_poly1305(cc, &br_poly1305_ctmul32_run);
#else
br_ssl_engine_set_poly1305(cc, &br_poly1305_ctmul_run);
#endif
#if BR_INT128 || BR_UMUL128
}
#endif
}
