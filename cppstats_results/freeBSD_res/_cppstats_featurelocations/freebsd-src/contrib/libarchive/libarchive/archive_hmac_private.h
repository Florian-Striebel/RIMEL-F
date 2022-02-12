
























#if !defined(ARCHIVE_HMAC_PRIVATE_H_INCLUDED)
#define ARCHIVE_HMAC_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif









int __libarchive_hmac_build_hack(void);

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1060
#define ARCHIVE_HMAC_USE_Apple_CommonCrypto
#endif
#endif

#if defined(ARCHIVE_HMAC_USE_Apple_CommonCrypto)
#include <CommonCrypto/CommonHMAC.h>

typedef CCHmacContext archive_hmac_sha1_ctx;

#elif defined(_WIN32) && !defined(__CYGWIN__) && defined(HAVE_BCRYPT_H)
#include <bcrypt.h>

typedef struct {
BCRYPT_ALG_HANDLE hAlg;
BCRYPT_HASH_HANDLE hHash;
DWORD hash_len;
PBYTE hash;

} archive_hmac_sha1_ctx;

#elif defined(HAVE_LIBMBEDCRYPTO) && defined(HAVE_MBEDTLS_MD_H)
#include <mbedtls/md.h>

typedef mbedtls_md_context_t archive_hmac_sha1_ctx;

#elif defined(HAVE_LIBNETTLE) && defined(HAVE_NETTLE_HMAC_H)
#include <nettle/hmac.h>

typedef struct hmac_sha1_ctx archive_hmac_sha1_ctx;

#elif defined(HAVE_LIBCRYPTO)
#include "archive_openssl_hmac_private.h"

typedef HMAC_CTX* archive_hmac_sha1_ctx;

#else

typedef int archive_hmac_sha1_ctx;

#endif



#define archive_hmac_sha1_init(ctx, key, key_len)__archive_hmac.__hmac_sha1_init(ctx, key, key_len)

#define archive_hmac_sha1_update(ctx, data, data_len)__archive_hmac.__hmac_sha1_update(ctx, data, data_len)

#define archive_hmac_sha1_final(ctx, out, out_len)__archive_hmac.__hmac_sha1_final(ctx, out, out_len)

#define archive_hmac_sha1_cleanup(ctx)__archive_hmac.__hmac_sha1_cleanup(ctx)



struct archive_hmac {

int (*__hmac_sha1_init)(archive_hmac_sha1_ctx *, const uint8_t *,
size_t);
void (*__hmac_sha1_update)(archive_hmac_sha1_ctx *, const uint8_t *,
size_t);
void (*__hmac_sha1_final)(archive_hmac_sha1_ctx *, uint8_t *, size_t *);
void (*__hmac_sha1_cleanup)(archive_hmac_sha1_ctx *);
};

extern const struct archive_hmac __archive_hmac;
#endif
