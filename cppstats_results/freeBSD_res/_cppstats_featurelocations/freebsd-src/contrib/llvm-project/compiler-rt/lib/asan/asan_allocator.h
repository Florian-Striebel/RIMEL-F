












#if !defined(ASAN_ALLOCATOR_H)
#define ASAN_ALLOCATOR_H

#include "asan_flags.h"
#include "asan_interceptors.h"
#include "asan_internal.h"
#include "sanitizer_common/sanitizer_allocator.h"
#include "sanitizer_common/sanitizer_list.h"
#include "sanitizer_common/sanitizer_platform.h"

namespace __asan {

enum AllocType {
FROM_MALLOC = 1,
FROM_NEW = 2,
FROM_NEW_BR = 3
};

class AsanChunk;

struct AllocatorOptions {
u32 quarantine_size_mb;
u32 thread_local_quarantine_size_kb;
u16 min_redzone;
u16 max_redzone;
u8 may_return_null;
u8 alloc_dealloc_mismatch;
s32 release_to_os_interval_ms;

void SetFrom(const Flags *f, const CommonFlags *cf);
void CopyTo(Flags *f, CommonFlags *cf);
};

void InitializeAllocator(const AllocatorOptions &options);
void ReInitializeAllocator(const AllocatorOptions &options);
void GetAllocatorOptions(AllocatorOptions *options);

class AsanChunkView {
public:
explicit AsanChunkView(AsanChunk *chunk) : chunk_(chunk) {}
bool IsValid() const;

bool IsAllocated() const;
bool IsQuarantined() const;
uptr Beg() const;
uptr End() const;
uptr UsedSize() const;
u32 UserRequestedAlignment() const;
uptr AllocTid() const;
uptr FreeTid() const;
bool Eq(const AsanChunkView &c) const { return chunk_ == c.chunk_; }
u32 GetAllocStackId() const;
u32 GetFreeStackId() const;
StackTrace GetAllocStack() const;
StackTrace GetFreeStack() const;
AllocType GetAllocType() const;
bool AddrIsInside(uptr addr, uptr access_size, sptr *offset) const {
if (addr >= Beg() && (addr + access_size) <= End()) {
*offset = addr - Beg();
return true;
}
return false;
}
bool AddrIsAtLeft(uptr addr, uptr access_size, sptr *offset) const {
(void)access_size;
if (addr < Beg()) {
*offset = Beg() - addr;
return true;
}
return false;
}
bool AddrIsAtRight(uptr addr, uptr access_size, sptr *offset) const {
if (addr + access_size > End()) {
*offset = addr - End();
return true;
}
return false;
}

private:
AsanChunk *const chunk_;
};

AsanChunkView FindHeapChunkByAddress(uptr address);
AsanChunkView FindHeapChunkByAllocBeg(uptr address);


class AsanChunkFifoList: public IntrusiveList<AsanChunk> {
public:
explicit AsanChunkFifoList(LinkerInitialized) { }
AsanChunkFifoList() { clear(); }
void Push(AsanChunk *n);
void PushList(AsanChunkFifoList *q);
AsanChunk *Pop();
uptr size() { return size_; }
void clear() {
IntrusiveList<AsanChunk>::clear();
size_ = 0;
}
private:
uptr size_;
};

struct AsanMapUnmapCallback {
void OnMap(uptr p, uptr size) const;
void OnUnmap(uptr p, uptr size) const;
};

#if SANITIZER_CAN_USE_ALLOCATOR64
#if SANITIZER_FUCHSIA
const uptr kAllocatorSpace = ~(uptr)0;
const uptr kAllocatorSize = 0x40000000000ULL;
typedef DefaultSizeClassMap SizeClassMap;
#elif defined(__powerpc64__)
const uptr kAllocatorSpace = ~(uptr)0;
const uptr kAllocatorSize = 0x20000000000ULL;
typedef DefaultSizeClassMap SizeClassMap;
#elif defined(__aarch64__) && SANITIZER_ANDROID

const uptr kAllocatorSpace = ~(uptr)0;
const uptr kAllocatorSize = 0x2000000000ULL;
typedef VeryCompactSizeClassMap SizeClassMap;
#elif SANITIZER_RISCV64
const uptr kAllocatorSpace = ~(uptr)0;
const uptr kAllocatorSize = 0x2000000000ULL;
typedef VeryDenseSizeClassMap SizeClassMap;
#elif defined(__aarch64__)


const uptr kAllocatorSpace = 0x10000000000ULL;
const uptr kAllocatorSize = 0x10000000000ULL;
typedef DefaultSizeClassMap SizeClassMap;
#elif defined(__sparc__)
const uptr kAllocatorSpace = ~(uptr)0;
const uptr kAllocatorSize = 0x20000000000ULL;
typedef DefaultSizeClassMap SizeClassMap;
#elif SANITIZER_WINDOWS
const uptr kAllocatorSpace = ~(uptr)0;
const uptr kAllocatorSize = 0x8000000000ULL;
typedef DefaultSizeClassMap SizeClassMap;
#else
const uptr kAllocatorSpace = 0x600000000000ULL;
const uptr kAllocatorSize = 0x40000000000ULL;
typedef DefaultSizeClassMap SizeClassMap;
#endif
template <typename AddressSpaceViewTy>
struct AP64 {
static const uptr kSpaceBeg = kAllocatorSpace;
static const uptr kSpaceSize = kAllocatorSize;
static const uptr kMetadataSize = 0;
typedef __asan::SizeClassMap SizeClassMap;
typedef AsanMapUnmapCallback MapUnmapCallback;
static const uptr kFlags = 0;
using AddressSpaceView = AddressSpaceViewTy;
};

template <typename AddressSpaceView>
using PrimaryAllocatorASVT = SizeClassAllocator64<AP64<AddressSpaceView>>;
using PrimaryAllocator = PrimaryAllocatorASVT<LocalAddressSpaceView>;
#else
typedef CompactSizeClassMap SizeClassMap;
template <typename AddressSpaceViewTy>
struct AP32 {
static const uptr kSpaceBeg = 0;
static const u64 kSpaceSize = SANITIZER_MMAP_RANGE_SIZE;
static const uptr kMetadataSize = 0;
typedef __asan::SizeClassMap SizeClassMap;
static const uptr kRegionSizeLog = 20;
using AddressSpaceView = AddressSpaceViewTy;
typedef AsanMapUnmapCallback MapUnmapCallback;
static const uptr kFlags = 0;
};
template <typename AddressSpaceView>
using PrimaryAllocatorASVT = SizeClassAllocator32<AP32<AddressSpaceView> >;
using PrimaryAllocator = PrimaryAllocatorASVT<LocalAddressSpaceView>;
#endif

static const uptr kNumberOfSizeClasses = SizeClassMap::kNumClasses;

template <typename AddressSpaceView>
using AsanAllocatorASVT =
CombinedAllocator<PrimaryAllocatorASVT<AddressSpaceView>>;
using AsanAllocator = AsanAllocatorASVT<LocalAddressSpaceView>;
using AllocatorCache = AsanAllocator::AllocatorCache;

struct AsanThreadLocalMallocStorage {
uptr quarantine_cache[16];
AllocatorCache allocator_cache;
void CommitBack();
private:

AsanThreadLocalMallocStorage() {}
};

void *asan_memalign(uptr alignment, uptr size, BufferedStackTrace *stack,
AllocType alloc_type);
void asan_free(void *ptr, BufferedStackTrace *stack, AllocType alloc_type);
void asan_delete(void *ptr, uptr size, uptr alignment,
BufferedStackTrace *stack, AllocType alloc_type);

void *asan_malloc(uptr size, BufferedStackTrace *stack);
void *asan_calloc(uptr nmemb, uptr size, BufferedStackTrace *stack);
void *asan_realloc(void *p, uptr size, BufferedStackTrace *stack);
void *asan_reallocarray(void *p, uptr nmemb, uptr size,
BufferedStackTrace *stack);
void *asan_valloc(uptr size, BufferedStackTrace *stack);
void *asan_pvalloc(uptr size, BufferedStackTrace *stack);

void *asan_aligned_alloc(uptr alignment, uptr size, BufferedStackTrace *stack);
int asan_posix_memalign(void **memptr, uptr alignment, uptr size,
BufferedStackTrace *stack);
uptr asan_malloc_usable_size(const void *ptr, uptr pc, uptr bp);

uptr asan_mz_size(const void *ptr);
void asan_mz_force_lock();
void asan_mz_force_unlock();

void PrintInternalAllocatorStats();
void AsanSoftRssLimitExceededCallback(bool exceeded);

}
#endif
