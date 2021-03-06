























#include "test.h"
__FBSDID("$FreeBSD$");

static void test_linkify_tar(void)
{
struct archive_entry *entry, *e2;
struct archive_entry_linkresolver *resolver;


assert(NULL != (resolver = archive_entry_linkresolver_new()));
archive_entry_linkresolver_set_strategy(resolver,
ARCHIVE_FORMAT_TAR_USTAR);


assert(NULL != (entry = archive_entry_new()));
archive_entry_set_pathname(entry, "test1");
archive_entry_set_ino(entry, 1);
archive_entry_set_dev(entry, 2);
archive_entry_set_nlink(entry, 1);
archive_entry_set_size(entry, 10);
archive_entry_linkify(resolver, &entry, &e2);


assert(e2 == NULL);
assertEqualInt(10, archive_entry_size(entry));
assertEqualString("test1", archive_entry_pathname(entry));


archive_entry_set_pathname(entry, "test2");
archive_entry_set_nlink(entry, 2);
archive_entry_set_ino(entry, 2);
archive_entry_linkify(resolver, &entry, &e2);

assert(e2 == NULL);
assertEqualString("test2", archive_entry_pathname(entry));
assertEqualString(NULL, archive_entry_hardlink(entry));
assertEqualInt(10, archive_entry_size(entry));


archive_entry_linkify(resolver, &entry, &e2);
assert(e2 == NULL);
assertEqualString("test2", archive_entry_pathname(entry));
assertEqualString("test2", archive_entry_hardlink(entry));
assertEqualInt(0, archive_entry_size(entry));



archive_entry_set_pathname(entry, "test3");
archive_entry_set_nlink(entry, 2);
archive_entry_set_filetype(entry, AE_IFDIR);
archive_entry_set_ino(entry, 3);
archive_entry_set_hardlink(entry, NULL);
archive_entry_linkify(resolver, &entry, &e2);

assert(e2 == NULL);
assertEqualString("test3", archive_entry_pathname(entry));
assertEqualString(NULL, archive_entry_hardlink(entry));


archive_entry_linkify(resolver, &entry, &e2);
assert(e2 == NULL);
assertEqualString("test3", archive_entry_pathname(entry));
assertEqualString(NULL, archive_entry_hardlink(entry));

archive_entry_free(entry);
archive_entry_linkresolver_free(resolver);
}

static void test_linkify_old_cpio(void)
{
struct archive_entry *entry, *e2;
struct archive_entry_linkresolver *resolver;


assert(NULL != (resolver = archive_entry_linkresolver_new()));
archive_entry_linkresolver_set_strategy(resolver,
ARCHIVE_FORMAT_CPIO_POSIX);


assert(NULL != (entry = archive_entry_new()));
archive_entry_set_pathname(entry, "test1");
archive_entry_set_ino(entry, 1);
archive_entry_set_dev(entry, 2);
archive_entry_set_nlink(entry, 2);
archive_entry_set_size(entry, 10);
archive_entry_linkify(resolver, &entry, &e2);


assert(e2 == NULL);
assertEqualInt(10, archive_entry_size(entry));
assertEqualString("test1", archive_entry_pathname(entry));


archive_entry_linkify(resolver, &entry, &e2);
assert(e2 == NULL);
assertEqualString("test1", archive_entry_pathname(entry));
assertEqualString(NULL, archive_entry_hardlink(entry));
assertEqualInt(10, archive_entry_size(entry));

archive_entry_free(entry);
archive_entry_linkresolver_free(resolver);
}

static void test_linkify_new_cpio(void)
{
struct archive_entry *entry, *e2;
struct archive_entry_linkresolver *resolver;


assert(NULL != (resolver = archive_entry_linkresolver_new()));
archive_entry_linkresolver_set_strategy(resolver,
ARCHIVE_FORMAT_CPIO_SVR4_NOCRC);


assert(NULL != (entry = archive_entry_new()));
archive_entry_set_pathname(entry, "test1");
archive_entry_set_ino(entry, 1);
archive_entry_set_dev(entry, 2);
archive_entry_set_nlink(entry, 1);
archive_entry_set_size(entry, 10);
archive_entry_linkify(resolver, &entry, &e2);


assert(e2 == NULL);
assertEqualInt(10, archive_entry_size(entry));
assertEqualString("test1", archive_entry_pathname(entry));


archive_entry_set_pathname(entry, "test2");
archive_entry_set_nlink(entry, 3);
archive_entry_set_ino(entry, 2);
archive_entry_linkify(resolver, &entry, &e2);


assert(entry == NULL);
assert(e2 == NULL);


assert(NULL != (entry = archive_entry_new()));
archive_entry_set_pathname(entry, "test3");
archive_entry_set_ino(entry, 2);
archive_entry_set_dev(entry, 2);
archive_entry_set_nlink(entry, 2);
archive_entry_set_size(entry, 10);
archive_entry_linkify(resolver, &entry, &e2);


assertEqualString("test2", archive_entry_pathname(entry));
assertEqualInt(0, archive_entry_size(entry));
archive_entry_free(entry);
assert(NULL == e2);
archive_entry_free(e2);


assert(NULL != (entry = archive_entry_new()));
archive_entry_set_pathname(entry, "test4");
archive_entry_set_ino(entry, 2);
archive_entry_set_dev(entry, 2);
archive_entry_set_nlink(entry, 3);
archive_entry_set_size(entry, 10);
archive_entry_linkify(resolver, &entry, &e2);


assertEqualString("test3", archive_entry_pathname(entry));
assertEqualInt(0, archive_entry_size(entry));


assertEqualString("test4", archive_entry_pathname(e2));
assertEqualInt(10, archive_entry_size(e2));

archive_entry_free(entry);
archive_entry_free(e2);
archive_entry_linkresolver_free(resolver);
}

DEFINE_TEST(test_link_resolver)
{
test_linkify_tar();
test_linkify_old_cpio();
test_linkify_new_cpio();
}
