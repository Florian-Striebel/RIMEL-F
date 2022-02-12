























#include "inner.h"


size_t
br_rsa_i15_compute_privexp(void *d,
const br_rsa_private_key *sk, uint32_t e)
{





























uint16_t tmp[4 * ((BR_MAX_RSA_FACTOR + 14) / 15) + 12];
uint16_t *p, *q, *k, *m, *z, *phi;
const unsigned char *pbuf, *qbuf;
size_t plen, qlen, u, len, dlen;
uint32_t r, a, b, u0, v0, u1, v1, he, hr;
int i;




if (e < 3 || (e & 1) == 0) {
return 0;
}




pbuf = sk->p;
plen = sk->plen;
while (plen > 0 && *pbuf == 0) {
pbuf ++;
plen --;
}
if (plen < 5 || plen > (BR_MAX_RSA_FACTOR / 8)
|| (pbuf[plen - 1] & 1) != 1)
{
return 0;
}
qbuf = sk->q;
qlen = sk->qlen;
while (qlen > 0 && *qbuf == 0) {
qbuf ++;
qlen --;
}
if (qlen < 5 || qlen > (BR_MAX_RSA_FACTOR / 8)
|| (qbuf[qlen - 1] & 1) != 1)
{
return 0;
}




dlen = (sk->n_bitlen + 7) >> 3;
if (d == NULL) {
return dlen;
}

p = tmp;
br_i15_decode(p, pbuf, plen);
plen = (p[0] + 15) >> 4;
q = p + 1 + plen;
br_i15_decode(q, qbuf, qlen);
qlen = (q[0] + 15) >> 4;








p[1] --;
q[1] --;
phi = q + 1 + qlen;
br_i15_zero(phi, p[0]);
br_i15_mulacc(phi, p, q);
len = (phi[0] + 15) >> 4;
memmove(tmp, phi, (1 + len) * sizeof *phi);
phi = tmp;
phi[0] = br_i15_bit_length(phi + 1, len);
len = (phi[0] + 15) >> 4;






r = 0;
for (u = len; u >= 1; u --) {






uint32_t hi, lo;

hi = r >> 17;
lo = (r << 15) + phi[u];
phi[u] = br_divrem(hi, lo, e, &r);
}
if (r == 0) {
return 0;
}
k = phi;



















































































a = e;
b = r;
u0 = 1;
v0 = 0;
u1 = r;
v1 = e - 1;
hr = (r + 1) >> 1;
he = (e >> 1) + 1;
for (i = 0; i < 62; i ++) {
uint32_t oa, ob, agtb, bgta;
uint32_t sab, sba, da, db;
uint32_t ctl;

oa = a & 1;
ob = b & 1;
agtb = GT(a, b);
bgta = GT(b, a);

sab = oa & ob & agtb;
sba = oa & ob & bgta;


ctl = GT(v1, v0);
a -= b & -sab;
u0 -= (u1 - (r & -ctl)) & -sab;
v0 -= (v1 - (e & -ctl)) & -sab;


ctl = GT(v0, v1);
b -= a & -sba;
u1 -= (u0 - (r & -ctl)) & -sba;
v1 -= (v0 - (e & -ctl)) & -sba;

da = NOT(oa) | sab;
db = (oa & NOT(ob)) | sba;


ctl = v0 & 1;
a ^= (a ^ (a >> 1)) & -da;
u0 ^= (u0 ^ ((u0 >> 1) + (hr & -ctl))) & -da;
v0 ^= (v0 ^ ((v0 >> 1) + (he & -ctl))) & -da;


ctl = v1 & 1;
b ^= (b ^ (b >> 1)) & -db;
u1 ^= (u1 ^ ((u1 >> 1) + (hr & -ctl))) & -db;
v1 ^= (v1 ^ ((v1 >> 1) + (he & -ctl))) & -db;
}





if (a != 1) {
return 0;
}







m = k + 1 + len;
m[0] = (2 << 4) + 2;
m[1] = v0 & 0x7FFF;
m[2] = (v0 >> 15) & 0x7FFF;
m[3] = v0 >> 30;
z = m + 4;
br_i15_zero(z, k[0]);
z[1] = u0 & 0x7FFF;
z[2] = (u0 >> 15) & 0x7FFF;
z[3] = u0 >> 30;
br_i15_mulacc(z, k, m);




br_i15_encode(d, dlen, z);
return dlen;
}
