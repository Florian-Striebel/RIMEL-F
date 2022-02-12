























#include "inner.h"


uint32_t
br_rsa_pkcs1_sig_unpad(const unsigned char *sig, size_t sig_len,
const unsigned char *hash_oid, size_t hash_len,
unsigned char *hash_out)
{
static const unsigned char pad1[] = {
0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

unsigned char pad2[43];
size_t u, x2, x3, pad_len, zlen;

if (sig_len < 11) {
return 0;
}































if (memcmp(sig, pad1, sizeof pad1) != 0) {
return 0;
}
for (u = sizeof pad1; u < sig_len; u ++) {
if (sig[u] != 0xFF) {
break;
}
}







if (hash_oid == NULL) {
if (sig_len - u != hash_len + 1 || sig[u] != 0x00) {
return 0;
}
} else {
x3 = hash_oid[0];
pad_len = x3 + 9;
memset(pad2, 0, pad_len);
zlen = sig_len - u - hash_len;
if (zlen == pad_len) {
x2 = x3 + 2;
} else if (zlen == pad_len + 2) {
x2 = x3 + 4;
pad_len = zlen;
pad2[pad_len - 4] = 0x05;
} else {
return 0;
}
pad2[1] = 0x30;
pad2[2] = x2 + hash_len + 4;
pad2[3] = 0x30;
pad2[4] = x2;
pad2[5] = 0x06;
memcpy(pad2 + 6, hash_oid, x3 + 1);
pad2[pad_len - 2] = 0x04;
pad2[pad_len - 1] = hash_len;
if (memcmp(pad2, sig + u, pad_len) != 0) {
return 0;
}
}
memcpy(hash_out, sig + sig_len - hash_len, hash_len);
return 1;
}
