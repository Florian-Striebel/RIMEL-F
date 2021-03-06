
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_IO_H)
#include <io.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#include <sys/disk.h>
#elif defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/disklabel.h>
#include <sys/dkio.h>
#elif defined(__DragonFly__)
#include <sys/diskslice.h>
#endif

#include "archive.h"
#include "archive_private.h"
#include "archive_string.h"

#if !defined(O_BINARY)
#define O_BINARY 0
#endif
#if !defined(O_CLOEXEC)
#define O_CLOEXEC 0
#endif

struct read_file_data {
int fd;
size_t block_size;
void *buffer;
mode_t st_mode;
char use_lseek;
enum fnt_e { FNT_STDIN, FNT_MBS, FNT_WCS } filename_type;
union {
char m[1];
wchar_t w[1];
} filename;
};

static int file_open(struct archive *, void *);
static int file_close(struct archive *, void *);
static int file_close2(struct archive *, void *);
static int file_switch(struct archive *, void *, void *);
static ssize_t file_read(struct archive *, void *, const void **buff);
static int64_t file_seek(struct archive *, void *, int64_t request, int);
static int64_t file_skip(struct archive *, void *, int64_t request);
static int64_t file_skip_lseek(struct archive *, void *, int64_t request);

int
archive_read_open_file(struct archive *a, const char *filename,
size_t block_size)
{
return (archive_read_open_filename(a, filename, block_size));
}

int
archive_read_open_filename(struct archive *a, const char *filename,
size_t block_size)
{
const char *filenames[2];
filenames[0] = filename;
filenames[1] = NULL;
return archive_read_open_filenames(a, filenames, block_size);
}

int
archive_read_open_filenames(struct archive *a, const char **filenames,
size_t block_size)
{
struct read_file_data *mine;
const char *filename = NULL;
if (filenames)
filename = *(filenames++);

archive_clear_error(a);
do
{
if (filename == NULL)
filename = "";
mine = (struct read_file_data *)calloc(1,
sizeof(*mine) + strlen(filename));
if (mine == NULL)
goto no_memory;
strcpy(mine->filename.m, filename);
mine->block_size = block_size;
mine->fd = -1;
mine->buffer = NULL;
mine->st_mode = mine->use_lseek = 0;
if (filename == NULL || filename[0] == '\0') {
mine->filename_type = FNT_STDIN;
} else
mine->filename_type = FNT_MBS;
if (archive_read_append_callback_data(a, mine) != (ARCHIVE_OK))
return (ARCHIVE_FATAL);
if (filenames == NULL)
break;
filename = *(filenames++);
} while (filename != NULL && filename[0] != '\0');
archive_read_set_open_callback(a, file_open);
archive_read_set_read_callback(a, file_read);
archive_read_set_skip_callback(a, file_skip);
archive_read_set_close_callback(a, file_close);
archive_read_set_switch_callback(a, file_switch);
archive_read_set_seek_callback(a, file_seek);

return (archive_read_open1(a));
no_memory:
archive_set_error(a, ENOMEM, "No memory");
return (ARCHIVE_FATAL);
}

int
archive_read_open_filename_w(struct archive *a, const wchar_t *wfilename,
size_t block_size)
{
struct read_file_data *mine = (struct read_file_data *)calloc(1,
sizeof(*mine) + wcslen(wfilename) * sizeof(wchar_t));
if (!mine)
{
archive_set_error(a, ENOMEM, "No memory");
return (ARCHIVE_FATAL);
}
mine->fd = -1;
mine->block_size = block_size;

if (wfilename == NULL || wfilename[0] == L'\0') {
mine->filename_type = FNT_STDIN;
} else {
#if defined(_WIN32) && !defined(__CYGWIN__)
mine->filename_type = FNT_WCS;
wcscpy(mine->filename.w, wfilename);
#else





struct archive_string fn;

archive_string_init(&fn);
if (archive_string_append_from_wcs(&fn, wfilename,
wcslen(wfilename)) != 0) {
if (errno == ENOMEM)
archive_set_error(a, errno,
"Can't allocate memory");
else
archive_set_error(a, EINVAL,
"Failed to convert a wide-character"
" filename to a multi-byte filename");
archive_string_free(&fn);
free(mine);
return (ARCHIVE_FATAL);
}
mine->filename_type = FNT_MBS;
strcpy(mine->filename.m, fn.s);
archive_string_free(&fn);
#endif
}
if (archive_read_append_callback_data(a, mine) != (ARCHIVE_OK))
return (ARCHIVE_FATAL);
archive_read_set_open_callback(a, file_open);
archive_read_set_read_callback(a, file_read);
archive_read_set_skip_callback(a, file_skip);
archive_read_set_close_callback(a, file_close);
archive_read_set_switch_callback(a, file_switch);
archive_read_set_seek_callback(a, file_seek);

return (archive_read_open1(a));
}

static int
file_open(struct archive *a, void *client_data)
{
struct stat st;
struct read_file_data *mine = (struct read_file_data *)client_data;
void *buffer;
const char *filename = NULL;
#if defined(_WIN32) && !defined(__CYGWIN__)
const wchar_t *wfilename = NULL;
#endif
int fd = -1;
int is_disk_like = 0;
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
off_t mediasize = 0;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
struct disklabel dl;
#elif defined(__DragonFly__)
struct partinfo pi;
#endif

archive_clear_error(a);
if (mine->filename_type == FNT_STDIN) {









fd = 0;
#if defined(__CYGWIN__) || defined(_WIN32)
setmode(0, O_BINARY);
#endif
filename = "";
} else if (mine->filename_type == FNT_MBS) {
filename = mine->filename.m;
fd = open(filename, O_RDONLY | O_BINARY | O_CLOEXEC);
__archive_ensure_cloexec_flag(fd);
if (fd < 0) {
archive_set_error(a, errno,
"Failed to open '%s'", filename);
return (ARCHIVE_FATAL);
}
} else {
#if defined(_WIN32) && !defined(__CYGWIN__)
wfilename = mine->filename.w;
fd = _wopen(wfilename, O_RDONLY | O_BINARY);
if (fd < 0 && errno == ENOENT) {
wchar_t *fullpath;
fullpath = __la_win_permissive_name_w(wfilename);
if (fullpath != NULL) {
fd = _wopen(fullpath, O_RDONLY | O_BINARY);
free(fullpath);
}
}
if (fd < 0) {
archive_set_error(a, errno,
"Failed to open '%S'", wfilename);
return (ARCHIVE_FATAL);
}
#else
archive_set_error(a, ARCHIVE_ERRNO_MISC,
"Unexpedted operation in archive_read_open_filename");
goto fail;
#endif
}
if (fstat(fd, &st) != 0) {
#if defined(_WIN32) && !defined(__CYGWIN__)
if (mine->filename_type == FNT_WCS)
archive_set_error(a, errno, "Can't stat '%S'",
wfilename);
else
#endif
archive_set_error(a, errno, "Can't stat '%s'",
filename);
goto fail;
}


















if (S_ISREG(st.st_mode)) {

archive_read_extract_set_skip_file(a, st.st_dev, st.st_ino);

is_disk_like = 1;
}
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)

else if (S_ISCHR(st.st_mode) &&
ioctl(fd, DIOCGMEDIASIZE, &mediasize) == 0 &&
mediasize > 0) {
is_disk_like = 1;
}
#elif defined(__NetBSD__) || defined(__OpenBSD__)

else if ((S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode)) &&
ioctl(fd, DIOCGDINFO, &dl) == 0 &&
dl.d_partitions[DISKPART(st.st_rdev)].p_size > 0) {
is_disk_like = 1;
}
#elif defined(__DragonFly__)

else if (S_ISCHR(st.st_mode) &&
ioctl(fd, DIOCGPART, &pi) == 0 &&
pi.media_size > 0) {
is_disk_like = 1;
}
#elif defined(__linux__)

else if (S_ISBLK(st.st_mode) &&
lseek(fd, 0, SEEK_CUR) == 0 &&
lseek(fd, 0, SEEK_SET) == 0 &&
lseek(fd, 0, SEEK_END) > 0 &&
lseek(fd, 0, SEEK_SET) == 0) {
is_disk_like = 1;
}
#endif




if (is_disk_like) {
size_t new_block_size = 64 * 1024;
while (new_block_size < mine->block_size
&& new_block_size < 64 * 1024 * 1024)
new_block_size *= 2;
mine->block_size = new_block_size;
}
buffer = malloc(mine->block_size);
if (buffer == NULL) {
archive_set_error(a, ENOMEM, "No memory");
goto fail;
}
mine->buffer = buffer;
mine->fd = fd;

mine->st_mode = st.st_mode;


if (is_disk_like)
mine->use_lseek = 1;

return (ARCHIVE_OK);
fail:




if (fd != -1 && fd != 0)
close(fd);
return (ARCHIVE_FATAL);
}

static ssize_t
file_read(struct archive *a, void *client_data, const void **buff)
{
struct read_file_data *mine = (struct read_file_data *)client_data;
ssize_t bytes_read;














*buff = mine->buffer;
for (;;) {
bytes_read = read(mine->fd, mine->buffer, mine->block_size);
if (bytes_read < 0) {
if (errno == EINTR)
continue;
else if (mine->filename_type == FNT_STDIN)
archive_set_error(a, errno,
"Error reading stdin");
else if (mine->filename_type == FNT_MBS)
archive_set_error(a, errno,
"Error reading '%s'", mine->filename.m);
else
archive_set_error(a, errno,
"Error reading '%S'", mine->filename.w);
}
return (bytes_read);
}
}





















static int64_t
file_skip_lseek(struct archive *a, void *client_data, int64_t request)
{
struct read_file_data *mine = (struct read_file_data *)client_data;
#if defined(_WIN32) && !defined(__CYGWIN__)

int64_t old_offset, new_offset;
#else
off_t old_offset, new_offset;
#endif








if ((old_offset = lseek(mine->fd, 0, SEEK_CUR)) >= 0 &&
(new_offset = lseek(mine->fd, request, SEEK_CUR)) >= 0)
return (new_offset - old_offset);


mine->use_lseek = 0;


if (errno == ESPIPE)
return (0);


if (mine->filename_type == FNT_STDIN)
archive_set_error(a, errno, "Error seeking in stdin");
else if (mine->filename_type == FNT_MBS)
archive_set_error(a, errno, "Error seeking in '%s'",
mine->filename.m);
else
archive_set_error(a, errno, "Error seeking in '%S'",
mine->filename.w);
return (-1);
}







static int64_t
file_skip(struct archive *a, void *client_data, int64_t request)
{
struct read_file_data *mine = (struct read_file_data *)client_data;


if (mine->use_lseek)
return (file_skip_lseek(a, client_data, request));


return (0);
}




static int64_t
file_seek(struct archive *a, void *client_data, int64_t request, int whence)
{
struct read_file_data *mine = (struct read_file_data *)client_data;
int64_t r;



r = lseek(mine->fd, request, whence);
if (r >= 0)
return r;


if (mine->filename_type == FNT_STDIN)
archive_set_error(a, errno, "Error seeking in stdin");
else if (mine->filename_type == FNT_MBS)
archive_set_error(a, errno, "Error seeking in '%s'",
mine->filename.m);
else
archive_set_error(a, errno, "Error seeking in '%S'",
mine->filename.w);
return (ARCHIVE_FATAL);
}

static int
file_close2(struct archive *a, void *client_data)
{
struct read_file_data *mine = (struct read_file_data *)client_data;

(void)a;


if (mine->fd >= 0) {











if (!S_ISREG(mine->st_mode)
&& !S_ISCHR(mine->st_mode)
&& !S_ISBLK(mine->st_mode)) {
ssize_t bytesRead;
do {
bytesRead = read(mine->fd, mine->buffer,
mine->block_size);
} while (bytesRead > 0);
}

if (mine->filename_type != FNT_STDIN)
close(mine->fd);
}
free(mine->buffer);
mine->buffer = NULL;
mine->fd = -1;
return (ARCHIVE_OK);
}

static int
file_close(struct archive *a, void *client_data)
{
struct read_file_data *mine = (struct read_file_data *)client_data;
file_close2(a, client_data);
free(mine);
return (ARCHIVE_OK);
}

static int
file_switch(struct archive *a, void *client_data1, void *client_data2)
{
file_close2(a, client_data1);
return file_open(a, client_data2);
}
