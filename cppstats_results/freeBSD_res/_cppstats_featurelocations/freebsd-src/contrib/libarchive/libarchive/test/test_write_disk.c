























#include "test.h"
__FBSDID("$FreeBSD$");

#define UMASK 022







#define MODE_MASK 0777777

static void create(struct archive_entry *ae, const char *msg)
{
struct archive *ad;
struct stat st;


assert((ad = archive_write_disk_new()) != NULL);
failure("%s", msg);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
assertEqualInt(0, archive_write_free(ad));


assert(0 == stat(archive_entry_pathname(ae), &st));
failure("%s", msg);

#if !defined(_WIN32) || defined(__CYGWIN__)


if (archive_entry_filetype(ae) == AE_IFDIR)
st.st_mode &= ~S_ISGID;
assertEqualInt(st.st_mode & MODE_MASK,
archive_entry_mode(ae) & ~UMASK & MODE_MASK);
#endif
}

static void create_reg_file(struct archive_entry *ae, const char *msg)
{
static const char data[]="abcdefghijklmnopqrstuvwxyz";
struct archive *ad;


assert((ad = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(ad, ARCHIVE_EXTRACT_TIME);
failure("%s", msg);

















archive_entry_set_size(ae, sizeof(data));
archive_entry_set_mtime(ae, 123456789, 0);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(sizeof(data), archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
assertEqualInt(0, archive_write_free(ad));


assertIsReg(archive_entry_pathname(ae), archive_entry_mode(ae) & 0777);
assertFileSize(archive_entry_pathname(ae), sizeof(data));

assertFileMtime(archive_entry_pathname(ae), 123456789, 0);
failure("No atime given, so atime should get set to current time");
assertFileAtimeRecent(archive_entry_pathname(ae));
}

static void create_reg_file2(struct archive_entry *ae, const char *msg)
{
const int datasize = 100000;
char *data;
struct archive *ad;
int i;

data = malloc(datasize);
for (i = 0; i < datasize; i++)
data[i] = (char)(i % 256);


assert((ad = archive_write_disk_new()) != NULL);
failure("%s", msg);




archive_entry_set_size(ae, datasize);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
for (i = 0; i < datasize - 999; i += 1000) {
assertEqualIntA(ad, ARCHIVE_OK,
archive_write_data_block(ad, data + i, 1000, i));
}
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
assertEqualInt(0, archive_write_free(ad));


assertIsReg(archive_entry_pathname(ae), archive_entry_mode(ae) & 0777);
assertFileSize(archive_entry_pathname(ae), i);
assertFileContents(data, datasize, archive_entry_pathname(ae));
free(data);
}

static void create_reg_file3(struct archive_entry *ae, const char *msg)
{
static const char data[]="abcdefghijklmnopqrstuvwxyz";
struct archive *ad;
struct stat st;


assert((ad = archive_write_disk_new()) != NULL);
failure("%s", msg);

archive_entry_set_size(ae, 5);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(5, archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
assertEqualInt(0, archive_write_free(ad));


assert(0 == stat(archive_entry_pathname(ae), &st));
failure("st.st_mode=%o archive_entry_mode(ae)=%o",
st.st_mode, archive_entry_mode(ae));
#if !defined(_WIN32) || defined(__CYGWIN__)
assertEqualInt(st.st_mode, (archive_entry_mode(ae) & ~UMASK));
#endif
assertEqualInt(st.st_size, 5);
}


static void create_reg_file4(struct archive_entry *ae, const char *msg)
{
static const char data[]="abcdefghijklmnopqrstuvwxyz";
struct archive *ad;
struct stat st;


assert((ad = archive_write_disk_new()) != NULL);

assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(ARCHIVE_OK,
archive_write_data_block(ad, data, sizeof(data), 0));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
assertEqualInt(0, archive_write_free(ad));


assert(0 == stat(archive_entry_pathname(ae), &st));
failure("st.st_mode=%o archive_entry_mode(ae)=%o",
st.st_mode, archive_entry_mode(ae));
#if !defined(_WIN32) || defined(__CYGWIN__)
assertEqualInt(st.st_mode, (archive_entry_mode(ae) & ~UMASK));
#endif
failure("%s", msg);
assertEqualInt(st.st_size, sizeof(data));
}

#if defined(_WIN32) && !defined(__CYGWIN__)
static void create_reg_file_win(struct archive_entry *ae, const char *msg)
{
static const char data[]="abcdefghijklmnopqrstuvwxyz";
struct archive *ad;
struct _stat st;
wchar_t *p, *fname;
size_t l;


assert((ad = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(ad, ARCHIVE_EXTRACT_TIME);
failure("%s", msg);
archive_entry_set_size(ae, sizeof(data));
archive_entry_set_mtime(ae, 123456789, 0);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(sizeof(data), archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
assertEqualInt(0, archive_write_free(ad));


l = wcslen(archive_entry_pathname_w(ae));
fname = malloc((l + 1) * sizeof(wchar_t));
assert(NULL != fname);
wcscpy(fname, archive_entry_pathname_w(ae));
p = fname;


if (((p[0] >= L'a' && p[0] <= L'z') ||
(p[0] >= L'A' && p[0] <= L'Z')) &&
p[1] == L':' && p[2] == L'\\') {
p += 3;
}

for (; *p != L'\0'; p++)
if (*p == L':' || *p == L'*' || *p == L'?' ||
*p == L'"' || *p == L'<' || *p == L'>' || *p == L'|')
*p = '_';
assert(0 == _wstat(fname, &st));
failure("st.st_mode=%o archive_entry_mode(ae)=%o",
st.st_mode, archive_entry_mode(ae));
assertEqualInt(st.st_size, sizeof(data));
free(fname);
}
#endif

DEFINE_TEST(test_write_disk)
{
struct archive_entry *ae;
#if defined(_WIN32) && !defined(__CYGWIN__)
wchar_t *fullpath;
DWORD l;
#endif


assertUmask(UMASK);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file(ae, "Test creating a regular file");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file2");
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file2(ae, "Test creating another regular file");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file3");
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file3(ae, "Regular file with size restriction");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file3");
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file4(ae, "Regular file with unspecified size");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0724);
create(ae, "Test creating a file over an existing file.");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir");
archive_entry_set_mode(ae, S_IFDIR | 0555);
create(ae, "Test creating a regular dir.");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFDIR | 0742);
create(ae, "Test creating a dir over an existing file.");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0744);
create(ae, "Test creating a file over an existing dir.");
archive_entry_free(ae);

#if defined(_WIN32) && !defined(__CYGWIN__)

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(ae, L"f:i*l?e\"f<i>l|e");
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file_win(ae, "Test creating a regular file"
" with unusable characters in its file name");
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(ae, L"d:i*r?e\"c<t>o|ry/file1");
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file_win(ae, "Test creating a regular file"
" with unusable characters in its file name");
archive_entry_free(ae);


assert((l = GetCurrentDirectoryW(0, NULL)) != 0);
assert((fullpath = malloc((l + 20) * sizeof(wchar_t))) != NULL);
assert((l = GetCurrentDirectoryW(l, fullpath)) != 0);
wcscat(fullpath, L"\\f:i*l?e\"f<i>l|e");
assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(ae, fullpath);
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file_win(ae, "Test creating a regular file"
" with unusable characters in its file name");
archive_entry_free(ae);
free(fullpath);


assert((l = GetCurrentDirectoryW(0, NULL)) != 0);
assert((fullpath = malloc((l + 30) * sizeof(wchar_t))) != NULL);
assert((l = GetCurrentDirectoryW(l, fullpath)) != 0);
wcscat(fullpath, L"\\d:i*r?e\"c<t>o|ry/file1");
assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(ae, fullpath);
archive_entry_set_mode(ae, S_IFREG | 0755);
create_reg_file_win(ae, "Test creating a regular file"
" with unusable characters in its file name");
archive_entry_free(ae);
free(fullpath);
#endif
}
