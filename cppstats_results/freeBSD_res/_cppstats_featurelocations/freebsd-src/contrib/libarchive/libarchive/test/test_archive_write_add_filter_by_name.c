

























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test_filter_by_name(const char *filter_name, int filter_code,
int (*can_filter_prog)(void))
{
struct archive_entry *ae;
struct archive *a;
size_t used;
size_t buffsize = 1024 * 128;
char *buff;
int r;

assert((buff = calloc(1, buffsize)) != NULL);
if (buff == NULL)
return;


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
r = archive_write_add_filter_by_name(a, filter_name);
if (r == ARCHIVE_WARN) {
if (!can_filter_prog()) {
skipping("%s filter not supported on this platform",
filter_name);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
free(buff);
return;
}
} else if (r == ARCHIVE_FATAL &&
(strcmp(archive_error_string(a),
"lzma compression not supported on this platform") == 0 ||
strcmp(archive_error_string(a),
"xz compression not supported on this platform") == 0)) {
skipping("%s filter not supported on this platform", filter_name);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
free(buff);
return;
} else {
if (!assertEqualIntA(a, ARCHIVE_OK, r)) {
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
free(buff);
return;
}
}
if (filter_code == ARCHIVE_FILTER_LRZIP)
{





assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "lrzip:compression=none"));
}
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_per_block(a, 10));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 0);
assertEqualInt(1, archive_entry_mtime(ae));
archive_entry_set_ctime(ae, 1, 0);
assertEqualInt(1, archive_entry_ctime(ae));
archive_entry_set_atime(ae, 1, 0);
assertEqualInt(1, archive_entry_atime(ae));
archive_entry_copy_pathname(ae, "file");
assertEqualString("file", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((AE_IFREG | 0755), archive_entry_mode(ae));
archive_entry_set_size(ae, 8);
assertEqualInt(0, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(8, archive_write_data(a, "12345678", 8));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(8, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, filter_code, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
free(buff);
}

static int
canAlways(void)
{
return 1;
}

static int
cannot(void)
{
return 0;
}

DEFINE_TEST(test_archive_write_add_filter_by_name_b64encode)
{
test_filter_by_name("b64encode", ARCHIVE_FILTER_UU, canAlways);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_bzip2)
{
test_filter_by_name("bzip2", ARCHIVE_FILTER_BZIP2, canBzip2);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_compress)
{
test_filter_by_name("compress", ARCHIVE_FILTER_COMPRESS, canAlways);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_grzip)
{
test_filter_by_name("grzip", ARCHIVE_FILTER_GRZIP, canGrzip);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_gzip)
{
test_filter_by_name("gzip", ARCHIVE_FILTER_GZIP, canGzip);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_lrzip)
{
test_filter_by_name("lrzip", ARCHIVE_FILTER_LRZIP, canLrzip);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_lz4)
{
test_filter_by_name("lz4", ARCHIVE_FILTER_LZ4, canLz4);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_lzip)
{
test_filter_by_name("lzip", ARCHIVE_FILTER_LZIP, cannot);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_lzma)
{
test_filter_by_name("lzma", ARCHIVE_FILTER_LZMA, cannot);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_lzop)
{
test_filter_by_name("lzop", ARCHIVE_FILTER_LZOP, canLzop);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_uuencode)
{
test_filter_by_name("uuencode", ARCHIVE_FILTER_UU, canAlways);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_xz)
{
test_filter_by_name("xz", ARCHIVE_FILTER_XZ, cannot);
}

DEFINE_TEST(test_archive_write_add_filter_by_name_zstd)
{
test_filter_by_name("zstd", ARCHIVE_FILTER_ZSTD, canZstd);
}
