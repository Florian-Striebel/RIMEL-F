
























#if !defined(ATF_C_DETAIL_USER_H)
#define ATF_C_DETAIL_USER_H

#include <sys/types.h>

#include <stdbool.h>








uid_t atf_user_euid(void);
bool atf_user_is_member_of_group(gid_t);
bool atf_user_is_root(void);
bool atf_user_is_unprivileged(void);

#endif
