



























#if !defined(ARCHIVE_READ_DISK_PRIVATE_H_INCLUDED)
#define ARCHIVE_READ_DISK_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

#include "archive_platform_acl.h"

struct tree;
struct archive_entry;

struct archive_read_disk {
struct archive archive;


struct archive_entry *entry;







char symlink_mode;






char follow_symlinks;


struct tree *tree;
int (*open_on_current_dir)(struct tree*, const char *, int);
int (*tree_current_dir_fd)(struct tree*);
int (*tree_enter_working_dir)(struct tree*);


int flags;

const char * (*lookup_gname)(void *private, int64_t gid);
void (*cleanup_gname)(void *private);
void *lookup_gname_data;
const char * (*lookup_uname)(void *private, int64_t uid);
void (*cleanup_uname)(void *private);
void *lookup_uname_data;

int (*metadata_filter_func)(struct archive *, void *,
struct archive_entry *);
void *metadata_filter_data;


struct archive *matching;


void (*excluded_cb_func)(struct archive *, void *,
struct archive_entry *);
void *excluded_cb_data;
};

const char *
archive_read_disk_entry_setup_path(struct archive_read_disk *,
struct archive_entry *, int *);

int
archive_read_disk_entry_setup_acls(struct archive_read_disk *,
struct archive_entry *, int *);
#endif
