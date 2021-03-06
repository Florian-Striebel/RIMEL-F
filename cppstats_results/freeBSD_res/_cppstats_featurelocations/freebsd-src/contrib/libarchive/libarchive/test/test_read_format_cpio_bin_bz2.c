























#include "test.h"
__FBSDID("$FreeBSD$");

static unsigned char archive[] = {
'B','Z','h','9','1','A','Y','&','S','Y',134,'J',208,'4',0,0,30,246,141,253,
8,2,0,' ',1,'*','&',20,0,'`',' ',' ',2,0,128,0,'B',4,8,' ',0,'T','P',0,'4',
0,13,6,137,168,245,27,'Q',160,'a',25,169,5,'I',187,'(',10,'d','E',177,177,
142,218,232,'r',130,'4','D',247,'<','Z',190,'U',237,236,'d',227,31,' ','z',
192,'E','_',23,'r','E','8','P',144,134,'J',208,'4'};

DEFINE_TEST(test_read_format_cpio_bin_bz2)
{
struct archive_entry *ae;
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
r = archive_read_support_filter_bzip2(a);
if (r != ARCHIVE_OK) {
skipping("bzip2 support unavailable");
archive_read_close(a);
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, archive, sizeof(archive)));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assert(archive_filter_code(a, 0) == ARCHIVE_FILTER_BZIP2);
assert(archive_format(a) == ARCHIVE_FORMAT_CPIO_BIN_LE);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


