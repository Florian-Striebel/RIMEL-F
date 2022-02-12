















#include "openbsd-compat.h"
#include "fido.h"

#if OPENSSL_VERSION_NUMBER < 0x10100000L

#include <assert.h>
#include <string.h>

#include <openssl/err.h>
#include <openssl/hmac.h>

#define CRYPTOerror(r) CRYPTOerr(ERR_LIB_CRYPTO, (r))


int
HKDF(uint8_t *out_key, size_t out_len, const EVP_MD *digest,
const uint8_t *secret, size_t secret_len, const uint8_t *salt,
size_t salt_len, const uint8_t *info, size_t info_len)
{
uint8_t prk[EVP_MAX_MD_SIZE];
size_t prk_len;

if (!HKDF_extract(prk, &prk_len, digest, secret, secret_len, salt,
salt_len))
return 0;
if (!HKDF_expand(out_key, out_len, digest, prk, prk_len, info,
info_len))
return 0;

return 1;
}


int
HKDF_extract(uint8_t *out_key, size_t *out_len,
const EVP_MD *digest, const uint8_t *secret, size_t secret_len,
const uint8_t *salt, size_t salt_len)
{
unsigned int len;





if (salt_len > INT_MAX || HMAC(digest, salt, (int)salt_len, secret,
secret_len, out_key, &len) == NULL) {
CRYPTOerror(ERR_R_CRYPTO_LIB);
return 0;
}
*out_len = len;
return 1;
}


int
HKDF_expand(uint8_t *out_key, size_t out_len,
const EVP_MD *digest, const uint8_t *prk, size_t prk_len,
const uint8_t *info, size_t info_len)
{
const size_t digest_len = EVP_MD_size(digest);
uint8_t previous[EVP_MAX_MD_SIZE];
size_t n, done = 0;
unsigned int i;
int ret = 0;
HMAC_CTX hmac;


n = (out_len + digest_len - 1) / digest_len;
if (out_len + digest_len < out_len || n > 255 || prk_len > INT_MAX) {
CRYPTOerror(EVP_R_TOO_LARGE);
return 0;
}

HMAC_CTX_init(&hmac);
if (!HMAC_Init_ex(&hmac, prk, (int)prk_len, digest, NULL))
goto out;

for (i = 0; i < n; i++) {
uint8_t ctr = i + 1;
size_t todo;

if (i != 0 && (!HMAC_Init_ex(&hmac, NULL, 0, NULL, NULL) ||
!HMAC_Update(&hmac, previous, digest_len)))
goto out;

if (!HMAC_Update(&hmac, info, info_len) ||
!HMAC_Update(&hmac, &ctr, 1) ||
!HMAC_Final(&hmac, previous, NULL))
goto out;

todo = digest_len;
if (done + todo > out_len)
todo = out_len - done;

memcpy(out_key + done, previous, todo);
done += todo;
}

ret = 1;

out:
HMAC_CTX_cleanup(&hmac);
explicit_bzero(previous, sizeof(previous));
if (ret != 1)
CRYPTOerror(ERR_R_CRYPTO_LIB);
return ret;
}
#endif
