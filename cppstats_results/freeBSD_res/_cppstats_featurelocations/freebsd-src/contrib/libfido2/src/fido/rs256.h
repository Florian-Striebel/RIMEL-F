





#if !defined(_FIDO_RS256_H)
#define _FIDO_RS256_H

#include <openssl/rsa.h>

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

rs256_pk_t *rs256_pk_new(void);
void rs256_pk_free(rs256_pk_t **);
EVP_PKEY *rs256_pk_to_EVP_PKEY(const rs256_pk_t *);

int rs256_pk_from_RSA(rs256_pk_t *, const RSA *);
int rs256_pk_from_ptr(rs256_pk_t *, const void *, size_t);

#if defined(__cplusplus)
}
#endif

#endif
