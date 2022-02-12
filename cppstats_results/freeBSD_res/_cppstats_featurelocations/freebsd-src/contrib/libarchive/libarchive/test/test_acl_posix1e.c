























#include "test.h"
__FBSDID("$FreeBSD: src/lib/libarchive/test/test_acl_basic.c,v 1.6 2008/10/19 00:13:57 kientzle Exp $");










static struct archive_test_acl_t acls0[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_WRITE,
ARCHIVE_ENTRY_ACL_OTHER, 0, "" },
};

static struct archive_test_acl_t acls1[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 77, "user77" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_WRITE,
ARCHIVE_ENTRY_ACL_OTHER, -1, "" },
};

static struct archive_test_acl_t acls2[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_EXECUTE | ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 77, "user77" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0,
ARCHIVE_ENTRY_ACL_USER, 78, "user78" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0007,
ARCHIVE_ENTRY_ACL_GROUP, 78, "group78" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_WRITE | ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_OTHER, -1, "" },
};





static struct archive_test_acl_t acls_nfs4[] = {

{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 78, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DENY, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 78, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_AUDIT, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 78, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALARM, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 78, "" },


{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_EVERYONE, -1, "" },


{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_READ | ARCHIVE_ENTRY_ACL_ENTRY_FILE_INHERIT,
ARCHIVE_ENTRY_ACL_USER, 79, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_READ | ARCHIVE_ENTRY_ACL_ENTRY_FILE_INHERIT,
ARCHIVE_ENTRY_ACL_USER_OBJ, -1, "" },
};

DEFINE_TEST(test_acl_posix1e)
{
struct archive_entry *ae;
int i;


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0777);










assertEntrySetAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]));
failure("Basic ACLs shouldn't be stored as extended ACLs");
assert(0 == archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
failure("Basic ACLs should set mode to 0142, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0142);


assertEntrySetAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]));
failure("One extended ACL should flag all ACLs to be returned.");


assert((archive_entry_acl_types(ae) &
ARCHIVE_ENTRY_ACL_TYPE_NFS4) == 0);
assert((archive_entry_acl_types(ae) &
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E) != 0);

assert(4 == archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
assertEntryCompareAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]),
ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0142);
failure("Basic ACLs should set mode to 0142, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0142);



assertEntrySetAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]));
assertEqualInt(6, archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
assertEntryCompareAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]),
ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0543);
failure("Basic ACLs should set mode to 0543, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0543);





assertEntrySetAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]));
failure("Basic ACLs shouldn't be stored as extended ACLs");
assert(0 == archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
failure("Basic ACLs should set mode to 0142, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0142);





assertEntrySetAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]));
for (i = 0; i < (int)(sizeof(acls_nfs4)/sizeof(acls_nfs4[0])); ++i) {
struct archive_test_acl_t *p = &acls_nfs4[i];
failure("Malformed ACL test #%d", i);
assertEqualInt(ARCHIVE_FAILED,
archive_entry_acl_add_entry(ae,
p->type, p->permset, p->tag, p->qual, p->name));
assertEqualInt(6,
archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
}
archive_entry_free(ae);
}
