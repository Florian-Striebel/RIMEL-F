























#include "test.h"
__FBSDID("$FreeBSD$");

static void
gname_cleanup(void *d)
{
int *mp = d;
assertEqualInt(*mp, 0x13579);
*mp = 0x2468;
}

static const char *
gname_lookup(void *d, int64_t g)
{
int *mp = d;
assertEqualInt(*mp, 0x13579);
if (g == 1)
return ("FOOGROUP");
return ("NOTFOOGROUP");
}

static void
uname_cleanup(void *d)
{
int *mp = d;
assertEqualInt(*mp, 0x1234);
*mp = 0x2345;
}

static const char *
uname_lookup(void *d, int64_t u)
{
int *mp = d;
assertEqualInt(*mp, 0x1234);
if (u == 1)
return ("FOO");
return ("NOTFOO");
}

#if !defined(__CYGWIN__) && !defined(__HAIKU__)






static const char *zero_groups[] = {
"root",
"wheel"
};
#endif

DEFINE_TEST(test_read_disk)
{
struct archive *a;
int gmagic = 0x13579, umagic = 0x1234;
#if !defined(__CYGWIN__) && !defined(__HAIKU__)
const char *p;
size_t i;
#endif

assert((a = archive_read_disk_new()) != NULL);


assert(archive_read_disk_gname(a, 0) == NULL);
assert(archive_read_disk_uname(a, 0) == NULL);


assertEqualInt(ARCHIVE_OK, archive_read_disk_set_gname_lookup(a,
&gmagic, &gname_lookup, &gname_cleanup));

assertEqualString(archive_read_disk_gname(a, 0), "NOTFOOGROUP");
assertEqualString(archive_read_disk_gname(a, 1), "FOOGROUP");


assertEqualInt(ARCHIVE_OK,
archive_read_disk_set_gname_lookup(a, NULL, NULL, NULL));

assertEqualInt(gmagic, 0x2468);


assertEqualInt(ARCHIVE_OK, archive_read_disk_set_uname_lookup(a,
&umagic, &uname_lookup, &uname_cleanup));
assertEqualString(archive_read_disk_uname(a, 0), "NOTFOO");
assertEqualString(archive_read_disk_uname(a, 1), "FOO");
assertEqualInt(ARCHIVE_OK,
archive_read_disk_set_uname_lookup(a, NULL, NULL, NULL));
assertEqualInt(umagic, 0x2345);


if (archive_read_disk_set_standard_lookup(a) != ARCHIVE_OK) {
skipping("standard uname/gname lookup");
} else {
#if defined(__CYGWIN__) || defined(__HAIKU__)


skipping("standard uname/gname lookup");
#else




assertEqualString(archive_read_disk_uname(a, 0), "root");


p = archive_read_disk_gname(a, 0);
assert(p != NULL);
if (p != NULL) {
i = 0;
while (i < sizeof(zero_groups)/sizeof(zero_groups[0])) {
if (strcmp(zero_groups[i], p) == 0)
break;
++i;
}
if (i == sizeof(zero_groups)/sizeof(zero_groups[0])) {






failure("group 0 didn't have any of the expected names");
assertEqualString(p, zero_groups[0]);
}
}
#endif
}


assertEqualInt(ARCHIVE_OK,
archive_read_disk_set_gname_lookup(a, NULL, NULL, NULL));
assertEqualInt(ARCHIVE_OK,
archive_read_disk_set_uname_lookup(a, NULL, NULL, NULL));
assert(archive_read_disk_gname(a, 0) == NULL);
assert(archive_read_disk_uname(a, 0) == NULL);


gmagic = 0x13579;
umagic = 0x1234;
assertEqualInt(ARCHIVE_OK, archive_read_disk_set_gname_lookup(a,
&gmagic, &gname_lookup, &gname_cleanup));
assertEqualInt(ARCHIVE_OK, archive_read_disk_set_uname_lookup(a,
&umagic, &uname_lookup, &uname_cleanup));


assertEqualInt(ARCHIVE_OK, archive_read_free(a));


assertEqualInt(gmagic, 0x2468);
assertEqualInt(umagic, 0x2345);
}
