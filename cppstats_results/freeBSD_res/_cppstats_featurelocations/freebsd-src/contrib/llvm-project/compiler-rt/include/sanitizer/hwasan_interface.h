











#if !defined(SANITIZER_HWASAN_INTERFACE_H)
#define SANITIZER_HWASAN_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif








void __hwasan_init_static(void);



const char* __hwasan_default_options(void);

void __hwasan_enable_allocator_tagging(void);
void __hwasan_disable_allocator_tagging(void);



void __hwasan_tag_memory(const volatile void *p, unsigned char tag,
size_t size);


void *__hwasan_tag_pointer(const volatile void *p, unsigned char tag);






void __hwasan_handle_longjmp(const void *sp_dst);



void __hwasan_handle_vfork(const void *sp_dst);



void __hwasan_thread_enter();



void __hwasan_thread_exit();



void __hwasan_print_shadow(const volatile void *x, size_t size);


void __hwasan_print_memory_usage();



intptr_t __hwasan_test_shadow(const volatile void *x, size_t size);


void __hwasan_set_error_report_callback(void (*callback)(const char *));

int __sanitizer_posix_memalign(void **memptr, size_t alignment, size_t size);
void * __sanitizer_memalign(size_t alignment, size_t size);
void * __sanitizer_aligned_alloc(size_t alignment, size_t size);
void * __sanitizer___libc_memalign(size_t alignment, size_t size);
void * __sanitizer_valloc(size_t size);
void * __sanitizer_pvalloc(size_t size);
void __sanitizer_free(void *ptr);
void __sanitizer_cfree(void *ptr);
size_t __sanitizer_malloc_usable_size(const void *ptr);
struct mallinfo __sanitizer_mallinfo();
int __sanitizer_mallopt(int cmd, int value);
void __sanitizer_malloc_stats(void);
void * __sanitizer_calloc(size_t nmemb, size_t size);
void * __sanitizer_realloc(void *ptr, size_t size);
void * __sanitizer_reallocarray(void *ptr, size_t nmemb, size_t size);
void * __sanitizer_malloc(size_t size);
#if defined(__cplusplus)
}
#endif

#endif
