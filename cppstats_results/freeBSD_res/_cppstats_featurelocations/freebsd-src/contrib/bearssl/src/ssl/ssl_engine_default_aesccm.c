























#include "inner.h"


void
br_ssl_engine_set_default_aes_ccm(br_ssl_engine_context *cc)
{
#if BR_AES_X86NI || BR_POWER8
const br_block_ctrcbc_class *ictrcbc;
#endif

br_ssl_engine_set_ccm(cc,
&br_sslrec_in_ccm_vtable,
&br_sslrec_out_ccm_vtable);
#if BR_AES_X86NI
ictrcbc = br_aes_x86ni_ctrcbc_get_vtable();
if (ictrcbc != NULL) {
br_ssl_engine_set_aes_ctrcbc(cc, ictrcbc);
} else {
#if BR_64
br_ssl_engine_set_aes_ctrcbc(cc, &br_aes_ct64_ctrcbc_vtable);
#else
br_ssl_engine_set_aes_ctrcbc(cc, &br_aes_ct_ctrcbc_vtable);
#endif
}
#elif BR_POWER8
ictrcbc = br_aes_pwr8_ctrcbc_get_vtable();
if (ictrcbc != NULL) {
br_ssl_engine_set_aes_ctrcbc(cc, ictrcbc);
} else {
#if BR_64
br_ssl_engine_set_aes_ctrcbc(cc, &br_aes_ct64_ctrcbc_vtable);
#else
br_ssl_engine_set_aes_ctrcbc(cc, &br_aes_ct_ctrcbc_vtable);
#endif
}
#else
#if BR_64
br_ssl_engine_set_aes_ctrcbc(cc, &br_aes_ct64_ctrcbc_vtable);
#else
br_ssl_engine_set_aes_ctrcbc(cc, &br_aes_ct_ctrcbc_vtable);
#endif
#endif
}
