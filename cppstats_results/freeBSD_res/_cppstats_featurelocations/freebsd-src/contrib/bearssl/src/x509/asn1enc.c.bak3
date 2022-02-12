























#include "inner.h"


br_asn1_uint
br_asn1_uint_prepare(const void *xdata, size_t xlen)
{
const unsigned char *x;
br_asn1_uint t;

x = xdata;
while (xlen > 0 && *x == 0) {
x ++;
xlen --;
}
t.data = x;
t.len = xlen;
t.asn1len = xlen;
if (xlen == 0 || x[0] >= 0x80) {
t.asn1len ++;
}
return t;
}


size_t
br_asn1_encode_length(void *dest, size_t len)
{
unsigned char *buf;
size_t z;
int i, j;

buf = dest;
if (len < 0x80) {
if (buf != NULL) {
*buf = len;
}
return 1;
}
i = 0;
for (z = len; z != 0; z >>= 8) {
i ++;
}
if (buf != NULL) {
*buf ++ = 0x80 + i;
for (j = i - 1; j >= 0; j --) {
*buf ++ = len >> (j << 3);
}
}
return i + 1;
}


size_t
br_asn1_encode_uint(void *dest, br_asn1_uint pp)
{
unsigned char *buf;
size_t lenlen;

if (dest == NULL) {
return 1 + br_asn1_encode_length(NULL, pp.asn1len) + pp.asn1len;
}
buf = dest;
*buf ++ = 0x02;
lenlen = br_asn1_encode_length(buf, pp.asn1len);
buf += lenlen;
*buf = 0x00;
memcpy(buf + pp.asn1len - pp.len, pp.data, pp.len);
return 1 + lenlen + pp.asn1len;
}
