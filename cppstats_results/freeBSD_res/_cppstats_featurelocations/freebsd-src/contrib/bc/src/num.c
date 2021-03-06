


































#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <limits.h>

#include <num.h>
#include <rand.h>
#include <vm.h>




static void bc_num_m(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale);







static inline size_t bc_num_mulOverflow(size_t a, size_t b) {
size_t res = a * b;
if (BC_ERR(BC_VM_MUL_OVERFLOW(a, b, res))) bc_err(BC_ERR_MATH_OVERFLOW);
return res;
}







static inline ssize_t bc_num_neg(size_t n, bool neg) {
return (((ssize_t) n) ^ -((ssize_t) neg)) + neg;
}






ssize_t bc_num_cmpZero(const BcNum *n) {
return bc_num_neg((n)->len != 0, BC_NUM_NEG(n));
}






static inline size_t bc_num_int(const BcNum *n) {
return n->len ? n->len - BC_NUM_RDX_VAL(n) : 0;
}






static void bc_num_expand(BcNum *restrict n, size_t req) {

assert(n != NULL);

req = req >= BC_NUM_DEF_SIZE ? req : BC_NUM_DEF_SIZE;

if (req > n->cap) {

BC_SIG_LOCK;

n->num = bc_vm_realloc(n->num, BC_NUM_SIZE(req));
n->cap = req;

BC_SIG_UNLOCK;
}
}






static void bc_num_setToZero(BcNum *restrict n, size_t scale) {
assert(n != NULL);
n->scale = scale;
n->len = n->rdx = 0;
}

void bc_num_zero(BcNum *restrict n) {
bc_num_setToZero(n, 0);
}

void bc_num_one(BcNum *restrict n) {
bc_num_zero(n);
n->len = 1;
n->num[0] = 1;
}






static void bc_num_clean(BcNum *restrict n) {


while (BC_NUM_NONZERO(n) && !n->num[n->len - 1]) n->len -= 1;


if (BC_NUM_ZERO(n)) n->rdx = 0;
else {


size_t rdx = BC_NUM_RDX_VAL(n);
if (n->len < rdx) n->len = rdx;
}
}









static size_t bc_num_log10(size_t i) {
size_t len;
for (len = 1; i; i /= BC_BASE, ++len);
assert(len - 1 <= BC_BASE_DIGS + 1);
return len - 1;
}








static inline size_t bc_num_zeroDigits(const BcDig *n) {
assert(*n >= 0);
assert(((size_t) *n) < BC_BASE_POW);
return BC_BASE_DIGS - bc_num_log10((size_t) *n);
}







static size_t bc_num_intDigits(const BcNum *n) {
size_t digits = bc_num_int(n) * BC_BASE_DIGS;
if (digits > 0) digits -= bc_num_zeroDigits(n->num + n->len - 1);
return digits;
}










static size_t bc_num_nonZeroLen(const BcNum *restrict n) {
size_t i, len = n->len;
assert(len == BC_NUM_RDX_VAL(n));
for (i = len - 1; i < len && !n->num[i]; --i);
assert(i + 1 > 0);
return i + 1;
}









static BcDig bc_num_addDigits(BcDig a, BcDig b, bool *carry) {

assert(((BcBigDig) BC_BASE_POW) * 2 == ((BcDig) BC_BASE_POW) * 2);
assert(a < BC_BASE_POW);
assert(b < BC_BASE_POW);

a += b + *carry;
*carry = (a >= BC_BASE_POW);
if (*carry) a -= BC_BASE_POW;

assert(a >= 0);
assert(a < BC_BASE_POW);

return a;
}









static BcDig bc_num_subDigits(BcDig a, BcDig b, bool *carry) {

assert(a < BC_BASE_POW);
assert(b < BC_BASE_POW);

b += *carry;
*carry = (a < b);
if (*carry) a += BC_BASE_POW;

assert(a - b >= 0);
assert(a - b < BC_BASE_POW);

return a - b;
}







static void bc_num_addArrays(BcDig *restrict a, const BcDig *restrict b,
size_t len)
{
size_t i;
bool carry = false;

for (i = 0; i < len; ++i) a[i] = bc_num_addDigits(a[i], b[i], &carry);


for (; carry; ++i) a[i] = bc_num_addDigits(a[i], 0, &carry);
}







static void bc_num_subArrays(BcDig *restrict a, const BcDig *restrict b,
size_t len)
{
size_t i;
bool carry = false;

for (i = 0; i < len; ++i) a[i] = bc_num_subDigits(a[i], b[i], &carry);


for (; carry; ++i) a[i] = bc_num_subDigits(a[i], 0, &carry);
}








static void bc_num_mulArray(const BcNum *restrict a, BcBigDig b,
BcNum *restrict c)
{
size_t i;
BcBigDig carry = 0;

assert(b <= BC_BASE_POW);


if (a->len + 1 > c->cap) bc_num_expand(c, a->len + 1);


memset(c->num, 0, BC_NUM_SIZE(c->cap));


for (i = 0; i < a->len; ++i) {
BcBigDig in = ((BcBigDig) a->num[i]) * b + carry;
c->num[i] = in % BC_BASE_POW;
carry = in / BC_BASE_POW;
}

assert(carry < BC_BASE_POW);


c->num[i] = (BcDig) carry;
c->len = a->len;
c->len += (carry != 0);

bc_num_clean(c);


assert(!BC_NUM_NEG(c) || BC_NUM_NONZERO(c));
assert(BC_NUM_RDX_VAL(c) <= c->len || !c->len);
assert(!c->len || c->num[c->len - 1] || BC_NUM_RDX_VAL(c) == c->len);
}









static void bc_num_divArray(const BcNum *restrict a, BcBigDig b,
BcNum *restrict c, BcBigDig *rem)
{
size_t i;
BcBigDig carry = 0;

assert(c->cap >= a->len);


for (i = a->len - 1; i < a->len; --i) {
BcBigDig in = ((BcBigDig) a->num[i]) + carry * BC_BASE_POW;
assert(in / b < BC_BASE_POW);
c->num[i] = (BcDig) (in / b);
carry = in % b;
}


c->len = a->len;
bc_num_clean(c);
*rem = carry;


assert(!BC_NUM_NEG(c) || BC_NUM_NONZERO(c));
assert(BC_NUM_RDX_VAL(c) <= c->len || !c->len);
assert(!c->len || c->num[c->len - 1] || BC_NUM_RDX_VAL(c) == c->len);
}








static ssize_t bc_num_compare(const BcDig *restrict a, const BcDig *restrict b,
size_t len)
{
size_t i;
BcDig c = 0;
for (i = len - 1; i < len && !(c = a[i] - b[i]); --i);
return bc_num_neg(i + 1, c < 0);
}

ssize_t bc_num_cmp(const BcNum *a, const BcNum *b) {

size_t i, min, a_int, b_int, diff, ardx, brdx;
BcDig *max_num, *min_num;
bool a_max, neg = false;
ssize_t cmp;

assert(a != NULL && b != NULL);


if (a == b) return 0;


if (BC_NUM_ZERO(a)) return bc_num_neg(b->len != 0, !BC_NUM_NEG(b));
if (BC_NUM_ZERO(b)) return bc_num_cmpZero(a);
if (BC_NUM_NEG(a)) {
if (BC_NUM_NEG(b)) neg = true;
else return -1;
}
else if (BC_NUM_NEG(b)) return 1;


a_int = bc_num_int(a);
b_int = bc_num_int(b);
a_int -= b_int;


if (a_int) return neg ? -((ssize_t) a_int) : (ssize_t) a_int;


ardx = BC_NUM_RDX_VAL(a);
brdx = BC_NUM_RDX_VAL(b);
a_max = (ardx > brdx);


if (a_max) {
min = brdx;
diff = ardx - brdx;
max_num = a->num + diff;
min_num = b->num;
}
else {
min = ardx;
diff = brdx - ardx;
max_num = b->num + diff;
min_num = a->num;
}


cmp = bc_num_compare(max_num, min_num, b_int + min);


if (cmp) return bc_num_neg((size_t) cmp, !a_max == !neg);



for (max_num -= diff, i = diff - 1; i < diff; --i) {
if (max_num[i]) return bc_num_neg(1, !a_max == !neg);
}

return 0;
}

void bc_num_truncate(BcNum *restrict n, size_t places) {

size_t nrdx, places_rdx;

if (!places) return;



nrdx = BC_NUM_RDX_VAL(n);
places_rdx = nrdx ? nrdx - BC_NUM_RDX(n->scale - places) : 0;


assert(places <= n->scale && (BC_NUM_ZERO(n) || places_rdx <= n->len));

n->scale -= places;
BC_NUM_RDX_SET(n, nrdx - places_rdx);


if (BC_NUM_NONZERO(n)) {

size_t pow;



pow = n->scale % BC_BASE_DIGS;
pow = pow ? BC_BASE_DIGS - pow : 0;
pow = bc_num_pow10[pow];

n->len -= places_rdx;



memmove(n->num, n->num + places_rdx, BC_NUM_SIZE(n->len));


if (BC_NUM_NONZERO(n)) n->num[0] -= n->num[0] % (BcDig) pow;

bc_num_clean(n);
}
}

void bc_num_extend(BcNum *restrict n, size_t places) {

size_t nrdx, places_rdx;

if (!places) return;


if (BC_NUM_ZERO(n)) {
n->scale += places;
return;
}



nrdx = BC_NUM_RDX_VAL(n);
places_rdx = BC_NUM_RDX(places + n->scale) - nrdx;



if (places_rdx) {
bc_num_expand(n, bc_vm_growSize(n->len, places_rdx));
memmove(n->num + places_rdx, n->num, BC_NUM_SIZE(n->len));
memset(n->num, 0, BC_NUM_SIZE(places_rdx));
}


BC_NUM_RDX_SET(n, nrdx + places_rdx);
n->scale += places;
n->len += places_rdx;

assert(BC_NUM_RDX_VAL(n) == BC_NUM_RDX(n->scale));
}




static void bc_num_retireMul(BcNum *restrict n, size_t scale,
bool neg1, bool neg2)
{

if (n->scale < scale) bc_num_extend(n, scale - n->scale);
else bc_num_truncate(n, n->scale - scale);

bc_num_clean(n);


if (BC_NUM_NONZERO(n)) n->rdx = BC_NUM_NEG_VAL(n, !neg1 != !neg2);
}








static void bc_num_split(const BcNum *restrict n, size_t idx,
BcNum *restrict a, BcNum *restrict b)
{

assert(BC_NUM_ZERO(a));
assert(BC_NUM_ZERO(b));


if (idx < n->len) {


b->len = n->len - idx;
a->len = idx;
a->scale = b->scale = 0;
BC_NUM_RDX_SET(a, 0);
BC_NUM_RDX_SET(b, 0);

assert(a->cap >= a->len);
assert(b->cap >= b->len);



memcpy(b->num, n->num + idx, BC_NUM_SIZE(b->len));
memcpy(a->num, n->num, BC_NUM_SIZE(idx));

bc_num_clean(b);
}

else bc_num_copy(a, n);

bc_num_clean(a);
}







static void bc_num_shiftRdx(const BcNum *restrict n, BcNum *restrict r) {

size_t rdx = BC_NUM_RDX_VAL(n);

r->len = n->len - rdx;
r->cap = n->cap - rdx;
r->num = n->num + rdx;

BC_NUM_RDX_SET_NEG(r, 0, BC_NUM_NEG(n));
r->scale = 0;
}






static size_t bc_num_shiftZero(BcNum *restrict n) {

size_t i;



assert(!BC_NUM_RDX_VAL(n) || BC_NUM_ZERO(n));

for (i = 0; i < n->len && !n->num[i]; ++i);

n->len -= i;
n->num += i;

return i;
}







static void bc_num_unshiftZero(BcNum *restrict n, size_t places_rdx) {
n->len += places_rdx;
n->num -= places_rdx;
}









static void bc_num_shift(BcNum *restrict n, BcBigDig dig) {

size_t i, len = n->len;
BcBigDig carry = 0, pow;
BcDig *ptr = n->num;

assert(dig < BC_BASE_DIGS);


pow = bc_num_pow10[dig];
dig = bc_num_pow10[BC_BASE_DIGS - dig];



for (i = len - 1; i < len; --i) {
BcBigDig in, temp;
in = ((BcBigDig) ptr[i]);
temp = carry * dig;
carry = in % pow;
ptr[i] = ((BcDig) (in / pow)) + (BcDig) temp;
}

assert(!carry);
}







static void bc_num_shiftLeft(BcNum *restrict n, size_t places) {

BcBigDig dig;
size_t places_rdx;
bool shift;

if (!places) return;


if (places > n->scale) {
size_t size = bc_vm_growSize(BC_NUM_RDX(places - n->scale), n->len);
if (size > SIZE_MAX - 1) bc_err(BC_ERR_MATH_OVERFLOW);
}


if (BC_NUM_ZERO(n)) {
if (n->scale >= places) n->scale -= places;
else n->scale = 0;
return;
}






dig = (BcBigDig) (places % BC_BASE_DIGS);
shift = (dig != 0);


places_rdx = BC_NUM_RDX(places);





if (n->scale) {

size_t nrdx = BC_NUM_RDX_VAL(n);


if (nrdx >= places_rdx) {

size_t mod = n->scale % BC_BASE_DIGS, revdig;



mod = mod ? mod : BC_BASE_DIGS;


revdig = dig ? BC_BASE_DIGS - dig : 0;


if (mod + revdig > BC_BASE_DIGS) places_rdx = 1;
else places_rdx = 0;
}
else places_rdx -= nrdx;
}


if (places_rdx) {
bc_num_expand(n, bc_vm_growSize(n->len, places_rdx));
memmove(n->num + places_rdx, n->num, BC_NUM_SIZE(n->len));
memset(n->num, 0, BC_NUM_SIZE(places_rdx));
n->len += places_rdx;
}


if (places > n->scale) {
n->scale = 0;
BC_NUM_RDX_SET(n, 0);
}
else {
n->scale -= places;
BC_NUM_RDX_SET(n, BC_NUM_RDX(n->scale));
}


if (shift) bc_num_shift(n, BC_BASE_DIGS - dig);

bc_num_clean(n);
}

void bc_num_shiftRight(BcNum *restrict n, size_t places) {

BcBigDig dig;
size_t places_rdx, scale, scale_mod, int_len, expand;
bool shift;

if (!places) return;


if (BC_NUM_ZERO(n)) {
n->scale += places;
bc_num_expand(n, BC_NUM_RDX(n->scale));
return;
}


dig = (BcBigDig) (places % BC_BASE_DIGS);
shift = (dig != 0);

scale = n->scale;


scale_mod = scale % BC_BASE_DIGS;
scale_mod = scale_mod ? scale_mod : BC_BASE_DIGS;


int_len = bc_num_int(n);
places_rdx = BC_NUM_RDX(places);


if (scale_mod + dig > BC_BASE_DIGS) {
expand = places_rdx - 1;
places_rdx = 1;
}
else {
expand = places_rdx;
places_rdx = 0;
}


if (expand > int_len) expand -= int_len;
else expand = 0;


bc_num_extend(n, places_rdx * BC_BASE_DIGS);
bc_num_expand(n, bc_vm_growSize(expand, n->len));
memset(n->num + n->len, 0, BC_NUM_SIZE(expand));


n->len += expand;
n->scale = 0;
BC_NUM_RDX_SET(n, 0);


if (shift) bc_num_shift(n, dig);

n->scale = scale + places;
BC_NUM_RDX_SET(n, BC_NUM_RDX(n->scale));

bc_num_clean(n);

assert(BC_NUM_RDX_VAL(n) <= n->len && n->len <= n->cap);
assert(BC_NUM_RDX_VAL(n) == BC_NUM_RDX(n->scale));
}







static inline void bc_num_inv(BcNum *a, BcNum *b, size_t scale) {
assert(BC_NUM_NONZERO(a));
bc_num_div(&vm.one, a, b, scale);
}










static bool bc_num_nonInt(const BcNum *restrict n, BcNum *restrict r) {

bool zero;
size_t i, rdx = BC_NUM_RDX_VAL(n);

if (!rdx) {
memcpy(r, n, sizeof(BcNum));
return false;
}

zero = true;

for (i = 0; zero && i < rdx; ++i) zero = (n->num[i] == 0);

if (BC_ERR(!zero)) return true;

bc_num_shiftRdx(n, r);

return false;
}

#if BC_ENABLE_EXTRA_MATH









static BcBigDig bc_num_intop(const BcNum *a, const BcNum *b, BcNum *restrict c)
{
BcNum temp;

if (BC_ERR(bc_num_nonInt(b, &temp))) bc_err(BC_ERR_MATH_NON_INTEGER);

bc_num_copy(c, a);

return bc_num_bigdig(&temp);
}
#endif











static void bc_num_as(BcNum *a, BcNum *b, BcNum *restrict c, size_t sub) {

BcDig *ptr_c, *ptr_l, *ptr_r;
size_t i, min_rdx, max_rdx, diff, a_int, b_int, min_len, max_len, max_int;
size_t len_l, len_r, ardx, brdx;
bool b_neg, do_sub, do_rev_sub, carry, c_neg;

if (BC_NUM_ZERO(b)) {
bc_num_copy(c, a);
return;
}

if (BC_NUM_ZERO(a)) {
bc_num_copy(c, b);
c->rdx = BC_NUM_NEG_VAL(c, BC_NUM_NEG(b) != sub);
return;
}



b_neg = (BC_NUM_NEG(b) != sub);



do_sub = (BC_NUM_NEG(a) != b_neg);

a_int = bc_num_int(a);
b_int = bc_num_int(b);
max_int = BC_MAX(a_int, b_int);



ardx = BC_NUM_RDX_VAL(a);
brdx = BC_NUM_RDX_VAL(b);
min_rdx = BC_MIN(ardx, brdx);
max_rdx = BC_MAX(ardx, brdx);
diff = max_rdx - min_rdx;

max_len = max_int + max_rdx;

if (do_sub) {


if (a_int != b_int) do_rev_sub = (a_int < b_int);
else if (ardx > brdx)
do_rev_sub = (bc_num_compare(a->num + diff, b->num, b->len) < 0);
else
do_rev_sub = (bc_num_compare(a->num, b->num + diff, a->len) <= 0);
}
else {



max_len += 1;
do_rev_sub = (a_int < b_int);
}

assert(max_len <= c->cap);


if (do_rev_sub) {
ptr_l = b->num;
ptr_r = a->num;
len_l = b->len;
len_r = a->len;
}
else {
ptr_l = a->num;
ptr_r = b->num;
len_l = a->len;
len_r = b->len;
}

ptr_c = c->num;
carry = false;



if (diff) {




if ((ardx > brdx) != do_rev_sub) {




memcpy(ptr_c, ptr_l, BC_NUM_SIZE(diff));
ptr_l += diff;
len_l -= diff;
}
else {



if (do_sub) {



for (i = 0; i < diff; i++)
ptr_c[i] = bc_num_subDigits(0, ptr_r[i], &carry);
}
else {


memcpy(ptr_c, ptr_r, BC_NUM_SIZE(diff));
}


ptr_r += diff;
len_r -= diff;
}


ptr_c += diff;
}


min_len = BC_MIN(len_l, len_r);








if (do_sub) {


for (i = 0; i < min_len; ++i)
ptr_c[i] = bc_num_subDigits(ptr_l[i], ptr_r[i], &carry);


for (; i < len_l; ++i) ptr_c[i] = bc_num_subDigits(ptr_l[i], 0, &carry);
}
else {


for (i = 0; i < min_len; ++i)
ptr_c[i] = bc_num_addDigits(ptr_l[i], ptr_r[i], &carry);


for (; i < len_l; ++i) ptr_c[i] = bc_num_addDigits(ptr_l[i], 0, &carry);


ptr_c[i] = bc_num_addDigits(0, 0, &carry);
}

assert(carry == false);



c_neg = BC_NUM_NEG(a) != (do_sub && do_rev_sub);
BC_NUM_RDX_SET_NEG(c, max_rdx, c_neg);
c->len = max_len;
c->scale = BC_MAX(a->scale, b->scale);

bc_num_clean(c);
}









static void bc_num_m_simp(const BcNum *a, const BcNum *b, BcNum *restrict c) {

size_t i, alen = a->len, blen = b->len, clen;
BcDig *ptr_a = a->num, *ptr_b = b->num, *ptr_c;
BcBigDig sum = 0, carry = 0;

assert(sizeof(sum) >= sizeof(BcDig) * 2);
assert(!BC_NUM_RDX_VAL(a) && !BC_NUM_RDX_VAL(b));


clen = bc_vm_growSize(alen, blen);
bc_num_expand(c, bc_vm_growSize(clen, 1));


ptr_c = c->num;
memset(ptr_c, 0, BC_NUM_SIZE(c->cap));





for (i = 0; i < clen; ++i) {

ssize_t sidx = (ssize_t) (i - blen + 1);
size_t j, k;


j = (size_t) BC_MAX(0, sidx);
k = BC_MIN(i, blen - 1);



for (; j < alen && k < blen; ++j, --k) {

sum += ((BcBigDig) ptr_a[j]) * ((BcBigDig) ptr_b[k]);

if (sum >= ((BcBigDig) BC_BASE_POW) * BC_BASE_POW) {
carry += sum / BC_BASE_POW;
sum %= BC_BASE_POW;
}
}


if (sum >= BC_BASE_POW) {
carry += sum / BC_BASE_POW;
sum %= BC_BASE_POW;
}


ptr_c[i] = (BcDig) sum;
assert(ptr_c[i] < BC_BASE_POW);
sum = carry;
carry = 0;
}



assert(!sum);

c->len = clen;
}










static void bc_num_shiftAddSub(BcNum *restrict n, const BcNum *restrict a,
size_t shift, BcNumShiftAddOp op)
{
assert(n->len >= shift + a->len);
assert(!BC_NUM_RDX_VAL(n) && !BC_NUM_RDX_VAL(a));
op(n->num + shift, a->num, a->len);
}




static void bc_num_k(const BcNum *a, const BcNum *b, BcNum *restrict c) {

size_t max, max2, total;
BcNum l1, h1, l2, h2, m2, m1, z0, z1, z2, temp;
BcDig *digs, *dig_ptr;
BcNumShiftAddOp op;
bool aone = BC_NUM_ONE(a);

assert(BC_NUM_ZERO(c));

if (BC_NUM_ZERO(a) || BC_NUM_ZERO(b)) return;

if (aone || BC_NUM_ONE(b)) {
bc_num_copy(c, aone ? b : a);
if ((aone && BC_NUM_NEG(a)) || BC_NUM_NEG(b)) BC_NUM_NEG_TGL(c);
return;
}


if (a->len < BC_NUM_KARATSUBA_LEN || b->len < BC_NUM_KARATSUBA_LEN) {
bc_num_m_simp(a, b, c);
return;
}



max = BC_MAX(a->len, b->len);
max = BC_MAX(max, BC_NUM_DEF_SIZE);
max2 = (max + 1) / 2;



total = bc_vm_arraySize(BC_NUM_KARATSUBA_ALLOCS, max);

BC_SIG_LOCK;


digs = dig_ptr = bc_vm_malloc(BC_NUM_SIZE(total));


bc_num_setup(&l1, dig_ptr, max);
dig_ptr += max;
bc_num_setup(&h1, dig_ptr, max);
dig_ptr += max;
bc_num_setup(&l2, dig_ptr, max);
dig_ptr += max;
bc_num_setup(&h2, dig_ptr, max);
dig_ptr += max;
bc_num_setup(&m1, dig_ptr, max);
dig_ptr += max;
bc_num_setup(&m2, dig_ptr, max);



max = bc_vm_growSize(max, 1);
bc_num_init(&z0, max);
bc_num_init(&z1, max);
bc_num_init(&z2, max);
max = bc_vm_growSize(max, max) + 1;
bc_num_init(&temp, max);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;


bc_num_expand(c, max);
c->len = max;
memset(c->num, 0, BC_NUM_SIZE(c->len));


bc_num_split(a, max2, &l1, &h1);
bc_num_split(b, max2, &l2, &h2);


bc_num_sub(&h1, &l1, &m1, 0);
bc_num_sub(&l2, &h2, &m2, 0);






if (BC_NUM_NONZERO(&h1) && BC_NUM_NONZERO(&h2)) {

assert(BC_NUM_RDX_VALID_NP(h1));
assert(BC_NUM_RDX_VALID_NP(h2));

bc_num_m(&h1, &h2, &z2, 0);
bc_num_clean(&z2);

bc_num_shiftAddSub(c, &z2, max2 * 2, bc_num_addArrays);
bc_num_shiftAddSub(c, &z2, max2, bc_num_addArrays);
}

if (BC_NUM_NONZERO(&l1) && BC_NUM_NONZERO(&l2)) {

assert(BC_NUM_RDX_VALID_NP(l1));
assert(BC_NUM_RDX_VALID_NP(l2));

bc_num_m(&l1, &l2, &z0, 0);
bc_num_clean(&z0);

bc_num_shiftAddSub(c, &z0, max2, bc_num_addArrays);
bc_num_shiftAddSub(c, &z0, 0, bc_num_addArrays);
}

if (BC_NUM_NONZERO(&m1) && BC_NUM_NONZERO(&m2)) {

assert(BC_NUM_RDX_VALID_NP(m1));
assert(BC_NUM_RDX_VALID_NP(m1));

bc_num_m(&m1, &m2, &z1, 0);
bc_num_clean(&z1);

op = (BC_NUM_NEG_NP(m1) != BC_NUM_NEG_NP(m2)) ?
bc_num_subArrays : bc_num_addArrays;
bc_num_shiftAddSub(c, &z1, max2, op);
}

err:
BC_SIG_MAYLOCK;
free(digs);
bc_num_free(&temp);
bc_num_free(&z2);
bc_num_free(&z1);
bc_num_free(&z0);
BC_LONGJMP_CONT;
}










static void bc_num_m(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale) {

BcNum cpa, cpb;
size_t ascale, bscale, ardx, brdx, azero = 0, bzero = 0, zero, len, rscale;

assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));

bc_num_zero(c);

ascale = a->scale;
bscale = b->scale;


scale = BC_MAX(scale, ascale);
scale = BC_MAX(scale, bscale);
rscale = ascale + bscale;
scale = BC_MIN(rscale, scale);



if ((a->len == 1 || b->len == 1) && !a->rdx && !b->rdx) {

BcNum *operand;
BcBigDig dig;


if (a->len == 1) {
dig = (BcBigDig) a->num[0];
operand = b;
}
else {
dig = (BcBigDig) b->num[0];
operand = a;
}

bc_num_mulArray(operand, dig, c);


if (BC_NUM_NONZERO(c))
c->rdx = BC_NUM_NEG_VAL(c, BC_NUM_NEG(a) != BC_NUM_NEG(b));

return;
}

assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));

BC_SIG_LOCK;



bc_num_init(&cpa, a->len + BC_NUM_RDX_VAL(a));
bc_num_init(&cpb, b->len + BC_NUM_RDX_VAL(b));

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;

bc_num_copy(&cpa, a);
bc_num_copy(&cpb, b);

assert(BC_NUM_RDX_VALID_NP(cpa));
assert(BC_NUM_RDX_VALID_NP(cpb));

BC_NUM_NEG_CLR_NP(cpa);
BC_NUM_NEG_CLR_NP(cpb);

assert(BC_NUM_RDX_VALID_NP(cpa));
assert(BC_NUM_RDX_VALID_NP(cpb));


ardx = BC_NUM_RDX_VAL_NP(cpa) * BC_BASE_DIGS;
bc_num_shiftLeft(&cpa, ardx);

brdx = BC_NUM_RDX_VAL_NP(cpb) * BC_BASE_DIGS;
bc_num_shiftLeft(&cpb, brdx);




BC_SIG_LOCK;

BC_UNSETJMP;


azero = bc_num_shiftZero(&cpa);
bzero = bc_num_shiftZero(&cpb);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;

bc_num_clean(&cpa);
bc_num_clean(&cpb);

bc_num_k(&cpa, &cpb, c);




zero = bc_vm_growSize(azero, bzero);
len = bc_vm_growSize(c->len, zero);

bc_num_expand(c, len);


bc_num_shiftLeft(c, (len - c->len) * BC_BASE_DIGS);
bc_num_shiftRight(c, ardx + brdx);

bc_num_retireMul(c, scale, BC_NUM_NEG(a), BC_NUM_NEG(b));

err:
BC_SIG_MAYLOCK;
bc_num_unshiftZero(&cpb, bzero);
bc_num_free(&cpb);
bc_num_unshiftZero(&cpa, azero);
bc_num_free(&cpa);
BC_LONGJMP_CONT;
}







static bool bc_num_nonZeroDig(BcDig *restrict a, size_t len) {
size_t i;
bool nonzero = false;
for (i = len - 1; !nonzero && i < len; --i) nonzero = (a[i] != 0);
return nonzero;
}










static ssize_t bc_num_divCmp(const BcDig *a, const BcNum *b, size_t len) {

ssize_t cmp;

if (b->len > len && a[len]) cmp = bc_num_compare(a, b->num, len + 1);
else if (b->len <= len) {
if (a[len]) cmp = 1;
else cmp = bc_num_compare(a, b->num, len);
}
else cmp = -1;

return cmp;
}









static void bc_num_divExtend(BcNum *restrict a, BcNum *restrict b,
BcBigDig divisor)
{
size_t pow;

assert(divisor < BC_BASE_POW);

pow = BC_BASE_DIGS - bc_num_log10((size_t) divisor);

bc_num_shiftLeft(a, pow);
bc_num_shiftLeft(b, pow);
}









static void bc_num_d_long(BcNum *restrict a, BcNum *restrict b,
BcNum *restrict c, size_t scale)
{
BcBigDig divisor;
size_t len, end, i, rdx;
BcNum cpb;
bool nonzero = false;

assert(b->len < a->len);

len = b->len;
end = a->len - len;

assert(len >= 1);



bc_num_expand(c, a->len);
memset(c->num, 0, c->cap * sizeof(BcDig));


BC_NUM_RDX_SET(c, BC_NUM_RDX_VAL(a));
c->scale = a->scale;
c->len = a->len;



divisor = (BcBigDig) b->num[len - 1];



if (len > 1 && bc_num_nonZeroDig(b->num, len - 1)) {







nonzero = (divisor > 1 << ((10 * BC_BASE_DIGS) / 6 + 1));


if (!nonzero) {



bc_num_divExtend(a, b, divisor);



len = BC_MAX(a->len, b->len);
bc_num_expand(a, len + 1);


if (len + 1 > a->len) a->len = len + 1;



len = b->len;
end = a->len - len;
divisor = (BcBigDig) b->num[len - 1];

nonzero = bc_num_nonZeroDig(b->num, len - 1);
}
}



divisor += nonzero;


bc_num_expand(c, a->len);
memset(c->num, 0, BC_NUM_SIZE(c->cap));

assert(c->scale >= scale);
rdx = BC_NUM_RDX_VAL(c) - BC_NUM_RDX(scale);

BC_SIG_LOCK;

bc_num_init(&cpb, len + 1);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;


for (i = end - 1; i < end && i >= rdx && BC_NUM_NONZERO(a); --i) {

ssize_t cmp;
BcDig *n;
BcBigDig result;

n = a->num + i;
assert(n >= a->num);
result = 0;

cmp = bc_num_divCmp(n, b, len);




while (cmp >= 0) {

BcBigDig n1, dividend, quotient;



n1 = (BcBigDig) n[len];
dividend = n1 * BC_BASE_POW + (BcBigDig) n[len - 1];
quotient = (dividend / divisor);




if (quotient <= 1) {
quotient = 1;
bc_num_subArrays(n, b->num, len);
}
else {

assert(quotient <= BC_BASE_POW);


bc_num_mulArray(b, (BcBigDig) quotient, &cpb);
bc_num_subArrays(n, cpb.num, cpb.len);
}



result += quotient;
assert(result <= BC_BASE_POW);




if (nonzero) cmp = bc_num_divCmp(n, b, len);
else cmp = -1;
}

assert(result < BC_BASE_POW);


c->num[i] = (BcDig) result;
}

err:
BC_SIG_MAYLOCK;
bc_num_free(&cpb);
BC_LONGJMP_CONT;
}








static void bc_num_d(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale) {

size_t len, cpardx;
BcNum cpa, cpb;

if (BC_NUM_ZERO(b)) bc_err(BC_ERR_MATH_DIVIDE_BY_ZERO);

if (BC_NUM_ZERO(a)) {
bc_num_setToZero(c, scale);
return;
}

if (BC_NUM_ONE(b)) {
bc_num_copy(c, a);
bc_num_retireMul(c, scale, BC_NUM_NEG(a), BC_NUM_NEG(b));
return;
}


if (!BC_NUM_RDX_VAL(a) && !BC_NUM_RDX_VAL(b) && b->len == 1 && !scale) {
BcBigDig rem;
bc_num_divArray(a, (BcBigDig) b->num[0], c, &rem);
bc_num_retireMul(c, scale, BC_NUM_NEG(a), BC_NUM_NEG(b));
return;
}

len = bc_num_divReq(a, b, scale);

BC_SIG_LOCK;




bc_num_init(&cpa, len);
bc_num_copy(&cpa, a);
bc_num_createCopy(&cpb, b);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;

len = b->len;



if (len > cpa.len) {
bc_num_expand(&cpa, bc_vm_growSize(len, 2));
bc_num_extend(&cpa, (len - cpa.len) * BC_BASE_DIGS);
}

cpardx = BC_NUM_RDX_VAL_NP(cpa);
cpa.scale = cpardx * BC_BASE_DIGS;


bc_num_extend(&cpa, b->scale);
cpardx = BC_NUM_RDX_VAL_NP(cpa) - BC_NUM_RDX(b->scale);
BC_NUM_RDX_SET_NP(cpa, cpardx);
cpa.scale = cpardx * BC_BASE_DIGS;


if (scale > cpa.scale) {
bc_num_extend(&cpa, scale);
cpardx = BC_NUM_RDX_VAL_NP(cpa);
cpa.scale = cpardx * BC_BASE_DIGS;
}


if (cpa.cap == cpa.len) bc_num_expand(&cpa, bc_vm_growSize(cpa.len, 1));


cpa.num[cpa.len++] = 0;






if (cpardx == cpa.len) cpa.len = bc_num_nonZeroLen(&cpa);
if (BC_NUM_RDX_VAL_NP(cpb) == cpb.len) cpb.len = bc_num_nonZeroLen(&cpb);
cpb.scale = 0;
BC_NUM_RDX_SET_NP(cpb, 0);

bc_num_d_long(&cpa, &cpb, c, scale);

bc_num_retireMul(c, scale, BC_NUM_NEG(a), BC_NUM_NEG(b));

err:
BC_SIG_MAYLOCK;
bc_num_free(&cpb);
bc_num_free(&cpa);
BC_LONGJMP_CONT;
}













static void bc_num_r(BcNum *a, BcNum *b, BcNum *restrict c,
BcNum *restrict d, size_t scale, size_t ts)
{
BcNum temp;
bool neg;

if (BC_NUM_ZERO(b)) bc_err(BC_ERR_MATH_DIVIDE_BY_ZERO);

if (BC_NUM_ZERO(a)) {
bc_num_setToZero(c, ts);
bc_num_setToZero(d, ts);
return;
}

BC_SIG_LOCK;

bc_num_init(&temp, d->cap);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;


bc_num_d(a, b, c, scale);


if (scale) scale = ts + 1;

assert(BC_NUM_RDX_VALID(c));
assert(BC_NUM_RDX_VALID(b));


bc_num_m(c, b, &temp, scale);
bc_num_sub(a, &temp, d, scale);


if (ts > d->scale && BC_NUM_NONZERO(d)) bc_num_extend(d, ts - d->scale);

neg = BC_NUM_NEG(d);
bc_num_retireMul(d, ts, BC_NUM_NEG(a), BC_NUM_NEG(b));
d->rdx = BC_NUM_NEG_VAL(d, BC_NUM_NONZERO(d) ? neg : false);

err:
BC_SIG_MAYLOCK;
bc_num_free(&temp);
BC_LONGJMP_CONT;
}









static void bc_num_rem(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale) {

BcNum c1;
size_t ts;

ts = bc_vm_growSize(scale, b->scale);
ts = BC_MAX(ts, a->scale);

BC_SIG_LOCK;


bc_num_init(&c1, bc_num_mulReq(a, b, ts));

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;

bc_num_r(a, b, &c1, c, scale, ts);

err:
BC_SIG_MAYLOCK;
bc_num_free(&c1);
BC_LONGJMP_CONT;
}








static void bc_num_p(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale) {

BcNum copy, btemp;
BcBigDig exp;
size_t powrdx, resrdx;
bool neg;

if (BC_ERR(bc_num_nonInt(b, &btemp))) bc_err(BC_ERR_MATH_NON_INTEGER);

if (BC_NUM_ZERO(&btemp)) {
bc_num_one(c);
return;
}

if (BC_NUM_ZERO(a)) {
if (BC_NUM_NEG_NP(btemp)) bc_err(BC_ERR_MATH_DIVIDE_BY_ZERO);
bc_num_setToZero(c, scale);
return;
}

if (BC_NUM_ONE(&btemp)) {
if (!BC_NUM_NEG_NP(btemp)) bc_num_copy(c, a);
else bc_num_inv(a, c, scale);
return;
}

neg = BC_NUM_NEG_NP(btemp);
BC_NUM_NEG_CLR_NP(btemp);

exp = bc_num_bigdig(&btemp);

BC_SIG_LOCK;

bc_num_createCopy(&copy, a);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;



if (!neg) {
size_t max = BC_MAX(scale, a->scale), scalepow;
scalepow = bc_num_mulOverflow(a->scale, exp);
scale = BC_MIN(scalepow, max);
}



for (powrdx = a->scale; !(exp & 1); exp >>= 1) {
powrdx <<= 1;
assert(BC_NUM_RDX_VALID_NP(copy));
bc_num_mul(&copy, &copy, &copy, powrdx);
}



bc_num_copy(c, &copy);
resrdx = powrdx;



while (exp >>= 1) {

powrdx <<= 1;
assert(BC_NUM_RDX_VALID_NP(copy));
bc_num_mul(&copy, &copy, &copy, powrdx);



if (exp & 1) {
resrdx += powrdx;
assert(BC_NUM_RDX_VALID(c));
assert(BC_NUM_RDX_VALID_NP(copy));
bc_num_mul(c, &copy, c, resrdx);
}
}


if (neg) bc_num_inv(c, c, scale);


if (c->scale > scale) bc_num_truncate(c, c->scale - scale);

bc_num_clean(c);

err:
BC_SIG_MAYLOCK;
bc_num_free(&copy);
BC_LONGJMP_CONT;
}

#if BC_ENABLE_EXTRA_MATH







static void bc_num_place(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale) {

BcBigDig val;

BC_UNUSED(scale);

val = bc_num_intop(a, b, c);


if (val < c->scale) bc_num_truncate(c, c->scale - val);
else if (val > c->scale) bc_num_extend(c, val - c->scale);
}




static void bc_num_left(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale) {

BcBigDig val;

BC_UNUSED(scale);

val = bc_num_intop(a, b, c);

bc_num_shiftLeft(c, (size_t) val);
}




static void bc_num_right(BcNum *a, BcNum *b, BcNum *restrict c, size_t scale) {

BcBigDig val;

BC_UNUSED(scale);

val = bc_num_intop(a, b, c);

if (BC_NUM_ZERO(c)) return;

bc_num_shiftRight(c, (size_t) val);
}
#endif



















static void bc_num_binary(BcNum *a, BcNum *b, BcNum *c, size_t scale,
BcNumBinOp op, size_t req)
{
BcNum *ptr_a, *ptr_b, num2;
bool init = false;

assert(a != NULL && b != NULL && c != NULL && op != NULL);

assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));

BC_SIG_LOCK;


if (c == a) {

ptr_a = &num2;

memcpy(ptr_a, c, sizeof(BcNum));
init = true;
}
else {
ptr_a = a;
}


if (c == b) {

ptr_b = &num2;

if (c != a) {
memcpy(ptr_b, c, sizeof(BcNum));
init = true;
}
}
else {
ptr_b = b;
}



if (init) {

bc_num_init(c, req);



BC_SETJMP_LOCKED(err);
BC_SIG_UNLOCK;
}
else {
BC_SIG_UNLOCK;
bc_num_expand(c, req);
}






op(ptr_a, ptr_b, c, scale);

assert(!BC_NUM_NEG(c) || BC_NUM_NONZERO(c));
assert(BC_NUM_RDX_VAL(c) <= c->len || !c->len);
assert(BC_NUM_RDX_VALID(c));
assert(!c->len || c->num[c->len - 1] || BC_NUM_RDX_VAL(c) == c->len);

err:

if (init) {
BC_SIG_MAYLOCK;
bc_num_free(&num2);
BC_LONGJMP_CONT;
}
}

#if !defined(NDEBUG) || BC_ENABLE_LIBRARY










bool bc_num_strValid(const char *restrict val) {

bool radix = false;
size_t i, len = strlen(val);






if (!len) return true;


for (i = 0; i < len; ++i) {

BcDig c = val[i];


if (c == '.') {


if (radix) return false;

radix = true;
continue;
}


if (!(isdigit(c) || isupper(c))) return false;
}

return true;
}
#endif








static BcBigDig bc_num_parseChar(char c, size_t base) {

assert(isupper(c) || isdigit(c));


if (isupper(c)) {


c = BC_NUM_NUM_LETTER(c);


c = ((size_t) c) >= base ? (char) base - 1 : c;
}

else c -= '0';

return (BcBigDig) (uchar) c;
}







static void bc_num_parseDecimal(BcNum *restrict n, const char *restrict val) {

size_t len, i, temp, mod;
const char *ptr;
bool zero = true, rdx;


for (i = 0; val[i] == '0'; ++i);

val += i;
assert(!val[0] || isalnum(val[0]) || val[0] == '.');



if (!val[0]) return;



len = strlen(val);


ptr = strchr(val, '.');
rdx = (ptr != NULL);




for (i = 0; i < len && (zero = (val[i] == '0' || val[i] == '.')); ++i);




n->scale = (size_t) (rdx * (((uintptr_t) (val + len)) -
(((uintptr_t) ptr) + 1)));


BC_NUM_RDX_SET(n, BC_NUM_RDX(n->scale));



i = len - (ptr == val ? 0 : i) - rdx;
temp = BC_NUM_ROUND_POW(i);
mod = n->scale % BC_BASE_DIGS;
i = mod ? BC_BASE_DIGS - mod : 0;
n->len = ((temp + i) / BC_BASE_DIGS);


bc_num_expand(n, n->len);
memset(n->num, 0, BC_NUM_SIZE(n->len));

if (zero) {


n->len = n->rdx = 0;
}
else {


BcBigDig exp, pow;

assert(i <= BC_NUM_BIGDIG_MAX);


exp = (BcBigDig) i;
pow = bc_num_pow10[exp];



for (i = len - 1; i < len; --i, ++exp) {

char c = val[i];


if (c == '.') exp -= 1;
else {


size_t idx = exp / BC_BASE_DIGS;


if (isupper(c)) c = '9';


n->num[idx] += (((BcBigDig) c) - '0') * pow;


if ((exp + 1) % BC_BASE_DIGS == 0) pow = 1;
else pow *= BC_BASE;
}
}
}
}







static void bc_num_parseBase(BcNum *restrict n, const char *restrict val,
BcBigDig base)
{
BcNum temp, mult1, mult2, result1, result2, *m1, *m2, *ptr;
char c = 0;
bool zero = true;
BcBigDig v;
size_t i, digs, len = strlen(val);


for (i = 0; zero && i < len; ++i) zero = (val[i] == '.' || val[i] == '0');
if (zero) return;

BC_SIG_LOCK;

bc_num_init(&temp, BC_NUM_BIGDIG_LOG10);
bc_num_init(&mult1, BC_NUM_BIGDIG_LOG10);

BC_SETJMP_LOCKED(int_err);

BC_SIG_UNLOCK;






for (i = 0; i < len && (c = val[i]) && c != '.'; ++i) {


v = bc_num_parseChar(c, base);


bc_num_mulArray(n, base, &mult1);


bc_num_bigdig2num(&temp, v);
bc_num_add(&mult1, &temp, n, 0);
}



if (i == len && !val[i]) goto int_err;


assert(val[i] == '.');

BC_SIG_LOCK;


BC_UNSETJMP;

bc_num_init(&mult2, BC_NUM_BIGDIG_LOG10);
bc_num_init(&result1, BC_NUM_DEF_SIZE);
bc_num_init(&result2, BC_NUM_DEF_SIZE);
bc_num_one(&mult1);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;


m1 = &mult1;
m2 = &mult2;


for (i += 1, digs = 0; i < len && (c = val[i]); ++i, ++digs) {

size_t rdx;


v = bc_num_parseChar(c, base);




bc_num_mulArray(&result1, base, &result2);


bc_num_bigdig2num(&temp, v);


bc_num_add(&result2, &temp, &result1, 0);


bc_num_mulArray(m1, base, m2);

rdx = BC_NUM_RDX_VAL(m2);

if (m2->len < rdx) m2->len = rdx;


ptr = m1;
m1 = m2;
m2 = ptr;
}





bc_num_div(&result1, m1, &result2, digs * 2);


bc_num_truncate(&result2, digs);


bc_num_add(n, &result2, n, digs);


if (BC_NUM_NONZERO(n)) {
if (n->scale < digs) bc_num_extend(n, digs - n->scale);
}
else bc_num_zero(n);

err:
BC_SIG_MAYLOCK;
bc_num_free(&result2);
bc_num_free(&result1);
bc_num_free(&mult2);
int_err:
BC_SIG_MAYLOCK;
bc_num_free(&mult1);
bc_num_free(&temp);
BC_LONGJMP_CONT;
}





static inline void bc_num_printNewline(void) {
#if !BC_ENABLE_LIBRARY
if (vm.nchars >= vm.line_len - 1 && vm.line_len) {
bc_vm_putchar('\\', bc_flush_none);
bc_vm_putchar('\n', bc_flush_err);
}
#endif
}






static void bc_num_putchar(int c, bool bslash) {
if (c != '\n' && bslash) bc_num_printNewline();
bc_vm_putchar(c, bc_flush_save);
}

#if !BC_ENABLE_LIBRARY












static void bc_num_printChar(size_t n, size_t len, bool rdx, bool bslash) {
BC_UNUSED(rdx);
BC_UNUSED(len);
BC_UNUSED(bslash);
assert(len == 1);
bc_vm_putchar((uchar) n, bc_flush_save);
}

#endif











static void bc_num_printDigits(size_t n, size_t len, bool rdx, bool bslash) {

size_t exp, pow;


bc_num_putchar(rdx ? '.' : ' ', true);


for (exp = 0, pow = 1; exp < len - 1; ++exp, pow *= BC_BASE);


for (exp = 0; exp < len; pow /= BC_BASE, ++exp) {


size_t dig = n / pow;


n -= dig * pow;


bc_num_putchar(((uchar) dig) + '0', bslash || exp != len - 1);
}
}












static void bc_num_printHex(size_t n, size_t len, bool rdx, bool bslash) {

BC_UNUSED(len);
BC_UNUSED(bslash);

assert(len == 1);

if (rdx) bc_num_putchar('.', true);

bc_num_putchar(bc_num_hex_digits[n], bslash);
}







static void bc_num_printDecimal(const BcNum *restrict n, bool newline) {

size_t i, j, rdx = BC_NUM_RDX_VAL(n);
bool zero = true;
size_t buffer[BC_BASE_DIGS];


for (i = n->len - 1; i < n->len; --i) {

BcDig n9 = n->num[i];
size_t temp;
bool irdx = (i == rdx - 1);


zero = (zero & !irdx);
temp = n->scale % BC_BASE_DIGS;
temp = i || !temp ? 0 : BC_BASE_DIGS - temp;

memset(buffer, 0, BC_BASE_DIGS * sizeof(size_t));


for (j = 0; n9 && j < BC_BASE_DIGS; ++j) {
buffer[j] = ((size_t) n9) % BC_BASE;
n9 /= BC_BASE;
}


for (j = BC_BASE_DIGS - 1; j < BC_BASE_DIGS && j >= temp; --j) {


bool print_rdx = (irdx & (j == BC_BASE_DIGS - 1));


zero = (zero && buffer[j] == 0);

if (!zero) {






bc_num_printHex(buffer[j], 1, print_rdx,
!newline || (j > temp || i != 0));
}
}
}
}

#if BC_ENABLE_EXTRA_MATH








static void bc_num_printExponent(const BcNum *restrict n,
bool eng, bool newline)
{
size_t places, mod, nrdx = BC_NUM_RDX_VAL(n);
bool neg = (n->len <= nrdx);
BcNum temp, exp;
BcDig digs[BC_NUM_BIGDIG_LOG10];

BC_SIG_LOCK;

bc_num_createCopy(&temp, n);

BC_SETJMP_LOCKED(exit);

BC_SIG_UNLOCK;



if (neg) {


size_t i, idx = bc_num_nonZeroLen(n) - 1;

places = 1;


for (i = BC_BASE_DIGS - 1; i < BC_BASE_DIGS; --i) {
if (bc_num_pow10[i] > (BcBigDig) n->num[idx]) places += 1;
else break;
}



places += (nrdx - (idx + 1)) * BC_BASE_DIGS;
mod = places % 3;


if (eng && mod != 0) places += 3 - mod;


bc_num_shiftLeft(&temp, places);
}
else {



places = bc_num_intDigits(n) - 1;



mod = places % 3;
if (eng && mod != 0) places -= 3 - (3 - mod);


bc_num_shiftRight(&temp, places);
}


bc_num_printDecimal(&temp, newline);


bc_num_putchar('e', !newline);


if (!places) {
bc_num_printHex(0, 1, false, !newline);
goto exit;
}


if (neg) bc_num_putchar('-', true);


bc_num_setup(&exp, digs, BC_NUM_BIGDIG_LOG10);
bc_num_bigdig2num(&exp, (BcBigDig) places);


bc_num_printDecimal(&exp, newline);

exit:
BC_SIG_MAYLOCK;
bc_num_free(&temp);
BC_LONGJMP_CONT;
}
#endif











static void bc_num_printFixup(BcNum *restrict n, BcBigDig rem,
BcBigDig pow, size_t idx)
{
size_t i, len = n->len - idx;
BcBigDig acc;
BcDig *a = n->num + idx;



if (len < 2) return;



for (i = len - 1; i > 0; --i) {




acc = ((BcBigDig) a[i]) * rem + ((BcBigDig) a[i - 1]);


a[i - 1] = (BcDig) (acc % pow);


acc /= pow;
acc += (BcBigDig) a[i];


if (acc >= BC_BASE_POW) {


if (i == len - 1) {


len = bc_vm_growSize(len, 1);
bc_num_expand(n, bc_vm_growSize(len, idx));


a = n->num + idx;


a[len - 1] = 0;
}


a[i + 1] += acc / BC_BASE_POW;
acc %= BC_BASE_POW;
}

assert(acc < BC_BASE_POW);


a[i] = (BcDig) acc;
}


n->len = len + idx;
}









static void bc_num_printPrepare(BcNum *restrict n, BcBigDig rem, BcBigDig pow) {

size_t i;



for (i = 0; i < n->len; ++i) bc_num_printFixup(n, rem, pow, i);




for (i = 0; i < n->len; ++i) {

assert(pow == ((BcBigDig) ((BcDig) pow)));


if (n->num[i] >= (BcDig) pow) {


if (i + 1 == n->len) {


n->len = bc_vm_growSize(n->len, 1);
bc_num_expand(n, n->len);


n->num[i + 1] = 0;
}

assert(pow < BC_BASE_POW);


n->num[i + 1] += n->num[i] / ((BcDig) pow);
n->num[i] %= (BcDig) pow;
}
}
}

static void bc_num_printNum(BcNum *restrict n, BcBigDig base, size_t len,
BcNumDigitOp print, bool newline)
{
BcVec stack;
BcNum intp, fracp1, fracp2, digit, flen1, flen2, *n1, *n2, *temp;
BcBigDig dig = 0, *ptr, acc, exp;
size_t i, j, nrdx, idigits;
bool radix;
BcDig digit_digs[BC_NUM_BIGDIG_LOG10 + 1];

assert(base > 1);


if (BC_NUM_ZERO(n)) {
print(0, len, false, !newline);
return;
}







































nrdx = BC_NUM_RDX_VAL(n);

BC_SIG_LOCK;


bc_vec_init(&stack, sizeof(BcBigDig), BC_DTOR_NONE);
bc_num_init(&fracp1, nrdx);


bc_num_createCopy(&intp, n);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;


bc_num_truncate(&intp, intp.scale);


bc_num_sub(n, &intp, &fracp1, 0);






if (base != vm.last_base) {

vm.last_pow = 1;
vm.last_exp = 0;


while (vm.last_pow * base <= BC_BASE_POW) {
vm.last_pow *= base;
vm.last_exp += 1;
}


vm.last_rem = BC_BASE_POW - vm.last_pow;
vm.last_base = base;
}

exp = vm.last_exp;





if (vm.last_rem != 0) bc_num_printPrepare(&intp, vm.last_rem, vm.last_pow);





for (i = 0; i < intp.len; ++i) {


acc = (BcBigDig) intp.num[i];


for (j = 0; j < exp && (i < intp.len - 1 || acc != 0); ++j)
{

if (j != exp - 1) {
dig = acc % base;
acc /= base;
}
else {
dig = acc;
acc = 0;
}

assert(dig < base);


bc_vec_push(&stack, &dig);
}

assert(acc == 0);
}


for (i = 0; i < stack.len; ++i) {

ptr = bc_vec_item_rev(&stack, i);

assert(ptr != NULL);






print(*ptr, len, false, !newline ||
(n->scale != 0 || i == stack.len - 1));
}


if (!n->scale) goto err;

BC_SIG_LOCK;


BC_UNSETJMP;

bc_num_init(&fracp2, nrdx);
bc_num_setup(&digit, digit_digs, sizeof(digit_digs) / sizeof(BcDig));
bc_num_init(&flen1, BC_NUM_BIGDIG_LOG10);
bc_num_init(&flen2, BC_NUM_BIGDIG_LOG10);

BC_SETJMP_LOCKED(frac_err);

BC_SIG_UNLOCK;

bc_num_one(&flen1);

radix = true;


n1 = &flen1;
n2 = &flen2;

fracp2.scale = n->scale;
BC_NUM_RDX_SET_NP(fracp2, BC_NUM_RDX(fracp2.scale));


while ((idigits = bc_num_intDigits(n1)) <= n->scale) {


bc_num_expand(&fracp2, fracp1.len + 1);
bc_num_mulArray(&fracp1, base, &fracp2);

nrdx = BC_NUM_RDX_VAL_NP(fracp2);


if (fracp2.len < nrdx) fracp2.len = nrdx;


dig = bc_num_bigdig2(&fracp2);


bc_num_bigdig2num(&digit, dig);
bc_num_sub(&fracp2, &digit, &fracp1, 0);






print(dig, len, radix, !newline || idigits != n->scale);


bc_num_mulArray(n1, base, n2);

radix = false;


temp = n1;
n1 = n2;
n2 = temp;
}

frac_err:
BC_SIG_MAYLOCK;
bc_num_free(&flen2);
bc_num_free(&flen1);
bc_num_free(&fracp2);
err:
BC_SIG_MAYLOCK;
bc_num_free(&fracp1);
bc_num_free(&intp);
bc_vec_free(&stack);
BC_LONGJMP_CONT;
}








static void bc_num_printBase(BcNum *restrict n, BcBigDig base, bool newline) {

size_t width;
BcNumDigitOp print;
bool neg = BC_NUM_NEG(n);



BC_NUM_NEG_CLR(n);



if (base <= BC_NUM_MAX_POSIX_IBASE) {
width = 1;
print = bc_num_printHex;
}
else {
assert(base <= BC_BASE_POW);
width = bc_num_log10(base - 1);
print = bc_num_printDigits;
}


bc_num_printNum(n, base, width, print, newline);


n->rdx = BC_NUM_NEG_VAL(n, neg);
}

#if !BC_ENABLE_LIBRARY

void bc_num_stream(BcNum *restrict n) {
bc_num_printNum(n, BC_NUM_STREAM_BASE, 1, bc_num_printChar, false);
}

#endif

void bc_num_setup(BcNum *restrict n, BcDig *restrict num, size_t cap) {
assert(n != NULL);
n->num = num;
n->cap = cap;
bc_num_zero(n);
}

void bc_num_init(BcNum *restrict n, size_t req) {

BcDig *num;

BC_SIG_ASSERT_LOCKED;

assert(n != NULL);



req = req >= BC_NUM_DEF_SIZE ? req : BC_NUM_DEF_SIZE;


if (req != BC_NUM_DEF_SIZE || (num = bc_vm_takeTemp()) == NULL)
num = bc_vm_malloc(BC_NUM_SIZE(req));

bc_num_setup(n, num, req);
}

void bc_num_clear(BcNum *restrict n) {
n->num = NULL;
n->cap = 0;
}

void bc_num_free(void *num) {

BcNum *n = (BcNum*) num;

BC_SIG_ASSERT_LOCKED;

assert(n != NULL);

if (n->cap == BC_NUM_DEF_SIZE) bc_vm_addTemp(n->num);
else free(n->num);
}

void bc_num_copy(BcNum *d, const BcNum *s) {

assert(d != NULL && s != NULL);

if (d == s) return;

bc_num_expand(d, s->len);
d->len = s->len;



d->rdx = s->rdx;
d->scale = s->scale;
memcpy(d->num, s->num, BC_NUM_SIZE(d->len));
}

void bc_num_createCopy(BcNum *d, const BcNum *s) {
BC_SIG_ASSERT_LOCKED;
bc_num_init(d, s->len);
bc_num_copy(d, s);
}

void bc_num_createFromBigdig(BcNum *restrict n, BcBigDig val) {
BC_SIG_ASSERT_LOCKED;
bc_num_init(n, BC_NUM_BIGDIG_LOG10);
bc_num_bigdig2num(n, val);
}

size_t bc_num_scale(const BcNum *restrict n) {
return n->scale;
}

size_t bc_num_len(const BcNum *restrict n) {

size_t len = n->len;


if (BC_NUM_ZERO(n)) return n->scale ? n->scale : 1;


if (BC_NUM_RDX_VAL(n) == len) {





size_t zero, scale;


len = bc_num_nonZeroLen(n);


scale = n->scale % BC_BASE_DIGS;
scale = scale ? scale : BC_BASE_DIGS;


zero = bc_num_zeroDigits(n->num + len - 1);


len = len * BC_BASE_DIGS - zero - (BC_BASE_DIGS - scale);
}

else len = bc_num_intDigits(n) + n->scale;

return len;
}

void bc_num_parse(BcNum *restrict n, const char *restrict val, BcBigDig base) {

assert(n != NULL && val != NULL && base);
assert(base >= BC_NUM_MIN_BASE && base <= vm.maxes[BC_PROG_GLOBALS_IBASE]);
assert(bc_num_strValid(val));



if (!val[1]) {
BcBigDig dig = bc_num_parseChar(val[0], BC_NUM_MAX_LBASE);
bc_num_bigdig2num(n, dig);
}
else if (base == BC_BASE) bc_num_parseDecimal(n, val);
else bc_num_parseBase(n, val, base);

assert(BC_NUM_RDX_VALID(n));
}

void bc_num_print(BcNum *restrict n, BcBigDig base, bool newline) {

assert(n != NULL);
assert(BC_ENABLE_EXTRA_MATH || base >= BC_NUM_MIN_BASE);


bc_num_printNewline();

if (BC_NUM_NONZERO(n)) {


if (BC_NUM_NEG(n)) bc_num_putchar('-', true);


if (BC_Z && BC_NUM_RDX_VAL(n) == n->len)
bc_num_printHex(0, 1, false, !newline);
}


if (BC_NUM_ZERO(n)) bc_num_printHex(0, 1, false, !newline);
else if (base == BC_BASE) bc_num_printDecimal(n, newline);
#if BC_ENABLE_EXTRA_MATH
else if (base == 0 || base == 1)
bc_num_printExponent(n, base != 0, newline);
#endif
else bc_num_printBase(n, base, newline);

if (newline) bc_num_putchar('\n', false);
}

BcBigDig bc_num_bigdig2(const BcNum *restrict n) {







BcBigDig r = 0;
size_t nrdx = BC_NUM_RDX_VAL(n);

assert(n != NULL);
assert(!BC_NUM_NEG(n));
assert(bc_num_cmp(n, &vm.max) < 0);
assert(n->len - nrdx <= 3);



switch (n->len - nrdx) {

case 3:
{
r = (BcBigDig) n->num[nrdx + 2];
}

BC_FALLTHROUGH

case 2:
{
r = r * BC_BASE_POW + (BcBigDig) n->num[nrdx + 1];
}

BC_FALLTHROUGH

case 1:
{
r = r * BC_BASE_POW + (BcBigDig) n->num[nrdx];
}
}

return r;
}

BcBigDig bc_num_bigdig(const BcNum *restrict n) {

assert(n != NULL);






if (BC_ERR(BC_NUM_NEG(n))) bc_err(BC_ERR_MATH_NEGATIVE);
if (BC_ERR(bc_num_cmp(n, &vm.max) >= 0)) bc_err(BC_ERR_MATH_OVERFLOW);

return bc_num_bigdig2(n);
}

void bc_num_bigdig2num(BcNum *restrict n, BcBigDig val) {

BcDig *ptr;
size_t i;

assert(n != NULL);

bc_num_zero(n);


if (!val) return;



bc_num_expand(n, BC_NUM_BIGDIG_LOG10);



for (ptr = n->num, i = 0; val; ++i, val /= BC_BASE_POW)
ptr[i] = val % BC_BASE_POW;

n->len = i;
}

#if BC_ENABLE_EXTRA_MATH

void bc_num_rng(const BcNum *restrict n, BcRNG *rng) {

BcNum temp, temp2, intn, frac;
BcRand state1, state2, inc1, inc2;
size_t nrdx = BC_NUM_RDX_VAL(n);






BC_SIG_LOCK;

bc_num_init(&temp, n->len);
bc_num_init(&temp2, n->len);
bc_num_init(&frac, nrdx);
bc_num_init(&intn, bc_num_int(n));

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;

assert(BC_NUM_RDX_VALID_NP(vm.max));

memcpy(frac.num, n->num, BC_NUM_SIZE(nrdx));
frac.len = nrdx;
BC_NUM_RDX_SET_NP(frac, nrdx);
frac.scale = n->scale;

assert(BC_NUM_RDX_VALID_NP(frac));
assert(BC_NUM_RDX_VALID_NP(vm.max2));



bc_num_mul(&frac, &vm.max2, &temp, 0);
bc_num_truncate(&temp, temp.scale);
bc_num_copy(&frac, &temp);


memcpy(intn.num, n->num + nrdx, BC_NUM_SIZE(bc_num_int(n)));
intn.len = bc_num_int(n);



assert(BC_NUM_NONZERO(&vm.max));


if (BC_NUM_NONZERO(&frac)) {


bc_num_divmod(&frac, &vm.max, &temp, &temp2, 0);






state1 = (BcRand) bc_num_bigdig2(&temp2);
state2 = (BcRand) bc_num_bigdig2(&temp);
}
else state1 = state2 = 0;


if (BC_NUM_NONZERO(&intn)) {


bc_num_divmod(&intn, &vm.max, &temp, &temp2, 0);



inc1 = (BcRand) bc_num_bigdig2(&temp2);


if (bc_num_cmp(&temp, &vm.max) >= 0) {
bc_num_copy(&temp2, &temp);
bc_num_mod(&temp2, &vm.max, &temp, 0);
}



inc2 = (BcRand) bc_num_bigdig2(&temp);
}
else inc1 = inc2 = 0;

bc_rand_seed(rng, state1, state2, inc1, inc2);

err:
BC_SIG_MAYLOCK;
bc_num_free(&intn);
bc_num_free(&frac);
bc_num_free(&temp2);
bc_num_free(&temp);
BC_LONGJMP_CONT;
}

void bc_num_createFromRNG(BcNum *restrict n, BcRNG *rng) {

BcRand s1, s2, i1, i2;
BcNum conv, temp1, temp2, temp3;
BcDig temp1_num[BC_RAND_NUM_SIZE], temp2_num[BC_RAND_NUM_SIZE];
BcDig conv_num[BC_NUM_BIGDIG_LOG10];

BC_SIG_LOCK;

bc_num_init(&temp3, 2 * BC_RAND_NUM_SIZE);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;

bc_num_setup(&temp1, temp1_num, sizeof(temp1_num) / sizeof(BcDig));
bc_num_setup(&temp2, temp2_num, sizeof(temp2_num) / sizeof(BcDig));
bc_num_setup(&conv, conv_num, sizeof(conv_num) / sizeof(BcDig));



assert(BC_NUM_NONZERO(&vm.max));



assert(BC_NUM_NONZERO(&vm.max2));

bc_rand_getRands(rng, &s1, &s2, &i1, &i2);


bc_num_bigdig2num(&conv, (BcBigDig) s2);

assert(BC_NUM_RDX_VALID_NP(conv));


bc_num_mul(&conv, &vm.max, &temp1, 0);


bc_num_bigdig2num(&conv, (BcBigDig) s1);
bc_num_add(&conv, &temp1, &temp2, 0);


bc_num_div(&temp2, &vm.max2, &temp3, BC_RAND_STATE_BITS);



bc_num_bigdig2num(&conv, (BcBigDig) i2);

assert(BC_NUM_RDX_VALID_NP(conv));


bc_num_mul(&conv, &vm.max, &temp1, 0);


bc_num_bigdig2num(&conv, (BcBigDig) i1);
bc_num_add(&conv, &temp1, &temp2, 0);


bc_num_add(&temp2, &temp3, n, 0);

assert(BC_NUM_RDX_VALID(n));

err:
BC_SIG_MAYLOCK;
bc_num_free(&temp3);
BC_LONGJMP_CONT;
}

void bc_num_irand(BcNum *restrict a, BcNum *restrict b, BcRNG *restrict rng) {

BcNum atemp;
size_t i, len;

assert(a != b);

if (BC_ERR(BC_NUM_NEG(a))) bc_err(BC_ERR_MATH_NEGATIVE);


if (BC_NUM_ZERO(a) || BC_NUM_ONE(a)) return;

if (BC_ERR(bc_num_nonInt(a, &atemp))) bc_err(BC_ERR_MATH_NON_INTEGER);

assert(atemp.len);

len = atemp.len - 1;


for (i = 0; i < len; ++i)
b->num[i] = (BcDig) bc_rand_bounded(rng, BC_BASE_POW);




if (atemp.num[i] != 1) {
b->num[i] = (BcDig) bc_rand_bounded(rng, (BcRand) atemp.num[i]);
b->len = atemp.len;
}

else b->len = len;

bc_num_clean(b);

assert(BC_NUM_RDX_VALID(b));
}
#endif

size_t bc_num_addReq(const BcNum *a, const BcNum *b, size_t scale) {

size_t aint, bint, ardx, brdx;




BC_UNUSED(scale);

ardx = BC_NUM_RDX_VAL(a);
aint = bc_num_int(a);
assert(aint <= a->len && ardx <= a->len);

brdx = BC_NUM_RDX_VAL(b);
bint = bc_num_int(b);
assert(bint <= b->len && brdx <= b->len);

ardx = BC_MAX(ardx, brdx);
aint = BC_MAX(aint, bint);

return bc_vm_growSize(bc_vm_growSize(ardx, aint), 1);
}

size_t bc_num_mulReq(const BcNum *a, const BcNum *b, size_t scale) {

size_t max, rdx;



rdx = bc_vm_growSize(BC_NUM_RDX_VAL(a), BC_NUM_RDX_VAL(b));

max = BC_NUM_RDX(scale);

max = bc_vm_growSize(BC_MAX(max, rdx), 1);
rdx = bc_vm_growSize(bc_vm_growSize(bc_num_int(a), bc_num_int(b)), max);

return rdx;
}

size_t bc_num_divReq(const BcNum *a, const BcNum *b, size_t scale) {

size_t max, rdx;



rdx = bc_vm_growSize(BC_NUM_RDX_VAL(a), BC_NUM_RDX_VAL(b));

max = BC_NUM_RDX(scale);

max = bc_vm_growSize(BC_MAX(max, rdx), 1);
rdx = bc_vm_growSize(bc_num_int(a), max);

return rdx;
}

size_t bc_num_powReq(const BcNum *a, const BcNum *b, size_t scale) {
BC_UNUSED(scale);
return bc_vm_growSize(bc_vm_growSize(a->len, b->len), 1);
}

#if BC_ENABLE_EXTRA_MATH
size_t bc_num_placesReq(const BcNum *a, const BcNum *b, size_t scale) {
BC_UNUSED(scale);
return a->len + b->len - BC_NUM_RDX_VAL(a) - BC_NUM_RDX_VAL(b);
}
#endif

void bc_num_add(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, false, bc_num_as, bc_num_addReq(a, b, scale));
}

void bc_num_sub(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, true, bc_num_as, bc_num_addReq(a, b, scale));
}

void bc_num_mul(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, scale, bc_num_m, bc_num_mulReq(a, b, scale));
}

void bc_num_div(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, scale, bc_num_d, bc_num_divReq(a, b, scale));
}

void bc_num_mod(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, scale, bc_num_rem, bc_num_divReq(a, b, scale));
}

void bc_num_pow(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, scale, bc_num_p, bc_num_powReq(a, b, scale));
}

#if BC_ENABLE_EXTRA_MATH
void bc_num_places(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, scale, bc_num_place, bc_num_placesReq(a, b, scale));
}

void bc_num_lshift(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, scale, bc_num_left, bc_num_placesReq(a, b, scale));
}

void bc_num_rshift(BcNum *a, BcNum *b, BcNum *c, size_t scale) {
assert(BC_NUM_RDX_VALID(a));
assert(BC_NUM_RDX_VALID(b));
bc_num_binary(a, b, c, scale, bc_num_right, bc_num_placesReq(a, b, scale));
}
#endif

void bc_num_sqrt(BcNum *restrict a, BcNum *restrict b, size_t scale) {

BcNum num1, num2, half, f, fprime, *x0, *x1, *temp;
size_t pow, len, rdx, req, resscale;
BcDig half_digs[1];

assert(a != NULL && b != NULL && a != b);

if (BC_ERR(BC_NUM_NEG(a))) bc_err(BC_ERR_MATH_NEGATIVE);



if (a->scale > scale) scale = a->scale;


len = bc_vm_growSize(bc_num_intDigits(a), 1);
rdx = BC_NUM_RDX(scale);


req = bc_vm_growSize(BC_MAX(rdx, BC_NUM_RDX_VAL(a)), len >> 1);

BC_SIG_LOCK;




bc_num_init(b, bc_vm_growSize(req, 1));

BC_SIG_UNLOCK;

assert(a != NULL && b != NULL && a != b);
assert(a->num != NULL && b->num != NULL);


if (BC_NUM_ZERO(a)) {
bc_num_setToZero(b, scale);
return;
}


if (BC_NUM_ONE(a)) {
bc_num_one(b);
bc_num_extend(b, scale);
return;
}


rdx = BC_NUM_RDX(scale);
rdx = BC_MAX(rdx, BC_NUM_RDX_VAL(a));
len = bc_vm_growSize(a->len, rdx);

BC_SIG_LOCK;

bc_num_init(&num1, len);
bc_num_init(&num2, len);
bc_num_setup(&half, half_digs, sizeof(half_digs) / sizeof(BcDig));



bc_num_one(&half);
half.num[0] = BC_BASE_POW / 2;
half.len = 1;
BC_NUM_RDX_SET_NP(half, 1);
half.scale = 1;

bc_num_init(&f, len);
bc_num_init(&fprime, len);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;


x0 = &num1;
x1 = &num2;


bc_num_one(x0);


pow = bc_num_intDigits(a);




if (pow) {



if (pow & 1) x0->num[0] = 2;
else x0->num[0] = 6;

pow -= 2 - (pow & 1);
bc_num_shiftLeft(x0, pow / 2);
}


x0->scale = x0->rdx = 0;
resscale = (scale + BC_BASE_DIGS) + 2;



while (bc_num_cmp(x1, x0)) {

assert(BC_NUM_NONZERO(x0));




bc_num_div(a, x0, &f, resscale);
bc_num_add(x0, &f, &fprime, resscale);

assert(BC_NUM_RDX_VALID_NP(fprime));
assert(BC_NUM_RDX_VALID_NP(half));

bc_num_mul(&fprime, &half, x1, resscale);


temp = x0;
x0 = x1;
x1 = temp;
}


bc_num_copy(b, x0);
if (b->scale > scale) bc_num_truncate(b, b->scale - scale);

assert(!BC_NUM_NEG(b) || BC_NUM_NONZERO(b));
assert(BC_NUM_RDX_VALID(b));
assert(BC_NUM_RDX_VAL(b) <= b->len || !b->len);
assert(!b->len || b->num[b->len - 1] || BC_NUM_RDX_VAL(b) == b->len);

err:
BC_SIG_MAYLOCK;
bc_num_free(&fprime);
bc_num_free(&f);
bc_num_free(&num2);
bc_num_free(&num1);
BC_LONGJMP_CONT;
}

void bc_num_divmod(BcNum *a, BcNum *b, BcNum *c, BcNum *d, size_t scale) {

size_t ts, len;
BcNum *ptr_a, num2;
bool init = false;





ts = BC_MAX(scale + b->scale, a->scale);
len = bc_num_mulReq(a, b, ts);

assert(a != NULL && b != NULL && c != NULL && d != NULL);
assert(c != d && a != d && b != d && b != c);


if (c == a) {

memcpy(&num2, c, sizeof(BcNum));
ptr_a = &num2;

BC_SIG_LOCK;

bc_num_init(c, len);

init = true;

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;
}
else {
ptr_a = a;
bc_num_expand(c, len);
}


if (BC_NUM_NONZERO(a) && !BC_NUM_RDX_VAL(a) &&
!BC_NUM_RDX_VAL(b) && b->len == 1 && !scale)
{
BcBigDig rem;

bc_num_divArray(ptr_a, (BcBigDig) b->num[0], c, &rem);

assert(rem < BC_BASE_POW);

d->num[0] = (BcDig) rem;
d->len = (rem != 0);
}

else bc_num_r(ptr_a, b, c, d, scale, ts);

assert(!BC_NUM_NEG(c) || BC_NUM_NONZERO(c));
assert(BC_NUM_RDX_VALID(c));
assert(BC_NUM_RDX_VAL(c) <= c->len || !c->len);
assert(!c->len || c->num[c->len - 1] || BC_NUM_RDX_VAL(c) == c->len);
assert(!BC_NUM_NEG(d) || BC_NUM_NONZERO(d));
assert(BC_NUM_RDX_VALID(d));
assert(BC_NUM_RDX_VAL(d) <= d->len || !d->len);
assert(!d->len || d->num[d->len - 1] || BC_NUM_RDX_VAL(d) == d->len);

err:

if (init) {
BC_SIG_MAYLOCK;
bc_num_free(&num2);
BC_LONGJMP_CONT;
}
}

void bc_num_modexp(BcNum *a, BcNum *b, BcNum *c, BcNum *restrict d) {

BcNum base, exp, two, temp, atemp, btemp, ctemp;
BcDig two_digs[2];

assert(a != NULL && b != NULL && c != NULL && d != NULL);
assert(a != d && b != d && c != d);

if (BC_ERR(BC_NUM_ZERO(c))) bc_err(BC_ERR_MATH_DIVIDE_BY_ZERO);

if (BC_ERR(BC_NUM_NEG(b))) bc_err(BC_ERR_MATH_NEGATIVE);

#if !defined(NDEBUG)

btemp.len = 0;
ctemp.len = 0;
#endif


if (BC_ERR(bc_num_nonInt(a, &atemp) || bc_num_nonInt(b, &btemp) ||
bc_num_nonInt(c, &ctemp)))
{
bc_err(BC_ERR_MATH_NON_INTEGER);
}

bc_num_expand(d, ctemp.len);

BC_SIG_LOCK;

bc_num_init(&base, ctemp.len);
bc_num_setup(&two, two_digs, sizeof(two_digs) / sizeof(BcDig));
bc_num_init(&temp, btemp.len + 1);
bc_num_createCopy(&exp, &btemp);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;

bc_num_one(&two);
two.num[0] = 2;
bc_num_one(d);


bc_num_rem(&atemp, &ctemp, &base, 0);



while (BC_NUM_NONZERO(&exp)) {


bc_num_divmod(&exp, &two, &exp, &temp, 0);

if (BC_NUM_ONE(&temp) && !BC_NUM_NEG_NP(temp)) {

assert(BC_NUM_RDX_VALID(d));
assert(BC_NUM_RDX_VALID_NP(base));

bc_num_mul(d, &base, &temp, 0);


bc_num_rem(&temp, &ctemp, d, 0);
}

assert(BC_NUM_RDX_VALID_NP(base));

bc_num_mul(&base, &base, &temp, 0);


bc_num_rem(&temp, &ctemp, &base, 0);
}

err:
BC_SIG_MAYLOCK;
bc_num_free(&exp);
bc_num_free(&temp);
bc_num_free(&base);
BC_LONGJMP_CONT;
assert(!BC_NUM_NEG(d) || d->len);
assert(BC_NUM_RDX_VALID(d));
assert(!d->len || d->num[d->len - 1] || BC_NUM_RDX_VAL(d) == d->len);
}

#if BC_DEBUG_CODE
void bc_num_printDebug(const BcNum *n, const char *name, bool emptyline) {
bc_file_puts(&vm.fout, bc_flush_none, name);
bc_file_puts(&vm.fout, bc_flush_none, ": ");
bc_num_printDecimal(n, true);
bc_file_putchar(&vm.fout, bc_flush_err, '\n');
if (emptyline) bc_file_putchar(&vm.fout, bc_flush_err, '\n');
vm.nchars = 0;
}

void bc_num_printDigs(const BcDig *n, size_t len, bool emptyline) {

size_t i;

for (i = len - 1; i < len; --i)
bc_file_printf(&vm.fout, " %lu", (unsigned long) n[i]);

bc_file_putchar(&vm.fout, bc_flush_err, '\n');
if (emptyline) bc_file_putchar(&vm.fout, bc_flush_err, '\n');
vm.nchars = 0;
}

void bc_num_printWithDigs(const BcNum *n, const char *name, bool emptyline) {
bc_file_puts(&vm.fout, bc_flush_none, name);
bc_file_printf(&vm.fout, " len: %zu, rdx: %zu, scale: %zu\n",
name, n->len, BC_NUM_RDX_VAL(n), n->scale);
bc_num_printDigs(n->num, n->len, emptyline);
}

void bc_num_dump(const char *varname, const BcNum *n) {

ulong i, scale = n->scale;

bc_file_printf(&vm.ferr, "\n%s = %s", varname,
n->len ? (BC_NUM_NEG(n) ? "-" : "+") : "0 ");

for (i = n->len - 1; i < n->len; --i) {

if (i + 1 == BC_NUM_RDX_VAL(n))
bc_file_puts(&vm.ferr, bc_flush_none, ". ");

if (scale / BC_BASE_DIGS != BC_NUM_RDX_VAL(n) - i - 1)
bc_file_printf(&vm.ferr, "%lu ", (unsigned long) n->num[i]);
else {

int mod = scale % BC_BASE_DIGS;
int d = BC_BASE_DIGS - mod;
BcDig div;

if (mod != 0) {
div = n->num[i] / ((BcDig) bc_num_pow10[(ulong) d]);
bc_file_printf(&vm.ferr, "%lu", (unsigned long) div);
}

div = n->num[i] % ((BcDig) bc_num_pow10[(ulong) d]);
bc_file_printf(&vm.ferr, " ' %lu ", (unsigned long) div);
}
}

bc_file_printf(&vm.ferr, "(%zu | %zu.%zu / %zu) %lu\n",
n->scale, n->len, BC_NUM_RDX_VAL(n), n->cap,
(unsigned long) (void*) n->num);

bc_file_flush(&vm.ferr, bc_flush_err);
}
#endif
