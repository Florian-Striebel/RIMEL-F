





#if !defined(_FIDO_CREDMAN_H)
#define _FIDO_CREDMAN_H

#include <stdint.h>
#include <stdlib.h>

#if defined(_FIDO_INTERNAL)
#include "blob.h"
#include "fido/err.h"
#include "fido/param.h"
#include "fido/types.h"
#else
#include <fido.h>
#include <fido/err.h>
#include <fido/param.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_FIDO_INTERNAL)
struct fido_credman_metadata {
uint64_t rk_existing;
uint64_t rk_remaining;
};

struct fido_credman_single_rp {
fido_rp_t rp_entity;
fido_blob_t rp_id_hash;
};

struct fido_credman_rp {
struct fido_credman_single_rp *ptr;
size_t n_alloc;
size_t n_rx;
};

struct fido_credman_rk {
fido_cred_t *ptr;
size_t n_alloc;
size_t n_rx;
};
#endif

typedef struct fido_credman_metadata fido_credman_metadata_t;
typedef struct fido_credman_rk fido_credman_rk_t;
typedef struct fido_credman_rp fido_credman_rp_t;

const char *fido_credman_rp_id(const fido_credman_rp_t *, size_t);
const char *fido_credman_rp_name(const fido_credman_rp_t *, size_t);

const fido_cred_t *fido_credman_rk(const fido_credman_rk_t *, size_t);
const unsigned char *fido_credman_rp_id_hash_ptr(const fido_credman_rp_t *,
size_t);

fido_credman_metadata_t *fido_credman_metadata_new(void);
fido_credman_rk_t *fido_credman_rk_new(void);
fido_credman_rp_t *fido_credman_rp_new(void);

int fido_credman_del_dev_rk(fido_dev_t *, const unsigned char *, size_t,
const char *);
int fido_credman_get_dev_metadata(fido_dev_t *, fido_credman_metadata_t *,
const char *);
int fido_credman_get_dev_rk(fido_dev_t *, const char *, fido_credman_rk_t *,
const char *);
int fido_credman_get_dev_rp(fido_dev_t *, fido_credman_rp_t *, const char *);
int fido_credman_set_dev_rk(fido_dev_t *, fido_cred_t *, const char *);

size_t fido_credman_rk_count(const fido_credman_rk_t *);
size_t fido_credman_rp_count(const fido_credman_rp_t *);
size_t fido_credman_rp_id_hash_len(const fido_credman_rp_t *, size_t);

uint64_t fido_credman_rk_existing(const fido_credman_metadata_t *);
uint64_t fido_credman_rk_remaining(const fido_credman_metadata_t *);

void fido_credman_metadata_free(fido_credman_metadata_t **);
void fido_credman_rk_free(fido_credman_rk_t **);
void fido_credman_rp_free(fido_credman_rp_t **);

#if defined(__cplusplus)
}
#endif

#endif
