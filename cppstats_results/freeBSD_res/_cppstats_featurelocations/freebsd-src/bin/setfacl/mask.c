

























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/acl.h>
#include <sys/stat.h>

#include <err.h>

#include "setfacl.h"


int
set_acl_mask(acl_t *prev_acl, const char *filename)
{
acl_entry_t entry;
acl_t acl;
acl_tag_t tag;
int entry_id;

entry = NULL;






if (have_mask)
return (0);

acl = acl_dup(*prev_acl);
if (acl == NULL)
err(1, "%s: acl_dup() failed", filename);

if (!n_flag) {







if (acl_calc_mask(&acl)) {
warn("%s: acl_calc_mask() failed", filename);
acl_free(acl);
return (-1);
}
} else {






entry_id = ACL_FIRST_ENTRY;

while (acl_get_entry(acl, entry_id, &entry) == 1) {
entry_id = ACL_NEXT_ENTRY;
if (acl_get_tag_type(entry, &tag) == -1)
err(1, "%s: acl_get_tag_type() failed",
filename);

if (tag == ACL_MASK) {
acl_free(acl);
return (0);
}
}







warnx("%s: warning: no mask entry", filename);
acl_free(acl);
return (0);
}

acl_free(*prev_acl);
*prev_acl = acl_dup(acl);
acl_free(acl);

return (0);
}
