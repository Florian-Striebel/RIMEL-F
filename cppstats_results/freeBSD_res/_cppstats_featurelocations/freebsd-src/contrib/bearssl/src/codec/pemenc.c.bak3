























#include "inner.h"





static char
b64char(uint32_t x)
{







uint32_t a, b, c;

a = x - 26;
b = x - 52;
c = x - 62;











return (char)(((x + 0x41) & ((a & b & c) >> 8))
| ((x + (0x61 - 26)) & ((~a & b & c) >> 8))
| ((x - (52 - 0x30)) & ((~a & ~b & c) >> 8))
| ((0x2B + ((x & 1) << 2)) & (~(a | b | c) >> 8)));
}


size_t
br_pem_encode(void *dest, const void *data, size_t len,
const char *banner, unsigned flags)
{
size_t dlen, banner_len, lines;
char *d;
unsigned char *buf;
size_t u;
int off, lim;

banner_len = strlen(banner);


if ((flags & BR_PEM_LINE64) != 0) {
lines = (len + 47) / 48;
} else {
lines = (len + 56) / 57;
}
dlen = (banner_len << 1) + 30 + (((len + 2) / 3) << 2)
+ lines + 2;
if ((flags & BR_PEM_CRLF) != 0) {
dlen += lines + 2;
}

if (dest == NULL) {
return dlen;
}

d = dest;







buf = (unsigned char *)d + dlen - len;
memmove(buf, data, len);

memcpy(d, "-----BEGIN ", 11);
d += 11;
memcpy(d, banner, banner_len);
d += banner_len;
memcpy(d, "-----", 5);
d += 5;
if ((flags & BR_PEM_CRLF) != 0) {
*d ++ = 0x0D;
}
*d ++ = 0x0A;

off = 0;
lim = (flags & BR_PEM_LINE64) != 0 ? 16 : 19;
for (u = 0; (u + 2) < len; u += 3) {
uint32_t w;

w = ((uint32_t)buf[u] << 16)
| ((uint32_t)buf[u + 1] << 8)
| (uint32_t)buf[u + 2];
*d ++ = b64char(w >> 18);
*d ++ = b64char((w >> 12) & 0x3F);
*d ++ = b64char((w >> 6) & 0x3F);
*d ++ = b64char(w & 0x3F);
if (++ off == lim) {
off = 0;
if ((flags & BR_PEM_CRLF) != 0) {
*d ++ = 0x0D;
}
*d ++ = 0x0A;
}
}
if (u < len) {
uint32_t w;

w = (uint32_t)buf[u] << 16;
if (u + 1 < len) {
w |= (uint32_t)buf[u + 1] << 8;
}
*d ++ = b64char(w >> 18);
*d ++ = b64char((w >> 12) & 0x3F);
if (u + 1 < len) {
*d ++ = b64char((w >> 6) & 0x3F);
} else {
*d ++ = 0x3D;
}
*d ++ = 0x3D;
off ++;
}
if (off != 0) {
if ((flags & BR_PEM_CRLF) != 0) {
*d ++ = 0x0D;
}
*d ++ = 0x0A;
}

memcpy(d, "-----END ", 9);
d += 9;
memcpy(d, banner, banner_len);
d += banner_len;
memcpy(d, "-----", 5);
d += 5;
if ((flags & BR_PEM_CRLF) != 0) {
*d ++ = 0x0D;
}
*d ++ = 0x0A;


*d ++ = 0x00;

return dlen;
}
