























#include "test.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>









struct checker {
struct archive *short_archive;
char *shortbuf;
size_t shortbuf_len;

struct archive *full_archive;
char *fullbuf;
size_t fullbuf_len;
};

static ssize_t
short_write_callback(struct archive *a, void *client_data, const void *buffer, size_t length)
{
(void)a;

struct checker *checker = client_data;
size_t to_write = length < 100 ? length : 100;
size_t new_len = checker->shortbuf_len + to_write;
char *new_buf = realloc(checker->shortbuf, new_len);
assert(new_buf != NULL);

checker->shortbuf = new_buf;
memcpy(checker->shortbuf + checker->shortbuf_len, buffer, to_write);
checker->shortbuf_len = new_len;

return to_write;
}

static ssize_t
full_write_callback(struct archive *a, void *client_data, const void *buffer, size_t length)
{
(void)a;

struct checker *checker = client_data;
size_t to_write = length;
size_t new_len = checker->fullbuf_len + to_write;
char *new_buf = realloc(checker->fullbuf, new_len);
assert(new_buf != NULL);

checker->fullbuf = new_buf;
memcpy(checker->fullbuf + checker->fullbuf_len, buffer, to_write);
checker->fullbuf_len = new_len;

return to_write;
}

static struct archive *
create_archive(struct checker *checker, archive_write_callback write_cb, int buffered)
{
struct archive *a;

assert((a = archive_write_new()) != NULL);

if (!buffered)
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 0));







assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_in_last_block(a, 1));


assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_format_pax(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_add_filter_none(a));

assertEqualIntA(a, ARCHIVE_OK,
archive_write_open(a, checker, NULL, write_cb, NULL));

return a;
}

static struct checker *
checker_new(int buffered)
{
struct checker *checker;

assert ((checker = calloc(1, sizeof *checker)) != NULL);

checker->short_archive = create_archive(checker, short_write_callback, buffered);
checker->full_archive = create_archive(checker, full_write_callback, buffered);

return checker;
}

static void
checker_add_file(struct checker *checker, const char *name, char *buffer, size_t len)
{
struct archive_entry *entry;
assert((entry = archive_entry_new()) != NULL);

archive_entry_set_pathname(entry, name);
archive_entry_set_mode(entry, AE_IFREG | 0755);
archive_entry_set_size(entry, len);

assertEqualIntA(checker->short_archive, ARCHIVE_OK,
archive_write_header(checker->short_archive, entry));
assertEqualIntA(checker->short_archive, len,
archive_write_data(checker->short_archive, buffer, len));

assertEqualIntA(checker->full_archive, ARCHIVE_OK,
archive_write_header(checker->full_archive, entry));
assertEqualIntA(checker->full_archive, len,
archive_write_data(checker->full_archive, buffer, len));

archive_entry_free(entry);
}

static void
checker_close(struct checker *checker)
{
assertEqualIntA(checker->short_archive, ARCHIVE_OK,
archive_write_close(checker->short_archive));
assertEqualIntA(checker->short_archive, ARCHIVE_OK,
archive_write_close(checker->full_archive));
}

static void
checker_check(struct checker *checker)
{
assertEqualInt(checker->shortbuf_len, checker->fullbuf_len);
assert(memcmp(checker->shortbuf, checker->fullbuf, checker->fullbuf_len) == 0);
}

static void
checker_free(struct checker *checker)
{
free(checker->shortbuf);
free(checker->fullbuf);
free(checker);
}

DEFINE_TEST(test_short_writes)
{
struct checker *checker;
uint16_t test_data[16384];
int i;

for (i = 0; i < 16384; i++)
test_data[i] = i;





checker = checker_new(1);
checker_add_file(checker, "a", (char *)test_data, 1024);
checker_close(checker);
assert(checker->shortbuf_len > 1024);
checker_check(checker);
checker_free(checker);





checker = checker_new(1);
checker_add_file(checker, "a", (char *)test_data, 21 * 1024);
checker_close(checker);
assert(checker->shortbuf_len > 21 * 1024);
checker_check(checker);
checker_free(checker);



checker = checker_new(0);
checker_add_file(checker, "a", (char *)test_data, 1024);
checker_close(checker);
assert(checker->shortbuf_len > 1024);
checker_check(checker);
checker_free(checker);
}
