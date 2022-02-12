























#include "inner.h"


void
br_ssl_engine_set_default_des_cbc(br_ssl_engine_context *cc)
{
br_ssl_engine_set_cbc(cc,
&br_sslrec_in_cbc_vtable,
&br_sslrec_out_cbc_vtable);
br_ssl_engine_set_des_cbc(cc,
&br_des_ct_cbcenc_vtable,
&br_des_ct_cbcdec_vtable);
}
