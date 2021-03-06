





































#if !defined(_FAKE_RFC2553_H)
#define _FAKE_RFC2553_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <limits.h>

#if defined(__cplusplus)
extern "C" {
#endif




#if !defined(HAVE_STRUCT_SOCKADDR_STORAGE)
#if !defined(_SS_MAXSIZE)
#define _SS_MAXSIZE 128
#define _SS_PADSIZE (_SS_MAXSIZE - sizeof (struct sockaddr))
struct sockaddr_storage {
struct sockaddr ss_sa;
char __ss_pad2[_SS_PADSIZE];
};
#define ss_family ss_sa.sa_family
#endif
#endif

#if !defined(IN6_IS_ADDR_LOOPBACK)
#define IN6_IS_ADDR_LOOPBACK(a) (((uint32_t *)(a))[0] == 0 && ((uint32_t *)(a))[1] == 0 && ((uint32_t *)(a))[2] == 0 && ((uint32_t *)(a))[3] == htonl(1))


#endif

#if !defined(HAVE_STRUCT_IN6_ADDR)
struct in6_addr {
uint8_t s6_addr[16];
};
#endif

#if !defined(HAVE_STRUCT_SOCKADDR_IN6)
struct sockaddr_in6 {
unsigned short sin6_family;
uint16_t sin6_port;
uint32_t sin6_flowinfo;
struct in6_addr sin6_addr;
};
#endif

#if !defined(AF_INET6)

#define AF_INET6 AF_MAX
#endif





#if !defined(NI_NUMERICHOST)
#define NI_NUMERICHOST (1)
#endif
#if !defined(NI_NAMEREQD)
#define NI_NAMEREQD (1<<1)
#endif
#if !defined(NI_NUMERICSERV)
#define NI_NUMERICSERV (1<<2)
#endif

#if !defined(AI_PASSIVE)
#define AI_PASSIVE (1)
#endif
#if !defined(AI_CANONNAME)
#define AI_CANONNAME (1<<1)
#endif
#if !defined(AI_NUMERICHOST)
#define AI_NUMERICHOST (1<<2)
#endif

#if !defined(NI_MAXSERV)
#define NI_MAXSERV 32
#endif
#if !defined(NI_MAXHOST)
#define NI_MAXHOST 1025
#endif

#if !defined(INT_MAX)
#define INT_MAX 0xffffffff
#endif

#if !defined(EAI_NODATA)
#define EAI_NODATA (INT_MAX - 1)
#endif
#if !defined(EAI_MEMORY)
#define EAI_MEMORY (INT_MAX - 2)
#endif
#if !defined(EAI_NONAME)
#define EAI_NONAME (INT_MAX - 3)
#endif
#if !defined(EAI_SYSTEM)
#define EAI_SYSTEM (INT_MAX - 4)
#endif

#if !defined(HAVE_STRUCT_ADDRINFO)
struct addrinfo {
int ai_flags;
int ai_family;
int ai_socktype;
int ai_protocol;
size_t ai_addrlen;
char *ai_canonname;
struct sockaddr *ai_addr;
struct addrinfo *ai_next;
};
#endif

#if !defined(HAVE_GETADDRINFO)
#if defined(getaddrinfo)
#undef getaddrinfo
#endif
#define getaddrinfo(a,b,c,d) (ssh_getaddrinfo(a,b,c,d))
int getaddrinfo(const char *, const char *,
const struct addrinfo *, struct addrinfo **);
#endif

#if !defined(HAVE_GAI_STRERROR) && !defined(HAVE_CONST_GAI_STRERROR_PROTO)
#define gai_strerror(a) (ssh_gai_strerror(a))
char *gai_strerror(int);
#endif

#if !defined(HAVE_FREEADDRINFO)
#define freeaddrinfo(a) (ssh_freeaddrinfo(a))
void freeaddrinfo(struct addrinfo *);
#endif

#if !defined(HAVE_GETNAMEINFO)
#define getnameinfo(a,b,c,d,e,f,g) (ssh_getnameinfo(a,b,c,d,e,f,g))
int getnameinfo(const struct sockaddr *, size_t, char *, size_t,
char *, size_t, int);
#endif

#if defined(__cplusplus)
}
#endif

#endif

