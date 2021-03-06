


























#if !defined(ARCHIVE_ACL_PRIVATE_H_INCLUDED)
#define ARCHIVE_ACL_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

#include "archive_string.h"

struct archive_acl_entry {
struct archive_acl_entry *next;
int type;
int tag;
int permset;
int id;
struct archive_mstring name;
};

struct archive_acl {
mode_t mode;
struct archive_acl_entry *acl_head;
struct archive_acl_entry *acl_p;
int acl_state;
wchar_t *acl_text_w;
char *acl_text;
int acl_types;
};

void archive_acl_clear(struct archive_acl *);
void archive_acl_copy(struct archive_acl *, struct archive_acl *);
int archive_acl_count(struct archive_acl *, int);
int archive_acl_types(struct archive_acl *);
int archive_acl_reset(struct archive_acl *, int);
int archive_acl_next(struct archive *, struct archive_acl *, int,
int *, int *, int *, int *, const char **);

int archive_acl_add_entry(struct archive_acl *, int, int, int, int, const char *);
int archive_acl_add_entry_w_len(struct archive_acl *,
int, int, int, int, const wchar_t *, size_t);
int archive_acl_add_entry_len(struct archive_acl *,
int, int, int, int, const char *, size_t);

wchar_t *archive_acl_to_text_w(struct archive_acl *, ssize_t *, int,
struct archive *);
char *archive_acl_to_text_l(struct archive_acl *, ssize_t *, int,
struct archive_string_conv *);




int archive_acl_from_text_w(struct archive_acl *, const wchar_t * ,
int );
int archive_acl_from_text_l(struct archive_acl *, const char * ,
int , struct archive_string_conv *);

#endif
