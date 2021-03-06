











#if !defined(SCUDO_ALLOCATOR_H_)
#define SCUDO_ALLOCATOR_H_

#include "scudo_platform.h"

namespace __scudo {

enum AllocType : u8 {
FromMalloc = 0,
FromNew = 1,
FromNewArray = 2,
FromMemalign = 3,
};

enum ChunkState : u8 {
ChunkAvailable = 0,
ChunkAllocated = 1,
ChunkQuarantine = 2
};






typedef u64 PackedHeader;
struct UnpackedHeader {
u64 Checksum : 16;
u64 ClassId : 8;
u64 SizeOrUnusedBytes : 20;

u64 State : 2;
u64 AllocType : 2;
u64 Offset : 16;



};

typedef atomic_uint64_t AtomicPackedHeader;
COMPILER_CHECK(sizeof(UnpackedHeader) == sizeof(PackedHeader));


const uptr MinAlignmentLog = FIRST_32_SECOND_64(3, 4);
const uptr MaxAlignmentLog = 24;
const uptr MinAlignment = 1 << MinAlignmentLog;
const uptr MaxAlignment = 1 << MaxAlignmentLog;



constexpr uptr RoundUpTo(uptr Size, uptr Boundary) {
return (Size + Boundary - 1) & ~(Boundary - 1);
}

namespace Chunk {
constexpr uptr getHeaderSize() {
return RoundUpTo(sizeof(PackedHeader), MinAlignment);
}
}

#if SANITIZER_CAN_USE_ALLOCATOR64
const uptr AllocatorSpace = ~0ULL;
struct AP64 {
static const uptr kSpaceBeg = AllocatorSpace;
static const uptr kSpaceSize = AllocatorSize;
static const uptr kMetadataSize = 0;
typedef __scudo::SizeClassMap SizeClassMap;
typedef NoOpMapUnmapCallback MapUnmapCallback;
static const uptr kFlags =
SizeClassAllocator64FlagMasks::kRandomShuffleChunks;
using AddressSpaceView = LocalAddressSpaceView;
};
typedef SizeClassAllocator64<AP64> PrimaryT;
#else
struct AP32 {
static const uptr kSpaceBeg = 0;
static const u64 kSpaceSize = SANITIZER_MMAP_RANGE_SIZE;
static const uptr kMetadataSize = 0;
typedef __scudo::SizeClassMap SizeClassMap;
static const uptr kRegionSizeLog = RegionSizeLog;
using AddressSpaceView = LocalAddressSpaceView;
typedef NoOpMapUnmapCallback MapUnmapCallback;
static const uptr kFlags =
SizeClassAllocator32FlagMasks::kRandomShuffleChunks |
SizeClassAllocator32FlagMasks::kUseSeparateSizeClassForBatch;
};
typedef SizeClassAllocator32<AP32> PrimaryT;
#endif

#include "scudo_allocator_secondary.h"

typedef LargeMmapAllocator SecondaryT;

#include "scudo_allocator_combined.h"

typedef CombinedAllocator BackendT;
typedef CombinedAllocator::AllocatorCache AllocatorCacheT;

void initScudo();

void *scudoAllocate(uptr Size, uptr Alignment, AllocType Type);
void scudoDeallocate(void *Ptr, uptr Size, uptr Alignment, AllocType Type);
void *scudoRealloc(void *Ptr, uptr Size);
void *scudoCalloc(uptr NMemB, uptr Size);
void *scudoValloc(uptr Size);
void *scudoPvalloc(uptr Size);
int scudoPosixMemalign(void **MemPtr, uptr Alignment, uptr Size);
void *scudoAlignedAlloc(uptr Alignment, uptr Size);
uptr scudoMallocUsableSize(void *Ptr);

}

#endif
