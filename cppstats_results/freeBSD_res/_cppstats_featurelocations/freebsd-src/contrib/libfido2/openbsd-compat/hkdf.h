














#if !defined(OPENSSL_HEADER_HKDF_H)
#define OPENSSL_HEADER_HKDF_H

#include <openssl/evp.h>

#if defined(__cplusplus)
extern "C" {
#endif












int HKDF(uint8_t *out_key, size_t out_len, const struct env_md_st *digest,
const uint8_t *secret, size_t secret_len, const uint8_t *salt,
size_t salt_len, const uint8_t *info, size_t info_len);







int HKDF_extract(uint8_t *out_key, size_t *out_len,
const struct env_md_st *digest, const uint8_t *secret,
size_t secret_len, const uint8_t *salt, size_t salt_len);







int HKDF_expand(uint8_t *out_key, size_t out_len,
const EVP_MD *digest, const uint8_t *prk, size_t prk_len,
const uint8_t *info, size_t info_len);


#if defined(__cplusplus)
}
#endif

#endif
