











#if !defined(SANITIZER_ATOMIC_H)
#define SANITIZER_ATOMIC_H

#include "sanitizer_internal_defs.h"

namespace __sanitizer {

enum memory_order {
memory_order_relaxed = 1 << 0,
memory_order_consume = 1 << 1,
memory_order_acquire = 1 << 2,
memory_order_release = 1 << 3,
memory_order_acq_rel = 1 << 4,
memory_order_seq_cst = 1 << 5
};

struct atomic_uint8_t {
typedef u8 Type;
volatile Type val_dont_use;
};

struct atomic_uint16_t {
typedef u16 Type;
volatile Type val_dont_use;
};

struct atomic_sint32_t {
typedef s32 Type;
volatile Type val_dont_use;
};

struct atomic_uint32_t {
typedef u32 Type;
volatile Type val_dont_use;
};

struct atomic_uint64_t {
typedef u64 Type;

volatile ALIGNED(8) Type val_dont_use;
};

struct atomic_uintptr_t {
typedef uptr Type;
volatile Type val_dont_use;
};

}

#if defined(__clang__) || defined(__GNUC__)
#include "sanitizer_atomic_clang.h"
#elif defined(_MSC_VER)
#include "sanitizer_atomic_msvc.h"
#else
#error "Unsupported compiler"
#endif

namespace __sanitizer {



template<typename T>
inline typename T::Type atomic_load_relaxed(const volatile T *a) {
return atomic_load(a, memory_order_relaxed);
}

template<typename T>
inline void atomic_store_relaxed(volatile T *a, typename T::Type v) {
atomic_store(a, v, memory_order_relaxed);
}

}

#endif
