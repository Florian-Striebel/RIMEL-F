

























#include "event2/event-config.h"
#include "util-internal.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(_WIN32)
#include <unistd.h>
#include <sys/time.h>
#endif
#include <errno.h>

#include "event2/event.h"
#include "event2/event_compat.h"
#include "event2/event_struct.h"

int called = 0;

#define NEVENT 20000

struct event *ev[NEVENT];

struct evutil_weakrand_state weakrand_state;

static int
rand_int(int n)
{
return evutil_weakrand_(&weakrand_state) % n;
}

static void
time_cb(evutil_socket_t fd, short event, void *arg)
{
struct timeval tv;
int i, j;

called++;

if (called < 10*NEVENT) {
for (i = 0; i < 10; i++) {
j = rand_int(NEVENT);
tv.tv_sec = 0;
tv.tv_usec = rand_int(50000);
if (tv.tv_usec % 2 || called < NEVENT)
evtimer_add(ev[j], &tv);
else
evtimer_del(ev[j]);
}
}
}

int
main(int argc, char **argv)
{
struct timeval tv;
int i;
#if defined(_WIN32)
WORD wVersionRequested;
WSADATA wsaData;

wVersionRequested = MAKEWORD(2, 2);

(void) WSAStartup(wVersionRequested, &wsaData);
#endif

evutil_weakrand_seed_(&weakrand_state, 0);


event_init();

for (i = 0; i < NEVENT; i++) {
ev[i] = malloc(sizeof(struct event));


evtimer_set(ev[i], time_cb, ev[i]);
tv.tv_sec = 0;
tv.tv_usec = rand_int(50000);
evtimer_add(ev[i], &tv);
}

event_dispatch();


printf("%d, %d\n", called, NEVENT);
return (called < NEVENT);
}

