
























#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "assembly.h"


#define memcpy __builtin_memcpy
#define memcmp __builtin_memcmp



#pragma redefine_extname __atomic_load_c SYMBOL_NAME(__atomic_load)
#pragma redefine_extname __atomic_store_c SYMBOL_NAME(__atomic_store)
#pragma redefine_extname __atomic_exchange_c SYMBOL_NAME(__atomic_exchange)
#pragma redefine_extname __atomic_compare_exchange_c SYMBOL_NAME( __atomic_compare_exchange)

#pragma redefine_extname __atomic_is_lock_free_c SYMBOL_NAME( __atomic_is_lock_free)





#if !defined(SPINLOCK_COUNT)
#define SPINLOCK_COUNT (1 << 10)
#endif
static const long SPINLOCK_MASK = SPINLOCK_COUNT - 1;






#if defined(__FreeBSD__) || defined(__DragonFly__)
#include <errno.h>

#include <sys/types.h>
#include <machine/atomic.h>
#include <sys/umtx.h>

typedef struct _usem Lock;
__inline static void unlock(Lock *l) {
__c11_atomic_store((_Atomic(uint32_t) *)&l->_count, 1, __ATOMIC_RELEASE);
__c11_atomic_thread_fence(__ATOMIC_SEQ_CST);
if (l->_has_waiters)
_umtx_op(l, UMTX_OP_SEM_WAKE, 1, 0, 0);
}
__inline static void lock(Lock *l) {
uint32_t old = 1;
while (!__c11_atomic_compare_exchange_weak((_Atomic(uint32_t) *)&l->_count,
&old, 0, __ATOMIC_ACQUIRE,
__ATOMIC_RELAXED)) {
_umtx_op(l, UMTX_OP_SEM_WAIT, 0, 0, 0);
old = 1;
}
}

static Lock locks[SPINLOCK_COUNT] = {[0 ... SPINLOCK_COUNT - 1] = {0, 1, 0}};

#elif defined(__APPLE__)
#include <libkern/OSAtomic.h>
typedef OSSpinLock Lock;
__inline static void unlock(Lock *l) { OSSpinLockUnlock(l); }


__inline static void lock(Lock *l) { OSSpinLockLock(l); }
static Lock locks[SPINLOCK_COUNT];

#else
typedef _Atomic(uintptr_t) Lock;

__inline static void unlock(Lock *l) {
__c11_atomic_store(l, 0, __ATOMIC_RELEASE);
}


__inline static void lock(Lock *l) {
uintptr_t old = 0;
while (!__c11_atomic_compare_exchange_weak(l, &old, 1, __ATOMIC_ACQUIRE,
__ATOMIC_RELAXED))
old = 0;
}

static Lock locks[SPINLOCK_COUNT];
#endif


static __inline Lock *lock_for_pointer(void *ptr) {
intptr_t hash = (intptr_t)ptr;



hash >>= 4;

intptr_t low = hash & SPINLOCK_MASK;


hash >>= 16;
hash ^= low;

return locks + (hash & SPINLOCK_MASK);
}


#define ATOMIC_ALWAYS_LOCK_FREE_OR_ALIGNED_LOCK_FREE(size, p) (__atomic_always_lock_free(size, p) || (__atomic_always_lock_free(size, 0) && ((uintptr_t)p % size) == 0))


#define IS_LOCK_FREE_1(p) ATOMIC_ALWAYS_LOCK_FREE_OR_ALIGNED_LOCK_FREE(1, p)
#define IS_LOCK_FREE_2(p) ATOMIC_ALWAYS_LOCK_FREE_OR_ALIGNED_LOCK_FREE(2, p)
#define IS_LOCK_FREE_4(p) ATOMIC_ALWAYS_LOCK_FREE_OR_ALIGNED_LOCK_FREE(4, p)
#define IS_LOCK_FREE_8(p) ATOMIC_ALWAYS_LOCK_FREE_OR_ALIGNED_LOCK_FREE(8, p)
#define IS_LOCK_FREE_16(p) ATOMIC_ALWAYS_LOCK_FREE_OR_ALIGNED_LOCK_FREE(16, p)



#define TRY_LOCK_FREE_CASE(n, type, ptr) case n: if (IS_LOCK_FREE_##n(ptr)) { LOCK_FREE_ACTION(type); } break;





#if defined(__SIZEOF_INT128__)
#define TRY_LOCK_FREE_CASE_16(p) TRY_LOCK_FREE_CASE(16, __uint128_t, p)
#else
#define TRY_LOCK_FREE_CASE_16(p)
#endif

#define LOCK_FREE_CASES(ptr) do { switch (size) { TRY_LOCK_FREE_CASE(1, uint8_t, ptr) TRY_LOCK_FREE_CASE(2, uint16_t, ptr) TRY_LOCK_FREE_CASE(4, uint32_t, ptr) TRY_LOCK_FREE_CASE(8, uint64_t, ptr) TRY_LOCK_FREE_CASE_16(ptr) default: break; } } while (0)













bool __atomic_is_lock_free_c(size_t size, void *ptr) {
#define LOCK_FREE_ACTION(type) return true;
LOCK_FREE_CASES(ptr);
#undef LOCK_FREE_ACTION
return false;
}



void __atomic_load_c(int size, void *src, void *dest, int model) {
#define LOCK_FREE_ACTION(type) *((type *)dest) = __c11_atomic_load((_Atomic(type) *)src, model); return;


LOCK_FREE_CASES(src);
#undef LOCK_FREE_ACTION
Lock *l = lock_for_pointer(src);
lock(l);
memcpy(dest, src, size);
unlock(l);
}



void __atomic_store_c(int size, void *dest, void *src, int model) {
#define LOCK_FREE_ACTION(type) __c11_atomic_store((_Atomic(type) *)dest, *(type *)src, model); return;


LOCK_FREE_CASES(dest);
#undef LOCK_FREE_ACTION
Lock *l = lock_for_pointer(dest);
lock(l);
memcpy(dest, src, size);
unlock(l);
}






int __atomic_compare_exchange_c(int size, void *ptr, void *expected,
void *desired, int success, int failure) {
#define LOCK_FREE_ACTION(type) return __c11_atomic_compare_exchange_strong( (_Atomic(type) *)ptr, (type *)expected, *(type *)desired, success, failure)



LOCK_FREE_CASES(ptr);
#undef LOCK_FREE_ACTION
Lock *l = lock_for_pointer(ptr);
lock(l);
if (memcmp(ptr, expected, size) == 0) {
memcpy(ptr, desired, size);
unlock(l);
return 1;
}
memcpy(expected, ptr, size);
unlock(l);
return 0;
}



void __atomic_exchange_c(int size, void *ptr, void *val, void *old, int model) {
#define LOCK_FREE_ACTION(type) *(type *)old = __c11_atomic_exchange((_Atomic(type) *)ptr, *(type *)val, model); return;



LOCK_FREE_CASES(ptr);
#undef LOCK_FREE_ACTION
Lock *l = lock_for_pointer(ptr);
lock(l);
memcpy(old, ptr, size);
memcpy(ptr, val, size);
unlock(l);
}





#if defined(__SIZEOF_INT128__)
#define OPTIMISED_CASES OPTIMISED_CASE(1, IS_LOCK_FREE_1, uint8_t) OPTIMISED_CASE(2, IS_LOCK_FREE_2, uint16_t) OPTIMISED_CASE(4, IS_LOCK_FREE_4, uint32_t) OPTIMISED_CASE(8, IS_LOCK_FREE_8, uint64_t) OPTIMISED_CASE(16, IS_LOCK_FREE_16, __uint128_t)





#else
#define OPTIMISED_CASES OPTIMISED_CASE(1, IS_LOCK_FREE_1, uint8_t) OPTIMISED_CASE(2, IS_LOCK_FREE_2, uint16_t) OPTIMISED_CASE(4, IS_LOCK_FREE_4, uint32_t) OPTIMISED_CASE(8, IS_LOCK_FREE_8, uint64_t)




#endif

#define OPTIMISED_CASE(n, lockfree, type) type __atomic_load_##n(type *src, int model) { if (lockfree(src)) return __c11_atomic_load((_Atomic(type) *)src, model); Lock *l = lock_for_pointer(src); lock(l); type val = *src; unlock(l); return val; }









OPTIMISED_CASES
#undef OPTIMISED_CASE

#define OPTIMISED_CASE(n, lockfree, type) void __atomic_store_##n(type *dest, type val, int model) { if (lockfree(dest)) { __c11_atomic_store((_Atomic(type) *)dest, val, model); return; } Lock *l = lock_for_pointer(dest); lock(l); *dest = val; unlock(l); return; }











OPTIMISED_CASES
#undef OPTIMISED_CASE

#define OPTIMISED_CASE(n, lockfree, type) type __atomic_exchange_##n(type *dest, type val, int model) { if (lockfree(dest)) return __c11_atomic_exchange((_Atomic(type) *)dest, val, model); Lock *l = lock_for_pointer(dest); lock(l); type tmp = *dest; *dest = val; unlock(l); return tmp; }










OPTIMISED_CASES
#undef OPTIMISED_CASE

#define OPTIMISED_CASE(n, lockfree, type) bool __atomic_compare_exchange_##n(type *ptr, type *expected, type desired, int success, int failure) { if (lockfree(ptr)) return __c11_atomic_compare_exchange_strong( (_Atomic(type) *)ptr, expected, desired, success, failure); Lock *l = lock_for_pointer(ptr); lock(l); if (*ptr == *expected) { *ptr = desired; unlock(l); return true; } *expected = *ptr; unlock(l); return false; }
















OPTIMISED_CASES
#undef OPTIMISED_CASE




#define ATOMIC_RMW(n, lockfree, type, opname, op) type __atomic_fetch_##opname##_##n(type *ptr, type val, int model) { if (lockfree(ptr)) return __c11_atomic_fetch_##opname((_Atomic(type) *)ptr, val, model); Lock *l = lock_for_pointer(ptr); lock(l); type tmp = *ptr; *ptr = tmp op val; unlock(l); return tmp; }











#define OPTIMISED_CASE(n, lockfree, type) ATOMIC_RMW(n, lockfree, type, add, +)
OPTIMISED_CASES
#undef OPTIMISED_CASE
#define OPTIMISED_CASE(n, lockfree, type) ATOMIC_RMW(n, lockfree, type, sub, -)
OPTIMISED_CASES
#undef OPTIMISED_CASE
#define OPTIMISED_CASE(n, lockfree, type) ATOMIC_RMW(n, lockfree, type, and, &)
OPTIMISED_CASES
#undef OPTIMISED_CASE
#define OPTIMISED_CASE(n, lockfree, type) ATOMIC_RMW(n, lockfree, type, or, |)
OPTIMISED_CASES
#undef OPTIMISED_CASE
#define OPTIMISED_CASE(n, lockfree, type) ATOMIC_RMW(n, lockfree, type, xor, ^)
OPTIMISED_CASES
#undef OPTIMISED_CASE
