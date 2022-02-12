























#include "test.h"
__FBSDID("$FreeBSD$");

#define UMASK 022






DEFINE_TEST(test_write_disk_secure744)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("archive_write_disk security checks not supported on Windows");
#else
struct archive *a;
struct archive_entry *ae;
size_t buff_size = 8192;
char *buff = malloc(buff_size);
char *p = buff;
int n = 0;
int t;

assert(buff != NULL);


assertUmask(UMASK);


assert((a = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_SYMLINKS);

while (p + 500 < buff + buff_size) {
memset(p, 'x', 100);
p += 100;
p[0] = '\0';

buff[0] = ((n / 1000) % 10) + '0';
buff[1] = ((n / 100) % 10)+ '0';
buff[2] = ((n / 10) % 10)+ '0';
buff[3] = ((n / 1) % 10)+ '0';
buff[4] = '_';
++n;


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, buff);
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_copy_symlink(ae, testworkdir);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);

*p++ = '/';
sprintf(p, "target%d", n);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, buff);
archive_entry_set_mode(ae, S_IFDIR | 0777);

t = archive_write_header(a, ae);
archive_entry_free(ae);
failure("Attempt to create target%d via %d-character symlink should have failed", n, (int)strlen(buff));
if(!assertEqualInt(ARCHIVE_FAILED, t)) {
break;
}
}
archive_free(a);
free(buff);
#endif
}
