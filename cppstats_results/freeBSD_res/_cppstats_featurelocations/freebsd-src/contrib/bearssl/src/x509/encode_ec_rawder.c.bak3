























#include "inner.h"


const unsigned char *
br_get_curve_OID(int curve)
{
static const unsigned char OID_secp256r1[] = {
0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07
};
static const unsigned char OID_secp384r1[] = {
0x05, 0x2b, 0x81, 0x04, 0x00, 0x22
};
static const unsigned char OID_secp521r1[] = {
0x05, 0x2b, 0x81, 0x04, 0x00, 0x23
};

switch (curve) {
case BR_EC_secp256r1: return OID_secp256r1;
case BR_EC_secp384r1: return OID_secp384r1;
case BR_EC_secp521r1: return OID_secp521r1;
default:
return NULL;
}
}


size_t
br_encode_ec_raw_der_inner(void *dest,
const br_ec_private_key *sk, const br_ec_public_key *pk,
int include_curve_oid)
{

























size_t len_version, len_privateKey, len_parameters, len_publicKey;
size_t len_publicKey_bits, len_seq;
const unsigned char *oid;

if (include_curve_oid) {
oid = br_get_curve_OID(sk->curve);
if (oid == NULL) {
return 0;
}
} else {
oid = NULL;
}
len_version = 3;
len_privateKey = 1 + len_of_len(sk->xlen) + sk->xlen;
if (include_curve_oid) {
len_parameters = 4 + oid[0];
} else {
len_parameters = 0;
}
if (pk == NULL) {
len_publicKey = 0;
len_publicKey_bits = 0;
} else {
len_publicKey_bits = 2 + len_of_len(pk->qlen) + pk->qlen;
len_publicKey = 1 + len_of_len(len_publicKey_bits)
+ len_publicKey_bits;
}
len_seq = len_version + len_privateKey + len_parameters + len_publicKey;
if (dest == NULL) {
return 1 + len_of_len(len_seq) + len_seq;
} else {
unsigned char *buf;
size_t lenlen;

buf = dest;
*buf ++ = 0x30;
lenlen = br_asn1_encode_length(buf, len_seq);
buf += lenlen;


*buf ++ = 0x02;
*buf ++ = 0x01;
*buf ++ = 0x01;


*buf ++ = 0x04;
buf += br_asn1_encode_length(buf, sk->xlen);
memcpy(buf, sk->x, sk->xlen);
buf += sk->xlen;


if (include_curve_oid) {
*buf ++ = 0xA0;
*buf ++ = oid[0] + 2;
*buf ++ = 0x06;
memcpy(buf, oid, oid[0] + 1);
buf += oid[0] + 1;
}


if (pk != NULL) {
*buf ++ = 0xA1;
buf += br_asn1_encode_length(buf, len_publicKey_bits);
*buf ++ = 0x03;
buf += br_asn1_encode_length(buf, pk->qlen + 1);
*buf ++ = 0x00;
memcpy(buf, pk->q, pk->qlen);

}

return 1 + lenlen + len_seq;
}
}


size_t
br_encode_ec_raw_der(void *dest,
const br_ec_private_key *sk, const br_ec_public_key *pk)
{
return br_encode_ec_raw_der_inner(dest, sk, pk, 1);
}
