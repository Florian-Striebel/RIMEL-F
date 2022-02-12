























#include "test.h"
__FBSDID("$FreeBSD$");

static void
group_cleanup(void *d)
{
int *mp = d;
assertEqualInt(*mp, 0x13579);
*mp = 0x2468;
}

static int64_t
group_lookup(void *d, const char *name, int64_t g)
{
int *mp = d;

(void)g;

assertEqualInt(*mp, 0x13579);
if (strcmp(name, "FOOGROUP"))
return (1);
return (73);
}

static void
user_cleanup(void *d)
{
int *mp = d;
assertEqualInt(*mp, 0x1234);
*mp = 0x2345;
}

static int64_t
user_lookup(void *d, const char *name, int64_t u)
{
int *mp = d;

(void)u;

assertEqualInt(*mp, 0x1234);
if (strcmp("FOO", name) == 0)
return (2);
return (74);
}

DEFINE_TEST(test_write_disk_lookup)
{
struct archive *a;
int gmagic = 0x13579, umagic = 0x1234;
int64_t id;

assert((a = archive_write_disk_new()) != NULL);


assertEqualInt(0, archive_write_disk_gid(a, "", 0));
assertEqualInt(12, archive_write_disk_gid(a, "root", 12));
assertEqualInt(12, archive_write_disk_gid(a, "wheel", 12));
assertEqualInt(0, archive_write_disk_uid(a, "", 0));
assertEqualInt(18, archive_write_disk_uid(a, "root", 18));


assertEqualInt(ARCHIVE_OK, archive_write_disk_set_group_lookup(a,
&gmagic, &group_lookup, &group_cleanup));

assertEqualInt(73, archive_write_disk_gid(a, "FOOGROUP", 8));
assertEqualInt(1, archive_write_disk_gid(a, "NOTFOOGROUP", 8));


assertEqualInt(ARCHIVE_OK,
archive_write_disk_set_group_lookup(a, NULL, NULL, NULL));

assertEqualInt(gmagic, 0x2468);


assertEqualInt(ARCHIVE_OK, archive_write_disk_set_user_lookup(a,
&umagic, &user_lookup, &user_cleanup));
assertEqualInt(2, archive_write_disk_uid(a, "FOO", 0));
assertEqualInt(74, archive_write_disk_uid(a, "NOTFOO", 1));
assertEqualInt(ARCHIVE_OK,
archive_write_disk_set_user_lookup(a, NULL, NULL, NULL));
assertEqualInt(umagic, 0x2345);


if (archive_write_disk_set_standard_lookup(a) != ARCHIVE_OK) {
skipping("standard uname/gname lookup");
} else {

id = archive_write_disk_gid(a, "wheel", 8);
if (id != 0)
id = archive_write_disk_gid(a, "root", 8);
failure("Unable to verify lookup of group #0");
#if defined(_WIN32) && !defined(__CYGWIN__)

assertEqualInt(8, id);
#else
assertEqualInt(0, id);
#endif


id = archive_write_disk_uid(a, "root", 8);
failure("Unable to verify lookup of user #0");
#if defined(_WIN32) && !defined(__CYGWIN__)

assertEqualInt(8, id);
#else
assertEqualInt(0, id);
#endif
}


assertEqualInt(ARCHIVE_OK,
archive_write_disk_set_group_lookup(a, NULL, NULL, NULL));
assertEqualInt(ARCHIVE_OK,
archive_write_disk_set_user_lookup(a, NULL, NULL, NULL));
assertEqualInt(0, archive_write_disk_gid(a, "", 0));
assertEqualInt(0, archive_write_disk_uid(a, "", 0));


gmagic = 0x13579;
umagic = 0x1234;
assertEqualInt(ARCHIVE_OK, archive_write_disk_set_group_lookup(a,
&gmagic, &group_lookup, &group_cleanup));
assertEqualInt(ARCHIVE_OK, archive_write_disk_set_user_lookup(a,
&umagic, &user_lookup, &user_cleanup));


assertEqualInt(ARCHIVE_OK, archive_read_free(a));


assertEqualInt(gmagic, 0x2468);
assertEqualInt(umagic, 0x2345);
}
