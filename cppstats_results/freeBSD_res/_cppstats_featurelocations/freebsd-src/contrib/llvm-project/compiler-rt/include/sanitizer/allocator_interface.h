









#if !defined(SANITIZER_ALLOCATOR_INTERFACE_H)
#define SANITIZER_ALLOCATOR_INTERFACE_H

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif




size_t __sanitizer_get_estimated_allocated_size(size_t size);



int __sanitizer_get_ownership(const volatile void *p);



size_t __sanitizer_get_allocated_size(const volatile void *p);


size_t __sanitizer_get_current_allocated_bytes(void);







size_t __sanitizer_get_heap_size(void);





size_t __sanitizer_get_free_bytes(void);



size_t __sanitizer_get_unmapped_bytes(void);






void __sanitizer_malloc_hook(const volatile void *ptr, size_t size);
void __sanitizer_free_hook(const volatile void *ptr);













int __sanitizer_install_malloc_and_free_hooks(
void (*malloc_hook)(const volatile void *, size_t),
void (*free_hook)(const volatile void *));






void __sanitizer_purge_allocator(void);
#if defined(__cplusplus)
}
#endif

#endif
