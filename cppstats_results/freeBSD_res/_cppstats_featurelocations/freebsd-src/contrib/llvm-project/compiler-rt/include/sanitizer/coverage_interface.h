










#if !defined(SANITIZER_COVERAG_INTERFACE_H)
#define SANITIZER_COVERAG_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif


void __sanitizer_cov_dump(void);


void __sanitizer_cov_reset(void);



void __sanitizer_dump_coverage(const uintptr_t *pcs, uintptr_t len);

#if defined(__cplusplus)
}
#endif

#endif
