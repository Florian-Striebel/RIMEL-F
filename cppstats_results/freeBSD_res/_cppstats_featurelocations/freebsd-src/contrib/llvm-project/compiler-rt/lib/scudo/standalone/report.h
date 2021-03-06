







#if !defined(SCUDO_REPORT_H_)
#define SCUDO_REPORT_H_

#include "internal_defs.h"

namespace scudo {




void NORETURN reportError(const char *Message);


void NORETURN reportInvalidFlag(const char *FlagType, const char *Value);


void NORETURN reportHeaderCorruption(void *Ptr);
void NORETURN reportHeaderRace(void *Ptr);


void NORETURN reportSanityCheckError(const char *Field);


void NORETURN reportAlignmentTooBig(uptr Alignment, uptr MaxAlignment);
void NORETURN reportAllocationSizeTooBig(uptr UserSize, uptr TotalSize,
uptr MaxSize);
void NORETURN reportOutOfMemory(uptr RequestedSize);
enum class AllocatorAction : u8 {
Recycling,
Deallocating,
Reallocating,
Sizing,
};
void NORETURN reportInvalidChunkState(AllocatorAction Action, void *Ptr);
void NORETURN reportMisalignedPointer(AllocatorAction Action, void *Ptr);
void NORETURN reportDeallocTypeMismatch(AllocatorAction Action, void *Ptr,
u8 TypeA, u8 TypeB);
void NORETURN reportDeleteSizeMismatch(void *Ptr, uptr Size, uptr ExpectedSize);


void NORETURN reportAlignmentNotPowerOfTwo(uptr Alignment);
void NORETURN reportInvalidPosixMemalignAlignment(uptr Alignment);
void NORETURN reportCallocOverflow(uptr Count, uptr Size);
void NORETURN reportPvallocOverflow(uptr Size);
void NORETURN reportInvalidAlignedAllocAlignment(uptr Size, uptr Alignment);

}

#endif
