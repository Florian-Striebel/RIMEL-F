












#if !defined(GWP_ASAN_INTERFACE_H_)
#define GWP_ASAN_INTERFACE_H_

#include "gwp_asan/common.h"

#if defined(__cplusplus)
extern "C" {
#endif
















bool __gwp_asan_error_is_mine(const gwp_asan::AllocatorState *State,
uintptr_t ErrorPtr = 0u);




gwp_asan::Error
__gwp_asan_diagnose_error(const gwp_asan::AllocatorState *State,
const gwp_asan::AllocationMetadata *Metadata,
uintptr_t ErrorPtr);





uintptr_t
__gwp_asan_get_internal_crash_address(const gwp_asan::AllocatorState *State);






const gwp_asan::AllocationMetadata *
__gwp_asan_get_metadata(const gwp_asan::AllocatorState *State,
const gwp_asan::AllocationMetadata *Metadata,
uintptr_t ErrorPtr);










uintptr_t __gwp_asan_get_allocation_address(
const gwp_asan::AllocationMetadata *AllocationMeta);


size_t __gwp_asan_get_allocation_size(
const gwp_asan::AllocationMetadata *AllocationMeta);




uint64_t __gwp_asan_get_allocation_thread_id(
const gwp_asan::AllocationMetadata *AllocationMeta);







size_t __gwp_asan_get_allocation_trace(
const gwp_asan::AllocationMetadata *AllocationMeta, uintptr_t *Buffer,
size_t BufferLen);




bool __gwp_asan_is_deallocated(
const gwp_asan::AllocationMetadata *AllocationMeta);




uint64_t __gwp_asan_get_deallocation_thread_id(
const gwp_asan::AllocationMetadata *AllocationMeta);








size_t __gwp_asan_get_deallocation_trace(
const gwp_asan::AllocationMetadata *AllocationMeta, uintptr_t *Buffer,
size_t BufferLen);

#if defined(__cplusplus)
}
#endif

#endif
