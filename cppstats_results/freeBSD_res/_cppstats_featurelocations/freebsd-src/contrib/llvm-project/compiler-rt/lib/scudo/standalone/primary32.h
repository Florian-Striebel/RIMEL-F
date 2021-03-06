







#if !defined(SCUDO_PRIMARY32_H_)
#define SCUDO_PRIMARY32_H_

#include "bytemap.h"
#include "common.h"
#include "list.h"
#include "local_cache.h"
#include "options.h"
#include "release.h"
#include "report.h"
#include "stats.h"
#include "string_utils.h"

namespace scudo {



















template <typename Config> class SizeClassAllocator32 {
public:
typedef typename Config::PrimaryCompactPtrT CompactPtrT;
typedef typename Config::SizeClassMap SizeClassMap;

static_assert(SizeClassMap::LargestClassId <= (UINT8_MAX - 1), "");

static_assert((1UL << Config::PrimaryRegionSizeLog) >= SizeClassMap::MaxSize,
"");
typedef SizeClassAllocator32<Config> ThisT;
typedef SizeClassAllocatorLocalCache<ThisT> CacheT;
typedef typename CacheT::TransferBatch TransferBatch;

static uptr getSizeByClassId(uptr ClassId) {
return (ClassId == SizeClassMap::BatchClassId)
? sizeof(TransferBatch)
: SizeClassMap::getSizeByClassId(ClassId);
}

static bool canAllocate(uptr Size) { return Size <= SizeClassMap::MaxSize; }

void init(s32 ReleaseToOsInterval) {
if (SCUDO_FUCHSIA)
reportError("SizeClassAllocator32 is not supported on Fuchsia");

if (SCUDO_TRUSTY)
reportError("SizeClassAllocator32 is not supported on Trusty");

DCHECK(isAligned(reinterpret_cast<uptr>(this), alignof(ThisT)));
PossibleRegions.init();
u32 Seed;
const u64 Time = getMonotonicTime();
if (!getRandom(reinterpret_cast<void *>(&Seed), sizeof(Seed)))
Seed = static_cast<u32>(
Time ^ (reinterpret_cast<uptr>(SizeClassInfoArray) >> 6));
for (uptr I = 0; I < NumClasses; I++) {
SizeClassInfo *Sci = getSizeClassInfo(I);
Sci->RandState = getRandomU32(&Seed);

Sci->MinRegionIndex = NumRegions;
Sci->ReleaseInfo.LastReleaseAtNs = Time;
}
setOption(Option::ReleaseInterval, static_cast<sptr>(ReleaseToOsInterval));
}

void unmapTestOnly() {
while (NumberOfStashedRegions > 0)
unmap(reinterpret_cast<void *>(RegionsStash[--NumberOfStashedRegions]),
RegionSize);
uptr MinRegionIndex = NumRegions, MaxRegionIndex = 0;
for (uptr I = 0; I < NumClasses; I++) {
SizeClassInfo *Sci = getSizeClassInfo(I);
if (Sci->MinRegionIndex < MinRegionIndex)
MinRegionIndex = Sci->MinRegionIndex;
if (Sci->MaxRegionIndex > MaxRegionIndex)
MaxRegionIndex = Sci->MaxRegionIndex;
*Sci = {};
}
for (uptr I = MinRegionIndex; I < MaxRegionIndex; I++)
if (PossibleRegions[I])
unmap(reinterpret_cast<void *>(I * RegionSize), RegionSize);
PossibleRegions.unmapTestOnly();
}

CompactPtrT compactPtr(UNUSED uptr ClassId, uptr Ptr) const {
return static_cast<CompactPtrT>(Ptr);
}

void *decompactPtr(UNUSED uptr ClassId, CompactPtrT CompactPtr) const {
return reinterpret_cast<void *>(static_cast<uptr>(CompactPtr));
}

TransferBatch *popBatch(CacheT *C, uptr ClassId) {
DCHECK_LT(ClassId, NumClasses);
SizeClassInfo *Sci = getSizeClassInfo(ClassId);
ScopedLock L(Sci->Mutex);
TransferBatch *B = Sci->FreeList.front();
if (B) {
Sci->FreeList.pop_front();
} else {
B = populateFreeList(C, ClassId, Sci);
if (UNLIKELY(!B))
return nullptr;
}
DCHECK_GT(B->getCount(), 0);
Sci->Stats.PoppedBlocks += B->getCount();
return B;
}

void pushBatch(uptr ClassId, TransferBatch *B) {
DCHECK_LT(ClassId, NumClasses);
DCHECK_GT(B->getCount(), 0);
SizeClassInfo *Sci = getSizeClassInfo(ClassId);
ScopedLock L(Sci->Mutex);
Sci->FreeList.push_front(B);
Sci->Stats.PushedBlocks += B->getCount();
if (ClassId != SizeClassMap::BatchClassId)
releaseToOSMaybe(Sci, ClassId);
}

void disable() {

for (sptr I = static_cast<sptr>(NumClasses) - 1; I >= 0; I--) {
if (static_cast<uptr>(I) == SizeClassMap::BatchClassId)
continue;
getSizeClassInfo(static_cast<uptr>(I))->Mutex.lock();
}
getSizeClassInfo(SizeClassMap::BatchClassId)->Mutex.lock();
RegionsStashMutex.lock();
PossibleRegions.disable();
}

void enable() {
PossibleRegions.enable();
RegionsStashMutex.unlock();
getSizeClassInfo(SizeClassMap::BatchClassId)->Mutex.unlock();
for (uptr I = 0; I < NumClasses; I++) {
if (I == SizeClassMap::BatchClassId)
continue;
getSizeClassInfo(I)->Mutex.unlock();
}
}

template <typename F> void iterateOverBlocks(F Callback) {
uptr MinRegionIndex = NumRegions, MaxRegionIndex = 0;
for (uptr I = 0; I < NumClasses; I++) {
SizeClassInfo *Sci = getSizeClassInfo(I);
if (Sci->MinRegionIndex < MinRegionIndex)
MinRegionIndex = Sci->MinRegionIndex;
if (Sci->MaxRegionIndex > MaxRegionIndex)
MaxRegionIndex = Sci->MaxRegionIndex;
}
for (uptr I = MinRegionIndex; I <= MaxRegionIndex; I++)
if (PossibleRegions[I] &&
(PossibleRegions[I] - 1U) != SizeClassMap::BatchClassId) {
const uptr BlockSize = getSizeByClassId(PossibleRegions[I] - 1U);
const uptr From = I * RegionSize;
const uptr To = From + (RegionSize / BlockSize) * BlockSize;
for (uptr Block = From; Block < To; Block += BlockSize)
Callback(Block);
}
}

void getStats(ScopedString *Str) {

uptr TotalMapped = 0;
uptr PoppedBlocks = 0;
uptr PushedBlocks = 0;
for (uptr I = 0; I < NumClasses; I++) {
SizeClassInfo *Sci = getSizeClassInfo(I);
TotalMapped += Sci->AllocatedUser;
PoppedBlocks += Sci->Stats.PoppedBlocks;
PushedBlocks += Sci->Stats.PushedBlocks;
}
Str->append("Stats: SizeClassAllocator32: %zuM mapped in %zu allocations; "
"remains %zu\n",
TotalMapped >> 20, PoppedBlocks, PoppedBlocks - PushedBlocks);
for (uptr I = 0; I < NumClasses; I++)
getStats(Str, I, 0);
}

bool setOption(Option O, sptr Value) {
if (O == Option::ReleaseInterval) {
const s32 Interval = Max(
Min(static_cast<s32>(Value), Config::PrimaryMaxReleaseToOsIntervalMs),
Config::PrimaryMinReleaseToOsIntervalMs);
atomic_store_relaxed(&ReleaseToOsIntervalMs, Interval);
return true;
}

return true;
}

uptr releaseToOS() {
uptr TotalReleasedBytes = 0;
for (uptr I = 0; I < NumClasses; I++) {
if (I == SizeClassMap::BatchClassId)
continue;
SizeClassInfo *Sci = getSizeClassInfo(I);
ScopedLock L(Sci->Mutex);
TotalReleasedBytes += releaseToOSMaybe(Sci, I, true);
}
return TotalReleasedBytes;
}

const char *getRegionInfoArrayAddress() const { return nullptr; }
static uptr getRegionInfoArraySize() { return 0; }

static BlockInfo findNearestBlock(UNUSED const char *RegionInfoData,
UNUSED uptr Ptr) {
return {};
}

AtomicOptions Options;

private:
static const uptr NumClasses = SizeClassMap::NumClasses;
static const uptr RegionSize = 1UL << Config::PrimaryRegionSizeLog;
static const uptr NumRegions =
SCUDO_MMAP_RANGE_SIZE >> Config::PrimaryRegionSizeLog;
static const u32 MaxNumBatches = SCUDO_ANDROID ? 4U : 8U;
typedef FlatByteMap<NumRegions> ByteMap;

struct SizeClassStats {
uptr PoppedBlocks;
uptr PushedBlocks;
};

struct ReleaseToOsInfo {
uptr PushedBlocksAtLastRelease;
uptr RangesReleased;
uptr LastReleasedBytes;
u64 LastReleaseAtNs;
};

struct alignas(SCUDO_CACHE_LINE_SIZE) SizeClassInfo {
HybridMutex Mutex;
SinglyLinkedList<TransferBatch> FreeList;
uptr CurrentRegion;
uptr CurrentRegionAllocated;
SizeClassStats Stats;
u32 RandState;
uptr AllocatedUser;


uptr MinRegionIndex;
uptr MaxRegionIndex;
ReleaseToOsInfo ReleaseInfo;
};
static_assert(sizeof(SizeClassInfo) % SCUDO_CACHE_LINE_SIZE == 0, "");

uptr computeRegionId(uptr Mem) {
const uptr Id = Mem >> Config::PrimaryRegionSizeLog;
CHECK_LT(Id, NumRegions);
return Id;
}

uptr allocateRegionSlow() {
uptr MapSize = 2 * RegionSize;
const uptr MapBase = reinterpret_cast<uptr>(
map(nullptr, MapSize, "scudo:primary", MAP_ALLOWNOMEM));
if (!MapBase)
return 0;
const uptr MapEnd = MapBase + MapSize;
uptr Region = MapBase;
if (isAligned(Region, RegionSize)) {
ScopedLock L(RegionsStashMutex);
if (NumberOfStashedRegions < MaxStashedRegions)
RegionsStash[NumberOfStashedRegions++] = MapBase + RegionSize;
else
MapSize = RegionSize;
} else {
Region = roundUpTo(MapBase, RegionSize);
unmap(reinterpret_cast<void *>(MapBase), Region - MapBase);
MapSize = RegionSize;
}
const uptr End = Region + MapSize;
if (End != MapEnd)
unmap(reinterpret_cast<void *>(End), MapEnd - End);
return Region;
}

uptr allocateRegion(SizeClassInfo *Sci, uptr ClassId) {
DCHECK_LT(ClassId, NumClasses);
uptr Region = 0;
{
ScopedLock L(RegionsStashMutex);
if (NumberOfStashedRegions > 0)
Region = RegionsStash[--NumberOfStashedRegions];
}
if (!Region)
Region = allocateRegionSlow();
if (LIKELY(Region)) {

const uptr RegionIndex = computeRegionId(Region);
if (RegionIndex < Sci->MinRegionIndex)
Sci->MinRegionIndex = RegionIndex;
if (RegionIndex > Sci->MaxRegionIndex)
Sci->MaxRegionIndex = RegionIndex;
PossibleRegions.set(RegionIndex, static_cast<u8>(ClassId + 1U));
}
return Region;
}

SizeClassInfo *getSizeClassInfo(uptr ClassId) {
DCHECK_LT(ClassId, NumClasses);
return &SizeClassInfoArray[ClassId];
}

NOINLINE TransferBatch *populateFreeList(CacheT *C, uptr ClassId,
SizeClassInfo *Sci) {
uptr Region;
uptr Offset;




if (Sci->CurrentRegion) {
Region = Sci->CurrentRegion;
DCHECK_GT(Sci->CurrentRegionAllocated, 0U);
Offset = Sci->CurrentRegionAllocated;
} else {
DCHECK_EQ(Sci->CurrentRegionAllocated, 0U);
Region = allocateRegion(Sci, ClassId);
if (UNLIKELY(!Region))
return nullptr;
C->getStats().add(StatMapped, RegionSize);
Sci->CurrentRegion = Region;
Offset = 0;
}

const uptr Size = getSizeByClassId(ClassId);
const u32 MaxCount = TransferBatch::getMaxCached(Size);
DCHECK_GT(MaxCount, 0U);





const u32 NumberOfBlocks =
Min(MaxNumBatches * MaxCount,
static_cast<u32>((RegionSize - Offset) / Size));
DCHECK_GT(NumberOfBlocks, 0U);

constexpr u32 ShuffleArraySize =
MaxNumBatches * TransferBatch::MaxNumCached;



CompactPtrT ShuffleArray[ShuffleArraySize];
DCHECK_LE(NumberOfBlocks, ShuffleArraySize);

uptr P = Region + Offset;
for (u32 I = 0; I < NumberOfBlocks; I++, P += Size)
ShuffleArray[I] = reinterpret_cast<CompactPtrT>(P);

if (ClassId != SizeClassMap::BatchClassId)
shuffle(ShuffleArray, NumberOfBlocks, &Sci->RandState);
for (u32 I = 0; I < NumberOfBlocks;) {
TransferBatch *B =
C->createBatch(ClassId, reinterpret_cast<void *>(ShuffleArray[I]));
if (UNLIKELY(!B))
return nullptr;
const u32 N = Min(MaxCount, NumberOfBlocks - I);
B->setFromArray(&ShuffleArray[I], N);
Sci->FreeList.push_back(B);
I += N;
}
TransferBatch *B = Sci->FreeList.front();
Sci->FreeList.pop_front();
DCHECK(B);
DCHECK_GT(B->getCount(), 0);

const uptr AllocatedUser = Size * NumberOfBlocks;
C->getStats().add(StatFree, AllocatedUser);
DCHECK_LE(Sci->CurrentRegionAllocated + AllocatedUser, RegionSize);



if (RegionSize - (Sci->CurrentRegionAllocated + AllocatedUser) < Size) {
Sci->CurrentRegion = 0;
Sci->CurrentRegionAllocated = 0;
} else {
Sci->CurrentRegionAllocated += AllocatedUser;
}
Sci->AllocatedUser += AllocatedUser;

return B;
}

void getStats(ScopedString *Str, uptr ClassId, uptr Rss) {
SizeClassInfo *Sci = getSizeClassInfo(ClassId);
if (Sci->AllocatedUser == 0)
return;
const uptr InUse = Sci->Stats.PoppedBlocks - Sci->Stats.PushedBlocks;
const uptr AvailableChunks = Sci->AllocatedUser / getSizeByClassId(ClassId);
Str->append(" %02zu (%6zu): mapped: %6zuK popped: %7zu pushed: %7zu "
"inuse: %6zu avail: %6zu rss: %6zuK releases: %6zu\n",
ClassId, getSizeByClassId(ClassId), Sci->AllocatedUser >> 10,
Sci->Stats.PoppedBlocks, Sci->Stats.PushedBlocks, InUse,
AvailableChunks, Rss >> 10, Sci->ReleaseInfo.RangesReleased);
}

NOINLINE uptr releaseToOSMaybe(SizeClassInfo *Sci, uptr ClassId,
bool Force = false) {
const uptr BlockSize = getSizeByClassId(ClassId);
const uptr PageSize = getPageSizeCached();

DCHECK_GE(Sci->Stats.PoppedBlocks, Sci->Stats.PushedBlocks);
const uptr BytesInFreeList =
Sci->AllocatedUser -
(Sci->Stats.PoppedBlocks - Sci->Stats.PushedBlocks) * BlockSize;
if (BytesInFreeList < PageSize)
return 0;
const uptr BytesPushed =
(Sci->Stats.PushedBlocks - Sci->ReleaseInfo.PushedBlocksAtLastRelease) *
BlockSize;
if (BytesPushed < PageSize)
return 0;




if (BlockSize < PageSize / 16U) {
if (!Force && BytesPushed < Sci->AllocatedUser / 16U)
return 0;

if ((BytesInFreeList * 100U) / Sci->AllocatedUser <
(100U - 1U - BlockSize / 16U))
return 0;
}

if (!Force) {
const s32 IntervalMs = atomic_load_relaxed(&ReleaseToOsIntervalMs);
if (IntervalMs < 0)
return 0;
if (Sci->ReleaseInfo.LastReleaseAtNs +
static_cast<u64>(IntervalMs) * 1000000 >
getMonotonicTime()) {
return 0;
}
}

const uptr First = Sci->MinRegionIndex;
const uptr Last = Sci->MaxRegionIndex;
DCHECK_NE(Last, 0U);
DCHECK_LE(First, Last);
uptr TotalReleasedBytes = 0;
const uptr Base = First * RegionSize;
const uptr NumberOfRegions = Last - First + 1U;
ReleaseRecorder Recorder(Base);
auto SkipRegion = [this, First, ClassId](uptr RegionIndex) {
return (PossibleRegions[First + RegionIndex] - 1U) != ClassId;
};
auto DecompactPtr = [](CompactPtrT CompactPtr) {
return reinterpret_cast<uptr>(CompactPtr);
};
releaseFreeMemoryToOS(Sci->FreeList, RegionSize, NumberOfRegions, BlockSize,
&Recorder, DecompactPtr, SkipRegion);
if (Recorder.getReleasedRangesCount() > 0) {
Sci->ReleaseInfo.PushedBlocksAtLastRelease = Sci->Stats.PushedBlocks;
Sci->ReleaseInfo.RangesReleased += Recorder.getReleasedRangesCount();
Sci->ReleaseInfo.LastReleasedBytes = Recorder.getReleasedBytes();
TotalReleasedBytes += Sci->ReleaseInfo.LastReleasedBytes;
}
Sci->ReleaseInfo.LastReleaseAtNs = getMonotonicTime();

return TotalReleasedBytes;
}

SizeClassInfo SizeClassInfoArray[NumClasses] = {};


ByteMap PossibleRegions = {};
atomic_s32 ReleaseToOsIntervalMs = {};


static constexpr uptr MaxStashedRegions = 4;
HybridMutex RegionsStashMutex;
uptr NumberOfStashedRegions = 0;
uptr RegionsStash[MaxStashedRegions] = {};
};

}

#endif
