










#include <ldns/config.h>

#include <ldns/ldns.h>
#include <ldns/dnssec.h>

#include <strings.h>
#include <time.h>

#if defined(HAVE_SSL)
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/md5.h>
#endif

ldns_rr *
ldns_dnssec_get_rrsig_for_name_and_type(const ldns_rdf *name,
const ldns_rr_type type,
const ldns_rr_list *rrs)
{
size_t i;
ldns_rr *candidate;

if (!name || !rrs) {
return NULL;
}

for (i = 0; i < ldns_rr_list_rr_count(rrs); i++) {
candidate = ldns_rr_list_rr(rrs, i);
if (ldns_rr_get_type(candidate) == LDNS_RR_TYPE_RRSIG) {
if (ldns_dname_compare(ldns_rr_owner(candidate),
name) == 0 &&
ldns_rdf2rr_type(ldns_rr_rrsig_typecovered(candidate))
== type
) {
return candidate;
}
}
}

return NULL;
}

ldns_rr *
ldns_dnssec_get_dnskey_for_rrsig(const ldns_rr *rrsig,
const ldns_rr_list *rrs)
{
size_t i;
ldns_rr *candidate;

if (!rrsig || !rrs) {
return NULL;
}

for (i = 0; i < ldns_rr_list_rr_count(rrs); i++) {
candidate = ldns_rr_list_rr(rrs, i);
if (ldns_rr_get_type(candidate) == LDNS_RR_TYPE_DNSKEY) {
if (ldns_dname_compare(ldns_rr_owner(candidate),
ldns_rr_rrsig_signame(rrsig)) == 0 &&
ldns_rdf2native_int16(ldns_rr_rrsig_keytag(rrsig)) ==
ldns_calc_keytag(candidate)
) {
return candidate;
}
}
}

return NULL;
}

ldns_rdf *
ldns_nsec_get_bitmap(const ldns_rr *nsec) {
if (ldns_rr_get_type(nsec) == LDNS_RR_TYPE_NSEC) {
return ldns_rr_rdf(nsec, 1);
} else if (ldns_rr_get_type(nsec) == LDNS_RR_TYPE_NSEC3) {
return ldns_rr_rdf(nsec, 5);
} else {
return NULL;
}
}



ldns_rdf *
ldns_dnssec_nsec3_closest_encloser(const ldns_rdf *qname,
ATTR_UNUSED(ldns_rr_type qtype),
const ldns_rr_list *nsec3s)
{

uint8_t algorithm;
uint32_t iterations;
uint8_t salt_length;
uint8_t *salt;

ldns_rdf *sname, *hashed_sname, *tmp;
bool flag;

bool exact_match_found;
bool in_range_found;

ldns_status status;
ldns_rdf *zone_name;

size_t nsec_i;
ldns_rr *nsec;
ldns_rdf *result = NULL;

if (!qname || !nsec3s || ldns_rr_list_rr_count(nsec3s) < 1) {
return NULL;
}

nsec = ldns_rr_list_rr(nsec3s, 0);
algorithm = ldns_nsec3_algorithm(nsec);
salt_length = ldns_nsec3_salt_length(nsec);
salt = ldns_nsec3_salt_data(nsec);
iterations = ldns_nsec3_iterations(nsec);

sname = ldns_rdf_clone(qname);

flag = false;

zone_name = ldns_dname_left_chop(ldns_rr_owner(nsec));


while (ldns_dname_label_count(sname) > 0) {
exact_match_found = false;
in_range_found = false;

hashed_sname = ldns_nsec3_hash_name(sname,
algorithm,
iterations,
salt_length,
salt);

status = ldns_dname_cat(hashed_sname, zone_name);
if(status != LDNS_STATUS_OK) {
LDNS_FREE(salt);
ldns_rdf_deep_free(zone_name);
ldns_rdf_deep_free(sname);
return NULL;
}

for (nsec_i = 0; nsec_i < ldns_rr_list_rr_count(nsec3s); nsec_i++) {
nsec = ldns_rr_list_rr(nsec3s, nsec_i);




if (ldns_dname_compare(ldns_rr_owner(nsec), hashed_sname) == 0) {
exact_match_found = true;
} else if (ldns_nsec_covers_name(nsec, hashed_sname)) {
in_range_found = true;
}

}
if (!exact_match_found && in_range_found) {
flag = true;
} else if (exact_match_found && flag) {
result = ldns_rdf_clone(sname);

ldns_rdf_deep_free(hashed_sname);
goto done;
} else if (exact_match_found && !flag) {

ldns_rdf_deep_free(hashed_sname);
goto done;
} else {
flag = false;
}

ldns_rdf_deep_free(hashed_sname);
tmp = sname;
sname = ldns_dname_left_chop(sname);
ldns_rdf_deep_free(tmp);
}

done:
LDNS_FREE(salt);
ldns_rdf_deep_free(zone_name);
ldns_rdf_deep_free(sname);

return result;
}

bool
ldns_dnssec_pkt_has_rrsigs(const ldns_pkt *pkt)
{
size_t i;
for (i = 0; i < ldns_pkt_ancount(pkt); i++) {
if (ldns_rr_get_type(ldns_rr_list_rr(ldns_pkt_answer(pkt), i)) ==
LDNS_RR_TYPE_RRSIG) {
return true;
}
}
for (i = 0; i < ldns_pkt_nscount(pkt); i++) {
if (ldns_rr_get_type(ldns_rr_list_rr(ldns_pkt_authority(pkt), i)) ==
LDNS_RR_TYPE_RRSIG) {
return true;
}
}
return false;
}

ldns_rr_list *
ldns_dnssec_pkt_get_rrsigs_for_name_and_type(const ldns_pkt *pkt,
const ldns_rdf *name,
ldns_rr_type type)
{
uint16_t t_netorder;
ldns_rr_list *sigs;
ldns_rr_list *sigs_covered;
ldns_rdf *rdf_t;

sigs = ldns_pkt_rr_list_by_name_and_type(pkt,
name,
LDNS_RR_TYPE_RRSIG,
LDNS_SECTION_ANY_NOQUESTION
);

t_netorder = htons(type);
rdf_t = ldns_rdf_new(LDNS_RDF_TYPE_TYPE, LDNS_RDF_SIZE_WORD, &t_netorder);
sigs_covered = ldns_rr_list_subtype_by_rdf(sigs, rdf_t, 0);

ldns_rdf_free(rdf_t);
ldns_rr_list_deep_free(sigs);

return sigs_covered;

}

ldns_rr_list *
ldns_dnssec_pkt_get_rrsigs_for_type(const ldns_pkt *pkt, ldns_rr_type type)
{
uint16_t t_netorder;
ldns_rr_list *sigs;
ldns_rr_list *sigs_covered;
ldns_rdf *rdf_t;

sigs = ldns_pkt_rr_list_by_type(pkt,
LDNS_RR_TYPE_RRSIG,
LDNS_SECTION_ANY_NOQUESTION
);

t_netorder = htons(type);
rdf_t = ldns_rdf_new(LDNS_RDF_TYPE_TYPE,
2,
&t_netorder);
sigs_covered = ldns_rr_list_subtype_by_rdf(sigs, rdf_t, 0);

ldns_rdf_free(rdf_t);
ldns_rr_list_deep_free(sigs);

return sigs_covered;

}


uint16_t
ldns_calc_keytag(const ldns_rr *key)
{
uint16_t ac16;
ldns_buffer *keybuf;
size_t keysize;

if (!key) {
return 0;
}

if (ldns_rr_get_type(key) != LDNS_RR_TYPE_DNSKEY &&
ldns_rr_get_type(key) != LDNS_RR_TYPE_KEY
) {
return 0;
}


keybuf = ldns_buffer_new(LDNS_MIN_BUFLEN);
if (!keybuf) {
return 0;
}
(void)ldns_rr_rdata2buffer_wire(keybuf, key);

keysize= ldns_buffer_position(keybuf);

ac16 = ldns_calc_keytag_raw(ldns_buffer_begin(keybuf), keysize);
ldns_buffer_free(keybuf);
return ac16;
}

uint16_t ldns_calc_keytag_raw(const uint8_t* key, size_t keysize)
{
unsigned int i;
uint32_t ac32;
uint16_t ac16;

if(keysize < 4) {
return 0;
}

if (key[3] == LDNS_RSAMD5) {
ac16 = 0;
if (keysize > 4) {
memmove(&ac16, key + keysize - 3, 2);
}
ac16 = ntohs(ac16);
return (uint16_t) ac16;
} else {
ac32 = 0;
for (i = 0; (size_t)i < keysize; ++i) {
ac32 += (i & 1) ? key[i] : key[i] << 8;
}
ac32 += (ac32 >> 16) & 0xFFFF;
return (uint16_t) (ac32 & 0xFFFF);
}
}

#if defined(HAVE_SSL)
DSA *
ldns_key_buf2dsa(const ldns_buffer *key)
{
return ldns_key_buf2dsa_raw((const unsigned char*)ldns_buffer_begin(key),
ldns_buffer_position(key));
}

DSA *
ldns_key_buf2dsa_raw(const unsigned char* key, size_t len)
{
uint8_t T;
uint16_t length;
uint16_t offset;
DSA *dsa;
BIGNUM *Q; BIGNUM *P;
BIGNUM *G; BIGNUM *Y;

if(len == 0)
return NULL;
T = (uint8_t)key[0];
length = (64 + T * 8);
offset = 1;

if (T > 8) {
return NULL;
}
if(len < (size_t)1 + SHA_DIGEST_LENGTH + 3*length)
return NULL;

Q = BN_bin2bn(key+offset, SHA_DIGEST_LENGTH, NULL);
offset += SHA_DIGEST_LENGTH;

P = BN_bin2bn(key+offset, (int)length, NULL);
offset += length;

G = BN_bin2bn(key+offset, (int)length, NULL);
offset += length;

Y = BN_bin2bn(key+offset, (int)length, NULL);
offset += length;


if(!Q || !P || !G || !Y || !(dsa = DSA_new())) {
BN_free(Q);
BN_free(P);
BN_free(G);
BN_free(Y);
return NULL;
}
#if OPENSSL_VERSION_NUMBER < 0x10100000 || defined(HAVE_LIBRESSL)
#if !defined(S_SPLINT_S)
dsa->p = P;
dsa->q = Q;
dsa->g = G;
dsa->pub_key = Y;
#endif
#else
if (!DSA_set0_pqg(dsa, P, Q, G)) {

BN_free(Q);
BN_free(P);
BN_free(G);

DSA_free(dsa);
BN_free(Y);
return NULL;
}
if (!DSA_set0_key(dsa, Y, NULL)) {

DSA_free(dsa);
BN_free(Y);
return NULL;
}
#endif
return dsa;
}

RSA *
ldns_key_buf2rsa(const ldns_buffer *key)
{
return ldns_key_buf2rsa_raw((const unsigned char*)ldns_buffer_begin(key),
ldns_buffer_position(key));
}

RSA *
ldns_key_buf2rsa_raw(const unsigned char* key, size_t len)
{
uint16_t offset;
uint16_t exp;
uint16_t int16;
RSA *rsa;
BIGNUM *modulus;
BIGNUM *exponent;

if (len == 0)
return NULL;
if (key[0] == 0) {
if(len < 3)
return NULL;



memmove(&int16, key+1, 2);
exp = ntohs(int16);
offset = 3;
} else {
exp = key[0];
offset = 1;
}


if(len < (size_t)offset + exp + 1)
return NULL;


exponent = BN_new();
if(!exponent) return NULL;
(void) BN_bin2bn(key+offset, (int)exp, exponent);
offset += exp;


modulus = BN_new();
if(!modulus) {
BN_free(exponent);
return NULL;
}

(void) BN_bin2bn(key+offset, (int)(len - offset), modulus);

rsa = RSA_new();
if(!rsa) {
BN_free(exponent);
BN_free(modulus);
return NULL;
}
#if OPENSSL_VERSION_NUMBER < 0x10100000 || defined(HAVE_LIBRESSL)
#if !defined(S_SPLINT_S)
rsa->n = modulus;
rsa->e = exponent;
#endif
#else
if (!RSA_set0_key(rsa, modulus, exponent, NULL)) {
BN_free(exponent);
BN_free(modulus);
RSA_free(rsa);
return NULL;
}
#endif

return rsa;
}

int
ldns_digest_evp(const unsigned char* data, unsigned int len, unsigned char* dest,
const EVP_MD* md)
{
EVP_MD_CTX* ctx;
ctx = EVP_MD_CTX_create();
if(!ctx)
return false;
if(!EVP_DigestInit_ex(ctx, md, NULL) ||
!EVP_DigestUpdate(ctx, data, len) ||
!EVP_DigestFinal_ex(ctx, dest, NULL)) {
EVP_MD_CTX_destroy(ctx);
return false;
}
EVP_MD_CTX_destroy(ctx);
return true;
}
#endif

ldns_rr *
ldns_key_rr2ds(const ldns_rr *key, ldns_hash h)
{
ldns_rdf *tmp;
ldns_rr *ds;
uint16_t keytag;
uint8_t sha1hash;
uint8_t *digest;
ldns_buffer *data_buf;
#if defined(USE_GOST)
const EVP_MD* md = NULL;
#endif

if (ldns_rr_get_type(key) != LDNS_RR_TYPE_DNSKEY) {
return NULL;
}

ds = ldns_rr_new();
if (!ds) {
return NULL;
}
ldns_rr_set_type(ds, LDNS_RR_TYPE_DS);
ldns_rr_set_owner(ds, ldns_rdf_clone(
ldns_rr_owner(key)));
ldns_rr_set_ttl(ds, ldns_rr_ttl(key));
ldns_rr_set_class(ds, ldns_rr_get_class(key));

switch(h) {
default:
case LDNS_SHA1:
digest = LDNS_XMALLOC(uint8_t, LDNS_SHA1_DIGEST_LENGTH);
if (!digest) {
ldns_rr_free(ds);
return NULL;
}
break;
case LDNS_SHA256:
digest = LDNS_XMALLOC(uint8_t, LDNS_SHA256_DIGEST_LENGTH);
if (!digest) {
ldns_rr_free(ds);
return NULL;
}
break;
case LDNS_HASH_GOST:
#if defined(USE_GOST)
(void)ldns_key_EVP_load_gost_id();
md = EVP_get_digestbyname("md_gost94");
if(!md) {
ldns_rr_free(ds);
return NULL;
}
digest = LDNS_XMALLOC(uint8_t, EVP_MD_size(md));
if (!digest) {
ldns_rr_free(ds);
return NULL;
}
break;
#else

ldns_rr_free(ds);
return NULL;
#endif
case LDNS_SHA384:
#if defined(USE_ECDSA)
digest = LDNS_XMALLOC(uint8_t, SHA384_DIGEST_LENGTH);
if (!digest) {
ldns_rr_free(ds);
return NULL;
}
break;
#else

ldns_rr_free(ds);
return NULL;
#endif
}

data_buf = ldns_buffer_new(LDNS_MAX_PACKETLEN);
if (!data_buf) {
LDNS_FREE(digest);
ldns_rr_free(ds);
return NULL;
}


keytag = htons(ldns_calc_keytag((ldns_rr*)key));
tmp = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_INT16,
sizeof(uint16_t),
&keytag);
ldns_rr_push_rdf(ds, tmp);


if ((tmp = ldns_rr_rdf(key, 2)) == NULL) {
LDNS_FREE(digest);
ldns_buffer_free(data_buf);
ldns_rr_free(ds);
return NULL;
} else {
ldns_rr_push_rdf(ds, ldns_rdf_clone( tmp ));
}


sha1hash = (uint8_t)h;
tmp = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_INT8,
sizeof(uint8_t),
&sha1hash);
ldns_rr_push_rdf(ds, tmp);



tmp = ldns_rdf_clone(ldns_rr_owner(key));
ldns_dname2canonical(tmp);
if (ldns_rdf2buffer_wire(data_buf, tmp) != LDNS_STATUS_OK) {
LDNS_FREE(digest);
ldns_buffer_free(data_buf);
ldns_rr_free(ds);
ldns_rdf_deep_free(tmp);
return NULL;
}
ldns_rdf_deep_free(tmp);


if (ldns_rr_rdata2buffer_wire(data_buf,
(ldns_rr*)key) != LDNS_STATUS_OK) {
LDNS_FREE(digest);
ldns_buffer_free(data_buf);
ldns_rr_free(ds);
return NULL;
}
switch(h) {
case LDNS_SHA1:
(void) ldns_sha1((unsigned char *) ldns_buffer_begin(data_buf),
(unsigned int) ldns_buffer_position(data_buf),
(unsigned char *) digest);

tmp = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_HEX,
LDNS_SHA1_DIGEST_LENGTH,
digest);
ldns_rr_push_rdf(ds, tmp);

break;
case LDNS_SHA256:
(void) ldns_sha256((unsigned char *) ldns_buffer_begin(data_buf),
(unsigned int) ldns_buffer_position(data_buf),
(unsigned char *) digest);
tmp = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_HEX,
LDNS_SHA256_DIGEST_LENGTH,
digest);
ldns_rr_push_rdf(ds, tmp);
break;
case LDNS_HASH_GOST:
#if defined(USE_GOST)
if(!ldns_digest_evp((unsigned char *) ldns_buffer_begin(data_buf),
(unsigned int) ldns_buffer_position(data_buf),
(unsigned char *) digest, md)) {
LDNS_FREE(digest);
ldns_buffer_free(data_buf);
ldns_rr_free(ds);
return NULL;
}
tmp = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_HEX,
(size_t)EVP_MD_size(md),
digest);
ldns_rr_push_rdf(ds, tmp);
#endif
break;
case LDNS_SHA384:
#if defined(USE_ECDSA)
(void) SHA384((unsigned char *) ldns_buffer_begin(data_buf),
(unsigned int) ldns_buffer_position(data_buf),
(unsigned char *) digest);
tmp = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_HEX,
SHA384_DIGEST_LENGTH,
digest);
ldns_rr_push_rdf(ds, tmp);
#endif
break;
}

LDNS_FREE(digest);
ldns_buffer_free(data_buf);
return ds;
}




























ldns_rdf *
ldns_dnssec_create_nsec_bitmap(ldns_rr_type rr_type_list[],
size_t size,
ldns_rr_type nsec_type)
{
uint8_t window;
uint8_t subtype;
int windows[256];
uint8_t windowpresent[256];
ldns_rr_type* d;
size_t i;

size_t sz;
uint8_t* data = NULL;
uint8_t* dptr;
ldns_rdf* rdf;

if (nsec_type != LDNS_RR_TYPE_NSEC &&
nsec_type != LDNS_RR_TYPE_NSEC3) {
return NULL;
}
memset(windows, 0, sizeof(int)*256);
memset(windowpresent, 0, 256);



for (d = rr_type_list; d < rr_type_list + size; d++) {
window = *d >> 8;
subtype = *d & 0xff;
windowpresent[window] = 1;
if (windows[window] < (int)subtype) {
windows[window] = (int)subtype;
}
}



sz = 0;
for (i = 0; i < 256; i++) {
if (windowpresent[i]) {
sz += windows[i] / 8 + 3;
}
}
if (sz > 0) {


dptr = data = LDNS_CALLOC(uint8_t, sz);
if (!data) {
return NULL;
}
for (i = 0; i < 256; i++) {
if (windowpresent[i]) {
*dptr++ = (uint8_t)i;
*dptr++ = (uint8_t)(windows[i] / 8 + 1);




windows[i] = (int)(dptr - data);

dptr += dptr[-1];
}
}
}



for (d = rr_type_list; d < rr_type_list + size; d++) {
subtype = *d & 0xff;
data[windows[*d >> 8] + subtype/8] |= (0x80 >> (subtype % 8));
}



rdf = ldns_rdf_new(LDNS_RDF_TYPE_BITMAP, sz, data);
if (!rdf) {
LDNS_FREE(data);
return NULL;
}
return rdf;
}

int
ldns_dnssec_rrsets_contains_type(const ldns_dnssec_rrsets *rrsets,
ldns_rr_type type)
{
const ldns_dnssec_rrsets *cur_rrset = rrsets;
while (cur_rrset) {
if (cur_rrset->type == type) {
return 1;
}
cur_rrset = cur_rrset->next;
}
return 0;
}

ldns_rr *
ldns_dnssec_create_nsec(const ldns_dnssec_name *from,
const ldns_dnssec_name *to,
ldns_rr_type nsec_type)
{
ldns_rr *nsec_rr;
ldns_rr_type types[65536];
size_t type_count = 0;
ldns_dnssec_rrsets *cur_rrsets;
int on_delegation_point;

if (!from || !to || (nsec_type != LDNS_RR_TYPE_NSEC)) {
return NULL;
}

nsec_rr = ldns_rr_new();
ldns_rr_set_type(nsec_rr, nsec_type);
ldns_rr_set_owner(nsec_rr, ldns_rdf_clone(ldns_dnssec_name_name(from)));
ldns_rr_push_rdf(nsec_rr, ldns_rdf_clone(ldns_dnssec_name_name(to)));

on_delegation_point = ldns_dnssec_rrsets_contains_type(
from->rrsets, LDNS_RR_TYPE_NS)
&& !ldns_dnssec_rrsets_contains_type(
from->rrsets, LDNS_RR_TYPE_SOA);

cur_rrsets = from->rrsets;
while (cur_rrsets) {


if ((on_delegation_point && (
cur_rrsets->type == LDNS_RR_TYPE_NS
|| cur_rrsets->type == LDNS_RR_TYPE_DS))
|| (!on_delegation_point &&
cur_rrsets->type != LDNS_RR_TYPE_RRSIG
&& cur_rrsets->type != LDNS_RR_TYPE_NSEC)) {

types[type_count] = cur_rrsets->type;
type_count++;
}
cur_rrsets = cur_rrsets->next;

}
types[type_count] = LDNS_RR_TYPE_RRSIG;
type_count++;
types[type_count] = LDNS_RR_TYPE_NSEC;
type_count++;

ldns_rr_push_rdf(nsec_rr, ldns_dnssec_create_nsec_bitmap(types,
type_count,
nsec_type));

return nsec_rr;
}

ldns_rr *
ldns_dnssec_create_nsec3(const ldns_dnssec_name *from,
const ldns_dnssec_name *to,
const ldns_rdf *zone_name,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
const uint8_t *salt)
{
ldns_rr *nsec_rr;
ldns_rr_type types[65536];
size_t type_count = 0;
ldns_dnssec_rrsets *cur_rrsets;
ldns_status status;
int on_delegation_point;

if (!from) {
return NULL;
}

nsec_rr = ldns_rr_new_frm_type(LDNS_RR_TYPE_NSEC3);
ldns_rr_set_owner(nsec_rr,
ldns_nsec3_hash_name(ldns_dnssec_name_name(from),
algorithm,
iterations,
salt_length,
salt));
status = ldns_dname_cat(ldns_rr_owner(nsec_rr), zone_name);
if(status != LDNS_STATUS_OK) {
ldns_rr_free(nsec_rr);
return NULL;
}
ldns_nsec3_add_param_rdfs(nsec_rr,
algorithm,
flags,
iterations,
salt_length,
salt);

on_delegation_point = ldns_dnssec_rrsets_contains_type(
from->rrsets, LDNS_RR_TYPE_NS)
&& !ldns_dnssec_rrsets_contains_type(
from->rrsets, LDNS_RR_TYPE_SOA);
cur_rrsets = from->rrsets;
while (cur_rrsets) {






if ((on_delegation_point && (
cur_rrsets->type == LDNS_RR_TYPE_NS
|| cur_rrsets->type == LDNS_RR_TYPE_DS))
|| (!on_delegation_point &&
cur_rrsets->type != LDNS_RR_TYPE_RRSIG)) {

types[type_count] = cur_rrsets->type;
type_count++;
}
cur_rrsets = cur_rrsets->next;
}



if (type_count > 0 &&
!(type_count == 1 && types[0] == LDNS_RR_TYPE_NS)) {
types[type_count] = LDNS_RR_TYPE_RRSIG;
type_count++;
}


if (to && to->hashed_name) {
(void) ldns_rr_set_rdf(nsec_rr,
ldns_rdf_clone(to->hashed_name),
4);
} else {
(void) ldns_rr_set_rdf(nsec_rr, NULL, 4);
}

ldns_rr_push_rdf(nsec_rr,
ldns_dnssec_create_nsec_bitmap(types,
type_count,
LDNS_RR_TYPE_NSEC3));

return nsec_rr;
}

ldns_rr *
ldns_create_nsec(ldns_rdf *cur_owner, ldns_rdf *next_owner, ldns_rr_list *rrs)
{








uint16_t i;
ldns_rr *i_rr;
uint16_t i_type;

ldns_rr *nsec = NULL;
ldns_rr_type i_type_list[65536];
size_t type_count = 0;

nsec = ldns_rr_new();
ldns_rr_set_type(nsec, LDNS_RR_TYPE_NSEC);
ldns_rr_set_owner(nsec, ldns_rdf_clone(cur_owner));
ldns_rr_push_rdf(nsec, ldns_rdf_clone(next_owner));

for (i = 0; i < ldns_rr_list_rr_count(rrs); i++) {
i_rr = ldns_rr_list_rr(rrs, i);
if (ldns_rdf_compare(cur_owner,
ldns_rr_owner(i_rr)) == 0) {
i_type = ldns_rr_get_type(i_rr);
if (i_type != LDNS_RR_TYPE_RRSIG && i_type != LDNS_RR_TYPE_NSEC) {
if (type_count == 0 || i_type_list[type_count-1] != i_type) {
i_type_list[type_count] = i_type;
type_count++;
}
}
}
}

i_type_list[type_count] = LDNS_RR_TYPE_RRSIG;
type_count++;
i_type_list[type_count] = LDNS_RR_TYPE_NSEC;
type_count++;

ldns_rr_push_rdf(nsec,
ldns_dnssec_create_nsec_bitmap(i_type_list,
type_count, LDNS_RR_TYPE_NSEC));

return nsec;
}

ldns_rdf *
ldns_nsec3_hash_name(const ldns_rdf *name,
uint8_t algorithm,
uint16_t iterations,
uint8_t salt_length,
const uint8_t *salt)
{
size_t hashed_owner_str_len;
ldns_rdf *cann;
ldns_rdf *hashed_owner;
unsigned char *hashed_owner_str;
char *hashed_owner_b32;
size_t hashed_owner_b32_len;
uint32_t cur_it;


unsigned char hash[LDNS_SHA1_DIGEST_LENGTH];
ldns_status status;


if (algorithm != LDNS_SHA1) {
return NULL;
}


cann = ldns_rdf_clone(name);
if(!cann) {
#if defined(STDERR_MSGS)
fprintf(stderr, "Memory error\n");
#endif
return NULL;
}
ldns_dname2canonical(cann);

hashed_owner_str_len = salt_length + ldns_rdf_size(cann);
hashed_owner_str = LDNS_XMALLOC(unsigned char, hashed_owner_str_len);
if(!hashed_owner_str) {
ldns_rdf_deep_free(cann);
return NULL;
}
memcpy(hashed_owner_str, ldns_rdf_data(cann), ldns_rdf_size(cann));
memcpy(hashed_owner_str + ldns_rdf_size(cann), salt, salt_length);
ldns_rdf_deep_free(cann);

for (cur_it = iterations + 1; cur_it > 0; cur_it--) {
(void) ldns_sha1((unsigned char *) hashed_owner_str,
(unsigned int) hashed_owner_str_len, hash);

LDNS_FREE(hashed_owner_str);
hashed_owner_str_len = salt_length + LDNS_SHA1_DIGEST_LENGTH;
hashed_owner_str = LDNS_XMALLOC(unsigned char, hashed_owner_str_len);
if (!hashed_owner_str) {
return NULL;
}
memcpy(hashed_owner_str, hash, LDNS_SHA1_DIGEST_LENGTH);
memcpy(hashed_owner_str + LDNS_SHA1_DIGEST_LENGTH, salt, salt_length);
hashed_owner_str_len = LDNS_SHA1_DIGEST_LENGTH + salt_length;
}

LDNS_FREE(hashed_owner_str);
hashed_owner_str = hash;
hashed_owner_str_len = LDNS_SHA1_DIGEST_LENGTH;

hashed_owner_b32 = LDNS_XMALLOC(char,
ldns_b32_ntop_calculate_size(hashed_owner_str_len) + 1);
if(!hashed_owner_b32) {
return NULL;
}
hashed_owner_b32_len = (size_t) ldns_b32_ntop_extended_hex(
(uint8_t *) hashed_owner_str,
hashed_owner_str_len,
hashed_owner_b32,
ldns_b32_ntop_calculate_size(hashed_owner_str_len)+1);
if (hashed_owner_b32_len < 1) {
#if defined(STDERR_MSGS)
fprintf(stderr, "Error in base32 extended hex encoding ");
fprintf(stderr, "of hashed owner name (name: ");
ldns_rdf_print(stderr, name);
fprintf(stderr, ", return code: %u)\n",
(unsigned int) hashed_owner_b32_len);
#endif
LDNS_FREE(hashed_owner_b32);
return NULL;
}
hashed_owner_b32[hashed_owner_b32_len] = '\0';

status = ldns_str2rdf_dname(&hashed_owner, hashed_owner_b32);
if (status != LDNS_STATUS_OK) {
#if defined(STDERR_MSGS)
fprintf(stderr, "Error creating rdf from %s\n", hashed_owner_b32);
#endif
LDNS_FREE(hashed_owner_b32);
return NULL;
}

LDNS_FREE(hashed_owner_b32);
return hashed_owner;
}

void
ldns_nsec3_add_param_rdfs(ldns_rr *rr,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
const uint8_t *salt)
{
ldns_rdf *salt_rdf = NULL;
uint8_t *salt_data = NULL;
ldns_rdf *old;

old = ldns_rr_set_rdf(rr,
ldns_rdf_new_frm_data(LDNS_RDF_TYPE_INT8,
1, (void*)&algorithm),
0);
if (old) ldns_rdf_deep_free(old);

old = ldns_rr_set_rdf(rr,
ldns_rdf_new_frm_data(LDNS_RDF_TYPE_INT8,
1, (void*)&flags),
1);
if (old) ldns_rdf_deep_free(old);

old = ldns_rr_set_rdf(rr,
ldns_native2rdf_int16(LDNS_RDF_TYPE_INT16,
iterations),
2);
if (old) ldns_rdf_deep_free(old);

salt_data = LDNS_XMALLOC(uint8_t, salt_length + 1);
if(!salt_data) {

return;
}
salt_data[0] = salt_length;
memcpy(salt_data + 1, salt, salt_length);
salt_rdf = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_NSEC3_SALT,
salt_length + 1,
salt_data);
if(!salt_rdf) {
LDNS_FREE(salt_data);

return;
}

old = ldns_rr_set_rdf(rr, salt_rdf, 3);
if (old) ldns_rdf_deep_free(old);
LDNS_FREE(salt_data);
}

static int
rr_list_delegation_only(const ldns_rdf *origin, const ldns_rr_list *rr_list)
{
size_t i;
ldns_rr *cur_rr;
if (!origin || !rr_list) return 0;
for (i = 0; i < ldns_rr_list_rr_count(rr_list); i++) {
cur_rr = ldns_rr_list_rr(rr_list, i);
if (ldns_dname_compare(ldns_rr_owner(cur_rr), origin) == 0) {
return 0;
}
if (ldns_rr_get_type(cur_rr) != LDNS_RR_TYPE_NS) {
return 0;
}
}
return 1;
}



ldns_rr *
ldns_create_nsec3(const ldns_rdf *cur_owner,
const ldns_rdf *cur_zone,
const ldns_rr_list *rrs,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
const uint8_t *salt,
bool emptynonterminal)
{
size_t i;
ldns_rr *i_rr;
uint16_t i_type;

ldns_rr *nsec = NULL;
ldns_rdf *hashed_owner = NULL;

ldns_status status;

ldns_rr_type i_type_list[1024];
size_t type_count = 0;

hashed_owner = ldns_nsec3_hash_name(cur_owner,
algorithm,
iterations,
salt_length,
salt);
status = ldns_dname_cat(hashed_owner, cur_zone);
if(status != LDNS_STATUS_OK) {
ldns_rdf_deep_free(hashed_owner);
return NULL;
}
nsec = ldns_rr_new_frm_type(LDNS_RR_TYPE_NSEC3);
if(!nsec) {
ldns_rdf_deep_free(hashed_owner);
return NULL;
}
ldns_rr_set_type(nsec, LDNS_RR_TYPE_NSEC3);
ldns_rr_set_owner(nsec, hashed_owner);

ldns_nsec3_add_param_rdfs(nsec,
algorithm,
flags,
iterations,
salt_length,
salt);
(void) ldns_rr_set_rdf(nsec, NULL, 4);


for (i = 0; i < ldns_rr_list_rr_count(rrs); i++) {
i_rr = ldns_rr_list_rr(rrs, i);
if (ldns_rdf_compare(cur_owner,
ldns_rr_owner(i_rr)) == 0) {
i_type = ldns_rr_get_type(i_rr);
if (type_count == 0 || i_type_list[type_count-1] != i_type) {
i_type_list[type_count] = i_type;
type_count++;
}
}
}



if (!emptynonterminal && !rr_list_delegation_only(cur_zone, rrs)) {
i_type_list[type_count] = LDNS_RR_TYPE_RRSIG;
type_count++;
}


if (ldns_dname_compare(cur_zone, cur_owner) == 0) {
i_type_list[type_count] = LDNS_RR_TYPE_SOA;
type_count++;
}

ldns_rr_push_rdf(nsec,
ldns_dnssec_create_nsec_bitmap(i_type_list,
type_count, LDNS_RR_TYPE_NSEC3));

return nsec;
}

uint8_t
ldns_nsec3_algorithm(const ldns_rr *nsec3_rr)
{
if (nsec3_rr &&
(ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3 ||
ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3PARAM)
&& (ldns_rr_rdf(nsec3_rr, 0) != NULL)
&& ldns_rdf_size(ldns_rr_rdf(nsec3_rr, 0)) > 0) {
return ldns_rdf2native_int8(ldns_rr_rdf(nsec3_rr, 0));
}
return 0;
}

uint8_t
ldns_nsec3_flags(const ldns_rr *nsec3_rr)
{
if (nsec3_rr &&
(ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3 ||
ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3PARAM)
&& (ldns_rr_rdf(nsec3_rr, 1) != NULL)
&& ldns_rdf_size(ldns_rr_rdf(nsec3_rr, 1)) > 0) {
return ldns_rdf2native_int8(ldns_rr_rdf(nsec3_rr, 1));
}
return 0;
}

bool
ldns_nsec3_optout(const ldns_rr *nsec3_rr)
{
return (ldns_nsec3_flags(nsec3_rr) & LDNS_NSEC3_VARS_OPTOUT_MASK);
}

uint16_t
ldns_nsec3_iterations(const ldns_rr *nsec3_rr)
{
if (nsec3_rr &&
(ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3 ||
ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3PARAM)
&& (ldns_rr_rdf(nsec3_rr, 2) != NULL)
&& ldns_rdf_size(ldns_rr_rdf(nsec3_rr, 2)) > 0) {
return ldns_rdf2native_int16(ldns_rr_rdf(nsec3_rr, 2));
}
return 0;

}

ldns_rdf *
ldns_nsec3_salt(const ldns_rr *nsec3_rr)
{
if (nsec3_rr &&
(ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3 ||
ldns_rr_get_type(nsec3_rr) == LDNS_RR_TYPE_NSEC3PARAM)
) {
return ldns_rr_rdf(nsec3_rr, 3);
}
return NULL;
}

uint8_t
ldns_nsec3_salt_length(const ldns_rr *nsec3_rr)
{
ldns_rdf *salt_rdf = ldns_nsec3_salt(nsec3_rr);
if (salt_rdf && ldns_rdf_size(salt_rdf) > 0) {
return (uint8_t) ldns_rdf_data(salt_rdf)[0];
}
return 0;
}


uint8_t *
ldns_nsec3_salt_data(const ldns_rr *nsec3_rr)
{
uint8_t salt_length;
uint8_t *salt;

ldns_rdf *salt_rdf = ldns_nsec3_salt(nsec3_rr);
if (salt_rdf && ldns_rdf_size(salt_rdf) > 0) {
salt_length = ldns_rdf_data(salt_rdf)[0];
salt = LDNS_XMALLOC(uint8_t, salt_length);
if(!salt) return NULL;
memcpy(salt, &ldns_rdf_data(salt_rdf)[1], salt_length);
return salt;
}
return NULL;
}

ldns_rdf *
ldns_nsec3_next_owner(const ldns_rr *nsec3_rr)
{
if (!nsec3_rr || ldns_rr_get_type(nsec3_rr) != LDNS_RR_TYPE_NSEC3) {
return NULL;
} else {
return ldns_rr_rdf(nsec3_rr, 4);
}
}

ldns_rdf *
ldns_nsec3_bitmap(const ldns_rr *nsec3_rr)
{
if (!nsec3_rr || ldns_rr_get_type(nsec3_rr) != LDNS_RR_TYPE_NSEC3) {
return NULL;
} else {
return ldns_rr_rdf(nsec3_rr, 5);
}
}

ldns_rdf *
ldns_nsec3_hash_name_frm_nsec3(const ldns_rr *nsec, const ldns_rdf *name)
{
uint8_t algorithm;
uint16_t iterations;
uint8_t salt_length;
uint8_t *salt = 0;

ldns_rdf *hashed_owner;

algorithm = ldns_nsec3_algorithm(nsec);
salt_length = ldns_nsec3_salt_length(nsec);
salt = ldns_nsec3_salt_data(nsec);
iterations = ldns_nsec3_iterations(nsec);

hashed_owner = ldns_nsec3_hash_name(name,
algorithm,
iterations,
salt_length,
salt);

LDNS_FREE(salt);
return hashed_owner;
}

bool
ldns_nsec_bitmap_covers_type(const ldns_rdf* bitmap, ldns_rr_type type)
{
uint8_t* dptr;
uint8_t* dend;






uint8_t window = type >> 8;
uint8_t subtype = type & 0xff;

if (! bitmap) {
return false;
}
assert(ldns_rdf_get_type(bitmap) == LDNS_RDF_TYPE_BITMAP);

dptr = ldns_rdf_data(bitmap);
dend = ldns_rdf_data(bitmap) + ldns_rdf_size(bitmap);




while (dptr < dend && dptr[0] <= window) {

if (dptr[0] == window && subtype / 8 < dptr[1] &&
dptr + dptr[1] + 2 <= dend) {

return dptr[2 + subtype / 8] & (0x80 >> (subtype % 8));
}
dptr += dptr[1] + 2;
}
return false;
}

ldns_status
ldns_nsec_bitmap_set_type(ldns_rdf* bitmap, ldns_rr_type type)
{
uint8_t* dptr;
uint8_t* dend;






uint8_t window = type >> 8;
uint8_t subtype = type & 0xff;

if (! bitmap) {
return false;
}
assert(ldns_rdf_get_type(bitmap) == LDNS_RDF_TYPE_BITMAP);

dptr = ldns_rdf_data(bitmap);
dend = ldns_rdf_data(bitmap) + ldns_rdf_size(bitmap);




while (dptr < dend && dptr[0] <= window) {

if (dptr[0] == window && subtype / 8 < dptr[1] &&
dptr + dptr[1] + 2 <= dend) {

dptr[2 + subtype / 8] |= (0x80 >> (subtype % 8));
return LDNS_STATUS_OK;
}
dptr += dptr[1] + 2;
}
return LDNS_STATUS_TYPE_NOT_IN_BITMAP;
}

ldns_status
ldns_nsec_bitmap_clear_type(ldns_rdf* bitmap, ldns_rr_type type)
{
uint8_t* dptr;
uint8_t* dend;






uint8_t window = type >> 8;
uint8_t subtype = type & 0xff;

if (! bitmap) {
return false;
}

assert(ldns_rdf_get_type(bitmap) == LDNS_RDF_TYPE_BITMAP);

dptr = ldns_rdf_data(bitmap);
dend = ldns_rdf_data(bitmap) + ldns_rdf_size(bitmap);




while (dptr < dend && dptr[0] <= window) {

if (dptr[0] == window && subtype / 8 < dptr[1] &&
dptr + dptr[1] + 2 <= dend) {

dptr[2 + subtype / 8] &= ~(0x80 >> (subtype % 8));
return LDNS_STATUS_OK;
}
dptr += dptr[1] + 2;
}
return LDNS_STATUS_TYPE_NOT_IN_BITMAP;
}


bool
ldns_nsec_covers_name(const ldns_rr *nsec, const ldns_rdf *name)
{
ldns_rdf *nsec_owner = ldns_rr_owner(nsec);
ldns_rdf *hash_next;
char *next_hash_str;
ldns_rdf *nsec_next = NULL;
ldns_status status;
ldns_rdf *chopped_dname;
bool result;

if (ldns_rr_get_type(nsec) == LDNS_RR_TYPE_NSEC) {
if (ldns_rr_rdf(nsec, 0) != NULL) {
nsec_next = ldns_rdf_clone(ldns_rr_rdf(nsec, 0));
} else {
return false;
}
} else if (ldns_rr_get_type(nsec) == LDNS_RR_TYPE_NSEC3) {
hash_next = ldns_nsec3_next_owner(nsec);
next_hash_str = ldns_rdf2str(hash_next);
nsec_next = ldns_dname_new_frm_str(next_hash_str);
LDNS_FREE(next_hash_str);
chopped_dname = ldns_dname_left_chop(nsec_owner);
status = ldns_dname_cat(nsec_next, chopped_dname);
ldns_rdf_deep_free(chopped_dname);
if (status != LDNS_STATUS_OK) {
printf("error catting: %s\n", ldns_get_errorstr_by_id(status));
}
} else {
ldns_rdf_deep_free(nsec_next);
return false;
}


if(ldns_dname_compare(nsec_owner, nsec_next) > 0) {
result = (ldns_dname_compare(nsec_owner, name) <= 0 ||
ldns_dname_compare(name, nsec_next) < 0);
} else if(ldns_dname_compare(nsec_owner, nsec_next) < 0) {
result = (ldns_dname_compare(nsec_owner, name) <= 0 &&
ldns_dname_compare(name, nsec_next) < 0);
} else {
result = true;
}

ldns_rdf_deep_free(nsec_next);
return result;
}

#if defined(HAVE_SSL)


ldns_status
ldns_pkt_verify_time(const ldns_pkt *p, ldns_rr_type t, const ldns_rdf *o,
const ldns_rr_list *k, const ldns_rr_list *s,
time_t check_time, ldns_rr_list *good_keys)
{
ldns_rr_list *rrset;
ldns_rr_list *sigs;
ldns_rr_list *sigs_covered;
ldns_rdf *rdf_t;
ldns_rr_type t_netorder;

if (!k) {
return LDNS_STATUS_ERR;

}

if (t == LDNS_RR_TYPE_RRSIG) {

return LDNS_STATUS_ERR;
}

if (s) {

sigs = (ldns_rr_list *)s;
} else {

sigs = ldns_pkt_rr_list_by_name_and_type(p, o,
LDNS_RR_TYPE_RRSIG,
LDNS_SECTION_ANY_NOQUESTION);
if (!sigs) {

return LDNS_STATUS_ERR;

}
}




t_netorder = htons(t);

rdf_t = ldns_rdf_new(LDNS_RDF_TYPE_TYPE, 2, &t_netorder);

sigs_covered = ldns_rr_list_subtype_by_rdf(sigs, rdf_t, 0);
ldns_rdf_free(rdf_t);
if (! sigs_covered) {
if (! s) {
ldns_rr_list_deep_free(sigs);
}
return LDNS_STATUS_ERR;
}
ldns_rr_list_deep_free(sigs_covered);

rrset = ldns_pkt_rr_list_by_name_and_type(p, o, t,
LDNS_SECTION_ANY_NOQUESTION);
if (!rrset) {
if (! s) {
ldns_rr_list_deep_free(sigs);
}
return LDNS_STATUS_ERR;
}
return ldns_verify_time(rrset, sigs, k, check_time, good_keys);
}

ldns_status
ldns_pkt_verify(const ldns_pkt *p, ldns_rr_type t, const ldns_rdf *o,
const ldns_rr_list *k, const ldns_rr_list *s, ldns_rr_list *good_keys)
{
return ldns_pkt_verify_time(p, t, o, k, s, ldns_time(NULL), good_keys);
}
#endif

ldns_status
ldns_dnssec_chain_nsec3_list(ldns_rr_list *nsec3_rrs)
{
size_t i;
char *next_nsec_owner_str;
ldns_rdf *next_nsec_owner_label;
ldns_rdf *next_nsec_rdf;
ldns_status status = LDNS_STATUS_OK;

for (i = 0; i < ldns_rr_list_rr_count(nsec3_rrs); i++) {
if (i == ldns_rr_list_rr_count(nsec3_rrs) - 1) {
next_nsec_owner_label =
ldns_dname_label(ldns_rr_owner(ldns_rr_list_rr(nsec3_rrs,
0)), 0);
next_nsec_owner_str = ldns_rdf2str(next_nsec_owner_label);
if (next_nsec_owner_str[strlen(next_nsec_owner_str) - 1]
== '.') {
next_nsec_owner_str[strlen(next_nsec_owner_str) - 1]
= '\0';
}
status = ldns_str2rdf_b32_ext(&next_nsec_rdf,
next_nsec_owner_str);
if (!ldns_rr_set_rdf(ldns_rr_list_rr(nsec3_rrs, i),
next_nsec_rdf, 4)) {

}

ldns_rdf_deep_free(next_nsec_owner_label);
LDNS_FREE(next_nsec_owner_str);
} else {
next_nsec_owner_label =
ldns_dname_label(ldns_rr_owner(ldns_rr_list_rr(nsec3_rrs,
i + 1)),
0);
next_nsec_owner_str = ldns_rdf2str(next_nsec_owner_label);
if (next_nsec_owner_str[strlen(next_nsec_owner_str) - 1]
== '.') {
next_nsec_owner_str[strlen(next_nsec_owner_str) - 1]
= '\0';
}
status = ldns_str2rdf_b32_ext(&next_nsec_rdf,
next_nsec_owner_str);
ldns_rdf_deep_free(next_nsec_owner_label);
LDNS_FREE(next_nsec_owner_str);
if (!ldns_rr_set_rdf(ldns_rr_list_rr(nsec3_rrs, i),
next_nsec_rdf, 4)) {

}
}
}
return status;
}

int
qsort_rr_compare_nsec3(const void *a, const void *b)
{
const ldns_rr *rr1 = * (const ldns_rr **) a;
const ldns_rr *rr2 = * (const ldns_rr **) b;
if (rr1 == NULL && rr2 == NULL) {
return 0;
}
if (rr1 == NULL) {
return -1;
}
if (rr2 == NULL) {
return 1;
}
return ldns_rdf_compare(ldns_rr_owner(rr1), ldns_rr_owner(rr2));
}

void
ldns_rr_list_sort_nsec3(ldns_rr_list *unsorted)
{
qsort(unsorted->_rrs,
ldns_rr_list_rr_count(unsorted),
sizeof(ldns_rr *),
qsort_rr_compare_nsec3);
}

int
ldns_dnssec_default_add_to_signatures( ATTR_UNUSED(ldns_rr *sig)
, ATTR_UNUSED(void *n)
)
{
return LDNS_SIGNATURE_LEAVE_ADD_NEW;
}

int
ldns_dnssec_default_leave_signatures( ATTR_UNUSED(ldns_rr *sig)
, ATTR_UNUSED(void *n)
)
{
return LDNS_SIGNATURE_LEAVE_NO_ADD;
}

int
ldns_dnssec_default_delete_signatures( ATTR_UNUSED(ldns_rr *sig)
, ATTR_UNUSED(void *n)
)
{
return LDNS_SIGNATURE_REMOVE_NO_ADD;
}

int
ldns_dnssec_default_replace_signatures( ATTR_UNUSED(ldns_rr *sig)
, ATTR_UNUSED(void *n)
)
{
return LDNS_SIGNATURE_REMOVE_ADD_NEW;
}

#if defined(HAVE_SSL)
ldns_rdf *
ldns_convert_dsa_rrsig_asn12rdf(const ldns_buffer *sig,
const long sig_len)
{
#if defined(USE_DSA)
ldns_rdf *sigdata_rdf;
DSA_SIG *dsasig;
const BIGNUM *R, *S;
unsigned char *dsasig_data = (unsigned char*)ldns_buffer_begin(sig);
size_t byte_offset;

dsasig = d2i_DSA_SIG(NULL,
(const unsigned char **)&dsasig_data,
sig_len);
if (!dsasig) {
DSA_SIG_free(dsasig);
return NULL;
}

dsasig_data = LDNS_XMALLOC(unsigned char, 41);
if(!dsasig_data) {
DSA_SIG_free(dsasig);
return NULL;
}
dsasig_data[0] = 0;
#if defined(HAVE_DSA_SIG_GET0)
DSA_SIG_get0(dsasig, &R, &S);
#else
R = dsasig->r;
S = dsasig->s;
#endif
byte_offset = (size_t) (20 - BN_num_bytes(R));
if (byte_offset > 20) {
DSA_SIG_free(dsasig);
LDNS_FREE(dsasig_data);
return NULL;
}
memset(&dsasig_data[1], 0, byte_offset);
BN_bn2bin(R, &dsasig_data[1 + byte_offset]);
byte_offset = (size_t) (20 - BN_num_bytes(S));
if (byte_offset > 20) {
DSA_SIG_free(dsasig);
LDNS_FREE(dsasig_data);
return NULL;
}
memset(&dsasig_data[21], 0, byte_offset);
BN_bn2bin(S, &dsasig_data[21 + byte_offset]);

sigdata_rdf = ldns_rdf_new(LDNS_RDF_TYPE_B64, 41, dsasig_data);
if(!sigdata_rdf) {
LDNS_FREE(dsasig_data);
}
DSA_SIG_free(dsasig);

return sigdata_rdf;
#else
(void)sig; (void)sig_len;
return NULL;
#endif
}

ldns_status
ldns_convert_dsa_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf)
{
#if defined(USE_DSA)

BIGNUM *R, *S;
DSA_SIG *dsasig;
unsigned char *raw_sig = NULL;
int raw_sig_len;

if(ldns_rdf_size(sig_rdf) < 1 + 2*SHA_DIGEST_LENGTH)
return LDNS_STATUS_SYNTAX_RDATA_ERR;

R = BN_new();
if(!R) return LDNS_STATUS_MEM_ERR;
(void) BN_bin2bn((unsigned char *) ldns_rdf_data(sig_rdf) + 1,
SHA_DIGEST_LENGTH, R);
S = BN_new();
if(!S) {
BN_free(R);
return LDNS_STATUS_MEM_ERR;
}
(void) BN_bin2bn((unsigned char *) ldns_rdf_data(sig_rdf) + 21,
SHA_DIGEST_LENGTH, S);

dsasig = DSA_SIG_new();
if (!dsasig) {
BN_free(R);
BN_free(S);
return LDNS_STATUS_MEM_ERR;
}
#if defined(HAVE_DSA_SIG_SET0)
if (! DSA_SIG_set0(dsasig, R, S))
return LDNS_STATUS_SSL_ERR;
#else
dsasig->r = R;
dsasig->s = S;
#endif

raw_sig_len = i2d_DSA_SIG(dsasig, &raw_sig);
if (raw_sig_len < 0) {
DSA_SIG_free(dsasig);
free(raw_sig);
return LDNS_STATUS_SSL_ERR;
}
if (ldns_buffer_reserve(target_buffer, (size_t) raw_sig_len)) {
ldns_buffer_write(target_buffer, raw_sig, (size_t)raw_sig_len);
}

DSA_SIG_free(dsasig);
free(raw_sig);

return ldns_buffer_status(target_buffer);
#else
(void)target_buffer; (void)sig_rdf;
return LDNS_STATUS_CRYPTO_ALGO_NOT_IMPL;
#endif
}

#if defined(USE_ECDSA)
#if !defined(S_SPLINT_S)
ldns_rdf *
ldns_convert_ecdsa_rrsig_asn1len2rdf(const ldns_buffer *sig,
const long sig_len, int num_bytes)
{
ECDSA_SIG* ecdsa_sig;
const BIGNUM *r, *s;
unsigned char *data = (unsigned char*)ldns_buffer_begin(sig);
ldns_rdf* rdf;
ecdsa_sig = d2i_ECDSA_SIG(NULL, (const unsigned char **)&data, sig_len);
if(!ecdsa_sig) return NULL;

#if defined(HAVE_ECDSA_SIG_GET0)
ECDSA_SIG_get0(ecdsa_sig, &r, &s);
#else
r = ecdsa_sig->r;
s = ecdsa_sig->s;
#endif

if(BN_num_bytes(r) > num_bytes ||
BN_num_bytes(s) > num_bytes) {
ECDSA_SIG_free(ecdsa_sig);
return NULL;
}
data = LDNS_XMALLOC(unsigned char, num_bytes*2);
if(!data) {
ECDSA_SIG_free(ecdsa_sig);
return NULL;
}


memset(data, 0, num_bytes*2);
BN_bn2bin(r, data+num_bytes-BN_num_bytes(r));
BN_bn2bin(s, data+num_bytes*2-BN_num_bytes(s));
rdf = ldns_rdf_new(LDNS_RDF_TYPE_B64, (size_t)(num_bytes*2), data);
ECDSA_SIG_free(ecdsa_sig);
return rdf;
}

ldns_status
ldns_convert_ecdsa_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf)
{







uint8_t pre[] = {0x30, 0x44, 0x02, 0x20};
int pre_len = 4;
uint8_t mid[] = {0x02, 0x20};
int mid_len = 2;
int raw_sig_len, r_high, s_high, r_rem=0, s_rem=0;
long bnsize = (long)ldns_rdf_size(sig_rdf) / 2;
uint8_t* d = ldns_rdf_data(sig_rdf);

if(bnsize < 16 || (size_t)bnsize*2 != ldns_rdf_size(sig_rdf))
return LDNS_STATUS_ERR;

while(r_rem < bnsize-1 && d[r_rem] == 0)
r_rem++;

while(s_rem < bnsize-1 && d[bnsize+s_rem] == 0)
s_rem++;

r_high = ((d[0+r_rem]&0x80)?1:0);
s_high = ((d[bnsize+s_rem]&0x80)?1:0);
raw_sig_len = pre_len + r_high + bnsize - r_rem + mid_len +
s_high + bnsize - s_rem;
if(ldns_buffer_reserve(target_buffer, (size_t) raw_sig_len)) {
ldns_buffer_write_u8(target_buffer, pre[0]);
ldns_buffer_write_u8(target_buffer, raw_sig_len-2);
ldns_buffer_write_u8(target_buffer, pre[2]);
ldns_buffer_write_u8(target_buffer, bnsize + r_high - r_rem);
if(r_high)
ldns_buffer_write_u8(target_buffer, 0);
ldns_buffer_write(target_buffer, d+r_rem, bnsize-r_rem);
ldns_buffer_write(target_buffer, mid, mid_len-1);
ldns_buffer_write_u8(target_buffer, bnsize + s_high - s_rem);
if(s_high)
ldns_buffer_write_u8(target_buffer, 0);
ldns_buffer_write(target_buffer, d+bnsize+s_rem, bnsize-s_rem);
}
return ldns_buffer_status(target_buffer);
}

#endif
#endif

#if defined(USE_ED25519) || defined(USE_ED448)

static void print_hex(const char* str, uint8_t* d, int len)
{
const char hex[] = "0123456789abcdef";
int i;
printf("%s [len=%d]: ", str, len);
for(i=0; i<len; i++) {
int x = (d[i]&0xf0)>>4;
int y = (d[i]&0x0f);
printf("%c%c", hex[x], hex[y]);
}
printf("\n");
}
#endif

#if defined(USE_ED25519)
ldns_rdf *
ldns_convert_ed25519_rrsig_asn12rdf(const ldns_buffer *sig, long sig_len)
{
unsigned char *data = (unsigned char*)ldns_buffer_begin(sig);
ldns_rdf* rdf = NULL;


print_hex("sig in ASN", data, sig_len);

return rdf;
}

ldns_status
ldns_convert_ed25519_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf)
{


print_hex("sig raw", ldns_rdf_data(sig_rdf), ldns_rdf_size(sig_rdf));
return ldns_buffer_status(target_buffer);
}
#endif

#if defined(USE_ED448)
ldns_rdf *
ldns_convert_ed448_rrsig_asn12rdf(const ldns_buffer *sig, long sig_len)
{
unsigned char *data = (unsigned char*)ldns_buffer_begin(sig);
ldns_rdf* rdf = NULL;


print_hex("sig in ASN", data, sig_len);

return rdf;
}

ldns_status
ldns_convert_ed448_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf)
{


print_hex("sig raw", ldns_rdf_data(sig_rdf), ldns_rdf_size(sig_rdf));
return ldns_buffer_status(target_buffer);
}
#endif

#endif
