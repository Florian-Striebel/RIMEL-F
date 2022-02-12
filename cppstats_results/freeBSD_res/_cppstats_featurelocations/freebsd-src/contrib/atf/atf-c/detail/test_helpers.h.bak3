
























#if defined(ATF_C_DETAIL_TEST_HELPERS_H)
#error "Cannot include test_helpers.h more than once."
#else
#define ATF_C_DETAIL_TEST_HELPERS_H
#endif

#include <stdbool.h>

#include <atf-c.h>

#include <atf-c/detail/env.h>
#include <atf-c/error_fwd.h>
#include <atf-c/tc.h>

struct atf_dynstr;
struct atf_fs_path;

#define CE(stm) ATF_CHECK(!atf_is_error(stm))
#define RE(stm) ATF_REQUIRE(!atf_is_error(stm))

#define HEADER_TC(name, hdrname) ATF_TC(name); ATF_TC_HEAD(name, tc) { const char *cc; atf_tc_set_md_var(tc, "descr", "Tests that the " hdrname " file can " "be included on its own, without any prerequisites"); cc = atf_env_get_with_default("ATF_BUILD_CC", ATF_BUILD_CC); atf_tc_set_md_var(tc, "require.progs", cc); } ATF_TC_BODY(name, tc) { header_check(hdrname); }














#define BUILD_TC(name, sfile, descr, failmsg) ATF_TC(name); ATF_TC_HEAD(name, tc) { const char *cc; atf_tc_set_md_var(tc, "descr", descr); cc = atf_env_get_with_default("ATF_BUILD_CC", ATF_BUILD_CC); atf_tc_set_md_var(tc, "require.progs", cc); } ATF_TC_BODY(name, tc) { if (!build_check_c_o_srcdir(tc, sfile)) atf_tc_fail("%s", failmsg); }














bool build_check_c_o(const char *);
bool build_check_c_o_srcdir(const atf_tc_t *, const char *);
void header_check(const char *);
void get_process_helpers_path(const atf_tc_t *, const bool,
struct atf_fs_path *);
bool read_line(int, struct atf_dynstr *);
void run_h_tc(atf_tc_t *, const char *, const char *, const char *);
