











#if !defined(SANITIZER_ALLOCATOR_INTERNAL_H)
#define SANITIZER_ALLOCATOR_INTERNAL_H

#include "sanitizer_allocator.h"
#include "sanitizer_internal_defs.h"

namespace __sanitizer {



typedef CompactSizeClassMap InternalSizeClassMap;

struct AP32 {
static const uptr kSpaceBeg = 0;
static const u64 kSpaceSize = SANITIZER_MMAP_RANGE_SIZE;
static const uptr kMetadataSize = 0;
typedef InternalSizeClassMap SizeClassMap;
static const uptr kRegionSizeLog = 20;
using AddressSpaceView = LocalAddressSpaceView;
typedef NoOpMapUnmapCallback MapUnmapCallback;
static const uptr kFlags = 0;
};
typedef SizeClassAllocator32<AP32> PrimaryInternalAllocator;

typedef CombinedAllocator<PrimaryInternalAllocator,
LargeMmapAllocatorPtrArrayStatic>
InternalAllocator;
typedef InternalAllocator::AllocatorCache InternalAllocatorCache;

void *InternalAlloc(uptr size, InternalAllocatorCache *cache = nullptr,
uptr alignment = 0);
void *InternalRealloc(void *p, uptr size,
InternalAllocatorCache *cache = nullptr);
void *InternalReallocArray(void *p, uptr count, uptr size,
InternalAllocatorCache *cache = nullptr);
void *InternalCalloc(uptr count, uptr size,
InternalAllocatorCache *cache = nullptr);
void InternalFree(void *p, InternalAllocatorCache *cache = nullptr);
InternalAllocator *internal_allocator();

}

#endif
