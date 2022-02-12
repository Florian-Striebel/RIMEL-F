














#if !defined(SANITIZER_QUARANTINE_H)
#define SANITIZER_QUARANTINE_H

#include "sanitizer_internal_defs.h"
#include "sanitizer_mutex.h"
#include "sanitizer_list.h"

namespace __sanitizer {

template<typename Node> class QuarantineCache;

struct QuarantineBatch {
static const uptr kSize = 1021;
QuarantineBatch *next;
uptr size;
uptr count;
void *batch[kSize];

void init(void *ptr, uptr size) {
count = 1;
batch[0] = ptr;
this->size = size + sizeof(QuarantineBatch);
}


uptr quarantined_size() const {
return size - sizeof(QuarantineBatch);
}

void push_back(void *ptr, uptr size) {
CHECK_LT(count, kSize);
batch[count++] = ptr;
this->size += size;
}

bool can_merge(const QuarantineBatch* const from) const {
return count + from->count <= kSize;
}

void merge(QuarantineBatch* const from) {
CHECK_LE(count + from->count, kSize);
CHECK_GE(size, sizeof(QuarantineBatch));

for (uptr i = 0; i < from->count; ++i)
batch[count + i] = from->batch[i];
count += from->count;
size += from->quarantined_size();

from->count = 0;
from->size = sizeof(QuarantineBatch);
}
};

COMPILER_CHECK(sizeof(QuarantineBatch) <= (1 << 13));





template<typename Callback, typename Node>
class Quarantine {
public:
typedef QuarantineCache<Callback> Cache;

explicit Quarantine(LinkerInitialized)
: cache_(LINKER_INITIALIZED) {
}

void Init(uptr size, uptr cache_size) {


CHECK((size == 0 && cache_size == 0) || cache_size != 0);

atomic_store_relaxed(&max_size_, size);
atomic_store_relaxed(&min_size_, size / 10 * 9);
atomic_store_relaxed(&max_cache_size_, cache_size);

cache_mutex_.Init();
recycle_mutex_.Init();
}

uptr GetSize() const { return atomic_load_relaxed(&max_size_); }
uptr GetCacheSize() const {
return atomic_load_relaxed(&max_cache_size_);
}

void Put(Cache *c, Callback cb, Node *ptr, uptr size) {
uptr cache_size = GetCacheSize();
if (cache_size) {
c->Enqueue(cb, ptr, size);
} else {

cb.Recycle(ptr);
}

if (c->Size() > cache_size)
Drain(c, cb);
}

void NOINLINE Drain(Cache *c, Callback cb) {
{
SpinMutexLock l(&cache_mutex_);
cache_.Transfer(c);
}
if (cache_.Size() > GetSize() && recycle_mutex_.TryLock())
Recycle(atomic_load_relaxed(&min_size_), cb);
}

void NOINLINE DrainAndRecycle(Cache *c, Callback cb) {
{
SpinMutexLock l(&cache_mutex_);
cache_.Transfer(c);
}
recycle_mutex_.Lock();
Recycle(0, cb);
}

void PrintStats() const {

Printf("Quarantine limits: global: %zdMb; thread local: %zdKb\n",
GetSize() >> 20, GetCacheSize() >> 10);
cache_.PrintStats();
}

private:

char pad0_[kCacheLineSize];
atomic_uintptr_t max_size_;
atomic_uintptr_t min_size_;
atomic_uintptr_t max_cache_size_;
char pad1_[kCacheLineSize];
StaticSpinMutex cache_mutex_;
StaticSpinMutex recycle_mutex_;
Cache cache_;
char pad2_[kCacheLineSize];

void NOINLINE Recycle(uptr min_size, Callback cb) REQUIRES(recycle_mutex_)
RELEASE(recycle_mutex_) {
Cache tmp;
{
SpinMutexLock l(&cache_mutex_);





uptr cache_size = cache_.Size();
uptr overhead_size = cache_.OverheadSize();
CHECK_GE(cache_size, overhead_size);



const uptr kOverheadThresholdPercents = 100;
if (cache_size > overhead_size &&
overhead_size * (100 + kOverheadThresholdPercents) >
cache_size * kOverheadThresholdPercents) {
cache_.MergeBatches(&tmp);
}


while (cache_.Size() > min_size) {
tmp.EnqueueBatch(cache_.DequeueBatch());
}
}
recycle_mutex_.Unlock();
DoRecycle(&tmp, cb);
}

void NOINLINE DoRecycle(Cache *c, Callback cb) {
while (QuarantineBatch *b = c->DequeueBatch()) {
const uptr kPrefetch = 16;
CHECK(kPrefetch <= ARRAY_SIZE(b->batch));
for (uptr i = 0; i < kPrefetch; i++)
PREFETCH(b->batch[i]);
for (uptr i = 0, count = b->count; i < count; i++) {
if (i + kPrefetch < count)
PREFETCH(b->batch[i + kPrefetch]);
cb.Recycle((Node*)b->batch[i]);
}
cb.Deallocate(b);
}
}
};


template<typename Callback>
class QuarantineCache {
public:
explicit QuarantineCache(LinkerInitialized) {
}

QuarantineCache()
: size_() {
list_.clear();
}


uptr Size() const {
return atomic_load_relaxed(&size_);
}


uptr OverheadSize() const {
return list_.size() * sizeof(QuarantineBatch);
}

void Enqueue(Callback cb, void *ptr, uptr size) {
if (list_.empty() || list_.back()->count == QuarantineBatch::kSize) {
QuarantineBatch *b = (QuarantineBatch *)cb.Allocate(sizeof(*b));
CHECK(b);
b->init(ptr, size);
EnqueueBatch(b);
} else {
list_.back()->push_back(ptr, size);
SizeAdd(size);
}
}

void Transfer(QuarantineCache *from_cache) {
list_.append_back(&from_cache->list_);
SizeAdd(from_cache->Size());

atomic_store_relaxed(&from_cache->size_, 0);
}

void EnqueueBatch(QuarantineBatch *b) {
list_.push_back(b);
SizeAdd(b->size);
}

QuarantineBatch *DequeueBatch() {
if (list_.empty())
return nullptr;
QuarantineBatch *b = list_.front();
list_.pop_front();
SizeSub(b->size);
return b;
}

void MergeBatches(QuarantineCache *to_deallocate) {
uptr extracted_size = 0;
QuarantineBatch *current = list_.front();
while (current && current->next) {
if (current->can_merge(current->next)) {
QuarantineBatch *extracted = current->next;

current->merge(extracted);
CHECK_EQ(extracted->count, 0);
CHECK_EQ(extracted->size, sizeof(QuarantineBatch));

list_.extract(current, extracted);
extracted_size += extracted->size;

to_deallocate->EnqueueBatch(extracted);
} else {
current = current->next;
}
}
SizeSub(extracted_size);
}

void PrintStats() const {
uptr batch_count = 0;
uptr total_overhead_bytes = 0;
uptr total_bytes = 0;
uptr total_quarantine_chunks = 0;
for (List::ConstIterator it = list_.begin(); it != list_.end(); ++it) {
batch_count++;
total_bytes += (*it).size;
total_overhead_bytes += (*it).size - (*it).quarantined_size();
total_quarantine_chunks += (*it).count;
}
uptr quarantine_chunks_capacity = batch_count * QuarantineBatch::kSize;
int chunks_usage_percent = quarantine_chunks_capacity == 0 ?
0 : total_quarantine_chunks * 100 / quarantine_chunks_capacity;
uptr total_quarantined_bytes = total_bytes - total_overhead_bytes;
int memory_overhead_percent = total_quarantined_bytes == 0 ?
0 : total_overhead_bytes * 100 / total_quarantined_bytes;
Printf("Global quarantine stats: batches: %zd; bytes: %zd (user: %zd); "
"chunks: %zd (capacity: %zd); %d%% chunks used; %d%% memory overhead"
"\n",
batch_count, total_bytes, total_quarantined_bytes,
total_quarantine_chunks, quarantine_chunks_capacity,
chunks_usage_percent, memory_overhead_percent);
}

private:
typedef IntrusiveList<QuarantineBatch> List;

List list_;
atomic_uintptr_t size_;

void SizeAdd(uptr add) {
atomic_store_relaxed(&size_, Size() + add);
}
void SizeSub(uptr sub) {
atomic_store_relaxed(&size_, Size() - sub);
}
};

}

#endif
