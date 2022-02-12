























#include "inner.h"


int
br_ssl_choose_hash(unsigned bf)
{
static const unsigned char pref[] = {
br_sha256_ID, br_sha384_ID, br_sha512_ID,
br_sha224_ID, br_sha1_ID
};
size_t u;

for (u = 0; u < sizeof pref; u ++) {
int x;

x = pref[u];
if ((bf >> x) & 1) {
return x;
}
}
return 0;
}
