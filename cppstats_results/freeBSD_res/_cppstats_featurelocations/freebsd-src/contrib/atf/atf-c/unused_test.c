
























#include <atf-c.h>

ATF_TC(this_is_used);
ATF_TC_HEAD(this_is_used, tc)
{
atf_tc_set_md_var(tc, "descr", "A test case that is not referenced");
}
ATF_TC_BODY(this_is_used, tc)
{
}

ATF_TC(this_is_unused);
ATF_TC_HEAD(this_is_unused, tc)
{
atf_tc_set_md_var(tc, "descr", "A test case that is referenced");
}
ATF_TC_BODY(this_is_unused, tc)
{
}

ATF_TP_ADD_TCS(tp)
{
ATF_TP_ADD_TC(tp, this_is_used);


return atf_no_error();
}
