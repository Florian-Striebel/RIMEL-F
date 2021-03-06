























#include "inner.h"


void
br_aes_ct_bitslice_invSbox(uint32_t *q)
{


















uint32_t q0, q1, q2, q3, q4, q5, q6, q7;

q0 = ~q[0];
q1 = ~q[1];
q2 = q[2];
q3 = q[3];
q4 = q[4];
q5 = ~q[5];
q6 = ~q[6];
q7 = q[7];
q[7] = q1 ^ q4 ^ q6;
q[6] = q0 ^ q3 ^ q5;
q[5] = q7 ^ q2 ^ q4;
q[4] = q6 ^ q1 ^ q3;
q[3] = q5 ^ q0 ^ q2;
q[2] = q4 ^ q7 ^ q1;
q[1] = q3 ^ q6 ^ q0;
q[0] = q2 ^ q5 ^ q7;

br_aes_ct_bitslice_Sbox(q);

q0 = ~q[0];
q1 = ~q[1];
q2 = q[2];
q3 = q[3];
q4 = q[4];
q5 = ~q[5];
q6 = ~q[6];
q7 = q[7];
q[7] = q1 ^ q4 ^ q6;
q[6] = q0 ^ q3 ^ q5;
q[5] = q7 ^ q2 ^ q4;
q[4] = q6 ^ q1 ^ q3;
q[3] = q5 ^ q0 ^ q2;
q[2] = q4 ^ q7 ^ q1;
q[1] = q3 ^ q6 ^ q0;
q[0] = q2 ^ q5 ^ q7;
}

static void
add_round_key(uint32_t *q, const uint32_t *sk)
{
int i;

for (i = 0; i < 8; i ++) {
q[i] ^= sk[i];
}
}

static void
inv_shift_rows(uint32_t *q)
{
int i;

for (i = 0; i < 8; i ++) {
uint32_t x;

x = q[i];
q[i] = (x & 0x000000FF)
| ((x & 0x00003F00) << 2) | ((x & 0x0000C000) >> 6)
| ((x & 0x000F0000) << 4) | ((x & 0x00F00000) >> 4)
| ((x & 0x03000000) << 6) | ((x & 0xFC000000) >> 2);
}
}

static inline uint32_t
rotr16(uint32_t x)
{
return (x << 16) | (x >> 16);
}

static void
inv_mix_columns(uint32_t *q)
{
uint32_t q0, q1, q2, q3, q4, q5, q6, q7;
uint32_t r0, r1, r2, r3, r4, r5, r6, r7;

q0 = q[0];
q1 = q[1];
q2 = q[2];
q3 = q[3];
q4 = q[4];
q5 = q[5];
q6 = q[6];
q7 = q[7];
r0 = (q0 >> 8) | (q0 << 24);
r1 = (q1 >> 8) | (q1 << 24);
r2 = (q2 >> 8) | (q2 << 24);
r3 = (q3 >> 8) | (q3 << 24);
r4 = (q4 >> 8) | (q4 << 24);
r5 = (q5 >> 8) | (q5 << 24);
r6 = (q6 >> 8) | (q6 << 24);
r7 = (q7 >> 8) | (q7 << 24);

q[0] = q5 ^ q6 ^ q7 ^ r0 ^ r5 ^ r7 ^ rotr16(q0 ^ q5 ^ q6 ^ r0 ^ r5);
q[1] = q0 ^ q5 ^ r0 ^ r1 ^ r5 ^ r6 ^ r7 ^ rotr16(q1 ^ q5 ^ q7 ^ r1 ^ r5 ^ r6);
q[2] = q0 ^ q1 ^ q6 ^ r1 ^ r2 ^ r6 ^ r7 ^ rotr16(q0 ^ q2 ^ q6 ^ r2 ^ r6 ^ r7);
q[3] = q0 ^ q1 ^ q2 ^ q5 ^ q6 ^ r0 ^ r2 ^ r3 ^ r5 ^ rotr16(q0 ^ q1 ^ q3 ^ q5 ^ q6 ^ q7 ^ r0 ^ r3 ^ r5 ^ r7);
q[4] = q1 ^ q2 ^ q3 ^ q5 ^ r1 ^ r3 ^ r4 ^ r5 ^ r6 ^ r7 ^ rotr16(q1 ^ q2 ^ q4 ^ q5 ^ q7 ^ r1 ^ r4 ^ r5 ^ r6);
q[5] = q2 ^ q3 ^ q4 ^ q6 ^ r2 ^ r4 ^ r5 ^ r6 ^ r7 ^ rotr16(q2 ^ q3 ^ q5 ^ q6 ^ r2 ^ r5 ^ r6 ^ r7);
q[6] = q3 ^ q4 ^ q5 ^ q7 ^ r3 ^ r5 ^ r6 ^ r7 ^ rotr16(q3 ^ q4 ^ q6 ^ q7 ^ r3 ^ r6 ^ r7);
q[7] = q4 ^ q5 ^ q6 ^ r4 ^ r6 ^ r7 ^ rotr16(q4 ^ q5 ^ q7 ^ r4 ^ r7);
}


void
br_aes_ct_bitslice_decrypt(unsigned num_rounds,
const uint32_t *skey, uint32_t *q)
{
unsigned u;

add_round_key(q, skey + (num_rounds << 3));
for (u = num_rounds - 1; u > 0; u --) {
inv_shift_rows(q);
br_aes_ct_bitslice_invSbox(q);
add_round_key(q, skey + (u << 3));
inv_mix_columns(q);
}
inv_shift_rows(q);
br_aes_ct_bitslice_invSbox(q);
add_round_key(q, skey);
}
