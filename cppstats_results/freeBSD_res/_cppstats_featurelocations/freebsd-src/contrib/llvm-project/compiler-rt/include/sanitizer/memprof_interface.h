











#if !defined(SANITIZER_MEMPROF_INTERFACE_H)
#define SANITIZER_MEMPROF_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif






void __memprof_record_access_range(void const volatile *addr, size_t size);






void __memprof_record_access(void const volatile *addr);






void __memprof_on_error(void);



void __memprof_print_accumulated_stats(void);








const char *__memprof_default_options(void);




int __memprof_profile_dump(void);

#if defined(__cplusplus)
}
#endif

#endif
