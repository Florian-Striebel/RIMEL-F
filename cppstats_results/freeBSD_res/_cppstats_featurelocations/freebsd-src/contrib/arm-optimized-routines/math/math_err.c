






#include "math_config.h"

#if WANT_ERRNO
#include <errno.h>


NOINLINE static double
with_errno (double y, int e)
{
errno = e;
return y;
}
#else
#define with_errno(x, e) (x)
#endif


NOINLINE static double
xflow (uint32_t sign, double y)
{
y = eval_as_double (opt_barrier_double (sign ? -y : y) * y);
return with_errno (y, ERANGE);
}

HIDDEN double
__math_uflow (uint32_t sign)
{
return xflow (sign, 0x1p-767);
}

#if WANT_ERRNO_UFLOW


HIDDEN double
__math_may_uflow (uint32_t sign)
{
return xflow (sign, 0x1.8p-538);
}
#endif

HIDDEN double
__math_oflow (uint32_t sign)
{
return xflow (sign, 0x1p769);
}

HIDDEN double
__math_divzero (uint32_t sign)
{
double y = opt_barrier_double (sign ? -1.0 : 1.0) / 0.0;
return with_errno (y, ERANGE);
}

HIDDEN double
__math_invalid (double x)
{
double y = (x - x) / (x - x);
return isnan (x) ? y : with_errno (y, EDOM);
}



HIDDEN double
__math_check_uflow (double y)
{
return y == 0.0 ? with_errno (y, ERANGE) : y;
}

HIDDEN double
__math_check_oflow (double y)
{
return isinf (y) ? with_errno (y, ERANGE) : y;
}
