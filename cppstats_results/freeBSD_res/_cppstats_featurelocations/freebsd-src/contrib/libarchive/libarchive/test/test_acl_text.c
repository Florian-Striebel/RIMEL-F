























#include "test.h"
__FBSDID("$FreeBSD$");








static struct archive_test_acl_t acls0[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_READ |
ARCHIVE_ENTRY_ACL_WRITE,
ARCHIVE_ENTRY_ACL_USER_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 100, "user100" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0,
ARCHIVE_ENTRY_ACL_USER, 1000, "user1000" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_READ |
ARCHIVE_ENTRY_ACL_WRITE,
ARCHIVE_ENTRY_ACL_GROUP, 78, "group78" },
{ ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_READ |
ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_OTHER, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DEFAULT,
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DEFAULT,
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DEFAULT, 0,
ARCHIVE_ENTRY_ACL_OTHER, -1, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_DEFAULT,
ARCHIVE_ENTRY_ACL_EXECUTE |
ARCHIVE_ENTRY_ACL_READ,
ARCHIVE_ENTRY_ACL_USER, 101, "user101"},
{ ARCHIVE_ENTRY_ACL_TYPE_DEFAULT,
ARCHIVE_ENTRY_ACL_EXECUTE,
ARCHIVE_ENTRY_ACL_GROUP, 79, "group79" },
};

static struct archive_test_acl_t acls1[] = {
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_WRITE_OWNER,
ARCHIVE_ENTRY_ACL_USER, 77, "user77" },
{ ARCHIVE_ENTRY_ACL_TYPE_DENY,
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_DELETE_CHILD |
ARCHIVE_ENTRY_ACL_DELETE |
ARCHIVE_ENTRY_ACL_ENTRY_FILE_INHERIT |
ARCHIVE_ENTRY_ACL_ENTRY_DIRECTORY_INHERIT |
ARCHIVE_ENTRY_ACL_ENTRY_INHERIT_ONLY |
ARCHIVE_ENTRY_ACL_ENTRY_NO_PROPAGATE_INHERIT,
ARCHIVE_ENTRY_ACL_USER, 101, "user101" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_ENTRY_INHERITED,
ARCHIVE_ENTRY_ACL_GROUP, 78, "group78" },
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
ARCHIVE_ENTRY_ACL_WRITE_OWNER,
ARCHIVE_ENTRY_ACL_USER_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_WRITE_DATA |
ARCHIVE_ENTRY_ACL_APPEND_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL,
ARCHIVE_ENTRY_ACL_GROUP_OBJ, 0, "" },
{ ARCHIVE_ENTRY_ACL_TYPE_ALLOW,
ARCHIVE_ENTRY_ACL_READ_DATA |
ARCHIVE_ENTRY_ACL_READ_ATTRIBUTES |
ARCHIVE_ENTRY_ACL_READ_NAMED_ATTRS |
ARCHIVE_ENTRY_ACL_READ_ACL |
ARCHIVE_ENTRY_ACL_SYNCHRONIZE,
ARCHIVE_ENTRY_ACL_EVERYONE, 0, "" },
};

const char* acltext[] = {
"user::rwx\n"
"group::r-x\n"
"other::r-x\n"
"user:user100:r-x\n"
"user:user1000:---\n"
"group:group78:rwx\n"
"default:user::r-x\n"
"default:group::r-x\n"
"default:other::---\n"
"default:user:user101:r-x\n"
"default:group:group79:--x",

"user::rwx\n"
"group::r-x\n"
"other::r-x\n"
"user:user100:r-x:100\n"
"user:user1000:---:1000\n"
"group:group78:rwx:78\n"
"default:user::r-x\n"
"default:group::r-x\n"
"default:other::---\n"
"default:user:user101:r-x:101\n"
"default:group:group79:--x:79",

"u::rwx\n"
"g::r-x\n"
"o::r-x\n"
"u:user100:r-x:100\n"
"u:user1000:---:1000\n"
"g:group78:rwx:78\n"
"d:user::r-x\n"
"d:group::r-x\n"
"d:other::---\n"
"d:user:user101:r-x:101\n"
"d:group:group79:--x:79",

"user::rwx\n"
"group::r-x\n"
"other::r-x\n"
"user:user100:r-x\n"
"user:user1000:---\n"
"group:group78:rwx",

"user::rwx,"
"group::r-x,"
"other::r-x,"
"user:user100:r-x,"
"user:user1000:---,"
"group:group78:rwx",

"user::rwx\n"
"group::r-x\n"
"other::r-x\n"
"user:user100:r-x:100\n"
"user:user1000:---:1000\n"
"group:group78:rwx:78",

"user::r-x\n"
"group::r-x\n"
"other::---\n"
"user:user101:r-x\n"
"group:group79:--x",

"user::r-x\n"
"group::r-x\n"
"other::---\n"
"user:user101:r-x:101\n"
"group:group79:--x:79",

"default:user::r-x\n"
"default:group::r-x\n"
"default:other::---\n"
"default:user:user101:r-x\n"
"default:group:group79:--x",

"user:user77:rw-p--a-R-c-o-:-------:allow\n"
"user:user101:-w-pdD--------:fdin---:deny\n"
"group:group78:r-----a-R-c---:------I:allow\n"
"owner@:rwxp--aARWcCo-:-------:allow\n"
"group@:rw-p--a-R-c---:-------:allow\n"
"everyone@:r-----a-R-c--s:-------:allow",

"user:user77:rw-p--a-R-c-o-:-------:allow:77\n"
"user:user101:-w-pdD--------:fdin---:deny:101\n"
"group:group78:r-----a-R-c---:------I:allow:78\n"
"owner@:rwxp--aARWcCo-:-------:allow\n"
"group@:rw-p--a-R-c---:-------:allow\n"
"everyone@:r-----a-R-c--s:-------:allow",

"user:user77:rwpaRco::allow:77\n"
"user:user101:wpdD:fdin:deny:101\n"
"group:group78:raRc:I:allow:78\n"
"owner@:rwxpaARWcCo::allow\n"
"group@:rwpaRc::allow\n"
"everyone@:raRcs::allow"
};

static wchar_t *
convert_s_to_ws(const char *s)
{
size_t len;
wchar_t *ws = NULL;

if (s != NULL) {
len = strlen(s) + 1;
ws = malloc(len * sizeof(wchar_t));
assert(mbstowcs(ws, s, len) != (size_t)-1);
}

return (ws);
}

static void
compare_acl_text(struct archive_entry *ae, int flags, const char *s)
{
char *text;
wchar_t *wtext;
wchar_t *ws;
ssize_t slen;

ws = convert_s_to_ws(s);

text = archive_entry_acl_to_text(ae, &slen, flags);
assertEqualString(text, s);
if (text != NULL)
assertEqualInt(strlen(text), slen);
wtext = archive_entry_acl_to_text_w(ae, &slen, flags);
assertEqualWString(wtext, ws);
if (wtext != NULL) {
assertEqualInt(wcslen(wtext), slen);
}
free(text);
free(wtext);
free(ws);
}

DEFINE_TEST(test_acl_from_text)
{
struct archive_entry *ae;
wchar_t *ws = NULL;


assert((ae = archive_entry_new()) != NULL);


assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text(ae, acltext[5],
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0755);
assertEqualInt(6, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));


assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text(ae, acltext[7],
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E, 0755);
assertEqualInt(11, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
archive_entry_acl_clear(ae);


ws = convert_s_to_ws(acltext[5]);

assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text_w(ae, ws,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_ACCESS, 0755);
assertEqualInt(6, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));

free(ws);
ws = convert_s_to_ws(acltext[7]);

assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text_w(ae, ws,
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E, 0755);
assertEqualInt(11, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
archive_entry_acl_clear(ae);


assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text(ae, acltext[7],
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT, 0);
assertEqualInt(5, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT));
archive_entry_acl_clear(ae);


assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text_w(ae, ws,
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT, 0);
assertEqualInt(5, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT));
archive_entry_acl_clear(ae);


assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text(ae, acltext[1],
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E, 0755);
assertEqualInt(11, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
archive_entry_acl_clear(ae);

free(ws);
ws = convert_s_to_ws(acltext[1]);
assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text_w(ae, ws,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E, 0755);
assertEqualInt(11, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
archive_entry_acl_clear(ae);


assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text(ae, acltext[2],
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E, 0755);
assertEqualInt(11, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
archive_entry_acl_clear(ae);

free(ws);
ws = convert_s_to_ws(acltext[2]);
assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text_w(ae, ws,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
assertEntryCompareAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]),
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E, 0755);
assertEqualInt(11, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_POSIX1E));
archive_entry_acl_clear(ae);


assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text(ae, acltext[10],
ARCHIVE_ENTRY_ACL_TYPE_NFS4));
assertEntryCompareAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]),
ARCHIVE_ENTRY_ACL_TYPE_NFS4, 0);
assertEqualInt(6, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_NFS4));
archive_entry_acl_clear(ae);

free(ws);
ws = convert_s_to_ws(acltext[10]);

assertEqualInt(ARCHIVE_OK,
archive_entry_acl_from_text_w(ae, ws,
ARCHIVE_ENTRY_ACL_TYPE_NFS4));
assertEntryCompareAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]),
ARCHIVE_ENTRY_ACL_TYPE_NFS4, 0);
assertEqualInt(6, archive_entry_acl_reset(ae,
ARCHIVE_ENTRY_ACL_TYPE_NFS4));
archive_entry_acl_clear(ae);

free(ws);
archive_entry_free(ae);
}

DEFINE_TEST(test_acl_to_text)
{
struct archive_entry *ae;


assert((ae = archive_entry_new()) != NULL);


assertEntrySetAcls(ae, acls0, sizeof(acls0)/sizeof(acls0[0]));


compare_acl_text(ae, 0, acltext[0]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS |
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT, acltext[0]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS |
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT |
ARCHIVE_ENTRY_ACL_STYLE_MARK_DEFAULT, acltext[0]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_STYLE_EXTRA_ID, acltext[1]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS, acltext[3]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS |
ARCHIVE_ENTRY_ACL_STYLE_SEPARATOR_COMMA,
acltext[4]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_ACCESS |
ARCHIVE_ENTRY_ACL_STYLE_EXTRA_ID, acltext[5]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_DEFAULT, acltext[6]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_DEFAULT |
ARCHIVE_ENTRY_ACL_STYLE_EXTRA_ID, acltext[7]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_TYPE_DEFAULT |
ARCHIVE_ENTRY_ACL_STYLE_MARK_DEFAULT, acltext[8]);


assertEntrySetAcls(ae, acls1, sizeof(acls1)/sizeof(acls1[0]));


compare_acl_text(ae, 0, acltext[9]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_STYLE_EXTRA_ID, acltext[10]);


compare_acl_text(ae, ARCHIVE_ENTRY_ACL_STYLE_EXTRA_ID |
ARCHIVE_ENTRY_ACL_STYLE_COMPACT, acltext[11]);

archive_entry_free(ae);
}
