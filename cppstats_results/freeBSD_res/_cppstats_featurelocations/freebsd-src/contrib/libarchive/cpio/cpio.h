


























#if !defined(CPIO_H_INCLUDED)
#define CPIO_H_INCLUDED

#include "cpio_platform.h"
#include <stdio.h>









struct cpio {

const char *argument;


int add_filter;
const char *filename;
int mode;
int compress;
const char *format;
int bytes_per_block;
int verbose;
int dot;
int quiet;
int extract_flags;
const char *compress_program;
int option_append;
int option_atime_restore;
int option_follow_links;
int option_link;
int option_list;
char option_null;
int option_numeric_uid_gid;
int option_pwb;
int option_rename;
char *destdir;
size_t destdir_len;
size_t pass_destpath_alloc;
char *pass_destpath;
int uid_override;
char *uname_override;
int gid_override;
char *gname_override;
int day_first;
const char *passphrase;


int fd;


struct archive *archive;
struct archive *archive_read_disk;
int argc;
char **argv;
int return_value;
struct archive_entry_linkresolver *linkresolver;

struct name_cache *uname_cache;
struct name_cache *gname_cache;


struct archive *matching;
char *buff;
size_t buff_size;
char *ppbuff;
};

const char *owner_parse(const char *, int *, int *);



enum {
OPTION_B64ENCODE = 1,
OPTION_GRZIP,
OPTION_INSECURE,
OPTION_LRZIP,
OPTION_LZ4,
OPTION_LZMA,
OPTION_LZOP,
OPTION_PASSPHRASE,
OPTION_NO_PRESERVE_OWNER,
OPTION_PRESERVE_OWNER,
OPTION_QUIET,
OPTION_UUENCODE,
OPTION_VERSION,
OPTION_ZSTD,
};

int cpio_getopt(struct cpio *cpio);

#endif
