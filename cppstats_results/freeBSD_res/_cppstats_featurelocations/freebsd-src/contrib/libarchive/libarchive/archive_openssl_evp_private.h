
























#if !defined(ARCHIVE_OPENSSL_EVP_PRIVATE_H_INCLUDED)
#define ARCHIVE_OPENSSL_EVP_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

#include <openssl/evp.h>
#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#include <stdlib.h>
#include <string.h>
static inline EVP_MD_CTX *EVP_MD_CTX_new(void)
{
EVP_MD_CTX *ctx = (EVP_MD_CTX *)calloc(1, sizeof(EVP_MD_CTX));
return ctx;
}

static inline void EVP_MD_CTX_free(EVP_MD_CTX *ctx)
{
EVP_MD_CTX_cleanup(ctx);
memset(ctx, 0, sizeof(*ctx));
free(ctx);
}
#endif

#endif
