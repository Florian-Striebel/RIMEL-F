























#include "inner.h"





typedef union {
const br_hash_class *vtable;
br_md5_context md5;
br_sha1_context sha1;
br_sha224_context sha224;
br_sha256_context sha256;
br_sha384_context sha384;
br_sha512_context sha512;
} gen_hash_context;






static size_t
get_state_offset(int id)
{
if (id >= 5) {




return offsetof(br_multihash_context, val_64)
+ ((size_t)(id - 5) * (8 * sizeof(uint64_t)));
} else {






unsigned x;

x = id - 1;
x = ((x + (x & (x >> 1))) << 2) + (x >> 1);
return offsetof(br_multihash_context, val_32)
+ x * sizeof(uint32_t);
}
}


void
br_multihash_zero(br_multihash_context *ctx)
{




memset(ctx, 0, sizeof *ctx);
}


void
br_multihash_init(br_multihash_context *ctx)
{
int i;

ctx->count = 0;
for (i = 1; i <= 6; i ++) {
const br_hash_class *hc;

hc = ctx->impl[i - 1];
if (hc != NULL) {
gen_hash_context g;

hc->init(&g.vtable);
hc->state(&g.vtable,
(unsigned char *)ctx + get_state_offset(i));
}
}
}


void
br_multihash_update(br_multihash_context *ctx, const void *data, size_t len)
{
const unsigned char *buf;
size_t ptr;

buf = data;
ptr = (size_t)ctx->count & 127;
while (len > 0) {
size_t clen;

clen = 128 - ptr;
if (clen > len) {
clen = len;
}
memcpy(ctx->buf + ptr, buf, clen);
ptr += clen;
buf += clen;
len -= clen;
ctx->count += (uint64_t)clen;
if (ptr == 128) {
int i;

for (i = 1; i <= 6; i ++) {
const br_hash_class *hc;

hc = ctx->impl[i - 1];
if (hc != NULL) {
gen_hash_context g;
unsigned char *state;

state = (unsigned char *)ctx
+ get_state_offset(i);
hc->set_state(&g.vtable,
state, ctx->count - 128);
hc->update(&g.vtable, ctx->buf, 128);
hc->state(&g.vtable, state);
}
}
ptr = 0;
}
}
}


size_t
br_multihash_out(const br_multihash_context *ctx, int id, void *dst)
{
const br_hash_class *hc;
gen_hash_context g;
const unsigned char *state;

hc = ctx->impl[id - 1];
if (hc == NULL) {
return 0;
}
state = (const unsigned char *)ctx + get_state_offset(id);
hc->set_state(&g.vtable, state, ctx->count & ~(uint64_t)127);
hc->update(&g.vtable, ctx->buf, ctx->count & (uint64_t)127);
hc->out(&g.vtable, dst);
return (hc->desc >> BR_HASHDESC_OUT_OFF) & BR_HASHDESC_OUT_MASK;
}
