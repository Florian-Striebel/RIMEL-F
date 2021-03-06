
























#if !defined(ATF_C_UTILS_H)
#define ATF_C_UTILS_H

#include <stdbool.h>
#include <unistd.h>

#include <atf-c/defs.h>

void atf_utils_cat_file(const char *, const char *);
bool atf_utils_compare_file(const char *, const char *);
void atf_utils_copy_file(const char *, const char *);
void atf_utils_create_file(const char *, const char *, ...)
ATF_DEFS_ATTRIBUTE_FORMAT_PRINTF(2, 3);
bool atf_utils_file_exists(const char *);
pid_t atf_utils_fork(void);
void atf_utils_free_charpp(char **);
bool atf_utils_grep_file(const char *, const char *, ...)
ATF_DEFS_ATTRIBUTE_FORMAT_PRINTF(1, 3);
bool atf_utils_grep_string(const char *, const char *, ...)
ATF_DEFS_ATTRIBUTE_FORMAT_PRINTF(1, 3);
char *atf_utils_readline(int);
void atf_utils_redirect(const int, const char *);
void atf_utils_wait(const pid_t, const int, const char *, const char *);
void atf_utils_reset_resultsfile(void);

#endif
