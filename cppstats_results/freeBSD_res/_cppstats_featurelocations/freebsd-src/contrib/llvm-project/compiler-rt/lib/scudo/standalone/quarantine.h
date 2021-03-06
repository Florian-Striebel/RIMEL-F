







#if !defined(SCUDO_QUARANTINE_H_)
#define SCUDO_QUARANTINE_H_

#include "list.h"
#include "mutex.h"
#include "string_utils.h"

namespace scudo {

struct QuarantineBatch {


static const u32 MaxCount = 1019;
QuarantineBatch *Next;
uptr Size;
u32 Count;
void *Batch[MaxCount];

void init(void *Ptr, uptr Size) {
Count = 1;
Batch[0] = Ptr;
this->Size = Size + sizeof(QuarantineBatch);
}


uptr getQuarantinedSize() const { return Size - sizeof(QuarantineBatch); }

void push_back(void *Ptr, uptr Size) {
DCHECK_LT(Count, MaxCount);
Batch[Count++] = Ptr;
this->Size += Size;
}

bool canMerge(const QuarantineBatch *const From) const {
return Count + From->Count <= MaxCount;
}

void merge(QuarantineBatch *const From) {
DCHECK_LE(Count + From->Count, MaxCount);
DCHECK_GE(Size, sizeof(QuarantineBatch));

for (uptr I = 0; I < From->Count; ++I)
Batch[Count + I] = From->Batch[I];
Count += From->Count;
Size += From->getQuarantinedSize();

From->Count = 0;
From->Size = sizeof(QuarantineBatch);
}

void shuffle(u32 State) { ::scudo::shuffle(Batch, Count, &State); }
};

static_assert(sizeof(QuarantineBatch) <= (1U << 13), "");


template <typename Callback> class QuarantineCache {
public:
void init() { DCHECK_EQ(atomic_load_relaxed(&Size), 0U); }


uptr getSize() const { return atomic_load_relaxed(&Size); }

uptr getOverheadSize() const { return List.size() * sizeof(QuarantineBatch); }

void enqueue(Callback Cb, void *Ptr, uptr Size) {
if (List.empty() || List.back()->Count == QuarantineBatch::MaxCount) {
QuarantineBatch *B =
reinterpret_cast<QuarantineBatch *>(Cb.allocate(sizeof(*B)));
DCHECK(B);
B->init(Ptr, Size);
enqueueBatch(B);
} else {
List.back()->push_back(Ptr, Size);
addToSize(Size);
}
}

void transfer(QuarantineCache *From) {
List.append_back(&From->List);
addToSize(From->getSize());
atomic_store_relaxed(&From->Size, 0);
}

void enqueueBatch(QuarantineBatch *B) {
List.push_back(B);
addToSize(B->Size);
}

QuarantineBatch *dequeueBatch() {
if (List.empty())
return nullptr;
QuarantineBatch *B = List.front();
List.pop_front();
subFromSize(B->Size);
return B;
}

void mergeBatches(QuarantineCache *ToDeallocate) {
uptr ExtractedSize = 0;
QuarantineBatch *Current = List.front();
while (Current && Current->Next) {
if (Current->canMerge(Current->Next)) {
QuarantineBatch *Extracted = Current->Next;

Current->merge(Extracted);
DCHECK_EQ(Extracted->Count, 0);
DCHECK_EQ(Extracted->Size, sizeof(QuarantineBatch));

List.extract(Current, Extracted);
ExtractedSize += Extracted->Size;

ToDeallocate->enqueueBatch(Extracted);
} else {
Current = Current->Next;
}
}
subFromSize(ExtractedSize);
}

void getStats(ScopedString *Str) const {
uptr BatchCount = 0;
uptr TotalOverheadBytes = 0;
uptr TotalBytes = 0;
uptr TotalQuarantineChunks = 0;
for (const QuarantineBatch &Batch : List) {
BatchCount++;
TotalBytes += Batch.Size;
TotalOverheadBytes += Batch.Size - Batch.getQuarantinedSize();
TotalQuarantineChunks += Batch.Count;
}
const uptr QuarantineChunksCapacity =
BatchCount * QuarantineBatch::MaxCount;
const uptr ChunksUsagePercent =
(QuarantineChunksCapacity == 0)
? 0
: TotalQuarantineChunks * 100 / QuarantineChunksCapacity;
const uptr TotalQuarantinedBytes = TotalBytes - TotalOverheadBytes;
const uptr MemoryOverheadPercent =
(TotalQuarantinedBytes == 0)
? 0
: TotalOverheadBytes * 100 / TotalQuarantinedBytes;
Str->append(
"Stats: Quarantine: batches: %zu; bytes: %zu (user: %zu); chunks: %zu "
"(capacity: %zu); %zu%% chunks used; %zu%% memory overhead\n",
BatchCount, TotalBytes, TotalQuarantinedBytes, TotalQuarantineChunks,
QuarantineChunksCapacity, ChunksUsagePercent, MemoryOverheadPercent);
}

private:
SinglyLinkedList<QuarantineBatch> List;
atomic_uptr Size = {};

void addToSize(uptr add) { atomic_store_relaxed(&Size, getSize() + add); }
void subFromSize(uptr sub) { atomic_store_relaxed(&Size, getSize() - sub); }
};





template <typename Callback, typename Node> class GlobalQuarantine {
public:
typedef QuarantineCache<Callback> CacheT;
using ThisT = GlobalQuarantine<Callback, Node>;

void init(uptr Size, uptr CacheSize) {
DCHECK(isAligned(reinterpret_cast<uptr>(this), alignof(ThisT)));
DCHECK_EQ(atomic_load_relaxed(&MaxSize), 0U);
DCHECK_EQ(atomic_load_relaxed(&MinSize), 0U);
DCHECK_EQ(atomic_load_relaxed(&MaxCacheSize), 0U);


CHECK((Size == 0 && CacheSize == 0) || CacheSize != 0);

atomic_store_relaxed(&MaxSize, Size);
atomic_store_relaxed(&MinSize, Size / 10 * 9);
atomic_store_relaxed(&MaxCacheSize, CacheSize);

Cache.init();
}

uptr getMaxSize() const { return atomic_load_relaxed(&MaxSize); }
uptr getCacheSize() const { return atomic_load_relaxed(&MaxCacheSize); }

void put(CacheT *C, Callback Cb, Node *Ptr, uptr Size) {
C->enqueue(Cb, Ptr, Size);
if (C->getSize() > getCacheSize())
drain(C, Cb);
}

void NOINLINE drain(CacheT *C, Callback Cb) {
{
ScopedLock L(CacheMutex);
Cache.transfer(C);
}
if (Cache.getSize() > getMaxSize() && RecycleMutex.tryLock())
recycle(atomic_load_relaxed(&MinSize), Cb);
}

void NOINLINE drainAndRecycle(CacheT *C, Callback Cb) {
{
ScopedLock L(CacheMutex);
Cache.transfer(C);
}
RecycleMutex.lock();
recycle(0, Cb);
}

void getStats(ScopedString *Str) const {

Cache.getStats(Str);
Str->append("Quarantine limits: global: %zuK; thread local: %zuK\n",
getMaxSize() >> 10, getCacheSize() >> 10);
}

void disable() {

RecycleMutex.lock();
CacheMutex.lock();
}

void enable() {
CacheMutex.unlock();
RecycleMutex.unlock();
}

private:

alignas(SCUDO_CACHE_LINE_SIZE) HybridMutex CacheMutex;
CacheT Cache;
alignas(SCUDO_CACHE_LINE_SIZE) HybridMutex RecycleMutex;
atomic_uptr MinSize = {};
atomic_uptr MaxSize = {};
alignas(SCUDO_CACHE_LINE_SIZE) atomic_uptr MaxCacheSize = {};

void NOINLINE recycle(uptr MinSize, Callback Cb) {
CacheT Tmp;
Tmp.init();
{
ScopedLock L(CacheMutex);





const uptr CacheSize = Cache.getSize();
const uptr OverheadSize = Cache.getOverheadSize();
DCHECK_GE(CacheSize, OverheadSize);



constexpr uptr OverheadThresholdPercents = 100;
if (CacheSize > OverheadSize &&
OverheadSize * (100 + OverheadThresholdPercents) >
CacheSize * OverheadThresholdPercents) {
Cache.mergeBatches(&Tmp);
}


while (Cache.getSize() > MinSize)
Tmp.enqueueBatch(Cache.dequeueBatch());
}
RecycleMutex.unlock();
doRecycle(&Tmp, Cb);
}

void NOINLINE doRecycle(CacheT *C, Callback Cb) {
while (QuarantineBatch *B = C->dequeueBatch()) {
const u32 Seed = static_cast<u32>(
(reinterpret_cast<uptr>(B) ^ reinterpret_cast<uptr>(C)) >> 4);
B->shuffle(Seed);
constexpr uptr NumberOfPrefetch = 8UL;
CHECK(NumberOfPrefetch <= ARRAY_SIZE(B->Batch));
for (uptr I = 0; I < NumberOfPrefetch; I++)
PREFETCH(B->Batch[I]);
for (uptr I = 0, Count = B->Count; I < Count; I++) {
if (I + NumberOfPrefetch < Count)
PREFETCH(B->Batch[I + NumberOfPrefetch]);
Cb.recycle(reinterpret_cast<Node *>(B->Batch[I]));
}
Cb.deallocate(B);
}
}
};

}

#endif
