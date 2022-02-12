
























#if !defined(ATF_C_DETAIL_ENV_H)
#define ATF_C_DETAIL_ENV_H

#include <stdbool.h>

#include <atf-c/error_fwd.h>

const char *atf_env_get(const char *);
const char *atf_env_get_with_default(const char *, const char *);
bool atf_env_has(const char *);
atf_error_t atf_env_set(const char *, const char *);
atf_error_t atf_env_unset(const char *);

#endif
