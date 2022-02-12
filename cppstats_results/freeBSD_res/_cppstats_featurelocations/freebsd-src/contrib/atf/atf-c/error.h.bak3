
























#if !defined(ATF_C_ERROR_H)
#define ATF_C_ERROR_H

#include <atf-c/error_fwd.h>

#include <stdbool.h>
#include <stddef.h>





struct atf_error {
bool m_free;
const char *m_type;
void *m_data;

void (*m_format)(struct atf_error *, char *, size_t);
};

atf_error_t atf_error_new(const char *, void *, size_t,
void (*)(const atf_error_t, char *, size_t));
void atf_error_free(atf_error_t);

atf_error_t atf_no_error(void);
bool atf_is_error(const atf_error_t);

bool atf_error_is(const atf_error_t, const char *);
const void *atf_error_data(const atf_error_t);
void atf_error_format(const atf_error_t, char *, size_t);





atf_error_t atf_libc_error(int, const char *, ...);
int atf_libc_error_code(const atf_error_t);
const char *atf_libc_error_msg(const atf_error_t);

atf_error_t atf_no_memory_error(void);

#endif
