










#if !defined(SANITIZER_SCUDO_INTERFACE_H_)
#define SANITIZER_SCUDO_INTERFACE_H_

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif


const char* __scudo_default_options(void);





void __scudo_set_rss_limit(size_t LimitMb, int HardLimit);




void __scudo_print_stats(void);
#if defined(__cplusplus)
}
#endif

#endif
