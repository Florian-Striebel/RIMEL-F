















#if !defined(ASAN_INTERFACE_INTERNAL_H)
#define ASAN_INTERFACE_INTERNAL_H

#include "sanitizer_common/sanitizer_internal_defs.h"

#include "asan_init_version.h"

using __sanitizer::uptr;
using __sanitizer::u64;
using __sanitizer::u32;

extern "C" {


SANITIZER_INTERFACE_ATTRIBUTE void __asan_init();





SANITIZER_INTERFACE_ATTRIBUTE void __asan_version_mismatch_check();



struct __asan_global_source_location {
const char *filename;
int line_no;
int column_no;
};


struct __asan_global {
uptr beg;
uptr size;
uptr size_with_redzone;
const char *name;
const char *module_name;

uptr has_dynamic_init;
__asan_global_source_location *location;

uptr odr_indicator;
};




SANITIZER_INTERFACE_ATTRIBUTE
void __asan_register_image_globals(uptr *flag);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_unregister_image_globals(uptr *flag);

SANITIZER_INTERFACE_ATTRIBUTE
void __asan_register_elf_globals(uptr *flag, void *start, void *stop);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_unregister_elf_globals(uptr *flag, void *start, void *stop);



SANITIZER_INTERFACE_ATTRIBUTE
void __asan_register_globals(__asan_global *globals, uptr n);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_unregister_globals(__asan_global *globals, uptr n);



SANITIZER_INTERFACE_ATTRIBUTE
void __asan_before_dynamic_init(const char *module_name);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_after_dynamic_init();


SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_shadow_00(uptr addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_shadow_f1(uptr addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_shadow_f2(uptr addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_shadow_f3(uptr addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_shadow_f5(uptr addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_shadow_f8(uptr addr, uptr size);





SANITIZER_INTERFACE_ATTRIBUTE
void __asan_poison_stack_memory(uptr addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_unpoison_stack_memory(uptr addr, uptr size);



SANITIZER_INTERFACE_ATTRIBUTE void __asan_handle_no_return();

SANITIZER_INTERFACE_ATTRIBUTE
void __asan_poison_memory_region(void const volatile *addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_unpoison_memory_region(void const volatile *addr, uptr size);

SANITIZER_INTERFACE_ATTRIBUTE
int __asan_address_is_poisoned(void const volatile *addr);

SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_region_is_poisoned(uptr beg, uptr size);

SANITIZER_INTERFACE_ATTRIBUTE
void __asan_describe_address(uptr addr);

SANITIZER_INTERFACE_ATTRIBUTE
int __asan_report_present();

SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_get_report_pc();
SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_get_report_bp();
SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_get_report_sp();
SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_get_report_address();
SANITIZER_INTERFACE_ATTRIBUTE
int __asan_get_report_access_type();
SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_get_report_access_size();
SANITIZER_INTERFACE_ATTRIBUTE
const char * __asan_get_report_description();

SANITIZER_INTERFACE_ATTRIBUTE
const char * __asan_locate_address(uptr addr, char *name, uptr name_size,
uptr *region_address, uptr *region_size);

SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_get_alloc_stack(uptr addr, uptr *trace, uptr size,
u32 *thread_id);

SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_get_free_stack(uptr addr, uptr *trace, uptr size,
u32 *thread_id);

SANITIZER_INTERFACE_ATTRIBUTE
void __asan_get_shadow_mapping(uptr *shadow_scale, uptr *shadow_offset);

SANITIZER_INTERFACE_ATTRIBUTE
void __asan_report_error(uptr pc, uptr bp, uptr sp,
uptr addr, int is_write, uptr access_size, u32 exp);

SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_death_callback(void (*callback)(void));
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_set_error_report_callback(void (*callback)(const char*));

SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __asan_on_error();

SANITIZER_INTERFACE_ATTRIBUTE void __asan_print_accumulated_stats();

SANITIZER_INTERFACE_ATTRIBUTE
const char *__asan_default_options();

SANITIZER_INTERFACE_ATTRIBUTE
extern uptr __asan_shadow_memory_dynamic_address;


SANITIZER_INTERFACE_ATTRIBUTE
extern int __asan_option_detect_stack_use_after_return;

SANITIZER_INTERFACE_ATTRIBUTE
extern uptr *__asan_test_only_reported_buggy_pointer;

SANITIZER_INTERFACE_ATTRIBUTE void __asan_load1(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load2(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load4(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load8(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load16(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store1(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store2(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store4(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store8(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store16(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_loadN(uptr p, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_storeN(uptr p, uptr size);

SANITIZER_INTERFACE_ATTRIBUTE void __asan_load1_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load2_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load4_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load8_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_load16_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store1_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store2_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store4_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store8_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_store16_noabort(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_loadN_noabort(uptr p, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_storeN_noabort(uptr p, uptr size);

SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_load1(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_load2(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_load4(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_load8(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_load16(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_store1(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_store2(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_store4(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_store8(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_store16(uptr p, u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_loadN(uptr p, uptr size,
u32 exp);
SANITIZER_INTERFACE_ATTRIBUTE void __asan_exp_storeN(uptr p, uptr size,
u32 exp);

SANITIZER_INTERFACE_ATTRIBUTE
void* __asan_memcpy(void *dst, const void *src, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void* __asan_memset(void *s, int c, uptr n);
SANITIZER_INTERFACE_ATTRIBUTE
void* __asan_memmove(void* dest, const void* src, uptr n);

SANITIZER_INTERFACE_ATTRIBUTE
void __asan_poison_cxx_array_cookie(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE
uptr __asan_load_cxx_array_cookie(uptr *p);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_poison_intra_object_redzone(uptr p, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_unpoison_intra_object_redzone(uptr p, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_alloca_poison(uptr addr, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void __asan_allocas_unpoison(uptr top, uptr bottom);

SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
const char* __asan_default_suppressions();

SANITIZER_INTERFACE_ATTRIBUTE void __asan_handle_vfork(void *sp);

SANITIZER_INTERFACE_ATTRIBUTE int __asan_update_allocation_context(
void *addr);
}

#endif
