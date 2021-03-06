











#if !defined(SANITIZER_ASAN_INTERFACE_H)
#define SANITIZER_ASAN_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif













void __asan_poison_memory_region(void const volatile *addr, size_t size);













void __asan_unpoison_memory_region(void const volatile *addr, size_t size);


#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)







#define ASAN_POISON_MEMORY_REGION(addr, size) __asan_poison_memory_region((addr), (size))









#define ASAN_UNPOISON_MEMORY_REGION(addr, size) __asan_unpoison_memory_region((addr), (size))

#else
#define ASAN_POISON_MEMORY_REGION(addr, size) ((void)(addr), (void)(size))

#define ASAN_UNPOISON_MEMORY_REGION(addr, size) ((void)(addr), (void)(size))

#endif











int __asan_address_is_poisoned(void const volatile *addr);









void *__asan_region_is_poisoned(void *beg, size_t size);






void __asan_describe_address(void *addr);








int __asan_report_present(void);








void *__asan_get_report_pc(void);








void *__asan_get_report_bp(void);








void *__asan_get_report_sp(void);








void *__asan_get_report_address(void);







int __asan_get_report_access_type(void);







size_t __asan_get_report_access_size(void);







const char *__asan_get_report_description(void);




















const char *__asan_locate_address(void *addr, char *name, size_t name_size,
void **region_address, size_t *region_size);













size_t __asan_get_alloc_stack(void *addr, void **trace, size_t size,
int *thread_id);













size_t __asan_get_free_stack(void *addr, void **trace, size_t size,
int *thread_id);






void __asan_get_shadow_mapping(size_t *shadow_scale, size_t *shadow_offset);











void __asan_report_error(void *pc, void *bp, void *sp,
void *addr, int is_write, size_t access_size);


void __asan_set_death_callback(void (*callback)(void));






void __asan_set_error_report_callback(void (*callback)(const char *));






void __asan_on_error(void);



void __asan_print_accumulated_stats(void);








const char* __asan_default_options(void);











void *__asan_get_current_fake_stack(void);



















void *__asan_addr_is_in_fake_stack(void *fake_stack, void *addr, void **beg,
void **end);






void __asan_handle_no_return(void);



int __asan_update_allocation_context(void* addr);

#if defined(__cplusplus)
}
#endif

#endif
