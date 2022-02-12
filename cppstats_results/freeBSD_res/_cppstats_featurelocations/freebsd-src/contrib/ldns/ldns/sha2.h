












































#if !defined(__LDNS_SHA2_H__)
#define __LDNS_SHA2_H__

#if defined(__cplusplus)
extern "C" {
#endif







#include <sys/types.h>

#if LDNS_BUILD_CONFIG_HAVE_INTTYPES_H

#include <inttypes.h>

#endif



#define LDNS_SHA256_BLOCK_LENGTH 64
#define LDNS_SHA256_DIGEST_LENGTH 32
#define LDNS_SHA256_DIGEST_STRING_LENGTH (LDNS_SHA256_DIGEST_LENGTH * 2 + 1)
#define LDNS_SHA384_BLOCK_LENGTH 128
#define LDNS_SHA384_DIGEST_LENGTH 48
#define LDNS_SHA384_DIGEST_STRING_LENGTH (LDNS_SHA384_DIGEST_LENGTH * 2 + 1)
#define LDNS_SHA512_BLOCK_LENGTH 128
#define LDNS_SHA512_DIGEST_LENGTH 64
#define LDNS_SHA512_DIGEST_STRING_LENGTH (LDNS_SHA512_DIGEST_LENGTH * 2 + 1)




typedef struct _ldns_sha256_CTX {
uint32_t state[8];
uint64_t bitcount;
uint8_t buffer[LDNS_SHA256_BLOCK_LENGTH];
} ldns_sha256_CTX;
typedef struct _ldns_sha512_CTX {
uint64_t state[8];
uint64_t bitcount[2];
uint8_t buffer[LDNS_SHA512_BLOCK_LENGTH];
} ldns_sha512_CTX;

typedef ldns_sha512_CTX ldns_sha384_CTX;



void ldns_sha256_init(ldns_sha256_CTX *);
void ldns_sha256_update(ldns_sha256_CTX*, const uint8_t*, size_t);
void ldns_sha256_final(uint8_t[LDNS_SHA256_DIGEST_LENGTH], ldns_sha256_CTX*);

void ldns_sha384_init(ldns_sha384_CTX*);
void ldns_sha384_update(ldns_sha384_CTX*, const uint8_t*, size_t);
void ldns_sha384_final(uint8_t[LDNS_SHA384_DIGEST_LENGTH], ldns_sha384_CTX*);

void ldns_sha512_init(ldns_sha512_CTX*);
void ldns_sha512_update(ldns_sha512_CTX*, const uint8_t*, size_t);
void ldns_sha512_final(uint8_t[LDNS_SHA512_DIGEST_LENGTH], ldns_sha512_CTX*);











unsigned char *ldns_sha256(unsigned char *data, unsigned int data_len, unsigned char *digest);











unsigned char *ldns_sha384(unsigned char *data, unsigned int data_len, unsigned char *digest);











unsigned char *ldns_sha512(unsigned char *data, unsigned int data_len, unsigned char *digest);

#if defined(__cplusplus)
}
#endif

#endif
