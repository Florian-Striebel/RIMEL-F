























#include "inner.h"






static void
gen_ccm_init(br_sslrec_ccm_context *cc,
const br_block_ctrcbc_class *bc_impl,
const void *key, size_t key_len,
const void *iv, size_t tag_len)
{
cc->seq = 0;
bc_impl->init(&cc->bc.vtable, key, key_len);
memcpy(cc->iv, iv, sizeof cc->iv);
cc->tag_len = tag_len;
}

static void
in_ccm_init(br_sslrec_ccm_context *cc,
const br_block_ctrcbc_class *bc_impl,
const void *key, size_t key_len,
const void *iv, size_t tag_len)
{
cc->vtable.in = &br_sslrec_in_ccm_vtable;
gen_ccm_init(cc, bc_impl, key, key_len, iv, tag_len);
}

static int
ccm_check_length(const br_sslrec_ccm_context *cc, size_t rlen)
{




size_t over;

over = 8 + cc->tag_len;
return rlen >= over && rlen <= (16384 + over);
}

static unsigned char *
ccm_decrypt(br_sslrec_ccm_context *cc,
int record_type, unsigned version, void *data, size_t *data_len)
{
br_ccm_context zc;
unsigned char *buf;
unsigned char nonce[12], header[13];
size_t len;

buf = (unsigned char *)data + 8;
len = *data_len - (8 + cc->tag_len);




memcpy(nonce, cc->iv, sizeof cc->iv);
memcpy(nonce + 4, data, 8);




br_enc64be(header, cc->seq ++);
header[8] = (unsigned char)record_type;
br_enc16be(header + 9, version);
br_enc16be(header + 11, len);




br_ccm_init(&zc, &cc->bc.vtable);
br_ccm_reset(&zc, nonce, sizeof nonce, sizeof header, len, cc->tag_len);
br_ccm_aad_inject(&zc, header, sizeof header);
br_ccm_flip(&zc);
br_ccm_run(&zc, 0, buf, len);
if (!br_ccm_check_tag(&zc, buf + len)) {
return NULL;
}
*data_len = len;
return buf;
}


const br_sslrec_in_ccm_class br_sslrec_in_ccm_vtable = {
{
sizeof(br_sslrec_ccm_context),
(int (*)(const br_sslrec_in_class *const *, size_t))
&ccm_check_length,
(unsigned char *(*)(const br_sslrec_in_class **,
int, unsigned, void *, size_t *))
&ccm_decrypt
},
(void (*)(const br_sslrec_in_ccm_class **,
const br_block_ctrcbc_class *, const void *, size_t,
const void *, size_t))
&in_ccm_init
};

static void
out_ccm_init(br_sslrec_ccm_context *cc,
const br_block_ctrcbc_class *bc_impl,
const void *key, size_t key_len,
const void *iv, size_t tag_len)
{
cc->vtable.out = &br_sslrec_out_ccm_vtable;
gen_ccm_init(cc, bc_impl, key, key_len, iv, tag_len);
}

static void
ccm_max_plaintext(const br_sslrec_ccm_context *cc,
size_t *start, size_t *end)
{
size_t len;

*start += 8;
len = *end - *start - cc->tag_len;
if (len > 16384) {
len = 16384;
}
*end = *start + len;
}

static unsigned char *
ccm_encrypt(br_sslrec_ccm_context *cc,
int record_type, unsigned version, void *data, size_t *data_len)
{
br_ccm_context zc;
unsigned char *buf;
unsigned char nonce[12], header[13];
size_t len;

buf = (unsigned char *)data;
len = *data_len;





memcpy(nonce, cc->iv, sizeof cc->iv);
br_enc64be(nonce + 4, cc->seq);




br_enc64be(header, cc->seq ++);
header[8] = (unsigned char)record_type;
br_enc16be(header + 9, version);
br_enc16be(header + 11, len);




br_ccm_init(&zc, &cc->bc.vtable);
br_ccm_reset(&zc, nonce, sizeof nonce, sizeof header, len, cc->tag_len);
br_ccm_aad_inject(&zc, header, sizeof header);
br_ccm_flip(&zc);
br_ccm_run(&zc, 1, buf, len);
br_ccm_get_tag(&zc, buf + len);




len += 8 + cc->tag_len;
buf -= 13;
memcpy(buf + 5, nonce + 4, 8);
buf[0] = (unsigned char)record_type;
br_enc16be(buf + 1, version);
br_enc16be(buf + 3, len);
*data_len = len + 5;
return buf;
}


const br_sslrec_out_ccm_class br_sslrec_out_ccm_vtable = {
{
sizeof(br_sslrec_ccm_context),
(void (*)(const br_sslrec_out_class *const *,
size_t *, size_t *))
&ccm_max_plaintext,
(unsigned char *(*)(const br_sslrec_out_class **,
int, unsigned, void *, size_t *))
&ccm_encrypt
},
(void (*)(const br_sslrec_out_ccm_class **,
const br_block_ctrcbc_class *, const void *, size_t,
const void *, size_t))
&out_ccm_init
};
