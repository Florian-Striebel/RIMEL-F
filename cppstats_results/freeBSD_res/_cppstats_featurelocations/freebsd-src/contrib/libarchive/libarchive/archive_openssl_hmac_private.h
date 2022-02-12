
























#if !defined(ARCHIVE_OPENSSL_HMAC_PRIVATE_H_INCLUDED)
#define ARCHIVE_OPENSSL_HMAC_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

#include <openssl/hmac.h>
#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER < 0x10100000L || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x20700000L)

#include <stdlib.h>
#include <string.h>
static inline HMAC_CTX *HMAC_CTX_new(void)
{
HMAC_CTX *ctx = (HMAC_CTX *)calloc(1, sizeof(HMAC_CTX));
return ctx;
}

static inline void HMAC_CTX_free(HMAC_CTX *ctx)
{
HMAC_CTX_cleanup(ctx);
memset(ctx, 0, sizeof(*ctx));
free(ctx);
}
#endif

#endif
