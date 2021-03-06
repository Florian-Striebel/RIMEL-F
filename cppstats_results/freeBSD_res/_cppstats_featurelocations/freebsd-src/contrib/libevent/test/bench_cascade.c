


























#include "event2/event-config.h"

#include <sys/types.h>
#include <sys/stat.h>
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/socket.h>
#include <sys/resource.h>
#endif
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined(EVENT__HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <errno.h>
#include <getopt.h>
#include <event.h>
#include <evutil.h>








static int fired;
static evutil_socket_t *pipes;
static struct event *events;

static void
read_cb(evutil_socket_t fd, short which, void *arg)
{
char ch;
evutil_socket_t sock = (evutil_socket_t)(ev_intptr_t)arg;

(void) recv(fd, &ch, sizeof(ch), 0);
if (sock >= 0) {
if (send(sock, "e", 1, 0) < 0)
perror("send");
}
fired++;
}

static struct timeval *
run_once(int num_pipes)
{
int i;
evutil_socket_t *cp;
static struct timeval ts, te, tv_timeout;

events = (struct event *)calloc(num_pipes, sizeof(struct event));
pipes = (evutil_socket_t *)calloc(num_pipes * 2, sizeof(evutil_socket_t));

if (events == NULL || pipes == NULL) {
perror("malloc");
exit(1);
}

for (cp = pipes, i = 0; i < num_pipes; i++, cp += 2) {
if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, cp) == -1) {
perror("socketpair");
exit(1);
}
}


evutil_gettimeofday(&ts, NULL);


evutil_timerclear(&tv_timeout);
tv_timeout.tv_sec = 60;

for (cp = pipes, i = 0; i < num_pipes; i++, cp += 2) {
evutil_socket_t fd = i < num_pipes - 1 ? cp[3] : -1;
event_set(&events[i], cp[0], EV_READ, read_cb,
(void *)(ev_intptr_t)fd);
event_add(&events[i], &tv_timeout);
}

fired = 0;


if (send(pipes[1], "e", 1, 0) < 0)
perror("send");

event_dispatch();

evutil_gettimeofday(&te, NULL);
evutil_timersub(&te, &ts, &te);

for (cp = pipes, i = 0; i < num_pipes; i++, cp += 2) {
event_del(&events[i]);
evutil_closesocket(cp[0]);
evutil_closesocket(cp[1]);
}

free(pipes);
free(events);

return (&te);
}

int
main(int argc, char **argv)
{
#if defined(HAVE_SETRLIMIT)
struct rlimit rl;
#endif
int i, c;
struct timeval *tv;

int num_pipes = 100;
#if defined(_WIN32)
WSADATA WSAData;
WSAStartup(0x101, &WSAData);
#endif

while ((c = getopt(argc, argv, "n:")) != -1) {
switch (c) {
case 'n':
num_pipes = atoi(optarg);
break;
default:
fprintf(stderr, "Illegal argument \"%c\"\n", c);
exit(1);
}
}

#if defined(HAVE_SETRLIMIT)
rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
if (setrlimit(RLIMIT_NOFILE, &rl) == -1) {
perror("setrlimit");
exit(1);
}
#endif

event_init();

for (i = 0; i < 25; i++) {
tv = run_once(num_pipes);
if (tv == NULL)
exit(1);
fprintf(stdout, "%ld\n",
tv->tv_sec * 1000000L + tv->tv_usec);
}

#if defined(_WIN32)
WSACleanup();
#endif

exit(0);
}
