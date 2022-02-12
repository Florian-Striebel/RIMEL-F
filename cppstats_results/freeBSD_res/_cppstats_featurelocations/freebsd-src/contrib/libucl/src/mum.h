









































#if !defined(__MUM_HASH__)
#define __MUM_HASH__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if defined(_MSC_VER)
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif



#if !defined(_MUM_USE_INT128)



#if defined(__GNUC__) && UINT_MAX != ULONG_MAX
#define _MUM_USE_INT128 1
#else
#define _MUM_USE_INT128 0
#endif
#endif

#if 0
#if defined(__GNUC__) && ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 9) || (__GNUC__ > 4))
#define _MUM_FRESH_GCC
#endif
#endif

#if defined(__GNUC__) && !defined(__llvm__) && defined(_MUM_FRESH_GCC)
#define _MUM_ATTRIBUTE_UNUSED __attribute__((unused))
#define _MUM_OPTIMIZE(opts) __attribute__((__optimize__ (opts)))
#define _MUM_TARGET(opts) __attribute__((__target__ (opts)))
#else
#define _MUM_ATTRIBUTE_UNUSED
#define _MUM_OPTIMIZE(opts)
#define _MUM_TARGET(opts)
#endif





static uint64_t _mum_hash_step_prime = 0x2e0bb864e9ea7df5ULL;
static uint64_t _mum_key_step_prime = 0xcdb32970830fcaa1ULL;
static uint64_t _mum_block_start_prime = 0xc42b5e2e6480b23bULL;
static uint64_t _mum_unroll_prime = 0x7b51ec3d22f7096fULL;
static uint64_t _mum_tail_prime = 0xaf47d47c99b1461bULL;
static uint64_t _mum_finish_prime1 = 0xa9a7ae7ceff79f3fULL;
static uint64_t _mum_finish_prime2 = 0xaf47d47c99b1461bULL;

static uint64_t _mum_primes [] = {
0X9ebdcae10d981691, 0X32b9b9b97a27ac7d, 0X29b5584d83d35bbd, 0X4b04e0e61401255f,
0X25e8f7b1f1c9d027, 0X80d4c8c000f3e881, 0Xbd1255431904b9dd, 0X8a3bd4485eee6d81,
0X3bc721b2aad05197, 0X71b1a19b907d6e33, 0X525e6c1084a8534b, 0X9e4c2cd340c1299f,
0Xde3add92e94caa37, 0X7e14eadb1f65311d, 0X3f5aa40f89812853, 0X33b15a3b587d15c9,
};



static inline uint64_t
_mum (uint64_t v, uint64_t p) {
uint64_t hi, lo;
#if _MUM_USE_INT128
#if defined(__aarch64__)




lo = v * p, hi;
asm ("umulh %0, %1, %2" : "=r" (hi) : "r" (v), "r" (p));
#else
__uint128_t r = (__uint128_t) v * (__uint128_t) p;
hi = (uint64_t) (r >> 64);
lo = (uint64_t) r;
#endif
#else


uint64_t hv = v >> 32, hp = p >> 32;
uint64_t lv = (uint32_t) v, lp = (uint32_t) p;
uint64_t rh = hv * hp;
uint64_t rm_0 = hv * lp;
uint64_t rm_1 = hp * lv;
uint64_t rl = lv * lp;
uint64_t t, carry = 0;



t = rl + (rm_0 << 32);
#if defined(MUM_TARGET_INDEPENDENT_HASH)
carry = t < rl;
#endif
lo = t + (rm_1 << 32);
#if defined(MUM_TARGET_INDEPENDENT_HASH)
carry += lo < t;
#endif
hi = rh + (rm_0 >> 32) + (rm_1 >> 32) + carry;
#endif



return hi + lo;
}

#if defined(_MSC_VER)
#define _mum_bswap_32(x) _byteswap_uint32_t (x)
#define _mum_bswap_64(x) _byteswap_uint64_t (x)
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define _mum_bswap_32(x) OSSwapInt32 (x)
#define _mum_bswap_64(x) OSSwapInt64 (x)
#elif defined(__GNUC__)
#define _mum_bswap32(x) __builtin_bswap32 (x)
#define _mum_bswap64(x) __builtin_bswap64 (x)
#else
#include <byteswap.h>
#define _mum_bswap32(x) bswap32 (x)
#define _mum_bswap64(x) bswap64 (x)
#endif

static inline uint64_t
_mum_le (uint64_t v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || !defined(MUM_TARGET_INDEPENDENT_HASH)
return v;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
return _mum_bswap64 (v);
#else
#error "Unknown endianness"
#endif
}

static inline uint32_t
_mum_le32 (uint32_t v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || !defined(MUM_TARGET_INDEPENDENT_HASH)
return v;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
return _mum_bswap32 (v);
#else
#error "Unknown endianness"
#endif
}









#if !defined(_MUM_UNROLL_FACTOR_POWER)
#if defined(__PPC64__) && !defined(MUM_TARGET_INDEPENDENT_HASH)
#define _MUM_UNROLL_FACTOR_POWER 3
#elif defined(__aarch64__) && !defined(MUM_TARGET_INDEPENDENT_HASH)
#define _MUM_UNROLL_FACTOR_POWER 4
#else
#define _MUM_UNROLL_FACTOR_POWER 2
#endif
#endif

#if _MUM_UNROLL_FACTOR_POWER < 1
#error "too small unroll factor"
#elif _MUM_UNROLL_FACTOR_POWER > 4
#error "We have not enough primes for such unroll factor"
#endif

#define _MUM_UNROLL_FACTOR (1 << _MUM_UNROLL_FACTOR_POWER)

static inline uint64_t _MUM_OPTIMIZE("unroll-loops")
_mum_hash_aligned (uint64_t start, const void *key, size_t len) {
uint64_t result = start;
const unsigned char *str = (const unsigned char *) key;
uint64_t u64;
int i;
size_t n;

result = _mum (result, _mum_block_start_prime);
while (len > _MUM_UNROLL_FACTOR * sizeof (uint64_t)) {



for (i = 0; i < _MUM_UNROLL_FACTOR; i++)
result ^= _mum (_mum_le (((uint64_t *) str)[i]), _mum_primes[i]);
len -= _MUM_UNROLL_FACTOR * sizeof (uint64_t);
str += _MUM_UNROLL_FACTOR * sizeof (uint64_t);


result = _mum (result, _mum_unroll_prime);
}
n = len / sizeof (uint64_t);
for (i = 0; i < (int)n; i++)
result ^= _mum (_mum_le (((uint64_t *) str)[i]), _mum_primes[i]);
len -= n * sizeof (uint64_t); str += n * sizeof (uint64_t);
switch (len) {
case 7:
u64 = _mum_le32 (*(uint32_t *) str);
u64 |= (uint64_t) str[4] << 32;
u64 |= (uint64_t) str[5] << 40;
u64 |= (uint64_t) str[6] << 48;
return result ^ _mum (u64, _mum_tail_prime);
case 6:
u64 = _mum_le32 (*(uint32_t *) str);
u64 |= (uint64_t) str[4] << 32;
u64 |= (uint64_t) str[5] << 40;
return result ^ _mum (u64, _mum_tail_prime);
case 5:
u64 = _mum_le32 (*(uint32_t *) str);
u64 |= (uint64_t) str[4] << 32;
return result ^ _mum (u64, _mum_tail_prime);
case 4:
u64 = _mum_le32 (*(uint32_t *) str);
return result ^ _mum (u64, _mum_tail_prime);
case 3:
u64 = str[0];
u64 |= (uint64_t) str[1] << 8;
u64 |= (uint64_t) str[2] << 16;
return result ^ _mum (u64, _mum_tail_prime);
case 2:
u64 = str[0];
u64 |= (uint64_t) str[1] << 8;
return result ^ _mum (u64, _mum_tail_prime);
case 1:
u64 = str[0];
return result ^ _mum (u64, _mum_tail_prime);
}
return result;
}


static inline uint64_t
_mum_final (uint64_t h) {
h ^= _mum (h, _mum_finish_prime1);
h ^= _mum (h, _mum_finish_prime2);
return h;
}

#if defined(__x86_64__) && defined(_MUM_FRESH_GCC)





static inline uint64_t _MUM_TARGET("arch=haswell")
_mum_hash_avx2 (const void * key, size_t len, uint64_t seed) {
return _mum_final (_mum_hash_aligned (seed + len, key, len));
}
#endif

#if !defined(_MUM_UNALIGNED_ACCESS)
#if defined(__x86_64__) || defined(__i386__) || defined(__PPC64__) || defined(__s390__) || defined(__m32c__) || defined(cris) || defined(__CR16__) || defined(__vax__) || defined(__m68k__) || defined(__aarch64__)



#define _MUM_UNALIGNED_ACCESS 1
#else
#define _MUM_UNALIGNED_ACCESS 0
#endif
#endif




#if !defined(_MUM_BLOCK_LEN)
#define _MUM_BLOCK_LEN 1024
#endif

#if _MUM_BLOCK_LEN < 8
#error "too small block length"
#endif

static inline uint64_t
#if defined(__x86_64__)
_MUM_TARGET("inline-all-stringops")
#endif
_mum_hash_default (const void *key, size_t len, uint64_t seed) {
uint64_t result;
const unsigned char *str = (const unsigned char *) key;
size_t block_len;
uint64_t buf[_MUM_BLOCK_LEN / sizeof (uint64_t)];

result = seed + len;
if (_MUM_UNALIGNED_ACCESS || ((size_t) str & 0x7) == 0)
result = _mum_hash_aligned (result, key, len);
else {
while (len != 0) {
block_len = len < _MUM_BLOCK_LEN ? len : _MUM_BLOCK_LEN;
memmove (buf, str, block_len);
result = _mum_hash_aligned (result, buf, block_len);
len -= block_len;
str += block_len;
}
}
return _mum_final (result);
}

static inline uint64_t
_mum_next_factor (void) {
uint64_t start = 0;
int i;

for (i = 0; i < 8; i++)
start = (start << 8) | rand() % 256;
return start;
}




static inline void
mum_hash_randomize (uint64_t seed) {
int i;

srand (seed);
_mum_hash_step_prime = _mum_next_factor ();
_mum_key_step_prime = _mum_next_factor ();
_mum_finish_prime1 = _mum_next_factor ();
_mum_finish_prime2 = _mum_next_factor ();
_mum_block_start_prime = _mum_next_factor ();
_mum_unroll_prime = _mum_next_factor ();
_mum_tail_prime = _mum_next_factor ();
for (i = 0; i < (int)(sizeof (_mum_primes) / sizeof (uint64_t)); i++)
_mum_primes[i] = _mum_next_factor ();
}


static inline uint64_t
mum_hash_init (uint64_t seed) {
return seed;
}


static inline uint64_t
mum_hash_step (uint64_t h, uint64_t key)
{
return _mum (h, _mum_hash_step_prime) ^ _mum (key, _mum_key_step_prime);
}


static inline uint64_t
mum_hash_finish (uint64_t h) {
return _mum_final (h);
}



static inline size_t
mum_hash64 (uint64_t key, uint64_t seed) {
return mum_hash_finish (mum_hash_step (mum_hash_init (seed), key));
}



static inline uint64_t
mum_hash (const void *key, size_t len, uint64_t seed) {
#if defined(__x86_64__) && defined(_MUM_FRESH_GCC)
static int avx2_support = 0;

if (avx2_support > 0)
return _mum_hash_avx2 (key, len, seed);
else if (! avx2_support) {
__builtin_cpu_init ();
avx2_support = __builtin_cpu_supports ("avx2") ? 1 : -1;
if (avx2_support > 0)
return _mum_hash_avx2 (key, len, seed);
}
#endif
return _mum_hash_default (key, len, seed);
}

#endif
