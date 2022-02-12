

























#include "test.h"
__FBSDID("$FreeBSD$");

struct archive_read;
extern void __archive_read_reset_passphrase(struct archive_read *);
extern const char * __archive_read_next_passphrase(struct archive_read *);

static void
test(int pristine)
{
struct archive* a = archive_read_new();

if (!pristine) {
archive_read_support_filter_all(a);
archive_read_support_format_all(a);
}

assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass1"));

assertEqualInt(ARCHIVE_FAILED, archive_read_add_passphrase(a, ""));

assertEqualInt(ARCHIVE_FAILED, archive_read_add_passphrase(a, NULL));

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_add_passphrase)
{
test(1);
test(0);
}

DEFINE_TEST(test_archive_read_add_passphrase_incorrect_sequance)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;

assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass1"));



assertEqualString(NULL, __archive_read_next_passphrase(ar));

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_add_passphrase_single)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;

assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass1"));

__archive_read_reset_passphrase(ar);

assertEqualString("pass1", __archive_read_next_passphrase(ar));


assertEqualString(NULL, __archive_read_next_passphrase(ar));

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_add_passphrase_multiple)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;

assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass1"));
assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass2"));

__archive_read_reset_passphrase(ar);

assertEqualString("pass1", __archive_read_next_passphrase(ar));

assertEqualString("pass2", __archive_read_next_passphrase(ar));


assertEqualString(NULL, __archive_read_next_passphrase(ar));

archive_read_free(a);
}

static const char *
callback1(struct archive *a, void *_client_data)
{
(void)a;
(void)_client_data;
return ("passCallBack");
}

DEFINE_TEST(test_archive_read_add_passphrase_set_callback1)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;

assertEqualInt(ARCHIVE_OK,
archive_read_set_passphrase_callback(a, NULL, callback1));

__archive_read_reset_passphrase(ar);

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));

archive_read_free(a);



a = archive_read_new();
ar = (struct archive_read *)a;
assertEqualInt(ARCHIVE_OK,
archive_read_set_passphrase_callback(a, NULL, callback1));

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));

archive_read_free(a);
}

static const char *
callback2(struct archive *a, void *_client_data)
{
int *cd = (int *)_client_data;

(void)a;

if (*cd == 0) {
*cd = 1;
return ("passCallBack");
}
return (NULL);
}

DEFINE_TEST(test_archive_read_add_passphrase_set_callback2)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;
int client_data = 0;

assertEqualInt(ARCHIVE_OK,
archive_read_set_passphrase_callback(a, &client_data, callback2));

__archive_read_reset_passphrase(ar);

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));


assertEqualString(NULL, __archive_read_next_passphrase(ar));

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_add_passphrase_set_callback3)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;
int client_data = 0;

assertEqualInt(ARCHIVE_OK,
archive_read_set_passphrase_callback(a, &client_data, callback2));

__archive_read_reset_passphrase(ar);

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));
__archive_read_reset_passphrase(ar);

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));


assertEqualString(NULL, __archive_read_next_passphrase(ar));

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_add_passphrase_multiple_with_callback)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;
int client_data = 0;

assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass1"));
assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass2"));
assertEqualInt(ARCHIVE_OK,
archive_read_set_passphrase_callback(a, &client_data, callback2));

__archive_read_reset_passphrase(ar);

assertEqualString("pass1", __archive_read_next_passphrase(ar));

assertEqualString("pass2", __archive_read_next_passphrase(ar));

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));


assertEqualString(NULL, __archive_read_next_passphrase(ar));

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_add_passphrase_multiple_with_callback2)
{
struct archive* a = archive_read_new();
struct archive_read *ar = (struct archive_read *)a;
int client_data = 0;

assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass1"));
assertEqualInt(ARCHIVE_OK, archive_read_add_passphrase(a, "pass2"));
assertEqualInt(ARCHIVE_OK,
archive_read_set_passphrase_callback(a, &client_data, callback2));

__archive_read_reset_passphrase(ar);

assertEqualString("pass1", __archive_read_next_passphrase(ar));

assertEqualString("pass2", __archive_read_next_passphrase(ar));

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));

__archive_read_reset_passphrase(ar);

assertEqualString("passCallBack", __archive_read_next_passphrase(ar));

assertEqualString("pass1", __archive_read_next_passphrase(ar));

assertEqualString("pass2", __archive_read_next_passphrase(ar));


assertEqualString(NULL, __archive_read_next_passphrase(ar));

archive_read_free(a);
}

