























#include "test.h"
__FBSDID("$FreeBSD$");









static struct archive_test_acl_t acls1[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DENY, ARCHIVE_ENTRY_ACL_READ_DATA,
ARCHIVE_ENTRY_ACL_USER, 77, "user77" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_READ_DATA,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DENY, ARCHIVE_ENTRY_ACL_WRITE_DATA,
ARCHIVE_ENTRY_ACL_EVERYONE, -1, "" },
};

static struct archive_test_acl_t acls2[] = {

{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, 0,
ARCHIVE_ENTRY_ACL_USER, 108, "user108" },
{ ARCHIVE_ENTRY_ACL_TYPE_DENY, 0,
ARCHIVE_ENTRY_ACL_USER, 109, "user109" },
{ ARCHIVE_ENTRY_ACL_TYPE_AUDIT, 0,
ARCHIVE_ENTRY_ACL_USER, 110, "user110" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALARM, 0,
ARCHIVE_ENTRY_ACL_USER, 111, "user111" },


{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER, 112, "user112" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_READ_DATA,
ARCHIVE_ENTRY_ACL_USER, 113, "user113" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_LIST_DIRECTORY,
ARCHIVE_ENTRY_ACL_USER, 114, "user114" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_WRITE_DATA,
ARCHIVE_ENTRY_ACL_USER, 115, "user115" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_ADD_FILE,
ARCHIVE_ENTRY_ACL_USER, 116, "user116" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_APPEND_DATA,
ARCHIVE_ENTRY_ACL_USER, 117, "user117" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_ADD_SUBDIRECTORY,
ARCHIVE_ENTRY_ACL_USER, 118, "user118" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS,
ARCHIVE_ENTRY_ACL_USER, 119, "user119" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_WRITE_NAMED_ATTRS,
ARCHIVE_ENTRY_ACL_USER, 120, "user120" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_DELETE_CHILD,
ARCHIVE_ENTRY_ACL_USER, 121, "user121" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES,
ARCHIVE_ENTRY_ACL_USER, 122, "user122" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_WRITE_ATTRIBUTES,
ARCHIVE_ENTRY_ACL_USER, 123, "user123" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_DELETE,
ARCHIVE_ENTRY_ACL_USER, 124, "user124" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_READ_ACL,
ARCHIVE_ENTRY_ACL_USER, 125, "user125" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_WRITE_ACL,
ARCHIVE_ENTRY_ACL_USER, 126, "user126" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_WRITE_OWNER,
ARCHIVE_ENTRY_ACL_USER, 127, "user127" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_USER, 128, "user128" },


{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA | ARCHIVE_ENTRY_ACL_ENTRY_FILE_INHERIT,
ARCHIVE_ENTRY_ACL_USER, 129, "user129" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA | ARCHIVE_ENTRY_ACL_ENTRY_DIRECTORY_INHERIT,
ARCHIVE_ENTRY_ACL_USER, 130, "user130" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA | ARCHIVE_ENTRY_ACL_ENTRY_NO_PROPAGATE_INHERIT,
ARCHIVE_ENTRY_ACL_USER, 131, "user131" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA | ARCHIVE_ENTRY_ACL_ENTRY_INHERIT_ONLY,
ARCHIVE_ENTRY_ACL_USER, 132, "user132" },
{ ARCHIVE_ENTRY_ACL_TYPE_AUDIT,
ARCHIVE_ENTRY_ACL_READ_DATA | ARCHIVE_ENTRY_ACL_ENTRY_SUCCESSFUL_ACCESS,
ARCHIVE_ENTRY_ACL_USER, 133, "user133" },
{ ARCHIVE_ENTRY_ACL_TYPE_AUDIT,
ARCHIVE_ENTRY_ACL_READ_DATA | ARCHIVE_ENTRY_ACL_ENTRY_FAILED_ACCESS,
ARCHIVE_ENTRY_ACL_USER, 134, "user134" },


{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER, 135, "user135" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_GROUP, 136, "group136" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_EVERYONE, -1, "" },
};





static struct archive_test_acl_t acls_bad[] = {

{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER, 78, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DEFAULT, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER, 78, "" },


{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_OTHER, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_MASK, -1, "" },


{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_EVERYONE, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW, ARCHIVE_ENTRY_ACL_WRITE,
ARCHIVE_ENTRY_ACL_EVERYONE, -1, "" },
};

DEFINE_TEST(test_acl_nfs4)
{
struct archive_entry *ae;
int i;


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0777);


assertEntrySetAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]));


assert((archive_entry_acl_types(ae) &
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E) == 0);
assert((archive_entry_acl_types(ae) &
ARCHIVE_ENTRY_ACL_TYPE_NFS4) != 0);

assertEqualInt(4,
archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_NFS4));
assertEntryCompareAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]),
ARCHIVE_ENTRY_ACL_TYPE_NFS4, 0);


assertEntrySetAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]));
assertEqualInt(32,
archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_NFS4));
assertEntryCompareAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]),
ARCHIVE_ENTRY_ACL_TYPE_NFS4, 0);





assertEntrySetAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]));
failure("Basic ACLs shouldn't be stored as extended ACLs");
assertEqualInt(4,
archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_NFS4));





assertEntrySetAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]));
for (i = 0; i < (int)(sizeof(acls_bad)/sizeof(acls_bad[0])); ++i) {
struct archive_test_acl_t *p = &acls_bad[i];
failure("Malformed ACL test #%d", i);
assertEqualInt(ARCHIVE_FAILED,
archive_entry_acl_add_entry(ae,
p->type, p->permset, p->tag, p->qual, p->name));
failure("Malformed ACL test #%d", i);
assertEqualInt(32,
archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_NFS4));
}
archive_entry_free(ae);
}
