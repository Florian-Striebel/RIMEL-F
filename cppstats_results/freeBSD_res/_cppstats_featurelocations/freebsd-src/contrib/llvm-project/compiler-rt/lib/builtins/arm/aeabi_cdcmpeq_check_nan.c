







#include "../int_lib.h"
#include <stdint.h>

AEABI_RTABI __attribute__((visibility("hidden"))) int
__aeabi_cdcmpeq_check_nan(double a, double b) {
return __builtin_isnan(a) || __builtin_isnan(b);
}
