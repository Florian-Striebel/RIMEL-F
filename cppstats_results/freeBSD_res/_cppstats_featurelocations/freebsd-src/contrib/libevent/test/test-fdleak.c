

























#include "event2/event-config.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(EVENT__HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(EVENT__HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"



#if defined(_WIN32)
#define MAX_REQUESTS 1000
#else
#define MAX_REQUESTS 4000
#endif


static struct sockaddr_in saddr;


static int num_requests;

static void start_client(struct event_base *base);

static void
my_perror(const char *s)
{
fprintf(stderr, "%s: %s",
s, evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
}








static void
server_read_cb(struct bufferevent *bev, void *ctx)
{
while (evbuffer_get_length(bufferevent_get_input(bev))) {
unsigned char tmp;
bufferevent_read(bev, &tmp, 1);
bufferevent_write(bev, &tmp, 1);
}
}


static void
server_event_cb(struct bufferevent *bev, short events, void *ctx)
{
if (events & BEV_EVENT_ERROR) {
my_perror("Error from bufferevent");
exit(1);
} else if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
bufferevent_free(bev);
}
}


static void
listener_accept_cb(struct evconnlistener *listener, evutil_socket_t sock,
struct sockaddr *addr, int len, void *ptr)
{
struct event_base *base = evconnlistener_get_base(listener);
struct bufferevent *bev = bufferevent_socket_new(base, sock,
BEV_OPT_CLOSE_ON_FREE);

bufferevent_setcb(bev, server_read_cb, NULL, server_event_cb, NULL);
bufferevent_enable(bev, EV_READ|EV_WRITE);
}


static void
start_loop(void)
{
struct event_base *base;
struct evconnlistener *listener;
struct sockaddr_storage ss;
ev_socklen_t socklen = sizeof(ss);
evutil_socket_t fd;

base = event_base_new();
if (base == NULL) {
puts("Could not open event base!");
exit(1);
}

listener = evconnlistener_new_bind(base, listener_accept_cb, NULL,
LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
-1, (struct sockaddr *)&saddr, sizeof(saddr));
if (listener == NULL) {
my_perror("Could not create listener!");
exit(1);
}
fd = evconnlistener_get_fd(listener);
if (fd < 0) {
puts("Couldn't get fd from listener");
exit(1);
}
if (getsockname(fd, (struct sockaddr *)&ss, &socklen) < 0) {
my_perror("getsockname()");
exit(1);
}
memcpy(&saddr, &ss, sizeof(saddr));
if (saddr.sin_family != AF_INET) {
puts("AF mismatch from getsockname().");
exit(1);
}

start_client(base);

event_base_dispatch(base);
}









static void
client_read_cb(struct bufferevent *bev, void *ctx)
{
unsigned char tmp;
struct event_base *base = bufferevent_get_base(bev);

bufferevent_read(bev, &tmp, 1);
if (tmp != 'A') {
puts("Incorrect data received!");
exit(2);
}
bufferevent_free(bev);

num_requests++;
if (num_requests == MAX_REQUESTS) {
event_base_loopbreak(base);
} else {
start_client(base);
}
}


static void
client_event_cb(struct bufferevent *bev, short events, void *ctx)
{
if (events & BEV_EVENT_CONNECTED) {
unsigned char tmp = 'A';
bufferevent_write(bev, &tmp, 1);
} else if (events & BEV_EVENT_ERROR) {
puts("Client socket got error!");
exit(2);
}

bufferevent_enable(bev, EV_READ);
}


static void
start_client(struct event_base *base)
{
struct bufferevent *bev = bufferevent_socket_new(base, -1,
BEV_OPT_CLOSE_ON_FREE);
bufferevent_setcb(bev, client_read_cb, NULL, client_event_cb, NULL);

if (bufferevent_socket_connect(bev, (struct sockaddr *)&saddr,
sizeof(saddr)) < 0) {
my_perror("Could not connect!");
bufferevent_free(bev);
exit(2);
}
}

int
main(int argc, char **argv)
{
#if defined(EVENT__HAVE_SETRLIMIT)


struct rlimit rl;
rl.rlim_cur = rl.rlim_max = 20;
if (setrlimit(RLIMIT_NOFILE, &rl) == -1) {
my_perror("setrlimit");
exit(3);
}
#endif

#if defined(_WIN32)
WSADATA WSAData;
WSAStartup(0x101, &WSAData);
#endif


memset(&saddr, 0, sizeof(saddr));
saddr.sin_family = AF_INET;
saddr.sin_addr.s_addr = htonl(0x7f000001);
saddr.sin_port = 0;

start_loop();

return 0;
}


