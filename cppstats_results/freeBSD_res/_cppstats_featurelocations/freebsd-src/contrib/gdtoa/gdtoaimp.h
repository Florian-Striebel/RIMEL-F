


















































































































































































#if !defined(GDTOAIMP_H_INCLUDED)
#define GDTOAIMP_H_INCLUDED

#define Long int

#include "gdtoa.h"
#include "gd_qnan.h"
#if defined(Honor_FLT_ROUNDS)
#include <fenv.h>
#endif

#if defined(DEBUG)
#include "stdio.h"
#define Bug(x) {fprintf(stderr, "%s\n", x); exit(1);}
#endif

#include "limits.h"
#include "stdlib.h"
#include "string.h"
#include "libc_private.h"

#include "namespace.h"
#include <pthread.h>
#include "un-namespace.h"
#include "xlocale_private.h"

#if defined(KR_headers)
#define Char char
#else
#define Char void
#endif

#if defined(MALLOC)
extern Char *MALLOC ANSI((size_t));
#else
#define MALLOC malloc
#endif

#define INFNAN_CHECK
#define USE_LOCALE
#define NO_LOCALE_CACHE
#define Honor_FLT_ROUNDS
#define Trust_FLT_ROUNDS

#undef IEEE_Arith
#undef Avoid_Underflow
#if defined(IEEE_MC68k)
#define IEEE_Arith
#endif
#if defined(IEEE_8087)
#define IEEE_Arith
#endif

#include "errno.h"
#if defined(Bad_float_h)

#if defined(IEEE_Arith)
#define DBL_DIG 15
#define DBL_MAX_10_EXP 308
#define DBL_MAX_EXP 1024
#define FLT_RADIX 2
#define DBL_MAX 1.7976931348623157e+308
#endif

#if defined(IBM)
#define DBL_DIG 16
#define DBL_MAX_10_EXP 75
#define DBL_MAX_EXP 63
#define FLT_RADIX 16
#define DBL_MAX 7.2370055773322621e+75
#endif

#if defined(VAX)
#define DBL_DIG 16
#define DBL_MAX_10_EXP 38
#define DBL_MAX_EXP 127
#define FLT_RADIX 2
#define DBL_MAX 1.7014118346046923e+38
#define n_bigtens 2
#endif

#if !defined(LONG_MAX)
#define LONG_MAX 2147483647
#endif

#else
#include "float.h"
#endif

#if defined(IEEE_Arith)
#define Scale_Bit 0x10
#define n_bigtens 5
#endif

#if defined(IBM)
#define n_bigtens 3
#endif

#if defined(VAX)
#define n_bigtens 2
#endif

#if !defined(__MATH_H__)
#include "math.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(IEEE_8087) + defined(IEEE_MC68k) + defined(VAX) + defined(IBM) != 1
Exactly one of IEEE_8087, IEEE_MC68k, VAX, or IBM should be defined.
#endif

typedef union { double d; ULong L[2]; } U;

#if defined(IEEE_8087)
#define word0(x) (x)->L[1]
#define word1(x) (x)->L[0]
#else
#define word0(x) (x)->L[0]
#define word1(x) (x)->L[1]
#endif
#define dval(x) (x)->d





#if defined(IEEE_8087) + defined(VAX)
#define Storeinc(a,b,c) (((unsigned short *)a)[1] = (unsigned short)b, ((unsigned short *)a)[0] = (unsigned short)c, a++)

#else
#define Storeinc(a,b,c) (((unsigned short *)a)[0] = (unsigned short)b, ((unsigned short *)a)[1] = (unsigned short)c, a++)

#endif







#if defined(IEEE_Arith)
#define Exp_shift 20
#define Exp_shift1 20
#define Exp_msk1 0x100000
#define Exp_msk11 0x100000
#define Exp_mask 0x7ff00000
#define P 53
#define Bias 1023
#define Emin (-1022)
#define Exp_1 0x3ff00000
#define Exp_11 0x3ff00000
#define Ebits 11
#define Frac_mask 0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask 0xfffff
#define Bndry_mask1 0xfffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Quick_max 14
#define Int_max 14

#if !defined(Flt_Rounds)
#if defined(FLT_ROUNDS)
#define Flt_Rounds FLT_ROUNDS
#else
#define Flt_Rounds 1
#endif
#endif

#else
#undef Sudden_Underflow
#define Sudden_Underflow
#if defined(IBM)
#undef Flt_Rounds
#define Flt_Rounds 0
#define Exp_shift 24
#define Exp_shift1 24
#define Exp_msk1 0x1000000
#define Exp_msk11 0x1000000
#define Exp_mask 0x7f000000
#define P 14
#define Bias 65
#define Exp_1 0x41000000
#define Exp_11 0x41000000
#define Ebits 8
#define Frac_mask 0xffffff
#define Frac_mask1 0xffffff
#define Bletch 4
#define Ten_pmax 22
#define Bndry_mask 0xefffff
#define Bndry_mask1 0xffffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 4
#define Tiny0 0x100000
#define Tiny1 0
#define Quick_max 14
#define Int_max 15
#else
#undef Flt_Rounds
#define Flt_Rounds 1
#define Exp_shift 23
#define Exp_shift1 7
#define Exp_msk1 0x80
#define Exp_msk11 0x800000
#define Exp_mask 0x7f80
#define P 56
#define Bias 129
#define Exp_1 0x40800000
#define Exp_11 0x4080
#define Ebits 8
#define Frac_mask 0x7fffff
#define Frac_mask1 0xffff007f
#define Ten_pmax 24
#define Bletch 2
#define Bndry_mask 0xffff007f
#define Bndry_mask1 0xffff007f
#define LSB 0x10000
#define Sign_bit 0x8000
#define Log2P 1
#define Tiny0 0x80
#define Tiny1 0
#define Quick_max 15
#define Int_max 15
#endif
#endif

#if !defined(IEEE_Arith)
#define ROUND_BIASED
#else
#if defined(ROUND_BIASED_without_Round_Up)
#undef ROUND_BIASED
#define ROUND_BIASED
#endif
#endif

#if defined(RND_PRODQUOT)
#define rounded_product(a,b) a = rnd_prod(a, b)
#define rounded_quotient(a,b) a = rnd_quot(a, b)
#if defined(KR_headers)
extern double rnd_prod(), rnd_quot();
#else
extern double rnd_prod(double, double), rnd_quot(double, double);
#endif
#else
#define rounded_product(a,b) a *= b
#define rounded_quotient(a,b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1*(DBL_MAX_EXP+Bias-1))
#define Big1 0xffffffff

#undef Pack_16
#if !defined(Pack_32)
#define Pack_32
#endif

#if defined(NO_LONG_LONG)
#undef ULLong
#if defined(Just_16)
#undef Pack_32
#define Pack_16





#endif
#else
#if !defined(Llong)
#define Llong long long
#endif
#if !defined(ULLong)
#define ULLong unsigned Llong
#endif
#endif

#if defined(Pack_32)
#define ULbits 32
#define kshift 5
#define kmask 31
#define ALL_ON 0xffffffff
#else
#define ULbits 16
#define kshift 4
#define kmask 15
#define ALL_ON 0xffff
#endif

#define MULTIPLE_THREADS
extern pthread_mutex_t __gdtoa_locks[2];
#define ACQUIRE_DTOA_LOCK(n) do { if (__isthreaded) _pthread_mutex_lock(&__gdtoa_locks[n]); } while(0)



#define FREE_DTOA_LOCK(n) do { if (__isthreaded) _pthread_mutex_unlock(&__gdtoa_locks[n]); } while(0)




#define Kmax 9

struct
Bigint {
struct Bigint *next;
int k, maxwds, sign, wds;
ULong x[1];
};

typedef struct Bigint Bigint;

#if defined(NO_STRING_H)
#if defined(DECLARE_SIZE_T)
typedef unsigned int size_t;
#endif
extern void memcpy_D2A ANSI((void*, const void*, size_t));
#define Bcopy(x,y) memcpy_D2A(&x->sign,&y->sign,y->wds*sizeof(ULong) + 2*sizeof(int))
#else
#define Bcopy(x,y) memcpy(&x->sign,&y->sign,y->wds*sizeof(ULong) + 2*sizeof(int))
#endif





#define dtoa __dtoa
#define gdtoa __gdtoa
#define freedtoa __freedtoa
#define strtodg __strtodg
#define g_ddfmt __g_ddfmt
#define g_dfmt __g_dfmt
#define g_ffmt __g_ffmt
#define g_Qfmt __g_Qfmt
#define g_xfmt __g_xfmt
#define g_xLfmt __g_xLfmt
#define strtoId __strtoId
#define strtoIdd __strtoIdd
#define strtoIf __strtoIf
#define strtoIQ __strtoIQ
#define strtoIx __strtoIx
#define strtoIxL __strtoIxL
#define strtord_l __strtord_l
#define strtordd __strtordd
#define strtorf __strtorf
#define strtorQ_l __strtorQ_l
#define strtorx_l __strtorx_l
#define strtorxL __strtorxL
#define strtodI __strtodI
#define strtopd __strtopd
#define strtopdd __strtopdd
#define strtopf __strtopf
#define strtopQ __strtopQ
#define strtopx __strtopx
#define strtopxL __strtopxL


#define Balloc __Balloc_D2A
#define Bfree __Bfree_D2A
#define ULtoQ __ULtoQ_D2A
#define ULtof __ULtof_D2A
#define ULtod __ULtod_D2A
#define ULtodd __ULtodd_D2A
#define ULtox __ULtox_D2A
#define ULtoxL __ULtoxL_D2A
#define any_on __any_on_D2A
#define b2d __b2d_D2A
#define bigtens __bigtens_D2A
#define cmp __cmp_D2A
#define copybits __copybits_D2A
#define d2b __d2b_D2A
#define decrement __decrement_D2A
#define diff __diff_D2A
#define dtoa_result __dtoa_result_D2A
#define g__fmt __g__fmt_D2A
#define gethex __gethex_D2A
#define hexdig __hexdig_D2A
#define hexdig_init_D2A __hexdig_init_D2A
#define hexnan __hexnan_D2A
#define hi0bits __hi0bits_D2A
#define hi0bits_D2A __hi0bits_D2A
#define i2b __i2b_D2A
#define increment __increment_D2A
#define lo0bits __lo0bits_D2A
#define lshift __lshift_D2A
#define match __match_D2A
#define mult __mult_D2A
#define multadd __multadd_D2A
#define nrv_alloc __nrv_alloc_D2A
#define pow5mult __pow5mult_D2A
#define quorem __quorem_D2A
#define ratio __ratio_D2A
#define rshift __rshift_D2A
#define rv_alloc __rv_alloc_D2A
#define s2b __s2b_D2A
#define set_ones __set_ones_D2A
#define strcp __strcp_D2A
#define strcp_D2A __strcp_D2A
#define strtoIg __strtoIg_D2A
#define sum __sum_D2A
#define tens __tens_D2A
#define tinytens __tinytens_D2A
#define tinytens __tinytens_D2A
#define trailz __trailz_D2A
#define ulp __ulp_D2A

extern char *dtoa_result;
extern CONST double bigtens[], tens[], tinytens[];
extern unsigned char hexdig[];

extern Bigint *Balloc ANSI((int));
extern void Bfree ANSI((Bigint*));
extern void ULtof ANSI((ULong*, ULong*, Long, int));
extern void ULtod ANSI((ULong*, ULong*, Long, int));
extern void ULtodd ANSI((ULong*, ULong*, Long, int));
extern void ULtoQ ANSI((ULong*, ULong*, Long, int));
extern void ULtox ANSI((UShort*, ULong*, Long, int));
extern void ULtoxL ANSI((ULong*, ULong*, Long, int));
extern ULong any_on ANSI((Bigint*, int));
extern double b2d ANSI((Bigint*, int*));
extern int cmp ANSI((Bigint*, Bigint*));
extern void copybits ANSI((ULong*, int, Bigint*));
extern Bigint *d2b ANSI((double, int*, int*));
extern void decrement ANSI((Bigint*));
extern Bigint *diff ANSI((Bigint*, Bigint*));
extern char *dtoa ANSI((double d, int mode, int ndigits,
int *decpt, int *sign, char **rve));
extern void freedtoa ANSI((char*));
extern char *gdtoa ANSI((FPI *fpi, int be, ULong *bits, int *kindp,
int mode, int ndigits, int *decpt, char **rve));
extern char *g__fmt ANSI((char*, char*, char*, int, ULong, size_t));
extern int gethex ANSI((CONST char**, FPI*, Long*, Bigint**, int));
extern void hexdig_init_D2A(Void);
extern int hexnan ANSI((CONST char**, FPI*, ULong*));
extern int hi0bits ANSI((ULong));
extern Bigint *i2b ANSI((int));
extern Bigint *increment ANSI((Bigint*));
extern int lo0bits ANSI((ULong*));
extern Bigint *lshift ANSI((Bigint*, int));
extern int match ANSI((CONST char**, char*));
extern Bigint *mult ANSI((Bigint*, Bigint*));
extern Bigint *multadd ANSI((Bigint*, int, int));
extern char *nrv_alloc ANSI((char*, char **, int));
extern Bigint *pow5mult ANSI((Bigint*, int));
extern int quorem ANSI((Bigint*, Bigint*));
extern double ratio ANSI((Bigint*, Bigint*));
extern void rshift ANSI((Bigint*, int));
extern char *rv_alloc ANSI((int));
extern Bigint *s2b ANSI((CONST char*, int, int, ULong, int));
extern Bigint *set_ones ANSI((Bigint*, int));
extern char *strcp ANSI((char*, const char*));
extern int strtodg_l ANSI((CONST char*, char**, FPI*, Long*, ULong*, locale_t));

extern int strtoId ANSI((CONST char *, char **, double *, double *));
extern int strtoIdd ANSI((CONST char *, char **, double *, double *));
extern int strtoIf ANSI((CONST char *, char **, float *, float *));
extern int strtoIg ANSI((CONST char*, char**, FPI*, Long*, Bigint**, int*));
extern int strtoIQ ANSI((CONST char *, char **, void *, void *));
extern int strtoIx ANSI((CONST char *, char **, void *, void *));
extern int strtoIxL ANSI((CONST char *, char **, void *, void *));
extern double strtod ANSI((const char *s00, char **se));
extern double strtod_l ANSI((const char *s00, char **se, locale_t));
extern int strtopQ ANSI((CONST char *, char **, Void *));
extern int strtopf ANSI((CONST char *, char **, float *));
extern int strtopd ANSI((CONST char *, char **, double *));
extern int strtopdd ANSI((CONST char *, char **, double *));
extern int strtopx ANSI((CONST char *, char **, Void *));
extern int strtopxL ANSI((CONST char *, char **, Void *));
extern int strtord_l ANSI((CONST char *, char **, int, double *, locale_t));
extern int strtordd ANSI((CONST char *, char **, int, double *));
extern int strtorf ANSI((CONST char *, char **, int, float *));
extern int strtorQ_l ANSI((CONST char *, char **, int, void *, locale_t));
extern int strtorx_l ANSI((CONST char *, char **, int, void *, locale_t));
extern int strtorxL ANSI((CONST char *, char **, int, void *));
extern Bigint *sum ANSI((Bigint*, Bigint*));
extern int trailz ANSI((Bigint*));
extern double ulp ANSI((U*));

#if defined(__cplusplus)
}
#endif









#if defined(IEEE_Arith)
#if !defined(NO_INFNAN_CHECK)
#undef INFNAN_CHECK
#define INFNAN_CHECK
#endif
#if defined(IEEE_MC68k)
#define _0 0
#define _1 1
#if !defined(NAN_WORD0)
#define NAN_WORD0 d_QNAN0
#endif
#if !defined(NAN_WORD1)
#define NAN_WORD1 d_QNAN1
#endif
#else
#define _0 1
#define _1 0
#if !defined(NAN_WORD0)
#define NAN_WORD0 d_QNAN1
#endif
#if !defined(NAN_WORD1)
#define NAN_WORD1 d_QNAN0
#endif
#endif
#else
#undef INFNAN_CHECK
#endif

#undef SI
#if defined(Sudden_Underflow)
#define SI 1
#else
#define SI 0
#endif

#endif
