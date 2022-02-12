#define ATOMIC_INLINE JEMALLOC_ALWAYS_INLINE
#define JEMALLOC_U8_ATOMICS
#if defined(JEMALLOC_GCC_ATOMIC_ATOMICS)
#include "jemalloc/internal/atomic_gcc_atomic.h"
#if !defined(JEMALLOC_GCC_U8_ATOMIC_ATOMICS)
#undef JEMALLOC_U8_ATOMICS
#endif
#elif defined(JEMALLOC_GCC_SYNC_ATOMICS)
#include "jemalloc/internal/atomic_gcc_sync.h"
#if !defined(JEMALLOC_GCC_U8_SYNC_ATOMICS)
#undef JEMALLOC_U8_ATOMICS
#endif
#elif defined(_MSC_VER)
#include "jemalloc/internal/atomic_msvc.h"
#elif defined(JEMALLOC_C11_ATOMICS)
#include "jemalloc/internal/atomic_c11.h"
#else
#error "Don't have atomics implemented on this platform."
#endif
#define ATOMIC_RELAXED atomic_memory_order_relaxed
#define ATOMIC_ACQUIRE atomic_memory_order_acquire
#define ATOMIC_RELEASE atomic_memory_order_release
#define ATOMIC_ACQ_REL atomic_memory_order_acq_rel
#define ATOMIC_SEQ_CST atomic_memory_order_seq_cst
#if (LG_SIZEOF_PTR == 3 || LG_SIZEOF_INT == 3)
#define JEMALLOC_ATOMIC_U64
#endif
JEMALLOC_GENERATE_ATOMICS(void *, p, LG_SIZEOF_PTR)
JEMALLOC_GENERATE_ATOMICS(bool, b, 0)
JEMALLOC_GENERATE_INT_ATOMICS(unsigned, u, LG_SIZEOF_INT)
JEMALLOC_GENERATE_INT_ATOMICS(size_t, zu, LG_SIZEOF_PTR)
JEMALLOC_GENERATE_INT_ATOMICS(ssize_t, zd, LG_SIZEOF_PTR)
JEMALLOC_GENERATE_INT_ATOMICS(uint8_t, u8, 0)
JEMALLOC_GENERATE_INT_ATOMICS(uint32_t, u32, 2)
#if defined(JEMALLOC_ATOMIC_U64)
JEMALLOC_GENERATE_INT_ATOMICS(uint64_t, u64, 3)
#endif
#undef ATOMIC_INLINE
