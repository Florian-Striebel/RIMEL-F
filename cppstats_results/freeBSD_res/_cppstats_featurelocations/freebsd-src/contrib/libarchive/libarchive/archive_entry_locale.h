


























#if !defined(ARCHIVE_ENTRY_LOCALE_H_INCLUDED)
#define ARCHIVE_ENTRY_LOCALE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

struct archive_entry;
struct archive_string_conv;












#define archive_entry_gname_l _archive_entry_gname_l
int _archive_entry_gname_l(struct archive_entry *,
const char **, size_t *, struct archive_string_conv *);
#define archive_entry_hardlink_l _archive_entry_hardlink_l
int _archive_entry_hardlink_l(struct archive_entry *,
const char **, size_t *, struct archive_string_conv *);
#define archive_entry_pathname_l _archive_entry_pathname_l
int _archive_entry_pathname_l(struct archive_entry *,
const char **, size_t *, struct archive_string_conv *);
#define archive_entry_symlink_l _archive_entry_symlink_l
int _archive_entry_symlink_l(struct archive_entry *,
const char **, size_t *, struct archive_string_conv *);
#define archive_entry_uname_l _archive_entry_uname_l
int _archive_entry_uname_l(struct archive_entry *,
const char **, size_t *, struct archive_string_conv *);
#define archive_entry_acl_text_l _archive_entry_acl_text_l
int _archive_entry_acl_text_l(struct archive_entry *, int,
const char **, size_t *, struct archive_string_conv *) __LA_DEPRECATED;
#define archive_entry_acl_to_text_l _archive_entry_acl_to_text_l
char *_archive_entry_acl_to_text_l(struct archive_entry *, ssize_t *, int,
struct archive_string_conv *);
#define archive_entry_acl_from_text_l _archive_entry_acl_from_text_l
int _archive_entry_acl_from_text_l(struct archive_entry *, const char* text,
int type, struct archive_string_conv *);
#define archive_entry_copy_gname_l _archive_entry_copy_gname_l
int _archive_entry_copy_gname_l(struct archive_entry *,
const char *, size_t, struct archive_string_conv *);
#define archive_entry_copy_hardlink_l _archive_entry_copy_hardlink_l
int _archive_entry_copy_hardlink_l(struct archive_entry *,
const char *, size_t, struct archive_string_conv *);
#define archive_entry_copy_link_l _archive_entry_copy_link_l
int _archive_entry_copy_link_l(struct archive_entry *,
const char *, size_t, struct archive_string_conv *);
#define archive_entry_copy_pathname_l _archive_entry_copy_pathname_l
int _archive_entry_copy_pathname_l(struct archive_entry *,
const char *, size_t, struct archive_string_conv *);
#define archive_entry_copy_symlink_l _archive_entry_copy_symlink_l
int _archive_entry_copy_symlink_l(struct archive_entry *,
const char *, size_t, struct archive_string_conv *);
#define archive_entry_copy_uname_l _archive_entry_copy_uname_l
int _archive_entry_copy_uname_l(struct archive_entry *,
const char *, size_t, struct archive_string_conv *);

#endif
