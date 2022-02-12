
























#include <atf-c.h>

#include "atf-c/detail/test_helpers.h"





HEADER_TC(include, "atf-c.h");





ATF_TP_ADD_TCS(tp)
{

ATF_TP_ADD_TC(tp, include);

return atf_no_error();
}
