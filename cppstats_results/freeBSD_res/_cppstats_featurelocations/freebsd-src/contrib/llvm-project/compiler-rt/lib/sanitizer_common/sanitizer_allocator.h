











#if !defined(SANITIZER_ALLOCATOR_H)
#define SANITIZER_ALLOCATOR_H

#include "sanitizer_common.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_lfstack.h"
#include "sanitizer_libc.h"
#include "sanitizer_list.h"
#include "sanitizer_local_address_space_view.h"
#include "sanitizer_mutex.h"
#include "sanitizer_procmaps.h"
#include "sanitizer_type_traits.h"

namespace __sanitizer {


extern const char *PrimaryAllocatorName;
extern const char *SecondaryAllocatorName;




bool AllocatorMayReturnNull();
void SetAllocatorMayReturnNull(bool may_return_null);



bool IsAllocatorOutOfMemory();

void SetAllocatorOutOfMemory();

void PrintHintAllocatorCannotReturnNull();


struct NoOpMapUnmapCallback {
void OnMap(uptr p, uptr size) const { }
void OnUnmap(uptr p, uptr size) const { }
};


typedef void (*ForEachChunkCallback)(uptr chunk, void *arg);

inline u32 Rand(u32 *state) {
return (*state = *state * 1103515245 + 12345) >> 16;
}

inline u32 RandN(u32 *state, u32 n) { return Rand(state) % n; }

template<typename T>
inline void RandomShuffle(T *a, u32 n, u32 *rand_state) {
if (n <= 1) return;
u32 state = *rand_state;
for (u32 i = n - 1; i > 0; i--)
Swap(a[i], a[RandN(&state, i + 1)]);
*rand_state = state;
}

#include "sanitizer_allocator_size_class_map.h"
#include "sanitizer_allocator_stats.h"
#include "sanitizer_allocator_primary64.h"
#include "sanitizer_allocator_bytemap.h"
#include "sanitizer_allocator_primary32.h"
#include "sanitizer_allocator_local_cache.h"
#include "sanitizer_allocator_secondary.h"
#include "sanitizer_allocator_combined.h"

}

#endif
