







#if !defined(__HTMXLINTRIN_H)
#define __HTMXLINTRIN_H

#if !defined(__HTM__)
#error "HTM instruction set not enabled"
#endif

#include <htmintrin.h>

#if defined(__powerpc__)

#if defined(__cplusplus)
extern "C" {
#endif

#define _TEXASR_PTR(TM_BUF) ((texasr_t *)((char *)(TM_BUF) + 0))
#define _TEXASRU_PTR(TM_BUF) ((texasru_t *)((char *)(TM_BUF) + 0))
#define _TEXASRL_PTR(TM_BUF) ((texasrl_t *)((char *)(TM_BUF) + 4))
#define _TFIAR_PTR(TM_BUF) ((tfiar_t *)((char *)(TM_BUF) + 8))

typedef char TM_buff_type[16];




#define _HTM_TBEGIN_STARTED 1

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_simple_begin (void)
{
if (__builtin_expect (__builtin_tbegin (0), 1))
return _HTM_TBEGIN_STARTED;
return 0;
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_begin (void* const __TM_buff)
{
*_TEXASRL_PTR (__TM_buff) = 0;
if (__builtin_expect (__builtin_tbegin (0), 1))
return _HTM_TBEGIN_STARTED;
#if defined(__powerpc64__)
*_TEXASR_PTR (__TM_buff) = __builtin_get_texasr ();
#else
*_TEXASRU_PTR (__TM_buff) = __builtin_get_texasru ();
*_TEXASRL_PTR (__TM_buff) = __builtin_get_texasr ();
#endif
*_TFIAR_PTR (__TM_buff) = __builtin_get_tfiar ();
return 0;
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_end (void)
{
if (__builtin_expect (__builtin_tend (0), 1))
return 1;
return 0;
}

extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_abort (void)
{
__builtin_tabort (0);
}

extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_named_abort (unsigned char const __code)
{
__builtin_tabort (__code);
}

extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_resume (void)
{
__builtin_tresume ();
}

extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_suspend (void)
{
__builtin_tsuspend ();
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_is_user_abort (void* const __TM_buff)
{
texasru_t texasru = *_TEXASRU_PTR (__TM_buff);
return _TEXASRU_ABORT (texasru);
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_is_named_user_abort (void* const __TM_buff, unsigned char *__code)
{
texasru_t texasru = *_TEXASRU_PTR (__TM_buff);

*__code = _TEXASRU_FAILURE_CODE (texasru);
return _TEXASRU_ABORT (texasru);
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_is_illegal (void* const __TM_buff)
{
texasru_t texasru = *_TEXASRU_PTR (__TM_buff);
return _TEXASRU_DISALLOWED (texasru);
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_is_footprint_exceeded (void* const __TM_buff)
{
texasru_t texasru = *_TEXASRU_PTR (__TM_buff);
return _TEXASRU_FOOTPRINT_OVERFLOW (texasru);
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_nesting_depth (void* const __TM_buff)
{
texasrl_t texasrl;

if (_HTM_STATE (__builtin_ttest ()) == _HTM_NONTRANSACTIONAL)
{
texasrl = *_TEXASRL_PTR (__TM_buff);
if (!_TEXASR_FAILURE_SUMMARY (texasrl))
texasrl = 0;
}
else
texasrl = (texasrl_t) __builtin_get_texasr ();

return _TEXASR_TRANSACTION_LEVEL (texasrl);
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_is_nested_too_deep(void* const __TM_buff)
{
texasru_t texasru = *_TEXASRU_PTR (__TM_buff);
return _TEXASRU_NESTING_OVERFLOW (texasru);
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_is_conflict(void* const __TM_buff)
{
texasru_t texasru = *_TEXASRU_PTR (__TM_buff);


return (_TEXASRU_EXTRACT_BITS (texasru, 14, 4)) ? 1 : 0;
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_is_failure_persistent(void* const __TM_buff)
{
texasru_t texasru = *_TEXASRU_PTR (__TM_buff);
return _TEXASRU_FAILURE_PERSISTENT (texasru);
}

extern __inline long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_failure_address(void* const __TM_buff)
{
return *_TFIAR_PTR (__TM_buff);
}

extern __inline long long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
__TM_failure_code(void* const __TM_buff)
{
return *_TEXASR_PTR (__TM_buff);
}

#if defined(__cplusplus)
}
#endif

#endif

#if defined(__s390__)

#include <stdint.h>





static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_simple_begin ()
{
return __builtin_tbegin_nofloat (0);
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_begin (void* const __tdb)
{
return __builtin_tbegin_nofloat (__tdb);
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_end ()
{
return __builtin_tend ();
}

static __inline void __attribute__((__always_inline__))
__TM_abort ()
{
return __builtin_tabort (_HTM_FIRST_USER_ABORT_CODE);
}

static __inline void __attribute__((__always_inline__, __nodebug__))
__TM_named_abort (unsigned char const __code)
{
return __builtin_tabort ((int)_HTM_FIRST_USER_ABORT_CODE + __code);
}

static __inline void __attribute__((__always_inline__, __nodebug__))
__TM_non_transactional_store (void* const __addr, long long const __value)
{
__builtin_non_tx_store ((uint64_t*)__addr, (uint64_t)__value);
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_nesting_depth (void* const __tdb_ptr)
{
int depth = __builtin_tx_nesting_depth ();
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

if (depth != 0)
return depth;

if (tdb->format != 1)
return 0;
return tdb->nesting_depth;
}



static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_is_user_abort (void* const __tdb_ptr)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

if (tdb->format != 1)
return 0;

return !!(tdb->abort_code >= _HTM_FIRST_USER_ABORT_CODE);
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_is_named_user_abort (void* const __tdb_ptr, unsigned char* __code)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

if (tdb->format != 1)
return 0;

if (tdb->abort_code >= _HTM_FIRST_USER_ABORT_CODE)
{
*__code = tdb->abort_code - _HTM_FIRST_USER_ABORT_CODE;
return 1;
}
return 0;
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_is_illegal (void* const __tdb_ptr)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

return (tdb->format == 1
&& (tdb->abort_code == 4
|| tdb->abort_code == 11 ));
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_is_footprint_exceeded (void* const __tdb_ptr)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

return (tdb->format == 1
&& (tdb->abort_code == 7
|| tdb->abort_code == 8 ));
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_is_nested_too_deep (void* const __tdb_ptr)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

return tdb->format == 1 && tdb->abort_code == 13;
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_is_conflict (void* const __tdb_ptr)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

return (tdb->format == 1
&& (tdb->abort_code == 9
|| tdb->abort_code == 10 ));
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_is_failure_persistent (long const __result)
{
return __result == _HTM_TBEGIN_PERSISTENT;
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_failure_address (void* const __tdb_ptr)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;
return tdb->atia;
}

static __inline long __attribute__((__always_inline__, __nodebug__))
__TM_failure_code (void* const __tdb_ptr)
{
struct __htm_tdb *tdb = (struct __htm_tdb*)__tdb_ptr;

return tdb->abort_code;
}

#endif

#endif
