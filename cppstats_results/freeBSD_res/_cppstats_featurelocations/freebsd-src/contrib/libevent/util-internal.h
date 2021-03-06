
























#if !defined(UTIL_INTERNAL_H_INCLUDED_)
#define UTIL_INTERNAL_H_INCLUDED_

#include "event2/event-config.h"
#include "evconfig-private.h"

#include <errno.h>


#include "log-internal.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(EVENT__HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#if defined(EVENT__HAVE_SYS_EVENTFD_H)
#include <sys/eventfd.h>
#endif
#include "event2/util.h"

#include "time-internal.h"
#include "ipv6-internal.h"

#if defined(__cplusplus)
extern "C" {
#endif


#if defined(EVENT__inline)
#define inline EVENT__inline
#endif
#if defined(EVENT____func__) && !defined(__func__)
#define __func__ EVENT____func__
#endif


#define EVUTIL_NIL_STMT_ ((void)0)





#define EVUTIL_NIL_CONDITION_(condition) do { (void)sizeof(!(condition)); } while(0)











#if !defined(_WIN32)

#if EAGAIN == EWOULDBLOCK
#define EVUTIL_ERR_IS_EAGAIN(e) ((e) == EAGAIN)

#else
#define EVUTIL_ERR_IS_EAGAIN(e) ((e) == EAGAIN || (e) == EWOULDBLOCK)

#endif


#define EVUTIL_ERR_RW_RETRIABLE(e) ((e) == EINTR || EVUTIL_ERR_IS_EAGAIN(e))


#define EVUTIL_ERR_CONNECT_RETRIABLE(e) ((e) == EINTR || (e) == EINPROGRESS)


#define EVUTIL_ERR_ACCEPT_RETRIABLE(e) ((e) == EINTR || EVUTIL_ERR_IS_EAGAIN(e) || (e) == ECONNABORTED)



#define EVUTIL_ERR_CONNECT_REFUSED(e) ((e) == ECONNREFUSED)


#else


#define EVUTIL_ERR_IS_EAGAIN(e) ((e) == WSAEWOULDBLOCK || (e) == EAGAIN)


#define EVUTIL_ERR_RW_RETRIABLE(e) ((e) == WSAEWOULDBLOCK || (e) == WSAEINTR)



#define EVUTIL_ERR_CONNECT_RETRIABLE(e) ((e) == WSAEWOULDBLOCK || (e) == WSAEINTR || (e) == WSAEINPROGRESS || (e) == WSAEINVAL)





#define EVUTIL_ERR_ACCEPT_RETRIABLE(e) EVUTIL_ERR_RW_RETRIABLE(e)


#define EVUTIL_ERR_CONNECT_REFUSED(e) ((e) == WSAECONNREFUSED)


#endif


#if defined(SHUT_RD)
#define EVUTIL_SHUT_RD SHUT_RD
#else
#define EVUTIL_SHUT_RD 0
#endif
#if defined(SHUT_WR)
#define EVUTIL_SHUT_WR SHUT_WR
#else
#define EVUTIL_SHUT_WR 1
#endif
#if defined(SHUT_BOTH)
#define EVUTIL_SHUT_BOTH SHUT_BOTH
#else
#define EVUTIL_SHUT_BOTH 2
#endif







#define EVUTIL_ASSERT_LIST_OK(dlist, type, field) do { struct type *elm1, *elm2, **nextp; if (LIST_EMPTY((dlist))) break; elm1 = LIST_FIRST((dlist)); elm2 = LIST_NEXT(elm1, field); while (elm1 && elm2) { EVUTIL_ASSERT(elm1 != elm2); elm1 = LIST_NEXT(elm1, field); elm2 = LIST_NEXT(elm2, field); if (!elm2) break; EVUTIL_ASSERT(elm1 != elm2); elm2 = LIST_NEXT(elm2, field); } nextp = &LIST_FIRST((dlist)); elm1 = LIST_FIRST((dlist)); while (elm1) { EVUTIL_ASSERT(*nextp == elm1); EVUTIL_ASSERT(nextp == elm1->field.le_prev); nextp = &LIST_NEXT(elm1, field); elm1 = *nextp; } } while (0)



































#define EVUTIL_ASSERT_TAILQ_OK(tailq, type, field) do { struct type *elm1, *elm2, **nextp; if (TAILQ_EMPTY((tailq))) break; elm1 = TAILQ_FIRST((tailq)); elm2 = TAILQ_NEXT(elm1, field); while (elm1 && elm2) { EVUTIL_ASSERT(elm1 != elm2); elm1 = TAILQ_NEXT(elm1, field); elm2 = TAILQ_NEXT(elm2, field); if (!elm2) break; EVUTIL_ASSERT(elm1 != elm2); elm2 = TAILQ_NEXT(elm2, field); } nextp = &TAILQ_FIRST((tailq)); elm1 = TAILQ_FIRST((tailq)); while (elm1) { EVUTIL_ASSERT(*nextp == elm1); EVUTIL_ASSERT(nextp == elm1->field.tqe_prev); nextp = &TAILQ_NEXT(elm1, field); elm1 = *nextp; } EVUTIL_ASSERT(nextp == (tailq)->tqh_last); } while (0)


































int EVUTIL_ISALPHA_(char c);
int EVUTIL_ISALNUM_(char c);
int EVUTIL_ISSPACE_(char c);
int EVUTIL_ISDIGIT_(char c);
int EVUTIL_ISXDIGIT_(char c);
int EVUTIL_ISPRINT_(char c);
int EVUTIL_ISLOWER_(char c);
int EVUTIL_ISUPPER_(char c);
char EVUTIL_TOUPPER_(char c);
char EVUTIL_TOLOWER_(char c);



void evutil_rtrim_lws_(char *);

















#define EVUTIL_UPCAST(ptr, type, field) ((type *)(((char*)(ptr)) - evutil_offsetof(type, field)))





int evutil_open_closeonexec_(const char *pathname, int flags, unsigned mode);

int evutil_read_file_(const char *filename, char **content_out, size_t *len_out,
int is_binary);

int evutil_socket_connect_(evutil_socket_t *fd_ptr, const struct sockaddr *sa, int socklen);

int evutil_socket_finished_connecting_(evutil_socket_t fd);

int evutil_ersatz_socketpair_(int, int , int, evutil_socket_t[]);

int evutil_resolve_(int family, const char *hostname, struct sockaddr *sa,
ev_socklen_t *socklen, int port);

const char *evutil_getenv_(const char *name);



struct evutil_weakrand_state {
ev_uint32_t seed;
};

#define EVUTIL_WEAKRAND_MAX EV_INT32_MAX










ev_uint32_t evutil_weakrand_seed_(struct evutil_weakrand_state *state, ev_uint32_t seed);




ev_int32_t evutil_weakrand_(struct evutil_weakrand_state *seed);



ev_int32_t evutil_weakrand_range_(struct evutil_weakrand_state *seed, ev_int32_t top);



#if defined(__GNUC__) && __GNUC__ >= 3
#define EVUTIL_UNLIKELY(p) __builtin_expect(!!(p),0)
#else
#define EVUTIL_UNLIKELY(p) (p)
#endif


#if defined(NDEBUG)
#define EVUTIL_ASSERT(cond) EVUTIL_NIL_CONDITION_(cond)
#define EVUTIL_FAILURE_CHECK(cond) 0
#else
#define EVUTIL_ASSERT(cond) do { if (EVUTIL_UNLIKELY(!(cond))) { event_errx(EVENT_ERR_ABORT_, "%s:%d: Assertion %s failed in %s", __FILE__,__LINE__,#cond,__func__); (void)fprintf(stderr, "%s:%d: Assertion %s failed in %s", __FILE__,__LINE__,#cond,__func__); abort(); } } while (0)













#define EVUTIL_FAILURE_CHECK(cond) EVUTIL_UNLIKELY(cond)
#endif

#if !defined(EVENT__HAVE_STRUCT_SOCKADDR_STORAGE)



struct sockaddr_storage {
union {
struct sockaddr ss_sa;
struct sockaddr_in ss_sin;
struct sockaddr_in6 ss_sin6;
char ss_padding[128];
} ss_union;
};
#define ss_family ss_union.ss_sa.sa_family
#endif




#define EVUTIL_EAI_NEED_RESOLVE -90002

struct evdns_base;
struct evdns_getaddrinfo_request;
typedef struct evdns_getaddrinfo_request* (*evdns_getaddrinfo_fn)(
struct evdns_base *base,
const char *nodename, const char *servname,
const struct evutil_addrinfo *hints_in,
void (*cb)(int, struct evutil_addrinfo *, void *), void *arg);
void evutil_set_evdns_getaddrinfo_fn_(evdns_getaddrinfo_fn fn);
typedef void (*evdns_getaddrinfo_cancel_fn)(
struct evdns_getaddrinfo_request *req);
void evutil_set_evdns_getaddrinfo_cancel_fn_(evdns_getaddrinfo_cancel_fn fn);

struct evutil_addrinfo *evutil_new_addrinfo_(struct sockaddr *sa,
ev_socklen_t socklen, const struct evutil_addrinfo *hints);
struct evutil_addrinfo *evutil_addrinfo_append_(struct evutil_addrinfo *first,
struct evutil_addrinfo *append);
void evutil_adjust_hints_for_addrconfig_(struct evutil_addrinfo *hints);
int evutil_getaddrinfo_common_(const char *nodename, const char *servname,
struct evutil_addrinfo *hints, struct evutil_addrinfo **res, int *portnum);

struct evdns_getaddrinfo_request *evutil_getaddrinfo_async_(
struct evdns_base *dns_base,
const char *nodename, const char *servname,
const struct evutil_addrinfo *hints_in,
void (*cb)(int, struct evutil_addrinfo *, void *), void *arg);
void evutil_getaddrinfo_cancel_async_(struct evdns_getaddrinfo_request *data);



int evutil_sockaddr_is_loopback_(const struct sockaddr *sa);







const char *evutil_format_sockaddr_port_(const struct sockaddr *sa, char *out, size_t outlen);

int evutil_hex_char_to_int_(char c);


void evutil_free_secure_rng_globals_(void);
void evutil_free_globals_(void);

#if defined(_WIN32)
HMODULE evutil_load_windows_system_library_(const TCHAR *library_name);
#endif

#if !defined(EV_SIZE_FMT)
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#define EV_U64_FMT "%I64u"
#define EV_I64_FMT "%I64d"
#define EV_I64_ARG(x) ((__int64)(x))
#define EV_U64_ARG(x) ((unsigned __int64)(x))
#else
#define EV_U64_FMT "%llu"
#define EV_I64_FMT "%lld"
#define EV_I64_ARG(x) ((long long)(x))
#define EV_U64_ARG(x) ((unsigned long long)(x))
#endif
#endif

#if defined(_WIN32)
#define EV_SOCK_FMT EV_I64_FMT
#define EV_SOCK_ARG(x) EV_I64_ARG((x))
#else
#define EV_SOCK_FMT "%d"
#define EV_SOCK_ARG(x) (x)
#endif

#if defined(__STDC__) && defined(__STDC_VERSION__) && !defined(__MINGW64_VERSION_MAJOR)
#if (__STDC_VERSION__ >= 199901L)
#define EV_SIZE_FMT "%zu"
#define EV_SSIZE_FMT "%zd"
#define EV_SIZE_ARG(x) (x)
#define EV_SSIZE_ARG(x) (x)
#endif
#endif

#if !defined(EV_SIZE_FMT)
#if (EVENT__SIZEOF_SIZE_T <= EVENT__SIZEOF_LONG)
#define EV_SIZE_FMT "%lu"
#define EV_SSIZE_FMT "%ld"
#define EV_SIZE_ARG(x) ((unsigned long)(x))
#define EV_SSIZE_ARG(x) ((long)(x))
#else
#define EV_SIZE_FMT EV_U64_FMT
#define EV_SSIZE_FMT EV_I64_FMT
#define EV_SIZE_ARG(x) EV_U64_ARG(x)
#define EV_SSIZE_ARG(x) EV_I64_ARG(x)
#endif
#endif

evutil_socket_t evutil_socket_(int domain, int type, int protocol);
evutil_socket_t evutil_accept4_(evutil_socket_t sockfd, struct sockaddr *addr,
ev_socklen_t *addrlen, int flags);


EVENT2_EXPORT_SYMBOL
int evutil_make_internal_pipe_(evutil_socket_t fd[2]);
evutil_socket_t evutil_eventfd_(unsigned initval, int flags);

#if defined(SOCK_NONBLOCK)
#define EVUTIL_SOCK_NONBLOCK SOCK_NONBLOCK
#else
#define EVUTIL_SOCK_NONBLOCK 0x4000000
#endif
#if defined(SOCK_CLOEXEC)
#define EVUTIL_SOCK_CLOEXEC SOCK_CLOEXEC
#else
#define EVUTIL_SOCK_CLOEXEC 0x80000000
#endif
#if defined(EFD_NONBLOCK)
#define EVUTIL_EFD_NONBLOCK EFD_NONBLOCK
#else
#define EVUTIL_EFD_NONBLOCK 0x4000
#endif
#if defined(EFD_CLOEXEC)
#define EVUTIL_EFD_CLOEXEC EFD_CLOEXEC
#else
#define EVUTIL_EFD_CLOEXEC 0x8000
#endif

void evutil_memclear_(void *mem, size_t len);

#if defined(__cplusplus)
}
#endif

#endif
