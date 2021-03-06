


























#if !defined(TEST_COMMON_H)
#define TEST_COMMON_H





#if defined(HAVE_CONFIG_H)

#include "config.h"
#elif defined(__FreeBSD__)

#include "config_freebsd.h"
#elif defined(__NetBSD__)

#include "config_netbsd.h"
#elif defined(_WIN32) && !defined(__CYGWIN__)

#include "config_windows.h"
#else

#error Oops: No config.h and no pre-built configuration in test.h.
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if HAVE_DIRENT_H
#include <dirent.h>
#endif
#if defined(HAVE_DIRECT_H)
#include <direct.h>
#define dirent direct
#endif
#include <errno.h>
#include <fcntl.h>
#if defined(HAVE_IO_H)
#include <io.h>
#endif
#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <wchar.h>
#if defined(HAVE_ACL_LIBACL_H)
#include <acl/libacl.h>
#endif
#if defined(HAVE_SYS_ACL_H)
#include <sys/acl.h>
#endif
#if defined(HAVE_SYS_RICHACL_H)
#include <sys/richacl.h>
#endif
#if defined(HAVE_WINDOWS_H)
#define NOCRYPT
#include <windows.h>
#include <winioctl.h>
#endif








#if defined(_WIN32) && !defined(__CYGWIN__)
#if !defined(__BORLANDC__)
#undef chdir
#define chdir _chdir
#define strdup _strdup
#endif
#endif


#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf sprintf_s
#endif

#if defined(__BORLANDC__)
#pragma warn -8068
#endif


#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7))

#if defined(__MINGW_PRINTF_FORMAT)
#define __LA_PRINTF_FORMAT __MINGW_PRINTF_FORMAT
#else
#define __LA_PRINTF_FORMAT __printf__
#endif
#define __LA_PRINTFLIKE(f,a) __attribute__((__format__(__LA_PRINTF_FORMAT, f, a)))
#else
#define __LA_PRINTFLIKE(f,a)
#endif


#if defined(__HAIKU__) || defined(__QNXNTO__)

#include <stdint.h>
#endif


#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif


#if !defined(__FBSDID)
#define __FBSDID(a) struct _undefined_hack
#endif

#if !defined(O_BINARY)
#define O_BINARY 0
#endif

#if !defined(__LIBARCHIVE_TEST_COMMON)
#define __LIBARCHIVE_TEST_COMMON
#endif

#include "archive_platform_acl.h"
#define ARCHIVE_TEST_ACL_TYPE_POSIX1E 1
#define ARCHIVE_TEST_ACL_TYPE_NFS4 2

#include "archive_platform_xattr.h"




#undef DEFINE_TEST
#define DEFINE_TEST(name) void name(void); void name(void)


#define assert(e) assertion_assert(__FILE__, __LINE__, (e), #e, NULL)

#define assertChdir(path) assertion_chdir(__FILE__, __LINE__, path)


#define assertChmod(pathname, mode) assertion_chmod(__FILE__, __LINE__, pathname, mode)


#define assertEqualFflags(patha, pathb) assertion_compare_fflags(__FILE__, __LINE__, patha, pathb, 0)


#define assertEqualInt(v1,v2) assertion_equal_int(__FILE__, __LINE__, (v1), #v1, (v2), #v2, NULL)


#define assertEqualString(v1,v2) assertion_equal_string(__FILE__, __LINE__, (v1), #v1, (v2), #v2, NULL, 0)

#define assertEqualUTF8String(v1,v2) assertion_equal_string(__FILE__, __LINE__, (v1), #v1, (v2), #v2, NULL, 1)


#define assertEqualWString(v1,v2) assertion_equal_wstring(__FILE__, __LINE__, (v1), #v1, (v2), #v2, NULL)


#define assertEqualMem(v1, v2, l) assertion_equal_mem(__FILE__, __LINE__, (v1), #v1, (v2), #v2, (l), #l, NULL)


#define assertMemoryFilledWith(v1, l, b) assertion_memory_filled_with(__FILE__, __LINE__, (v1), #v1, (l), #l, (b), #b, NULL)


#define assertEqualFile(f1, f2) assertion_equal_file(__FILE__, __LINE__, (f1), (f2))


#define assertEmptyFile(pathname) assertion_empty_file(__FILE__, __LINE__, (pathname))


#define assertNonEmptyFile(pathname) assertion_non_empty_file(__FILE__, __LINE__, (pathname))

#define assertFileAtime(pathname, sec, nsec) assertion_file_atime(__FILE__, __LINE__, pathname, sec, nsec)

#define assertFileAtimeRecent(pathname) assertion_file_atime_recent(__FILE__, __LINE__, pathname)

#define assertFileBirthtime(pathname, sec, nsec) assertion_file_birthtime(__FILE__, __LINE__, pathname, sec, nsec)

#define assertFileBirthtimeRecent(pathname) assertion_file_birthtime_recent(__FILE__, __LINE__, pathname)


#define assertFileExists(pathname) assertion_file_exists(__FILE__, __LINE__, pathname)


#define assertFileNotExists(pathname) assertion_file_not_exists(__FILE__, __LINE__, pathname)


#define assertFileContents(data, data_size, pathname) assertion_file_contents(__FILE__, __LINE__, data, data_size, pathname)


#define assertFileContainsNoInvalidStrings(pathname, strings) assertion_file_contains_no_invalid_strings(__FILE__, __LINE__, pathname, strings)

#define assertFileMtime(pathname, sec, nsec) assertion_file_mtime(__FILE__, __LINE__, pathname, sec, nsec)

#define assertFileMtimeRecent(pathname) assertion_file_mtime_recent(__FILE__, __LINE__, pathname)

#define assertFileNLinks(pathname, nlinks) assertion_file_nlinks(__FILE__, __LINE__, pathname, nlinks)

#define assertFileSize(pathname, size) assertion_file_size(__FILE__, __LINE__, pathname, size)

#define assertFileMode(pathname, mode) assertion_file_mode(__FILE__, __LINE__, pathname, mode)

#define assertTextFileContents(text, pathname) assertion_text_file_contents(__FILE__, __LINE__, text, pathname)

#define assertFileContainsLinesAnyOrder(pathname, lines) assertion_file_contains_lines_any_order(__FILE__, __LINE__, pathname, lines)

#define assertIsDir(pathname, mode) assertion_is_dir(__FILE__, __LINE__, pathname, mode)

#define assertIsHardlink(path1, path2) assertion_is_hardlink(__FILE__, __LINE__, path1, path2)

#define assertIsNotHardlink(path1, path2) assertion_is_not_hardlink(__FILE__, __LINE__, path1, path2)

#define assertIsReg(pathname, mode) assertion_is_reg(__FILE__, __LINE__, pathname, mode)

#define assertIsSymlink(pathname, contents, isdir) assertion_is_symlink(__FILE__, __LINE__, pathname, contents, isdir)


#define assertMakeDir(dirname, mode) assertion_make_dir(__FILE__, __LINE__, dirname, mode)

#define assertMakeFile(path, mode, contents) assertion_make_file(__FILE__, __LINE__, path, mode, -1, contents)

#define assertMakeBinFile(path, mode, csize, contents) assertion_make_file(__FILE__, __LINE__, path, mode, csize, contents)

#define assertMakeHardlink(newfile, oldfile) assertion_make_hardlink(__FILE__, __LINE__, newfile, oldfile)

#define assertMakeSymlink(newfile, linkto, targetIsDir) assertion_make_symlink(__FILE__, __LINE__, newfile, linkto, targetIsDir)

#define assertSetNodump(path) assertion_set_nodump(__FILE__, __LINE__, path)

#define assertUmask(mask) assertion_umask(__FILE__, __LINE__, mask)


#define assertUnequalFflags(patha, pathb) assertion_compare_fflags(__FILE__, __LINE__, patha, pathb, 1)

#define assertUtimes(pathname, atime, atime_nsec, mtime, mtime_nsec) assertion_utimes(__FILE__, __LINE__, pathname, atime, atime_nsec, mtime, mtime_nsec)

#if !defined(PROGRAM)
#define assertEntrySetAcls(entry, acls, count) assertion_entry_set_acls(__FILE__, __LINE__, entry, acls, count)

#define assertEntryCompareAcls(entry, acls, count, type, mode) assertion_entry_compare_acls(__FILE__, __LINE__, entry, acls, count, type, mode)

#endif







#define skipping skipping_setup(__FILE__, __LINE__);test_skipping



void failure(const char *fmt, ...) __LA_PRINTFLIKE(1, 2);
int assertion_assert(const char *, int, int, const char *, void *);
int assertion_chdir(const char *, int, const char *);
int assertion_chmod(const char *, int, const char *, int);
int assertion_compare_fflags(const char *, int, const char *, const char *,
int);
int assertion_empty_file(const char *, int, const char *);
int assertion_equal_file(const char *, int, const char *, const char *);
int assertion_equal_int(const char *, int, long long, const char *, long long, const char *, void *);
int assertion_equal_mem(const char *, int, const void *, const char *, const void *, const char *, size_t, const char *, void *);
int assertion_memory_filled_with(const char *, int, const void *, const char *, size_t, const char *, char, const char *, void *);
int assertion_equal_string(const char *, int, const char *v1, const char *, const char *v2, const char *, void *, int);
int assertion_equal_wstring(const char *, int, const wchar_t *v1, const char *, const wchar_t *v2, const char *, void *);
int assertion_file_atime(const char *, int, const char *, long, long);
int assertion_file_atime_recent(const char *, int, const char *);
int assertion_file_birthtime(const char *, int, const char *, long, long);
int assertion_file_birthtime_recent(const char *, int, const char *);
int assertion_file_contains_lines_any_order(const char *, int, const char *, const char **);
int assertion_file_contains_no_invalid_strings(const char *, int, const char *, const char **);
int assertion_file_contents(const char *, int, const void *, int, const char *);
int assertion_file_exists(const char *, int, const char *);
int assertion_file_mode(const char *, int, const char *, int);
int assertion_file_mtime(const char *, int, const char *, long, long);
int assertion_file_mtime_recent(const char *, int, const char *);
int assertion_file_nlinks(const char *, int, const char *, int);
int assertion_file_not_exists(const char *, int, const char *);
int assertion_file_size(const char *, int, const char *, long);
int assertion_is_dir(const char *, int, const char *, int);
int assertion_is_hardlink(const char *, int, const char *, const char *);
int assertion_is_not_hardlink(const char *, int, const char *, const char *);
int assertion_is_reg(const char *, int, const char *, int);
int assertion_is_symlink(const char *, int, const char *, const char *, int);
int assertion_make_dir(const char *, int, const char *, int);
int assertion_make_file(const char *, int, const char *, int, int, const void *);
int assertion_make_hardlink(const char *, int, const char *newpath, const char *);
int assertion_make_symlink(const char *, int, const char *newpath, const char *, int);
int assertion_non_empty_file(const char *, int, const char *);
int assertion_set_nodump(const char *, int, const char *);
int assertion_text_file_contents(const char *, int, const char *buff, const char *f);
int assertion_umask(const char *, int, int);
int assertion_utimes(const char *, int, const char *, long, long, long, long );
int assertion_version(const char*, int, const char *, const char *);

void skipping_setup(const char *, int);
void test_skipping(const char *fmt, ...) __LA_PRINTFLIKE(1, 2);


int systemf(const char *fmt, ...) __LA_PRINTFLIKE(1, 2);


void sleepUntilAfter(time_t);


int canSymlink(void);


int canBzip2(void);


int canGrzip(void);


int canGzip(void);


int canRunCommand(const char *);


int canLrzip(void);


int canLz4(void);


int canZstd(void);


int canLzip(void);


int canLzma(void);


int canLzop(void);


int canXz(void);


int canNodump(void);


int setTestAcl(const char *path);


void *getXattr(const char *, const char *, size_t *);


int setXattr(const char *, const char *, const void *, size_t);


int is_LargeInode(const char *);

#if ARCHIVE_ACL_SUNOS

void *sunacl_get(int cmd, int *aclcnt, int fd, const char *path);
#endif



char *slurpfile(size_t *, const char *fmt, ...) __LA_PRINTFLIKE(2, 3);


void dumpfile(const char *filename, void *, size_t);


void extract_reference_file(const char *);

void copy_reference_file(const char *);




void extract_reference_files(const char **);


mode_t umasked(mode_t expected_mode);


extern const char *testworkdir;

#if !defined(PROGRAM)




#include "archive.h"
#include "archive_entry.h"


struct archive_test_acl_t {
int type;
int permset;
int tag;
int qual;
const char *name;
};


int assertion_entry_set_acls(const char *, int, struct archive_entry *,
struct archive_test_acl_t *, int);


int assertion_entry_compare_acls(const char *, int, struct archive_entry *,
struct archive_test_acl_t *, int, int, int);


int read_open_memory(struct archive *, const void *, size_t, size_t);

int read_open_memory_minimal(struct archive *, const void *, size_t, size_t);

int read_open_memory_seek(struct archive *, const void *, size_t, size_t);


#define assertA(e) assertion_assert(__FILE__, __LINE__, (e), #e, (a))
#define assertEqualIntA(a,v1,v2) assertion_equal_int(__FILE__, __LINE__, (v1), #v1, (v2), #v2, (a))

#define assertEqualStringA(a,v1,v2) assertion_equal_string(__FILE__, __LINE__, (v1), #v1, (v2), #v2, (a), 0)


#else





extern const char *testprogfile;


extern const char *testprog;

void assertVersion(const char *prog, const char *base);

#endif

#if defined(USE_DMALLOC)
#include <dmalloc.h>
#endif

#endif
