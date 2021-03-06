































#if !defined(__SOCKUTILS_H__)
#define __SOCKUTILS_H__

#if defined(_MSC_VER)
#pragma once
#endif

#include "pcap/socket.h"

#if !defined(_WIN32)

#include <unistd.h>







#define closesocket(a) close(a)
#endif





#if ((defined(__MINGW32__)) && (_WIN32_WINNT < 0x0501))
int WSAAPI getnameinfo(const struct sockaddr*,socklen_t,char*,DWORD,
char*,DWORD,int);
#endif


























#define SOCKBUF_CHECKONLY 1

#define SOCKBUF_BUFFERIZE 0


#define SOCKOPEN_CLIENT 0

#define SOCKOPEN_SERVER 1




#define SOCK_RECEIVEALL_NO 0x00000000
#define SOCK_RECEIVEALL_YES 0x00000001

#define SOCK_EOF_ISNT_ERROR 0x00000000
#define SOCK_EOF_IS_ERROR 0x00000002





#if defined(__cplusplus)
extern "C" {
#endif










int sock_init(char *errbuf, int errbuflen);
void sock_cleanup(void);
void sock_fmterror(const char *caller, int errcode, char *errbuf, int errbuflen);
void sock_geterror(const char *caller, char *errbuf, int errbufsize);
int sock_initaddress(const char *address, const char *port,
struct addrinfo *hints, struct addrinfo **addrinfo,
char *errbuf, int errbuflen);
int sock_recv(SOCKET sock, void *buffer, size_t size, int receiveall,
char *errbuf, int errbuflen);
int sock_recv_dgram(SOCKET sock, void *buffer, size_t size,
char *errbuf, int errbuflen);
SOCKET sock_open(struct addrinfo *addrinfo, int server, int nconn, char *errbuf, int errbuflen);
int sock_close(SOCKET sock, char *errbuf, int errbuflen);

int sock_send(SOCKET sock, const char *buffer, size_t size,
char *errbuf, int errbuflen);
int sock_bufferize(const char *buffer, int size, char *tempbuf, int *offset, int totsize, int checkonly, char *errbuf, int errbuflen);
int sock_discard(SOCKET sock, int size, char *errbuf, int errbuflen);
int sock_check_hostlist(char *hostlist, const char *sep, struct sockaddr_storage *from, char *errbuf, int errbuflen);
int sock_cmpaddr(struct sockaddr_storage *first, struct sockaddr_storage *second);

int sock_getmyinfo(SOCKET sock, char *address, int addrlen, char *port, int portlen, int flags, char *errbuf, int errbuflen);

int sock_getascii_addrport(const struct sockaddr_storage *sockaddr, char *address, int addrlen, char *port, int portlen, int flags, char *errbuf, int errbuflen);
int sock_present2network(const char *address, struct sockaddr_storage *sockaddr, int addr_family, char *errbuf, int errbuflen);

#if defined(__cplusplus)
}
#endif









#endif
