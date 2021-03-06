























#if !defined(BRSSL_H__)
#define BRSSL_H__

#if !defined(_STANDALONE)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#elif !defined(STAND_H)
#include <stand.h>
#endif

#include "bearssl.h"







void *xmalloc(size_t len);




void xfree(void *buf);




char *xstrdup(const void *src);





void *xblobdup(const void *src, size_t len);





br_x509_pkey *xpkeydup(const br_x509_pkey *pk);





void xfreepkey(br_x509_pkey *pk);








#define VECTOR(type) struct { type *buf; size_t ptr, len; }







#define VEC_INIT { 0, 0, 0 }




#define VEC_CLEAR(vec) do { xfree((vec).buf); (vec).buf = NULL; (vec).ptr = 0; (vec).len = 0; } while (0)










#define VEC_CLEAREXT(vec, fun) do { size_t vec_tmp; for (vec_tmp = 0; vec_tmp < (vec).ptr; vec_tmp ++) { (fun)(&(vec).buf[vec_tmp]); } VEC_CLEAR(vec); } while (0)










#define VEC_ADD(vec, x) do { (vec).buf = vector_expand((vec).buf, sizeof *((vec).buf), &(vec).ptr, &(vec).len, 1); (vec).buf[(vec).ptr ++] = (x); } while (0)








#define VEC_ADDMANY(vec, xp, num) do { size_t vec_num = (num); (vec).buf = vector_expand((vec).buf, sizeof *((vec).buf), &(vec).ptr, &(vec).len, vec_num); memcpy((vec).buf + (vec).ptr, (xp), vec_num * sizeof *((vec).buf)); (vec).ptr += vec_num; } while (0)











#define VEC_ELT(vec, idx) ((vec).buf[idx])




#define VEC_LEN(vec) ((vec).ptr)




#define VEC_TOARRAY(vec) xblobdup((vec).buf, sizeof *((vec).buf) * (vec).ptr)




void *vector_expand(void *buf,
size_t esize, size_t *ptr, size_t *len, size_t extra);




typedef VECTOR(unsigned char) bvector;








int eqstr(const char *s1, const char *s2);





size_t parse_size(const char *s);




typedef struct {
const char *name;
unsigned version;
const char *comment;
} protocol_version;




extern const protocol_version protocol_versions[];





unsigned parse_version(const char *name, size_t len);




typedef struct {
const char *name;
const br_hash_class *hclass;
const char *comment;
} hash_function;




extern const hash_function hash_functions[];







unsigned parse_hash_functions(const char *arg);





const char *get_curve_name(int id);










int get_curve_name_ext(int id, char *dst, size_t len);




typedef struct {
const char *name;
uint16_t suite;
unsigned req;
const char *comment;
} cipher_suite;




extern const cipher_suite cipher_suites[];




#define REQ_TLS12 0x0001
#define REQ_SHA1 0x0002
#define REQ_SHA256 0x0004
#define REQ_SHA384 0x0008
#define REQ_AESCBC 0x0010
#define REQ_AESGCM 0x0020
#define REQ_AESCCM 0x0040
#define REQ_CHAPOL 0x0080
#define REQ_3DESCBC 0x0100
#define REQ_RSAKEYX 0x0200
#define REQ_ECDHE_RSA 0x0400
#define REQ_ECDHE_ECDSA 0x0800
#define REQ_ECDH 0x1000










cipher_suite *parse_suites(const char *arg, size_t *num);





const char *get_suite_name(unsigned suite);










int get_suite_name_ext(unsigned suite, char *dst, size_t len);




int uses_ecdhe(unsigned suite);




void list_names(void);




void list_curves(void);




const char *ec_curve_name(int curve);





int get_curve_by_name(const char *str);




const char *hash_function_name(int id);







unsigned char *read_file(const char *fname, size_t *len);





int write_file(const char *fname, const void *data, size_t len);






int looks_like_DER(const unsigned char *buf, size_t len);




typedef struct {
char *name;
unsigned char *data;
size_t data_len;
} pem_object;




void free_pem_object_contents(pem_object *po);










pem_object *decode_pem(const void *src, size_t len, size_t *num);
















br_x509_certificate *read_certificates(const char *fname, size_t *num);





void free_certificates(br_x509_certificate *certs, size_t num);







br_x509_trust_anchor *certificate_to_trust_anchor(br_x509_certificate *xc);




typedef VECTOR(br_x509_trust_anchor) anchor_list;





void free_ta_contents(br_x509_trust_anchor *ta);







size_t read_trust_anchors(anchor_list *dst, const char *fname);





int get_cert_signer_algo(br_x509_certificate *xc);






typedef struct {
const br_x509_class *vtable;
const br_x509_class **inner;
} x509_noanchor_context;
extern const br_x509_class x509_noanchor_vtable;




void x509_noanchor_init(x509_noanchor_context *xwc,
const br_x509_class **inner);




typedef struct {
int key_type;
union {
br_rsa_private_key rsa;
br_ec_private_key ec;
} key;
} private_key;





private_key *read_private_key(const char *fname);




void free_private_key(private_key *sk);







const unsigned char *get_hash_oid(int id);





const br_hash_class *get_hash_impl(int id);








const char *find_error_name(int err, const char **comment);











const char *get_algo_name(const void *algo, int long_name);















int run_ssl_engine(br_ssl_engine_context *eng,
unsigned long fd, unsigned flags);

#define RUN_ENGINE_VERBOSE 0x0001
#define RUN_ENGINE_TRACE 0x0002





int do_client(int argc, char *argv[]);





int do_server(int argc, char *argv[]);





int do_verify(int argc, char *argv[]);





int do_skey(int argc, char *argv[]);





int do_ta(int argc, char *argv[]);





int do_chain(int argc, char *argv[]);






int do_twrch(int argc, char *argv[]);





int do_impl(int argc, char *argv[]);

#endif
