





#if !defined(_FIDO_TYPES_H)
#define _FIDO_TYPES_H

#if defined(__MINGW32__)
#include <sys/types.h>
#endif

#include <signal.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct fido_dev;

typedef void *fido_dev_io_open_t(const char *);
typedef void fido_dev_io_close_t(void *);
typedef int fido_dev_io_read_t(void *, unsigned char *, size_t, int);
typedef int fido_dev_io_write_t(void *, const unsigned char *, size_t);
typedef int fido_dev_rx_t(struct fido_dev *, uint8_t, unsigned char *, size_t, int);
typedef int fido_dev_tx_t(struct fido_dev *, uint8_t, const unsigned char *, size_t);

typedef struct fido_dev_io {
fido_dev_io_open_t *open;
fido_dev_io_close_t *close;
fido_dev_io_read_t *read;
fido_dev_io_write_t *write;
} fido_dev_io_t;

typedef struct fido_dev_transport {
fido_dev_rx_t *rx;
fido_dev_tx_t *tx;
} fido_dev_transport_t;

typedef enum {
FIDO_OPT_OMIT = 0,
FIDO_OPT_FALSE,
FIDO_OPT_TRUE,
} fido_opt_t;

typedef void fido_log_handler_t(const char *);

#if defined(_WIN32)
typedef int fido_sigset_t;
#else
typedef sigset_t fido_sigset_t;
#endif

#if defined(_FIDO_INTERNAL)
#include "packed.h"
#include "blob.h"


typedef struct es256_pk {
unsigned char x[32];
unsigned char y[32];
} es256_pk_t;


typedef struct es256_sk {
unsigned char d[32];
} es256_sk_t;


typedef struct rs256_pk {
unsigned char n[256];
unsigned char e[3];
} rs256_pk_t;


typedef struct eddsa_pk {
unsigned char x[32];
} eddsa_pk_t;

PACKED_TYPE(fido_authdata_t,
struct fido_authdata {
unsigned char rp_id_hash[32];
uint8_t flags;
uint32_t sigcount;

})

PACKED_TYPE(fido_attcred_raw_t,
struct fido_attcred_raw {
unsigned char aaguid[16];
uint16_t id_len;
uint8_t body[];
})

typedef struct fido_attcred {
unsigned char aaguid[16];
fido_blob_t id;
int type;
union {
es256_pk_t es256;
rs256_pk_t rs256;
eddsa_pk_t eddsa;
} pubkey;
} fido_attcred_t;

typedef struct fido_attstmt {
fido_blob_t x5c;
fido_blob_t sig;
} fido_attstmt_t;

typedef struct fido_rp {
char *id;
char *name;
} fido_rp_t;

typedef struct fido_user {
fido_blob_t id;
char *icon;
char *name;
char *display_name;
} fido_user_t;

typedef struct fido_cred_ext {
int mask;
int prot;
} fido_cred_ext_t;

typedef struct fido_cred {
fido_blob_t cd;
fido_blob_t cdh;
fido_rp_t rp;
fido_user_t user;
fido_blob_array_t excl;
fido_opt_t rk;
fido_opt_t uv;
fido_cred_ext_t ext;
int type;
char *fmt;
fido_cred_ext_t authdata_ext;
fido_blob_t authdata_cbor;
fido_blob_t authdata_raw;
fido_authdata_t authdata;
fido_attcred_t attcred;
fido_attstmt_t attstmt;
fido_blob_t largeblob_key;
fido_blob_t blob;
} fido_cred_t;

typedef struct fido_assert_extattr {
int mask;
fido_blob_t hmac_secret_enc;
fido_blob_t blob;
} fido_assert_extattr_t;

typedef struct _fido_assert_stmt {
fido_blob_t id;
fido_user_t user;
fido_blob_t hmac_secret;
fido_assert_extattr_t authdata_ext;
fido_blob_t authdata_cbor;
fido_authdata_t authdata;
fido_blob_t sig;
fido_blob_t largeblob_key;
} fido_assert_stmt;

typedef struct fido_assert_ext {
int mask;
fido_blob_t hmac_salt;
} fido_assert_ext_t;

typedef struct fido_assert {
char *rp_id;
fido_blob_t cd;
fido_blob_t cdh;
fido_blob_array_t allow_list;
fido_opt_t up;
fido_opt_t uv;
fido_assert_ext_t ext;
fido_assert_stmt *stmt;
size_t stmt_cnt;
size_t stmt_len;
} fido_assert_t;

typedef struct fido_opt_array {
char **name;
bool *value;
size_t len;
} fido_opt_array_t;

typedef struct fido_str_array {
char **ptr;
size_t len;
} fido_str_array_t;

typedef struct fido_byte_array {
uint8_t *ptr;
size_t len;
} fido_byte_array_t;

typedef struct fido_algo {
char *type;
int cose;
} fido_algo_t;

typedef struct fido_algo_array {
fido_algo_t *ptr;
size_t len;
} fido_algo_array_t;

typedef struct fido_cbor_info {
fido_str_array_t versions;
fido_str_array_t extensions;
fido_str_array_t transports;
unsigned char aaguid[16];
fido_opt_array_t options;
uint64_t maxmsgsiz;
fido_byte_array_t protocols;
fido_algo_array_t algorithms;
uint64_t maxcredcntlst;
uint64_t maxcredidlen;
uint64_t fwversion;
uint64_t maxcredbloblen;
} fido_cbor_info_t;

typedef struct fido_dev_info {
char *path;
int16_t vendor_id;
int16_t product_id;
char *manufacturer;
char *product;
fido_dev_io_t io;
fido_dev_transport_t transport;
} fido_dev_info_t;

PACKED_TYPE(fido_ctap_info_t,

struct fido_ctap_info {
uint64_t nonce;
uint32_t cid;
uint8_t protocol;
uint8_t major;
uint8_t minor;
uint8_t build;
uint8_t flags;
})

typedef struct fido_dev {
uint64_t nonce;
fido_ctap_info_t attr;
uint32_t cid;
char *path;
void *io_handle;
fido_dev_io_t io;
bool io_own;
size_t rx_len;
size_t tx_len;
int flags;
fido_dev_transport_t transport;
uint64_t maxmsgsize;
} fido_dev_t;

#else
typedef struct fido_assert fido_assert_t;
typedef struct fido_cbor_info fido_cbor_info_t;
typedef struct fido_cred fido_cred_t;
typedef struct fido_dev fido_dev_t;
typedef struct fido_dev_info fido_dev_info_t;
typedef struct es256_pk es256_pk_t;
typedef struct es256_sk es256_sk_t;
typedef struct rs256_pk rs256_pk_t;
typedef struct eddsa_pk eddsa_pk_t;
#endif

#if defined(__cplusplus)
}
#endif

#endif
