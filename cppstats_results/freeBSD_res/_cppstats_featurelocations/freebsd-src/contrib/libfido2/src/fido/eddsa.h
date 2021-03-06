





#if !defined(_FIDO_EDDSA_H)
#define _FIDO_EDDSA_H

#include <openssl/ec.h>

#include <stdint.h>
#include <stdlib.h>

#if defined(_FIDO_INTERNAL)
#include "types.h"
#else
#include <fido.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

eddsa_pk_t *eddsa_pk_new(void);
void eddsa_pk_free(eddsa_pk_t **);
EVP_PKEY *eddsa_pk_to_EVP_PKEY(const eddsa_pk_t *);

int eddsa_pk_from_EVP_PKEY(eddsa_pk_t *, const EVP_PKEY *);
int eddsa_pk_from_ptr(eddsa_pk_t *, const void *, size_t);

#if defined(_FIDO_INTERNAL)

#if defined(LIBRESSL_VERSION_NUMBER) || OPENSSL_VERSION_NUMBER < 0x10101000L
#define EVP_PKEY_ED25519 EVP_PKEY_NONE
int EVP_PKEY_get_raw_public_key(const EVP_PKEY *, unsigned char *, size_t *);
EVP_PKEY *EVP_PKEY_new_raw_public_key(int, ENGINE *, const unsigned char *,
size_t);
int EVP_DigestVerify(EVP_MD_CTX *, const unsigned char *, size_t,
const unsigned char *, size_t);
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L
EVP_MD_CTX *EVP_MD_CTX_new(void);
void EVP_MD_CTX_free(EVP_MD_CTX *);
#endif

#endif

#if defined(__cplusplus)
}
#endif

#endif
