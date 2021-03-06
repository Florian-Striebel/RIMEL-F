
























#include "atf-c/detail/user.h"

#include <sys/param.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>

#include "atf-c/detail/sanity.h"





uid_t
atf_user_euid(void)
{
return geteuid();
}

bool
atf_user_is_member_of_group(gid_t gid)
{
static gid_t groups[NGROUPS_MAX];
static int ngroups = -1;
bool found;
int i;

if (ngroups == -1) {
ngroups = getgroups(NGROUPS_MAX, groups);
INV(ngroups >= 0);
}

found = false;
for (i = 0; !found && i < ngroups; i++)
if (groups[i] == gid)
found = true;
return found;
}

bool
atf_user_is_root(void)
{
return geteuid() == 0;
}

bool
atf_user_is_unprivileged(void)
{
return geteuid() != 0;
}
