







#if !defined(SCUDO_INTERFACE_H_)
#define SCUDO_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

extern "C" {

__attribute__((weak)) const char *__scudo_default_options();



__attribute__((weak)) void __scudo_allocate_hook(void *ptr, size_t size);
__attribute__((weak)) void __scudo_deallocate_hook(void *ptr);

void __scudo_print_stats(void);

typedef void (*iterate_callback)(uintptr_t base, size_t size, void *arg);















































void __scudo_get_error_info(struct scudo_error_info *error_info,
uintptr_t fault_addr, const char *stack_depot,
const char *region_info, const char *ring_buffer,
const char *memory, const char *memory_tags,
uintptr_t memory_addr, size_t memory_size);

enum scudo_error_type {
UNKNOWN,
USE_AFTER_FREE,
BUFFER_OVERFLOW,
BUFFER_UNDERFLOW,
};

struct scudo_error_report {
enum scudo_error_type error_type;

uintptr_t allocation_address;
uintptr_t allocation_size;

uint32_t allocation_tid;
uintptr_t allocation_trace[64];

uint32_t deallocation_tid;
uintptr_t deallocation_trace[64];
};

struct scudo_error_info {
struct scudo_error_report reports[3];
};

const char *__scudo_get_stack_depot_addr();
size_t __scudo_get_stack_depot_size();

const char *__scudo_get_region_info_addr();
size_t __scudo_get_region_info_size();

const char *__scudo_get_ring_buffer_addr();
size_t __scudo_get_ring_buffer_size();

#if !defined(M_DECAY_TIME)
#define M_DECAY_TIME -100
#endif

#if !defined(M_PURGE)
#define M_PURGE -101
#endif




#if !defined(M_MEMTAG_TUNING)
#define M_MEMTAG_TUNING -102
#endif





#if !defined(M_THREAD_DISABLE_MEM_INIT)
#define M_THREAD_DISABLE_MEM_INIT -103
#endif

#if !defined(M_CACHE_COUNT_MAX)
#define M_CACHE_COUNT_MAX -200
#endif

#if !defined(M_CACHE_SIZE_MAX)
#define M_CACHE_SIZE_MAX -201
#endif

#if !defined(M_TSDS_COUNT_MAX)
#define M_TSDS_COUNT_MAX -202
#endif


#if !defined(M_MEMTAG_TUNING_BUFFER_OVERFLOW)
#define M_MEMTAG_TUNING_BUFFER_OVERFLOW 0
#endif


#if !defined(M_MEMTAG_TUNING_UAF)
#define M_MEMTAG_TUNING_UAF 1
#endif

}

#endif
