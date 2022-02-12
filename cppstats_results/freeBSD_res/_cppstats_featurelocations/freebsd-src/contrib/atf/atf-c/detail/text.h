
























#if !defined(ATF_C_DETAIL_TEXT_H)
#define ATF_C_DETAIL_TEXT_H

#include <stdarg.h>
#include <stdbool.h>

#include <atf-c/detail/list.h>
#include <atf-c/error_fwd.h>

atf_error_t atf_text_for_each_word(const char *, const char *,
atf_error_t (*)(const char *, void *),
void *);
atf_error_t atf_text_format(char **, const char *, ...);
atf_error_t atf_text_format_ap(char **, const char *, va_list);
atf_error_t atf_text_split(const char *, const char *, atf_list_t *);
atf_error_t atf_text_to_bool(const char *, bool *);
atf_error_t atf_text_to_long(const char *, long *);

#endif
