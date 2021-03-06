























#include "inner.h"


size_t
br_encode_rsa_raw_der(void *dest, const br_rsa_private_key *sk,
const br_rsa_public_key *pk, const void *d, size_t dlen)
{





















br_asn1_uint num[9];
size_t u, slen;





num[0] = br_asn1_uint_prepare(NULL, 0);
num[1] = br_asn1_uint_prepare(pk->n, pk->nlen);
num[2] = br_asn1_uint_prepare(pk->e, pk->elen);
num[3] = br_asn1_uint_prepare(d, dlen);
num[4] = br_asn1_uint_prepare(sk->p, sk->plen);
num[5] = br_asn1_uint_prepare(sk->q, sk->qlen);
num[6] = br_asn1_uint_prepare(sk->dp, sk->dplen);
num[7] = br_asn1_uint_prepare(sk->dq, sk->dqlen);
num[8] = br_asn1_uint_prepare(sk->iq, sk->iqlen);




slen = 0;
for (u = 0; u < 9; u ++) {
uint32_t ilen;

ilen = num[u].asn1len;
slen += 1 + len_of_len(ilen) + ilen;
}

if (dest == NULL) {
return 1 + len_of_len(slen) + slen;
} else {
unsigned char *buf;
size_t lenlen;

buf = dest;
*buf ++ = 0x30;
lenlen = br_asn1_encode_length(buf, slen);
buf += lenlen;
for (u = 0; u < 9; u ++) {
buf += br_asn1_encode_uint(buf, num[u]);
}
return 1 + lenlen + slen;
}
}
