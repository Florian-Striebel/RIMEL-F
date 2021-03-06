































#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#if defined(HAVE_STDINT_H)
#include <stdint.h>
#elif defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#include <assert.h>

#include "support.h"
#include "asn1.h"

static void asn_error_func(const struct asn_buf *, const char *, ...);

void (*asn_error)(const struct asn_buf *, const char *, ...) = asn_error_func;







enum asn_err
asn_get_header(struct asn_buf *b, u_char *type, asn_len_t *len)
{
u_int length;

if (b->asn_len == 0) {
asn_error(b, "no identifier for header");
return (ASN_ERR_EOBUF);
}
*type = *b->asn_cptr;
if ((*type & ASN_TYPE_MASK) > 0x1e) {
asn_error(b, "tags > 0x1e not supported (%#x)",
*type & ASN_TYPE_MASK);
return (ASN_ERR_FAILED);
}
b->asn_cptr++;
b->asn_len--;
if (b->asn_len == 0) {
asn_error(b, "no length field");
return (ASN_ERR_EOBUF);
}
if (*b->asn_cptr & 0x80) {
length = *b->asn_cptr++ & 0x7f;
b->asn_len--;
if (length == 0) {
asn_error(b, "indefinite length not supported");
return (ASN_ERR_FAILED);
}
if (length > ASN_MAXLENLEN) {
asn_error(b, "long length too long (%u)", length);
return (ASN_ERR_FAILED);
}
if (length > b->asn_len) {
asn_error(b, "long length truncated");
return (ASN_ERR_EOBUF);
}
*len = 0;
while (length--) {
*len = (*len << 8) | *b->asn_cptr++;
b->asn_len--;
}
} else {
*len = *b->asn_cptr++;
b->asn_len--;
}

#if defined(BOGUS_CVE_2019_5610_FIX)






if (*len > b->asn_len) {
asn_error(b, "lenen %u exceeding asn_len %u", *len, b->asn_len);
return (ASN_ERR_EOBUF);
}
#endif
return (ASN_ERR_OK);
}






static u_int
asn_put_len(u_char *ptr, asn_len_t len)
{
u_int lenlen, lenlen1;
asn_len_t tmp;

if (len > ASN_MAXLEN) {
asn_error(NULL, "encoding length too long: (%u)", len);
return (0);
}

if (len <= 127) {
if (ptr)
*ptr++ = (u_char)len;
return (1);
} else {
lenlen = 0;

for (tmp = len; tmp != 0; tmp >>= 8)
lenlen++;
if (ptr != NULL) {
*ptr++ = (u_char)lenlen | 0x80;
lenlen1 = lenlen;
while (lenlen1-- > 0) {
ptr[lenlen1] = len & 0xff;
len >>= 8;
}
}
return (lenlen + 1);
}
}






enum asn_err
asn_put_header(struct asn_buf *b, u_char type, asn_len_t len)
{
u_int lenlen;


if ((type & ASN_TYPE_MASK) > 0x1e) {
asn_error(NULL, "types > 0x1e not supported (%#x)",
type & ASN_TYPE_MASK);
return (ASN_ERR_FAILED);
}
if (b->asn_len == 0)
return (ASN_ERR_EOBUF);

*b->asn_ptr++ = type;
b->asn_len--;


if ((lenlen = asn_put_len(NULL, len)) == 0)
return (ASN_ERR_FAILED);
if (b->asn_len < lenlen)
return (ASN_ERR_EOBUF);

(void)asn_put_len(b->asn_ptr, len);
b->asn_ptr += lenlen;
b->asn_len -= lenlen;
return (ASN_ERR_OK);
}









#define TEMP_LEN (1 + ASN_MAXLENLEN + 1)
enum asn_err
asn_put_temp_header(struct asn_buf *b, u_char type, u_char **ptr)
{
int ret;

if (b->asn_len < TEMP_LEN)
return (ASN_ERR_EOBUF);
*ptr = b->asn_ptr;
if ((ret = asn_put_header(b, type, ASN_MAXLEN)) == ASN_ERR_OK)
assert(b->asn_ptr == *ptr + TEMP_LEN);
return (ret);
}
enum asn_err
asn_commit_header(struct asn_buf *b, u_char *ptr, size_t *moved)
{
asn_len_t len;
u_int lenlen, shift;


len = b->asn_ptr - (ptr + TEMP_LEN);


lenlen = asn_put_len(ptr + 1, len);
if (lenlen > TEMP_LEN - 1)
return (ASN_ERR_FAILED);

if (lenlen < TEMP_LEN - 1) {

shift = (TEMP_LEN - 1) - lenlen;
memmove(ptr + 1 + lenlen, ptr + TEMP_LEN, len);
b->asn_ptr -= shift;
b->asn_len += shift;
if (moved != NULL)
*moved = shift;
}
return (ASN_ERR_OK);
}
#undef TEMP_LEN








static enum asn_err
asn_get_real_integer(struct asn_buf *b, asn_len_t len, int64_t *vp)
{
uint64_t val;
int neg = 0;
enum asn_err err;

if (b->asn_len < len) {
asn_error(b, "truncated integer");
return (ASN_ERR_EOBUF);
}
if (len == 0) {
asn_error(b, "zero-length integer");
*vp = 0;
return (ASN_ERR_BADLEN);
}
err = ASN_ERR_OK;
if (len > 8) {
asn_error(b, "integer too long");
err = ASN_ERR_RANGE;
} else if (len > 1 &&
((*b->asn_cptr == 0x00 && (b->asn_cptr[1] & 0x80) == 0) ||
(*b->asn_cptr == 0xff && (b->asn_cptr[1] & 0x80) == 0x80))) {
asn_error(b, "non-minimal integer");
err = ASN_ERR_BADLEN;
}

if (*b->asn_cptr & 0x80)
neg = 1;
val = 0;
while (len--) {
val <<= 8;
val |= neg ? (u_char)~*b->asn_cptr : *b->asn_cptr;
b->asn_len--;
b->asn_cptr++;
}
if (neg) {
*vp = -(int64_t)val - 1;
} else
*vp = (int64_t)val;
return (err);
}





static enum asn_err
asn_put_real_integer(struct asn_buf *b, u_char type, int64_t ival)
{
int i, neg = 0;
#define OCTETS 8
u_char buf[OCTETS];
uint64_t val;
enum asn_err ret;

if (ival < 0) {


val = (uint64_t)-(ival + 1);
neg = 1;
} else
val = (uint64_t)ival;


for (i = OCTETS - 1; i >= 0; i--) {
buf[i] = val & 0xff;
if (neg)
buf[i] = ~buf[i];
val >>= 8;
}

for (i = 0; i < OCTETS - 1; i++)
if (!((buf[i] == 0xff && (buf[i + 1] & 0x80) != 0) ||
(buf[i] == 0x00 && (buf[i + 1] & 0x80) == 0)))
break;
if ((ret = asn_put_header(b, type, OCTETS - i)))
return (ret);
if (OCTETS - (u_int)i > b->asn_len)
return (ASN_ERR_EOBUF);

while (i < OCTETS) {
*b->asn_ptr++ = buf[i++];
b->asn_len--;
}
return (ASN_ERR_OK);
#undef OCTETS
}







static enum asn_err
asn_get_real_unsigned(struct asn_buf *b, asn_len_t len, uint64_t *vp)
{
*vp = 0;
if (b->asn_len < len) {
asn_error(b, "truncated integer");
return (ASN_ERR_EOBUF);
}
if (len == 0) {

asn_error(b, "zero-length integer");
return (ASN_ERR_BADLEN);
}
if (len > 1 && *b->asn_cptr == 0x00 && (b->asn_cptr[1] & 0x80) == 0) {

asn_error(b, "non-minimal unsigned");
b->asn_cptr += len;
b->asn_len -= len;
return (ASN_ERR_BADLEN);

}

enum asn_err err = ASN_ERR_OK;

if ((*b->asn_cptr & 0x80) || len > 9 ||
(len == 9 && *b->asn_cptr != 0)) {

*vp = 0xffffffffffffffffULL;
asn_error(b, "unsigned too large or negative");
b->asn_cptr += len;
b->asn_len -= len;
return (ASN_ERR_RANGE);
}

while (len--) {
*vp = (*vp << 8) | *b->asn_cptr++;
b->asn_len--;
}
return (err);
}





static int
asn_put_real_unsigned(struct asn_buf *b, u_char type, uint64_t val)
{
int i;
#define OCTETS 9
u_char buf[OCTETS];
enum asn_err ret;


for (i = OCTETS - 1; i >= 0; i--) {
buf[i] = val & 0xff;
val >>= 8;
}

for (i = 0; i < OCTETS - 1; i++)
if (!(buf[i] == 0x00 && (buf[i + 1] & 0x80) == 0))
break;
if ((ret = asn_put_header(b, type, OCTETS - i)))
return (ret);
if (OCTETS - (u_int)i > b->asn_len)
return (ASN_ERR_EOBUF);

while (i < OCTETS) {
*b->asn_ptr++ = buf[i++];
b->asn_len--;
}
#undef OCTETS
return (ASN_ERR_OK);
}




enum asn_err
asn_get_integer_raw(struct asn_buf *b, asn_len_t len, int32_t *vp)
{
int64_t val;
enum asn_err ret;

if ((ret = asn_get_real_integer(b, len, &val)) == ASN_ERR_OK) {
if (len > 4) {
asn_error(b, "integer too long");
ret = ASN_ERR_BADLEN;
} else if (val > INT32_MAX || val < INT32_MIN) {

asn_error(b, "integer out of range");
ret = ASN_ERR_RANGE;
}
*vp = (int32_t)val;
}
return (ret);
}

enum asn_err
asn_get_integer(struct asn_buf *b, int32_t *vp)
{
asn_len_t len;
u_char type;
enum asn_err err;

if ((err = asn_get_header(b, &type, &len)) != ASN_ERR_OK)
return (err);
if (type != ASN_TYPE_INTEGER) {
asn_error(b, "bad type for integer (%u)", type);
return (ASN_ERR_TAG);
}

return (asn_get_integer_raw(b, len, vp));
}

enum asn_err
asn_put_integer(struct asn_buf *b, int32_t val)
{
return (asn_put_real_integer(b, ASN_TYPE_INTEGER, val));
}










enum asn_err
asn_get_octetstring_raw(struct asn_buf *b, asn_len_t len, u_char *octets,
u_int *noctets)
{
enum asn_err err = ASN_ERR_OK;

if (*noctets < len) {
asn_error(b, "octetstring truncated");
err = ASN_ERR_RANGE;
}
if (b->asn_len < len) {
asn_error(b, "truncatet octetstring");
return (ASN_ERR_EOBUF);
}
if (*noctets < len)
memcpy(octets, b->asn_cptr, *noctets);
else
memcpy(octets, b->asn_cptr, len);
*noctets = len;
b->asn_cptr += len;
b->asn_len -= len;
return (err);
}

enum asn_err
asn_get_octetstring(struct asn_buf *b, u_char *octets, u_int *noctets)
{
enum asn_err err;
u_char type;
asn_len_t len;

if ((err = asn_get_header(b, &type, &len)) != ASN_ERR_OK)
return (err);
if (type != ASN_TYPE_OCTETSTRING) {
asn_error(b, "bad type for octetstring (%u)", type);
return (ASN_ERR_TAG);
}
return (asn_get_octetstring_raw(b, len, octets, noctets));
}

enum asn_err
asn_put_octetstring(struct asn_buf *b, const u_char *octets, u_int noctets)
{
enum asn_err ret;

if ((ret = asn_put_header(b, ASN_TYPE_OCTETSTRING, noctets)) != ASN_ERR_OK)
return (ret);
if (b->asn_len < noctets)
return (ASN_ERR_EOBUF);

memcpy(b->asn_ptr, octets, noctets);
b->asn_ptr += noctets;
b->asn_len -= noctets;
return (ASN_ERR_OK);
}






enum asn_err
asn_get_null_raw(struct asn_buf *b, asn_len_t len)
{
if (len != 0) {
if (b->asn_len < len) {
asn_error(b, "truncated NULL");
return (ASN_ERR_EOBUF);
}
asn_error(b, "bad length for NULL (%u)", len);
b->asn_len -= len;
b->asn_ptr += len;
return (ASN_ERR_BADLEN);
}
return (ASN_ERR_OK);
}

enum asn_err
asn_get_null(struct asn_buf *b)
{
u_char type;
asn_len_t len;
enum asn_err err;

if ((err = asn_get_header(b, &type, &len)) != ASN_ERR_OK)
return (err);
if (type != ASN_TYPE_NULL) {
asn_error(b, "bad type for NULL (%u)", type);
return (ASN_ERR_TAG);
}
return (asn_get_null_raw(b, len));
}

enum asn_err
asn_put_null(struct asn_buf *b)
{
return (asn_put_header(b, ASN_TYPE_NULL, 0));
}

enum asn_err
asn_put_exception(struct asn_buf *b, u_int except)
{
return (asn_put_header(b, ASN_CLASS_CONTEXT | except, 0));
}






enum asn_err
asn_get_objid_raw(struct asn_buf *b, asn_len_t len, struct asn_oid *oid)
{
asn_subid_t subid;
enum asn_err err;

if (b->asn_len < len) {
asn_error(b, "truncated OBJID");
return (ASN_ERR_EOBUF);
}
oid->len = 0;
if (len == 0) {
asn_error(b, "short OBJID");
oid->subs[oid->len++] = 0;
oid->subs[oid->len++] = 0;
return (ASN_ERR_BADLEN);
}
err = ASN_ERR_OK;
while (len != 0) {
if (oid->len == ASN_MAXOIDLEN) {
asn_error(b, "OID too long (%u)", oid->len);
b->asn_cptr += len;
b->asn_len -= len;
return (ASN_ERR_BADLEN);
}
subid = 0;
do {
if (len == 0) {
asn_error(b, "unterminated subid");
return (ASN_ERR_EOBUF);
}
if (subid > (ASN_MAXID >> 7)) {
asn_error(b, "OID subid too larger");
err = ASN_ERR_RANGE;
}
subid = (subid << 7) | (*b->asn_cptr & 0x7f);
len--;
b->asn_len--;
} while (*b->asn_cptr++ & 0x80);
if (oid->len == 0) {
if (subid < 80) {
oid->subs[oid->len++] = subid / 40;
oid->subs[oid->len++] = subid % 40;
} else {
oid->subs[oid->len++] = 2;
oid->subs[oid->len++] = subid - 80;
}
} else {
oid->subs[oid->len++] = subid;
}
}
return (err);

}

enum asn_err
asn_get_objid(struct asn_buf *b, struct asn_oid *oid)
{
u_char type;
asn_len_t len;
enum asn_err err;

if ((err = asn_get_header(b, &type, &len)) != ASN_ERR_OK)
return (err);
if (type != ASN_TYPE_OBJID) {
asn_error(b, "bad type for OBJID (%u)", type);
return (ASN_ERR_TAG);
}
return (asn_get_objid_raw(b, len, oid));
}

enum asn_err
asn_put_objid(struct asn_buf *b, const struct asn_oid *oid)
{
asn_subid_t first, sub;
enum asn_err err, err1;
u_int i, oidlen;
asn_len_t len;

err = ASN_ERR_OK;
if (oid->len == 0) {

asn_error(NULL, "short oid");
err = ASN_ERR_RANGE;
first = 0;
oidlen = 2;
} else if (oid->len == 1) {

asn_error(NULL, "short oid");
if (oid->subs[0] > 2)
asn_error(NULL, "oid[0] too large (%u)", oid->subs[0]);
err = ASN_ERR_RANGE;
first = oid->subs[0] * 40;
oidlen = 2;
} else {
if (oid->len > ASN_MAXOIDLEN) {
asn_error(NULL, "oid too long %u", oid->len);
err = ASN_ERR_RANGE;
}
if (oid->subs[0] > 2 ||
(oid->subs[0] < 2 && oid->subs[1] >= 40) ||
(oid->subs[0] == 2 && oid->subs[1] > ASN_MAXID - 2 * 40)) {
asn_error(NULL, "oid out of range (%u,%u)",
oid->subs[0], oid->subs[1]);
err = ASN_ERR_RANGE;
}
first = 40 * oid->subs[0] + oid->subs[1];
oidlen = oid->len;
}
len = 0;
for (i = 1; i < oidlen; i++) {
sub = (i == 1) ? first : oid->subs[i];
if (sub > ASN_MAXID) {
asn_error(NULL, "oid subid too large");
err = ASN_ERR_RANGE;
}
len += (sub <= 0x7f) ? 1
: (sub <= 0x3fff) ? 2
: (sub <= 0x1fffff) ? 3
: (sub <= 0xfffffff) ? 4
: 5;
}
if ((err1 = asn_put_header(b, ASN_TYPE_OBJID, len)) != ASN_ERR_OK)
return (err1);
if (b->asn_len < len)
return (ASN_ERR_EOBUF);

for (i = 1; i < oidlen; i++) {
sub = (i == 1) ? first : oid->subs[i];
if (sub <= 0x7f) {
*b->asn_ptr++ = sub;
b->asn_len--;
} else if (sub <= 0x3fff) {
*b->asn_ptr++ = (sub >> 7) | 0x80;
*b->asn_ptr++ = sub & 0x7f;
b->asn_len -= 2;
} else if (sub <= 0x1fffff) {
*b->asn_ptr++ = (sub >> 14) | 0x80;
*b->asn_ptr++ = ((sub >> 7) & 0x7f) | 0x80;
*b->asn_ptr++ = sub & 0x7f;
b->asn_len -= 3;
} else if (sub <= 0xfffffff) {
*b->asn_ptr++ = (sub >> 21) | 0x80;
*b->asn_ptr++ = ((sub >> 14) & 0x7f) | 0x80;
*b->asn_ptr++ = ((sub >> 7) & 0x7f) | 0x80;
*b->asn_ptr++ = sub & 0x7f;
b->asn_len -= 4;
} else {
*b->asn_ptr++ = (sub >> 28) | 0x80;
*b->asn_ptr++ = ((sub >> 21) & 0x7f) | 0x80;
*b->asn_ptr++ = ((sub >> 14) & 0x7f) | 0x80;
*b->asn_ptr++ = ((sub >> 7) & 0x7f) | 0x80;
*b->asn_ptr++ = sub & 0x7f;
b->asn_len -= 5;
}
}
return (err);
}





enum asn_err
asn_get_sequence(struct asn_buf *b, asn_len_t *len)
{
u_char type;
enum asn_err err;

if ((err = asn_get_header(b, &type, len)) != ASN_ERR_OK)
return (err);
if (type != (ASN_TYPE_SEQUENCE|ASN_TYPE_CONSTRUCTED)) {
asn_error(b, "bad sequence type %u", type);
return (ASN_ERR_TAG);
}
if (*len > b->asn_len) {
asn_error(b, "truncated sequence");
return (ASN_ERR_EOBUF);
}
return (ASN_ERR_OK);
}






enum asn_err
asn_get_ipaddress_raw(struct asn_buf *b, asn_len_t len, u_char *addr)
{
u_int i;

if (b->asn_len < len) {
asn_error(b, "truncated ip-address");
return (ASN_ERR_EOBUF);
}
if (len < 4) {
asn_error(b, "short length for ip-Address %u", len);
for (i = 0; i < len; i++)
*addr++ = *b->asn_cptr++;
while (i++ < len)
*addr++ = 0;
b->asn_len -= len;
return (ASN_ERR_BADLEN);
}
for (i = 0; i < 4; i++)
*addr++ = *b->asn_cptr++;
b->asn_cptr += len - 4;
b->asn_len -= len;
return (ASN_ERR_OK);
}

enum asn_err
asn_get_ipaddress(struct asn_buf *b, u_char *addr)
{
u_char type;
asn_len_t len;
enum asn_err err;

if ((err = asn_get_header(b, &type, &len)) != ASN_ERR_OK)
return (err);
if (type != (ASN_CLASS_APPLICATION|ASN_APP_IPADDRESS)) {
asn_error(b, "bad type for ip-address %u", type);
return (ASN_ERR_TAG);
}
return (asn_get_ipaddress_raw(b, len, addr));
}

enum asn_err
asn_put_ipaddress(struct asn_buf *b, const u_char *addr)
{
enum asn_err err;

if ((err = asn_put_header(b, ASN_CLASS_APPLICATION|ASN_APP_IPADDRESS,
4)) != ASN_ERR_OK)
return (err);
if (b->asn_len < 4)
return (ASN_ERR_EOBUF);

memcpy(b->asn_ptr, addr, 4);
b->asn_ptr += 4;
b->asn_len -= 4;
return (ASN_ERR_OK);
}







enum asn_err
asn_get_uint32_raw(struct asn_buf *b, asn_len_t len, uint32_t *vp)
{
uint64_t v;
enum asn_err err;

if ((err = asn_get_real_unsigned(b, len, &v)) == ASN_ERR_OK) {
if (v > UINT32_MAX) {
asn_error(b, "uint32 too large %llu", v);
err = ASN_ERR_RANGE;
}
*vp = (uint32_t)v;
}
return (err);
}

enum asn_err
asn_put_uint32(struct asn_buf *b, u_char type, uint32_t val)
{
uint64_t v = val;

return (asn_put_real_unsigned(b, ASN_CLASS_APPLICATION|type, v));
}





enum asn_err
asn_get_counter64_raw(struct asn_buf *b, asn_len_t len, uint64_t *vp)
{
return (asn_get_real_unsigned(b, len, vp));
}

enum asn_err
asn_put_counter64(struct asn_buf *b, uint64_t val)
{
return (asn_put_real_unsigned(b,
ASN_CLASS_APPLICATION | ASN_APP_COUNTER64, val));
}





enum asn_err
asn_get_timeticks(struct asn_buf *b, uint32_t *vp)
{
asn_len_t len;
u_char type;
enum asn_err err;

if ((err = asn_get_header(b, &type, &len)) != ASN_ERR_OK)
return (err);
if (type != (ASN_CLASS_APPLICATION|ASN_APP_TIMETICKS)) {
asn_error(b, "bad type for timeticks %u", type);
return (ASN_ERR_TAG);
}
return (asn_get_uint32_raw(b, len, vp));
}

enum asn_err
asn_put_timeticks(struct asn_buf *b, uint32_t val)
{
uint64_t v = val;

return (asn_put_real_unsigned(b,
ASN_CLASS_APPLICATION | ASN_APP_TIMETICKS, v));
}




void
asn_slice_oid(struct asn_oid *dest, const struct asn_oid *src,
u_int from, u_int to)
{
if (from >= to) {
dest->len = 0;
return;
}
dest->len = to - from;
memcpy(dest->subs, &src->subs[from], dest->len * sizeof(dest->subs[0]));
}




void
asn_append_oid(struct asn_oid *to, const struct asn_oid *from)
{
memcpy(&to->subs[to->len], &from->subs[0],
from->len * sizeof(from->subs[0]));
to->len += from->len;
}




enum asn_err
asn_skip(struct asn_buf *b, asn_len_t len)
{
if (b->asn_len < len)
return (ASN_ERR_EOBUF);
b->asn_cptr += len;
b->asn_len -= len;
return (ASN_ERR_OK);
}




enum asn_err
asn_pad(struct asn_buf *b, asn_len_t len)
{
if (b->asn_len < len)
return (ASN_ERR_EOBUF);
b->asn_ptr += len;
b->asn_len -= len;

return (ASN_ERR_OK);
}








int
asn_compare_oid(const struct asn_oid *o1, const struct asn_oid *o2)
{
u_long i;

for (i = 0; i < o1->len && i < o2->len; i++) {
if (o1->subs[i] < o2->subs[i])
return (-1);
if (o1->subs[i] > o2->subs[i])
return (+1);
}
if (o1->len < o2->len)
return (-1);
if (o1->len > o2->len)
return (+1);
return (0);
}




int
asn_is_suboid(const struct asn_oid *o1, const struct asn_oid *o2)
{
u_long i;

for (i = 0; i < o1->len; i++)
if (i >= o2->len || o1->subs[i] != o2->subs[i])
return (0);
return (1);
}







char *
asn_oid2str_r(const struct asn_oid *oid, char *buf)
{
u_int len, i;
char *ptr;

if ((len = oid->len) > ASN_MAXOIDLEN)
len = ASN_MAXOIDLEN;
buf[0] = '\0';
for (i = 0, ptr = buf; i < len; i++) {
if (i > 0)
*ptr++ = '.';
ptr += sprintf(ptr, "%u", oid->subs[i]);
}
return (buf);
}




char *
asn_oid2str(const struct asn_oid *oid)
{
static char str[ASN_OIDSTRLEN];

return (asn_oid2str_r(oid, str));
}


static void
asn_error_func(const struct asn_buf *b, const char *err, ...)
{
va_list ap;
u_long i;

fprintf(stderr, "ASN.1: ");
va_start(ap, err);
vfprintf(stderr, err, ap);
va_end(ap);

if (b != NULL) {
fprintf(stderr, " at");
for (i = 0; b->asn_len > i; i++)
fprintf(stderr, " %02x", b->asn_cptr[i]);
}
fprintf(stderr, "\n");
}
