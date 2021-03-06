























#include "inner.h"





static void
mkrand(const br_prng_class **rng, uint16_t *x, uint32_t esize)
{
size_t u, len;
unsigned m;

len = (esize + 15) >> 4;
(*rng)->generate(rng, x + 1, len * sizeof(uint16_t));
for (u = 1; u < len; u ++) {
x[u] &= 0x7FFF;
}
m = esize & 15;
if (m == 0) {
x[len] &= 0x7FFF;
} else {
x[len] &= 0x7FFF >> (15 - m);
}
}





static const unsigned char SMALL_PRIMES[] = {
0x2E, 0xAB, 0x92, 0xD1, 0x8B, 0x12, 0x47, 0x31, 0x54, 0x0A,
0x99, 0x5D, 0x25, 0x5E, 0xE2, 0x14, 0x96, 0x29, 0x1E, 0xB7,
0x78, 0x70, 0xCC, 0x1F, 0xA5, 0xAB, 0x8D, 0x72, 0x11, 0x37,
0xFB, 0xD8, 0x1E, 0x3F, 0x5B, 0x34, 0x30, 0x17, 0x8B, 0xE5,
0x26, 0x28, 0x23, 0xA1, 0x8A, 0xA4, 0x29, 0xEA, 0xFD, 0x9E,
0x39, 0x60, 0x8A, 0xF3, 0xB5, 0xA6, 0xEB, 0x3F, 0x02, 0xB6,
0x16, 0xC3, 0x96, 0x9D, 0x38, 0xB0, 0x7D, 0x82, 0x87, 0x0C,
0xF7, 0xBE, 0x24, 0xE5, 0x5F, 0x41, 0x04, 0x79, 0x76, 0x40,
0xE7, 0x00, 0x22, 0x7E, 0xB5, 0x85, 0x7F, 0x8D, 0x01, 0x50,
0xE9, 0xD3, 0x29, 0x42, 0x08, 0xB3, 0x51, 0x40, 0x7B, 0xD7,
0x8D, 0xCC, 0x10, 0x01, 0x64, 0x59, 0x28, 0xB6, 0x53, 0xF3,
0x50, 0x4E, 0xB1, 0xF2, 0x58, 0xCD, 0x6E, 0xF5, 0x56, 0x3E,
0x66, 0x2F, 0xD7, 0x07, 0x7F, 0x52, 0x4C, 0x13, 0x24, 0xDC,
0x8E, 0x8D, 0xCC, 0xED, 0x77, 0xC4, 0x21, 0xD2, 0xFD, 0x08,
0xEA, 0xD7, 0xC0, 0x5C, 0x13, 0x82, 0x81, 0x31, 0x2F, 0x2B,
0x08, 0xE4, 0x80, 0x04, 0x7A, 0x0C, 0x8A, 0x3C, 0xDC, 0x22,
0xE4, 0x5A, 0x7A, 0xB0, 0x12, 0x5E, 0x4A, 0x76, 0x94, 0x77,
0xC2, 0x0E, 0x92, 0xBA, 0x8A, 0xA0, 0x1F, 0x14, 0x51, 0x1E,
0x66, 0x6C, 0x38, 0x03, 0x6C, 0xC7, 0x4A, 0x4B, 0x70, 0x80,
0xAF, 0xCA, 0x84, 0x51, 0xD8, 0xD2, 0x26, 0x49, 0xF5, 0xA8,
0x5E, 0x35, 0x4B, 0xAC, 0xCE, 0x29, 0x92, 0x33, 0xB7, 0xA2,
0x69, 0x7D, 0x0C, 0xE0, 0x9C, 0xDB, 0x04, 0xD6, 0xB4, 0xBC,
0x39, 0xD7, 0x7F, 0x9E, 0x9D, 0x78, 0x38, 0x7F, 0x51, 0x54,
0x50, 0x8B, 0x9E, 0x9C, 0x03, 0x6C, 0xF5, 0x9D, 0x2C, 0x74,
0x57, 0xF0, 0x27, 0x2A, 0xC3, 0x47, 0xCA, 0xB9, 0xD7, 0x5C,
0xFF, 0xC2, 0xAC, 0x65, 0x4E, 0xBD
};










#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define TEMPS MAX(1024, 7 * ((((BR_MAX_RSA_SIZE + 1) >> 1) + 29) / 15))










static uint32_t
trial_divisions(const uint16_t *x, uint16_t *t)
{
uint16_t *y;
uint16_t x0i;

y = t;
t += 1 + ((x[0] + 15) >> 4);
x0i = br_i15_ninv15(x[1]);
br_i15_decode_reduce(y, SMALL_PRIMES, sizeof SMALL_PRIMES, x);
return br_i15_moddiv(y, y, x, x0i, t);
}








static uint32_t
miller_rabin(const br_prng_class **rng, const uint16_t *x, int n,
uint16_t *t, size_t tlen)
{
















unsigned char *xm1d2;
size_t xlen, xm1d2_len, xm1d2_len_u16, u;
uint32_t asize;
unsigned cc;
uint16_t x0i;




xm1d2 = (unsigned char *)t;
xm1d2_len = ((x[0] - (x[0] >> 4)) + 7) >> 3;
br_i15_encode(xm1d2, xm1d2_len, x);
cc = 0;
for (u = 0; u < xm1d2_len; u ++) {
unsigned w;

w = xm1d2[u];
xm1d2[u] = (unsigned char)((w >> 1) | cc);
cc = w << 7;
}




xm1d2_len_u16 = (xm1d2_len + 1) >> 1;
t += xm1d2_len_u16;
tlen -= xm1d2_len_u16;

xlen = (x[0] + 15) >> 4;
asize = x[0] - 1 - EQ0(x[0] & 15);
x0i = br_i15_ninv15(x[1]);
while (n -- > 0) {
uint16_t *a;
uint32_t eq1, eqm1;






a = t;
a[0] = x[0];
a[xlen] = 0;
mkrand(rng, a, asize);






br_i15_modpow_opt(a, xm1d2, xm1d2_len,
x, x0i, t + 1 + xlen, tlen - 1 - xlen);






eq1 = a[1] ^ 1;
eqm1 = a[1] ^ (x[1] - 1);
for (u = 2; u <= xlen; u ++) {
eq1 |= a[u];
eqm1 |= a[u] ^ x[u];
}

if ((EQ0(eq1) | EQ0(eqm1)) == 0) {
return 0;
}
}
return 1;
}





static void
mkprime(const br_prng_class **rng, uint16_t *x, uint32_t esize,
uint32_t pubexp, uint16_t *t, size_t tlen)
{
size_t len;

x[0] = esize;
len = (esize + 15) >> 4;
for (;;) {
size_t u;
uint32_t m3, m5, m7, m11;
int rounds;





mkrand(rng, x, esize);
if ((esize & 15) == 0) {
x[len] |= 0x6000;
} else if ((esize & 15) == 1) {
x[len] |= 0x0001;
x[len - 1] |= 0x4000;
} else {
x[len] |= 0x0003 << ((esize & 15) - 2);
}
x[1] |= 0x0003;










m3 = 0;
m5 = 0;
m7 = 0;
m11 = 0;
for (u = 0; u < len; u ++) {
uint32_t w;

w = x[1 + u];
m3 += w << (u & 1);
m3 = (m3 & 0xFF) + (m3 >> 8);
m5 += w << ((4 - u) & 3);
m5 = (m5 & 0xFF) + (m5 >> 8);
m7 += w;
m7 = (m7 & 0x1FF) + (m7 >> 9);
m11 += w << (5 & -(u & 1));
m11 = (m11 & 0x3FF) + (m11 >> 10);
}










m3 = (m3 & 0x0F) + (m3 >> 4);
m3 = (m3 & 0x0F) + (m3 >> 4);
m3 = ((m3 * 43) >> 5) & 3;

m5 = (m5 & 0xFF) + (m5 >> 8);
m5 = (m5 & 0x0F) + (m5 >> 4);
m5 = (m5 & 0x0F) + (m5 >> 4);
m5 -= 10 & -GT(m5, 9);
m5 -= 5 & -GT(m5, 4);

m7 = (m7 & 0x3F) + (m7 >> 6);
m7 = (m7 & 7) + (m7 >> 3);
m7 = ((m7 * 147) >> 7) & 7;




m11 = (m11 & 0x1F) + 66 - (m11 >> 5);
m11 -= 88 & -GT(m11, 87);
m11 -= 44 & -GT(m11, 43);
m11 -= 22 & -GT(m11, 21);
m11 -= 11 & -GT(m11, 10);











if (m3 == 0 || m5 == 0 || m7 == 0 || m11 == 0) {
continue;
}
if ((pubexp == 3 && m3 == 1)
|| (pubexp == 5 && m5 == 5)
|| (pubexp == 7 && m5 == 7)
|| (pubexp == 11 && m5 == 11))
{
continue;
}




if (!trial_divisions(x, t)) {
continue;
}















if (esize < 320) {
rounds = 12;
} else if (esize < 480) {
rounds = 9;
} else if (esize < 693) {
rounds = 6;
} else if (esize < 906) {
rounds = 4;
} else if (esize < 1386) {
rounds = 3;
} else {
rounds = 2;
}

if (miller_rabin(rng, x, rounds, t, tlen)) {
return;
}
}
}











static uint32_t
invert_pubexp(uint16_t *d, const uint16_t *m, uint32_t e, uint16_t *t)
{
uint16_t *f;
uint32_t r;

f = t;
t += 1 + ((m[0] + 15) >> 4);




br_i15_zero(d, m[0]);
d[1] = 1;
br_i15_zero(f, m[0]);
f[1] = e & 0x7FFF;
f[2] = (e >> 15) & 0x7FFF;
f[3] = e >> 30;
r = br_i15_moddiv(d, f, m, br_i15_ninv15(m[1]), t);












br_i15_add(d, m, (uint32_t)(1 - (d[1] & 1)));

return r;
}




static void
bufswap(void *b1, void *b2, size_t len)
{
size_t u;
unsigned char *buf1, *buf2;

buf1 = b1;
buf2 = b2;
for (u = 0; u < len; u ++) {
unsigned w;

w = buf1[u];
buf1[u] = buf2[u];
buf2[u] = w;
}
}


uint32_t
br_rsa_i15_keygen(const br_prng_class **rng,
br_rsa_private_key *sk, void *kbuf_priv,
br_rsa_public_key *pk, void *kbuf_pub,
unsigned size, uint32_t pubexp)
{
uint32_t esize_p, esize_q;
size_t plen, qlen, tlen;
uint16_t *p, *q, *t;
uint16_t tmp[TEMPS];
uint32_t r;

if (size < BR_MIN_RSA_SIZE || size > BR_MAX_RSA_SIZE) {
return 0;
}
if (pubexp == 0) {
pubexp = 3;
} else if (pubexp == 1 || (pubexp & 1) == 0) {
return 0;
}

esize_p = (size + 1) >> 1;
esize_q = size - esize_p;
sk->n_bitlen = size;
sk->p = kbuf_priv;
sk->plen = (esize_p + 7) >> 3;
sk->q = sk->p + sk->plen;
sk->qlen = (esize_q + 7) >> 3;
sk->dp = sk->q + sk->qlen;
sk->dplen = sk->plen;
sk->dq = sk->dp + sk->dplen;
sk->dqlen = sk->qlen;
sk->iq = sk->dq + sk->dqlen;
sk->iqlen = sk->plen;

if (pk != NULL) {
pk->n = kbuf_pub;
pk->nlen = (size + 7) >> 3;
pk->e = pk->n + pk->nlen;
pk->elen = 4;
br_enc32be(pk->e, pubexp);
while (*pk->e == 0) {
pk->e ++;
pk->elen --;
}
}







esize_p += MUL15(esize_p, 17477) >> 18;
esize_q += MUL15(esize_q, 17477) >> 18;
plen = (esize_p + 15) >> 4;
qlen = (esize_q + 15) >> 4;
p = tmp;
q = p + 1 + plen;
t = q + 1 + qlen;
tlen = ((sizeof tmp) / sizeof(uint16_t)) - (2 + plen + qlen);







for (;;) {
mkprime(rng, p, esize_p, pubexp, t, tlen);
br_i15_rshift(p, 1);
if (invert_pubexp(t, p, pubexp, t + 1 + plen)) {
br_i15_add(p, p, 1);
p[1] |= 1;
br_i15_encode(sk->p, sk->plen, p);
br_i15_encode(sk->dp, sk->dplen, t);
break;
}
}

for (;;) {
mkprime(rng, q, esize_q, pubexp, t, tlen);
br_i15_rshift(q, 1);
if (invert_pubexp(t, q, pubexp, t + 1 + qlen)) {
br_i15_add(q, q, 1);
q[1] |= 1;
br_i15_encode(sk->q, sk->qlen, q);
br_i15_encode(sk->dq, sk->dqlen, t);
break;
}
}
















if (esize_p == esize_q && br_i15_sub(p, q, 0) == 1) {
bufswap(p, q, (1 + plen) * sizeof *p);
bufswap(sk->p, sk->q, sk->plen);
bufswap(sk->dp, sk->dq, sk->dplen);
}














q[0] = p[0];
if (plen > qlen) {
q[plen] = 0;
t ++;
tlen --;
}
br_i15_zero(t, p[0]);
t[1] = 1;
r = br_i15_moddiv(t, q, p, br_i15_ninv15(p[1]), t + 1 + plen);
br_i15_encode(sk->iq, sk->iqlen, t);




if (pk != NULL) {
br_i15_zero(t, p[0]);
br_i15_mulacc(t, p, q);
br_i15_encode(pk->n, pk->nlen, t);
}

return r;
}
