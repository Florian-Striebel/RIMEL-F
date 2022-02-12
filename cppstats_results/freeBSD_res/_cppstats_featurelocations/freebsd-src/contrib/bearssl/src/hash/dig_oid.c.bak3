























#include "inner.h"







static const unsigned char md5_OID[] = {
0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05
};

static const unsigned char sha1_OID[] = {
0x2B, 0x0E, 0x03, 0x02, 0x1A
};

static const unsigned char sha224_OID[] = {
0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04
};

static const unsigned char sha256_OID[] = {
0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01
};

static const unsigned char sha384_OID[] = {
0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02
};

static const unsigned char sha512_OID[] = {
0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03
};


const unsigned char *
br_digest_OID(int digest_id, size_t *len)
{
switch (digest_id) {
case br_md5_ID:
*len = sizeof md5_OID;
return md5_OID;
case br_sha1_ID:
*len = sizeof sha1_OID;
return sha1_OID;
case br_sha224_ID:
*len = sizeof sha224_OID;
return sha224_OID;
case br_sha256_ID:
*len = sizeof sha256_OID;
return sha256_OID;
case br_sha384_ID:
*len = sizeof sha384_OID;
return sha384_OID;
case br_sha512_ID:
*len = sizeof sha512_OID;
return sha512_OID;
default:
*len = 0;
return NULL;
}
}
