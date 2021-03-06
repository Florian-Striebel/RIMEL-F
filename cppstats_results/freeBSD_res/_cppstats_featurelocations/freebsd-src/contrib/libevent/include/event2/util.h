
























#if !defined(EVENT2_UTIL_H_INCLUDED_)
#define EVENT2_UTIL_H_INCLUDED_







#include <event2/visibility.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <event2/event-config.h>
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(EVENT__HAVE_STDINT_H)
#include <stdint.h>
#elif defined(EVENT__HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#if defined(EVENT__HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(EVENT__HAVE_STDDEF_H)
#include <stddef.h>
#endif
#if defined(_MSC_VER)
#include <BaseTsd.h>
#endif
#include <stdarg.h>
#if defined(EVENT__HAVE_NETDB_H)
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <netdb.h>
#endif

#if defined(_WIN32)
#include <winsock2.h>
#if defined(EVENT__HAVE_GETADDRINFO)

#include <ws2tcpip.h>
#endif
#else
#if defined(EVENT__HAVE_ERRNO_H)
#include <errno.h>
#endif
#include <sys/socket.h>
#endif

#include <time.h>


#if defined(EVENT__SIZEOF_VOID__) && !defined(EVENT__SIZEOF_VOID_P)
#define EVENT__SIZEOF_VOID_P EVENT__SIZEOF_VOID__
#endif






























#if defined(EVENT__HAVE_UINT64_T)
#define ev_uint64_t uint64_t
#define ev_int64_t int64_t
#elif defined(_WIN32)
#define ev_uint64_t unsigned __int64
#define ev_int64_t signed __int64
#elif EVENT__SIZEOF_LONG_LONG == 8
#define ev_uint64_t unsigned long long
#define ev_int64_t long long
#elif EVENT__SIZEOF_LONG == 8
#define ev_uint64_t unsigned long
#define ev_int64_t long
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint64_t ...
#define ev_int64_t ...
#else
#error "No way to define ev_uint64_t"
#endif

#if defined(EVENT__HAVE_UINT32_T)
#define ev_uint32_t uint32_t
#define ev_int32_t int32_t
#elif defined(_WIN32)
#define ev_uint32_t unsigned int
#define ev_int32_t signed int
#elif EVENT__SIZEOF_LONG == 4
#define ev_uint32_t unsigned long
#define ev_int32_t signed long
#elif EVENT__SIZEOF_INT == 4
#define ev_uint32_t unsigned int
#define ev_int32_t signed int
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint32_t ...
#define ev_int32_t ...
#else
#error "No way to define ev_uint32_t"
#endif

#if defined(EVENT__HAVE_UINT16_T)
#define ev_uint16_t uint16_t
#define ev_int16_t int16_t
#elif defined(_WIN32)
#define ev_uint16_t unsigned short
#define ev_int16_t signed short
#elif EVENT__SIZEOF_INT == 2
#define ev_uint16_t unsigned int
#define ev_int16_t signed int
#elif EVENT__SIZEOF_SHORT == 2
#define ev_uint16_t unsigned short
#define ev_int16_t signed short
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint16_t ...
#define ev_int16_t ...
#else
#error "No way to define ev_uint16_t"
#endif

#if defined(EVENT__HAVE_UINT8_T)
#define ev_uint8_t uint8_t
#define ev_int8_t int8_t
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uint8_t ...
#define ev_int8_t ...
#else
#define ev_uint8_t unsigned char
#define ev_int8_t signed char
#endif

#if defined(EVENT__HAVE_UINTPTR_T)
#define ev_uintptr_t uintptr_t
#define ev_intptr_t intptr_t
#elif EVENT__SIZEOF_VOID_P <= 4
#define ev_uintptr_t ev_uint32_t
#define ev_intptr_t ev_int32_t
#elif EVENT__SIZEOF_VOID_P <= 8
#define ev_uintptr_t ev_uint64_t
#define ev_intptr_t ev_int64_t
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_uintptr_t ...
#define ev_intptr_t ...
#else
#error "No way to define ev_uintptr_t"
#endif

#if defined(EVENT__ssize_t)
#define ev_ssize_t EVENT__ssize_t
#else
#define ev_ssize_t ssize_t
#endif








#if defined(_WIN32)
#define ev_off_t ev_int64_t
#elif EVENT__SIZEOF_OFF_T == 8
#define ev_off_t ev_int64_t
#elif EVENT__SIZEOF_OFF_T == 4
#define ev_off_t ev_int32_t
#elif defined(EVENT_IN_DOXYGEN_)
#define ev_off_t ...
#else
#define ev_off_t off_t
#endif

















#if !defined(EVENT__HAVE_STDINT_H)
#define EV_UINT64_MAX ((((ev_uint64_t)0xffffffffUL) << 32) | 0xffffffffUL)
#define EV_INT64_MAX ((((ev_int64_t) 0x7fffffffL) << 32) | 0xffffffffL)
#define EV_INT64_MIN ((-EV_INT64_MAX) - 1)
#define EV_UINT32_MAX ((ev_uint32_t)0xffffffffUL)
#define EV_INT32_MAX ((ev_int32_t) 0x7fffffffL)
#define EV_INT32_MIN ((-EV_INT32_MAX) - 1)
#define EV_UINT16_MAX ((ev_uint16_t)0xffffUL)
#define EV_INT16_MAX ((ev_int16_t) 0x7fffL)
#define EV_INT16_MIN ((-EV_INT16_MAX) - 1)
#define EV_UINT8_MAX 255
#define EV_INT8_MAX 127
#define EV_INT8_MIN ((-EV_INT8_MAX) - 1)
#else
#define EV_UINT64_MAX UINT64_MAX
#define EV_INT64_MAX INT64_MAX
#define EV_INT64_MIN INT64_MIN
#define EV_UINT32_MAX UINT32_MAX
#define EV_INT32_MAX INT32_MAX
#define EV_INT32_MIN INT32_MIN
#define EV_UINT16_MAX UINT16_MAX
#define EV_INT16_MAX INT16_MAX
#define EV_UINT8_MAX UINT8_MAX
#define EV_INT8_MAX INT8_MAX
#define EV_INT8_MIN INT8_MIN

#endif







#if EVENT__SIZEOF_SIZE_T == 8
#define EV_SIZE_MAX EV_UINT64_MAX
#define EV_SSIZE_MAX EV_INT64_MAX
#elif EVENT__SIZEOF_SIZE_T == 4
#define EV_SIZE_MAX EV_UINT32_MAX
#define EV_SSIZE_MAX EV_INT32_MAX
#elif defined(EVENT_IN_DOXYGEN_)
#define EV_SIZE_MAX ...
#define EV_SSIZE_MAX ...
#else
#error "No way to define SIZE_MAX"
#endif

#define EV_SSIZE_MIN ((-EV_SSIZE_MAX) - 1)


#if defined(_WIN32)
#define ev_socklen_t int
#elif defined(EVENT__socklen_t)
#define ev_socklen_t EVENT__socklen_t
#else
#define ev_socklen_t socklen_t
#endif

#if defined(EVENT__HAVE_STRUCT_SOCKADDR_STORAGE___SS_FAMILY)
#if !defined(EVENT__HAVE_STRUCT_SOCKADDR_STORAGE_SS_FAMILY) && !defined(ss_family)

#define ss_family __ss_family
#endif
#endif




#if defined(_WIN32)
#define evutil_socket_t intptr_t
#else
#define evutil_socket_t int
#endif













struct evutil_monotonic_timer
#if defined(EVENT_IN_DOXYGEN_)
{}
#endif
;

#define EV_MONOT_PRECISE 1
#define EV_MONOT_FALLBACK 2







EVENT2_EXPORT_SYMBOL int
evutil_date_rfc1123(char *date, const size_t datelen, const struct tm *tm);






EVENT2_EXPORT_SYMBOL
struct evutil_monotonic_timer * evutil_monotonic_timer_new(void);




EVENT2_EXPORT_SYMBOL
void evutil_monotonic_timer_free(struct evutil_monotonic_timer *timer);




EVENT2_EXPORT_SYMBOL
int evutil_configure_monotonic_time(struct evutil_monotonic_timer *timer,
int flags);











EVENT2_EXPORT_SYMBOL
int evutil_gettime_monotonic(struct evutil_monotonic_timer *timer,
struct timeval *tp);












EVENT2_EXPORT_SYMBOL
int evutil_socketpair(int d, int type, int protocol, evutil_socket_t sv[2]);





EVENT2_EXPORT_SYMBOL
int evutil_make_socket_nonblocking(evutil_socket_t sock);












EVENT2_EXPORT_SYMBOL
int evutil_make_listen_socket_reuseable(evutil_socket_t sock);











EVENT2_EXPORT_SYMBOL
int evutil_make_listen_socket_reuseable_port(evutil_socket_t sock);







EVENT2_EXPORT_SYMBOL
int evutil_make_socket_closeonexec(evutil_socket_t sock);







EVENT2_EXPORT_SYMBOL
int evutil_closesocket(evutil_socket_t sock);
#define EVUTIL_CLOSESOCKET(s) evutil_closesocket(s)












EVENT2_EXPORT_SYMBOL
int evutil_make_tcp_listen_socket_deferred(evutil_socket_t sock);

#if defined(_WIN32)

#define EVUTIL_SOCKET_ERROR() WSAGetLastError()

#define EVUTIL_SET_SOCKET_ERROR(errcode) do { WSASetLastError(errcode); } while (0)


EVENT2_EXPORT_SYMBOL
int evutil_socket_geterror(evutil_socket_t sock);

EVENT2_EXPORT_SYMBOL
const char *evutil_socket_error_to_string(int errcode);
#elif defined(EVENT_IN_DOXYGEN_)















#define EVUTIL_SOCKET_ERROR() ...

#define EVUTIL_SET_SOCKET_ERROR(errcode) ...

#define evutil_socket_geterror(sock) ...

#define evutil_socket_error_to_string(errcode) ...

#else
#define EVUTIL_SOCKET_ERROR() (errno)
#define EVUTIL_SET_SOCKET_ERROR(errcode) do { errno = (errcode); } while (0)

#define evutil_socket_geterror(sock) (errno)
#define evutil_socket_error_to_string(errcode) (strerror(errcode))
#endif










#if defined(EVENT__HAVE_TIMERADD)
#define evutil_timeradd(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))
#define evutil_timersub(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))
#else
#define evutil_timeradd(tvp, uvp, vvp) do { (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec; (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec; if ((vvp)->tv_usec >= 1000000) { (vvp)->tv_sec++; (vvp)->tv_usec -= 1000000; } } while (0)








#define evutil_timersub(tvp, uvp, vvp) do { (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec; (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec; if ((vvp)->tv_usec < 0) { (vvp)->tv_sec--; (vvp)->tv_usec += 1000000; } } while (0)








#endif

#if defined(EVENT__HAVE_TIMERCLEAR)
#define evutil_timerclear(tvp) timerclear(tvp)
#else
#define evutil_timerclear(tvp) (tvp)->tv_sec = (tvp)->tv_usec = 0
#endif




#define evutil_timercmp(tvp, uvp, cmp) (((tvp)->tv_sec == (uvp)->tv_sec) ? ((tvp)->tv_usec cmp (uvp)->tv_usec) : ((tvp)->tv_sec cmp (uvp)->tv_sec))




#if defined(EVENT__HAVE_TIMERISSET)
#define evutil_timerisset(tvp) timerisset(tvp)
#else
#define evutil_timerisset(tvp) ((tvp)->tv_sec || (tvp)->tv_usec)
#endif


#if defined(offsetof)
#define evutil_offsetof(type, field) offsetof(type, field)
#else
#define evutil_offsetof(type, field) ((off_t)(&((type *)0)->field))
#endif



EVENT2_EXPORT_SYMBOL
ev_int64_t evutil_strtoll(const char *s, char **endptr, int base);


#if defined(EVENT__HAVE_GETTIMEOFDAY)
#define evutil_gettimeofday(tv, tz) gettimeofday((tv), (tz))
#else
struct timezone;
EVENT2_EXPORT_SYMBOL
int evutil_gettimeofday(struct timeval *tv, struct timezone *tz);
#endif




EVENT2_EXPORT_SYMBOL
int evutil_snprintf(char *buf, size_t buflen, const char *format, ...)
#if defined(__GNUC__)
__attribute__((format(printf, 3, 4)))
#endif
;



EVENT2_EXPORT_SYMBOL
int evutil_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap)
#if defined(__GNUC__)
__attribute__((format(printf, 3, 0)))
#endif
;


EVENT2_EXPORT_SYMBOL
const char *evutil_inet_ntop(int af, const void *src, char *dst, size_t len);

EVENT2_EXPORT_SYMBOL
int evutil_inet_pton(int af, const char *src, void *dst);
struct sockaddr;





















EVENT2_EXPORT_SYMBOL
int evutil_parse_sockaddr_port(const char *str, struct sockaddr *out, int *outlen);






EVENT2_EXPORT_SYMBOL
int evutil_sockaddr_cmp(const struct sockaddr *sa1, const struct sockaddr *sa2,
int include_port);




EVENT2_EXPORT_SYMBOL
int evutil_ascii_strcasecmp(const char *str1, const char *str2);



EVENT2_EXPORT_SYMBOL
int evutil_ascii_strncasecmp(const char *str1, const char *str2, size_t n);



#if defined(EVENT__HAVE_STRUCT_ADDRINFO)
#define evutil_addrinfo addrinfo
#else





struct evutil_addrinfo {
int ai_flags;
int ai_family;
int ai_socktype;
int ai_protocol;
size_t ai_addrlen;
char *ai_canonname;
struct sockaddr *ai_addr;
struct evutil_addrinfo *ai_next;
};
#endif







#if defined(EAI_ADDRFAMILY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_ADDRFAMILY EAI_ADDRFAMILY
#else
#define EVUTIL_EAI_ADDRFAMILY -901
#endif
#if defined(EAI_AGAIN) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_AGAIN EAI_AGAIN
#else
#define EVUTIL_EAI_AGAIN -902
#endif
#if defined(EAI_BADFLAGS) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_BADFLAGS EAI_BADFLAGS
#else
#define EVUTIL_EAI_BADFLAGS -903
#endif
#if defined(EAI_FAIL) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_FAIL EAI_FAIL
#else
#define EVUTIL_EAI_FAIL -904
#endif
#if defined(EAI_FAMILY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_FAMILY EAI_FAMILY
#else
#define EVUTIL_EAI_FAMILY -905
#endif
#if defined(EAI_MEMORY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_MEMORY EAI_MEMORY
#else
#define EVUTIL_EAI_MEMORY -906
#endif



#if defined(EAI_NODATA) && defined(EVENT__HAVE_GETADDRINFO) && (!defined(EAI_NONAME) || EAI_NODATA != EAI_NONAME)
#define EVUTIL_EAI_NODATA EAI_NODATA
#else
#define EVUTIL_EAI_NODATA -907
#endif
#if defined(EAI_NONAME) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_NONAME EAI_NONAME
#else
#define EVUTIL_EAI_NONAME -908
#endif
#if defined(EAI_SERVICE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SERVICE EAI_SERVICE
#else
#define EVUTIL_EAI_SERVICE -909
#endif
#if defined(EAI_SOCKTYPE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SOCKTYPE EAI_SOCKTYPE
#else
#define EVUTIL_EAI_SOCKTYPE -910
#endif
#if defined(EAI_SYSTEM) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SYSTEM EAI_SYSTEM
#else
#define EVUTIL_EAI_SYSTEM -911
#endif

#define EVUTIL_EAI_CANCEL -90001

#if defined(AI_PASSIVE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_PASSIVE AI_PASSIVE
#else
#define EVUTIL_AI_PASSIVE 0x1000
#endif
#if defined(AI_CANONNAME) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_CANONNAME AI_CANONNAME
#else
#define EVUTIL_AI_CANONNAME 0x2000
#endif
#if defined(AI_NUMERICHOST) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_NUMERICHOST AI_NUMERICHOST
#else
#define EVUTIL_AI_NUMERICHOST 0x4000
#endif
#if defined(AI_NUMERICSERV) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_NUMERICSERV AI_NUMERICSERV
#else
#define EVUTIL_AI_NUMERICSERV 0x8000
#endif
#if defined(AI_V4MAPPED) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_V4MAPPED AI_V4MAPPED
#else
#define EVUTIL_AI_V4MAPPED 0x10000
#endif
#if defined(AI_ALL) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_ALL AI_ALL
#else
#define EVUTIL_AI_ALL 0x20000
#endif
#if defined(AI_ADDRCONFIG) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_ADDRCONFIG AI_ADDRCONFIG
#else
#define EVUTIL_AI_ADDRCONFIG 0x40000
#endif


struct evutil_addrinfo;











EVENT2_EXPORT_SYMBOL
int evutil_getaddrinfo(const char *nodename, const char *servname,
const struct evutil_addrinfo *hints_in, struct evutil_addrinfo **res);


EVENT2_EXPORT_SYMBOL
void evutil_freeaddrinfo(struct evutil_addrinfo *ai);

EVENT2_EXPORT_SYMBOL
const char *evutil_gai_strerror(int err);










EVENT2_EXPORT_SYMBOL
void evutil_secure_rng_get_bytes(void *buf, size_t n);

















EVENT2_EXPORT_SYMBOL
int evutil_secure_rng_init(void);














EVENT2_EXPORT_SYMBOL
int evutil_secure_rng_set_urandom_device_file(char *fname);















EVENT2_EXPORT_SYMBOL
void evutil_secure_rng_add_bytes(const char *dat, size_t datlen);

#if defined(__cplusplus)
}
#endif

#endif
