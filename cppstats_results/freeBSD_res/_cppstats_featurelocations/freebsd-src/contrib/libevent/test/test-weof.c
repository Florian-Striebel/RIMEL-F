

























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
#include <signal.h>
#include <errno.h>

#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/event_compat.h"
#include "event2/util.h"

evutil_socket_t pair[2];
int test_okay = 1;
int called = 0;

static void
write_cb(evutil_socket_t fd, short event, void *arg)
{
const char *test = "test string";
int len;

len = send(fd, test, (int)strlen(test) + 1, 0);

printf("%s: write %d%s\n", __func__,
len, len ? "" : " - means EOF");

if (len > 0) {
if (!called)
event_add(arg, NULL);
evutil_closesocket(pair[0]);
} else if (called == 1)
test_okay = 0;

called++;
}

int
main(int argc, char **argv)
{
struct event ev;

#if defined(_WIN32)
WORD wVersionRequested;
WSADATA wsaData;

wVersionRequested = MAKEWORD(2, 2);

(void) WSAStartup(wVersionRequested, &wsaData);
#endif

#if !defined(_WIN32)
if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
return (1);
#endif

if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
return (1);


event_init();


event_set(&ev, pair[1], EV_WRITE, write_cb, &ev);

event_add(&ev, NULL);

event_dispatch();

return (test_okay);
}

