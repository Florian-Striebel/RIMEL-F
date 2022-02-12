

























#include "event2/event-config.h"

#include <sys/types.h>
#include <sys/stat.h>
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(EVENT__HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(_WIN32)
#include <unistd.h>
#endif
#include <errno.h>

#include <event.h>

int
main(int argc, char **argv)
{
#if defined(_WIN32)
WORD wVersionRequested;
WSADATA wsaData;

wVersionRequested = MAKEWORD(2, 2);

(void) WSAStartup(wVersionRequested, &wsaData);
#endif


event_init();

return (0);
}

