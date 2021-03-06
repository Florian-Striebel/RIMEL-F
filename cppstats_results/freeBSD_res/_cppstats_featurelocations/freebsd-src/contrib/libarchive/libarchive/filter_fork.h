


























#if !defined(FILTER_FORK_H)
#define FILTER_FORK_H

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

int
__archive_create_child(const char *cmd, int *child_stdin, int *child_stdout,
#if defined(_WIN32) && !defined(__CYGWIN__)
HANDLE *out_child);
#else
pid_t *out_child);
#endif

void
__archive_check_child(int in, int out);

#endif
