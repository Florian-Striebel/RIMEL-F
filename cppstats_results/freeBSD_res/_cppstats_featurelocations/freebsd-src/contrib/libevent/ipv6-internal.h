




























#if !defined(IPV6_INTERNAL_H_INCLUDED_)
#define IPV6_INTERNAL_H_INCLUDED_

#include "event2/event-config.h"
#include "evconfig-private.h"

#include <sys/types.h>
#if defined(EVENT__HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#include "event2/util.h"

#if defined(__cplusplus)
extern "C" {
#endif







#if !defined(EVENT__HAVE_STRUCT_IN6_ADDR)
struct in6_addr {
ev_uint8_t s6_addr[16];
};
#endif

#if !defined(EVENT__HAVE_SA_FAMILY_T)
typedef int sa_family_t;
#endif

#if !defined(EVENT__HAVE_STRUCT_SOCKADDR_IN6)
struct sockaddr_in6 {


sa_family_t sin6_family;
ev_uint16_t sin6_port;
struct in6_addr sin6_addr;
};
#endif

#if !defined(AF_INET6)
#define AF_INET6 3333
#endif
#if !defined(PF_INET6)
#define PF_INET6 AF_INET6
#endif

#if defined(__cplusplus)
}
#endif

#endif
