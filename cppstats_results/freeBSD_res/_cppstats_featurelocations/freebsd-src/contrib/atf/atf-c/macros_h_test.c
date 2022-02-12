
























#include <atf-c/macros.h>

void atf_require_inside_if(void);
void atf_require_equal_inside_if(void);
void atf_check_errno_semicolons(void);
void atf_require_errno_semicolons(void);

void
atf_require_inside_if(void)
{




if (true)
ATF_REQUIRE(true);
else
ATF_REQUIRE(true);
}

void
atf_require_equal_inside_if(void)
{




if (true)
ATF_REQUIRE_EQ(true, true);
else
ATF_REQUIRE_EQ(true, true);
}

void
atf_check_errno_semicolons(void)
{


ATF_CHECK_ERRNO(1, 1 == 1);
ATF_CHECK_ERRNO(2, 2 == 2);
}

void
atf_require_errno_semicolons(void)
{


ATF_REQUIRE_ERRNO(1, 1 == 1);
ATF_REQUIRE_ERRNO(2, 2 == 2);
}



#define TEST_MACRO_1 invalid + name
#define TEST_MACRO_2 invalid + name
#define TEST_MACRO_3 invalid + name
ATF_TC(TEST_MACRO_1);
ATF_TC_HEAD(TEST_MACRO_1, tc) { if (tc != NULL) {} }
ATF_TC_BODY(TEST_MACRO_1, tc) { if (tc != NULL) {} }
atf_tc_t *test_name_1 = &ATF_TC_NAME(TEST_MACRO_1);
atf_tc_pack_t *test_pack_1 = &ATF_TC_PACK_NAME(TEST_MACRO_1);
void (*head_1)(atf_tc_t *) = ATF_TC_HEAD_NAME(TEST_MACRO_1);
void (*body_1)(const atf_tc_t *) = ATF_TC_BODY_NAME(TEST_MACRO_1);
ATF_TC_WITH_CLEANUP(TEST_MACRO_2);
ATF_TC_HEAD(TEST_MACRO_2, tc) { if (tc != NULL) {} }
ATF_TC_BODY(TEST_MACRO_2, tc) { if (tc != NULL) {} }
ATF_TC_CLEANUP(TEST_MACRO_2, tc) { if (tc != NULL) {} }
atf_tc_t *test_name_2 = &ATF_TC_NAME(TEST_MACRO_2);
atf_tc_pack_t *test_pack_2 = &ATF_TC_PACK_NAME(TEST_MACRO_2);
void (*head_2)(atf_tc_t *) = ATF_TC_HEAD_NAME(TEST_MACRO_2);
void (*body_2)(const atf_tc_t *) = ATF_TC_BODY_NAME(TEST_MACRO_2);
void (*cleanup_2)(const atf_tc_t *) = ATF_TC_CLEANUP_NAME(TEST_MACRO_2);
ATF_TC_WITHOUT_HEAD(TEST_MACRO_3);
ATF_TC_BODY(TEST_MACRO_3, tc) { if (tc != NULL) {} }
atf_tc_t *test_name_3 = &ATF_TC_NAME(TEST_MACRO_3);
atf_tc_pack_t *test_pack_3 = &ATF_TC_PACK_NAME(TEST_MACRO_3);
void (*body_3)(const atf_tc_t *) = ATF_TC_BODY_NAME(TEST_MACRO_3);
