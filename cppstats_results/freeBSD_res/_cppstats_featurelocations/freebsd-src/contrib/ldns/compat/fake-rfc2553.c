





































#include <ldns/config.h>
#include <ldns/common.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "compat/fake-rfc2553.h"

#if !defined(HAVE_GETNAMEINFO)
int getnameinfo(const struct sockaddr *sa, size_t ATTR_UNUSED(salen), char *host,
size_t hostlen, char *serv, size_t servlen, int flags)
{
struct sockaddr_in *sin = (struct sockaddr_in *)sa;
struct hostent *hp;
char tmpserv[16];

if (serv != NULL) {
snprintf(tmpserv, sizeof(tmpserv), "%d", ntohs(sin->sin_port));
if (strlcpy(serv, tmpserv, servlen) >= servlen)
return (EAI_MEMORY);
}

if (host != NULL) {
if (flags & NI_NUMERICHOST) {
if (strlcpy(host, inet_ntoa(sin->sin_addr),
hostlen) >= hostlen)
return (EAI_MEMORY);
else
return (0);
} else {
hp = gethostbyaddr((char *)&sin->sin_addr,
sizeof(struct in_addr), AF_INET);
if (hp == NULL)
return (EAI_NODATA);

if (strlcpy(host, hp->h_name, hostlen) >= hostlen)
return (EAI_MEMORY);
else
return (0);
}
}
return (0);
}
#endif

#if !defined(HAVE_GAI_STRERROR)
#if defined(HAVE_CONST_GAI_STRERROR_PROTO)
const char *
#else
char *
#endif
gai_strerror(int err)
{
switch (err) {
case EAI_NODATA:
return ("no address associated with name");
case EAI_MEMORY:
return ("memory allocation failure.");
case EAI_NONAME:
return ("nodename nor servname provided, or not known");
default:
return ("unknown/invalid error.");
}
}
#endif

#if !defined(HAVE_FREEADDRINFO)
void
freeaddrinfo(struct addrinfo *ai)
{
struct addrinfo *next;

for(; ai != NULL;) {
next = ai->ai_next;
free(ai);
ai = next;
}
}
#endif

#if !defined(HAVE_GETADDRINFO)
static struct
addrinfo *malloc_ai(int port, u_long addr, const struct addrinfo *hints)
{
struct addrinfo *ai;

ai = malloc(sizeof(*ai) + sizeof(struct sockaddr_in));
if (ai == NULL)
return (NULL);

memset(ai, '\0', sizeof(*ai) + sizeof(struct sockaddr_in));

ai->ai_addr = (struct sockaddr *)(ai + 1);

ai->ai_addrlen = sizeof(struct sockaddr_in);
ai->ai_addr->sa_family = ai->ai_family = AF_INET;

((struct sockaddr_in *)(ai)->ai_addr)->sin_port = port;
((struct sockaddr_in *)(ai)->ai_addr)->sin_addr.s_addr = addr;


if (hints->ai_socktype)
ai->ai_socktype = hints->ai_socktype;
else
ai->ai_socktype = SOCK_STREAM;

if (hints->ai_protocol)
ai->ai_protocol = hints->ai_protocol;

return (ai);
}

int
getaddrinfo(const char *hostname, const char *servname,
const struct addrinfo *hints, struct addrinfo **res)
{
struct hostent *hp;
struct servent *sp;
struct in_addr in;
int i;
long int port;
u_long addr;

port = 0;
if (servname != NULL) {
char *cp;

port = strtol(servname, &cp, 10);
if (port > 0 && port <= 65535 && *cp == '\0')
port = htons(port);
else if ((sp = getservbyname(servname, NULL)) != NULL)
port = sp->s_port;
else
port = 0;
}

if (hints && hints->ai_flags & AI_PASSIVE) {
addr = htonl(0x00000000);
if (hostname && inet_aton(hostname, &in) != 0)
addr = in.s_addr;
*res = malloc_ai(port, addr, hints);
if (*res == NULL)
return (EAI_MEMORY);
return (0);
}

if (!hostname) {
*res = malloc_ai(port, htonl(0x7f000001), hints);
if (*res == NULL)
return (EAI_MEMORY);
return (0);
}

if (inet_aton(hostname, &in)) {
*res = malloc_ai(port, in.s_addr, hints);
if (*res == NULL)
return (EAI_MEMORY);
return (0);
}


if (hints && hints->ai_flags & AI_NUMERICHOST)
return (EAI_NONAME);

hp = gethostbyname(hostname);
if (hp && hp->h_name && hp->h_name[0] && hp->h_addr_list[0]) {
struct addrinfo *cur, *prev;

cur = prev = *res = NULL;
for (i = 0; hp->h_addr_list[i]; i++) {
struct in_addr *in = (struct in_addr *)hp->h_addr_list[i];

cur = malloc_ai(port, in->s_addr, hints);
if (cur == NULL) {
if (*res != NULL)
freeaddrinfo(*res);
return (EAI_MEMORY);
}
if (prev)
prev->ai_next = cur;
else
*res = cur;

prev = cur;
}
return (0);
}

return (EAI_NODATA);
}
#endif
