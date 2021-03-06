























#include "test.h"
__FBSDID("$FreeBSD$");

#include <limits.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#if !defined(__BORLANDC__)
#define getcwd _getcwd
#endif
#endif




static int
atimeIsUpdated(void)
{
const char *fn = "fs_noatime";
struct stat st;
#if defined(_WIN32) && !defined(CYGWIN)
char *buff = NULL;
char *ptr;
int r;

r = systemf("fsutil behavior query disableLastAccess > query_atime");
if (r == 0) {
buff = slurpfile(NULL, "query_atime");
if (buff != NULL) {
ptr = buff;
while(*ptr != '\0' && !isdigit(*ptr)) {
ptr++;
}
if (*ptr == '0') {
free(buff);
return(1);
} else if (*ptr == '1' || *ptr == '2') {
free(buff);
return(0);
}
free(buff);
}
}
#endif
if (!assertMakeFile(fn, 0666, "a"))
return (0);
if (!assertUtimes(fn, 1, 0, 1, 0))
return (0);

if (!assertTextFileContents("a", fn))
return (0);
if (stat(fn, &st) != 0)
return (0);

if (st.st_atime > 1)
return (1);
return (0);
}

static void
test_basic(void)
{
struct archive *a;
struct archive_entry *ae;
const void *p;
char *initial_cwd, *cwd;
size_t size;
int64_t offset;
int file_count;
#if defined(_WIN32) && !defined(__CYGWIN__)
wchar_t *wcwd, *wp, *fullpath;
#endif

assertMakeDir("dir1", 0755);
assertMakeFile("dir1/file1", 0644, "0123456789");
assertMakeFile("dir1/file2", 0644, "hello world");
assertMakeDir("dir1/sub1", 0755);
assertMakeFile("dir1/sub1/file1", 0644, "0123456789");
assertMakeDir("dir1/sub2", 0755);
assertMakeFile("dir1/sub2/file1", 0644, "0123456789");
assertMakeFile("dir1/sub2/file2", 0644, "0123456789");
assertMakeDir("dir1/sub2/sub1", 0755);
assertMakeDir("dir1/sub2/sub2", 0755);
assertMakeDir("dir1/sub2/sub3", 0755);
assertMakeFile("dir1/sub2/sub3/file", 0644, "xyz");
file_count = 12;

assert((ae = archive_entry_new()) != NULL);
assert((a = archive_read_disk_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "dir1"));

while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "dir1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 11);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 11);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "hello world", 11);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 11);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub2/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub2/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub2/sub1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub2/sub2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub2/sub3") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (strcmp(archive_entry_pathname(ae),
"dir1/sub2/sub3/file") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 3);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 3);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "xyz", 3);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 3);
assertEqualInt(0, archive_read_disk_can_descend(a));
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));




assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open_w(a, L"dir1"));

file_count = 12;
while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (wcscmp(archive_entry_pathname_w(ae), L"dir1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 11);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 11);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "hello world", 11);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 11);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub2/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub2/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
assertEqualInt(0, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub2/sub1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub2/sub2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub2/sub3") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
assertEqualInt(1, archive_read_disk_can_descend(a));
} else if (wcscmp(archive_entry_pathname_w(ae),
L"dir1/sub2/sub3/file") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 3);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 3);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "xyz", 3);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 3);
assertEqualInt(0, archive_read_disk_can_descend(a));
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));




assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "dir1/file1"));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(0, archive_read_disk_can_descend(a));
assertEqualString(archive_entry_pathname(ae), "dir1/file1");
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));


#if defined(_WIN32) && !defined(__CYGWIN__)



assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "dir1/*1"));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(0, archive_read_disk_can_descend(a));
assertEqualString(archive_entry_pathname(ae), "dir1/file1");
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(1, archive_read_disk_can_descend(a));
assertEqualString(archive_entry_pathname(ae), "dir1/sub1");
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);


assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_descend(a));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(0, archive_read_disk_can_descend(a));
assertEqualString(archive_entry_pathname(ae), "dir1/sub1/file1");
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));




wcwd = _wgetcwd(NULL, 0);
fullpath = malloc(sizeof(wchar_t) * (wcslen(wcwd) + 32));
wcscpy(fullpath, L"//?/");
wcscat(fullpath, wcwd);
wcscat(fullpath, L"/dir1/file1");
free(wcwd);
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open_w(a, fullpath));
while ((wcwd = wcschr(fullpath, L'\\')) != NULL)
*wcwd = L'/';


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(0, archive_read_disk_can_descend(a));
assertEqualWString(archive_entry_pathname_w(ae), fullpath);
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
free(fullpath);




wcwd = _wgetcwd(NULL, 0);
fullpath = malloc(sizeof(wchar_t) * (wcslen(wcwd) + 32));
wcscpy(fullpath, L"//?/");
wcscat(fullpath, wcwd);
wcscat(fullpath, L"/dir1/*1");
free(wcwd);
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open_w(a, fullpath));
while ((wcwd = wcschr(fullpath, L'\\')) != NULL)
*wcwd = L'/';


wp = wcsrchr(fullpath, L'/');
wcscpy(wp+1, L"file1");
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(0, archive_read_disk_can_descend(a));
assertEqualWString(archive_entry_pathname_w(ae), fullpath);
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);


wcscpy(wp+1, L"sub1");
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(1, archive_read_disk_can_descend(a));
assertEqualWString(archive_entry_pathname_w(ae), fullpath);
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);


assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_descend(a));


wcscpy(wp+1, L"sub1/file1");
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
assertEqualInt(0, archive_read_disk_can_descend(a));
assertEqualWString(archive_entry_pathname_w(ae), fullpath);
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
free(fullpath);

#endif








#if defined(PATH_MAX) && !defined(__GLIBC__)
initial_cwd = getcwd(NULL, PATH_MAX);
#else
initial_cwd = getcwd(NULL, 0);
#endif

assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "dir1"));


file_count = 12;
while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae),
"dir1/sub1/file1") == 0)



break;
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualInt(ARCHIVE_OK, archive_read_free(a));


failure(
"Current working directory does not return to the initial"
"directory");
#if defined(PATH_MAX) && !defined(__GLIBC__)
cwd = getcwd(NULL, PATH_MAX);
#else
cwd = getcwd(NULL, 0);
#endif
assertEqualString(initial_cwd, cwd);
free(initial_cwd);
free(cwd);

archive_entry_free(ae);
}

static void
test_symlink_hybrid(void)
{
struct archive *a;
struct archive_entry *ae;
const void *p;
size_t size;
int64_t offset;
int file_count;

if (!canSymlink()) {
skipping("Can't test symlinks on this filesystem");
return;
}




assertMakeDir("h", 0755);
assertChdir("h");
assertMakeDir("d1", 0755);
assertMakeSymlink("ld1", "d1", 1);
assertMakeFile("d1/file1", 0644, "d1/file1");
assertMakeFile("d1/file2", 0644, "d1/file2");
assertMakeSymlink("d1/link1", "file1", 0);
assertMakeSymlink("d1/linkX", "fileX", 0);
assertMakeSymlink("link2", "d1/file2", 0);
assertMakeSymlink("linkY", "d1/fileY", 0);
assertChdir("..");

assert((ae = archive_entry_new()) != NULL);
assert((a = archive_read_disk_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_set_symlink_hybrid(a));




assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "h/ld1"));
file_count = 5;

while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "h/ld1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae),
"h/ld1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"h/ld1/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file2", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"h/ld1/link1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae),
"h/ld1/linkX") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));




assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "h"));
file_count = 9;

while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "h") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "h/d1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae),
"h/d1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"h/d1/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file2", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae), "h/ld1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae),
"h/d1/link1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae),
"h/d1/linkX") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae),
"h/link2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae),
"h/linkY") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));

assertEqualInt(ARCHIVE_OK, archive_read_free(a));
archive_entry_free(ae);
}

static void
test_symlink_logical(void)
{
struct archive *a;
struct archive_entry *ae;
const void *p;
size_t size;
int64_t offset;
int file_count;

if (!canSymlink()) {
skipping("Can't test symlinks on this filesystem");
return;
}




assertMakeDir("l", 0755);
assertChdir("l");
assertMakeDir("d1", 0755);
assertMakeSymlink("ld1", "d1", 1);
assertMakeFile("d1/file1", 0644, "d1/file1");
assertMakeFile("d1/file2", 0644, "d1/file2");
assertMakeSymlink("d1/link1", "file1", 0);
assertMakeSymlink("d1/linkX", "fileX", 0);
assertMakeSymlink("link2", "d1/file2", 0);
assertMakeSymlink("linkY", "d1/fileY", 0);
assertChdir("..");



assert((a = archive_read_disk_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_set_symlink_logical(a));




assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "l/ld1"));
file_count = 5;

while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
if (strcmp(archive_entry_pathname(ae), "l/ld1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file2", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/link1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/linkX") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));




assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "l"));
file_count = 13;

while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
if (strcmp(archive_entry_pathname(ae), "l") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "l/d1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae),
"l/d1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/d1/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file2", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/d1/link1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/d1/linkX") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae), "l/ld1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/file2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file2", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/link1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/ld1/linkX") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae),
"l/link2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d1/file2", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
} else if (strcmp(archive_entry_pathname(ae),
"l/linkY") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));

assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_symlink_logical_loop(void)
{
struct archive *a;
struct archive_entry *ae;
const void *p;
size_t size;
int64_t offset;
int file_count;

if (!canSymlink()) {
skipping("Can't test symlinks on this filesystem");
return;
}




assertMakeDir("l2", 0755);
assertChdir("l2");
assertMakeDir("d1", 0755);
assertMakeDir("d1/d2", 0755);
assertMakeDir("d1/d2/d3", 0755);
assertMakeDir("d2", 0755);
assertMakeFile("d2/file1", 0644, "d2/file1");
assertMakeSymlink("d1/d2/ld1", "../../d1", 1);
assertMakeSymlink("d1/d2/ld2", "../../d2", 1);
assertChdir("..");

assert((ae = archive_entry_new()) != NULL);
assert((a = archive_read_disk_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_set_symlink_logical(a));




assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "l2/d1"));
file_count = 6;

while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "l2/d1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "l2/d1/d2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "l2/d1/d2/d3") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "l2/d1/d2/ld1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
} else if (strcmp(archive_entry_pathname(ae), "l2/d1/d2/ld2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae),
"l2/d1/d2/ld2/file1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 8);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 8);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "d2/file1", 8);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 8);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));

assertEqualInt(ARCHIVE_OK, archive_read_free(a));
archive_entry_free(ae);
}

static void
test_restore_atime(void)
{
struct archive *a;
struct archive_entry *ae;
const void *p;
size_t size;
int64_t offset;
int file_count;
const char *skip_test_restore_atime;

skip_test_restore_atime = getenv("SKIP_TEST_RESTORE_ATIME");
if (skip_test_restore_atime != NULL) {
skipping("Skipping restore atime tests due to "
"SKIP_TEST_RESTORE_ATIME environment variable");
return;
}
if (!atimeIsUpdated()) {
skipping("Can't test restoring atime on this filesystem");
return;
}

assertMakeDir("at", 0755);
assertMakeFile("at/f1", 0644, "0123456789");
assertMakeFile("at/f2", 0644, "hello world");
assertMakeFile("at/fe", 0644, NULL);
assertUtimes("at/f1", 886600, 0, 886600, 0);
assertUtimes("at/f2", 886611, 0, 886611, 0);
assertUtimes("at/fe", 886611, 0, 886611, 0);
assertUtimes("at", 886622, 0, 886622, 0);
file_count = 4;

assert((ae = archive_entry_new()) != NULL);
assert((a = archive_read_disk_new()) != NULL);




failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "at"));
while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "at") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "at/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
} else if (strcmp(archive_entry_pathname(ae), "at/f2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 11);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 11);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "hello world", 11);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 11);
} else if (strcmp(archive_entry_pathname(ae), "at/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

failure("There must be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));





failure("Atime should be restored");
assertFileAtimeRecent("at/f1");
failure("Atime should be restored");
assertFileAtimeRecent("at/f2");
failure("The atime of a empty file should not be changed");
assertFileAtime("at/fe", 886611, 0);


assertEqualInt(ARCHIVE_OK, archive_read_close(a));




assertUtimes("at/f1", 886600, 0, 886600, 0);
assertUtimes("at/f2", 886611, 0, 886611, 0);
assertUtimes("at/fe", 886611, 0, 886611, 0);
assertUtimes("at", 886622, 0, 886622, 0);
file_count = 4;
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_set_atime_restored(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "at"));

failure("Directory traversals should work as well");
while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "at") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "at/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
} else if (strcmp(archive_entry_pathname(ae), "at/f2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 11);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 11);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "hello world", 11);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 11);
} else if (strcmp(archive_entry_pathname(ae), "at/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

failure("There must be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));

failure("Atime should be restored");
assertFileAtime("at", 886622, 0);
failure("Atime should be restored");
assertFileAtime("at/f1", 886600, 0);
failure("Atime should be restored");
assertFileAtime("at/f2", 886611, 0);
failure("The atime of a empty file should not be changed");
assertFileAtime("at/fe", 886611, 0);


assertEqualInt(ARCHIVE_OK, archive_read_close(a));





assertUtimes("at/f1", 886600, 0, 886600, 0);
assertUtimes("at/f2", 886611, 0, 886611, 0);
assertUtimes("at/fe", 886611, 0, 886611, 0);
assertUtimes("at", 886622, 0, 886622, 0);
file_count = 4;
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_set_atime_restored(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "at"));

failure("Directory traversals should work as well");
while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "at") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "at/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
} else if (strcmp(archive_entry_pathname(ae), "at/f2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 11);
} else if (strcmp(archive_entry_pathname(ae), "at/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

failure("There must be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));

failure("Atime should be restored");
assertFileAtime("at", 886622, 0);
failure("Atime should be restored");
assertFileAtime("at/f1", 886600, 0);
failure("Atime should be restored");
assertFileAtime("at/f2", 886611, 0);
failure("The atime of a empty file should not be changed");
assertFileAtime("at/fe", 886611, 0);

if (!canNodump()) {

assertEqualInt(ARCHIVE_OK, archive_read_free(a));
archive_entry_free(ae);
skipping("Can't test atime with nodump on this filesystem");
return;
}


assertEqualInt(ARCHIVE_OK, archive_read_close(a));





assertSetNodump("at/f1");
assertSetNodump("at/f2");
assertUtimes("at/f1", 886600, 0, 886600, 0);
assertUtimes("at/f2", 886611, 0, 886611, 0);
assertUtimes("at/fe", 886611, 0, 886611, 0);
assertUtimes("at", 886622, 0, 886622, 0);
file_count = 2;
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_set_behavior(a,
ARCHIVE_READDISK_RESTORE_ATIME | ARCHIVE_READDISK_HONOR_NODUMP));
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "at"));

failure("Directory traversals should work as well");
while (file_count--) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "at") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "at/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
if (archive_entry_filetype(ae) == AE_IFDIR) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

failure("There must be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));

failure("Atime should be restored");
assertFileAtime("at", 886622, 0);
failure("Atime should be restored");
assertFileAtime("at/f1", 886600, 0);
failure("Atime should be restored");
assertFileAtime("at/f2", 886611, 0);
failure("The atime of a empty file should not be changed");
assertFileAtime("at/fe", 886611, 0);


assertEqualInt(ARCHIVE_OK, archive_read_free(a));
archive_entry_free(ae);
}

static int
metadata_filter(struct archive *a, void *data, struct archive_entry *ae)
{
(void)data;

failure("CTime should be set");
assertEqualInt(8, archive_entry_ctime_is_set(ae));
failure("MTime should be set");
assertEqualInt(16, archive_entry_mtime_is_set(ae));

if (archive_entry_mtime(ae) < 886611)
return (0);
if (archive_read_disk_can_descend(a)) {

failure("archive_read_disk_can_descend should work"
" in metadata filter");
assertEqualIntA(a, 1, archive_read_disk_can_descend(a));
failure("archive_read_disk_descend should work"
" in metadata filter");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_descend(a));
}
return (1);
}

static void
test_callbacks(void)
{
struct archive *a;
struct archive *m;
struct archive_entry *ae;
const void *p;
size_t size;
int64_t offset;
int file_count;

assertMakeDir("cb", 0755);
assertMakeFile("cb/f1", 0644, "0123456789");
assertMakeFile("cb/f2", 0644, "hello world");
assertMakeFile("cb/fe", 0644, NULL);
assertUtimes("cb/f1", 886600, 0, 886600, 0);
assertUtimes("cb/f2", 886611, 0, 886611, 0);
assertUtimes("cb/fe", 886611, 0, 886611, 0);
assertUtimes("cb", 886622, 0, 886622, 0);

assert((ae = archive_entry_new()) != NULL);
assert((a = archive_read_disk_new()) != NULL);
if (a == NULL) {
archive_entry_free(ae);
return;
}
assert((m = archive_match_new()) != NULL);
if (m == NULL) {
archive_entry_free(ae);
archive_read_free(a);
archive_match_free(m);
return;
}




file_count = 3;
assertEqualIntA(m, ARCHIVE_OK,
archive_match_exclude_pattern(m, "cb/f2"));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_set_matching(a, m, NULL, NULL));
failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "cb"));
while (file_count--) {
archive_entry_clear(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
failure("File 'cb/f2' should be exclueded");
assert(strcmp(archive_entry_pathname(ae), "cb/f2") != 0);
if (strcmp(archive_entry_pathname(ae), "cb") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "cb/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
} else if (strcmp(archive_entry_pathname(ae), "cb/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
if (archive_read_disk_can_descend(a)) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

failure("There should be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));


assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_set_matching(a, NULL, NULL, NULL));




assertUtimes("cb/f1", 886600, 0, 886600, 0);
assertUtimes("cb/f2", 886611, 0, 886611, 0);
assertUtimes("cb/fe", 886611, 0, 886611, 0);
assertUtimes("cb", 886622, 0, 886622, 0);
file_count = 3;
assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_set_metadata_filter_callback(a, metadata_filter,
NULL));
failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "cb"));

while (file_count--) {
archive_entry_clear(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
failure("File 'cb/f1' should be excluded");
assert(strcmp(archive_entry_pathname(ae), "cb/f1") != 0);
if (strcmp(archive_entry_pathname(ae), "cb") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "cb/f2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 11);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 11);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "hello world", 11);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 11);
} else if (strcmp(archive_entry_pathname(ae), "cb/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
}

failure("There should be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_free(a));
assertEqualInt(ARCHIVE_OK, archive_match_free(m));
archive_entry_free(ae);
}

static void
test_nodump(void)
{
struct archive *a;
struct archive_entry *ae;
const void *p;
size_t size;
int64_t offset;
int file_count;

if (!canNodump()) {
skipping("Can't test nodump on this filesystem");
return;
}

assertMakeDir("nd", 0755);
assertMakeFile("nd/f1", 0644, "0123456789");
assertMakeFile("nd/f2", 0644, "hello world");
assertMakeFile("nd/fe", 0644, NULL);
assertSetNodump("nd/f2");
assertUtimes("nd/f1", 886600, 0, 886600, 0);
assertUtimes("nd/f2", 886611, 0, 886611, 0);
assertUtimes("nd/fe", 886611, 0, 886611, 0);
assertUtimes("nd", 886622, 0, 886622, 0);

assert((ae = archive_entry_new()) != NULL);
assert((a = archive_read_disk_new()) != NULL);




failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "nd"));

file_count = 4;
while (file_count--) {
archive_entry_clear(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "nd") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "nd/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
} else if (strcmp(archive_entry_pathname(ae), "nd/f2") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 11);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 11);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "hello world", 11);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 11);
} else if (strcmp(archive_entry_pathname(ae), "nd/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
if (archive_read_disk_can_descend(a)) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

failure("There should be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));




assertUtimes("nd/f1", 886600, 0, 886600, 0);
assertUtimes("nd/f2", 886611, 0, 886611, 0);
assertUtimes("nd/fe", 886611, 0, 886611, 0);
assertUtimes("nd", 886622, 0, 886622, 0);

assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_set_behavior(a,
ARCHIVE_READDISK_RESTORE_ATIME | ARCHIVE_READDISK_HONOR_NODUMP));
failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "nd"));

file_count = 3;
while (file_count--) {
archive_entry_clear(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
failure("File 'nd/f2' should be exclueded");
assert(strcmp(archive_entry_pathname(ae), "nd/f2") != 0);
if (strcmp(archive_entry_pathname(ae), "nd") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
} else if (strcmp(archive_entry_pathname(ae), "nd/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
} else if (strcmp(archive_entry_pathname(ae), "nd/fe") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 0);
}
if (archive_read_disk_can_descend(a)) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}

failure("There should be no entry");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));

failure("Atime should be restored");
assertFileAtime("nd/f2", 886611, 0);


assertEqualInt(ARCHIVE_OK, archive_read_free(a));
archive_entry_free(ae);
}

static void
test_parent(void)
{
struct archive *a;
struct archive_entry *ae;
const void *p;
size_t size;
int64_t offset;
int file_count;
int match_count;
int r;

assertMakeDir("lock", 0311);
assertMakeDir("lock/dir1", 0755);
assertMakeFile("lock/dir1/f1", 0644, "0123456789");
assertMakeDir("lock/lock2", 0311);
assertMakeDir("lock/lock2/dir1", 0755);
assertMakeFile("lock/lock2/dir1/f1", 0644, "0123456789");

assert((ae = archive_entry_new()) != NULL);
assert((a = archive_read_disk_new()) != NULL);




assertChdir("lock/dir1");

failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "."));

file_count = 2;
match_count = 0;
while (file_count--) {
archive_entry_clear(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), ".") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
++match_count;
} else if (strcmp(archive_entry_pathname(ae), "./f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
++match_count;
}
if (archive_read_disk_can_descend(a)) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}
failure("Did not match expected filenames");
assertEqualInt(match_count, 2);





failure("There should be no entry and no error");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));

assertChdir("../..");




failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "lock/dir1"));

file_count = 2;
match_count = 0;
while (file_count--) {
archive_entry_clear(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "lock/dir1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
++match_count;
} else if (strcmp(archive_entry_pathname(ae), "lock/dir1/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
++match_count;
}
if (archive_read_disk_can_descend(a)) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}
failure("Did not match expected filenames");
assertEqualInt(match_count, 2);





failure("There should be no entry and no error");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));




failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "lock/dir1/."));

file_count = 2;
match_count = 0;
while (file_count--) {
archive_entry_clear(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae), "lock/dir1/.") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
++match_count;
} else if (strcmp(archive_entry_pathname(ae), "lock/dir1/./f1") == 0) {
assertEqualInt(archive_entry_filetype(ae), AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
++match_count;
}
if (archive_read_disk_can_descend(a)) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}
failure("Did not match expected filenames");
assertEqualInt(match_count, 2);





failure("There should be no entry and no error");
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));











assertChdir("lock");

failure("Directory traversals should work as well");
assertEqualIntA(a, ARCHIVE_OK, archive_read_disk_open(a, "lock2/dir1"));

archive_entry_clear(ae);
r = archive_read_next_header2(a, ae);
if (r == ARCHIVE_FAILED) {
#if defined(O_PATH) || defined(O_SEARCH) || (defined(__FreeBSD__) && defined(O_EXEC))

assertEqualIntA(a, ARCHIVE_OK, r);
#endif

archive_read_close(a);
} else {
file_count = 2;
match_count = 0;
while (file_count--) {
if (file_count == 0)
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header2(a, ae));
if (strcmp(archive_entry_pathname(ae),
"lock2/dir1") == 0) {
assertEqualInt(archive_entry_filetype(ae),
AE_IFDIR);
++match_count;
} else if (strcmp(archive_entry_pathname(ae),
"lock2/dir1/f1") == 0) {
assertEqualInt(archive_entry_filetype(ae),
AE_IFREG);
assertEqualInt(archive_entry_size(ae), 10);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &p, &size,
&offset));
assertEqualInt((int)size, 10);
assertEqualInt((int)offset, 0);
assertEqualMem(p, "0123456789", 10);
assertEqualInt(ARCHIVE_EOF,
archive_read_data_block(a, &p, &size,
&offset));
assertEqualInt((int)size, 0);
assertEqualInt((int)offset, 10);
++match_count;
}
if (archive_read_disk_can_descend(a)) {

assertEqualIntA(a, ARCHIVE_OK,
archive_read_disk_descend(a));
}
}
failure("Did not match expected filenames");
assertEqualInt(match_count, 2);





failure("There should be no entry and no error");
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_next_header2(a, ae));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
}

assertChdir("..");
assertChmod("lock", 0755);
assertChmod("lock/lock2", 0755);


assertEqualInt(ARCHIVE_OK, archive_read_free(a));
archive_entry_free(ae);
}

DEFINE_TEST(test_read_disk_directory_traversals)
{

test_basic();

test_symlink_hybrid();

test_symlink_logical();

test_symlink_logical_loop();

test_restore_atime();

test_callbacks();

test_nodump();

test_parent();
}
