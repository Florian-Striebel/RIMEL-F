
































#if !defined(bsnmp_support_h_)
#define bsnmp_support_h_

#include <sys/cdefs.h>

#if !defined(HAVE_ERR_H)
void err(int, const char *, ...) __printflike(2, 3) __dead2;
void errx(int, const char *, ...) __printflike(2, 3) __dead2;

void warn(const char *, ...) __printflike(1, 2);
void warnx(const char *, ...) __printflike(1, 2);
#endif

#if !defined(HAVE_STRLCPY)
size_t strlcpy(char *, const char *, size_t);
#endif

#if !defined(HAVE_GETADDRINFO)

struct addrinfo {
u_int ai_flags;
int ai_family;
int ai_socktype;
int ai_protocol;
struct sockaddr *ai_addr;
int ai_addrlen;
struct addrinfo *ai_next;
};
#define AI_CANONNAME 0x0001

int getaddrinfo(const char *, const char *, const struct addrinfo *,
struct addrinfo **);
const char *gai_strerror(int);
void freeaddrinfo(struct addrinfo *);

#endif




#if !defined(INT32_MIN)
#define INT32_MIN (-0x7fffffff-1)
#endif
#if !defined(INT32_MAX)
#define INT32_MAX (0x7fffffff)
#endif
#if !defined(UINT32_MAX)
#define UINT32_MAX (0xffffffff)
#endif




#if !defined(SA_SIZE)

#define SA_SIZE(sa) ( (!(sa) || ((struct sockaddr *)(sa))->sa_len == 0) ? sizeof(long) : 1 + ( (((struct sockaddr *)(sa))->sa_len - 1) | (sizeof(long) - 1) ) )




#endif

#endif
