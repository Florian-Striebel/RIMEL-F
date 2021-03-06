











#if !defined(SANITIZER_LSAN_INTERFACE_H)
#define SANITIZER_LSAN_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif


void __lsan_disable(void);
void __lsan_enable(void);


void __lsan_ignore_object(const void *p);












void __lsan_register_root_region(const void *p, size_t size);
void __lsan_unregister_root_region(const void *p, size_t size);









void __lsan_do_leak_check(void);








int __lsan_do_recoverable_leak_check(void);







int __lsan_is_turned_off(void);



const char *__lsan_default_options(void);



const char *__lsan_default_suppressions(void);
#if defined(__cplusplus)
}

namespace __lsan {
class ScopedDisabler {
public:
ScopedDisabler() { __lsan_disable(); }
~ScopedDisabler() { __lsan_enable(); }
};
}
#endif

#endif
