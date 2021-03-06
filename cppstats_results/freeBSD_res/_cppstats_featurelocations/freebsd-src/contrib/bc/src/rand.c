








































#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#if !defined(_WIN32)
#include <unistd.h>
#else
#include <Windows.h>
#include <bcrypt.h>
#endif

#include <status.h>
#include <rand.h>
#include <vm.h>

#if BC_ENABLE_EXTRA_MATH

#if !BC_RAND_BUILTIN







static BcRandState bc_rand_addition(uint_fast64_t a, uint_fast64_t b) {

BcRandState res;

res.lo = a + b;
res.hi = (res.lo < a);

return res;
}







static BcRandState bc_rand_addition2(BcRandState a, BcRandState b) {

BcRandState temp, res;

res = bc_rand_addition(a.lo, b.lo);
temp = bc_rand_addition(a.hi, b.hi);
res.hi += temp.lo;

return res;
}







static BcRandState bc_rand_multiply(uint_fast64_t a, uint_fast64_t b) {

uint_fast64_t al, ah, bl, bh, c0, c1, c2, c3;
BcRandState carry, res;

al = BC_RAND_TRUNC32(a);
ah = BC_RAND_CHOP32(a);
bl = BC_RAND_TRUNC32(b);
bh = BC_RAND_CHOP32(b);

c0 = al * bl;
c1 = al * bh;
c2 = ah * bl;
c3 = ah * bh;

carry = bc_rand_addition(c1, c2);

res = bc_rand_addition(c0, (BC_RAND_TRUNC32(carry.lo)) << 32);
res.hi += BC_RAND_CHOP32(carry.lo) + c3 + (carry.hi << 32);

return res;
}







static BcRandState bc_rand_multiply2(BcRandState a, BcRandState b) {

BcRandState c0, c1, c2, carry;

c0 = bc_rand_multiply(a.lo, b.lo);
c1 = bc_rand_multiply(a.lo, b.hi);
c2 = bc_rand_multiply(a.hi, b.lo);

carry = bc_rand_addition2(c1, c2);
carry.hi = carry.lo;
carry.lo = 0;

return bc_rand_addition2(c0, carry);
}

#endif






static void bc_rand_setModified(BcRNGData *r) {

#if BC_RAND_BUILTIN
r->inc |= (BcRandState) 1UL;
#else
r->inc.lo |= (uint_fast64_t) 1UL;
#endif
}






static void bc_rand_clearModified(BcRNGData *r) {

#if BC_RAND_BUILTIN
r->inc &= ~((BcRandState) 1UL);
#else
r->inc.lo &= ~(1UL);
#endif
}







static void bc_rand_copy(BcRNGData *d, BcRNGData *s) {
bool unmod = BC_RAND_NOTMODIFIED(d);
memcpy(d, s, sizeof(BcRNGData));
if (!unmod) bc_rand_setModified(d);
else if (!BC_RAND_NOTMODIFIED(s)) bc_rand_clearModified(d);
}

#if !defined(_WIN32)






static ulong bc_rand_frand(void* ptr) {

ulong buf[1];
int fd;
ssize_t nread;

assert(ptr != NULL);

fd = *((int*)ptr);

nread = read(fd, buf, sizeof(ulong));

if (BC_ERR(nread != sizeof(ulong))) bc_vm_fatalError(BC_ERR_FATAL_IO_ERR);

return *((ulong*)buf);
}
#else






static ulong bc_rand_winrand(void *ptr) {

ulong buf[1];
NTSTATUS s;

BC_UNUSED(ptr);

buf[0] = 0;

s = BCryptGenRandom(NULL, (char*) buf, sizeof(ulong),
BCRYPT_USE_SYSTEM_PREFERRED_RNG);

if (BC_ERR(!BCRYPT_SUCCESS(s))) buf[0] = 0;

return buf[0];
}
#endif









static ulong bc_rand_rand(void *ptr) {

size_t i;
ulong res = 0;

BC_UNUSED(ptr);


for (i = 0; i < sizeof(ulong); ++i)
res |= ((ulong) (rand() & BC_RAND_SRAND_BITS)) << (i * CHAR_BIT);

return res;
}







static BcRandState bc_rand_inc(BcRNGData *r) {

BcRandState inc;

#if BC_RAND_BUILTIN
inc = r->inc | 1;
#else
inc.lo = r->inc.lo | 1;
inc.hi = r->inc.hi;
#endif

return inc;
}





static void bc_rand_setupInc(BcRNGData *r) {

#if BC_RAND_BUILTIN
r->inc <<= 1UL;
#else
r->inc.hi <<= 1UL;
r->inc.hi |= (r->inc.lo & (1UL << (BC_LONG_BIT - 1))) >> (BC_LONG_BIT - 1);
r->inc.lo <<= 1UL;
#endif
}







static void bc_rand_seedState(BcRandState *state, ulong val1, ulong val2) {

#if BC_RAND_BUILTIN
*state = ((BcRandState) val1) | ((BcRandState) val2) << (BC_LONG_BIT);
#else
state->lo = val1;
state->hi = val2;
#endif
}









static void bc_rand_seedRNG(BcRNGData *r, ulong state1, ulong state2,
ulong inc1, ulong inc2)
{
bc_rand_seedState(&r->state, state1, state2);
bc_rand_seedState(&r->inc, inc1, inc2);
bc_rand_setupInc(r);
}







static void bc_rand_fill(BcRNGData *r, BcRandUlong fulong, void *ptr) {

ulong state1, state2, inc1, inc2;

state1 = fulong(ptr);
state2 = fulong(ptr);

inc1 = fulong(ptr);
inc2 = fulong(ptr);

bc_rand_seedRNG(r, state1, state2, inc1, inc2);
}





static void bc_rand_step(BcRNGData *r) {
BcRandState temp = bc_rand_mul2(r->state, bc_rand_multiplier);
r->state = bc_rand_add2(temp, bc_rand_inc(r));
}






static BcRand bc_rand_output(BcRNGData *r) {
return BC_RAND_ROT(BC_RAND_FOLD(r->state), BC_RAND_ROTAMT(r->state));
}







static void bc_rand_seedZeroes(BcRNG *r, BcRNGData *rng, size_t idx) {

BcRNGData *rng2;


if (r->v.len <= idx) return;


rng2 = bc_vec_item_rev(&r->v, idx);


if (BC_RAND_ZERO(rng2)) {

size_t i;


for (i = 1; i < r->v.len; ++i)
bc_rand_copy(bc_vec_item_rev(&r->v, i), rng);
}
}

void bc_rand_srand(BcRNGData *rng) {

int fd = 0;

BC_SIG_LOCK;

#if !defined(_WIN32)


fd = open("/dev/urandom", O_RDONLY);

if (BC_NO_ERR(fd >= 0)) {
bc_rand_fill(rng, bc_rand_frand, &fd);
close(fd);
}
else {


fd = open("/dev/random", O_RDONLY);

if (BC_NO_ERR(fd >= 0)) {
bc_rand_fill(rng, bc_rand_frand, &fd);
close(fd);
}
}
#else

bc_rand_fill(rng, bc_rand_winrand, NULL);
#endif


while (BC_ERR(BC_RAND_ZERO(rng))) bc_rand_fill(rng, bc_rand_rand, NULL);

BC_SIG_UNLOCK;
}







static void bc_rand_propagate(BcRNG *r, BcRNGData *rng) {


if (r->v.len <= 1) return;


if (BC_RAND_NOTMODIFIED(rng)) {

size_t i;
bool go = true;


for (i = 1; go && i < r->v.len; ++i) {

BcRNGData *rng2 = bc_vec_item_rev(&r->v, i);

go = BC_RAND_NOTMODIFIED(rng2);

bc_rand_copy(rng2, rng);
}


bc_rand_seedZeroes(r, rng, i);
}

else bc_rand_seedZeroes(r, rng, 1);
}

BcRand bc_rand_int(BcRNG *r) {


BcRNGData *rng = bc_vec_top(&r->v);
BcRand res;


if (BC_ERR(BC_RAND_ZERO(rng))) bc_rand_srand(rng);

BC_SIG_LOCK;


bc_rand_step(rng);
bc_rand_propagate(r, rng);
res = bc_rand_output(rng);

BC_SIG_UNLOCK;

return res;
}

BcRand bc_rand_bounded(BcRNG *r, BcRand bound) {


BcRand rand, threshold = (0 - bound) % bound;

do {
rand = bc_rand_int(r);
} while (rand < threshold);

return rand % bound;
}

void bc_rand_seed(BcRNG *r, ulong state1, ulong state2, ulong inc1, ulong inc2)
{

BcRNGData *rng = bc_vec_top(&r->v);


bc_rand_seedState(&rng->inc, inc1, inc2);
bc_rand_setupInc(rng);
bc_rand_setModified(rng);



if (!state1 && !state2) {
memcpy(&rng->state, &rng->inc, sizeof(BcRandState));
bc_rand_step(rng);
}
else bc_rand_seedState(&rng->state, state1, state2);


bc_rand_propagate(r, rng);
}








static BcRandState bc_rand_getInc(BcRNGData *r) {

BcRandState res;

#if BC_RAND_BUILTIN
res = r->inc >> 1;
#else
res = r->inc;
res.lo >>= 1;
res.lo |= (res.hi & 1) << (BC_LONG_BIT - 1);
res.hi >>= 1;
#endif

return res;
}

void bc_rand_getRands(BcRNG *r, BcRand *s1, BcRand *s2, BcRand *i1, BcRand *i2)
{
BcRandState inc;
BcRNGData *rng = bc_vec_top(&r->v);

if (BC_ERR(BC_RAND_ZERO(rng))) bc_rand_srand(rng);


inc = bc_rand_getInc(rng);


*s1 = BC_RAND_TRUNC(rng->state);
*s2 = BC_RAND_CHOP(rng->state);


*i1 = BC_RAND_TRUNC(inc);
*i2 = BC_RAND_CHOP(inc);
}

void bc_rand_push(BcRNG *r) {

BcRNGData *rng = bc_vec_pushEmpty(&r->v);



memset(rng, 0, sizeof(BcRNGData));


if (r->v.len > 1) bc_rand_copy(rng, bc_vec_item_rev(&r->v, 1));
}

void bc_rand_pop(BcRNG *r, bool reset) {
bc_vec_npop(&r->v, reset ? r->v.len - 1 : 1);
}

void bc_rand_init(BcRNG *r) {
BC_SIG_ASSERT_LOCKED;
bc_vec_init(&r->v, sizeof(BcRNGData), BC_DTOR_NONE);
bc_rand_push(r);
}

#if BC_RAND_USE_FREE
void bc_rand_free(BcRNG *r) {
BC_SIG_ASSERT_LOCKED;
bc_vec_free(&r->v);
}
#endif

#endif
