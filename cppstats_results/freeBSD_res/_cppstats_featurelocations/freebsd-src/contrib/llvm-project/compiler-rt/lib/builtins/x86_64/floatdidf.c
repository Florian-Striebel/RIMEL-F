





#if defined(__x86_64__) || defined(_M_X64)

#include "../int_lib.h"

double __floatdidf(int64_t a) { return (double)a; }

#endif
