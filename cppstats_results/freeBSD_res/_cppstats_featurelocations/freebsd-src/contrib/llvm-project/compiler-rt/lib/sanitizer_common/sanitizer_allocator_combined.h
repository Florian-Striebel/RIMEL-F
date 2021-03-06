










#if !defined(SANITIZER_ALLOCATOR_H)
#error This file must be included inside sanitizer_allocator.h
#endif







template <class PrimaryAllocator,
class LargeMmapAllocatorPtrArray = DefaultLargeMmapAllocatorPtrArray>
class CombinedAllocator {
public:
using AllocatorCache = typename PrimaryAllocator::AllocatorCache;
using SecondaryAllocator =
LargeMmapAllocator<typename PrimaryAllocator::MapUnmapCallback,
LargeMmapAllocatorPtrArray,
typename PrimaryAllocator::AddressSpaceView>;

void InitLinkerInitialized(s32 release_to_os_interval_ms) {
stats_.InitLinkerInitialized();
primary_.Init(release_to_os_interval_ms);
secondary_.InitLinkerInitialized();
}

void Init(s32 release_to_os_interval_ms, uptr heap_start = 0) {
stats_.Init();
primary_.Init(release_to_os_interval_ms, heap_start);
secondary_.Init();
}

void *Allocate(AllocatorCache *cache, uptr size, uptr alignment) {

if (size == 0)
size = 1;
if (size + alignment < size) {
Report("WARNING: %s: CombinedAllocator allocation overflow: "
"0x%zx bytes with 0x%zx alignment requested\n",
SanitizerToolName, size, alignment);
return nullptr;
}
uptr original_size = size;




if (alignment > 8)
size = RoundUpTo(size, alignment);






void *res;
if (primary_.CanAllocate(size, alignment))
res = cache->Allocate(&primary_, primary_.ClassID(size));
else
res = secondary_.Allocate(&stats_, original_size, alignment);
if (alignment > 8)
CHECK_EQ(reinterpret_cast<uptr>(res) & (alignment - 1), 0);
return res;
}

s32 ReleaseToOSIntervalMs() const {
return primary_.ReleaseToOSIntervalMs();
}

void SetReleaseToOSIntervalMs(s32 release_to_os_interval_ms) {
primary_.SetReleaseToOSIntervalMs(release_to_os_interval_ms);
}

void ForceReleaseToOS() {
primary_.ForceReleaseToOS();
}

void Deallocate(AllocatorCache *cache, void *p) {
if (!p) return;
if (primary_.PointerIsMine(p))
cache->Deallocate(&primary_, primary_.GetSizeClass(p), p);
else
secondary_.Deallocate(&stats_, p);
}

void *Reallocate(AllocatorCache *cache, void *p, uptr new_size,
uptr alignment) {
if (!p)
return Allocate(cache, new_size, alignment);
if (!new_size) {
Deallocate(cache, p);
return nullptr;
}
CHECK(PointerIsMine(p));
uptr old_size = GetActuallyAllocatedSize(p);
uptr memcpy_size = Min(new_size, old_size);
void *new_p = Allocate(cache, new_size, alignment);
if (new_p)
internal_memcpy(new_p, p, memcpy_size);
Deallocate(cache, p);
return new_p;
}

bool PointerIsMine(void *p) {
if (primary_.PointerIsMine(p))
return true;
return secondary_.PointerIsMine(p);
}

bool FromPrimary(void *p) {
return primary_.PointerIsMine(p);
}

void *GetMetaData(const void *p) {
if (primary_.PointerIsMine(p))
return primary_.GetMetaData(p);
return secondary_.GetMetaData(p);
}

void *GetBlockBegin(const void *p) {
if (primary_.PointerIsMine(p))
return primary_.GetBlockBegin(p);
return secondary_.GetBlockBegin(p);
}



void *GetBlockBeginFastLocked(void *p) {
if (primary_.PointerIsMine(p))
return primary_.GetBlockBegin(p);
return secondary_.GetBlockBeginFastLocked(p);
}

uptr GetActuallyAllocatedSize(void *p) {
if (primary_.PointerIsMine(p))
return primary_.GetActuallyAllocatedSize(p);
return secondary_.GetActuallyAllocatedSize(p);
}

uptr TotalMemoryUsed() {
return primary_.TotalMemoryUsed() + secondary_.TotalMemoryUsed();
}

void TestOnlyUnmap() { primary_.TestOnlyUnmap(); }

void InitCache(AllocatorCache *cache) {
cache->Init(&stats_);
}

void DestroyCache(AllocatorCache *cache) {
cache->Destroy(&primary_, &stats_);
}

void SwallowCache(AllocatorCache *cache) {
cache->Drain(&primary_);
}

void GetStats(AllocatorStatCounters s) const {
stats_.Get(s);
}

void PrintStats() {
primary_.PrintStats();
secondary_.PrintStats();
}



void ForceLock() NO_THREAD_SAFETY_ANALYSIS {
primary_.ForceLock();
secondary_.ForceLock();
}

void ForceUnlock() NO_THREAD_SAFETY_ANALYSIS {
secondary_.ForceUnlock();
primary_.ForceUnlock();
}



void ForEachChunk(ForEachChunkCallback callback, void *arg) {
primary_.ForEachChunk(callback, arg);
secondary_.ForEachChunk(callback, arg);
}

private:
PrimaryAllocator primary_;
SecondaryAllocator secondary_;
AllocatorGlobalStats stats_;
};
