















#if !defined(MEMPROF_INTERFACE_INTERNAL_H)
#define MEMPROF_INTERFACE_INTERNAL_H

#include "sanitizer_common/sanitizer_internal_defs.h"

#include "memprof_init_version.h"

using __sanitizer::u32;
using __sanitizer::u64;
using __sanitizer::uptr;

extern "C" {


SANITIZER_INTERFACE_ATTRIBUTE void __memprof_init();
SANITIZER_INTERFACE_ATTRIBUTE void __memprof_preinit();
SANITIZER_INTERFACE_ATTRIBUTE void __memprof_version_mismatch_check_v1();

SANITIZER_INTERFACE_ATTRIBUTE
void __memprof_record_access(void const volatile *addr);

SANITIZER_INTERFACE_ATTRIBUTE
void __memprof_record_access_range(void const volatile *addr, uptr size);

SANITIZER_INTERFACE_ATTRIBUTE void __memprof_print_accumulated_stats();

SANITIZER_INTERFACE_ATTRIBUTE
const char *__memprof_default_options();

SANITIZER_INTERFACE_ATTRIBUTE
extern uptr __memprof_shadow_memory_dynamic_address;

SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE extern char
__memprof_profile_filename[1];
SANITIZER_INTERFACE_ATTRIBUTE int __memprof_profile_dump();

SANITIZER_INTERFACE_ATTRIBUTE void __memprof_load(uptr p);
SANITIZER_INTERFACE_ATTRIBUTE void __memprof_store(uptr p);

SANITIZER_INTERFACE_ATTRIBUTE
void *__memprof_memcpy(void *dst, const void *src, uptr size);
SANITIZER_INTERFACE_ATTRIBUTE
void *__memprof_memset(void *s, int c, uptr n);
SANITIZER_INTERFACE_ATTRIBUTE
void *__memprof_memmove(void *dest, const void *src, uptr n);
}

#endif
