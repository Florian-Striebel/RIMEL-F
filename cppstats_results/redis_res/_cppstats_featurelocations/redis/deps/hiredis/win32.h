#if !defined(_WIN32_HELPER_INCLUDE)
#define _WIN32_HELPER_INCLUDE
#if defined(_MSC_VER)

#include <winsock2.h>

#if !defined(inline)
#define inline __inline
#endif

#if !defined(strcasecmp)
#define strcasecmp stricmp
#endif

#if !defined(strncasecmp)
#define strncasecmp strnicmp
#endif

#if !defined(va_copy)
#define va_copy(d,s) ((d) = (s))
#endif

#if !defined(snprintf)
#define snprintf c99_snprintf

__inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
int count = -1;

if (size != 0)
count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
if (count == -1)
count = _vscprintf(format, ap);

return count;
}

__inline int c99_snprintf(char* str, size_t size, const char* format, ...)
{
int count;
va_list ap;

va_start(ap, format);
count = c99_vsnprintf(str, size, format, ap);
va_end(ap);

return count;
}
#endif
#endif

#if defined(_WIN32)
#define strerror_r(errno,buf,len) strerror_s(buf,len,errno)
#endif

#endif
