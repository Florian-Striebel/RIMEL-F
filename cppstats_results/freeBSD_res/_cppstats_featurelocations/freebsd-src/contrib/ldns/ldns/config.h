






#define HAVE_ARPA_INET_H 1


#define HAVE_ATTR_FORMAT 1


#define HAVE_ATTR_UNUSED 1














#define HAVE_BZERO 1


#define HAVE_CALLOC 1


#define HAVE_CTIME_R 1


#define HAVE_DANE_CA_FILE 0


#define HAVE_DANE_CA_PATH 0



#define HAVE_DECL_NID_SECP384R1 1











#define HAVE_DECL_NID_X9_62_PRIME256V1 1


#define HAVE_DLFCN_H 1


#define HAVE_DSA_GET0_KEY 1


#define HAVE_DSA_GET0_PQG 1


#define HAVE_DSA_SIG_GET0 1


#define HAVE_DSA_SIG_SET0 1


#define HAVE_ECDSA_SIG_GET0 1


#define HAVE_ENDPROTOENT 1


#define HAVE_ENDSERVENT 1


#define HAVE_ENGINE_LOAD_CRYPTODEV 1





#define HAVE_EVP_MD_CTX_NEW 1


#define HAVE_EVP_PKEY_BASE_ID 1


#define HAVE_EVP_PKEY_KEYGEN 1


#define HAVE_EVP_SHA256 1


#define HAVE_EVP_SHA384 1


#define HAVE_EVP_SHA512 1


#define HAVE_FCNTL 1


#define HAVE_FORK 1


#define HAVE_GETADDRINFO 1


#define HAVE_GETOPT_H 1


#define HAVE_GMTIME_R 1


#define HAVE_HMAC_UPDATE 1


#define HAVE_INET_ATON 1


#define HAVE_INET_NTOP 1


#define HAVE_INET_PTON 1


#define HAVE_INTTYPES_H 1





#define HAVE_ISASCII 1


#define HAVE_ISBLANK 1








#define HAVE_LOCALTIME_R 1



#define HAVE_MALLOC 1


#define HAVE_MEMMOVE 1


#define HAVE_MEMORY_H 1


#define HAVE_MEMSET 1


#define HAVE_NETDB_H 1








#define HAVE_NETINET_IN_H 1























#define HAVE_OPENSSL_ERR_H 1


#define HAVE_OPENSSL_RAND_H 1


#define HAVE_OPENSSL_SSL_H 1





#define HAVE_POLL 1





#define HAVE_RANDOM 1



#define HAVE_REALLOC 1


#define HAVE_SLEEP 1


#define HAVE_SNPRINTF 1


#define HAVE_SSL


#define HAVE_STDARG_H 1


#define HAVE_STDBOOL_H 1


#define HAVE_STDINT_H 1


#define HAVE_STDLIB_H 1


#define HAVE_STRINGS_H 1


#define HAVE_STRING_H 1


#define HAVE_STRLCPY 1


#define HAVE_STRTOUL 1





#define HAVE_SYS_MOUNT_H 1


#define HAVE_SYS_PARAM_H 1


#define HAVE_SYS_SOCKET_H 1


#define HAVE_SYS_STAT_H 1


#define HAVE_SYS_TYPES_H 1


#define HAVE_TIMEGM 1


#define HAVE_TIME_H 1


#define HAVE_UNISTD_H 1


#define HAVE_VFORK 1








#define HAVE_WORKING_FORK 1


#define HAVE_WORKING_VFORK 1





#define HAVE__BOOL 1








#define LT_OBJDIR ".libs/"


#define PACKAGE_BUGREPORT "libdns@nlnetlabs.nl"


#define PACKAGE_NAME "ldns"


#define PACKAGE_STRING "ldns 1.7.0"


#define PACKAGE_TARNAME "libdns"


#define PACKAGE_URL ""


#define PACKAGE_VERSION "1.7.0"








#define RRTYPE_OPENPGPKEY








#define SIZEOF_TIME_T 8


#define STDC_HEADERS 1





#define SYSCONFDIR sysconfdir


#define USE_DANE 1


#define USE_DANE_TA_USAGE 1


#define USE_DANE_VERIFY 1


#define USE_DSA 1


#define USE_ECDSA 1











#define USE_SHA2 1


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



#if defined AC_APPLE_UNIVERSAL_BUILD
#if defined __BIG_ENDIAN__
#define WORDS_BIGENDIAN 1
#endif
#else
#if !defined(WORDS_BIGENDIAN)

#endif
#endif

























#if !defined(__cplusplus)

#endif





















































#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#if !defined(LITTLE_ENDIAN)
#define LITTLE_ENDIAN 1234
#endif

#if !defined(BIG_ENDIAN)
#define BIG_ENDIAN 4321
#endif

#if !defined(BYTE_ORDER)
#if defined(WORDS_BIGENDIAN)
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#endif

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

#if defined(HAVE_WINSOCK2_H)
#include <winsock2.h>
#endif

#if defined(HAVE_WS2TCPIP_H)
#include <ws2tcpip.h>
#endif



#if defined(HAVE_WINSOCK2_H)
#define FD_SET_T (u_int)
#else
#define FD_SET_T
#endif




#if defined(__cplusplus)
extern "C" {
#endif

int ldns_b64_ntop(uint8_t const *src, size_t srclength,
char *target, size_t targsize);




static inline size_t ldns_b64_ntop_calculate_size(size_t srcsize)
{
return ((((srcsize + 2) / 3) * 4) + 1);
}
int ldns_b64_pton(char const *src, uint8_t *target, size_t targsize);




static inline size_t ldns_b64_pton_calculate_size(size_t srcsize)
{
return (((((srcsize + 3) / 4) * 3)) + 1);
}





int ldns_dname_compare_v(const void *a, const void *b);

#if !defined(HAVE_SLEEP)

#define sleep(x) Sleep((x)*1000)
#endif

#if !defined(HAVE_RANDOM)
#define srandom(x) srand(x)
#define random(x) rand(x)
#endif

#if !defined(HAVE_TIMEGM)
#include <time.h>
time_t timegm (struct tm *tm);
#endif
#if !defined(HAVE_GMTIME_R)
struct tm *gmtime_r(const time_t *timep, struct tm *result);
#endif
#if !defined(HAVE_LOCALTIME_R)
struct tm *localtime_r(const time_t *timep, struct tm *result);
#endif
#if !defined(HAVE_ISBLANK)
int isblank(int c);
#endif
#if !defined(HAVE_ISASCII)
int isascii(int c);
#endif
#if !defined(HAVE_SNPRINTF)
#include <stdarg.h>
int snprintf (char *str, size_t count, const char *fmt, ...);
int vsnprintf (char *str, size_t count, const char *fmt, va_list arg);
#endif
#if !defined(HAVE_INET_PTON)
int inet_pton(int af, const char* src, void* dst);
#endif
#if !defined(HAVE_INET_NTOP)
const char *inet_ntop(int af, const void *src, char *dst, size_t size);
#endif
#if !defined(HAVE_INET_ATON)
int inet_aton(const char *cp, struct in_addr *addr);
#endif
#if !defined(HAVE_MEMMOVE)
void *memmove(void *dest, const void *src, size_t n);
#endif
#if !defined(HAVE_STRLCPY)
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#if defined(USE_WINSOCK)
#define SOCK_INVALID INVALID_SOCKET
#define close_socket(_s) do { if (_s > SOCK_INVALID) {closesocket(_s); _s = SOCK_INVALID;} } while(0)
#else
#define SOCK_INVALID -1
#define close_socket(_s) do { if (_s > SOCK_INVALID) {close(_s); _s = SOCK_INVALID;} } while(0)
#endif

#if defined(__cplusplus)
}
#endif
#if !defined(HAVE_GETADDRINFO)
#include "compat/fake-rfc2553.h"
#endif
#if !defined(HAVE_STRTOUL)
#define strtoul (unsigned long)strtol
#endif

