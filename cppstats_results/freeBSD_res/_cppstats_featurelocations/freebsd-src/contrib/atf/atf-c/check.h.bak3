
























#if !defined(ATF_C_CHECK_H)
#define ATF_C_CHECK_H

#include <stdbool.h>

#include <atf-c/error_fwd.h>





struct atf_check_result_impl;
struct atf_check_result {
struct atf_check_result_impl *pimpl;
};
typedef struct atf_check_result atf_check_result_t;


void atf_check_result_fini(atf_check_result_t *);


const char *atf_check_result_stdout(const atf_check_result_t *);
const char *atf_check_result_stderr(const atf_check_result_t *);
bool atf_check_result_exited(const atf_check_result_t *);
int atf_check_result_exitcode(const atf_check_result_t *);
bool atf_check_result_signaled(const atf_check_result_t *);
int atf_check_result_termsig(const atf_check_result_t *);





atf_error_t atf_check_build_c_o(const char *, const char *,
const char *const [],
bool *);
atf_error_t atf_check_build_cpp(const char *, const char *,
const char *const [],
bool *);
atf_error_t atf_check_build_cxx_o(const char *, const char *,
const char *const [],
bool *);
atf_error_t atf_check_exec_array(const char *const *, atf_check_result_t *);

#endif
