
























#if !defined(ARCHIVE_CRYPTOR_PRIVATE_H_INCLUDED)
#define ARCHIVE_CRYPTOR_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif









int __libarchive_cryptor_build_hack(void);

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1080
#define ARCHIVE_CRYPTOR_USE_Apple_CommonCrypto
#endif
#endif

#if defined(ARCHIVE_CRYPTOR_USE_Apple_CommonCrypto)
#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#define AES_BLOCK_SIZE 16
#define AES_MAX_KEY_SIZE kCCKeySizeAES256

typedef struct {
CCCryptorRef ctx;
uint8_t key[AES_MAX_KEY_SIZE];
unsigned key_len;
uint8_t nonce[AES_BLOCK_SIZE];
uint8_t encr_buf[AES_BLOCK_SIZE];
unsigned encr_pos;
} archive_crypto_ctx;

#elif defined(_WIN32) && !defined(__CYGWIN__) && defined(HAVE_BCRYPT_H)
#include <bcrypt.h>


#if !defined(BCRYPT_SUCCESS)
#define BCRYPT_SUCCESS(r) ((NTSTATUS)(r) == STATUS_SUCCESS)
#endif

#define AES_MAX_KEY_SIZE 32
#define AES_BLOCK_SIZE 16
typedef struct {
BCRYPT_ALG_HANDLE hAlg;
BCRYPT_KEY_HANDLE hKey;
PBYTE keyObj;
DWORD keyObj_len;
uint8_t nonce[AES_BLOCK_SIZE];
uint8_t encr_buf[AES_BLOCK_SIZE];
unsigned encr_pos;
} archive_crypto_ctx;

#elif defined(HAVE_LIBMBEDCRYPTO) && defined(HAVE_MBEDTLS_AES_H)
#include <mbedtls/aes.h>
#include <mbedtls/md.h>
#include <mbedtls/pkcs5.h>

#define AES_MAX_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

typedef struct {
mbedtls_aes_context ctx;
uint8_t key[AES_MAX_KEY_SIZE];
unsigned key_len;
uint8_t nonce[AES_BLOCK_SIZE];
uint8_t encr_buf[AES_BLOCK_SIZE];
unsigned encr_pos;
} archive_crypto_ctx;

#elif defined(HAVE_LIBNETTLE) && defined(HAVE_NETTLE_AES_H)
#if defined(HAVE_NETTLE_PBKDF2_H)
#include <nettle/pbkdf2.h>
#endif
#include <nettle/aes.h>
#include <nettle/version.h>

typedef struct {
#if NETTLE_VERSION_MAJOR < 3
struct aes_ctx ctx;
#else
union {
struct aes128_ctx c128;
struct aes192_ctx c192;
struct aes256_ctx c256;
} ctx;
#endif
uint8_t key[AES_MAX_KEY_SIZE];
unsigned key_len;
uint8_t nonce[AES_BLOCK_SIZE];
uint8_t encr_buf[AES_BLOCK_SIZE];
unsigned encr_pos;
} archive_crypto_ctx;

#elif defined(HAVE_LIBCRYPTO)
#include "archive_openssl_evp_private.h"
#define AES_BLOCK_SIZE 16
#define AES_MAX_KEY_SIZE 32

typedef struct {
EVP_CIPHER_CTX *ctx;
const EVP_CIPHER *type;
uint8_t key[AES_MAX_KEY_SIZE];
unsigned key_len;
uint8_t nonce[AES_BLOCK_SIZE];
uint8_t encr_buf[AES_BLOCK_SIZE];
unsigned encr_pos;
} archive_crypto_ctx;

#else

#define AES_BLOCK_SIZE 16
#define AES_MAX_KEY_SIZE 32
typedef int archive_crypto_ctx;

#endif


#define archive_pbkdf2_sha1(pw, pw_len, salt, salt_len, rounds, dk, dk_len)__archive_cryptor.pbkdf2sha1(pw, pw_len, salt, salt_len, rounds, dk, dk_len)


#define archive_decrypto_aes_ctr_init(ctx, key, key_len) __archive_cryptor.decrypto_aes_ctr_init(ctx, key, key_len)

#define archive_decrypto_aes_ctr_update(ctx, in, in_len, out, out_len) __archive_cryptor.decrypto_aes_ctr_update(ctx, in, in_len, out, out_len)

#define archive_decrypto_aes_ctr_release(ctx) __archive_cryptor.decrypto_aes_ctr_release(ctx)


#define archive_encrypto_aes_ctr_init(ctx, key, key_len) __archive_cryptor.encrypto_aes_ctr_init(ctx, key, key_len)

#define archive_encrypto_aes_ctr_update(ctx, in, in_len, out, out_len) __archive_cryptor.encrypto_aes_ctr_update(ctx, in, in_len, out, out_len)

#define archive_encrypto_aes_ctr_release(ctx) __archive_cryptor.encrypto_aes_ctr_release(ctx)




struct archive_cryptor
{

int (*pbkdf2sha1)(const char *pw, size_t pw_len, const uint8_t *salt,
size_t salt_len, unsigned rounds, uint8_t *derived_key,
size_t derived_key_len);

int (*decrypto_aes_ctr_init)(archive_crypto_ctx *, const uint8_t *, size_t);
int (*decrypto_aes_ctr_update)(archive_crypto_ctx *, const uint8_t *,
size_t, uint8_t *, size_t *);
int (*decrypto_aes_ctr_release)(archive_crypto_ctx *);
int (*encrypto_aes_ctr_init)(archive_crypto_ctx *, const uint8_t *, size_t);
int (*encrypto_aes_ctr_update)(archive_crypto_ctx *, const uint8_t *,
size_t, uint8_t *, size_t *);
int (*encrypto_aes_ctr_release)(archive_crypto_ctx *);
};

extern const struct archive_cryptor __archive_cryptor;

#endif
