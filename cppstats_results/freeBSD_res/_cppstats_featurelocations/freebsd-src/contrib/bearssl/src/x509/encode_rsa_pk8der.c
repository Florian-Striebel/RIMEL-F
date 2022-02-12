























#include "inner.h"


size_t
br_encode_rsa_pkcs8_der(void *dest, const br_rsa_private_key *sk,
const br_rsa_public_key *pk, const void *d, size_t dlen)
{

































static const unsigned char PK8_HEAD[] = {
0x02, 0x01, 0x00,
0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00,
0x04
};

size_t len_raw, len_seq;

len_raw = br_encode_rsa_raw_der(NULL, sk, pk, d, dlen);
len_seq = (sizeof PK8_HEAD) + len_of_len(len_raw) + len_raw;
if (dest == NULL) {
return 1 + len_of_len(len_seq) + len_seq;
} else {
unsigned char *buf;
size_t lenlen;

buf = dest;
*buf ++ = 0x30;
lenlen = br_asn1_encode_length(buf, len_seq);
buf += lenlen;


memcpy(buf, PK8_HEAD, sizeof PK8_HEAD);
buf += sizeof PK8_HEAD;


buf += br_asn1_encode_length(buf, len_raw);
br_encode_rsa_raw_der(buf, sk, pk, d, dlen);

return 1 + lenlen + len_seq;
}
}
