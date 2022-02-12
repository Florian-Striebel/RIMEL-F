























#if !defined(BR_BEARSSL_HMAC_H__)
#define BR_BEARSSL_HMAC_H__

#include <stddef.h>
#include <stdint.h>

#include "bearssl_hash.h"

#if defined(__cplusplus)
extern "C" {
#endif






























typedef struct {
#if !defined(BR_DOXYGEN_IGNORE)
const br_hash_class *dig_vtable;
unsigned char ksi[64], kso[64];
#endif
} br_hmac_key_context;












void br_hmac_key_init(br_hmac_key_context *kc,
const br_hash_class *digest_vtable, const void *key, size_t key_len);










static inline const br_hash_class *br_hmac_key_get_digest(
const br_hmac_key_context *kc)
{
return kc->dig_vtable;
}









typedef struct {
#if !defined(BR_DOXYGEN_IGNORE)
br_hash_compat_context dig;
unsigned char kso[64];
size_t out_len;
#endif
} br_hmac_context;



















void br_hmac_init(br_hmac_context *ctx,
const br_hmac_key_context *kc, size_t out_len);
















static inline size_t
br_hmac_size(br_hmac_context *ctx)
{
return ctx->out_len;
}










static inline const br_hash_class *br_hmac_get_digest(
const br_hmac_context *hc)
{
return hc->dig.vtable;
}









void br_hmac_update(br_hmac_context *ctx, const void *data, size_t len);
















size_t br_hmac_out(const br_hmac_context *ctx, void *out);
































size_t br_hmac_outCT(const br_hmac_context *ctx,
const void *data, size_t len, size_t min_len, size_t max_len,
void *out);

#if defined(__cplusplus)
}
#endif

#endif
