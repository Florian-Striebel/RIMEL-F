


































#if !defined(BC_NUM_H)
#define BC_NUM_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#include <status.h>
#include <vector.h>
#include <bcl.h>

#if !defined(BC_ENABLE_EXTRA_MATH)
#define BC_ENABLE_EXTRA_MATH (1)
#endif


#define BC_BASE (10)


typedef unsigned long ulong;



typedef BclBigDig BcBigDig;

#if BC_LONG_BIT >= 64


#define BC_NUM_BIGDIG_MAX ((BcBigDig) UINT64_MAX)


#define BC_BASE_DIGS (9)


#define BC_BASE_POW (1000000000)


#define BC_NUM_BIGDIG_C UINT64_C


typedef int_least32_t BcDig;

#elif BC_LONG_BIT >= 32


#define BC_NUM_BIGDIG_MAX ((BcBigDig) UINT32_MAX)


#define BC_BASE_DIGS (4)


#define BC_BASE_POW (10000)


#define BC_NUM_BIGDIG_C UINT32_C


typedef int_least16_t BcDig;

#else


#error BC_LONG_BIT must be at least 32

#endif


#define BC_NUM_DEF_SIZE (8)


typedef struct BcNum {




BcDig *restrict num;





size_t rdx;






size_t scale;



size_t len;



size_t cap;

} BcNum;

#if BC_ENABLE_EXTRA_MATH


struct BcRNG;

#endif


#define BC_NUM_MIN_BASE (BC_NUM_BIGDIG_C(2))


#define BC_NUM_MAX_POSIX_IBASE (BC_NUM_BIGDIG_C(16))


#define BC_NUM_MAX_IBASE (BC_NUM_BIGDIG_C(36))


#define BC_NUM_MAX_LBASE (BC_NUM_BIGDIG_C('Z' + BC_BASE + 1))


#define BC_NUM_PRINT_WIDTH (BC_NUM_BIGDIG_C(69))


#define BC_NUM_STREAM_BASE (256)


#if !defined(BC_NUM_KARATSUBA_LEN)
#define BC_NUM_KARATSUBA_LEN (BC_NUM_BIGDIG_C(32))
#elif BC_NUM_KARATSUBA_LEN < 16
#error BC_NUM_KARATSUBA_LEN must be at least 16.
#endif



#define BC_NUM_BIGDIG_LOG10 (BC_NUM_DEF_SIZE)






#define BC_NUM_NONZERO(n) ((n)->len)






#define BC_NUM_ZERO(n) (!BC_NUM_NONZERO(n))






#define BC_NUM_ONE(n) ((n)->len == 1 && (n)->rdx == 0 && (n)->num[0] == 1)






#define BC_NUM_NUM_LETTER(c) ((c) - 'A' + BC_BASE)





#define BC_NUM_KARATSUBA_ALLOCS (6)








#define BC_NUM_ROUND_POW(s) (bc_vm_growSize((s), BC_BASE_DIGS - 1))






#define BC_NUM_RDX(s) (BC_NUM_ROUND_POW(s) / BC_BASE_DIGS)






#define BC_NUM_RDX_VAL(n) ((n)->rdx >> 1)







#define BC_NUM_RDX_VAL_NP(n) ((n).rdx >> 1)






#define BC_NUM_RDX_SET(n, v) ((n)->rdx = (((v) << 1) | ((n)->rdx & (BcBigDig) 1)))







#define BC_NUM_RDX_SET_NP(n, v) ((n).rdx = (((v) << 1) | ((n).rdx & (BcBigDig) 1)))








#define BC_NUM_RDX_SET_NEG(n, v, neg) ((n)->rdx = (((v) << 1) | (neg)))







#define BC_NUM_RDX_VALID(n) (BC_NUM_ZERO(n) || BC_NUM_RDX_VAL(n) * BC_BASE_DIGS >= (n)->scale)








#define BC_NUM_RDX_VALID_NP(n) ((!(n).len) || BC_NUM_RDX_VAL_NP(n) * BC_BASE_DIGS >= (n).scale)







#define BC_NUM_NEG(n) ((n)->rdx & ((BcBigDig) 1))







#define BC_NUM_NEG_NP(n) ((n).rdx & ((BcBigDig) 1))





#define BC_NUM_NEG_CLR(n) ((n)->rdx &= ~((BcBigDig) 1))





#define BC_NUM_NEG_CLR_NP(n) ((n).rdx &= ~((BcBigDig) 1))





#define BC_NUM_NEG_SET(n) ((n)->rdx |= ((BcBigDig) 1))





#define BC_NUM_NEG_TGL(n) ((n)->rdx ^= ((BcBigDig) 1))





#define BC_NUM_NEG_TGL_NP(n) ((n).rdx ^= ((BcBigDig) 1))







#define BC_NUM_NEG_VAL(n, v) (((n)->rdx & ~((BcBigDig) 1)) | (v))








#define BC_NUM_NEG_VAL_NP(n, v) (((n).rdx & ~((BcBigDig) 1)) | (v))






#define BC_NUM_SIZE(n) ((n) * sizeof(BcDig))


#if BC_DEBUG_CODE
#define BC_NUM_PRINT(x) fprintf(stderr, "%s = %lu\n", #x, (unsigned long)(x))
#define DUMP_NUM bc_num_dump
#else
#undef DUMP_NUM
#define DUMP_NUM(x,y)
#define BC_NUM_PRINT(x)
#endif








typedef void (*BcNumBinaryOp)(BcNum* a, BcNum* b, BcNum* c, size_t scale);










typedef void (*BcNumBinOp)(BcNum* a, BcNum* b, BcNum* restrict c, size_t scale);











typedef size_t (*BcNumBinaryOpReq)(const BcNum* a, const BcNum* b,
size_t scale);












typedef void (*BcNumDigitOp)(size_t n, size_t len, bool rdx, bool bslash);








typedef void (*BcNumShiftAddOp)(BcDig* restrict a, const BcDig* restrict b,
size_t len);






void bc_num_init(BcNum *restrict n, size_t req);









void bc_num_setup(BcNum *restrict n, BcDig *restrict num, size_t cap);







void bc_num_copy(BcNum *d, const BcNum *s);







void bc_num_createCopy(BcNum *d, const BcNum *s);







void bc_num_createFromBigdig(BcNum *restrict n, BcBigDig val);






void bc_num_clear(BcNum *restrict n);





void bc_num_free(void *num);






size_t bc_num_scale(const BcNum *restrict n);









size_t bc_num_len(const BcNum *restrict n);








BcBigDig bc_num_bigdig(const BcNum *restrict n);







BcBigDig bc_num_bigdig2(const BcNum *restrict n);







void bc_num_bigdig2num(BcNum *restrict n, BcBigDig val);

#if BC_ENABLE_EXTRA_MATH








void bc_num_irand(BcNum *restrict a, BcNum *restrict b,
struct BcRNG *restrict rng);






void bc_num_rng(const BcNum *restrict n, struct BcRNG *rng);






void bc_num_createFromRNG(BcNum *restrict n, struct BcRNG *rng);

#endif








void bc_num_add(BcNum *a, BcNum *b, BcNum *c, size_t scale);








void bc_num_sub(BcNum *a, BcNum *b, BcNum *c, size_t scale);








void bc_num_mul(BcNum *a, BcNum *b, BcNum *c, size_t scale);








void bc_num_div(BcNum *a, BcNum *b, BcNum *c, size_t scale);








void bc_num_mod(BcNum *a, BcNum *b, BcNum *c, size_t scale);








void bc_num_pow(BcNum *a, BcNum *b, BcNum *c, size_t scale);
#if BC_ENABLE_EXTRA_MATH








void bc_num_places(BcNum *a, BcNum *b, BcNum *c, size_t scale);








void bc_num_lshift(BcNum *a, BcNum *b, BcNum *c, size_t scale);








void bc_num_rshift(BcNum *a, BcNum *b, BcNum *c, size_t scale);

#endif







void bc_num_sqrt(BcNum *restrict a, BcNum *restrict b, size_t scale);









void bc_num_divmod(BcNum *a, BcNum *b, BcNum *c, BcNum *d, size_t scale);










size_t bc_num_addReq(const BcNum* a, const BcNum* b, size_t scale);










size_t bc_num_mulReq(const BcNum *a, const BcNum *b, size_t scale);










size_t bc_num_divReq(const BcNum *a, const BcNum *b, size_t scale);










size_t bc_num_powReq(const BcNum *a, const BcNum *b, size_t scale);

#if BC_ENABLE_EXTRA_MATH










size_t bc_num_placesReq(const BcNum *a, const BcNum *b, size_t scale);

#endif







void bc_num_truncate(BcNum *restrict n, size_t places);







void bc_num_extend(BcNum *restrict n, size_t places);








void bc_num_shiftRight(BcNum *restrict n, size_t places);









ssize_t bc_num_cmp(const BcNum *a, const BcNum *b);








void bc_num_modexp(BcNum *a, BcNum *b, BcNum *c, BcNum *restrict d);





void bc_num_zero(BcNum *restrict n);





void bc_num_one(BcNum *restrict n);






ssize_t bc_num_cmpZero(const BcNum *n);

#if !defined(NDEBUG) || BC_ENABLE_LIBRARY









bool bc_num_strValid(const char *restrict val);

#endif







void bc_num_parse(BcNum *restrict n, const char *restrict val, BcBigDig base);








void bc_num_print(BcNum *restrict n, BcBigDig base, bool newline);

#if !BC_ENABLE_LIBRARY





void bc_num_stream(BcNum *restrict n);

#endif

#if BC_DEBUG_CODE







void bc_num_printDebug(const BcNum *n, const char *name, bool emptyline);







void bc_num_printDigs(const BcDig* n, size_t len, bool emptyline);







void bc_num_printWithDigs(const BcNum *n, const char *name, bool emptyline);






void bc_num_dump(const char *varname, const BcNum *n);

#endif


extern const char bc_num_hex_digits[];



extern const BcBigDig bc_num_pow10[BC_BASE_DIGS + 1];


extern const BcDig bc_num_bigdigMax[];


extern const size_t bc_num_bigdigMax_size;


extern const BcDig bc_num_bigdigMax2[];


extern const size_t bc_num_bigdigMax2_size;

#endif
