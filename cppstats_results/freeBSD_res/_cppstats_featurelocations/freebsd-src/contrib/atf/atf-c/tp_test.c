
























#include "atf-c/tp.h"

#include <string.h>
#include <unistd.h>

#include <atf-c.h>

ATF_TC(getopt);
ATF_TC_HEAD(getopt, tc)
{
atf_tc_set_md_var(tc, "descr", "Checks if getopt(3) global state is "
"reset by the test program driver so that test cases can use "
"getopt(3) again");
}
ATF_TC_BODY(getopt, tc)
{



int argc = 4;
char arg1[] = "progname";
char arg2[] = "-Z";
char arg3[] = "-s";
char arg4[] = "foo";
char *const argv[] = { arg1, arg2, arg3, arg4, NULL };

int ch;
bool zflag;




zflag = false;
while ((ch = getopt(argc, argv, ":Z")) != -1) {
switch (ch) {
case 'Z':
zflag = true;
break;

case '?':
default:
if (optopt != 's')
atf_tc_fail("Unexpected unknown option -%c found", optopt);
}
}

ATF_REQUIRE(zflag);
ATF_REQUIRE_EQ_MSG(1, argc - optind, "Invalid number of arguments left "
"after the call to getopt(3)");
ATF_CHECK_STREQ_MSG("foo", argv[optind], "The non-option argument is "
"invalid");
}





ATF_TP_ADD_TCS(tp)
{
ATF_TP_ADD_TC(tp, getopt);

return atf_no_error();
}
