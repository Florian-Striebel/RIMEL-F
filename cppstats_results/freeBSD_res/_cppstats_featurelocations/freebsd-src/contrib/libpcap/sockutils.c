































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

















#include "ftmacros.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#else
#define INT_MAX 2147483647
#endif

#include "pcap-int.h"

#include "sockutils.h"
#include "portability.h"

#if defined(_WIN32)





#define WINSOCK_MAJOR_VERSION 2
#define WINSOCK_MINOR_VERSION 2

static int sockcount = 0;
#endif


#if defined(_WIN32)
#define SHUT_WR SD_SEND
#endif


#define SOCK_ERRBUF_SIZE 1024


#define SOCKET_NO_NAME_AVAILABLE "No name available"
#define SOCKET_NO_PORT_AVAILABLE "No port available"
#define SOCKET_NAME_NULL_DAD "Null address (possibly DAD Phase)"















#if defined(_WIN32) && !defined(_SSIZE_T_DEFINED)
typedef int ssize_t;
#endif







static int sock_ismcastaddr(const struct sockaddr *saddr);











void sock_fmterror(const char *caller, int errcode, char *errbuf, int errbuflen)
{
if (errbuf == NULL)
return;

#if defined(_WIN32)
pcap_fmt_errmsg_for_win32_err(errbuf, errbuflen, errcode,
"%s", caller);
#else
pcap_fmt_errmsg_for_errno(errbuf, errbuflen, errcode,
"%s", caller);
#endif
}





















void sock_geterror(const char *caller, char *errbuf, int errbuflen)
{
#if defined(_WIN32)
sock_fmterror(caller, GetLastError(), errbuf, errbuflen);
#else
sock_fmterror(caller, errno, errbuf, errbuflen);
#endif
}




















#if defined(_WIN32)
int sock_init(char *errbuf, int errbuflen)
{
if (sockcount == 0)
{
WSADATA wsaData;

if (WSAStartup(MAKEWORD(WINSOCK_MAJOR_VERSION,
WINSOCK_MINOR_VERSION), &wsaData) != 0)
{
if (errbuf)
pcap_snprintf(errbuf, errbuflen, "Failed to initialize Winsock\n");

WSACleanup();

return -1;
}
}

sockcount++;
return 0;
}
#else
int sock_init(char *errbuf _U_, int errbuflen _U_)
{



return 0;
}
#endif










void sock_cleanup(void)
{
#if defined(_WIN32)
sockcount--;

if (sockcount == 0)
WSACleanup();
#endif
}






static int sock_ismcastaddr(const struct sockaddr *saddr)
{
if (saddr->sa_family == PF_INET)
{
struct sockaddr_in *saddr4 = (struct sockaddr_in *) saddr;
if (IN_MULTICAST(ntohl(saddr4->sin_addr.s_addr))) return 0;
else return -1;
}
else
{
struct sockaddr_in6 *saddr6 = (struct sockaddr_in6 *) saddr;
if (IN6_IS_ADDR_MULTICAST(&saddr6->sin6_addr)) return 0;
else return -1;
}
}
































SOCKET sock_open(struct addrinfo *addrinfo, int server, int nconn, char *errbuf, int errbuflen)
{
SOCKET sock;
#if defined(SO_NOSIGPIPE) || defined(IPV6_V6ONLY) || defined(IPV6_BINDV6ONLY)
int on = 1;
#endif

sock = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
if (sock == INVALID_SOCKET)
{
sock_geterror("socket()", errbuf, errbuflen);
return INVALID_SOCKET;
}







#if defined(SO_NOSIGPIPE)
if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (char *)&on,
sizeof (int)) == -1)
{
sock_geterror("setsockopt(SO_NOSIGPIPE)", errbuf, errbuflen);
closesocket(sock);
return INVALID_SOCKET;
}
#endif


if (server)
{






int optval = 1;
(void)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
(char *)&optval, sizeof (optval));

#if defined(IPV6_V6ONLY) || defined(IPV6_BINDV6ONLY)




























#if !defined(IPV6_V6ONLY)

#define IPV6_V6ONLY IPV6_BINDV6ONLY
#endif
if (addrinfo->ai_family == PF_INET6)
{
if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY,
(char *)&on, sizeof (int)) == -1)
{
if (errbuf)
pcap_snprintf(errbuf, errbuflen, "setsockopt(IPV6_V6ONLY)");
closesocket(sock);
return INVALID_SOCKET;
}
}
#endif


if (bind(sock, addrinfo->ai_addr, (int) addrinfo->ai_addrlen) != 0)
{
sock_geterror("bind()", errbuf, errbuflen);
closesocket(sock);
return INVALID_SOCKET;
}

if (addrinfo->ai_socktype == SOCK_STREAM)
if (listen(sock, nconn) == -1)
{
sock_geterror("listen()", errbuf, errbuflen);
closesocket(sock);
return INVALID_SOCKET;
}


return sock;
}
else
{
struct addrinfo *tempaddrinfo;
char *errbufptr;
size_t bufspaceleft;

tempaddrinfo = addrinfo;
errbufptr = errbuf;
bufspaceleft = errbuflen;
*errbufptr = 0;






while (tempaddrinfo)
{

if (connect(sock, tempaddrinfo->ai_addr, (int) tempaddrinfo->ai_addrlen) == -1)
{
size_t msglen;
char TmpBuffer[100];
char SocketErrorMessage[SOCK_ERRBUF_SIZE];





sock_geterror("Connect to socket failed",
SocketErrorMessage, sizeof(SocketErrorMessage));


sock_getascii_addrport((struct sockaddr_storage *) tempaddrinfo->ai_addr, TmpBuffer, sizeof(TmpBuffer), NULL, 0, NI_NUMERICHOST, TmpBuffer, sizeof(TmpBuffer));

pcap_snprintf(errbufptr, bufspaceleft,
"Is the server properly installed on %s? %s", TmpBuffer, SocketErrorMessage);


msglen = strlen(errbufptr);

errbufptr[msglen] = ' ';
errbufptr[msglen + 1] = 0;

bufspaceleft = bufspaceleft - (msglen + 1);
errbufptr += (msglen + 1);

tempaddrinfo = tempaddrinfo->ai_next;
}
else
break;
}





if (tempaddrinfo == NULL)
{
closesocket(sock);
return INVALID_SOCKET;
}
else
return sock;
}
}



















int sock_close(SOCKET sock, char *errbuf, int errbuflen)
{





if (shutdown(sock, SHUT_WR))
{
sock_geterror("shutdown()", errbuf, errbuflen);

closesocket(sock);
return -1;
}

closesocket(sock);
return 0;
}
















static void
get_gai_errstring(char *errbuf, int errbuflen, const char *prefix, int err,
const char *hostname, const char *portname)
{
char hostport[PCAP_ERRBUF_SIZE];

if (hostname != NULL && portname != NULL)
pcap_snprintf(hostport, PCAP_ERRBUF_SIZE, "%s:%s",
hostname, portname);
else if (hostname != NULL)
pcap_snprintf(hostport, PCAP_ERRBUF_SIZE, "%s",
hostname);
else if (portname != NULL)
pcap_snprintf(hostport, PCAP_ERRBUF_SIZE, ":%s",
portname);
else
pcap_snprintf(hostport, PCAP_ERRBUF_SIZE, "<no host or port!>");
switch (err)
{
#if defined(EAI_ADDRFAMILY)
case EAI_ADDRFAMILY:
pcap_snprintf(errbuf, errbuflen,
"%sAddress family for %s not supported",
prefix, hostport);
break;
#endif

case EAI_AGAIN:
pcap_snprintf(errbuf, errbuflen,
"%s%s could not be resolved at this time",
prefix, hostport);
break;

case EAI_BADFLAGS:
pcap_snprintf(errbuf, errbuflen,
"%sThe ai_flags parameter for looking up %s had an invalid value",
prefix, hostport);
break;

case EAI_FAIL:
pcap_snprintf(errbuf, errbuflen,
"%sA non-recoverable error occurred when attempting to resolve %s",
prefix, hostport);
break;

case EAI_FAMILY:
pcap_snprintf(errbuf, errbuflen,
"%sThe address family for looking up %s was not recognized",
prefix, hostport);
break;

case EAI_MEMORY:
pcap_snprintf(errbuf, errbuflen,
"%sOut of memory trying to allocate storage when looking up %s",
prefix, hostport);
break;











#if defined(EAI_NODATA) && EAI_NODATA != EAI_NONAME
case EAI_NODATA:
pcap_snprintf(errbuf, errbuflen,
"%sNo address associated with %s",
prefix, hostport);
break;
#endif

case EAI_NONAME:
pcap_snprintf(errbuf, errbuflen,
"%sThe host name %s couldn't be resolved",
prefix, hostport);
break;

case EAI_SERVICE:
pcap_snprintf(errbuf, errbuflen,
"%sThe service value specified when looking up %s as not recognized for the socket type",
prefix, hostport);
break;

case EAI_SOCKTYPE:
pcap_snprintf(errbuf, errbuflen,
"%sThe socket type specified when looking up %s as not recognized",
prefix, hostport);
break;

#if defined(EAI_SYSTEM)
case EAI_SYSTEM:



pcap_snprintf(errbuf, errbuflen,
"%sAn error occurred when looking up %s: %s",
prefix, hostport, pcap_strerror(errno));
break;
#endif

#if defined(EAI_BADHINTS)
case EAI_BADHINTS:
pcap_snprintf(errbuf, errbuflen,
"%sInvalid value for hints when looking up %s",
prefix, hostport);
break;
#endif

#if defined(EAI_PROTOCOL)
case EAI_PROTOCOL:
pcap_snprintf(errbuf, errbuflen,
"%sResolved protocol when looking up %s is unknown",
prefix, hostport);
break;
#endif

#if defined(EAI_OVERFLOW)
case EAI_OVERFLOW:
pcap_snprintf(errbuf, errbuflen,
"%sArgument buffer overflow when looking up %s",
prefix, hostport);
break;
#endif

default:
pcap_snprintf(errbuf, errbuflen,
"%sgetaddrinfo() error %d when looking up %s",
prefix, err, hostport);
break;
}
}







































int sock_initaddress(const char *host, const char *port,
struct addrinfo *hints, struct addrinfo **addrinfo, char *errbuf, int errbuflen)
{
int retval;

retval = getaddrinfo(host, port, hints, addrinfo);
if (retval != 0)
{
if (errbuf)
{
get_gai_errstring(errbuf, errbuflen, "", retval,
host, port);
}
return -1;
}













if (((*addrinfo)->ai_family != PF_INET) &&
((*addrinfo)->ai_family != PF_INET6))
{
if (errbuf)
pcap_snprintf(errbuf, errbuflen, "getaddrinfo(): socket type not supported");
freeaddrinfo(*addrinfo);
*addrinfo = NULL;
return -1;
}




if (((*addrinfo)->ai_socktype == SOCK_STREAM) &&
(sock_ismcastaddr((*addrinfo)->ai_addr) == 0))
{
if (errbuf)
pcap_snprintf(errbuf, errbuflen, "getaddrinfo(): multicast addresses are not valid when using TCP streams");
freeaddrinfo(*addrinfo);
*addrinfo = NULL;
return -1;
}

return 0;
}




























int sock_send(SOCKET sock, const char *buffer, size_t size,
char *errbuf, int errbuflen)
{
int remaining;
ssize_t nsent;

if (size > INT_MAX)
{
if (errbuf)
{
pcap_snprintf(errbuf, errbuflen,
"Can't send more than %u bytes with sock_send",
INT_MAX);
}
return -1;
}
remaining = (int)size;

do {
#if defined(MSG_NOSIGNAL)






nsent = send(sock, buffer, remaining, MSG_NOSIGNAL);
#else
nsent = send(sock, buffer, remaining, 0);
#endif

if (nsent == -1)
{





int errcode;

#if defined(_WIN32)
errcode = GetLastError();
if (errcode == WSAECONNRESET ||
errcode == WSAECONNABORTED)
{






return -2;
}
sock_fmterror("send()", errcode, errbuf, errbuflen);
#else
errcode = errno;
if (errcode == ECONNRESET || errcode == EPIPE)
{





return -2;
}
sock_fmterror("send()", errcode, errbuf, errbuflen);
#endif
return -1;
}

remaining -= nsent;
buffer += nsent;
} while (remaining != 0);

return 0;
}
























































int sock_bufferize(const char *buffer, int size, char *tempbuf, int *offset, int totsize, int checkonly, char *errbuf, int errbuflen)
{
if ((*offset + size) > totsize)
{
if (errbuf)
pcap_snprintf(errbuf, errbuflen, "Not enough space in the temporary send buffer.");
return -1;
}

if (!checkonly)
memcpy(tempbuf + (*offset), buffer, size);

(*offset) += size;

return 0;
}















































int sock_recv(SOCKET sock, void *buffer, size_t size, int flags,
char *errbuf, int errbuflen)
{
char *bufp = buffer;
int remaining;
ssize_t nread;

if (size == 0)
{
return 0;
}
if (size > INT_MAX)
{
if (errbuf)
{
pcap_snprintf(errbuf, errbuflen,
"Can't read more than %u bytes with sock_recv",
INT_MAX);
}
return -1;
}

bufp = (char *) buffer;
remaining = (int) size;





for (;;) {
nread = recv(sock, bufp, remaining, 0);

if (nread == -1)
{
#if !defined(_WIN32)
if (errno == EINTR)
return -3;
#endif
sock_geterror("recv()", errbuf, errbuflen);
return -1;
}

if (nread == 0)
{
if ((flags & SOCK_EOF_IS_ERROR) ||
(remaining != (int) size))
{





if (errbuf)
{
pcap_snprintf(errbuf, errbuflen,
"The other host terminated the connection.");
}
return -1;
}
else
return 0;
}





if (!(flags & SOCK_RECEIVEALL_YES))
{



return (int) nread;
}

bufp += nread;
remaining -= nread;

if (remaining == 0)
return (int) size;
}
}






int sock_recv_dgram(SOCKET sock, void *buffer, size_t size,
char *errbuf, int errbuflen)
{
ssize_t nread;
#if !defined(_WIN32)
struct msghdr message;
struct iovec iov;
#endif

if (size == 0)
{
return 0;
}
if (size > INT_MAX)
{
if (errbuf)
{
pcap_snprintf(errbuf, errbuflen,
"Can't read more than %u bytes with sock_recv_dgram",
INT_MAX);
}
return -1;
}






#if defined(_WIN32)
nread = recv(sock, buffer, size, 0);
if (nread == SOCKET_ERROR)
{













sock_geterror("recv()", errbuf, errbuflen);
return -1;
}
#else











message.msg_name = NULL;
message.msg_namelen = 0;
iov.iov_base = buffer;
iov.iov_len = size;
message.msg_iov = &iov;
message.msg_iovlen = 1;
#if defined(HAVE_STRUCT_MSGHDR_MSG_CONTROL)
message.msg_control = NULL;
message.msg_controllen = 0;
#endif
#if defined(HAVE_STRUCT_MSGHDR_MSG_FLAGS)
message.msg_flags = 0;
#endif
nread = recvmsg(sock, &message, 0);
if (nread == -1)
{
if (errno == EINTR)
return -3;
sock_geterror("recv()", errbuf, errbuflen);
return -1;
}
#if defined(HAVE_STRUCT_MSGHDR_MSG_FLAGS)





if (message.msg_flags & MSG_TRUNC)
{






pcap_snprintf(errbuf, errbuflen, "recv(): Message too long");
return -1;
}
#endif
#endif





return (int)nread;
}


























int sock_discard(SOCKET sock, int size, char *errbuf, int errbuflen)
{
#define TEMP_BUF_SIZE 32768

char buffer[TEMP_BUF_SIZE];









while (size > TEMP_BUF_SIZE)
{
if (sock_recv(sock, buffer, TEMP_BUF_SIZE, SOCK_RECEIVEALL_YES, errbuf, errbuflen) == -1)
return -1;

size -= TEMP_BUF_SIZE;
}





if (size)
{
if (sock_recv(sock, buffer, size, SOCK_RECEIVEALL_YES, errbuf, errbuflen) == -1)
return -1;
}

return 0;
}





























int sock_check_hostlist(char *hostlist, const char *sep, struct sockaddr_storage *from, char *errbuf, int errbuflen)
{

if ((hostlist) && (hostlist[0]))
{
char *token;
struct addrinfo *addrinfo, *ai_next;
char *temphostlist;
char *lasts;
int getaddrinfo_failed = 0;





temphostlist = strdup(hostlist);
if (temphostlist == NULL)
{
sock_geterror("sock_check_hostlist(), malloc() failed", errbuf, errbuflen);
return -2;
}

token = pcap_strtok_r(temphostlist, sep, &lasts);


addrinfo = NULL;

while (token != NULL)
{
struct addrinfo hints;
int retval;

addrinfo = NULL;
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

retval = getaddrinfo(token, NULL, &hints, &addrinfo);
if (retval != 0)
{
if (errbuf)
get_gai_errstring(errbuf, errbuflen,
"Allowed host list error: ",
retval, token, NULL);





getaddrinfo_failed = 1;


token = pcap_strtok_r(NULL, sep, &lasts);
continue;
}


ai_next = addrinfo;
while (ai_next)
{
if (sock_cmpaddr(from, (struct sockaddr_storage *) ai_next->ai_addr) == 0)
{
free(temphostlist);
freeaddrinfo(addrinfo);
return 0;
}





ai_next = ai_next->ai_next;
}

freeaddrinfo(addrinfo);
addrinfo = NULL;


token = pcap_strtok_r(NULL, sep, &lasts);
}

if (addrinfo)
{
freeaddrinfo(addrinfo);
addrinfo = NULL;
}

free(temphostlist);

if (getaddrinfo_failed) {






return -2;
} else {




if (errbuf)
pcap_snprintf(errbuf, errbuflen, "The host is not in the allowed host list. Connection refused.");
return -1;
}
}


return 1;
}



















int sock_cmpaddr(struct sockaddr_storage *first, struct sockaddr_storage *second)
{
if (first->ss_family == second->ss_family)
{
if (first->ss_family == AF_INET)
{
if (memcmp(&(((struct sockaddr_in *) first)->sin_addr),
&(((struct sockaddr_in *) second)->sin_addr),
sizeof(struct in_addr)) == 0)
return 0;
}
else
{
if (memcmp(&(((struct sockaddr_in6 *) first)->sin6_addr),
&(((struct sockaddr_in6 *) second)->sin6_addr),
sizeof(struct in6_addr)) == 0)
return 0;
}
}

return -1;
}










































int sock_getmyinfo(SOCKET sock, char *address, int addrlen, char *port, int portlen, int flags, char *errbuf, int errbuflen)
{
struct sockaddr_storage mysockaddr;
socklen_t sockaddrlen;


sockaddrlen = sizeof(struct sockaddr_storage);

if (getsockname(sock, (struct sockaddr *) &mysockaddr, &sockaddrlen) == -1)
{
sock_geterror("getsockname()", errbuf, errbuflen);
return 0;
}


return sock_getascii_addrport(&mysockaddr, address, addrlen, port, portlen, flags, errbuf, errbuflen);
}

















































int sock_getascii_addrport(const struct sockaddr_storage *sockaddr, char *address, int addrlen, char *port, int portlen, int flags, char *errbuf, int errbuflen)
{
socklen_t sockaddrlen;
int retval;

retval = -1;

#if defined(_WIN32)
if (sockaddr->ss_family == AF_INET)
sockaddrlen = sizeof(struct sockaddr_in);
else
sockaddrlen = sizeof(struct sockaddr_in6);
#else
sockaddrlen = sizeof(struct sockaddr_storage);
#endif

if ((flags & NI_NUMERICHOST) == 0)
{
if ((sockaddr->ss_family == AF_INET6) &&
(memcmp(&((struct sockaddr_in6 *) sockaddr)->sin6_addr, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", sizeof(struct in6_addr)) == 0))
{
if (address)
pcap_strlcpy(address, SOCKET_NAME_NULL_DAD, addrlen);
return retval;
}
}

if (getnameinfo((struct sockaddr *) sockaddr, sockaddrlen, address, addrlen, port, portlen, flags) != 0)
{

if (errbuf)
{
sock_geterror("getnameinfo()", errbuf, errbuflen);
errbuf[errbuflen - 1] = 0;
}

if (address)
{
pcap_strlcpy(address, SOCKET_NO_NAME_AVAILABLE, addrlen);
address[addrlen - 1] = 0;
}

if (port)
{
pcap_strlcpy(port, SOCKET_NO_PORT_AVAILABLE, portlen);
port[portlen - 1] = 0;
}

retval = 0;
}

return retval;
}





































int sock_present2network(const char *address, struct sockaddr_storage *sockaddr, int addr_family, char *errbuf, int errbuflen)
{
int retval;
struct addrinfo *addrinfo;
struct addrinfo hints;

memset(&hints, 0, sizeof(hints));

hints.ai_family = addr_family;

if ((retval = sock_initaddress(address, "22222" , &hints, &addrinfo, errbuf, errbuflen)) == -1)
return 0;

if (addrinfo->ai_family == PF_INET)
memcpy(sockaddr, addrinfo->ai_addr, sizeof(struct sockaddr_in));
else
memcpy(sockaddr, addrinfo->ai_addr, sizeof(struct sockaddr_in6));

if (addrinfo->ai_next != NULL)
{
freeaddrinfo(addrinfo);

if (errbuf)
pcap_snprintf(errbuf, errbuflen, "More than one socket requested; using the first one returned");
return -2;
}

freeaddrinfo(addrinfo);
return -1;
}
