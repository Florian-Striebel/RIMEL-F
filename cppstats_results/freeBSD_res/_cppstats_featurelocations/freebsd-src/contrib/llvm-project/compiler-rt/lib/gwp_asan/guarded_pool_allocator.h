







#if !defined(GWP_ASAN_GUARDED_POOL_ALLOCATOR_H_)
#define GWP_ASAN_GUARDED_POOL_ALLOCATOR_H_

#include "gwp_asan/common.h"
#include "gwp_asan/definitions.h"
#include "gwp_asan/mutex.h"
#include "gwp_asan/options.h"
#include "gwp_asan/platform_specific/guarded_pool_allocator_fuchsia.h"
#include "gwp_asan/platform_specific/guarded_pool_allocator_posix.h"
#include "gwp_asan/platform_specific/guarded_pool_allocator_tls.h"

#include <stddef.h>
#include <stdint.h>


namespace gwp_asan {







class GuardedPoolAllocator {
public:

static constexpr const char *kGwpAsanMetadataName = "GWP-ASan Metadata";






constexpr GuardedPoolAllocator() {}
GuardedPoolAllocator(const GuardedPoolAllocator &) = delete;
GuardedPoolAllocator &operator=(const GuardedPoolAllocator &) = delete;





~GuardedPoolAllocator() = default;




void init(const options::Options &Opts);
void uninitTestOnly();




void disable();
void enable();

typedef void (*iterate_callback)(uintptr_t base, size_t size, void *arg);



void iterate(void *Base, size_t Size, iterate_callback Cb, void *Arg);




void stop();


GWP_ASAN_ALWAYS_INLINE bool shouldSample() {






if (GWP_ASAN_UNLIKELY(getThreadLocals()->NextSampleCounter == 0))
getThreadLocals()->NextSampleCounter =
((getRandomUnsigned32() % (AdjustedSampleRatePlusOne - 1)) + 1) &
ThreadLocalPackedVariables::NextSampleCounterMask;

return GWP_ASAN_UNLIKELY(--getThreadLocals()->NextSampleCounter == 0);
}



GWP_ASAN_ALWAYS_INLINE bool pointerIsMine(const void *Ptr) const {
return State.pointerIsMine(Ptr);
}







void *allocate(size_t Size, size_t Alignment = alignof(max_align_t));



void deallocate(void *Ptr);


size_t getSize(const void *Ptr);


const AllocationMetadata *getMetadataRegion() const { return Metadata; }


const AllocatorState *getAllocatorState() const { return &State; }


protected:


static size_t getRequiredBackingSize(size_t Size, size_t Alignment,
size_t PageSize);



static uintptr_t alignUp(uintptr_t Ptr, size_t Alignment);
static uintptr_t alignDown(uintptr_t Ptr, size_t Alignment);

private:

static constexpr const char *kGwpAsanAliveSlotName = "GWP-ASan Alive Slot";


static constexpr const char *kGwpAsanGuardPageName = "GWP-ASan Guard Page";

static constexpr const char *kGwpAsanFreeSlotsName = "GWP-ASan Metadata";

static constexpr size_t kInvalidSlotID = SIZE_MAX;










void *map(size_t Size, const char *Name) const;
void unmap(void *Ptr, size_t Size) const;









void *reserveGuardedPool(size_t Size);


void allocateInGuardedPool(void *Ptr, size_t Size) const;


void deallocateInGuardedPool(void *Ptr, size_t Size) const;
void unreserveGuardedPool();



static size_t getPlatformPageSize();



AllocationMetadata *addrToMetadata(uintptr_t Ptr) const;



size_t reserveSlot();


void freeSlot(size_t SlotIndex);




void trapOnAddress(uintptr_t Address, Error E);

static GuardedPoolAllocator *getSingleton();


void installAtFork();

gwp_asan::AllocatorState State;


Mutex PoolMutex;



Mutex BacktraceMutex;



size_t NumSampledAllocations = 0;


AllocationMetadata *Metadata = nullptr;


size_t *FreeSlots = nullptr;

size_t FreeSlotsLength = 0;


bool PerfectlyRightAlign = false;



options::Backtrace_t Backtrace = nullptr;







uint32_t AdjustedSampleRatePlusOne = 0;


PlatformSpecificMapData GuardedPagePoolPlatformData = {};

class ScopedRecursiveGuard {
public:
ScopedRecursiveGuard() { getThreadLocals()->RecursiveGuard = true; }
~ScopedRecursiveGuard() { getThreadLocals()->RecursiveGuard = false; }
};


void initPRNG();



uint32_t getRandomUnsigned32();
};
}

#endif
