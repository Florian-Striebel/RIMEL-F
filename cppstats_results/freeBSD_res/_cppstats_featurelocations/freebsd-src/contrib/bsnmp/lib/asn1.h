































#if !defined(asn1_h_)
#define asn1_h_

#include <sys/types.h>

struct asn_buf {
union {
u_char *ptr;
const u_char *cptr;
} asn_u;
size_t asn_len;
};
#define asn_cptr asn_u.cptr
#define asn_ptr asn_u.ptr


#define ASN_MAXID 0xffffffff
#define ASN_MAXOIDLEN 128


#define ASN_OIDSTRLEN (ASN_MAXOIDLEN * (10 + 1) - 1 + 1)


typedef uint32_t asn_subid_t;

struct asn_oid {
u_int len;
asn_subid_t subs[ASN_MAXOIDLEN];
};

enum asn_err {

ASN_ERR_OK = 0,

ASN_ERR_FAILED = 1 | 0x1000,

ASN_ERR_BADLEN = 2,

ASN_ERR_EOBUF = 3 | 0x1000,

ASN_ERR_RANGE = 4,

ASN_ERR_TAG = 5 | 0x1000,
};
#define ASN_ERR_STOPPED(E) (((E) & 0x1000) != 0)



typedef uint32_t asn_len_t;


#define ASN_MAXLEN 65535
#define ASN_MAXLENLEN 2


#define ASN_MAXOCTETSTRING 65535

extern void (*asn_error)(const struct asn_buf *, const char *, ...);

enum asn_err asn_get_header(struct asn_buf *, u_char *, asn_len_t *);
enum asn_err asn_put_header(struct asn_buf *, u_char, asn_len_t);

enum asn_err asn_put_temp_header(struct asn_buf *, u_char, u_char **);
enum asn_err asn_commit_header(struct asn_buf *, u_char *, size_t *);

enum asn_err asn_get_integer_raw(struct asn_buf *, asn_len_t, int32_t *);
enum asn_err asn_get_integer(struct asn_buf *, int32_t *);
enum asn_err asn_put_integer(struct asn_buf *, int32_t);

enum asn_err asn_get_octetstring_raw(struct asn_buf *, asn_len_t, u_char *, u_int *);
enum asn_err asn_get_octetstring(struct asn_buf *, u_char *, u_int *);
enum asn_err asn_put_octetstring(struct asn_buf *, const u_char *, u_int);

enum asn_err asn_get_null_raw(struct asn_buf *b, asn_len_t);
enum asn_err asn_get_null(struct asn_buf *);
enum asn_err asn_put_null(struct asn_buf *);

enum asn_err asn_put_exception(struct asn_buf *, u_int);

enum asn_err asn_get_objid_raw(struct asn_buf *, asn_len_t, struct asn_oid *);
enum asn_err asn_get_objid(struct asn_buf *, struct asn_oid *);
enum asn_err asn_put_objid(struct asn_buf *, const struct asn_oid *);

enum asn_err asn_get_sequence(struct asn_buf *, asn_len_t *);

enum asn_err asn_get_ipaddress_raw(struct asn_buf *, asn_len_t, u_char *);
enum asn_err asn_get_ipaddress(struct asn_buf *, u_char *);
enum asn_err asn_put_ipaddress(struct asn_buf *, const u_char *);

enum asn_err asn_get_uint32_raw(struct asn_buf *, asn_len_t, uint32_t *);
enum asn_err asn_put_uint32(struct asn_buf *, u_char, uint32_t);

enum asn_err asn_get_counter64_raw(struct asn_buf *, asn_len_t, uint64_t *);
enum asn_err asn_put_counter64(struct asn_buf *, uint64_t);

enum asn_err asn_get_timeticks(struct asn_buf *, uint32_t *);
enum asn_err asn_put_timeticks(struct asn_buf *, uint32_t);

enum asn_err asn_skip(struct asn_buf *, asn_len_t);
enum asn_err asn_pad(struct asn_buf *, asn_len_t);





void asn_slice_oid(struct asn_oid *, const struct asn_oid *, u_int, u_int);


void asn_append_oid(struct asn_oid *, const struct asn_oid *);


int asn_compare_oid(const struct asn_oid *, const struct asn_oid *);


int asn_is_suboid(const struct asn_oid *, const struct asn_oid *);


char *asn_oid2str_r(const struct asn_oid *, char *);


char *asn_oid2str(const struct asn_oid *);

enum {
ASN_TYPE_BOOLEAN = 0x01,
ASN_TYPE_INTEGER = 0x02,
ASN_TYPE_BITSTRING = 0x03,
ASN_TYPE_OCTETSTRING = 0x04,
ASN_TYPE_NULL = 0x05,
ASN_TYPE_OBJID = 0x06,
ASN_TYPE_SEQUENCE = 0x10,

ASN_TYPE_CONSTRUCTED = 0x20,
ASN_CLASS_UNIVERSAL = 0x00,
ASN_CLASS_APPLICATION = 0x40,
ASN_CLASS_CONTEXT = 0x80,
ASN_CLASS_PRIVATE = 0xc0,
ASN_TYPE_MASK = 0x1f,

ASN_APP_IPADDRESS = 0x00,
ASN_APP_COUNTER = 0x01,
ASN_APP_GAUGE = 0x02,
ASN_APP_TIMETICKS = 0x03,
ASN_APP_OPAQUE = 0x04,
ASN_APP_COUNTER64 = 0x06,

ASN_EXCEPT_NOSUCHOBJECT = 0x00,
ASN_EXCEPT_NOSUCHINSTANCE = 0x01,
ASN_EXCEPT_ENDOFMIBVIEW = 0x02,
};

#endif
