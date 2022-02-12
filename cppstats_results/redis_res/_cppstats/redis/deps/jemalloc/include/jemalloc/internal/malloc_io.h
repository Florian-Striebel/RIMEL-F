#if defined(_WIN32)
#if defined(_WIN64)
#define FMT64_PREFIX "ll"
#define FMTPTR_PREFIX "ll"
#else
#define FMT64_PREFIX "ll"
#define FMTPTR_PREFIX ""
#endif
#define FMTd32 "d"
#define FMTu32 "u"
#define FMTx32 "x"
#define FMTd64 FMT64_PREFIX "d"
#define FMTu64 FMT64_PREFIX "u"
#define FMTx64 FMT64_PREFIX "x"
#define FMTdPTR FMTPTR_PREFIX "d"
#define FMTuPTR FMTPTR_PREFIX "u"
#define FMTxPTR FMTPTR_PREFIX "x"
#else
#include <inttypes.h>
#define FMTd32 PRId32
#define FMTu32 PRIu32
#define FMTx32 PRIx32
#define FMTd64 PRId64
#define FMTu64 PRIu64
#define FMTx64 PRIx64
#define FMTdPTR PRIdPTR
#define FMTuPTR PRIuPTR
#define FMTxPTR PRIxPTR
#endif
#define BUFERROR_BUF 64
#define MALLOC_PRINTF_BUFSIZE 4096
int buferror(int err, char *buf, size_t buflen);
uintmax_t malloc_strtoumax(const char *restrict nptr, char **restrict endptr,
int base);
void malloc_write(const char *s);
size_t malloc_vsnprintf(char *str, size_t size, const char *format,
va_list ap);
size_t malloc_snprintf(char *str, size_t size, const char *format, ...)
JEMALLOC_FORMAT_PRINTF(3, 4);
void malloc_vcprintf(void (*write_cb)(void *, const char *), void *cbopaque,
const char *format, va_list ap);
void malloc_cprintf(void (*write_cb)(void *, const char *), void *cbopaque,
const char *format, ...) JEMALLOC_FORMAT_PRINTF(3, 4);
void malloc_printf(const char *format, ...) JEMALLOC_FORMAT_PRINTF(1, 2);
static inline ssize_t
malloc_write_fd(int fd, const void *buf, size_t count) {
#if defined(JEMALLOC_USE_SYSCALL) && defined(SYS_write)
long result = syscall(SYS_write, fd, buf, count);
#else
ssize_t result = (ssize_t)write(fd, buf,
#if defined(_WIN32)
(unsigned int)
#endif
count);
#endif
return (ssize_t)result;
}
static inline ssize_t
malloc_read_fd(int fd, void *buf, size_t count) {
#if defined(JEMALLOC_USE_SYSCALL) && defined(SYS_read)
long result = syscall(SYS_read, fd, buf, count);
#else
ssize_t result = read(fd, buf,
#if defined(_WIN32)
(unsigned int)
#endif
count);
#endif
return (ssize_t)result;
}
