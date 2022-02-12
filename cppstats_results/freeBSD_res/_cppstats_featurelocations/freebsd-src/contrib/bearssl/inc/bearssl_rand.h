























#if !defined(BR_BEARSSL_RAND_H__)
#define BR_BEARSSL_RAND_H__

#include <stddef.h>
#include <stdint.h>

#include "bearssl_block.h"
#include "bearssl_hash.h"

#if defined(__cplusplus)
extern "C" {
#endif

































































































typedef struct br_prng_class_ br_prng_class;
struct br_prng_class_ {




size_t context_size;




















void (*init)(const br_prng_class **ctx, const void *params,
const void *seed, size_t seed_len);











void (*generate)(const br_prng_class **ctx, void *out, size_t len);











void (*update)(const br_prng_class **ctx,
const void *seed, size_t seed_len);
};







typedef struct {





const br_prng_class *vtable;
#if !defined(BR_DOXYGEN_IGNORE)
unsigned char K[64];
unsigned char V[64];
const br_hash_class *digest_class;
#endif
} br_hmac_drbg_context;




extern const br_prng_class br_hmac_drbg_vtable;























void br_hmac_drbg_init(br_hmac_drbg_context *ctx,
const br_hash_class *digest_class, const void *seed, size_t seed_len);













void br_hmac_drbg_generate(br_hmac_drbg_context *ctx, void *out, size_t len);













void br_hmac_drbg_update(br_hmac_drbg_context *ctx,
const void *seed, size_t seed_len);











static inline const br_hash_class *
br_hmac_drbg_get_hash(const br_hmac_drbg_context *ctx)
{
return ctx->digest_class;
}

















typedef int (*br_prng_seeder)(const br_prng_class **ctx);
















br_prng_seeder br_prng_seeder_system(const char **name);







typedef struct {





const br_prng_class *vtable;
#if !defined(BR_DOXYGEN_IGNORE)
br_aes_gen_ctr_keys sk;
uint32_t cc;
#endif
} br_aesctr_drbg_context;




extern const br_prng_class br_aesctr_drbg_vtable;



















void br_aesctr_drbg_init(br_aesctr_drbg_context *ctx,
const br_block_ctr_class *aesctr, const void *seed, size_t seed_len);











void br_aesctr_drbg_generate(br_aesctr_drbg_context *ctx,
void *out, size_t len);













void br_aesctr_drbg_update(br_aesctr_drbg_context *ctx,
const void *seed, size_t seed_len);

#if defined(__cplusplus)
}
#endif

#endif
