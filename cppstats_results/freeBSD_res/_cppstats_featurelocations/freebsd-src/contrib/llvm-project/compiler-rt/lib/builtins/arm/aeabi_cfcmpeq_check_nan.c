







#include "../int_lib.h"
#include <stdint.h>

AEABI_RTABI __attribute__((visibility("hidden"))) int
__aeabi_cfcmpeq_check_nan(float a, float b) {
return __builtin_isnan(a) || __builtin_isnan(b);
}
