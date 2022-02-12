
























#if !defined(ATF_C_DETAIL_DYNSTR_H)
#define ATF_C_DETAIL_DYNSTR_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <atf-c/error_fwd.h>





struct atf_dynstr {
char *m_data;
size_t m_datasize;
size_t m_length;
};
typedef struct atf_dynstr atf_dynstr_t;


extern const size_t atf_dynstr_npos;


atf_error_t atf_dynstr_init(atf_dynstr_t *);
atf_error_t atf_dynstr_init_ap(atf_dynstr_t *, const char *, va_list);
atf_error_t atf_dynstr_init_fmt(atf_dynstr_t *, const char *, ...);
atf_error_t atf_dynstr_init_raw(atf_dynstr_t *, const void *, size_t);
atf_error_t atf_dynstr_init_rep(atf_dynstr_t *, size_t, char);
atf_error_t atf_dynstr_init_substr(atf_dynstr_t *, const atf_dynstr_t *,
size_t, size_t);
atf_error_t atf_dynstr_copy(atf_dynstr_t *, const atf_dynstr_t *);
void atf_dynstr_fini(atf_dynstr_t *);
char *atf_dynstr_fini_disown(atf_dynstr_t *);


const char *atf_dynstr_cstring(const atf_dynstr_t *);
size_t atf_dynstr_length(const atf_dynstr_t *);
size_t atf_dynstr_rfind_ch(const atf_dynstr_t *, char);


atf_error_t atf_dynstr_append_ap(atf_dynstr_t *, const char *, va_list);
atf_error_t atf_dynstr_append_fmt(atf_dynstr_t *, const char *, ...);
void atf_dynstr_clear(atf_dynstr_t *);
atf_error_t atf_dynstr_prepend_ap(atf_dynstr_t *, const char *, va_list);
atf_error_t atf_dynstr_prepend_fmt(atf_dynstr_t *, const char *, ...);


bool atf_equal_dynstr_cstring(const atf_dynstr_t *, const char *);
bool atf_equal_dynstr_dynstr(const atf_dynstr_t *, const atf_dynstr_t *);

#endif
