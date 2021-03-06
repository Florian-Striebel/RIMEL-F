























#include "test.h"
__FBSDID("$FreeBSD$");

static const char *
gname_lookup(void *d, int64_t g)
{
(void)d;
(void)g;
return ("FOOGROUP");
}

static const char *
uname_lookup(void *d, int64_t u)
{
(void)d;
(void)u;
return ("FOO");
}

DEFINE_TEST(test_read_disk_entry_from_file)
{
struct archive *a;
struct archive_entry *entry;
FILE *f;

assert((a = archive_read_disk_new()) != NULL);

assertEqualInt(ARCHIVE_OK, archive_read_disk_set_uname_lookup(a,
NULL, &uname_lookup, NULL));
assertEqualInt(ARCHIVE_OK, archive_read_disk_set_gname_lookup(a,
NULL, &gname_lookup, NULL));
assertEqualString(archive_read_disk_uname(a, 0), "FOO");
assertEqualString(archive_read_disk_gname(a, 0), "FOOGROUP");


f = fopen("foo", "wb");
assert(f != NULL);
assertEqualInt(4, fwrite("1234", 1, 4, f));
fclose(f);


entry = archive_entry_new();
assert(entry != NULL);
archive_entry_copy_pathname(entry, "foo");
assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_entry_from_file(a, entry, -1, NULL));


assertEqualString(archive_entry_uname(entry), "FOO");
assertEqualString(archive_entry_gname(entry), "FOOGROUP");
assertEqualInt(archive_entry_size(entry), 4);


archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
