














#if !defined(SANITIZER_LIBC_H)
#define SANITIZER_LIBC_H



#include "sanitizer_internal_defs.h"

namespace __sanitizer {




s64 internal_atoll(const char *nptr);
void *internal_memchr(const void *s, int c, uptr n);
void *internal_memrchr(const void *s, int c, uptr n);
int internal_memcmp(const void* s1, const void* s2, uptr n);
void *internal_memcpy(void *dest, const void *src, uptr n);
void *internal_memmove(void *dest, const void *src, uptr n);

void *internal_memset(void *s, int c, uptr n);
char* internal_strchr(const char *s, int c);
char *internal_strchrnul(const char *s, int c);
int internal_strcmp(const char *s1, const char *s2);
uptr internal_strcspn(const char *s, const char *reject);
char *internal_strdup(const char *s);
uptr internal_strlen(const char *s);
uptr internal_strlcat(char *dst, const char *src, uptr maxlen);
char *internal_strncat(char *dst, const char *src, uptr n);
int internal_strncmp(const char *s1, const char *s2, uptr n);
uptr internal_strlcpy(char *dst, const char *src, uptr maxlen);
char *internal_strncpy(char *dst, const char *src, uptr n);
uptr internal_strnlen(const char *s, uptr maxlen);
char *internal_strrchr(const char *s, int c);
char *internal_strstr(const char *haystack, const char *needle);

s64 internal_simple_strtoll(const char *nptr, const char **endptr, int base);
int internal_snprintf(char *buffer, uptr length, const char *format, ...);



bool mem_is_zero(const char *mem, uptr size);




#define kInvalidFd ((fd_t)-1)
#define kStdinFd ((fd_t)0)
#define kStdoutFd ((fd_t)1)
#define kStderrFd ((fd_t)2)

uptr internal_ftruncate(fd_t fd, uptr size);


void NORETURN internal__exit(int exitcode);
void internal_sleep(unsigned seconds);
void internal_usleep(u64 useconds);

uptr internal_getpid();
uptr internal_getppid();

int internal_dlinfo(void *handle, int request, void *p);


uptr internal_sched_yield();


bool internal_iserror(uptr retval, int *rverrno = nullptr);

}

#endif
