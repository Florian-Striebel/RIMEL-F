







#if !defined(__HTMINTRIN_H)
#define __HTMINTRIN_H

#if !defined(__HTM__)
#error "HTM instruction set not enabled"
#endif

#if defined(__powerpc__)

#include <stdint.h>

typedef uint64_t texasr_t;
typedef uint32_t texasru_t;
typedef uint32_t texasrl_t;
typedef uintptr_t tfiar_t;
typedef uintptr_t tfhar_t;

#define _HTM_STATE(CR0) ((CR0 >> 1) & 0x3)
#define _HTM_NONTRANSACTIONAL 0x0
#define _HTM_SUSPENDED 0x1
#define _HTM_TRANSACTIONAL 0x2

#define _TEXASR_EXTRACT_BITS(TEXASR,BITNUM,SIZE) (((TEXASR) >> (63-(BITNUM))) & ((1<<(SIZE))-1))

#define _TEXASRU_EXTRACT_BITS(TEXASR,BITNUM,SIZE) (((TEXASR) >> (31-(BITNUM))) & ((1<<(SIZE))-1))


#define _TEXASR_FAILURE_CODE(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 7, 8)

#define _TEXASRU_FAILURE_CODE(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 7, 8)


#define _TEXASR_FAILURE_PERSISTENT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 7, 1)

#define _TEXASRU_FAILURE_PERSISTENT(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 7, 1)


#define _TEXASR_DISALLOWED(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 8, 1)

#define _TEXASRU_DISALLOWED(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 8, 1)


#define _TEXASR_NESTING_OVERFLOW(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 9, 1)

#define _TEXASRU_NESTING_OVERFLOW(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 9, 1)


#define _TEXASR_FOOTPRINT_OVERFLOW(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 10, 1)

#define _TEXASRU_FOOTPRINT_OVERFLOW(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 10, 1)


#define _TEXASR_SELF_INDUCED_CONFLICT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 11, 1)

#define _TEXASRU_SELF_INDUCED_CONFLICT(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 11, 1)


#define _TEXASR_NON_TRANSACTIONAL_CONFLICT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 12, 1)

#define _TEXASRU_NON_TRANSACTIONAL_CONFLICT(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 12, 1)


#define _TEXASR_TRANSACTION_CONFLICT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 13, 1)

#define _TEXASRU_TRANSACTION_CONFLICT(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 13, 1)


#define _TEXASR_TRANSLATION_INVALIDATION_CONFLICT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 14, 1)

#define _TEXASRU_TRANSLATION_INVALIDATION_CONFLICT(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 14, 1)


#define _TEXASR_IMPLEMENTAION_SPECIFIC(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 15, 1)

#define _TEXASRU_IMPLEMENTAION_SPECIFIC(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 15, 1)


#define _TEXASR_INSTRUCTION_FETCH_CONFLICT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 16, 1)

#define _TEXASRU_INSTRUCTION_FETCH_CONFLICT(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 16, 1)


#define _TEXASR_ABORT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 31, 1)

#define _TEXASRU_ABORT(TEXASRU) _TEXASRU_EXTRACT_BITS(TEXASRU, 31, 1)



#define _TEXASR_SUSPENDED(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 32, 1)


#define _TEXASR_PRIVILEGE(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 35, 2)


#define _TEXASR_FAILURE_SUMMARY(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 36, 1)


#define _TEXASR_TFIAR_EXACT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 37, 1)


#define _TEXASR_ROT(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 38, 1)


#define _TEXASR_TRANSACTION_LEVEL(TEXASR) _TEXASR_EXTRACT_BITS(TEXASR, 63, 12)


#endif

#if defined(__s390__)


#define _HTM_TBEGIN_STARTED 0
#define _HTM_TBEGIN_INDETERMINATE 1
#define _HTM_TBEGIN_TRANSIENT 2
#define _HTM_TBEGIN_PERSISTENT 3


#define _HTM_FIRST_USER_ABORT_CODE 256




struct __htm_tdb {
unsigned char format;
unsigned char flags;
unsigned char reserved1[4];
unsigned short nesting_depth;
unsigned long long abort_code;
unsigned long long conflict_token;
unsigned long long atia;
unsigned char eaid;
unsigned char dxc;
unsigned char reserved2[2];
unsigned int program_int_id;
unsigned long long exception_id;
unsigned long long bea;
unsigned char reserved3[72];
unsigned long long gprs[16];
} __attribute__((__packed__, __aligned__ (8)));




static __inline int __attribute__((__always_inline__, __nodebug__))
__builtin_tbegin_retry_null (int __retry)
{
int cc, i = 0;

while ((cc = __builtin_tbegin(0)) == _HTM_TBEGIN_TRANSIENT
&& i++ < __retry)
__builtin_tx_assist(i);

return cc;
}

static __inline int __attribute__((__always_inline__, __nodebug__))
__builtin_tbegin_retry_tdb (void *__tdb, int __retry)
{
int cc, i = 0;

while ((cc = __builtin_tbegin(__tdb)) == _HTM_TBEGIN_TRANSIENT
&& i++ < __retry)
__builtin_tx_assist(i);

return cc;
}

#define __builtin_tbegin_retry(tdb, retry) (__builtin_constant_p(tdb == 0) && tdb == 0 ? __builtin_tbegin_retry_null(retry) : __builtin_tbegin_retry_tdb(tdb, retry))




static __inline int __attribute__((__always_inline__, __nodebug__))
__builtin_tbegin_retry_nofloat_null (int __retry)
{
int cc, i = 0;

while ((cc = __builtin_tbegin_nofloat(0)) == _HTM_TBEGIN_TRANSIENT
&& i++ < __retry)
__builtin_tx_assist(i);

return cc;
}

static __inline int __attribute__((__always_inline__, __nodebug__))
__builtin_tbegin_retry_nofloat_tdb (void *__tdb, int __retry)
{
int cc, i = 0;

while ((cc = __builtin_tbegin_nofloat(__tdb)) == _HTM_TBEGIN_TRANSIENT
&& i++ < __retry)
__builtin_tx_assist(i);

return cc;
}

#define __builtin_tbegin_retry_nofloat(tdb, retry) (__builtin_constant_p(tdb == 0) && tdb == 0 ? __builtin_tbegin_retry_nofloat_null(retry) : __builtin_tbegin_retry_nofloat_tdb(tdb, retry))




#endif

#endif
