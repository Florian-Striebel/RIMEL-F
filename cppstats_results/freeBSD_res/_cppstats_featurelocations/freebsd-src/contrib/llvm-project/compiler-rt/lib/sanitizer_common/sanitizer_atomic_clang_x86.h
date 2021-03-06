












#if !defined(SANITIZER_ATOMIC_CLANG_X86_H)
#define SANITIZER_ATOMIC_CLANG_X86_H

namespace __sanitizer {

inline void proc_yield(int cnt) {
__asm__ __volatile__("" ::: "memory");
for (int i = 0; i < cnt; i++)
__asm__ __volatile__("pause");
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

__asm__ __volatile__("" ::: "memory");
} else {

__asm__ __volatile__("" ::: "memory");
v = a->val_dont_use;
__asm__ __volatile__("" ::: "memory");
}
} else {

__asm__ __volatile__(
"movq %1, %%mm0;"
"movq %%mm0, %0;"
"emms;"
: "=m" (v)
: "m" (a->val_dont_use)
:
#if defined(__MMX__)
"mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7",
#endif
"memory");
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

__asm__ __volatile__("" ::: "memory");
a->val_dont_use = v;
__asm__ __volatile__("" ::: "memory");
} else {

__asm__ __volatile__("" ::: "memory");
a->val_dont_use = v;
__sync_synchronize();
}
} else {

__asm__ __volatile__(
"movq %1, %%mm0;"
"movq %%mm0, %0;"
"emms;"
: "=m" (a->val_dont_use)
: "m" (v)
:
#if defined(__MMX__)
"mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7",
#endif
"memory");
if (mo == memory_order_seq_cst)
__sync_synchronize();
}
}

}

#endif
