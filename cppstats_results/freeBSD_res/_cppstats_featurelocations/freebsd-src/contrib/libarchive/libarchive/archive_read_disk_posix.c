



























#if !defined(_WIN32) || defined(__CYGWIN__)

#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif
#if defined(HAVE_SYS_MOUNT_H)
#include <sys/mount.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_SYS_STATFS_H)
#include <sys/statfs.h>
#endif
#if defined(HAVE_SYS_STATVFS_H)
#include <sys/statvfs.h>
#endif
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_LINUX_MAGIC_H)
#include <linux/magic.h>
#endif
#if defined(HAVE_LINUX_FS_H)
#include <linux/fs.h>
#endif




#if defined(HAVE_LINUX_EXT2_FS_H)
#include <linux/ext2_fs.h>
#endif
#if defined(HAVE_EXT2FS_EXT2_FS_H) && !defined(__CYGWIN__)
#include <ext2fs/ext2_fs.h>
#endif
#if defined(HAVE_DIRECT_H)
#include <direct.h>
#endif
#if defined(HAVE_DIRENT_H)
#include <dirent.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
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
#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif

#include "archive.h"
#include "archive_string.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_read_disk_private.h"

#if !defined(HAVE_FCHDIR)
#error fchdir function required.
#endif
#if !defined(O_BINARY)
#define O_BINARY 0
#endif
#if !defined(O_CLOEXEC)
#define O_CLOEXEC 0
#endif


























struct restore_time {
const char *name;
time_t mtime;
long mtime_nsec;
time_t atime;
long atime_nsec;
mode_t filetype;
int noatime;
};

struct tree_entry {
int depth;
struct tree_entry *next;
struct tree_entry *parent;
struct archive_string name;
size_t dirname_length;
int64_t dev;
int64_t ino;
int flags;
int filesystem_id;

int symlink_parent_fd;

struct restore_time restore_time;
};

struct filesystem {
int64_t dev;
int synthetic;
int remote;
int noatime;
#if defined(USE_READDIR_R)
size_t name_max;
#endif
long incr_xfer_size;
long max_xfer_size;
long min_xfer_size;
long xfer_align;





unsigned char *allocation_ptr;

unsigned char *buff;
size_t buff_size;
};


#define isDir 1
#define isDirLink 2
#define needsFirstVisit 4
#define needsDescent 8
#define needsOpen 16
#define needsAscent 32




struct tree {
struct tree_entry *stack;
struct tree_entry *current;
DIR *d;
#define INVALID_DIR_HANDLE NULL
struct dirent *de;
#if defined(USE_READDIR_R)
struct dirent *dirent;
size_t dirent_allocated;
#endif
int flags;
int visit_type;

int tree_errno;


struct archive_string path;


const char *basename;

size_t dirname_length;

int depth;
int openCount;
int maxOpenCount;
int initial_dir_fd;
int working_dir_fd;

struct stat lst;
struct stat st;
int descend;
int nlink;

struct restore_time restore_time;

struct entry_sparse {
int64_t length;
int64_t offset;
} *sparse_list, *current_sparse;
int sparse_count;
int sparse_list_size;

char initial_symlink_mode;
char symlink_mode;
struct filesystem *current_filesystem;
struct filesystem *filesystem_table;
int initial_filesystem_id;
int current_filesystem_id;
int max_filesystem_id;
int allocated_filesystem;

int entry_fd;
int entry_eof;
int64_t entry_remaining_bytes;
int64_t entry_total;
unsigned char *entry_buff;
size_t entry_buff_size;
};


#define hasStat 16
#define hasLstat 32
#define onWorkingDir 64

#define needsRestoreTimes 128
#define onInitialDir 256

static int
tree_dir_next_posix(struct tree *t);

#if defined(HAVE_DIRENT_D_NAMLEN)

#define D_NAMELEN(dp) (dp)->d_namlen
#else
#define D_NAMELEN(dp) (strlen((dp)->d_name))
#endif


static struct tree *tree_open(const char *, int, int);
static struct tree *tree_reopen(struct tree *, const char *, int);
static void tree_close(struct tree *);
static void tree_free(struct tree *);
static void tree_push(struct tree *, const char *, int, int64_t, int64_t,
struct restore_time *);
static int tree_enter_initial_dir(struct tree *);
static int tree_enter_working_dir(struct tree *);
static int tree_current_dir_fd(struct tree *);



















#define TREE_REGULAR 1
#define TREE_POSTDESCENT 2
#define TREE_POSTASCENT 3
#define TREE_ERROR_DIR -1
#define TREE_ERROR_FATAL -2

static int tree_next(struct tree *);



















static const char *tree_current_path(struct tree *);
static const char *tree_current_access_path(struct tree *);







static const struct stat *tree_current_stat(struct tree *);
static const struct stat *tree_current_lstat(struct tree *);
static int tree_current_is_symblic_link_target(struct tree *);




static int tree_current_is_physical_dir(struct tree *);

static int tree_current_is_dir(struct tree *);
static int update_current_filesystem(struct archive_read_disk *a,
int64_t dev);
static int setup_current_filesystem(struct archive_read_disk *);
static int tree_target_is_same_as_parent(struct tree *, const struct stat *);

static int _archive_read_disk_open(struct archive *, const char *);
static int _archive_read_free(struct archive *);
static int _archive_read_close(struct archive *);
static int _archive_read_data_block(struct archive *,
const void **, size_t *, int64_t *);
static int _archive_read_next_header(struct archive *,
struct archive_entry **);
static int _archive_read_next_header2(struct archive *,
struct archive_entry *);
static const char *trivial_lookup_gname(void *, int64_t gid);
static const char *trivial_lookup_uname(void *, int64_t uid);
static int setup_sparse(struct archive_read_disk *, struct archive_entry *);
static int close_and_restore_time(int fd, struct tree *,
struct restore_time *);
static int open_on_current_dir(struct tree *, const char *, int);
static int tree_dup(int);


static struct archive_vtable *
archive_read_disk_vtable(void)
{
static struct archive_vtable av;
static int inited = 0;

if (!inited) {
av.archive_free = _archive_read_free;
av.archive_close = _archive_read_close;
av.archive_read_data_block = _archive_read_data_block;
av.archive_read_next_header = _archive_read_next_header;
av.archive_read_next_header2 = _archive_read_next_header2;
inited = 1;
}
return (&av);
}

const char *
archive_read_disk_gname(struct archive *_a, la_int64_t gid)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
if (ARCHIVE_OK != __archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_gname"))
return (NULL);
if (a->lookup_gname == NULL)
return (NULL);
return ((*a->lookup_gname)(a->lookup_gname_data, gid));
}

const char *
archive_read_disk_uname(struct archive *_a, la_int64_t uid)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
if (ARCHIVE_OK != __archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_uname"))
return (NULL);
if (a->lookup_uname == NULL)
return (NULL);
return ((*a->lookup_uname)(a->lookup_uname_data, uid));
}

int
archive_read_disk_set_gname_lookup(struct archive *_a,
void *private_data,
const char * (*lookup_gname)(void *private, la_int64_t gid),
void (*cleanup_gname)(void *private))
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
archive_check_magic(&a->archive, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_set_gname_lookup");

if (a->cleanup_gname != NULL && a->lookup_gname_data != NULL)
(a->cleanup_gname)(a->lookup_gname_data);

a->lookup_gname = lookup_gname;
a->cleanup_gname = cleanup_gname;
a->lookup_gname_data = private_data;
return (ARCHIVE_OK);
}

int
archive_read_disk_set_uname_lookup(struct archive *_a,
void *private_data,
const char * (*lookup_uname)(void *private, la_int64_t uid),
void (*cleanup_uname)(void *private))
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
archive_check_magic(&a->archive, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_set_uname_lookup");

if (a->cleanup_uname != NULL && a->lookup_uname_data != NULL)
(a->cleanup_uname)(a->lookup_uname_data);

a->lookup_uname = lookup_uname;
a->cleanup_uname = cleanup_uname;
a->lookup_uname_data = private_data;
return (ARCHIVE_OK);
}




struct archive *
archive_read_disk_new(void)
{
struct archive_read_disk *a;

a = (struct archive_read_disk *)calloc(1, sizeof(*a));
if (a == NULL)
return (NULL);
a->archive.magic = ARCHIVE_READ_DISK_MAGIC;
a->archive.state = ARCHIVE_STATE_NEW;
a->archive.vtable = archive_read_disk_vtable();
a->entry = archive_entry_new2(&a->archive);
a->lookup_uname = trivial_lookup_uname;
a->lookup_gname = trivial_lookup_gname;
a->flags = ARCHIVE_READDISK_MAC_COPYFILE;
a->open_on_current_dir = open_on_current_dir;
a->tree_current_dir_fd = tree_current_dir_fd;
a->tree_enter_working_dir = tree_enter_working_dir;
return (&a->archive);
}

static int
_archive_read_free(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
int r;

if (_a == NULL)
return (ARCHIVE_OK);
archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY | ARCHIVE_STATE_FATAL, "archive_read_free");

if (a->archive.state != ARCHIVE_STATE_CLOSED)
r = _archive_read_close(&a->archive);
else
r = ARCHIVE_OK;

tree_free(a->tree);
if (a->cleanup_gname != NULL && a->lookup_gname_data != NULL)
(a->cleanup_gname)(a->lookup_gname_data);
if (a->cleanup_uname != NULL && a->lookup_uname_data != NULL)
(a->cleanup_uname)(a->lookup_uname_data);
archive_string_free(&a->archive.error_string);
archive_entry_free(a->entry);
a->archive.magic = 0;
__archive_clean(&a->archive);
free(a);
return (r);
}

static int
_archive_read_close(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY | ARCHIVE_STATE_FATAL, "archive_read_close");

if (a->archive.state != ARCHIVE_STATE_FATAL)
a->archive.state = ARCHIVE_STATE_CLOSED;

tree_close(a->tree);

return (ARCHIVE_OK);
}

static void
setup_symlink_mode(struct archive_read_disk *a, char symlink_mode,
int follow_symlinks)
{
a->symlink_mode = symlink_mode;
a->follow_symlinks = follow_symlinks;
if (a->tree != NULL) {
a->tree->initial_symlink_mode = a->symlink_mode;
a->tree->symlink_mode = a->symlink_mode;
}
}

int
archive_read_disk_set_symlink_logical(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_set_symlink_logical");
setup_symlink_mode(a, 'L', 1);
return (ARCHIVE_OK);
}

int
archive_read_disk_set_symlink_physical(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_set_symlink_physical");
setup_symlink_mode(a, 'P', 0);
return (ARCHIVE_OK);
}

int
archive_read_disk_set_symlink_hybrid(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_set_symlink_hybrid");
setup_symlink_mode(a, 'H', 1);
return (ARCHIVE_OK);
}

int
archive_read_disk_set_atime_restored(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_restore_atime");
#if defined(HAVE_UTIMES)
a->flags |= ARCHIVE_READDISK_RESTORE_ATIME;
if (a->tree != NULL)
a->tree->flags |= needsRestoreTimes;
return (ARCHIVE_OK);
#else

archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Cannot restore access time on this system");
a->flags &= ~ARCHIVE_READDISK_RESTORE_ATIME;
return (ARCHIVE_WARN);
#endif
}

int
archive_read_disk_set_behavior(struct archive *_a, int flags)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
int r = ARCHIVE_OK;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_honor_nodump");

a->flags = flags;

if (flags & ARCHIVE_READDISK_RESTORE_ATIME)
r = archive_read_disk_set_atime_restored(_a);
else {
if (a->tree != NULL)
a->tree->flags &= ~needsRestoreTimes;
}
return (r);
}






static const char *
trivial_lookup_gname(void *private_data, int64_t gid)
{
(void)private_data;
(void)gid;
return (NULL);
}

static const char *
trivial_lookup_uname(void *private_data, int64_t uid)
{
(void)private_data;
(void)uid;
return (NULL);
}





static int
setup_suitable_read_buffer(struct archive_read_disk *a)
{
struct tree *t = a->tree;
struct filesystem *cf = t->current_filesystem;
size_t asize;
size_t s;

if (cf->allocation_ptr == NULL) {



long xfer_align = (cf->xfer_align == -1)?4096:cf->xfer_align;

if (cf->max_xfer_size != -1)
asize = cf->max_xfer_size + xfer_align;
else {
long incr = cf->incr_xfer_size;


if (incr < 0)
incr = cf->min_xfer_size;
if (cf->min_xfer_size < 0) {
incr = xfer_align;
asize = xfer_align;
} else
asize = cf->min_xfer_size;



while (asize < 1024*64)
asize += incr;


asize += xfer_align;
}
cf->allocation_ptr = malloc(asize);
if (cf->allocation_ptr == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Couldn't allocate memory");
a->archive.state = ARCHIVE_STATE_FATAL;
return (ARCHIVE_FATAL);
}




s = (uintptr_t)cf->allocation_ptr;
s %= xfer_align;
if (s > 0)
s = xfer_align - s;





cf->buff = cf->allocation_ptr + s;
cf->buff_size = asize - xfer_align;
}
return (ARCHIVE_OK);
}

static int
_archive_read_data_block(struct archive *_a, const void **buff,
size_t *size, int64_t *offset)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
struct tree *t = a->tree;
int r;
ssize_t bytes;
int64_t sparse_bytes;
size_t buffbytes;
int empty_sparse_region = 0;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC, ARCHIVE_STATE_DATA,
"archive_read_data_block");

if (t->entry_eof || t->entry_remaining_bytes <= 0) {
r = ARCHIVE_EOF;
goto abort_read_data;
}




if (t->entry_fd < 0) {
int flags = O_RDONLY | O_BINARY | O_CLOEXEC;






#if defined(O_DIRECT) && 0
if (t->current_filesystem->xfer_align != -1 &&
t->nlink == 1)
flags |= O_DIRECT;
#endif
#if defined(O_NOATIME)



if ((t->flags & needsRestoreTimes) != 0 &&
t->restore_time.noatime == 0)
flags |= O_NOATIME;
#endif
t->entry_fd = open_on_current_dir(t,
tree_current_access_path(t), flags);
__archive_ensure_cloexec_flag(t->entry_fd);
#if defined(O_NOATIME)






if (flags & O_NOATIME) {
if (t->entry_fd >= 0)
t->restore_time.noatime = 1;
else if (errno == EPERM)
flags &= ~O_NOATIME;
}
#endif
if (t->entry_fd < 0) {
archive_set_error(&a->archive, errno,
"Couldn't open %s", tree_current_path(t));
r = ARCHIVE_FAILED;
tree_enter_initial_dir(t);
goto abort_read_data;
}
tree_enter_initial_dir(t);
}




if (t->current_filesystem->allocation_ptr == NULL) {
r = setup_suitable_read_buffer(a);
if (r != ARCHIVE_OK) {
a->archive.state = ARCHIVE_STATE_FATAL;
goto abort_read_data;
}
}
t->entry_buff = t->current_filesystem->buff;
t->entry_buff_size = t->current_filesystem->buff_size;

buffbytes = t->entry_buff_size;
if ((int64_t)buffbytes > t->current_sparse->length)
buffbytes = t->current_sparse->length;

if (t->current_sparse->length == 0)
empty_sparse_region = 1;





if (t->current_sparse->offset > t->entry_total) {
if (lseek(t->entry_fd,
(off_t)t->current_sparse->offset, SEEK_SET) < 0) {
archive_set_error(&a->archive, errno, "Seek error");
r = ARCHIVE_FATAL;
a->archive.state = ARCHIVE_STATE_FATAL;
goto abort_read_data;
}
sparse_bytes = t->current_sparse->offset - t->entry_total;
t->entry_remaining_bytes -= sparse_bytes;
t->entry_total += sparse_bytes;
}




if (buffbytes > 0) {
bytes = read(t->entry_fd, t->entry_buff, buffbytes);
if (bytes < 0) {
archive_set_error(&a->archive, errno, "Read error");
r = ARCHIVE_FATAL;
a->archive.state = ARCHIVE_STATE_FATAL;
goto abort_read_data;
}
} else
bytes = 0;




if (bytes == 0 && !empty_sparse_region) {

t->entry_eof = 1;
r = ARCHIVE_EOF;
goto abort_read_data;
}
*buff = t->entry_buff;
*size = bytes;
*offset = t->entry_total;
t->entry_total += bytes;
t->entry_remaining_bytes -= bytes;
if (t->entry_remaining_bytes == 0) {

close_and_restore_time(t->entry_fd, t, &t->restore_time);
t->entry_fd = -1;
t->entry_eof = 1;
}
t->current_sparse->offset += bytes;
t->current_sparse->length -= bytes;
if (t->current_sparse->length == 0 && !t->entry_eof)
t->current_sparse++;
return (ARCHIVE_OK);

abort_read_data:
*buff = NULL;
*size = 0;
*offset = t->entry_total;
if (t->entry_fd >= 0) {

close_and_restore_time(t->entry_fd, t, &t->restore_time);
t->entry_fd = -1;
}
return (r);
}

static int
next_entry(struct archive_read_disk *a, struct tree *t,
struct archive_entry *entry)
{
const struct stat *st;
const struct stat *lst;
const char *name;
int delayed, delayed_errno, descend, r;
struct archive_string delayed_str;

delayed = ARCHIVE_OK;
delayed_errno = 0;
archive_string_init(&delayed_str);

st = NULL;
lst = NULL;
t->descend = 0;
do {
switch (tree_next(t)) {
case TREE_ERROR_FATAL:
archive_set_error(&a->archive, t->tree_errno,
"%s: Unable to continue traversing directory tree",
tree_current_path(t));
a->archive.state = ARCHIVE_STATE_FATAL;
tree_enter_initial_dir(t);
return (ARCHIVE_FATAL);
case TREE_ERROR_DIR:
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"%s: Couldn't visit directory",
tree_current_path(t));
tree_enter_initial_dir(t);
return (ARCHIVE_FAILED);
case 0:
tree_enter_initial_dir(t);
return (ARCHIVE_EOF);
case TREE_POSTDESCENT:
case TREE_POSTASCENT:
break;
case TREE_REGULAR:
lst = tree_current_lstat(t);
if (lst == NULL) {
if (errno == ENOENT && t->depth > 0) {
delayed = ARCHIVE_WARN;
delayed_errno = errno;
if (delayed_str.length == 0) {
archive_string_sprintf(&delayed_str,
"%s", tree_current_path(t));
} else {
archive_string_sprintf(&delayed_str,
" %s", tree_current_path(t));
}
} else {
archive_set_error(&a->archive, errno,
"%s: Cannot stat",
tree_current_path(t));
tree_enter_initial_dir(t);
return (ARCHIVE_FAILED);
}
}
break;
}
} while (lst == NULL);

#if defined(__APPLE__)
if (a->flags & ARCHIVE_READDISK_MAC_COPYFILE) {

const char *bname = strrchr(tree_current_path(t), '/');
if (bname == NULL)
bname = tree_current_path(t);
else
++bname;
if (bname[0] == '.' && bname[1] == '_')
return (ARCHIVE_RETRY);
}
#endif

archive_entry_copy_pathname(entry, tree_current_path(t));



if (a->matching) {
r = archive_match_path_excluded(a->matching, entry);
if (r < 0) {
archive_set_error(&(a->archive), errno,
"Failed : %s", archive_error_string(a->matching));
return (r);
}
if (r) {
if (a->excluded_cb_func)
a->excluded_cb_func(&(a->archive),
a->excluded_cb_data, entry);
return (ARCHIVE_RETRY);
}
}




switch(t->symlink_mode) {
case 'H':

t->symlink_mode = 'P';


case 'L':

descend = tree_current_is_dir(t);

a->symlink_mode = 'L';
a->follow_symlinks = 1;

st = tree_current_stat(t);
if (st != NULL && !tree_target_is_same_as_parent(t, st))
break;



default:

descend = tree_current_is_physical_dir(t);

a->symlink_mode = 'P';
a->follow_symlinks = 0;

st = lst;
break;
}

if (update_current_filesystem(a, st->st_dev) != ARCHIVE_OK) {
a->archive.state = ARCHIVE_STATE_FATAL;
tree_enter_initial_dir(t);
return (ARCHIVE_FATAL);
}
if (t->initial_filesystem_id == -1)
t->initial_filesystem_id = t->current_filesystem_id;
if (a->flags & ARCHIVE_READDISK_NO_TRAVERSE_MOUNTS) {
if (t->initial_filesystem_id != t->current_filesystem_id)
descend = 0;
}
t->descend = descend;





if (a->flags & ARCHIVE_READDISK_HONOR_NODUMP) {
#if defined(HAVE_STRUCT_STAT_ST_FLAGS) && defined(UF_NODUMP)
if (st->st_flags & UF_NODUMP)
return (ARCHIVE_RETRY);
#elif (defined(FS_IOC_GETFLAGS) && defined(FS_NODUMP_FL) && defined(HAVE_WORKING_FS_IOC_GETFLAGS)) || (defined(EXT2_IOC_GETFLAGS) && defined(EXT2_NODUMP_FL) && defined(HAVE_WORKING_EXT2_IOC_GETFLAGS))



if (S_ISREG(st->st_mode) || S_ISDIR(st->st_mode)) {
int stflags;

t->entry_fd = open_on_current_dir(t,
tree_current_access_path(t),
O_RDONLY | O_NONBLOCK | O_CLOEXEC);
__archive_ensure_cloexec_flag(t->entry_fd);
if (t->entry_fd >= 0) {
r = ioctl(t->entry_fd,
#if defined(FS_IOC_GETFLAGS)
FS_IOC_GETFLAGS,
#else
EXT2_IOC_GETFLAGS,
#endif
&stflags);
#if defined(FS_NODUMP_FL)
if (r == 0 && (stflags & FS_NODUMP_FL) != 0)
#else
if (r == 0 && (stflags & EXT2_NODUMP_FL) != 0)
#endif
return (ARCHIVE_RETRY);
}
}
#endif
}

archive_entry_copy_stat(entry, st);




t->restore_time.mtime = archive_entry_mtime(entry);
t->restore_time.mtime_nsec = archive_entry_mtime_nsec(entry);
t->restore_time.atime = archive_entry_atime(entry);
t->restore_time.atime_nsec = archive_entry_atime_nsec(entry);
t->restore_time.filetype = archive_entry_filetype(entry);
t->restore_time.noatime = t->current_filesystem->noatime;




if (a->matching) {
r = archive_match_time_excluded(a->matching, entry);
if (r < 0) {
archive_set_error(&(a->archive), errno,
"Failed : %s", archive_error_string(a->matching));
return (r);
}
if (r) {
if (a->excluded_cb_func)
a->excluded_cb_func(&(a->archive),
a->excluded_cb_data, entry);
return (ARCHIVE_RETRY);
}
}


name = archive_read_disk_uname(&(a->archive), archive_entry_uid(entry));
if (name != NULL)
archive_entry_copy_uname(entry, name);
name = archive_read_disk_gname(&(a->archive), archive_entry_gid(entry));
if (name != NULL)
archive_entry_copy_gname(entry, name);




if (a->matching) {
r = archive_match_owner_excluded(a->matching, entry);
if (r < 0) {
archive_set_error(&(a->archive), errno,
"Failed : %s", archive_error_string(a->matching));
return (r);
}
if (r) {
if (a->excluded_cb_func)
a->excluded_cb_func(&(a->archive),
a->excluded_cb_data, entry);
return (ARCHIVE_RETRY);
}
}




if (a->metadata_filter_func) {
if (!a->metadata_filter_func(&(a->archive),
a->metadata_filter_data, entry))
return (ARCHIVE_RETRY);
}




archive_entry_copy_sourcepath(entry, tree_current_access_path(t));
r = archive_read_disk_entry_from_file(&(a->archive), entry,
t->entry_fd, st);

if (r == ARCHIVE_OK) {
r = delayed;
if (r != ARCHIVE_OK) {
archive_string_sprintf(&delayed_str, ": %s",
"File removed before we read it");
archive_set_error(&(a->archive), delayed_errno,
"%s", delayed_str.s);
}
}
archive_string_free(&delayed_str);

return (r);
}

static int
_archive_read_next_header(struct archive *_a, struct archive_entry **entryp)
{
int ret;
struct archive_read_disk *a = (struct archive_read_disk *)_a;
*entryp = NULL;
ret = _archive_read_next_header2(_a, a->entry);
*entryp = a->entry;
return ret;
}

static int
_archive_read_next_header2(struct archive *_a, struct archive_entry *entry)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
struct tree *t;
int r;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_HEADER | ARCHIVE_STATE_DATA,
"archive_read_next_header2");

t = a->tree;
if (t->entry_fd >= 0) {
close_and_restore_time(t->entry_fd, t, &t->restore_time);
t->entry_fd = -1;
}

archive_entry_clear(entry);

for (;;) {
r = next_entry(a, t, entry);
if (t->entry_fd >= 0) {
close(t->entry_fd);
t->entry_fd = -1;
}

if (r == ARCHIVE_RETRY) {
archive_entry_clear(entry);
continue;
}
break;
}


tree_enter_initial_dir(t);






switch (r) {
case ARCHIVE_EOF:
a->archive.state = ARCHIVE_STATE_EOF;
break;
case ARCHIVE_OK:
case ARCHIVE_WARN:

archive_entry_copy_sourcepath(entry, tree_current_path(t));
t->entry_total = 0;
if (archive_entry_filetype(entry) == AE_IFREG) {
t->nlink = archive_entry_nlink(entry);
t->entry_remaining_bytes = archive_entry_size(entry);
t->entry_eof = (t->entry_remaining_bytes == 0)? 1: 0;
if (!t->entry_eof &&
setup_sparse(a, entry) != ARCHIVE_OK)
return (ARCHIVE_FATAL);
} else {
t->entry_remaining_bytes = 0;
t->entry_eof = 1;
}
a->archive.state = ARCHIVE_STATE_DATA;
break;
case ARCHIVE_RETRY:
break;
case ARCHIVE_FATAL:
a->archive.state = ARCHIVE_STATE_FATAL;
break;
}

__archive_reset_read_data(&a->archive);
return (r);
}

static int
setup_sparse(struct archive_read_disk *a, struct archive_entry *entry)
{
struct tree *t = a->tree;
int64_t length, offset;
int i;

t->sparse_count = archive_entry_sparse_reset(entry);
if (t->sparse_count+1 > t->sparse_list_size) {
free(t->sparse_list);
t->sparse_list_size = t->sparse_count + 1;
t->sparse_list = malloc(sizeof(t->sparse_list[0]) *
t->sparse_list_size);
if (t->sparse_list == NULL) {
t->sparse_list_size = 0;
archive_set_error(&a->archive, ENOMEM,
"Can't allocate data");
a->archive.state = ARCHIVE_STATE_FATAL;
return (ARCHIVE_FATAL);
}
}
for (i = 0; i < t->sparse_count; i++) {
archive_entry_sparse_next(entry, &offset, &length);
t->sparse_list[i].offset = offset;
t->sparse_list[i].length = length;
}
if (i == 0) {
t->sparse_list[i].offset = 0;
t->sparse_list[i].length = archive_entry_size(entry);
} else {
t->sparse_list[i].offset = archive_entry_size(entry);
t->sparse_list[i].length = 0;
}
t->current_sparse = t->sparse_list;

return (ARCHIVE_OK);
}

int
archive_read_disk_set_matching(struct archive *_a, struct archive *_ma,
void (*_excluded_func)(struct archive *, void *, struct archive_entry *),
void *_client_data)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_ANY, "archive_read_disk_set_matching");
a->matching = _ma;
a->excluded_cb_func = _excluded_func;
a->excluded_cb_data = _client_data;
return (ARCHIVE_OK);
}

int
archive_read_disk_set_metadata_filter_callback(struct archive *_a,
int (*_metadata_filter_func)(struct archive *, void *,
struct archive_entry *), void *_client_data)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC, ARCHIVE_STATE_ANY,
"archive_read_disk_set_metadata_filter_callback");

a->metadata_filter_func = _metadata_filter_func;
a->metadata_filter_data = _client_data;
return (ARCHIVE_OK);
}

int
archive_read_disk_can_descend(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
struct tree *t = a->tree;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_HEADER | ARCHIVE_STATE_DATA,
"archive_read_disk_can_descend");

return (t->visit_type == TREE_REGULAR && t->descend);
}





int
archive_read_disk_descend(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
struct tree *t = a->tree;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_HEADER | ARCHIVE_STATE_DATA,
"archive_read_disk_descend");

if (t->visit_type != TREE_REGULAR || !t->descend)
return (ARCHIVE_OK);











if (tree_current_is_physical_dir(t)) {
tree_push(t, t->basename, t->current_filesystem_id,
t->lst.st_dev, t->lst.st_ino, &t->restore_time);
if (t->stack->parent->parent != NULL)
t->stack->flags |= isDir;
else
t->stack->flags |= isDirLink;
} else if (tree_current_is_dir(t)) {
tree_push(t, t->basename, t->current_filesystem_id,
t->st.st_dev, t->st.st_ino, &t->restore_time);
t->stack->flags |= isDirLink;
}
t->descend = 0;
return (ARCHIVE_OK);
}

int
archive_read_disk_open(struct archive *_a, const char *pathname)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_NEW | ARCHIVE_STATE_CLOSED,
"archive_read_disk_open");
archive_clear_error(&a->archive);

return (_archive_read_disk_open(_a, pathname));
}

int
archive_read_disk_open_w(struct archive *_a, const wchar_t *pathname)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;
struct archive_string path;
int ret;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC,
ARCHIVE_STATE_NEW | ARCHIVE_STATE_CLOSED,
"archive_read_disk_open_w");
archive_clear_error(&a->archive);


archive_string_init(&path);
if (archive_string_append_from_wcs(&path, pathname,
wcslen(pathname)) != 0) {
if (errno == ENOMEM)
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory");
else
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Can't convert a path to a char string");
a->archive.state = ARCHIVE_STATE_FATAL;
ret = ARCHIVE_FATAL;
} else
ret = _archive_read_disk_open(_a, path.s);

archive_string_free(&path);
return (ret);
}

static int
_archive_read_disk_open(struct archive *_a, const char *pathname)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;

if (a->tree != NULL)
a->tree = tree_reopen(a->tree, pathname,
a->flags & ARCHIVE_READDISK_RESTORE_ATIME);
else
a->tree = tree_open(pathname, a->symlink_mode,
a->flags & ARCHIVE_READDISK_RESTORE_ATIME);
if (a->tree == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate tar data");
a->archive.state = ARCHIVE_STATE_FATAL;
return (ARCHIVE_FATAL);
}
a->archive.state = ARCHIVE_STATE_HEADER;

return (ARCHIVE_OK);
}





int
archive_read_disk_current_filesystem(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC, ARCHIVE_STATE_DATA,
"archive_read_disk_current_filesystem");

return (a->tree->current_filesystem_id);
}

static int
update_current_filesystem(struct archive_read_disk *a, int64_t dev)
{
struct tree *t = a->tree;
int i, fid;

if (t->current_filesystem != NULL &&
t->current_filesystem->dev == dev)
return (ARCHIVE_OK);

for (i = 0; i < t->max_filesystem_id; i++) {
if (t->filesystem_table[i].dev == dev) {

t->current_filesystem_id = i;
t->current_filesystem = &(t->filesystem_table[i]);
return (ARCHIVE_OK);
}
}




fid = t->max_filesystem_id++;
if (t->max_filesystem_id > t->allocated_filesystem) {
size_t s;
void *p;

s = t->max_filesystem_id * 2;
p = realloc(t->filesystem_table,
s * sizeof(*t->filesystem_table));
if (p == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate tar data");
return (ARCHIVE_FATAL);
}
t->filesystem_table = (struct filesystem *)p;
t->allocated_filesystem = s;
}
t->current_filesystem_id = fid;
t->current_filesystem = &(t->filesystem_table[fid]);
t->current_filesystem->dev = dev;
t->current_filesystem->allocation_ptr = NULL;
t->current_filesystem->buff = NULL;



return (setup_current_filesystem(a));
}





int
archive_read_disk_current_filesystem_is_synthetic(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC, ARCHIVE_STATE_DATA,
"archive_read_disk_current_filesystem");

return (a->tree->current_filesystem->synthetic);
}





int
archive_read_disk_current_filesystem_is_remote(struct archive *_a)
{
struct archive_read_disk *a = (struct archive_read_disk *)_a;

archive_check_magic(_a, ARCHIVE_READ_DISK_MAGIC, ARCHIVE_STATE_DATA,
"archive_read_disk_current_filesystem");

return (a->tree->current_filesystem->remote);
}

#if defined(_PC_REC_INCR_XFER_SIZE) && defined(_PC_REC_MAX_XFER_SIZE) &&defined(_PC_REC_MIN_XFER_SIZE) && defined(_PC_REC_XFER_ALIGN)

static int
get_xfer_size(struct tree *t, int fd, const char *path)
{
t->current_filesystem->xfer_align = -1;
errno = 0;
if (fd >= 0) {
t->current_filesystem->incr_xfer_size =
fpathconf(fd, _PC_REC_INCR_XFER_SIZE);
t->current_filesystem->max_xfer_size =
fpathconf(fd, _PC_REC_MAX_XFER_SIZE);
t->current_filesystem->min_xfer_size =
fpathconf(fd, _PC_REC_MIN_XFER_SIZE);
t->current_filesystem->xfer_align =
fpathconf(fd, _PC_REC_XFER_ALIGN);
} else if (path != NULL) {
t->current_filesystem->incr_xfer_size =
pathconf(path, _PC_REC_INCR_XFER_SIZE);
t->current_filesystem->max_xfer_size =
pathconf(path, _PC_REC_MAX_XFER_SIZE);
t->current_filesystem->min_xfer_size =
pathconf(path, _PC_REC_MIN_XFER_SIZE);
t->current_filesystem->xfer_align =
pathconf(path, _PC_REC_XFER_ALIGN);
}

if (t->current_filesystem->xfer_align == -1)
return ((errno == EINVAL)?1:-1);
else
return (0);
}
#else
static int
get_xfer_size(struct tree *t, int fd, const char *path)
{
(void)t;
(void)fd;
(void)path;
return (1);
}
#endif

#if defined(HAVE_STATVFS)
static inline __LA_UNUSED void
set_statvfs_transfer_size(struct filesystem *fs, const struct statvfs *sfs)
{
fs->xfer_align = sfs->f_frsize > 0 ? (long)sfs->f_frsize : -1;
fs->max_xfer_size = -1;
#if defined(HAVE_STRUCT_STATVFS_F_IOSIZE)
fs->min_xfer_size = sfs->f_iosize > 0 ? (long)sfs->f_iosize : -1;
fs->incr_xfer_size = sfs->f_iosize > 0 ? (long)sfs->f_iosize : -1;
#else
fs->min_xfer_size = sfs->f_bsize > 0 ? (long)sfs->f_bsize : -1;
fs->incr_xfer_size = sfs->f_bsize > 0 ? (long)sfs->f_bsize : -1;
#endif
}
#endif

#if defined(HAVE_STRUCT_STATFS)
static inline __LA_UNUSED void
set_statfs_transfer_size(struct filesystem *fs, const struct statfs *sfs)
{
fs->xfer_align = sfs->f_bsize > 0 ? (long)sfs->f_bsize : -1;
fs->max_xfer_size = -1;
#if defined(HAVE_STRUCT_STATFS_F_IOSIZE)
fs->min_xfer_size = sfs->f_iosize > 0 ? (long)sfs->f_iosize : -1;
fs->incr_xfer_size = sfs->f_iosize > 0 ? (long)sfs->f_iosize : -1;
#else
fs->min_xfer_size = sfs->f_bsize > 0 ? (long)sfs->f_bsize : -1;
fs->incr_xfer_size = sfs->f_bsize > 0 ? (long)sfs->f_bsize : -1;
#endif
}
#endif

#if defined(HAVE_STRUCT_STATFS) && defined(HAVE_STATFS) && defined(HAVE_FSTATFS) && defined(MNT_LOCAL) && !defined(ST_LOCAL)





static int
setup_current_filesystem(struct archive_read_disk *a)
{
struct tree *t = a->tree;
struct statfs sfs;
#if defined(HAVE_GETVFSBYNAME) && defined(VFCF_SYNTHETIC)









#if defined(HAVE_STRUCT_XVFSCONF)
struct xvfsconf vfc;
#else
struct vfsconf vfc;
#endif
#endif
int r, xr = 0;
#if !defined(HAVE_STRUCT_STATFS_F_NAMEMAX)
long nm;
#endif

t->current_filesystem->synthetic = -1;
t->current_filesystem->remote = -1;
if (tree_current_is_symblic_link_target(t)) {
#if defined(HAVE_OPENAT)




int fd = openat(tree_current_dir_fd(t),
tree_current_access_path(t), O_RDONLY | O_CLOEXEC);
__archive_ensure_cloexec_flag(fd);
if (fd < 0) {
archive_set_error(&a->archive, errno,
"openat failed");
return (ARCHIVE_FAILED);
}
r = fstatfs(fd, &sfs);
if (r == 0)
xr = get_xfer_size(t, fd, NULL);
close(fd);
#else
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
r = statfs(tree_current_access_path(t), &sfs);
if (r == 0)
xr = get_xfer_size(t, -1, tree_current_access_path(t));
#endif
} else {
r = fstatfs(tree_current_dir_fd(t), &sfs);
if (r == 0)
xr = get_xfer_size(t, tree_current_dir_fd(t), NULL);
}
if (r == -1 || xr == -1) {
archive_set_error(&a->archive, errno, "statfs failed");
return (ARCHIVE_FAILED);
} else if (xr == 1) {

set_statfs_transfer_size(t->current_filesystem, &sfs);
}
if (sfs.f_flags & MNT_LOCAL)
t->current_filesystem->remote = 0;
else
t->current_filesystem->remote = 1;

#if defined(HAVE_GETVFSBYNAME) && defined(VFCF_SYNTHETIC)
r = getvfsbyname(sfs.f_fstypename, &vfc);
if (r == -1) {
archive_set_error(&a->archive, errno, "getvfsbyname failed");
return (ARCHIVE_FAILED);
}
if (vfc.vfc_flags & VFCF_SYNTHETIC)
t->current_filesystem->synthetic = 1;
else
t->current_filesystem->synthetic = 0;
#endif

#if defined(MNT_NOATIME)
if (sfs.f_flags & MNT_NOATIME)
t->current_filesystem->noatime = 1;
else
#endif
t->current_filesystem->noatime = 0;

#if defined(USE_READDIR_R)

#if defined(HAVE_STRUCT_STATFS_F_NAMEMAX)
t->current_filesystem->name_max = sfs.f_namemax;
#else
#if defined(_PC_NAME_MAX)

if (tree_current_is_symblic_link_target(t)) {
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
nm = pathconf(tree_current_access_path(t), _PC_NAME_MAX);
} else
nm = fpathconf(tree_current_dir_fd(t), _PC_NAME_MAX);
#else
nm = -1;
#endif
if (nm == -1)
t->current_filesystem->name_max = NAME_MAX;
else
t->current_filesystem->name_max = nm;
#endif
#endif
return (ARCHIVE_OK);
}

#elif (defined(HAVE_STATVFS) || defined(HAVE_FSTATVFS)) && defined(ST_LOCAL)




static int
setup_current_filesystem(struct archive_read_disk *a)
{
struct tree *t = a->tree;
struct statvfs svfs;
int r, xr = 0;

t->current_filesystem->synthetic = -1;
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
if (tree_current_is_symblic_link_target(t)) {
r = statvfs(tree_current_access_path(t), &svfs);
if (r == 0)
xr = get_xfer_size(t, -1, tree_current_access_path(t));
} else {
#if defined(HAVE_FSTATVFS)
r = fstatvfs(tree_current_dir_fd(t), &svfs);
if (r == 0)
xr = get_xfer_size(t, tree_current_dir_fd(t), NULL);
#else
r = statvfs(".", &svfs);
if (r == 0)
xr = get_xfer_size(t, -1, ".");
#endif
}
if (r == -1 || xr == -1) {
t->current_filesystem->remote = -1;
archive_set_error(&a->archive, errno, "statvfs failed");
return (ARCHIVE_FAILED);
} else if (xr == 1) {


set_statvfs_transfer_size(t->current_filesystem, &svfs);
}
if (svfs.f_flag & ST_LOCAL)
t->current_filesystem->remote = 0;
else
t->current_filesystem->remote = 1;

#if defined(ST_NOATIME)
if (svfs.f_flag & ST_NOATIME)
t->current_filesystem->noatime = 1;
else
#endif
t->current_filesystem->noatime = 0;


t->current_filesystem->name_max = svfs.f_namemax;
return (ARCHIVE_OK);
}

#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_LINUX_MAGIC_H) &&defined(HAVE_STATFS) && defined(HAVE_FSTATFS)





#if !defined(CIFS_SUPER_MAGIC)
#define CIFS_SUPER_MAGIC 0xFF534D42
#endif
#if !defined(DEVFS_SUPER_MAGIC)
#define DEVFS_SUPER_MAGIC 0x1373
#endif




static int
setup_current_filesystem(struct archive_read_disk *a)
{
struct tree *t = a->tree;
struct statfs sfs;
#if defined(HAVE_STATVFS)
struct statvfs svfs;
#endif
int r, vr = 0, xr = 0;

if (tree_current_is_symblic_link_target(t)) {
#if defined(HAVE_OPENAT)




int fd = openat(tree_current_dir_fd(t),
tree_current_access_path(t), O_RDONLY | O_CLOEXEC);
__archive_ensure_cloexec_flag(fd);
if (fd < 0) {
archive_set_error(&a->archive, errno,
"openat failed");
return (ARCHIVE_FAILED);
}
#if defined(HAVE_FSTATVFS)
vr = fstatvfs(fd, &svfs);
#endif
r = fstatfs(fd, &sfs);
if (r == 0)
xr = get_xfer_size(t, fd, NULL);
close(fd);
#else
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
#if defined(HAVE_STATVFS)
vr = statvfs(tree_current_access_path(t), &svfs);
#endif
r = statfs(tree_current_access_path(t), &sfs);
if (r == 0)
xr = get_xfer_size(t, -1, tree_current_access_path(t));
#endif
} else {
#if defined(HAVE_FSTATFS)
#if defined(HAVE_FSTATVFS)
vr = fstatvfs(tree_current_dir_fd(t), &svfs);
#endif
r = fstatfs(tree_current_dir_fd(t), &sfs);
if (r == 0)
xr = get_xfer_size(t, tree_current_dir_fd(t), NULL);
#else
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
#if defined(HAVE_STATVFS)
vr = statvfs(".", &svfs);
#endif
r = statfs(".", &sfs);
if (r == 0)
xr = get_xfer_size(t, -1, ".");
#endif
}
if (r == -1 || xr == -1 || vr == -1) {
t->current_filesystem->synthetic = -1;
t->current_filesystem->remote = -1;
archive_set_error(&a->archive, errno, "statfs failed");
return (ARCHIVE_FAILED);
} else if (xr == 1) {

#if defined(HAVE_STATVFS)
set_statvfs_transfer_size(t->current_filesystem, &svfs);
#else
set_statfs_transfer_size(t->current_filesystem, &sfs);
#endif
}
switch (sfs.f_type) {
case AFS_SUPER_MAGIC:
case CIFS_SUPER_MAGIC:
case CODA_SUPER_MAGIC:
case NCP_SUPER_MAGIC:
case NFS_SUPER_MAGIC:
case SMB_SUPER_MAGIC:
t->current_filesystem->remote = 1;
t->current_filesystem->synthetic = 0;
break;
case DEVFS_SUPER_MAGIC:
case PROC_SUPER_MAGIC:
case USBDEVICE_SUPER_MAGIC:
t->current_filesystem->remote = 0;
t->current_filesystem->synthetic = 1;
break;
default:
t->current_filesystem->remote = 0;
t->current_filesystem->synthetic = 0;
break;
}

#if defined(ST_NOATIME)
#if defined(HAVE_STATVFS)
if (svfs.f_flag & ST_NOATIME)
#else
if (sfs.f_flags & ST_NOATIME)
#endif
t->current_filesystem->noatime = 1;
else
#endif
t->current_filesystem->noatime = 0;

#if defined(USE_READDIR_R)

t->current_filesystem->name_max = sfs.f_namelen;
#endif
return (ARCHIVE_OK);
}

#elif defined(HAVE_SYS_STATVFS_H) &&(defined(HAVE_STATVFS) || defined(HAVE_FSTATVFS))





static int
setup_current_filesystem(struct archive_read_disk *a)
{
struct tree *t = a->tree;
struct statvfs svfs;
int r, xr = 0;

t->current_filesystem->synthetic = -1;
t->current_filesystem->remote = -1;
if (tree_current_is_symblic_link_target(t)) {
#if defined(HAVE_OPENAT)




int fd = openat(tree_current_dir_fd(t),
tree_current_access_path(t), O_RDONLY | O_CLOEXEC);
__archive_ensure_cloexec_flag(fd);
if (fd < 0) {
archive_set_error(&a->archive, errno,
"openat failed");
return (ARCHIVE_FAILED);
}
r = fstatvfs(fd, &svfs);
if (r == 0)
xr = get_xfer_size(t, fd, NULL);
close(fd);
#else
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
r = statvfs(tree_current_access_path(t), &svfs);
if (r == 0)
xr = get_xfer_size(t, -1, tree_current_access_path(t));
#endif
} else {
#if defined(HAVE_FSTATVFS)
r = fstatvfs(tree_current_dir_fd(t), &svfs);
if (r == 0)
xr = get_xfer_size(t, tree_current_dir_fd(t), NULL);
#else
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
r = statvfs(".", &svfs);
if (r == 0)
xr = get_xfer_size(t, -1, ".");
#endif
}
if (r == -1 || xr == -1) {
t->current_filesystem->synthetic = -1;
t->current_filesystem->remote = -1;
archive_set_error(&a->archive, errno, "statvfs failed");
return (ARCHIVE_FAILED);
} else if (xr == 1) {

set_statvfs_transfer_size(t->current_filesystem, &svfs);
}

#if defined(ST_NOATIME)
if (svfs.f_flag & ST_NOATIME)
t->current_filesystem->noatime = 1;
else
#endif
t->current_filesystem->noatime = 0;

#if defined(USE_READDIR_R)

t->current_filesystem->name_max = svfs.f_namemax;
#endif
return (ARCHIVE_OK);
}

#else





static int
setup_current_filesystem(struct archive_read_disk *a)
{
struct tree *t = a->tree;
#if defined(_PC_NAME_MAX) && defined(USE_READDIR_R)
long nm;
#endif
t->current_filesystem->synthetic = -1;
t->current_filesystem->remote = -1;
t->current_filesystem->noatime = 0;
(void)get_xfer_size(t, -1, ".");
t->current_filesystem->xfer_align = -1;
t->current_filesystem->max_xfer_size = -1;
t->current_filesystem->min_xfer_size = -1;
t->current_filesystem->incr_xfer_size = -1;

#if defined(USE_READDIR_R)

#if defined(_PC_NAME_MAX)
if (tree_current_is_symblic_link_target(t)) {
if (tree_enter_working_dir(t) != 0) {
archive_set_error(&a->archive, errno, "fchdir failed");
return (ARCHIVE_FAILED);
}
nm = pathconf(tree_current_access_path(t), _PC_NAME_MAX);
} else
nm = fpathconf(tree_current_dir_fd(t), _PC_NAME_MAX);
if (nm == -1)
#endif




#if defined(NAME_MAX) && NAME_MAX >= 255
t->current_filesystem->name_max = NAME_MAX;
#else


t->current_filesystem->name_max = PATH_MAX;
#endif
#if defined(_PC_NAME_MAX)
else
t->current_filesystem->name_max = nm;
#endif
#endif
return (ARCHIVE_OK);
}

#endif

static int
close_and_restore_time(int fd, struct tree *t, struct restore_time *rt)
{
#if !defined(HAVE_UTIMES)
(void)t;
(void)rt;
return (close(fd));
#else
#if defined(HAVE_FUTIMENS) && !defined(__CYGWIN__)
struct timespec timespecs[2];
#endif
struct timeval times[2];

if ((t->flags & needsRestoreTimes) == 0 || rt->noatime) {
if (fd >= 0)
return (close(fd));
else
return (0);
}

#if defined(HAVE_FUTIMENS) && !defined(__CYGWIN__)
timespecs[1].tv_sec = rt->mtime;
timespecs[1].tv_nsec = rt->mtime_nsec;

timespecs[0].tv_sec = rt->atime;
timespecs[0].tv_nsec = rt->atime_nsec;

if (futimens(fd, timespecs) == 0)
return (close(fd));
#endif

times[1].tv_sec = rt->mtime;
times[1].tv_usec = rt->mtime_nsec / 1000;

times[0].tv_sec = rt->atime;
times[0].tv_usec = rt->atime_nsec / 1000;

#if !defined(HAVE_FUTIMENS) && defined(HAVE_FUTIMES) && !defined(__CYGWIN__)
if (futimes(fd, times) == 0)
return (close(fd));
#endif
close(fd);
#if defined(HAVE_FUTIMESAT)
if (futimesat(tree_current_dir_fd(t), rt->name, times) == 0)
return (0);
#endif
#if defined(HAVE_LUTIMES)
if (lutimes(rt->name, times) != 0)
#else
if (AE_IFLNK != rt->filetype && utimes(rt->name, times) != 0)
#endif
return (-1);
#endif
return (0);
}

static int
open_on_current_dir(struct tree *t, const char *path, int flags)
{
#if defined(HAVE_OPENAT)
return (openat(tree_current_dir_fd(t), path, flags));
#else
if (tree_enter_working_dir(t) != 0)
return (-1);
return (open(path, flags));
#endif
}

static int
tree_dup(int fd)
{
int new_fd;
#if defined(F_DUPFD_CLOEXEC)
static volatile int can_dupfd_cloexec = 1;

if (can_dupfd_cloexec) {
new_fd = fcntl(fd, F_DUPFD_CLOEXEC, 0);
if (new_fd != -1)
return (new_fd);



can_dupfd_cloexec = 0;
}
#endif
new_fd = dup(fd);
__archive_ensure_cloexec_flag(new_fd);
return (new_fd);
}




static void
tree_push(struct tree *t, const char *path, int filesystem_id,
int64_t dev, int64_t ino, struct restore_time *rt)
{
struct tree_entry *te;

te = calloc(1, sizeof(*te));
te->next = t->stack;
te->parent = t->current;
if (te->parent)
te->depth = te->parent->depth + 1;
t->stack = te;
archive_string_init(&te->name);
te->symlink_parent_fd = -1;
archive_strcpy(&te->name, path);
te->flags = needsDescent | needsOpen | needsAscent;
te->filesystem_id = filesystem_id;
te->dev = dev;
te->ino = ino;
te->dirname_length = t->dirname_length;
te->restore_time.name = te->name.s;
if (rt != NULL) {
te->restore_time.mtime = rt->mtime;
te->restore_time.mtime_nsec = rt->mtime_nsec;
te->restore_time.atime = rt->atime;
te->restore_time.atime_nsec = rt->atime_nsec;
te->restore_time.filetype = rt->filetype;
te->restore_time.noatime = rt->noatime;
}
}




static void
tree_append(struct tree *t, const char *name, size_t name_length)
{
size_t size_needed;

t->path.s[t->dirname_length] = '\0';
t->path.length = t->dirname_length;

while (name_length > 1 && name[name_length - 1] == '/')
name_length--;


size_needed = name_length + t->dirname_length + 2;
archive_string_ensure(&t->path, size_needed);

if (t->dirname_length > 0 && t->path.s[archive_strlen(&t->path)-1] != '/')
archive_strappend_char(&t->path, '/');
t->basename = t->path.s + archive_strlen(&t->path);
archive_strncat(&t->path, name, name_length);
t->restore_time.name = t->basename;
}




static struct tree *
tree_open(const char *path, int symlink_mode, int restore_time)
{
struct tree *t;

if ((t = calloc(1, sizeof(*t))) == NULL)
return (NULL);
archive_string_init(&t->path);
archive_string_ensure(&t->path, 31);
t->initial_symlink_mode = symlink_mode;
return (tree_reopen(t, path, restore_time));
}

static struct tree *
tree_reopen(struct tree *t, const char *path, int restore_time)
{
#if defined(O_PATH)

const int o_flag = O_PATH;
#elif defined(O_SEARCH)

const int o_flag = O_SEARCH;
#elif defined(__FreeBSD__) && defined(O_EXEC)

const int o_flag = O_EXEC;
#endif

t->flags = (restore_time != 0)?needsRestoreTimes:0;
t->flags |= onInitialDir;
t->visit_type = 0;
t->tree_errno = 0;
t->dirname_length = 0;
t->depth = 0;
t->descend = 0;
t->current = NULL;
t->d = INVALID_DIR_HANDLE;
t->symlink_mode = t->initial_symlink_mode;
archive_string_empty(&t->path);
t->entry_fd = -1;
t->entry_eof = 0;
t->entry_remaining_bytes = 0;
t->initial_filesystem_id = -1;


tree_push(t, path, 0, 0, 0, NULL);
t->stack->flags = needsFirstVisit;
t->maxOpenCount = t->openCount = 1;
t->initial_dir_fd = open(".", O_RDONLY | O_CLOEXEC);
#if defined(O_PATH) || defined(O_SEARCH) || (defined(__FreeBSD__) && defined(O_EXEC))






if (t->initial_dir_fd < 0)
t->initial_dir_fd = open(".", o_flag | O_CLOEXEC);
#endif
__archive_ensure_cloexec_flag(t->initial_dir_fd);
t->working_dir_fd = tree_dup(t->initial_dir_fd);
return (t);
}

static int
tree_descent(struct tree *t)
{
int flag, new_fd, r = 0;

t->dirname_length = archive_strlen(&t->path);
flag = O_RDONLY | O_CLOEXEC;
#if defined(O_DIRECTORY)
flag |= O_DIRECTORY;
#endif
new_fd = open_on_current_dir(t, t->stack->name.s, flag);
__archive_ensure_cloexec_flag(new_fd);
if (new_fd < 0) {
t->tree_errno = errno;
r = TREE_ERROR_DIR;
} else {
t->depth++;

if (t->stack->flags & isDirLink) {
t->stack->symlink_parent_fd = t->working_dir_fd;
t->openCount++;
if (t->openCount > t->maxOpenCount)
t->maxOpenCount = t->openCount;
} else
close(t->working_dir_fd);

t->working_dir_fd = new_fd;
t->flags &= ~onWorkingDir;
}
return (r);
}




static int
tree_ascend(struct tree *t)
{
struct tree_entry *te;
int new_fd, r = 0, prev_dir_fd;

te = t->stack;
prev_dir_fd = t->working_dir_fd;
if (te->flags & isDirLink)
new_fd = te->symlink_parent_fd;
else {
new_fd = open_on_current_dir(t, "..", O_RDONLY | O_CLOEXEC);
__archive_ensure_cloexec_flag(new_fd);
}
if (new_fd < 0) {
t->tree_errno = errno;
r = TREE_ERROR_FATAL;
} else {

t->working_dir_fd = new_fd;
t->flags &= ~onWorkingDir;


close_and_restore_time(prev_dir_fd, t, &te->restore_time);
if (te->flags & isDirLink) {
t->openCount--;
te->symlink_parent_fd = -1;
}
t->depth--;
}
return (r);
}




static int
tree_enter_initial_dir(struct tree *t)
{
int r = 0;

if ((t->flags & onInitialDir) == 0) {
r = fchdir(t->initial_dir_fd);
if (r == 0) {
t->flags &= ~onWorkingDir;
t->flags |= onInitialDir;
}
}
return (r);
}




static int
tree_enter_working_dir(struct tree *t)
{
int r = 0;






if (t->depth > 0 && (t->flags & onWorkingDir) == 0) {
r = fchdir(t->working_dir_fd);
if (r == 0) {
t->flags &= ~onInitialDir;
t->flags |= onWorkingDir;
}
}
return (r);
}

static int
tree_current_dir_fd(struct tree *t)
{
return (t->working_dir_fd);
}




static void
tree_pop(struct tree *t)
{
struct tree_entry *te;

t->path.s[t->dirname_length] = '\0';
t->path.length = t->dirname_length;
if (t->stack == t->current && t->current != NULL)
t->current = t->current->parent;
te = t->stack;
t->stack = te->next;
t->dirname_length = te->dirname_length;
t->basename = t->path.s + t->dirname_length;
while (t->basename[0] == '/')
t->basename++;
archive_string_free(&te->name);
free(te);
}




static int
tree_next(struct tree *t)
{
int r;

while (t->stack != NULL) {

if (t->d != INVALID_DIR_HANDLE) {
r = tree_dir_next_posix(t);
if (r == 0)
continue;
return (r);
}

if (t->stack->flags & needsFirstVisit) {

t->current = t->stack;
tree_append(t, t->stack->name.s,
archive_strlen(&(t->stack->name)));


t->stack->flags &= ~needsFirstVisit;
return (t->visit_type = TREE_REGULAR);
} else if (t->stack->flags & needsDescent) {

t->current = t->stack;
tree_append(t, t->stack->name.s,
archive_strlen(&(t->stack->name)));
t->stack->flags &= ~needsDescent;
r = tree_descent(t);
if (r != 0) {
tree_pop(t);
t->visit_type = r;
} else
t->visit_type = TREE_POSTDESCENT;
return (t->visit_type);
} else if (t->stack->flags & needsOpen) {
t->stack->flags &= ~needsOpen;
r = tree_dir_next_posix(t);
if (r == 0)
continue;
return (r);
} else if (t->stack->flags & needsAscent) {

r = tree_ascend(t);
tree_pop(t);
t->visit_type = r != 0 ? r : TREE_POSTASCENT;
return (t->visit_type);
} else {

tree_pop(t);
t->flags &= ~hasLstat;
t->flags &= ~hasStat;
}
}
return (t->visit_type = 0);
}

static int
tree_dir_next_posix(struct tree *t)
{
int r;
const char *name;
size_t namelen;

if (t->d == NULL) {
#if defined(USE_READDIR_R)
size_t dirent_size;
#endif

#if defined(HAVE_FDOPENDIR)
t->d = fdopendir(tree_dup(t->working_dir_fd));
#else
if (tree_enter_working_dir(t) == 0) {
t->d = opendir(".");
#if HAVE_DIRFD || defined(dirfd)
__archive_ensure_cloexec_flag(dirfd(t->d));
#endif
}
#endif
if (t->d == NULL) {
r = tree_ascend(t);
tree_pop(t);
t->tree_errno = errno;
t->visit_type = r != 0 ? r : TREE_ERROR_DIR;
return (t->visit_type);
}
#if defined(USE_READDIR_R)
dirent_size = offsetof(struct dirent, d_name) +
t->filesystem_table[t->current->filesystem_id].name_max + 1;
if (t->dirent == NULL || t->dirent_allocated < dirent_size) {
free(t->dirent);
t->dirent = malloc(dirent_size);
if (t->dirent == NULL) {
closedir(t->d);
t->d = INVALID_DIR_HANDLE;
(void)tree_ascend(t);
tree_pop(t);
t->tree_errno = ENOMEM;
t->visit_type = TREE_ERROR_DIR;
return (t->visit_type);
}
t->dirent_allocated = dirent_size;
}
#endif
}
for (;;) {
errno = 0;
#if defined(USE_READDIR_R)
r = readdir_r(t->d, t->dirent, &t->de);
#if defined(_AIX)






if (r == 9)
r = errno;
#endif
if (r != 0 || t->de == NULL) {
#else
t->de = readdir(t->d);
if (t->de == NULL) {
r = errno;
#endif
closedir(t->d);
t->d = INVALID_DIR_HANDLE;
if (r != 0) {
t->tree_errno = r;
t->visit_type = TREE_ERROR_DIR;
return (t->visit_type);
} else
return (0);
}
name = t->de->d_name;
namelen = D_NAMELEN(t->de);
t->flags &= ~hasLstat;
t->flags &= ~hasStat;
if (name[0] == '.' && name[1] == '\0')
continue;
if (name[0] == '.' && name[1] == '.' && name[2] == '\0')
continue;
tree_append(t, name, namelen);
return (t->visit_type = TREE_REGULAR);
}
}





static const struct stat *
tree_current_stat(struct tree *t)
{
if (!(t->flags & hasStat)) {
#if defined(HAVE_FSTATAT)
if (fstatat(tree_current_dir_fd(t),
tree_current_access_path(t), &t->st, 0) != 0)
#else
if (tree_enter_working_dir(t) != 0)
return NULL;
if (la_stat(tree_current_access_path(t), &t->st) != 0)
#endif
return NULL;
t->flags |= hasStat;
}
return (&t->st);
}




static const struct stat *
tree_current_lstat(struct tree *t)
{
if (!(t->flags & hasLstat)) {
#if defined(HAVE_FSTATAT)
if (fstatat(tree_current_dir_fd(t),
tree_current_access_path(t), &t->lst,
AT_SYMLINK_NOFOLLOW) != 0)
#else
if (tree_enter_working_dir(t) != 0)
return NULL;
if (lstat(tree_current_access_path(t), &t->lst) != 0)
#endif
return NULL;
t->flags |= hasLstat;
}
return (&t->lst);
}




static int
tree_current_is_dir(struct tree *t)
{
const struct stat *st;




if (t->flags & hasLstat) {

st = tree_current_lstat(t);
if (st == NULL)
return 0;
if (S_ISDIR(st->st_mode))
return 1;


if (!S_ISLNK(st->st_mode))
return 0;




}

st = tree_current_stat(t);

if (st == NULL)
return 0;

return (S_ISDIR(st->st_mode));
}






static int
tree_current_is_physical_dir(struct tree *t)
{
const struct stat *st;





if (t->flags & hasStat) {
st = tree_current_stat(t);
if (st == NULL)
return (0);
if (!S_ISDIR(st->st_mode))
return (0);
}








st = tree_current_lstat(t);

if (st == NULL)
return 0;

return (S_ISDIR(st->st_mode));
}




static int
tree_target_is_same_as_parent(struct tree *t, const struct stat *st)
{
struct tree_entry *te;

for (te = t->current->parent; te != NULL; te = te->parent) {
if (te->dev == (int64_t)st->st_dev &&
te->ino == (int64_t)st->st_ino)
return (1);
}
return (0);
}





static int
tree_current_is_symblic_link_target(struct tree *t)
{
static const struct stat *lst, *st;

lst = tree_current_lstat(t);
st = tree_current_stat(t);
return (st != NULL && lst != NULL &&
(int64_t)st->st_dev == t->current_filesystem->dev &&
st->st_dev != lst->st_dev);
}




static const char *
tree_current_access_path(struct tree *t)
{
return (t->basename);
}




static const char *
tree_current_path(struct tree *t)
{
return (t->path.s);
}




static void
tree_close(struct tree *t)
{

if (t == NULL)
return;
if (t->entry_fd >= 0) {
close_and_restore_time(t->entry_fd, t, &t->restore_time);
t->entry_fd = -1;
}

if (t->d != INVALID_DIR_HANDLE) {
closedir(t->d);
t->d = INVALID_DIR_HANDLE;
}

while (t->stack != NULL) {
if (t->stack->flags & isDirLink)
close(t->stack->symlink_parent_fd);
tree_pop(t);
}
if (t->working_dir_fd >= 0) {
close(t->working_dir_fd);
t->working_dir_fd = -1;
}
if (t->initial_dir_fd >= 0) {
close(t->initial_dir_fd);
t->initial_dir_fd = -1;
}
}




static void
tree_free(struct tree *t)
{
int i;

if (t == NULL)
return;
archive_string_free(&t->path);
#if defined(USE_READDIR_R)
free(t->dirent);
#endif
free(t->sparse_list);
for (i = 0; i < t->max_filesystem_id; i++)
free(t->filesystem_table[i].allocation_ptr);
free(t->filesystem_table);
free(t);
}

#endif
