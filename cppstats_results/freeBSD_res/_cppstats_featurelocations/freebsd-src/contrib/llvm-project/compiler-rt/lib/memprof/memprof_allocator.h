












#if !defined(MEMPROF_ALLOCATOR_H)
#define MEMPROF_ALLOCATOR_H

#include "memprof_flags.h"
#include "memprof_interceptors.h"
#include "memprof_internal.h"
#include "sanitizer_common/sanitizer_allocator.h"
#include "sanitizer_common/sanitizer_list.h"

#if !defined(__x86_64__)
#error Unsupported platform
#endif
#if !SANITIZER_CAN_USE_ALLOCATOR64
#error Only 64-bit allocator supported
#endif

namespace __memprof {

enum AllocType {
FROM_MALLOC = 1,
FROM_NEW = 2,
FROM_NEW_BR = 3
};

void InitializeAllocator();

struct MemprofMapUnmapCallback {
void OnMap(uptr p, uptr size) const;
void OnUnmap(uptr p, uptr size) const;
};

constexpr uptr kAllocatorSpace = 0x600000000000ULL;
constexpr uptr kAllocatorSize = 0x40000000000ULL;
typedef DefaultSizeClassMap SizeClassMap;
template <typename AddressSpaceViewTy>
struct AP64 {
static const uptr kSpaceBeg = kAllocatorSpace;
static const uptr kSpaceSize = kAllocatorSize;
static const uptr kMetadataSize = 0;
typedef __memprof::SizeClassMap SizeClassMap;
typedef MemprofMapUnmapCallback MapUnmapCallback;
static const uptr kFlags = 0;
using AddressSpaceView = AddressSpaceViewTy;
};

template <typename AddressSpaceView>
using PrimaryAllocatorASVT = SizeClassAllocator64<AP64<AddressSpaceView>>;
using PrimaryAllocator = PrimaryAllocatorASVT<LocalAddressSpaceView>;

static const uptr kNumberOfSizeClasses = SizeClassMap::kNumClasses;

template <typename AddressSpaceView>
using MemprofAllocatorASVT =
CombinedAllocator<PrimaryAllocatorASVT<AddressSpaceView>>;
using MemprofAllocator = MemprofAllocatorASVT<LocalAddressSpaceView>;
using AllocatorCache = MemprofAllocator::AllocatorCache;

struct MemprofThreadLocalMallocStorage {
AllocatorCache allocator_cache;
void CommitBack();

private:

MemprofThreadLocalMallocStorage() {}
};

void *memprof_memalign(uptr alignment, uptr size, BufferedStackTrace *stack,
AllocType alloc_type);
void memprof_free(void *ptr, BufferedStackTrace *stack, AllocType alloc_type);
void memprof_delete(void *ptr, uptr size, uptr alignment,
BufferedStackTrace *stack, AllocType alloc_type);

void *memprof_malloc(uptr size, BufferedStackTrace *stack);
void *memprof_calloc(uptr nmemb, uptr size, BufferedStackTrace *stack);
void *memprof_realloc(void *p, uptr size, BufferedStackTrace *stack);
void *memprof_reallocarray(void *p, uptr nmemb, uptr size,
BufferedStackTrace *stack);
void *memprof_valloc(uptr size, BufferedStackTrace *stack);
void *memprof_pvalloc(uptr size, BufferedStackTrace *stack);

void *memprof_aligned_alloc(uptr alignment, uptr size,
BufferedStackTrace *stack);
int memprof_posix_memalign(void **memptr, uptr alignment, uptr size,
BufferedStackTrace *stack);
uptr memprof_malloc_usable_size(const void *ptr, uptr pc, uptr bp);

void PrintInternalAllocatorStats();
void MemprofSoftRssLimitExceededCallback(bool exceeded);

}
#endif
