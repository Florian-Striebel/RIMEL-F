












#if !defined(SANITIZER_ATOMIC_CLANG_OTHER_H)
#define SANITIZER_ATOMIC_CLANG_OTHER_H

namespace __sanitizer {


inline void proc_yield(int cnt) {
__asm__ __volatile__("" ::: "memory");
}

template<typename T>
inline typename T::Type atomic_load(
const volatile T *a, memory_order mo) {
DCHECK(mo & (memory_order_relaxed | memory_order_consume
| memory_order_acquire | memory_order_seq_cst));
DCHECK(!((uptr)a % sizeof(*a)));
typename T::Type v;

if (sizeof(*a) < 8 || sizeof(void*) == 8) {

if (mo == memory_order_relaxed) {
v = a->val_dont_use;
} else if (mo == memory_order_consume) {


__asm__ __volatile__("" ::: "memory");
v = a->val_dont_use;
__asm__ __volatile__("" ::: "memory");
} else if (mo == memory_order_acquire) {
__asm__ __volatile__("" ::: "memory");
v = a->val_dont_use;
__sync_synchronize();
} else {

__sync_synchronize();
v = a->val_dont_use;
__sync_synchronize();
}
} else {
__atomic_load(const_cast<typename T::Type volatile *>(&a->val_dont_use), &v,
__ATOMIC_SEQ_CST);
}
return v;
}

template<typename T>
inline void atomic_store(volatile T *a, typename T::Type v, memory_order mo) {
DCHECK(mo & (memory_order_relaxed | memory_order_release
| memory_order_seq_cst));
DCHECK(!((uptr)a % sizeof(*a)));

if (sizeof(*a) < 8 || sizeof(void*) == 8) {

if (mo == memory_order_relaxed) {
a->val_dont_use = v;
} else if (mo == memory_order_release) {
__sync_synchronize();
a->val_dont_use = v;
__asm__ __volatile__("" ::: "memory");
} else {
__sync_synchronize();
a->val_dont_use = v;
__sync_synchronize();
}
} else {
__atomic_store(&a->val_dont_use, &v, __ATOMIC_SEQ_CST);
}
}

}

#endif
