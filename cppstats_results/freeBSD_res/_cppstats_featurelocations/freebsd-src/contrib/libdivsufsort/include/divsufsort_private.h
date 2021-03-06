

























#if !defined(_DIVSUFSORT_PRIVATE_H)
#define _DIVSUFSORT_PRIVATE_H 1

#if defined(__cplusplus)
extern "C" {
#endif

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <assert.h>
#include <stdio.h>
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_MEMORY_H
#include <memory.h>
#endif
#if HAVE_STDDEF_H
#include <stddef.h>
#endif
#if HAVE_STRINGS_H
#include <strings.h>
#endif
#if HAVE_INTTYPES_H
#include <inttypes.h>
#else
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#endif
#if defined(BUILD_DIVSUFSORT64)
#include "divsufsort64.h"
#if !defined(SAIDX_T)
#define SAIDX_T
#define saidx_t saidx64_t
#endif
#if !defined(PRIdSAIDX_T)
#define PRIdSAIDX_T PRIdSAIDX64_T
#endif
#define divsufsort divsufsort64
#define divbwt divbwt64
#define divsufsort_version divsufsort64_version
#define bw_transform bw_transform64
#define inverse_bw_transform inverse_bw_transform64
#define sufcheck sufcheck64
#define sa_search sa_search64
#define sa_simplesearch sa_simplesearch64
#define sssort sssort64
#define trsort trsort64
#else
#include "divsufsort.h"
#endif



#if !defined(UINT8_MAX)
#define UINT8_MAX (255)
#endif
#if defined(ALPHABET_SIZE) && (ALPHABET_SIZE < 1)
#undef ALPHABET_SIZE
#endif
#if !defined(ALPHABET_SIZE)
#define ALPHABET_SIZE (UINT8_MAX + 1)
#endif

#define BUCKET_A_SIZE (ALPHABET_SIZE)
#define BUCKET_B_SIZE (ALPHABET_SIZE * ALPHABET_SIZE)

#if defined(SS_INSERTIONSORT_THRESHOLD)
#if SS_INSERTIONSORT_THRESHOLD < 1
#undef SS_INSERTIONSORT_THRESHOLD
#define SS_INSERTIONSORT_THRESHOLD (1)
#endif
#else
#define SS_INSERTIONSORT_THRESHOLD (8)
#endif
#if defined(SS_BLOCKSIZE)
#if SS_BLOCKSIZE < 0
#undef SS_BLOCKSIZE
#define SS_BLOCKSIZE (0)
#elif 32768 <= SS_BLOCKSIZE
#undef SS_BLOCKSIZE
#define SS_BLOCKSIZE (32767)
#endif
#else
#define SS_BLOCKSIZE (1024)
#endif

#if SS_BLOCKSIZE == 0
#if defined(BUILD_DIVSUFSORT64)
#define SS_MISORT_STACKSIZE (96)
#else
#define SS_MISORT_STACKSIZE (64)
#endif
#elif SS_BLOCKSIZE <= 4096
#define SS_MISORT_STACKSIZE (16)
#else
#define SS_MISORT_STACKSIZE (24)
#endif
#if defined(BUILD_DIVSUFSORT64)
#define SS_SMERGE_STACKSIZE (64)
#else
#define SS_SMERGE_STACKSIZE (32)
#endif

#define TR_INSERTIONSORT_THRESHOLD (8)
#if defined(BUILD_DIVSUFSORT64)
#define TR_STACKSIZE (96)
#else
#define TR_STACKSIZE (64)
#endif



#if !defined(SWAP)
#define SWAP(_a, _b) do { t = (_a); (_a) = (_b); (_b) = t; } while(0)
#endif
#if !defined(MIN)
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#endif
#if !defined(MAX)
#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#endif
#define STACK_PUSH(_a, _b, _c, _d)do {assert(ssize < STACK_SIZE);stack[ssize].a = (_a), stack[ssize].b = (_b),stack[ssize].c = (_c), stack[ssize++].d = (_d);} while(0)





#define STACK_PUSH5(_a, _b, _c, _d, _e)do {assert(ssize < STACK_SIZE);stack[ssize].a = (_a), stack[ssize].b = (_b),stack[ssize].c = (_c), stack[ssize].d = (_d), stack[ssize++].e = (_e);} while(0)





#define STACK_POP(_a, _b, _c, _d)do {assert(0 <= ssize);if(ssize == 0) { return; }(_a) = stack[--ssize].a, (_b) = stack[ssize].b,(_c) = stack[ssize].c, (_d) = stack[ssize].d;} while(0)






#define STACK_POP5(_a, _b, _c, _d, _e)do {assert(0 <= ssize);if(ssize == 0) { return; }(_a) = stack[--ssize].a, (_b) = stack[ssize].b,(_c) = stack[ssize].c, (_d) = stack[ssize].d, (_e) = stack[ssize].e;} while(0)







#define BUCKET_A(_c0) bucket_A[(_c0)]
#if ALPHABET_SIZE == 256
#define BUCKET_B(_c0, _c1) (bucket_B[((_c1) << 8) | (_c0)])
#define BUCKET_BSTAR(_c0, _c1) (bucket_B[((_c0) << 8) | (_c1)])
#else
#define BUCKET_B(_c0, _c1) (bucket_B[(_c1) * ALPHABET_SIZE + (_c0)])
#define BUCKET_BSTAR(_c0, _c1) (bucket_B[(_c0) * ALPHABET_SIZE + (_c1)])
#endif




void
sssort(const sauchar_t *Td, const saidx_t *PA,
saidx_t *first, saidx_t *last,
saidx_t *buf, saidx_t bufsize,
saidx_t depth, saidx_t n, saint_t lastsuffix);

void
trsort(saidx_t *ISA, saidx_t *SA, saidx_t n, saidx_t depth);


#if defined(__cplusplus)
}
#endif

#endif
