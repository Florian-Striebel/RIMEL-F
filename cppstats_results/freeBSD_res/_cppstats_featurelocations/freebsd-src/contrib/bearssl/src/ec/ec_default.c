























#include "inner.h"


const br_ec_impl *
br_ec_get_default(void)
{
#if BR_LOMUL
return &br_ec_all_m15;
#else
return &br_ec_all_m31;
#endif
}
