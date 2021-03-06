








































#if !defined(BC_RAND_H)
#define BC_RAND_H

#include <stdint.h>
#include <inttypes.h>

#include <vector.h>
#include <num.h>

#if BC_ENABLE_EXTRA_MATH

#if BC_ENABLE_LIBRARY
#define BC_RAND_USE_FREE (1)
#else
#if !defined(NDEBUG)
#define BC_RAND_USE_FREE (1)
#else
#define BC_RAND_USE_FREE (0)
#endif
#endif






typedef ulong (*BcRandUlong)(void *ptr);

#if BC_LONG_BIT >= 64



#if defined(BC_RAND_BUILTIN)
#if BC_RAND_BUILTIN
#if !defined(__SIZEOF_INT128__)
#undef BC_RAND_BUILTIN
#define BC_RAND_BUILTIN (0)
#endif
#endif
#endif

#if !defined(BC_RAND_BUILTIN)
#if defined(__SIZEOF_INT128__)
#define BC_RAND_BUILTIN (1)
#else
#define BC_RAND_BUILTIN (0)
#endif
#endif


typedef uint64_t BcRand;


#define BC_RAND_ROTC (63)

#if BC_RAND_BUILTIN


typedef __uint128_t BcRandState;







#define bc_rand_mul(a, b) (((BcRandState) (a)) * ((BcRandState) (b)))







#define bc_rand_add(a, b) (((BcRandState) (a)) + ((BcRandState) (b)))







#define bc_rand_mul2(a, b) (((BcRandState) (a)) * ((BcRandState) (b)))







#define bc_rand_add2(a, b) (((BcRandState) (a)) + ((BcRandState) (b)))







#define BC_RAND_NOTMODIFIED(r) (((r)->inc & 1UL) == 0)






#define BC_RAND_ZERO(r) (!(r)->state)







#define BC_RAND_CONSTANT(h, l) ((((BcRandState) (h)) << 64) + (BcRandState) (l))






#define BC_RAND_TRUNC(s) ((uint64_t) (s))






#define BC_RAND_CHOP(s) ((uint64_t) ((s) >> 64UL))






#define BC_RAND_ROTAMT(s) ((unsigned int) ((s) >> 122UL))

#else


typedef struct BcRandState {


uint_fast64_t lo;


uint_fast64_t hi;

} BcRandState;







#define bc_rand_mul(a, b) (bc_rand_multiply((a), (b)))







#define bc_rand_add(a, b) (bc_rand_addition((a), (b)))







#define bc_rand_mul2(a, b) (bc_rand_multiply2((a), (b)))







#define bc_rand_add2(a, b) (bc_rand_addition2((a), (b)))







#define BC_RAND_NOTMODIFIED(r) (((r)->inc.lo & 1) == 0)






#define BC_RAND_ZERO(r) (!(r)->state.lo && !(r)->state.hi)







#define BC_RAND_CONSTANT(h, l) { .lo = (l), .hi = (h) }






#define BC_RAND_TRUNC(s) ((s).lo)






#define BC_RAND_CHOP(s) ((s).hi)






#define BC_RAND_ROTAMT(s) ((unsigned int) ((s).hi >> 58UL))


#define BC_RAND_BOTTOM32 (((uint_fast64_t) 0xffffffffULL))






#define BC_RAND_TRUNC32(n) ((n) & BC_RAND_BOTTOM32)






#define BC_RAND_CHOP32(n) ((n) >> 32)

#endif


#define BC_RAND_MULTIPLIER BC_RAND_CONSTANT(2549297995355413924ULL, 4865540595714422341ULL)







#define BC_RAND_FOLD(s) ((BcRand) (BC_RAND_CHOP(s) ^ BC_RAND_TRUNC(s)))

#else


#undef BC_RAND_BUILTIN
#define BC_RAND_BUILTIN (1)


typedef uint32_t BcRand;


#define BC_RAND_ROTC (31)


typedef uint_fast64_t BcRandState;







#define bc_rand_mul(a, b) (((BcRandState) (a)) * ((BcRandState) (b)))







#define bc_rand_add(a, b) (((BcRandState) (a)) + ((BcRandState) (b)))







#define bc_rand_mul2(a, b) (((BcRandState) (a)) * ((BcRandState) (b)))







#define bc_rand_add2(a, b) (((BcRandState) (a)) + ((BcRandState) (b)))







#define BC_RAND_NOTMODIFIED(r) (((r)->inc & 1UL) == 0)






#define BC_RAND_ZERO(r) (!(r)->state)






#define BC_RAND_CONSTANT(n) UINT64_C(n)


#define BC_RAND_MULTIPLIER BC_RAND_CONSTANT(6364136223846793005)






#define BC_RAND_TRUNC(s) ((uint32_t) (s))






#define BC_RAND_CHOP(s) ((uint32_t) ((s) >> 32UL))






#define BC_RAND_ROTAMT(s) ((unsigned int) ((s) >> 59UL))






#define BC_RAND_FOLD(s) ((BcRand) ((((s) >> 18U) ^ (s)) >> 27U))

#endif







#define BC_RAND_ROT(v, r) ((BcRand) (((v) >> (r)) | ((v) << ((0 - (r)) & BC_RAND_ROTC))))



#define BC_RAND_BITS (sizeof(BcRand) * CHAR_BIT)


#define BC_RAND_STATE_BITS (sizeof(BcRandState) * CHAR_BIT)



#define BC_RAND_NUM_SIZE (BC_NUM_BIGDIG_LOG10 * 2 + 2)


#define BC_RAND_SRAND_BITS ((1 << CHAR_BIT) - 1)


typedef struct BcRNGData {


BcRandState state;


BcRandState inc;

} BcRNGData;



typedef struct BcRNG {


BcVec v;

} BcRNG;





void bc_rand_init(BcRNG *r);

#if BC_RAND_USE_FREE






void bc_rand_free(BcRNG *r);

#endif






BcRand bc_rand_int(BcRNG *r);








BcRand bc_rand_bounded(BcRNG *r, BcRand bound);









void bc_rand_seed(BcRNG *r, ulong state1, ulong state2, ulong inc1, ulong inc2);





void bc_rand_push(BcRNG *r);







void bc_rand_pop(BcRNG *r, bool reset);









void bc_rand_getRands(BcRNG *r, BcRand *s1, BcRand *s2, BcRand *i1, BcRand *i2);





void bc_rand_srand(BcRNGData *rng);


extern const BcRandState bc_rand_multiplier;

#endif

#endif
