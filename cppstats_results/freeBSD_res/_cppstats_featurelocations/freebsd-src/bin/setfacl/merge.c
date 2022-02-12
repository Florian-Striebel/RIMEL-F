

























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/acl.h>
#include <sys/stat.h>

#include <err.h>

#include "setfacl.h"

static int merge_user_group(acl_entry_t *entry, acl_entry_t *entry_new,
int acl_brand);

static int
merge_user_group(acl_entry_t *entry, acl_entry_t *entry_new, int acl_brand)
{
acl_permset_t permset;
acl_entry_type_t entry_type;
acl_flagset_t flagset;
int have_entry;
uid_t *id, *id_new;

have_entry = 0;

id = acl_get_qualifier(*entry);
if (id == NULL)
err(1, "acl_get_qualifier() failed");
id_new = acl_get_qualifier(*entry_new);
if (id_new == NULL)
err(1, "acl_get_qualifier() failed");
if (*id == *id_new) {

if (acl_get_permset(*entry, &permset) == -1)
err(1, "acl_get_permset() failed");
if (acl_set_permset(*entry_new, permset) == -1)
err(1, "acl_set_permset() failed");

if (acl_brand == ACL_BRAND_NFS4) {
if (acl_get_entry_type_np(*entry, &entry_type))
err(1, "acl_get_entry_type_np() failed");
if (acl_set_entry_type_np(*entry_new, entry_type))
err(1, "acl_set_entry_type_np() failed");
if (acl_get_flagset_np(*entry, &flagset))
err(1, "acl_get_flagset_np() failed");
if (acl_set_flagset_np(*entry_new, flagset))
err(1, "acl_set_flagset_np() failed");
}

have_entry = 1;
}
acl_free(id);
acl_free(id_new);

return (have_entry);
}




int
merge_acl(acl_t acl, acl_t *prev_acl, const char *filename)
{
acl_entry_t entry, entry_new;
acl_permset_t permset;
acl_t acl_new;
acl_tag_t tag, tag_new;
acl_entry_type_t entry_type, entry_type_new;
acl_flagset_t flagset;
int entry_id, entry_id_new, have_entry, had_entry, entry_number = 0;
int acl_brand, prev_acl_brand;

acl_get_brand_np(acl, &acl_brand);
acl_get_brand_np(*prev_acl, &prev_acl_brand);

if (branding_mismatch(acl_brand, prev_acl_brand)) {
warnx("%s: branding mismatch; existing ACL is %s, "
"entry to be merged is %s", filename,
brand_name(prev_acl_brand), brand_name(acl_brand));
return (-1);
}

acl_new = acl_dup(*prev_acl);
if (acl_new == NULL)
err(1, "%s: acl_dup() failed", filename);

entry_id = ACL_FIRST_ENTRY;

while (acl_get_entry(acl, entry_id, &entry) == 1) {
entry_id = ACL_NEXT_ENTRY;
have_entry = 0;
had_entry = 0;


if (acl_get_tag_type(entry, &tag) == -1)
err(1, "%s: acl_get_tag_type() failed - "
"invalid ACL entry", filename);
if (tag == ACL_MASK)
have_mask = true;


entry_id_new = ACL_FIRST_ENTRY;
while (acl_get_entry(acl_new, entry_id_new, &entry_new) == 1) {
entry_id_new = ACL_NEXT_ENTRY;

if (acl_get_tag_type(entry, &tag) == -1)
err(1, "%s: acl_get_tag_type() failed",
filename);
if (acl_get_tag_type(entry_new, &tag_new) == -1)
err(1, "%s: acl_get_tag_type() failed",
filename);
if (tag != tag_new)
continue;





if (acl_brand == ACL_BRAND_NFS4) {
if (acl_get_entry_type_np(entry, &entry_type))
err(1, "%s: acl_get_entry_type_np() "
"failed", filename);
if (acl_get_entry_type_np(entry_new, &entry_type_new))
err(1, "%s: acl_get_entry_type_np() "
"failed", filename);
if (entry_type != entry_type_new)
continue;
}

switch(tag) {
case ACL_USER:
case ACL_GROUP:
have_entry = merge_user_group(&entry,
&entry_new, acl_brand);
if (have_entry == 0)
break;

case ACL_USER_OBJ:
case ACL_GROUP_OBJ:
case ACL_OTHER:
case ACL_MASK:
case ACL_EVERYONE:
if (acl_get_permset(entry, &permset) == -1)
err(1, "%s: acl_get_permset() failed",
filename);
if (acl_set_permset(entry_new, permset) == -1)
err(1, "%s: acl_set_permset() failed",
filename);

if (acl_brand == ACL_BRAND_NFS4) {
if (acl_get_entry_type_np(entry, &entry_type))
err(1, "%s: acl_get_entry_type_np() failed",
filename);
if (acl_set_entry_type_np(entry_new, entry_type))
err(1, "%s: acl_set_entry_type_np() failed",
filename);
if (acl_get_flagset_np(entry, &flagset))
err(1, "%s: acl_get_flagset_np() failed",
filename);
if (acl_set_flagset_np(entry_new, flagset))
err(1, "%s: acl_set_flagset_np() failed",
filename);
}
had_entry = have_entry = 1;
break;
default:

errx(1, "%s: invalid tag type: %i", filename, tag);
break;
}
}


if (had_entry == 0) {






if (acl_brand == ACL_BRAND_NFS4) {
if (acl_create_entry_np(&acl_new, &entry_new, entry_number) == -1) {
warn("%s: acl_create_entry_np() failed", filename);
acl_free(acl_new);
return (-1);
}






entry_number++;
} else {
if (acl_create_entry(&acl_new, &entry_new) == -1) {
warn("%s: acl_create_entry() failed", filename);
acl_free(acl_new);
return (-1);
}
}
if (acl_copy_entry(entry_new, entry) == -1)
err(1, "%s: acl_copy_entry() failed", filename);
}
}

acl_free(*prev_acl);
*prev_acl = acl_new;

return (0);
}

int
add_acl(acl_t acl, uint entry_number, acl_t *prev_acl, const char *filename)
{
acl_entry_t entry, entry_new;
acl_t acl_new;
int entry_id, acl_brand, prev_acl_brand;

acl_get_brand_np(acl, &acl_brand);
acl_get_brand_np(*prev_acl, &prev_acl_brand);

if (prev_acl_brand != ACL_BRAND_NFS4) {
warnx("%s: the '-a' option is only applicable to NFSv4 ACLs",
filename);
return (-1);
}

if (branding_mismatch(acl_brand, ACL_BRAND_NFS4)) {
warnx("%s: branding mismatch; existing ACL is NFSv4, "
"entry to be added is %s", filename,
brand_name(acl_brand));
return (-1);
}

acl_new = acl_dup(*prev_acl);
if (acl_new == NULL)
err(1, "%s: acl_dup() failed", filename);

entry_id = ACL_FIRST_ENTRY;

while (acl_get_entry(acl, entry_id, &entry) == 1) {
entry_id = ACL_NEXT_ENTRY;

if (acl_create_entry_np(&acl_new, &entry_new, entry_number) == -1) {
warn("%s: acl_create_entry_np() failed", filename);
acl_free(acl_new);
return (-1);
}







entry_number++;

if (acl_copy_entry(entry_new, entry) == -1)
err(1, "%s: acl_copy_entry() failed", filename);
}

acl_free(*prev_acl);
*prev_acl = acl_new;

return (0);
}
