







#define SINGLE_PRECISION
#include "../fp_lib.h"

AEABI_RTABI fp_t __aeabi_fsub(fp_t, fp_t);

AEABI_RTABI fp_t __aeabi_frsub(fp_t a, fp_t b) { return __aeabi_fsub(b, a); }
