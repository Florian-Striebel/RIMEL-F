











#include "int_lib.h"






#if defined(_MSC_VER) && !defined(__clang__)


#pragma warning(push)
#pragma warning(disable : 4724)
#endif

COMPILER_RT_ABI du_int __udivmoddi4(du_int a, du_int b, du_int *rem) {
const unsigned n_uword_bits = sizeof(su_int) * CHAR_BIT;
const unsigned n_udword_bits = sizeof(du_int) * CHAR_BIT;
udwords n;
n.all = a;
udwords d;
d.all = b;
udwords q;
udwords r;
unsigned sr;

if (n.s.high == 0) {
if (d.s.high == 0) {



if (rem)
*rem = n.s.low % d.s.low;
return n.s.low / d.s.low;
}



if (rem)
*rem = n.s.low;
return 0;
}

if (d.s.low == 0) {
if (d.s.high == 0) {



if (rem)
*rem = n.s.high % d.s.low;
return n.s.high / d.s.low;
}

if (n.s.low == 0) {



if (rem) {
r.s.high = n.s.high % d.s.high;
r.s.low = 0;
*rem = r.all;
}
return n.s.high / d.s.high;
}



if ((d.s.high & (d.s.high - 1)) == 0) {
if (rem) {
r.s.low = n.s.low;
r.s.high = n.s.high & (d.s.high - 1);
*rem = r.all;
}
return n.s.high >> __builtin_ctz(d.s.high);
}



sr = clzsi(d.s.high) - clzsi(n.s.high);

if (sr > n_uword_bits - 2) {
if (rem)
*rem = n.all;
return 0;
}
++sr;


q.s.low = 0;
q.s.high = n.s.low << (n_uword_bits - sr);

r.s.high = n.s.high >> sr;
r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
} else {
if (d.s.high == 0) {



if ((d.s.low & (d.s.low - 1)) == 0) {
if (rem)
*rem = n.s.low & (d.s.low - 1);
if (d.s.low == 1)
return n.all;
sr = __builtin_ctz(d.s.low);
q.s.high = n.s.high >> sr;
q.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
return q.all;
}



sr = 1 + n_uword_bits + clzsi(d.s.low) - clzsi(n.s.high);



if (sr == n_uword_bits) {
q.s.low = 0;
q.s.high = n.s.low;
r.s.high = 0;
r.s.low = n.s.high;
} else if (sr < n_uword_bits) {
q.s.low = 0;
q.s.high = n.s.low << (n_uword_bits - sr);
r.s.high = n.s.high >> sr;
r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
} else {
q.s.low = n.s.low << (n_udword_bits - sr);
q.s.high = (n.s.high << (n_udword_bits - sr)) |
(n.s.low >> (sr - n_uword_bits));
r.s.high = 0;
r.s.low = n.s.high >> (sr - n_uword_bits);
}
} else {



sr = clzsi(d.s.high) - clzsi(n.s.high);

if (sr > n_uword_bits - 1) {
if (rem)
*rem = n.all;
return 0;
}
++sr;


q.s.low = 0;
if (sr == n_uword_bits) {
q.s.high = n.s.low;
r.s.high = 0;
r.s.low = n.s.high;
} else {
q.s.high = n.s.low << (n_uword_bits - sr);
r.s.high = n.s.high >> sr;
r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
}
}
}





su_int carry = 0;
for (; sr > 0; --sr) {

r.s.high = (r.s.high << 1) | (r.s.low >> (n_uword_bits - 1));
r.s.low = (r.s.low << 1) | (q.s.high >> (n_uword_bits - 1));
q.s.high = (q.s.high << 1) | (q.s.low >> (n_uword_bits - 1));
q.s.low = (q.s.low << 1) | carry;






const di_int s = (di_int)(d.all - r.all - 1) >> (n_udword_bits - 1);
carry = s & 1;
r.all -= d.all & s;
}
q.all = (q.all << 1) | carry;
if (rem)
*rem = r.all;
return q.all;
}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
