

























#include "../util-internal.h"
#include "event2/event-config.h"

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#endif
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
#include <errno.h>

#include <event.h>
#include <evutil.h>

int test_okay = 1;
int called = 0;
struct timeval timeout = {60, 0};

static void
read_cb(evutil_socket_t fd, short event, void *arg)
{
char buf[256];
int len;

if (EV_TIMEOUT & event) {
printf("%s: Timeout!\n", __func__);
exit(1);
}

len = recv(fd, buf, sizeof(buf), 0);

printf("%s: read %d%s\n", __func__,
len, len ? "" : " - means EOF");

if (len) {
if (!called)
event_add(arg, &timeout);
} else if (called == 1)
test_okay = 0;

called++;
}

int
main(int argc, char **argv)
{
struct event ev;
const char *test = "test string";
evutil_socket_t pair[2];

#if defined(_WIN32)
WORD wVersionRequested;
WSADATA wsaData;

wVersionRequested = MAKEWORD(2, 2);

(void) WSAStartup(wVersionRequested, &wsaData);
#endif

if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
return (1);


if (send(pair[0], test, (int)strlen(test)+1, 0) < 0)
return (1);
shutdown(pair[0], EVUTIL_SHUT_WR);


event_init();


event_set(&ev, pair[1], EV_READ | EV_TIMEOUT, read_cb, &ev);

event_add(&ev, &timeout);

event_dispatch();

return (test_okay);
}

