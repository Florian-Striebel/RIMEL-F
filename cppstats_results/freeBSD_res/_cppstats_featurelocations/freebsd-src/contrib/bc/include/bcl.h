


































#if !defined(BC_BCL_H)
#define BC_BCL_H

#if defined(_WIN32)
#include <Windows.h>
#include <BaseTsd.h>
#include <stdio.h>
#include <io.h>
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>



#if defined(_WIN32)




#define inline

#define restrict __restrict
#define strdup _strdup
#define write(f, b, s) _write((f), (b), (unsigned int) (s))
#define read(f, b, s) _read((f), (b), (unsigned int) (s))
#define close _close
#define open(f, n, m) _sopen_s((f), (n), (m) | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)

#define sigjmp_buf jmp_buf
#define sigsetjmp(j, s) setjmp(j)
#define siglongjmp longjmp
#define isatty _isatty
#define STDIN_FILENO _fileno(stdin)
#define STDOUT_FILENO _fileno(stdout)
#define STDERR_FILENO _fileno(stderr)
#define ssize_t SSIZE_T
#define S_ISDIR(m) ((m) & _S_IFDIR)
#define O_RDONLY _O_RDONLY
#define stat _stat
#define fstat _fstat
#define BC_FILE_SEP '\\'

#else
#define BC_FILE_SEP '/'
#endif

#define BCL_SEED_ULONGS (4)
#define BCL_SEED_SIZE (sizeof(long) * BCL_SEED_ULONGS)



#if !defined(LONG_BIT)
#define LONG_BIT (32)
#endif

#if !defined(BC_LONG_BIT)
#define BC_LONG_BIT LONG_BIT
#endif

#if BC_LONG_BIT > LONG_BIT
#error BC_LONG_BIT cannot be greater than LONG_BIT
#endif








#if BC_LONG_BIT >= 64

typedef uint64_t BclBigDig;
typedef uint64_t BclRandInt;

#elif BC_LONG_BIT >= 32

typedef uint32_t BclBigDig;
typedef uint32_t BclRandInt;

#else

#error BC_LONG_BIT must be at least 32

#endif

#if !defined(BC_ENABLE_LIBRARY)
#define BC_ENABLE_LIBRARY (1)
#endif

#if BC_ENABLE_LIBRARY

typedef enum BclError {

BCL_ERROR_NONE,

BCL_ERROR_INVALID_NUM,
BCL_ERROR_INVALID_CONTEXT,
BCL_ERROR_SIGNAL,

BCL_ERROR_MATH_NEGATIVE,
BCL_ERROR_MATH_NON_INTEGER,
BCL_ERROR_MATH_OVERFLOW,
BCL_ERROR_MATH_DIVIDE_BY_ZERO,

BCL_ERROR_PARSE_INVALID_STR,

BCL_ERROR_FATAL_ALLOC_ERR,
BCL_ERROR_FATAL_UNKNOWN_ERR,

BCL_ERROR_NELEMS,

} BclError;

typedef struct BclNumber {

size_t i;

} BclNumber;

struct BclCtxt;

typedef struct BclCtxt* BclContext;

void bcl_handleSignal(void);
bool bcl_running(void);

BclError bcl_init(void);
void bcl_free(void);

bool bcl_abortOnFatalError(void);
void bcl_setAbortOnFatalError(bool abrt);
bool bcl_leadingZeroes(void);
void bcl_setLeadingZeroes(bool leadingZeroes);

void bcl_gc(void);

BclError bcl_pushContext(BclContext ctxt);
void bcl_popContext(void);
BclContext bcl_context(void);

BclContext bcl_ctxt_create(void);
void bcl_ctxt_free(BclContext ctxt);
void bcl_ctxt_freeNums(BclContext ctxt);

size_t bcl_ctxt_scale(BclContext ctxt);
void bcl_ctxt_setScale(BclContext ctxt, size_t scale);
size_t bcl_ctxt_ibase(BclContext ctxt);
void bcl_ctxt_setIbase(BclContext ctxt, size_t ibase);
size_t bcl_ctxt_obase(BclContext ctxt);
void bcl_ctxt_setObase(BclContext ctxt, size_t obase);

BclError bcl_err(BclNumber n);

BclNumber bcl_num_create(void);
void bcl_num_free(BclNumber n);

bool bcl_num_neg(BclNumber n);
void bcl_num_setNeg(BclNumber n, bool neg);
size_t bcl_num_scale(BclNumber n);
BclError bcl_num_setScale(BclNumber n, size_t scale);
size_t bcl_num_len(BclNumber n);

BclError bcl_copy(BclNumber d, BclNumber s);
BclNumber bcl_dup(BclNumber s);

BclError bcl_bigdig(BclNumber n, BclBigDig *result);
BclNumber bcl_bigdig2num(BclBigDig val);

BclNumber bcl_add(BclNumber a, BclNumber b);
BclNumber bcl_sub(BclNumber a, BclNumber b);
BclNumber bcl_mul(BclNumber a, BclNumber b);
BclNumber bcl_div(BclNumber a, BclNumber b);
BclNumber bcl_mod(BclNumber a, BclNumber b);
BclNumber bcl_pow(BclNumber a, BclNumber b);
BclNumber bcl_lshift(BclNumber a, BclNumber b);
BclNumber bcl_rshift(BclNumber a, BclNumber b);
BclNumber bcl_sqrt(BclNumber a);
BclError bcl_divmod(BclNumber a, BclNumber b, BclNumber *c, BclNumber *d);
BclNumber bcl_modexp(BclNumber a, BclNumber b, BclNumber c);

ssize_t bcl_cmp(BclNumber a, BclNumber b);

void bcl_zero(BclNumber n);
void bcl_one(BclNumber n);

BclNumber bcl_parse(const char *restrict val);
char* bcl_string(BclNumber n);

BclNumber bcl_irand(BclNumber a);
BclNumber bcl_frand(size_t places);
BclNumber bcl_ifrand(BclNumber a, size_t places);

BclError bcl_rand_seedWithNum(BclNumber n);
BclError bcl_rand_seed(unsigned char seed[BCL_SEED_SIZE]);
void bcl_rand_reseed(void);
BclNumber bcl_rand_seed2num(void);
BclRandInt bcl_rand_int(void);
BclRandInt bcl_rand_bounded(BclRandInt bound);

#endif

#endif
