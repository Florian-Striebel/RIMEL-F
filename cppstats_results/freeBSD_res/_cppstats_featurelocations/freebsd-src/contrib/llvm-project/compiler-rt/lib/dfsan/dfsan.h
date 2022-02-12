












#if !defined(DFSAN_H)
#define DFSAN_H

#include "sanitizer_common/sanitizer_internal_defs.h"

#include "dfsan_platform.h"

using __sanitizer::u32;
using __sanitizer::u8;
using __sanitizer::uptr;


typedef u8 dfsan_label;
typedef u32 dfsan_origin;

extern "C" {
void dfsan_add_label(dfsan_label label, void *addr, uptr size);
void dfsan_set_label(dfsan_label label, void *addr, uptr size);
dfsan_label dfsan_read_label(const void *addr, uptr size);
dfsan_label dfsan_union(dfsan_label l1, dfsan_label l2);

void dfsan_clear_arg_tls(uptr offset, uptr size);

void dfsan_clear_thread_local_state();



dfsan_origin dfsan_read_origin_of_first_taint(const void *addr, uptr size);


void dfsan_set_label_origin(dfsan_label label, dfsan_origin origin, void *addr,
uptr size);


void dfsan_mem_origin_transfer(const void *dst, const void *src, uptr len);
}

template <typename T>
void dfsan_set_label(dfsan_label label, T &data) {
dfsan_set_label(label, (void *)&data, sizeof(T));
}

namespace __dfsan {

extern bool dfsan_inited;
extern bool dfsan_init_is_running;

void initialize_interceptors();

inline dfsan_label *shadow_for(void *ptr) {
return (dfsan_label *)MEM_TO_SHADOW(ptr);
}

inline const dfsan_label *shadow_for(const void *ptr) {
return shadow_for(const_cast<void *>(ptr));
}

inline uptr unaligned_origin_for(uptr ptr) { return MEM_TO_ORIGIN(ptr); }

inline dfsan_origin *origin_for(void *ptr) {
auto aligned_addr = unaligned_origin_for(reinterpret_cast<uptr>(ptr)) &
~(sizeof(dfsan_origin) - 1);
return reinterpret_cast<dfsan_origin *>(aligned_addr);
}

inline const dfsan_origin *origin_for(const void *ptr) {
return origin_for(const_cast<void *>(ptr));
}

void dfsan_copy_memory(void *dst, const void *src, uptr size);

void dfsan_allocator_init();
void dfsan_deallocate(void *ptr);

void *dfsan_malloc(uptr size);
void *dfsan_calloc(uptr nmemb, uptr size);
void *dfsan_realloc(void *ptr, uptr size);
void *dfsan_reallocarray(void *ptr, uptr nmemb, uptr size);
void *dfsan_valloc(uptr size);
void *dfsan_pvalloc(uptr size);
void *dfsan_aligned_alloc(uptr alignment, uptr size);
void *dfsan_memalign(uptr alignment, uptr size);
int dfsan_posix_memalign(void **memptr, uptr alignment, uptr size);

void dfsan_init();

}

#endif
