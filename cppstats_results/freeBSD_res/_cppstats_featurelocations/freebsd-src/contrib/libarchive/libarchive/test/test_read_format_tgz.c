























#include "test.h"
__FBSDID("$FreeBSD$");

static unsigned char archive[] = {
31,139,8,0,222,'C','p','C',0,3,211,'c',160,'=','0','0','0','0','7','5','U',
0,210,134,230,166,6,200,'4',28,'(',24,26,24,27,155,24,152,24,154,27,155,')',
24,24,26,152,154,25,'2','(',152,210,193,'m',12,165,197,'%',137,'E','@',167,
148,'d',230,226,'U','G','H',30,234,15,'8','=',10,'F',193,'(',24,5,131,28,
0,0,29,172,5,240,0,6,0,0};

DEFINE_TEST(test_read_format_tgz)
{
struct archive_entry *ae;
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
assertEqualInt(ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_gzip(a);
if (r == ARCHIVE_WARN) {
skipping("gzip reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualInt(ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualInt(ARCHIVE_OK,
archive_read_open_memory(a, archive, sizeof(archive)));
assertEqualInt(ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_file_count(a));
assertEqualInt(archive_filter_code(a, 0),
ARCHIVE_FILTER_GZIP);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK,archive_read_free(a));
}


