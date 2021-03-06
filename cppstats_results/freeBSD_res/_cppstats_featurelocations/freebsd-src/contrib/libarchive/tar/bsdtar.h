


























#if !defined(BSDTAR_H_INCLUDED)
#define BSDTAR_H_INCLUDED

#include "bsdtar_platform.h"
#include <stdio.h>

#define DEFAULT_BYTES_PER_BLOCK (20*512)
#define ENV_READER_OPTIONS "TAR_READER_OPTIONS"
#define ENV_WRITER_OPTIONS "TAR_WRITER_OPTIONS"
#define IGNORE_WRONG_MODULE_NAME "__ignore_wrong_module_name__,"

struct creation_set;








struct bsdtar {

const char *filename;
char *pending_chdir;
const char *names_from_file;
int bytes_per_block;
int bytes_in_last_block;
int verbose;
unsigned int flags;
int extract_flags;
int readdisk_flags;
int strip_components;
int gid;
const char *gname;
int uid;
const char *uname;
const char *passphrase;
char mode;
char symlink_mode;
const char *option_options;
char day_first;
struct creation_set *cset;


int getopt_state;
char *getopt_word;


int fd;


int argc;
char **argv;
const char *argument;
size_t gs_width;
size_t u_width;
uid_t user_uid;
int return_value;
char warned_lead_slash;
char next_line_is_dir;





struct archive *diskreader;
struct archive_entry_linkresolver *resolver;
struct archive_dir *archive_dir;
struct name_cache *gname_cache;
char *buff;
size_t buff_size;
int first_fs;
struct archive *matching;
struct security *security;
struct name_cache *uname_cache;
struct siginfo_data *siginfo;
struct substitution *substitution;
char *ppbuff;
};


#define OPTFLAG_AUTO_COMPRESS (0x00000001)
#define OPTFLAG_ABSOLUTE_PATHS (0x00000002)
#define OPTFLAG_CHROOT (0x00000004)
#define OPTFLAG_FAST_READ (0x00000008)
#define OPTFLAG_IGNORE_ZEROS (0x00000010)
#define OPTFLAG_INTERACTIVE (0x00000020)
#define OPTFLAG_NO_OWNER (0x00000040)
#define OPTFLAG_NO_SUBDIRS (0x00000080)
#define OPTFLAG_NULL (0x00000100)
#define OPTFLAG_NUMERIC_OWNER (0x00000200)
#define OPTFLAG_O (0x00000400)
#define OPTFLAG_STDOUT (0x00000800)
#define OPTFLAG_TOTALS (0x00001000)
#define OPTFLAG_UNLINK_FIRST (0x00002000)
#define OPTFLAG_WARN_LINKS (0x00004000)
#define OPTFLAG_NO_XATTRS (0x00008000)
#define OPTFLAG_XATTRS (0x00010000)
#define OPTFLAG_NO_ACLS (0x00020000)
#define OPTFLAG_ACLS (0x00040000)
#define OPTFLAG_NO_FFLAGS (0x00080000)
#define OPTFLAG_FFLAGS (0x00100000)
#define OPTFLAG_NO_MAC_METADATA (0x00200000)
#define OPTFLAG_MAC_METADATA (0x00400000)


enum {
OPTION_ACLS = 1,
OPTION_B64ENCODE,
OPTION_CHECK_LINKS,
OPTION_CHROOT,
OPTION_CLEAR_NOCHANGE_FFLAGS,
OPTION_EXCLUDE,
OPTION_EXCLUDE_VCS,
OPTION_FFLAGS,
OPTION_FORMAT,
OPTION_GID,
OPTION_GNAME,
OPTION_GRZIP,
OPTION_HELP,
OPTION_HFS_COMPRESSION,
OPTION_IGNORE_ZEROS,
OPTION_INCLUDE,
OPTION_KEEP_NEWER_FILES,
OPTION_LRZIP,
OPTION_LZ4,
OPTION_LZIP,
OPTION_LZMA,
OPTION_LZOP,
OPTION_MAC_METADATA,
OPTION_NEWER_CTIME,
OPTION_NEWER_CTIME_THAN,
OPTION_NEWER_MTIME,
OPTION_NEWER_MTIME_THAN,
OPTION_NODUMP,
OPTION_NOPRESERVE_HFS_COMPRESSION,
OPTION_NO_ACLS,
OPTION_NO_FFLAGS,
OPTION_NO_MAC_METADATA,
OPTION_NO_SAFE_WRITES,
OPTION_NO_SAME_OWNER,
OPTION_NO_SAME_PERMISSIONS,
OPTION_NO_XATTRS,
OPTION_NULL,
OPTION_NUMERIC_OWNER,
OPTION_OLDER_CTIME,
OPTION_OLDER_CTIME_THAN,
OPTION_OLDER_MTIME,
OPTION_OLDER_MTIME_THAN,
OPTION_ONE_FILE_SYSTEM,
OPTION_OPTIONS,
OPTION_PASSPHRASE,
OPTION_POSIX,
OPTION_SAFE_WRITES,
OPTION_SAME_OWNER,
OPTION_STRIP_COMPONENTS,
OPTION_TOTALS,
OPTION_UID,
OPTION_UNAME,
OPTION_USE_COMPRESS_PROGRAM,
OPTION_UUENCODE,
OPTION_VERSION,
OPTION_XATTRS,
OPTION_ZSTD,
};

int bsdtar_getopt(struct bsdtar *);
void do_chdir(struct bsdtar *);
int edit_pathname(struct bsdtar *, struct archive_entry *);
int need_report(void);
int pathcmp(const char *a, const char *b);
void safe_fprintf(FILE *, const char *fmt, ...) __LA_PRINTF(2, 3);
void set_chdir(struct bsdtar *, const char *newdir);
const char *tar_i64toa(int64_t);
void tar_mode_c(struct bsdtar *bsdtar);
void tar_mode_r(struct bsdtar *bsdtar);
void tar_mode_t(struct bsdtar *bsdtar);
void tar_mode_u(struct bsdtar *bsdtar);
void tar_mode_x(struct bsdtar *bsdtar);
void usage(void) __LA_DEAD;
int yes(const char *fmt, ...) __LA_PRINTF(1, 2);

#if defined(HAVE_REGEX_H) || defined(HAVE_PCREPOSIX_H)
void add_substitution(struct bsdtar *, const char *);
int apply_substitution(struct bsdtar *, const char *, char **, int, int);
void cleanup_substitution(struct bsdtar *);
#endif

void cset_add_filter(struct creation_set *, const char *);
void cset_add_filter_program(struct creation_set *, const char *);
int cset_auto_compress(struct creation_set *, const char *);
void cset_free(struct creation_set *);
const char * cset_get_format(struct creation_set *);
struct creation_set *cset_new(void);
int cset_read_support_filter_program(struct creation_set *,
struct archive *);
void cset_set_format(struct creation_set *, const char *);
int cset_write_add_filters(struct creation_set *,
struct archive *, const void **);

const char * passphrase_callback(struct archive *, void *);
void passphrase_free(char *);
void list_item_verbose(struct bsdtar *, FILE *,
struct archive_entry *);

#endif
