























#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "brssl.h"


unsigned char *
read_file(const char *fname, size_t *len)
{
bvector vbuf = VEC_INIT;
FILE *f;

*len = 0;
f = fopen(fname, "rb");
if (f == NULL) {
fprintf(stderr,
"ERROR: could not open file '%s' for reading\n", fname);
return NULL;
}
for (;;) {
unsigned char tmp[1024];
size_t rlen;

rlen = fread(tmp, 1, sizeof tmp, f);
if (rlen == 0) {
unsigned char *buf;

if (ferror(f)) {
fprintf(stderr,
"ERROR: read error on file '%s'\n",
fname);
fclose(f);
return NULL;
}
buf = VEC_TOARRAY(vbuf);
*len = VEC_LEN(vbuf);
VEC_CLEAR(vbuf);
fclose(f);
return buf;
}
VEC_ADDMANY(vbuf, tmp, rlen);
}
}


int
write_file(const char *fname, const void *data, size_t len)
{
FILE *f;
const unsigned char *buf;

f = fopen(fname, "wb");
if (f == NULL) {
fprintf(stderr,
"ERROR: could not open file '%s' for reading\n", fname);
return -1;
}
buf = data;
while (len > 0) {
size_t wlen;

wlen = fwrite(buf, 1, len, f);
if (wlen == 0) {
fprintf(stderr,
"ERROR: could not write all bytes to '%s'\n",
fname);
fclose(f);
return -1;
}
buf += wlen;
len -= wlen;
}
if (ferror(f)) {
fprintf(stderr, "ERROR: write error on file '%s'\n", fname);
fclose(f);
return -1;
}
fclose(f);
return 0;
}


int
looks_like_DER(const unsigned char *buf, size_t len)
{
int fb;
size_t dlen;

if (len < 2) {
return 0;
}
if (*buf ++ != 0x30) {
return 0;
}
fb = *buf ++;
len -= 2;
if (fb < 0x80) {
return (size_t)fb == len;
} else if (fb == 0x80) {
return 0;
} else {
fb -= 0x80;
if (len < (size_t)fb + 2) {
return 0;
}
len -= (size_t)fb;
dlen = 0;
while (fb -- > 0) {
if (dlen > (len >> 8)) {
return 0;
}
dlen = (dlen << 8) + (size_t)*buf ++;
}
return dlen == len;
}
}

static void
vblob_append(void *cc, const void *data, size_t len)
{
bvector *bv;

bv = cc;
VEC_ADDMANY(*bv, data, len);
}


void
free_pem_object_contents(pem_object *po)
{
if (po != NULL) {
xfree(po->name);
xfree(po->data);
}
}


pem_object *
decode_pem(const void *src, size_t len, size_t *num)
{
VECTOR(pem_object) pem_list = VEC_INIT;
br_pem_decoder_context pc;
pem_object po, *pos;
const unsigned char *buf;
bvector bv = VEC_INIT;
int inobj;
int extra_nl;

*num = 0;
br_pem_decoder_init(&pc);
buf = src;
inobj = 0;
po.name = NULL;
po.data = NULL;
po.data_len = 0;
extra_nl = 1;
while (len > 0) {
size_t tlen;

tlen = br_pem_decoder_push(&pc, buf, len);
buf += tlen;
len -= tlen;
switch (br_pem_decoder_event(&pc)) {

case BR_PEM_BEGIN_OBJ:
po.name = xstrdup(br_pem_decoder_name(&pc));
br_pem_decoder_setdest(&pc, vblob_append, &bv);
inobj = 1;
break;

case BR_PEM_END_OBJ:
if (inobj) {
po.data = VEC_TOARRAY(bv);
po.data_len = VEC_LEN(bv);
VEC_ADD(pem_list, po);
VEC_CLEAR(bv);
po.name = NULL;
po.data = NULL;
po.data_len = 0;
inobj = 0;
}
break;

case BR_PEM_ERROR:
xfree(po.name);
VEC_CLEAR(bv);
fprintf(stderr,
"ERROR: invalid PEM encoding\n");
VEC_CLEAREXT(pem_list, &free_pem_object_contents);
return NULL;
}








if (len == 0 && extra_nl) {
extra_nl = 0;
buf = (const unsigned char *)"\n";
len = 1;
}
}
if (inobj) {
fprintf(stderr, "ERROR: unfinished PEM object\n");
xfree(po.name);
VEC_CLEAR(bv);
VEC_CLEAREXT(pem_list, &free_pem_object_contents);
return NULL;
}

*num = VEC_LEN(pem_list);
VEC_ADD(pem_list, po);
pos = VEC_TOARRAY(pem_list);
VEC_CLEAR(pem_list);
return pos;
}


br_x509_certificate *
read_certificates(const char *fname, size_t *num)
{
VECTOR(br_x509_certificate) cert_list = VEC_INIT;
unsigned char *buf;
size_t len;
pem_object *pos;
size_t u, num_pos;
br_x509_certificate *xcs;
br_x509_certificate dummy;

*num = 0;





buf = read_file(fname, &len);
if (buf == NULL) {
return NULL;
}




if (looks_like_DER(buf, len)) {
xcs = xmalloc(2 * sizeof *xcs);
xcs[0].data = buf;
xcs[0].data_len = len;
xcs[1].data = NULL;
xcs[1].data_len = 0;
*num = 1;
return xcs;
}

pos = decode_pem(buf, len, &num_pos);
xfree(buf);
if (pos == NULL) {
return NULL;
}
for (u = 0; u < num_pos; u ++) {
if (eqstr(pos[u].name, "CERTIFICATE")
|| eqstr(pos[u].name, "X509 CERTIFICATE"))
{
br_x509_certificate xc;

xc.data = pos[u].data;
xc.data_len = pos[u].data_len;
pos[u].data = NULL;
VEC_ADD(cert_list, xc);
}
}
for (u = 0; u < num_pos; u ++) {
free_pem_object_contents(&pos[u]);
}
xfree(pos);

if (VEC_LEN(cert_list) == 0) {
fprintf(stderr, "ERROR: no certificate in file '%s'\n", fname);
return NULL;
}
*num = VEC_LEN(cert_list);
dummy.data = NULL;
dummy.data_len = 0;
VEC_ADD(cert_list, dummy);
xcs = VEC_TOARRAY(cert_list);
VEC_CLEAR(cert_list);
return xcs;
}


void
free_certificates(br_x509_certificate *certs, size_t num)
{
size_t u;

for (u = 0; u < num; u ++) {
xfree(certs[u].data);
}
xfree(certs);
}
