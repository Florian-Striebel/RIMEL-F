























#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "bearssl.h"



























#if !(SERVER_RSA || SERVER_EC || SERVER_MIXED)
#define SERVER_RSA 1
#define SERVER_EC 0
#define SERVER_MIXED 0
#endif

#if SERVER_RSA
#include "chain-rsa.h"
#include "key-rsa.h"
#define SKEY RSA
#elif SERVER_EC
#include "chain-ec.h"
#include "key-ec.h"
#define SKEY EC
#elif SERVER_MIXED
#include "chain-ec+rsa.h"
#include "key-ec.h"
#define SKEY EC
#else
#error Must use one of RSA, EC or MIXED chains.
#endif







static int
host_bind(const char *host, const char *port)
{
struct addrinfo hints, *si, *p;
int fd;
int err;

memset(&hints, 0, sizeof hints);
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
err = getaddrinfo(host, port, &hints, &si);
if (err != 0) {
fprintf(stderr, "ERROR: getaddrinfo(): %s\n",
gai_strerror(err));
return -1;
}
fd = -1;
for (p = si; p != NULL; p = p->ai_next) {
struct sockaddr *sa;
struct sockaddr_in sa4;
struct sockaddr_in6 sa6;
size_t sa_len;
void *addr;
char tmp[INET6_ADDRSTRLEN + 50];
int opt;

sa = (struct sockaddr *)p->ai_addr;
if (sa->sa_family == AF_INET) {
sa4 = *(struct sockaddr_in *)sa;
sa = (struct sockaddr *)&sa4;
sa_len = sizeof sa4;
addr = &sa4.sin_addr;
if (host == NULL) {
sa4.sin_addr.s_addr = INADDR_ANY;
}
} else if (sa->sa_family == AF_INET6) {
sa6 = *(struct sockaddr_in6 *)sa;
sa = (struct sockaddr *)&sa6;
sa_len = sizeof sa6;
addr = &sa6.sin6_addr;
if (host == NULL) {
sa6.sin6_addr = in6addr_any;
}
} else {
addr = NULL;
sa_len = p->ai_addrlen;
}
if (addr != NULL) {
inet_ntop(p->ai_family, addr, tmp, sizeof tmp);
} else {
sprintf(tmp, "<unknown family: %d>",
(int)sa->sa_family);
}
fprintf(stderr, "binding to: %s\n", tmp);
fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
if (fd < 0) {
perror("socket()");
continue;
}
opt = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
opt = 0;
setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof opt);
if (bind(fd, sa, sa_len) < 0) {
perror("bind()");
close(fd);
continue;
}
break;
}
if (p == NULL) {
freeaddrinfo(si);
fprintf(stderr, "ERROR: failed to bind\n");
return -1;
}
freeaddrinfo(si);
if (listen(fd, 5) < 0) {
perror("listen()");
close(fd);
return -1;
}
fprintf(stderr, "bound.\n");
return fd;
}





static int
accept_client(int server_fd)
{
int fd;
struct sockaddr sa;
socklen_t sa_len;
char tmp[INET6_ADDRSTRLEN + 50];
const char *name;

sa_len = sizeof sa;
fd = accept(server_fd, &sa, &sa_len);
if (fd < 0) {
perror("accept()");
return -1;
}
name = NULL;
switch (sa.sa_family) {
case AF_INET:
name = inet_ntop(AF_INET,
&((struct sockaddr_in *)&sa)->sin_addr,
tmp, sizeof tmp);
break;
case AF_INET6:
name = inet_ntop(AF_INET6,
&((struct sockaddr_in6 *)&sa)->sin6_addr,
tmp, sizeof tmp);
break;
}
if (name == NULL) {
sprintf(tmp, "<unknown: %lu>", (unsigned long)sa.sa_family);
name = tmp;
}
fprintf(stderr, "accepting connection from: %s\n", name);
return fd;
}




static int
sock_read(void *ctx, unsigned char *buf, size_t len)
{
for (;;) {
ssize_t rlen;

rlen = read(*(int *)ctx, buf, len);
if (rlen <= 0) {
if (rlen < 0 && errno == EINTR) {
continue;
}
return -1;
}
return (int)rlen;
}
}




static int
sock_write(void *ctx, const unsigned char *buf, size_t len)
{
for (;;) {
ssize_t wlen;

wlen = write(*(int *)ctx, buf, len);
if (wlen <= 0) {
if (wlen < 0 && errno == EINTR) {
continue;
}
return -1;
}
return (int)wlen;
}
}




static const char *HTTP_RES =
"HTTP/1.0 200 OK\r\n"
"Content-Length: 46\r\n"
"Connection: close\r\n"
"Content-Type: text/html; charset=iso-8859-1\r\n"
"\r\n"
"<html>\r\n"
"<body>\r\n"
"<p>Test!</p>\r\n"
"</body>\r\n"
"</html>\r\n";













int
main(int argc, char *argv[])
{
const char *port;
int fd;

if (argc != 2) {
return EXIT_FAILURE;
}
port = argv[1];




signal(SIGPIPE, SIG_IGN);




fd = host_bind(NULL, port);
if (fd < 0) {
return EXIT_FAILURE;
}




for (;;) {
int cfd;
br_ssl_server_context sc;
unsigned char iobuf[BR_SSL_BUFSIZE_BIDI];
br_sslio_context ioc;
int lcwn, err;

cfd = accept_client(fd);
if (cfd < 0) {
return EXIT_FAILURE;
}














#if SERVER_RSA
#if SERVER_PROFILE_MIN_FS
#if SERVER_CHACHA20
br_ssl_server_init_mine2c(&sc, CHAIN, CHAIN_LEN, &SKEY);
#else
br_ssl_server_init_mine2g(&sc, CHAIN, CHAIN_LEN, &SKEY);
#endif
#elif SERVER_PROFILE_MIN_NOFS
br_ssl_server_init_minr2g(&sc, CHAIN, CHAIN_LEN, &SKEY);
#else
br_ssl_server_init_full_rsa(&sc, CHAIN, CHAIN_LEN, &SKEY);
#endif
#elif SERVER_EC
#if SERVER_PROFILE_MIN_FS
#if SERVER_CHACHA20
br_ssl_server_init_minf2c(&sc, CHAIN, CHAIN_LEN, &SKEY);
#else
br_ssl_server_init_minf2g(&sc, CHAIN, CHAIN_LEN, &SKEY);
#endif
#elif SERVER_PROFILE_MIN_NOFS
br_ssl_server_init_minv2g(&sc, CHAIN, CHAIN_LEN, &SKEY);
#else
br_ssl_server_init_full_ec(&sc, CHAIN, CHAIN_LEN,
BR_KEYTYPE_EC, &SKEY);
#endif
#else
#if SERVER_PROFILE_MIN_FS
#if SERVER_CHACHA20
br_ssl_server_init_minf2c(&sc, CHAIN, CHAIN_LEN, &SKEY);
#else
br_ssl_server_init_minf2g(&sc, CHAIN, CHAIN_LEN, &SKEY);
#endif
#elif SERVER_PROFILE_MIN_NOFS
br_ssl_server_init_minu2g(&sc, CHAIN, CHAIN_LEN, &SKEY);
#else
br_ssl_server_init_full_ec(&sc, CHAIN, CHAIN_LEN,
BR_KEYTYPE_RSA, &SKEY);
#endif
#endif








br_ssl_engine_set_buffer(&sc.eng, iobuf, sizeof iobuf, 1);




br_ssl_server_reset(&sc);




br_sslio_init(&ioc, &sc.eng, sock_read, &cfd, sock_write, &cfd);




lcwn = 0;
for (;;) {
unsigned char x;

if (br_sslio_read(&ioc, &x, 1) < 0) {
goto client_drop;
}
if (x == 0x0D) {
continue;
}
if (x == 0x0A) {
if (lcwn) {
break;
}
lcwn = 1;
} else {
lcwn = 0;
}
}




br_sslio_write_all(&ioc, HTTP_RES, strlen(HTTP_RES));
br_sslio_close(&ioc);

client_drop:
err = br_ssl_engine_last_error(&sc.eng);
if (err == 0) {
fprintf(stderr, "SSL closed (correctly).\n");
} else {
fprintf(stderr, "SSL error: %d\n", err);
}
close(cfd);
}
}
