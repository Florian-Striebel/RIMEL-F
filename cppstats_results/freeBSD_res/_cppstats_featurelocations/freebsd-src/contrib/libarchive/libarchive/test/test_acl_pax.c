
























#include "test.h"
__FBSDID("$FreeBSD$");









static unsigned char buff[16384];

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

static struct archive_test_acl_t acls3[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_WRITE_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_WRITE_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_WRITE_ACL |
ARCHIVE_ENTRY_ACL_WRITE_OWNER |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_USER_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_EVERYONE, 0, "" },
};

static struct archive_test_acl_t acls4[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_WRITE_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_WRITE_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_WRITE_ACL |
ARCHIVE_ENTRY_ACL_WRITE_OWNER |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_USER_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE |
ARCHIVE_ENTRY_ACL_ENTRY_INHERITED,
ARCHIVE_ENTRY_ACL_USER, 77, "user77" },
{ ARCHIVE_ENTRY_ACL_TYPE_DENY,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_USER, 78, "user78" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DENY,
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_WRITE_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_WRITE_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_WRITE_ACL |
ARCHIVE_ENTRY_ACL_WRITE_OWNER,
ARCHIVE_ENTRY_ACL_GROUP, 78, "group78" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_EVERYONE, 0, "" },
};

static struct archive_test_acl_t acls5[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_WRITE_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_WRITE_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_WRITE_ACL |
ARCHIVE_ENTRY_ACL_WRITE_OWNER |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_USER_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_WRITE_OWNER |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_USER, 77, "user77" },
{ ARCHIVE_ENTRY_ACL_TYPE_AUDIT,
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_ENTRY_SUCCESSFUL_ACCESS,
ARCHIVE_ENTRY_ACL_USER, 77, "user77" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALARM,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_ENTRY_FAILED_ACCESS,
ARCHIVE_ENTRY_ACL_GROUP, 78, "group78" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_EVERYONE, 0, "" },
};

DEFINE_TEST(test_acl_pax_posix1e)
{
struct archive *a;
struct archive_entry *ae;
size_t used;
FILE *f;
void *reference;
size_t reference_size;


assert(NULL != (a = archive_write_new()));
assertA(0 == archive_write_set_format_pax(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_set_bytes_per_block(a, 1));
assertA(0 == archive_write_set_bytes_in_last_block(a, 1));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0777);


assertEntrySetAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]));
assertA(0 == archive_write_header(a, ae));


assertEntrySetAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]));
assertA(0 == archive_write_header(a, ae));


assertEntrySetAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]));
assertA(0 == archive_write_header(a, ae));





assertEntrySetAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]));
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assert(NULL != (f = fopen("testout", "wb")));
assertEqualInt(used, (size_t)fwrite(buff, 1, (unsigned int)used, f));
fclose(f);


extract_reference_file("test_acl_pax_posix1e.tar");
reference = slurpfile(&reference_size, "test_acl_pax_posix1e.tar");


failure("Generated pax archive does not match reference; compare 'testout' to 'test_acl_pax_posix1e.tar' reference file.");
assertEqualMem(buff, reference, reference_size);
failure("Generated pax archive does not match reference; compare 'testout' to 'test_acl_pax_posix1e.tar' reference file.");
assertEqualInt((int)used, reference_size);
free(reference);


assert(NULL != (a = archive_read_new()));
assertA(0 == archive_read_support_format_all(a));
assertA(0 == archive_read_support_filter_all(a));
assertA(0 == archive_read_open_memory(a, buff, used));


assertA(0 == archive_read_next_header(a, &ae));
failure("Basic ACLs shouldn't be stored as extended ACLs");
assert(0 == archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
failure("Basic ACLs should set mode to 0142, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0142);


assertA(0 == archive_read_next_header(a, &ae));
failure("One extended ACL should flag all ACLs to be returned.");
assert(4 == archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
assertEntryCompareAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]),
ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0142);
failure("Basic ACLs should set mode to 0142, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0142);


assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(6, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
assertEntryCompareAcls(ae, acls2, sizeof(acls2)/sizeof(acls2[0]),
ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0543);
failure("Basic ACLs should set mode to 0543, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0543);


assertA(0 == archive_read_next_header(a, &ae));
failure("Basic ACLs shouldn't be stored as extended ACLs");
assert(0 == archive_entry_acl_reset(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
failure("Basic ACLs should set mode to 0142, not %04o",
archive_entry_mode(ae)&0777);
assert((archive_entry_mode(ae) & 0777) == 0142);


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_acl_pax_nfs4)
{
struct archive *a;
struct archive_entry *ae;
size_t used;
FILE *f;
void *reference;
size_t reference_size;


assert(NULL != (a = archive_write_new()));
assertA(0 == archive_write_set_format_pax(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_set_bytes_per_block(a, 1));
assertA(0 == archive_write_set_bytes_in_last_block(a, 1));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0777);


assertEntrySetAcls(ae, acls3, sizeof(acls3)/sizeof(acls3[0]));
assertA(0 == archive_write_header(a, ae));


assertEntrySetAcls(ae, acls4, sizeof(acls4)/sizeof(acls4[0]));
assertA(0 == archive_write_header(a, ae));


assertEntrySetAcls(ae, acls5, sizeof(acls5)/sizeof(acls5[0]));
assertA(0 == archive_write_header(a, ae));

archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assert(NULL != (f = fopen("testout", "wb")));
assertEqualInt(used, (size_t)fwrite(buff, 1, (unsigned int)used, f));
fclose(f);


extract_reference_file("test_acl_pax_nfs4.tar");
reference = slurpfile(&reference_size, "test_acl_pax_nfs4.tar");


failure("Generated pax archive does not match reference; compare 'testout' to 'test_acl_pax_nfs4.tar' reference file.");
assertEqualMem(buff, reference, reference_size);
failure("Generated pax archive does not match reference; compare 'testout' to 'test_acl_pax_nfs4.tar' reference file.");
assertEqualInt((int)used, reference_size);
free(reference);


assert(NULL != (a = archive_read_new()));
assertA(0 == archive_read_support_format_all(a));
assertA(0 == archive_read_support_filter_all(a));
assertA(0 == archive_read_open_memory(a, buff, used));


assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(3, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_ALLOW));
assertEntryCompareAcls(ae, acls3, sizeof(acls3)/sizeof(acls3[0]),
ARCHIVE_ENTRY_ACL_TYPE_ALLOW, 0);


assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(6, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_NFS4));
assertEntryCompareAcls(ae, acls4, sizeof(acls4)/sizeof(acls4[0]),
ARCHIVE_ENTRY_ACL_TYPE_NFS4, 0);


assertA(0 == archive_read_next_header(a, &ae));
assertEqualInt(6, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_NFS4));
assertEntryCompareAcls(ae, acls5, sizeof(acls5)/sizeof(acls5[0]),
ARCHIVE_ENTRY_ACL_TYPE_NFS4, 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
