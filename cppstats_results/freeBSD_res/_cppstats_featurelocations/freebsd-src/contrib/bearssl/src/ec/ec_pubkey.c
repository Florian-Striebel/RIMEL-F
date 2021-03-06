























#include "inner.h"

static const unsigned char POINT_LEN[] = {
0,
43,
43,
43,
51,
51,
61,
61,
61,
73,
73,
105,
105,
145,
145,
41,
41,
41,
49,
49,
57,
57,
65,
65,
97,
133,
65,
97,
129,
32,
56,
};


size_t
br_ec_compute_pub(const br_ec_impl *impl, br_ec_public_key *pk,
void *kbuf, const br_ec_private_key *sk)
{
int curve;
size_t len;

curve = sk->curve;
if (curve < 0 || curve >= 32 || curve >= (int)(sizeof POINT_LEN)
|| ((impl->supported_curves >> curve) & 1) == 0)
{
return 0;
}
if (kbuf == NULL) {
return POINT_LEN[curve];
}
len = impl->mulgen(kbuf, sk->x, sk->xlen, curve);
if (pk != NULL) {
pk->curve = curve;
pk->q = kbuf;
pk->qlen = len;
}
return len;
}
