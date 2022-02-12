











#if !defined(MSAN_INTERFACE_H)
#define MSAN_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif

void __msan_set_origin(const volatile void *a, size_t size, uint32_t origin);


uint32_t __msan_get_origin(const volatile void *a);




int __msan_origin_is_descendant_or_same(uint32_t this_id, uint32_t prev_id);


int __msan_get_track_origins(void);


uint32_t __msan_get_umr_origin(void);


void __msan_unpoison(const volatile void *a, size_t size);



void __msan_unpoison_string(const volatile char *a);


void __msan_unpoison_param(size_t n);




void __msan_poison(const volatile void *a, size_t size);



void __msan_partial_poison(const volatile void *data, void *shadow,
size_t size);



intptr_t __msan_test_shadow(const volatile void *x, size_t size);



void __msan_check_mem_is_initialized(const volatile void *x, size_t size);






void __msan_set_expect_umr(int expect_umr);




void __msan_set_keep_going(int keep_going);



void __msan_print_shadow(const volatile void *x, size_t size);



void __msan_dump_shadow(const volatile void *x, size_t size);


int __msan_has_dynamic_component(void);



void __msan_allocated_memory(const volatile void* data, size_t size);


void __sanitizer_dtor_callback(const volatile void* data, size_t size);



const char* __msan_default_options(void);


void __msan_set_death_callback(void (*callback)(void));





void __msan_copy_shadow(const volatile void *dst, const volatile void *src,
size_t size);


void __msan_scoped_disable_interceptor_checks(void);



void __msan_scoped_enable_interceptor_checks(void);

void __msan_start_switch_fiber(const void *bottom, size_t size);
void __msan_finish_switch_fiber(const void **bottom_old, size_t *size_old);

#if defined(__cplusplus)
}
#endif

#endif
