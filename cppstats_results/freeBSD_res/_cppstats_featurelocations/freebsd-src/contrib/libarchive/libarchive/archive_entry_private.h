


























#if !defined(ARCHIVE_ENTRY_PRIVATE_H_INCLUDED)
#define ARCHIVE_ENTRY_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

#include "archive_acl_private.h"
#include "archive_string.h"

struct ae_xattr {
struct ae_xattr *next;

char *name;
void *value;
size_t size;
};

struct ae_sparse {
struct ae_sparse *next;

int64_t offset;
int64_t length;
};

struct ae_digest {
unsigned char md5[16];
unsigned char rmd160[20];
unsigned char sha1[20];
unsigned char sha256[32];
unsigned char sha384[48];
unsigned char sha512[64];
};





















struct archive_entry {
struct archive *archive;





















void *stat;
int stat_valid;

struct aest {
int64_t aest_atime;
uint32_t aest_atime_nsec;
int64_t aest_ctime;
uint32_t aest_ctime_nsec;
int64_t aest_mtime;
uint32_t aest_mtime_nsec;
int64_t aest_birthtime;
uint32_t aest_birthtime_nsec;
int64_t aest_gid;
int64_t aest_ino;
uint32_t aest_nlink;
uint64_t aest_size;
int64_t aest_uid;








int aest_dev_is_broken_down;
dev_t aest_dev;
dev_t aest_devmajor;
dev_t aest_devminor;
int aest_rdev_is_broken_down;
dev_t aest_rdev;
dev_t aest_rdevmajor;
dev_t aest_rdevminor;
} ae_stat;

int ae_set;
#define AE_SET_HARDLINK 1
#define AE_SET_SYMLINK 2
#define AE_SET_ATIME 4
#define AE_SET_CTIME 8
#define AE_SET_MTIME 16
#define AE_SET_BIRTHTIME 32
#define AE_SET_SIZE 64
#define AE_SET_INO 128
#define AE_SET_DEV 256




struct archive_mstring ae_fflags_text;
unsigned long ae_fflags_set;
unsigned long ae_fflags_clear;
struct archive_mstring ae_gname;
struct archive_mstring ae_hardlink;
struct archive_mstring ae_pathname;
struct archive_mstring ae_symlink;
struct archive_mstring ae_uname;


struct archive_mstring ae_sourcepath;

#define AE_ENCRYPTION_NONE 0
#define AE_ENCRYPTION_DATA 1
#define AE_ENCRYPTION_METADATA 2
char encryption;

void *mac_metadata;
size_t mac_metadata_size;


struct ae_digest digest;


struct archive_acl acl;


struct ae_xattr *xattr_head;
struct ae_xattr *xattr_p;


struct ae_sparse *sparse_head;
struct ae_sparse *sparse_tail;
struct ae_sparse *sparse_p;


char strmode[12];


int ae_symlink_type;
};

int
archive_entry_set_digest(struct archive_entry *entry, int type,
const unsigned char *digest);

#endif
