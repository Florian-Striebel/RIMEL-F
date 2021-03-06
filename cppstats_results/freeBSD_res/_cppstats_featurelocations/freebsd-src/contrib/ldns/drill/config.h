



#define HAVE_ARPA_INET_H 1


#define HAVE_ASSERT_H 1


#define HAVE_CTYPE_H 1


#define HAVE_GETADDRINFO 1


#define HAVE_GETOPT_H 1


#define HAVE_HMAC_UPDATE 1


#define HAVE_INTTYPES_H 1


#define HAVE_ISBLANK 1


#define HAVE_LIBLDNS 1


#define HAVE_MEMORY_H 1


#define HAVE_NETINET_IF_ETHER_H 1


#define HAVE_NETINET_IN_H 1


#define HAVE_NETINET_IN_SYSTM_H 1


#define HAVE_NETINET_IP6_H 1


#define HAVE_NETINET_IP_H 1


#define HAVE_NETINET_UDP_H 1


#define HAVE_NET_IF_H 1


#define HAVE_OPENSSL_ERR_H 1


#define HAVE_OPENSSL_RAND_H 1


#define HAVE_OPENSSL_SSL_H 1


#define HAVE_SSL


#define HAVE_STDINT_H 1


#define HAVE_STDIO_H 1


#define HAVE_STDLIB_H 1


#define HAVE_STRINGS_H 1


#define HAVE_STRING_H 1


#define HAVE_SYS_MOUNT_H 1


#define HAVE_SYS_PARAM_H 1


#define HAVE_SYS_SELECT_H 1


#define HAVE_SYS_SOCKET_H 1


#define HAVE_SYS_STAT_H 1


#define HAVE_SYS_TIME_H 1


#define HAVE_SYS_TYPES_H 1


#define HAVE_TIME_H 1


#define HAVE_UNISTD_H 1








#define LDNS_TRUST_ANCHOR_FILE "/etc/unbound/root.key"


#define PACKAGE_BUGREPORT "libdns@nlnetlabs.nl"


#define PACKAGE_NAME "ldns"


#define PACKAGE_STRING "ldns 1.7.0"


#define PACKAGE_TARNAME "libdns"


#define PACKAGE_URL ""


#define PACKAGE_VERSION "1.7.0"


#define STDC_HEADERS 1


#if !defined(_ALL_SOURCE)
#define _ALL_SOURCE 1
#endif

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#if !defined(_POSIX_PTHREAD_SEMANTICS)
#define _POSIX_PTHREAD_SEMANTICS 1
#endif

#if !defined(_TANDEM_SOURCE)
#define _TANDEM_SOURCE 1
#endif

#if !defined(__EXTENSIONS__)
#define __EXTENSIONS__ 1
#endif






#define WINVER 0x0502



















#if !defined(__cplusplus)

#endif




































#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#if STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#endif

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#if defined(HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif

#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif

#if defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

#if defined(HAVE_NETINET_UDP_H)
#include <netinet/udp.h>
#endif

#if defined(HAVE_TIME_H)
#include <time.h>
#endif

#if defined(HAVE_NETINET_IN_SYSTM_H)
#include <netinet/in_systm.h>
#endif

#if defined(HAVE_NETINET_IP_H)
#include <netinet/ip.h>
#endif

#if defined(HAVE_NET_IF_H)
#include <net/if.h>
#endif

#if defined(HAVE_NETINET_IF_ETHER_H)
#include <netinet/if_ether.h>
#endif

#if defined(HAVE_WINSOCK2_H)
#define USE_WINSOCK 1
#include <winsock2.h>
#endif

#if defined(HAVE_WS2TCPIP_H)
#include <ws2tcpip.h>
#endif

#if !defined(EXIT_FAILURE)
#define EXIT_FAILURE 1
#endif
#if !defined(EXIT_SUCCESS)
#define EXIT_SUCCESS 0
#endif

#if defined(S_SPLINT_S)
#define FD_ZERO(a)
#define FD_SET(a,b)
#endif

