
































#if !defined(portability_h)
#define portability_h





#include <stdarg.h>

#include "pcap/funcattrs.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(HAVE_STRLCAT)
#define pcap_strlcat strlcat
#else
#if defined(_MSC_VER) || defined(__MINGW32__)




#define pcap_strlcat(x, y, z) strncat_s((x), (z), (y), _TRUNCATE)

#else



extern size_t pcap_strlcat(char * restrict dst, const char * restrict src, size_t dstsize);
#endif
#endif

#if defined(HAVE_STRLCPY)
#define pcap_strlcpy strlcpy
#else
#if defined(_MSC_VER) || defined(__MINGW32__)




#define pcap_strlcpy(x, y, z) strncpy_s((x), (z), (y), _TRUNCATE)

#else



extern size_t pcap_strlcpy(char * restrict dst, const char * restrict src, size_t dstsize);
#endif
#endif

#if defined(_MSC_VER)
#define isascii __isascii







#if !defined(strdup)
#define strdup _strdup
#endif
#endif























#if defined(HAVE_SNPRINTF)
#define pcap_snprintf snprintf
#else
extern int pcap_snprintf(char *, size_t, PCAP_FORMAT_STRING(const char *), ...)
PCAP_PRINTFLIKE(3, 4);
#endif

#if defined(HAVE_VSNPRINTF)
#define pcap_vsnprintf vsnprintf
#else
extern int pcap_vsnprintf(char *, size_t, const char *, va_list ap);
#endif





#if defined(HAVE_ASPRINTF)
#define pcap_asprintf asprintf
#else
extern int pcap_asprintf(char **, PCAP_FORMAT_STRING(const char *), ...)
PCAP_PRINTFLIKE(2, 3);
#endif

#if defined(HAVE_VASPRINTF)
#define pcap_vasprintf vasprintf
#else
extern int pcap_vasprintf(char **, const char *, va_list ap);
#endif

#if defined(HAVE_STRTOK_R)
#define pcap_strtok_r strtok_r
#else
#if defined(_WIN32)



#define pcap_strtok_r strtok_s
#else



extern char *pcap_strtok_r(char *, const char *, char **);
#endif
#endif

#if defined(_WIN32)
#if !defined(__cplusplus)
#define inline __inline
#endif
#endif

#if defined(__cplusplus)
}
#endif

#endif
