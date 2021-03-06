










#include <event2/event-config.h>

#include <sys/types.h>
#include <sys/stat.h>
#if !defined(_WIN32)
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event.h>

static void
fifo_read(evutil_socket_t fd, short event, void *arg)
{
char buf[255];
int len;
struct event *ev = arg;
#if defined(_WIN32)
DWORD dwBytesRead;
#endif

fprintf(stderr, "fifo_read called with fd: %d, event: %d, arg: %p\n",
(int)fd, event, arg);
#if defined(_WIN32)
len = ReadFile((HANDLE)fd, buf, sizeof(buf) - 1, &dwBytesRead, NULL);


if (len && dwBytesRead == 0) {
fprintf(stderr, "End Of File");
event_del(ev);
return;
}

buf[dwBytesRead] = '\0';
#else
len = read(fd, buf, sizeof(buf) - 1);

if (len <= 0) {
if (len == -1)
perror("read");
else if (len == 0)
fprintf(stderr, "Connection closed\n");
event_del(ev);
event_base_loopbreak(event_get_base(ev));
return;
}

buf[len] = '\0';
#endif
fprintf(stdout, "Read: %s\n", buf);
}


#if !defined(_WIN32)
static void
signal_cb(evutil_socket_t fd, short event, void *arg)
{
struct event_base *base = arg;
event_base_loopbreak(base);
}
#endif

int
main(int argc, char **argv)
{
struct event *evfifo;
struct event_base* base;
#if defined(_WIN32)
HANDLE socket;

socket = CreateFileA("test.txt",
GENERIC_READ,
0,
NULL,
OPEN_EXISTING,
FILE_ATTRIBUTE_NORMAL,
NULL);

if (socket == INVALID_HANDLE_VALUE)
return 1;

#else
struct event *signal_int;
struct stat st;
const char *fifo = "event.fifo";
int socket;

if (lstat(fifo, &st) == 0) {
if ((st.st_mode & S_IFMT) == S_IFREG) {
errno = EEXIST;
perror("lstat");
exit(1);
}
}

unlink(fifo);
if (mkfifo(fifo, 0600) == -1) {
perror("mkfifo");
exit(1);
}

socket = open(fifo, O_RDONLY | O_NONBLOCK, 0);

if (socket == -1) {
perror("open");
exit(1);
}

fprintf(stderr, "Write data to %s\n", fifo);
#endif

base = event_base_new();


#if defined(_WIN32)
evfifo = event_new(base, (evutil_socket_t)socket, EV_READ|EV_PERSIST, fifo_read,
event_self_cbarg());
#else

signal_int = evsignal_new(base, SIGINT, signal_cb, base);
event_add(signal_int, NULL);

evfifo = event_new(base, socket, EV_READ|EV_PERSIST, fifo_read,
event_self_cbarg());
#endif


event_add(evfifo, NULL);

event_base_dispatch(base);
event_base_free(base);
#if defined(_WIN32)
CloseHandle(socket);
#else
close(socket);
unlink(fifo);
#endif
libevent_global_shutdown();
return (0);
}

