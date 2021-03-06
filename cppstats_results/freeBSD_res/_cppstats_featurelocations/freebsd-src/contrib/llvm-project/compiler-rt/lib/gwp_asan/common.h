










#if !defined(GWP_ASAN_COMMON_H_)
#define GWP_ASAN_COMMON_H_

#include "gwp_asan/definitions.h"
#include "gwp_asan/options.h"

#include <stddef.h>
#include <stdint.h>

namespace gwp_asan {
enum class Error {
UNKNOWN,
USE_AFTER_FREE,
DOUBLE_FREE,
INVALID_FREE,
BUFFER_OVERFLOW,
BUFFER_UNDERFLOW
};

const char *ErrorToString(const Error &E);

static constexpr uint64_t kInvalidThreadID = UINT64_MAX;


uint64_t getThreadID();



struct AllocationMetadata {



static constexpr size_t kStackFrameStorageBytes = 256;




static constexpr size_t kMaxTraceLengthToCollect = 128;


void RecordAllocation(uintptr_t Addr, size_t RequestedSize);

void RecordDeallocation();

struct CallSiteInfo {

void RecordBacktrace(options::Backtrace_t Backtrace);


uint8_t CompressedTrace[kStackFrameStorageBytes];

uint64_t ThreadID = kInvalidThreadID;


size_t TraceSize = 0;
};



uintptr_t Addr = 0;

size_t RequestedSize = 0;

CallSiteInfo AllocationTrace;
CallSiteInfo DeallocationTrace;


bool IsDeallocated = false;
};




struct AllocatorState {
constexpr AllocatorState() {}



GWP_ASAN_ALWAYS_INLINE bool pointerIsMine(const void *Ptr) const {
uintptr_t P = reinterpret_cast<uintptr_t>(Ptr);
return P < GuardedPagePoolEnd && GuardedPagePool <= P;
}


uintptr_t slotToAddr(size_t N) const;


size_t maximumAllocationSize() const;


size_t getNearestSlot(uintptr_t Ptr) const;



bool isGuardPage(uintptr_t Ptr) const;


size_t MaxSimultaneousAllocations = 0;



uintptr_t GuardedPagePool = 0;
uintptr_t GuardedPagePoolEnd = 0;


size_t PageSize = 0;




Error FailureType = Error::UNKNOWN;
uintptr_t FailureAddress = 0;
};

}
#endif
