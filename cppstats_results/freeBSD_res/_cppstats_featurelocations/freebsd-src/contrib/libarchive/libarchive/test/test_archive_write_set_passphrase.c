

























#include "test.h"
__FBSDID("$FreeBSD$");

struct archive_write;
extern const char * __archive_write_get_passphrase(struct archive_write *);

static void
test(int pristine)
{
struct archive* a = archive_write_new();
struct archive_write* aw = (struct archive_write *)a;

if (!pristine) {
archive_write_add_filter_gzip(a);
archive_write_set_format_iso9660(a);
}

assertEqualInt(ARCHIVE_OK, archive_write_set_passphrase(a, "pass1"));

assertEqualInt(ARCHIVE_FAILED, archive_write_set_passphrase(a, ""));

assertEqualInt(ARCHIVE_FAILED, archive_write_set_passphrase(a, NULL));

assertEqualString("pass1", __archive_write_get_passphrase(aw));

assertEqualInt(ARCHIVE_OK, archive_write_set_passphrase(a, "pass2"));
assertEqualString("pass2", __archive_write_get_passphrase(aw));

archive_write_free(a);
}

DEFINE_TEST(test_archive_write_set_passphrase)
{
test(1);
test(0);
}


static const char *
callback1(struct archive *a, void *_client_data)
{
int *cnt;

(void)a;

cnt = (int *)_client_data;
*cnt += 1;
return ("passCallBack");
}

DEFINE_TEST(test_archive_write_set_passphrase_callback)
{
struct archive* a = archive_write_new();
struct archive_write* aw = (struct archive_write *)a;
int cnt = 0;

archive_write_set_format_zip(a);

assertEqualInt(ARCHIVE_OK,
archive_write_set_passphrase_callback(a, &cnt, callback1));

assertEqualString("passCallBack", __archive_write_get_passphrase(aw));
assertEqualInt(1, cnt);

assertEqualString("passCallBack", __archive_write_get_passphrase(aw));
assertEqualInt(1, cnt);

archive_write_free(a);
}
