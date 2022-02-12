










#if !defined(SANITIZER_ALLOCATOR_H)
#error This file must be included inside sanitizer_allocator.h
#endif

template<class SizeClassAllocator> struct SizeClassAllocator64LocalCache;






















struct SizeClassAllocator64FlagMasks {
enum {
kRandomShuffleChunks = 1,
};
};

template <typename Allocator>
class MemoryMapper {
public:
typedef typename Allocator::CompactPtrT CompactPtrT;

explicit MemoryMapper(const Allocator &allocator) : allocator_(allocator) {}

bool GetAndResetStats(uptr &ranges, uptr &bytes) {
ranges = released_ranges_count_;
released_ranges_count_ = 0;
bytes = released_bytes_;
released_bytes_ = 0;
return ranges != 0;
}

u64 *MapPackedCounterArrayBuffer(uptr count) {
buffer_.clear();
buffer_.resize(count);
return buffer_.data();
}


void ReleasePageRangeToOS(uptr class_id, CompactPtrT from, CompactPtrT to) {
const uptr region_base = allocator_.GetRegionBeginBySizeClass(class_id);
const uptr from_page = allocator_.CompactPtrToPointer(region_base, from);
const uptr to_page = allocator_.CompactPtrToPointer(region_base, to);
ReleaseMemoryPagesToOS(from_page, to_page);
released_ranges_count_++;
released_bytes_ += to_page - from_page;
}

private:
const Allocator &allocator_;
uptr released_ranges_count_ = 0;
uptr released_bytes_ = 0;
InternalMmapVector<u64> buffer_;
};

template <class Params>
class SizeClassAllocator64 {
public:
using AddressSpaceView = typename Params::AddressSpaceView;
static const uptr kSpaceBeg = Params::kSpaceBeg;
static const uptr kSpaceSize = Params::kSpaceSize;
static const uptr kMetadataSize = Params::kMetadataSize;
typedef typename Params::SizeClassMap SizeClassMap;
typedef typename Params::MapUnmapCallback MapUnmapCallback;

static const bool kRandomShuffleChunks =
Params::kFlags & SizeClassAllocator64FlagMasks::kRandomShuffleChunks;

typedef SizeClassAllocator64<Params> ThisT;
typedef SizeClassAllocator64LocalCache<ThisT> AllocatorCache;
typedef MemoryMapper<ThisT> MemoryMapperT;



typedef u32 CompactPtrT;
static const uptr kCompactPtrScale = 4;
CompactPtrT PointerToCompactPtr(uptr base, uptr ptr) const {
return static_cast<CompactPtrT>((ptr - base) >> kCompactPtrScale);
}
uptr CompactPtrToPointer(uptr base, CompactPtrT ptr32) const {
return base + (static_cast<uptr>(ptr32) << kCompactPtrScale);
}




void Init(s32 release_to_os_interval_ms, uptr heap_start = 0) {
uptr TotalSpaceSize = kSpaceSize + AdditionalSize();
PremappedHeap = heap_start != 0;
if (PremappedHeap) {
CHECK(!kUsingConstantSpaceBeg);
NonConstSpaceBeg = heap_start;
uptr RegionInfoSize = AdditionalSize();
RegionInfoSpace =
address_range.Init(RegionInfoSize, PrimaryAllocatorName);
CHECK_NE(RegionInfoSpace, ~(uptr)0);
CHECK_EQ(RegionInfoSpace,
address_range.MapOrDie(RegionInfoSpace, RegionInfoSize,
"SizeClassAllocator: region info"));
MapUnmapCallback().OnMap(RegionInfoSpace, RegionInfoSize);
} else {
if (kUsingConstantSpaceBeg) {
CHECK(IsAligned(kSpaceBeg, SizeClassMap::kMaxSize));
CHECK_EQ(kSpaceBeg,
address_range.Init(TotalSpaceSize, PrimaryAllocatorName,
kSpaceBeg));
} else {




NonConstSpaceBeg = address_range.InitAligned(
TotalSpaceSize, SizeClassMap::kMaxSize, PrimaryAllocatorName);
CHECK_NE(NonConstSpaceBeg, ~(uptr)0);
}
RegionInfoSpace = SpaceEnd();
MapWithCallbackOrDie(RegionInfoSpace, AdditionalSize(),
"SizeClassAllocator: region info");
}
SetReleaseToOSIntervalMs(release_to_os_interval_ms);

DCHECK_EQ(RegionInfoSpace % kCacheLineSize, 0);
}

s32 ReleaseToOSIntervalMs() const {
return atomic_load(&release_to_os_interval_ms_, memory_order_relaxed);
}

void SetReleaseToOSIntervalMs(s32 release_to_os_interval_ms) {
atomic_store(&release_to_os_interval_ms_, release_to_os_interval_ms,
memory_order_relaxed);
}

void ForceReleaseToOS() {
MemoryMapperT memory_mapper(*this);
for (uptr class_id = 1; class_id < kNumClasses; class_id++) {
BlockingMutexLock l(&GetRegionInfo(class_id)->mutex);
MaybeReleaseToOS(&memory_mapper, class_id, true );
}
}

static bool CanAllocate(uptr size, uptr alignment) {
return size <= SizeClassMap::kMaxSize &&
alignment <= SizeClassMap::kMaxSize;
}

NOINLINE void ReturnToAllocator(MemoryMapperT *memory_mapper,
AllocatorStats *stat, uptr class_id,
const CompactPtrT *chunks, uptr n_chunks) {
RegionInfo *region = GetRegionInfo(class_id);
uptr region_beg = GetRegionBeginBySizeClass(class_id);
CompactPtrT *free_array = GetFreeArray(region_beg);

BlockingMutexLock l(&region->mutex);
uptr old_num_chunks = region->num_freed_chunks;
uptr new_num_freed_chunks = old_num_chunks + n_chunks;


if (UNLIKELY(!EnsureFreeArraySpace(region, region_beg,
new_num_freed_chunks))) {
Report("FATAL: Internal error: %s's allocator exhausted the free list "
"space for size class %zd (%zd bytes).\n", SanitizerToolName,
class_id, ClassIdToSize(class_id));
Die();
}
for (uptr i = 0; i < n_chunks; i++)
free_array[old_num_chunks + i] = chunks[i];
region->num_freed_chunks = new_num_freed_chunks;
region->stats.n_freed += n_chunks;

MaybeReleaseToOS(memory_mapper, class_id, false );
}

NOINLINE bool GetFromAllocator(AllocatorStats *stat, uptr class_id,
CompactPtrT *chunks, uptr n_chunks) {
RegionInfo *region = GetRegionInfo(class_id);
uptr region_beg = GetRegionBeginBySizeClass(class_id);
CompactPtrT *free_array = GetFreeArray(region_beg);

BlockingMutexLock l(&region->mutex);
#if SANITIZER_WINDOWS



if (region->rtoi.last_released_bytes > 0) {
MmapFixedOrDie(region_beg, region->mapped_user,
"SizeClassAllocator: region data");
region->rtoi.n_freed_at_last_release = 0;
region->rtoi.last_released_bytes = 0;
}
#endif
if (UNLIKELY(region->num_freed_chunks < n_chunks)) {
if (UNLIKELY(!PopulateFreeArray(stat, class_id, region,
n_chunks - region->num_freed_chunks)))
return false;
CHECK_GE(region->num_freed_chunks, n_chunks);
}
region->num_freed_chunks -= n_chunks;
uptr base_idx = region->num_freed_chunks;
for (uptr i = 0; i < n_chunks; i++)
chunks[i] = free_array[base_idx + i];
region->stats.n_allocated += n_chunks;
return true;
}

bool PointerIsMine(const void *p) const {
uptr P = reinterpret_cast<uptr>(p);
if (kUsingConstantSpaceBeg && (kSpaceBeg % kSpaceSize) == 0)
return P / kSpaceSize == kSpaceBeg / kSpaceSize;
return P >= SpaceBeg() && P < SpaceEnd();
}

uptr GetRegionBegin(const void *p) {
if (kUsingConstantSpaceBeg)
return reinterpret_cast<uptr>(p) & ~(kRegionSize - 1);
uptr space_beg = SpaceBeg();
return ((reinterpret_cast<uptr>(p) - space_beg) & ~(kRegionSize - 1)) +
space_beg;
}

uptr GetRegionBeginBySizeClass(uptr class_id) const {
return SpaceBeg() + kRegionSize * class_id;
}

uptr GetSizeClass(const void *p) {
if (kUsingConstantSpaceBeg && (kSpaceBeg % kSpaceSize) == 0)
return ((reinterpret_cast<uptr>(p)) / kRegionSize) % kNumClassesRounded;
return ((reinterpret_cast<uptr>(p) - SpaceBeg()) / kRegionSize) %
kNumClassesRounded;
}

void *GetBlockBegin(const void *p) {
uptr class_id = GetSizeClass(p);
if (class_id >= kNumClasses) return nullptr;
uptr size = ClassIdToSize(class_id);
if (!size) return nullptr;
uptr chunk_idx = GetChunkIdx((uptr)p, size);
uptr reg_beg = GetRegionBegin(p);
uptr beg = chunk_idx * size;
uptr next_beg = beg + size;
const RegionInfo *region = AddressSpaceView::Load(GetRegionInfo(class_id));
if (region->mapped_user >= next_beg)
return reinterpret_cast<void*>(reg_beg + beg);
return nullptr;
}

uptr GetActuallyAllocatedSize(void *p) {
CHECK(PointerIsMine(p));
return ClassIdToSize(GetSizeClass(p));
}

static uptr ClassID(uptr size) { return SizeClassMap::ClassID(size); }

void *GetMetaData(const void *p) {
CHECK(kMetadataSize);
uptr class_id = GetSizeClass(p);
uptr size = ClassIdToSize(class_id);
uptr chunk_idx = GetChunkIdx(reinterpret_cast<uptr>(p), size);
uptr region_beg = GetRegionBeginBySizeClass(class_id);
return reinterpret_cast<void *>(GetMetadataEnd(region_beg) -
(1 + chunk_idx) * kMetadataSize);
}

uptr TotalMemoryUsed() {
uptr res = 0;
for (uptr i = 0; i < kNumClasses; i++)
res += GetRegionInfo(i)->allocated_user;
return res;
}


void TestOnlyUnmap() {
UnmapWithCallbackOrDie((uptr)address_range.base(), address_range.size());
}

static void FillMemoryProfile(uptr start, uptr rss, bool file, uptr *stats,
uptr stats_size) {
for (uptr class_id = 0; class_id < stats_size; class_id++)
if (stats[class_id] == start)
stats[class_id] = rss;
}

void PrintStats(uptr class_id, uptr rss) {
RegionInfo *region = GetRegionInfo(class_id);
if (region->mapped_user == 0) return;
uptr in_use = region->stats.n_allocated - region->stats.n_freed;
uptr avail_chunks = region->allocated_user / ClassIdToSize(class_id);
Printf(
"%s %02zd (%6zd): mapped: %6zdK allocs: %7zd frees: %7zd inuse: %6zd "
"num_freed_chunks %7zd avail: %6zd rss: %6zdK releases: %6zd "
"last released: %6zdK region: 0x%zx\n",
region->exhausted ? "F" : " ", class_id, ClassIdToSize(class_id),
region->mapped_user >> 10, region->stats.n_allocated,
region->stats.n_freed, in_use, region->num_freed_chunks, avail_chunks,
rss >> 10, region->rtoi.num_releases,
region->rtoi.last_released_bytes >> 10,
SpaceBeg() + kRegionSize * class_id);
}

void PrintStats() {
uptr rss_stats[kNumClasses];
for (uptr class_id = 0; class_id < kNumClasses; class_id++)
rss_stats[class_id] = SpaceBeg() + kRegionSize * class_id;
GetMemoryProfile(FillMemoryProfile, rss_stats, kNumClasses);

uptr total_mapped = 0;
uptr total_rss = 0;
uptr n_allocated = 0;
uptr n_freed = 0;
for (uptr class_id = 1; class_id < kNumClasses; class_id++) {
RegionInfo *region = GetRegionInfo(class_id);
if (region->mapped_user != 0) {
total_mapped += region->mapped_user;
total_rss += rss_stats[class_id];
}
n_allocated += region->stats.n_allocated;
n_freed += region->stats.n_freed;
}

Printf("Stats: SizeClassAllocator64: %zdM mapped (%zdM rss) in "
"%zd allocations; remains %zd\n", total_mapped >> 20,
total_rss >> 20, n_allocated, n_allocated - n_freed);
for (uptr class_id = 1; class_id < kNumClasses; class_id++)
PrintStats(class_id, rss_stats[class_id]);
}



void ForceLock() NO_THREAD_SAFETY_ANALYSIS {
for (uptr i = 0; i < kNumClasses; i++) {
GetRegionInfo(i)->mutex.Lock();
}
}

void ForceUnlock() NO_THREAD_SAFETY_ANALYSIS {
for (int i = (int)kNumClasses - 1; i >= 0; i--) {
GetRegionInfo(i)->mutex.Unlock();
}
}



void ForEachChunk(ForEachChunkCallback callback, void *arg) {
for (uptr class_id = 1; class_id < kNumClasses; class_id++) {
RegionInfo *region = GetRegionInfo(class_id);
uptr chunk_size = ClassIdToSize(class_id);
uptr region_beg = SpaceBeg() + class_id * kRegionSize;
uptr region_allocated_user_size =
AddressSpaceView::Load(region)->allocated_user;
for (uptr chunk = region_beg;
chunk < region_beg + region_allocated_user_size;
chunk += chunk_size) {

callback(chunk, arg);
}
}
}

static uptr ClassIdToSize(uptr class_id) {
return SizeClassMap::Size(class_id);
}

static uptr AdditionalSize() {
return RoundUpTo(sizeof(RegionInfo) * kNumClassesRounded,
GetPageSizeCached());
}

typedef SizeClassMap SizeClassMapT;
static const uptr kNumClasses = SizeClassMap::kNumClasses;
static const uptr kNumClassesRounded = SizeClassMap::kNumClassesRounded;








class PackedCounterArray {
public:
template <typename MemoryMapper>
PackedCounterArray(u64 num_counters, u64 max_value, MemoryMapper *mapper)
: n(num_counters) {
CHECK_GT(num_counters, 0);
CHECK_GT(max_value, 0);
constexpr u64 kMaxCounterBits = sizeof(*buffer) * 8ULL;


uptr counter_size_bits =
RoundUpToPowerOfTwo(MostSignificantSetBitIndex(max_value) + 1);
CHECK_LE(counter_size_bits, kMaxCounterBits);
counter_size_bits_log = Log2(counter_size_bits);
counter_mask = ~0ULL >> (kMaxCounterBits - counter_size_bits);

uptr packing_ratio = kMaxCounterBits >> counter_size_bits_log;
CHECK_GT(packing_ratio, 0);
packing_ratio_log = Log2(packing_ratio);
bit_offset_mask = packing_ratio - 1;

buffer = mapper->MapPackedCounterArrayBuffer(
RoundUpTo(n, 1ULL << packing_ratio_log) >> packing_ratio_log);
}

bool IsAllocated() const {
return !!buffer;
}

u64 GetCount() const {
return n;
}

uptr Get(uptr i) const {
DCHECK_LT(i, n);
uptr index = i >> packing_ratio_log;
uptr bit_offset = (i & bit_offset_mask) << counter_size_bits_log;
return (buffer[index] >> bit_offset) & counter_mask;
}

void Inc(uptr i) const {
DCHECK_LT(Get(i), counter_mask);
uptr index = i >> packing_ratio_log;
uptr bit_offset = (i & bit_offset_mask) << counter_size_bits_log;
buffer[index] += 1ULL << bit_offset;
}

void IncRange(uptr from, uptr to) const {
DCHECK_LE(from, to);
for (uptr i = from; i <= to; i++)
Inc(i);
}

private:
const u64 n;
u64 counter_size_bits_log;
u64 counter_mask;
u64 packing_ratio_log;
u64 bit_offset_mask;
u64* buffer;
};

template <class MemoryMapperT>
class FreePagesRangeTracker {
public:
FreePagesRangeTracker(MemoryMapperT *mapper, uptr class_id)
: memory_mapper(mapper),
class_id(class_id),
page_size_scaled_log(Log2(GetPageSizeCached() >> kCompactPtrScale)) {}

void NextPage(bool freed) {
if (freed) {
if (!in_the_range) {
current_range_start_page = current_page;
in_the_range = true;
}
} else {
CloseOpenedRange();
}
current_page++;
}

void Done() {
CloseOpenedRange();
}

private:
void CloseOpenedRange() {
if (in_the_range) {
memory_mapper->ReleasePageRangeToOS(
class_id, current_range_start_page << page_size_scaled_log,
current_page << page_size_scaled_log);
in_the_range = false;
}
}

MemoryMapperT *const memory_mapper = nullptr;
const uptr class_id = 0;
const uptr page_size_scaled_log = 0;
bool in_the_range = false;
uptr current_page = 0;
uptr current_range_start_page = 0;
};





template <typename MemoryMapper>
static void ReleaseFreeMemoryToOS(CompactPtrT *free_array,
uptr free_array_count, uptr chunk_size,
uptr allocated_pages_count,
MemoryMapper *memory_mapper,
uptr class_id) {
const uptr page_size = GetPageSizeCached();



uptr full_pages_chunk_count_max;
bool same_chunk_count_per_page;
if (chunk_size <= page_size && page_size % chunk_size == 0) {

full_pages_chunk_count_max = page_size / chunk_size;
same_chunk_count_per_page = true;
} else if (chunk_size <= page_size && page_size % chunk_size != 0 &&
chunk_size % (page_size % chunk_size) == 0) {



full_pages_chunk_count_max = page_size / chunk_size + 1;
same_chunk_count_per_page = true;
} else if (chunk_size <= page_size) {


full_pages_chunk_count_max = page_size / chunk_size + 2;
same_chunk_count_per_page = false;
} else if (chunk_size > page_size && chunk_size % page_size == 0) {

full_pages_chunk_count_max = 1;
same_chunk_count_per_page = true;
} else if (chunk_size > page_size) {


full_pages_chunk_count_max = 2;
same_chunk_count_per_page = false;
} else {
UNREACHABLE("All chunk_size/page_size ratios must be handled.");
}

PackedCounterArray counters(allocated_pages_count,
full_pages_chunk_count_max, memory_mapper);
if (!counters.IsAllocated())
return;

const uptr chunk_size_scaled = chunk_size >> kCompactPtrScale;
const uptr page_size_scaled = page_size >> kCompactPtrScale;
const uptr page_size_scaled_log = Log2(page_size_scaled);



if (chunk_size <= page_size && page_size % chunk_size == 0) {

for (uptr i = 0; i < free_array_count; i++)
counters.Inc(free_array[i] >> page_size_scaled_log);
} else {

for (uptr i = 0; i < free_array_count; i++) {
counters.IncRange(
free_array[i] >> page_size_scaled_log,
(free_array[i] + chunk_size_scaled - 1) >> page_size_scaled_log);
}
}



FreePagesRangeTracker<MemoryMapper> range_tracker(memory_mapper, class_id);
if (same_chunk_count_per_page) {

for (uptr i = 0; i < counters.GetCount(); i++)
range_tracker.NextPage(counters.Get(i) == full_pages_chunk_count_max);
} else {


const uptr pn =
chunk_size < page_size ? page_size_scaled / chunk_size_scaled : 1;
const uptr pnc = pn * chunk_size_scaled;





uptr prev_page_boundary = 0;
uptr current_boundary = 0;
for (uptr i = 0; i < counters.GetCount(); i++) {
uptr page_boundary = prev_page_boundary + page_size_scaled;
uptr chunks_per_page = pn;
if (current_boundary < page_boundary) {
if (current_boundary > prev_page_boundary)
chunks_per_page++;
current_boundary += pnc;
if (current_boundary < page_boundary) {
chunks_per_page++;
current_boundary += chunk_size_scaled;
}
}
prev_page_boundary = page_boundary;

range_tracker.NextPage(counters.Get(i) == chunks_per_page);
}
}
range_tracker.Done();
}

private:
friend class MemoryMapper<ThisT>;

ReservedAddressRange address_range;

static const uptr kRegionSize = kSpaceSize / kNumClassesRounded;




static const uptr kFreeArraySize = kRegionSize / 8;

static const bool kUsingConstantSpaceBeg = kSpaceBeg != ~(uptr)0;
uptr NonConstSpaceBeg;
uptr SpaceBeg() const {
return kUsingConstantSpaceBeg ? kSpaceBeg : NonConstSpaceBeg;
}
uptr SpaceEnd() const { return SpaceBeg() + kSpaceSize; }

COMPILER_CHECK((kRegionSize) >= (1ULL << (SANITIZER_WORDSIZE / 2)));

COMPILER_CHECK((kRegionSize) <= (1ULL << (SANITIZER_WORDSIZE / 2 + 4)));

static const uptr kUserMapSize = 1 << 16;

static const uptr kMetaMapSize = 1 << 16;

static const uptr kFreeArrayMapSize = 1 << 16;

atomic_sint32_t release_to_os_interval_ms_;

uptr RegionInfoSpace;


bool PremappedHeap;

struct Stats {
uptr n_allocated;
uptr n_freed;
};

struct ReleaseToOsInfo {
uptr n_freed_at_last_release;
uptr num_releases;
u64 last_release_at_ns;
u64 last_released_bytes;
};

struct ALIGNED(SANITIZER_CACHE_LINE_SIZE) RegionInfo {
BlockingMutex mutex;
uptr num_freed_chunks;
uptr mapped_free_array;
uptr allocated_user;
uptr allocated_meta;
uptr mapped_user;
uptr mapped_meta;
u32 rand_state;
bool exhausted;
Stats stats;
ReleaseToOsInfo rtoi;
};
COMPILER_CHECK(sizeof(RegionInfo) % kCacheLineSize == 0);

RegionInfo *GetRegionInfo(uptr class_id) const {
DCHECK_LT(class_id, kNumClasses);
RegionInfo *regions = reinterpret_cast<RegionInfo *>(RegionInfoSpace);
return &regions[class_id];
}

uptr GetMetadataEnd(uptr region_beg) const {
return region_beg + kRegionSize - kFreeArraySize;
}

uptr GetChunkIdx(uptr chunk, uptr size) const {
if (!kUsingConstantSpaceBeg)
chunk -= SpaceBeg();

uptr offset = chunk % kRegionSize;


if (offset >> (SANITIZER_WORDSIZE / 2))
return offset / size;
return (u32)offset / (u32)size;
}

CompactPtrT *GetFreeArray(uptr region_beg) const {
return reinterpret_cast<CompactPtrT *>(GetMetadataEnd(region_beg));
}

bool MapWithCallback(uptr beg, uptr size, const char *name) {
if (PremappedHeap)
return beg >= NonConstSpaceBeg &&
beg + size <= NonConstSpaceBeg + kSpaceSize;
uptr mapped = address_range.Map(beg, size, name);
if (UNLIKELY(!mapped))
return false;
CHECK_EQ(beg, mapped);
MapUnmapCallback().OnMap(beg, size);
return true;
}

void MapWithCallbackOrDie(uptr beg, uptr size, const char *name) {
if (PremappedHeap) {
CHECK_GE(beg, NonConstSpaceBeg);
CHECK_LE(beg + size, NonConstSpaceBeg + kSpaceSize);
return;
}
CHECK_EQ(beg, address_range.MapOrDie(beg, size, name));
MapUnmapCallback().OnMap(beg, size);
}

void UnmapWithCallbackOrDie(uptr beg, uptr size) {
if (PremappedHeap)
return;
MapUnmapCallback().OnUnmap(beg, size);
address_range.Unmap(beg, size);
}

bool EnsureFreeArraySpace(RegionInfo *region, uptr region_beg,
uptr num_freed_chunks) {
uptr needed_space = num_freed_chunks * sizeof(CompactPtrT);
if (region->mapped_free_array < needed_space) {
uptr new_mapped_free_array = RoundUpTo(needed_space, kFreeArrayMapSize);
CHECK_LE(new_mapped_free_array, kFreeArraySize);
uptr current_map_end = reinterpret_cast<uptr>(GetFreeArray(region_beg)) +
region->mapped_free_array;
uptr new_map_size = new_mapped_free_array - region->mapped_free_array;
if (UNLIKELY(!MapWithCallback(current_map_end, new_map_size,
"SizeClassAllocator: freearray")))
return false;
region->mapped_free_array = new_mapped_free_array;
}
return true;
}


bool IsRegionExhausted(RegionInfo *region, uptr class_id,
uptr additional_map_size) {
if (LIKELY(region->mapped_user + region->mapped_meta +
additional_map_size <= kRegionSize - kFreeArraySize))
return false;
if (!region->exhausted) {
region->exhausted = true;
Printf("%s: Out of memory. ", SanitizerToolName);
Printf("The process has exhausted %zuMB for size class %zu.\n",
kRegionSize >> 20, ClassIdToSize(class_id));
}
return true;
}

NOINLINE bool PopulateFreeArray(AllocatorStats *stat, uptr class_id,
RegionInfo *region, uptr requested_count) {

const uptr region_beg = GetRegionBeginBySizeClass(class_id);
const uptr size = ClassIdToSize(class_id);

const uptr total_user_bytes =
region->allocated_user + requested_count * size;

if (LIKELY(total_user_bytes > region->mapped_user)) {
if (UNLIKELY(region->mapped_user == 0)) {
if (!kUsingConstantSpaceBeg && kRandomShuffleChunks)

region->rand_state = static_cast<u32>(region_beg >> 12);






if (ReleaseToOSIntervalMs() >= 0)
region->rtoi.last_release_at_ns = MonotonicNanoTime();
}

const uptr user_map_size =
RoundUpTo(total_user_bytes - region->mapped_user, kUserMapSize);
if (UNLIKELY(IsRegionExhausted(region, class_id, user_map_size)))
return false;
if (UNLIKELY(!MapWithCallback(region_beg + region->mapped_user,
user_map_size,
"SizeClassAllocator: region data")))
return false;
stat->Add(AllocatorStatMapped, user_map_size);
region->mapped_user += user_map_size;
}
const uptr new_chunks_count =
(region->mapped_user - region->allocated_user) / size;

if (kMetadataSize) {

const uptr total_meta_bytes =
region->allocated_meta + new_chunks_count * kMetadataSize;
const uptr meta_map_size = (total_meta_bytes > region->mapped_meta) ?
RoundUpTo(total_meta_bytes - region->mapped_meta, kMetaMapSize) : 0;

if (meta_map_size) {
if (UNLIKELY(IsRegionExhausted(region, class_id, meta_map_size)))
return false;
if (UNLIKELY(!MapWithCallback(
GetMetadataEnd(region_beg) - region->mapped_meta - meta_map_size,
meta_map_size, "SizeClassAllocator: region metadata")))
return false;
region->mapped_meta += meta_map_size;
}
}



const uptr total_freed_chunks = region->num_freed_chunks + new_chunks_count;
if (UNLIKELY(!EnsureFreeArraySpace(region, region_beg, total_freed_chunks)))
return false;
CompactPtrT *free_array = GetFreeArray(region_beg);
for (uptr i = 0, chunk = region->allocated_user; i < new_chunks_count;
i++, chunk += size)
free_array[total_freed_chunks - 1 - i] = PointerToCompactPtr(0, chunk);
if (kRandomShuffleChunks)
RandomShuffle(&free_array[region->num_freed_chunks], new_chunks_count,
&region->rand_state);



region->num_freed_chunks += new_chunks_count;
region->allocated_user += new_chunks_count * size;
CHECK_LE(region->allocated_user, region->mapped_user);
region->allocated_meta += new_chunks_count * kMetadataSize;
CHECK_LE(region->allocated_meta, region->mapped_meta);
region->exhausted = false;





return true;
}






void MaybeReleaseToOS(MemoryMapperT *memory_mapper, uptr class_id,
bool force) {
RegionInfo *region = GetRegionInfo(class_id);
const uptr chunk_size = ClassIdToSize(class_id);
const uptr page_size = GetPageSizeCached();

uptr n = region->num_freed_chunks;
if (n * chunk_size < page_size)
return;
if ((region->stats.n_freed -
region->rtoi.n_freed_at_last_release) * chunk_size < page_size) {
return;
}

if (!force) {
s32 interval_ms = ReleaseToOSIntervalMs();
if (interval_ms < 0)
return;

if (region->rtoi.last_release_at_ns + interval_ms * 1000000ULL >
MonotonicNanoTime()) {
return;
}
}

ReleaseFreeMemoryToOS(
GetFreeArray(GetRegionBeginBySizeClass(class_id)), n, chunk_size,
RoundUpTo(region->allocated_user, page_size) / page_size, memory_mapper,
class_id);

uptr ranges, bytes;
if (memory_mapper->GetAndResetStats(ranges, bytes)) {
region->rtoi.n_freed_at_last_release = region->stats.n_freed;
region->rtoi.num_releases += ranges;
region->rtoi.last_released_bytes = bytes;
}
region->rtoi.last_release_at_ns = MonotonicNanoTime();
}
};
