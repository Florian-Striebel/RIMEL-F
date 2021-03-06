





#if !defined(_FIDO_ES256_H)
#define _FIDO_ES256_H

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

es256_pk_t *es256_pk_new(void);
void es256_pk_free(es256_pk_t **);
EVP_PKEY *es256_pk_to_EVP_PKEY(const es256_pk_t *);

int es256_pk_from_EC_KEY(es256_pk_t *, const EC_KEY *);
int es256_pk_from_ptr(es256_pk_t *, const void *, size_t);

#if defined(_FIDO_INTERNAL)
es256_sk_t *es256_sk_new(void);
void es256_sk_free(es256_sk_t **);
EVP_PKEY *es256_sk_to_EVP_PKEY(const es256_sk_t *);

int es256_derive_pk(const es256_sk_t *, es256_pk_t *);
int es256_sk_create(es256_sk_t *);

int es256_pk_set_x(es256_pk_t *, const unsigned char *);
int es256_pk_set_y(es256_pk_t *, const unsigned char *);
#endif

#if defined(__cplusplus)
}
#endif

#endif
