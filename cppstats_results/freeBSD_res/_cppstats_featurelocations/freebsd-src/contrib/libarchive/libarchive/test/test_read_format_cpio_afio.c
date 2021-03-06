
























#include "test.h"
__FBSDID("$FreeBSD$");
















static unsigned char archive[] = {
'0','7','0','7','0','7','0','0','0','1','4','3','1','2','5','3',
'2','1','1','0','0','6','4','4','0','0','1','7','5','1','0','0',
'1','7','5','1','0','0','0','0','0','1','0','0','0','0','0','0',
'1','1','3','3','2','2','4','5','0','2','0','0','0','0','0','0',
'6','0','0','0','0','0','0','0','0','0','2','1','f','i','l','e',
'1',0,'0','1','2','3','4','5','6','7','8','9','a','b','c','d',
'e','f','\n','0','7','0','7','2','7','0','0','0','0','0','0','6',
'3','0','0','0','0','0','0','0','0','0','0','0','D','A','A','E',
'6','m','1','0','0','6','4','4','0','0','0','1','0','0','0','0',
'0','0','0','0','0','3','E','9','0','0','0','0','0','0','0','1',
'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',
'4','B','6','9','4','A','1','0','n','0','0','0','6','0','0','0',
'0','0','0','0','0','s','0','0','0','0','0','0','0','0','0','0',
'0','0','0','0','1','1',':','f','i','l','e','2',0,'0','1','2',
'3','4','5','6','7','8','9','a','b','c','d','e','f','\n','0','7',
'0','7','0','7','0','0','0','0','0','0','0','0','0','0','0','0',
'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',
'0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0',
'0','0','0','0','0','0','0','0','0','0','0','0','0','1','3','0',
'0','0','0','0','0','1','1','2','7','3','T','R','A','I','L','E',
'R','!','!','!',0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};




static int
uid_size(void)
{
return (sizeof(uid_t));
}

DEFINE_TEST(test_read_format_cpio_afio)
{
unsigned char *p;
size_t size;
struct archive_entry *ae;
struct archive *a;


size = (sizeof(archive) + 5120 -1 / 5120) * 5120;
assert((p = malloc(size)) != NULL);
if (p == NULL)
return;
memset(p, 0, size);
memcpy(p, archive, sizeof(archive));
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, p, size));



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(17, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertA(archive_filter_code(a, 0) == ARCHIVE_FILTER_NONE);
assertA(archive_format(a) == ARCHIVE_FORMAT_CPIO_POSIX);



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(17, archive_entry_size(ae));
if (uid_size() > 4)
assertEqualInt(65536, archive_entry_uid(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertA(archive_filter_code(a, 0) == ARCHIVE_FILTER_NONE);
assertA(archive_format(a) == ARCHIVE_FORMAT_CPIO_AFIO_LARGE);
assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(p);
}
