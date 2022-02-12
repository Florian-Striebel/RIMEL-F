


























#if !defined(ARCHIVE_CMDLINE_PRIVATE_H)
#define ARCHIVE_CMDLINE_PRIVATE_H

#if !defined(__LIBARCHIVE_BUILD)
#if !defined(__LIBARCHIVE_TEST)
#error This header is only to be used internally to libarchive.
#endif
#endif

struct archive_cmdline {
char *path;
char **argv;
int argc;
};

struct archive_cmdline *__archive_cmdline_allocate(void);
int __archive_cmdline_parse(struct archive_cmdline *, const char *);
int __archive_cmdline_free(struct archive_cmdline *);

#endif
