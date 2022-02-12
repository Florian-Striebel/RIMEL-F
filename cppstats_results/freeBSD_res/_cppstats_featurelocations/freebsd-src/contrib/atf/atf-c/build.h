
























#if !defined(ATF_C_BUILD_H)
#define ATF_C_BUILD_H

#include <atf-c/error_fwd.h>

atf_error_t atf_build_c_o(const char *, const char *, const char *const [],
char ***);
atf_error_t atf_build_cpp(const char *, const char *, const char *const [],
char ***);
atf_error_t atf_build_cxx_o(const char *, const char *, const char *const [],
char ***);

#endif
