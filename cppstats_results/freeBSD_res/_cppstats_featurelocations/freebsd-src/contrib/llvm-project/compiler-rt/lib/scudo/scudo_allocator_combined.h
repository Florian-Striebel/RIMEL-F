












#if !defined(SCUDO_ALLOCATOR_COMBINED_H_)
#define SCUDO_ALLOCATOR_COMBINED_H_

#if !defined(SCUDO_ALLOCATOR_H_)
#error "This file must be included inside scudo_allocator.h."
#endif

class CombinedAllocator {
public:
using PrimaryAllocator = PrimaryT;
using SecondaryAllocator = SecondaryT;
using AllocatorCache = typename PrimaryAllocator::AllocatorCache;
void init(s32 ReleaseToOSIntervalMs) {
Primary.Init(ReleaseToOSIntervalMs);
Secondary.Init();
Stats.Init();
}



void *allocatePrimary(AllocatorCache *Cache, uptr ClassId) {
return Cache->Allocate(&Primary, ClassId);
}



void *allocateSecondary(uptr Size, uptr Alignment) {
return Secondary.Allocate(&Stats, Size, Alignment);
}

void deallocatePrimary(AllocatorCache *Cache, void *Ptr, uptr ClassId) {
Cache->Deallocate(&Primary, ClassId, Ptr);
}

void deallocateSecondary(void *Ptr) {
Secondary.Deallocate(&Stats, Ptr);
}

void initCache(AllocatorCache *Cache) {
Cache->Init(&Stats);
}

void destroyCache(AllocatorCache *Cache) {
Cache->Destroy(&Primary, &Stats);
}

void getStats(AllocatorStatCounters StatType) const {
Stats.Get(StatType);
}

void printStats() {
Primary.PrintStats();
Secondary.PrintStats();
}

private:
PrimaryAllocator Primary;
SecondaryAllocator Secondary;
AllocatorGlobalStats Stats;
};

#endif
