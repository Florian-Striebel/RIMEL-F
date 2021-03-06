







#if !defined(SCUDO_RELEASE_H_)
#define SCUDO_RELEASE_H_

#include "common.h"
#include "list.h"
#include "mutex.h"

namespace scudo {

class ReleaseRecorder {
public:
ReleaseRecorder(uptr Base, MapPlatformData *Data = nullptr)
: Base(Base), Data(Data) {}

uptr getReleasedRangesCount() const { return ReleasedRangesCount; }

uptr getReleasedBytes() const { return ReleasedBytes; }

uptr getBase() const { return Base; }


void releasePageRangeToOS(uptr From, uptr To) {
const uptr Size = To - From;
releasePagesToOS(Base, From, Size, Data);
ReleasedRangesCount++;
ReleasedBytes += Size;
}

private:
uptr ReleasedRangesCount = 0;
uptr ReleasedBytes = 0;
uptr Base = 0;
MapPlatformData *Data = nullptr;
};









class PackedCounterArray {
public:
PackedCounterArray(uptr NumberOfRegions, uptr CountersPerRegion,
uptr MaxValue)
: Regions(NumberOfRegions), NumCounters(CountersPerRegion) {
DCHECK_GT(Regions, 0);
DCHECK_GT(NumCounters, 0);
DCHECK_GT(MaxValue, 0);
constexpr uptr MaxCounterBits = sizeof(*Buffer) * 8UL;


const uptr CounterSizeBits =
roundUpToPowerOfTwo(getMostSignificantSetBitIndex(MaxValue) + 1);
DCHECK_LE(CounterSizeBits, MaxCounterBits);
CounterSizeBitsLog = getLog2(CounterSizeBits);
CounterMask = ~(static_cast<uptr>(0)) >> (MaxCounterBits - CounterSizeBits);

const uptr PackingRatio = MaxCounterBits >> CounterSizeBitsLog;
DCHECK_GT(PackingRatio, 0);
PackingRatioLog = getLog2(PackingRatio);
BitOffsetMask = PackingRatio - 1;

SizePerRegion =
roundUpTo(NumCounters, static_cast<uptr>(1U) << PackingRatioLog) >>
PackingRatioLog;
BufferSize = SizePerRegion * sizeof(*Buffer) * Regions;
if (BufferSize <= (StaticBufferCount * sizeof(Buffer[0])) &&
Mutex.tryLock()) {
Buffer = &StaticBuffer[0];
memset(Buffer, 0, BufferSize);
} else {
Buffer = reinterpret_cast<uptr *>(
map(nullptr, roundUpTo(BufferSize, getPageSizeCached()),
"scudo:counters", MAP_ALLOWNOMEM));
}
}
~PackedCounterArray() {
if (!isAllocated())
return;
if (Buffer == &StaticBuffer[0])
Mutex.unlock();
else
unmap(reinterpret_cast<void *>(Buffer),
roundUpTo(BufferSize, getPageSizeCached()));
}

bool isAllocated() const { return !!Buffer; }

uptr getCount() const { return NumCounters; }

uptr get(uptr Region, uptr I) const {
DCHECK_LT(Region, Regions);
DCHECK_LT(I, NumCounters);
const uptr Index = I >> PackingRatioLog;
const uptr BitOffset = (I & BitOffsetMask) << CounterSizeBitsLog;
return (Buffer[Region * SizePerRegion + Index] >> BitOffset) & CounterMask;
}

void inc(uptr Region, uptr I) const {
DCHECK_LT(get(Region, I), CounterMask);
const uptr Index = I >> PackingRatioLog;
const uptr BitOffset = (I & BitOffsetMask) << CounterSizeBitsLog;
DCHECK_LT(BitOffset, SCUDO_WORDSIZE);
Buffer[Region * SizePerRegion + Index] += static_cast<uptr>(1U)
<< BitOffset;
}

void incRange(uptr Region, uptr From, uptr To) const {
DCHECK_LE(From, To);
const uptr Top = Min(To + 1, NumCounters);
for (uptr I = From; I < Top; I++)
inc(Region, I);
}

uptr getBufferSize() const { return BufferSize; }

static const uptr StaticBufferCount = 2048U;

private:
const uptr Regions;
const uptr NumCounters;
uptr CounterSizeBitsLog;
uptr CounterMask;
uptr PackingRatioLog;
uptr BitOffsetMask;

uptr SizePerRegion;
uptr BufferSize;
uptr *Buffer;

static HybridMutex Mutex;
static uptr StaticBuffer[StaticBufferCount];
};

template <class ReleaseRecorderT> class FreePagesRangeTracker {
public:
explicit FreePagesRangeTracker(ReleaseRecorderT *Recorder)
: Recorder(Recorder), PageSizeLog(getLog2(getPageSizeCached())) {}

void processNextPage(bool Freed) {
if (Freed) {
if (!InRange) {
CurrentRangeStatePage = CurrentPage;
InRange = true;
}
} else {
closeOpenedRange();
}
CurrentPage++;
}

void skipPages(uptr N) {
closeOpenedRange();
CurrentPage += N;
}

void finish() { closeOpenedRange(); }

private:
void closeOpenedRange() {
if (InRange) {
Recorder->releasePageRangeToOS((CurrentRangeStatePage << PageSizeLog),
(CurrentPage << PageSizeLog));
InRange = false;
}
}

ReleaseRecorderT *const Recorder;
const uptr PageSizeLog;
bool InRange = false;
uptr CurrentPage = 0;
uptr CurrentRangeStatePage = 0;
};

template <class TransferBatchT, class ReleaseRecorderT, typename DecompactPtrT,
typename SkipRegionT>
NOINLINE void
releaseFreeMemoryToOS(const IntrusiveList<TransferBatchT> &FreeList,
uptr RegionSize, uptr NumberOfRegions, uptr BlockSize,
ReleaseRecorderT *Recorder, DecompactPtrT DecompactPtr,
SkipRegionT SkipRegion) {
const uptr PageSize = getPageSizeCached();



uptr FullPagesBlockCountMax;
bool SameBlockCountPerPage;
if (BlockSize <= PageSize) {
if (PageSize % BlockSize == 0) {

FullPagesBlockCountMax = PageSize / BlockSize;
SameBlockCountPerPage = true;
} else if (BlockSize % (PageSize % BlockSize) == 0) {



FullPagesBlockCountMax = PageSize / BlockSize + 1;
SameBlockCountPerPage = true;
} else {


FullPagesBlockCountMax = PageSize / BlockSize + 2;
SameBlockCountPerPage = false;
}
} else {
if (BlockSize % PageSize == 0) {

FullPagesBlockCountMax = 1;
SameBlockCountPerPage = true;
} else {


FullPagesBlockCountMax = 2;
SameBlockCountPerPage = false;
}
}

const uptr PagesCount = roundUpTo(RegionSize, PageSize) / PageSize;
PackedCounterArray Counters(NumberOfRegions, PagesCount,
FullPagesBlockCountMax);
if (!Counters.isAllocated())
return;

const uptr PageSizeLog = getLog2(PageSize);
const uptr RoundedRegionSize = PagesCount << PageSizeLog;
const uptr RoundedSize = NumberOfRegions * RoundedRegionSize;



if (BlockSize <= PageSize && PageSize % BlockSize == 0) {

for (const auto &It : FreeList) {
for (u32 I = 0; I < It.getCount(); I++) {
const uptr P = DecompactPtr(It.get(I)) - Recorder->getBase();
if (P >= RoundedSize)
continue;
const uptr RegionIndex = NumberOfRegions == 1U ? 0 : P / RegionSize;
const uptr PInRegion = P - RegionIndex * RegionSize;
Counters.inc(RegionIndex, PInRegion >> PageSizeLog);
}
}
} else {

DCHECK_GE(RegionSize, BlockSize);
const uptr LastBlockInRegion = ((RegionSize / BlockSize) - 1U) * BlockSize;
for (const auto &It : FreeList) {
for (u32 I = 0; I < It.getCount(); I++) {
const uptr P = DecompactPtr(It.get(I)) - Recorder->getBase();
if (P >= RoundedSize)
continue;
const uptr RegionIndex = NumberOfRegions == 1U ? 0 : P / RegionSize;
uptr PInRegion = P - RegionIndex * RegionSize;
Counters.incRange(RegionIndex, PInRegion >> PageSizeLog,
(PInRegion + BlockSize - 1) >> PageSizeLog);


if (PInRegion == LastBlockInRegion) {
PInRegion += BlockSize;
while (PInRegion < RoundedRegionSize) {
Counters.incRange(RegionIndex, PInRegion >> PageSizeLog,
(PInRegion + BlockSize - 1) >> PageSizeLog);
PInRegion += BlockSize;
}
}
}
}
}



FreePagesRangeTracker<ReleaseRecorderT> RangeTracker(Recorder);
if (SameBlockCountPerPage) {

for (uptr I = 0; I < NumberOfRegions; I++) {
if (SkipRegion(I)) {
RangeTracker.skipPages(PagesCount);
continue;
}
for (uptr J = 0; J < PagesCount; J++)
RangeTracker.processNextPage(Counters.get(I, J) ==
FullPagesBlockCountMax);
}
} else {


const uptr Pn = BlockSize < PageSize ? PageSize / BlockSize : 1;
const uptr Pnc = Pn * BlockSize;





for (uptr I = 0; I < NumberOfRegions; I++) {
if (SkipRegion(I)) {
RangeTracker.skipPages(PagesCount);
continue;
}
uptr PrevPageBoundary = 0;
uptr CurrentBoundary = 0;
for (uptr J = 0; J < PagesCount; J++) {
const uptr PageBoundary = PrevPageBoundary + PageSize;
uptr BlocksPerPage = Pn;
if (CurrentBoundary < PageBoundary) {
if (CurrentBoundary > PrevPageBoundary)
BlocksPerPage++;
CurrentBoundary += Pnc;
if (CurrentBoundary < PageBoundary) {
BlocksPerPage++;
CurrentBoundary += BlockSize;
}
}
PrevPageBoundary = PageBoundary;
RangeTracker.processNextPage(Counters.get(I, J) == BlocksPerPage);
}
}
}
RangeTracker.finish();
}

}

#endif
