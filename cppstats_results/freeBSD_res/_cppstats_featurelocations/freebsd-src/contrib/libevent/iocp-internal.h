

























#if !defined(IOCP_INTERNAL_H_INCLUDED_)
#define IOCP_INTERNAL_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

struct event_overlapped;
struct event_iocp_port;
struct evbuffer;
typedef void (*iocp_callback)(struct event_overlapped *, ev_uintptr_t, ev_ssize_t, int success);




#if defined(_WIN32)








struct event_overlapped {
OVERLAPPED overlapped;
iocp_callback cb;
};



typedef BOOL (WINAPI *AcceptExPtr)(SOCKET, SOCKET, PVOID, DWORD, DWORD, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL (WINAPI *ConnectExPtr)(SOCKET, const struct sockaddr *, int, PVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef void (WINAPI *GetAcceptExSockaddrsPtr)(PVOID, DWORD, DWORD, DWORD, LPSOCKADDR *, LPINT, LPSOCKADDR *, LPINT);




struct win32_extension_fns {
AcceptExPtr AcceptEx;
ConnectExPtr ConnectEx;
GetAcceptExSockaddrsPtr GetAcceptExSockaddrs;
};





struct event_iocp_port {

HANDLE port;

CRITICAL_SECTION lock;

short n_threads;

short shutdown;


long ms;

HANDLE *threads;

short n_live_threads;

HANDLE *shutdownSemaphore;
};

const struct win32_extension_fns *event_get_win32_extension_fns_(void);
#else

struct event_overlapped {
iocp_callback cb;
};
#endif







void event_overlapped_init_(struct event_overlapped *, iocp_callback cb);





struct evbuffer *evbuffer_overlapped_new_(evutil_socket_t fd);


evutil_socket_t evbuffer_overlapped_get_fd_(struct evbuffer *buf);

void evbuffer_overlapped_set_fd_(struct evbuffer *buf, evutil_socket_t fd);













int evbuffer_launch_read_(struct evbuffer *buf, size_t n, struct event_overlapped *ol);













int evbuffer_launch_write_(struct evbuffer *buf, ev_ssize_t n, struct event_overlapped *ol);


void evbuffer_commit_read_(struct evbuffer *, ev_ssize_t);
void evbuffer_commit_write_(struct evbuffer *, ev_ssize_t);





struct event_iocp_port *event_iocp_port_launch_(int n_cpus);




int event_iocp_port_associate_(struct event_iocp_port *port, evutil_socket_t fd,
ev_uintptr_t key);







int event_iocp_shutdown_(struct event_iocp_port *port, long waitMsec);


int event_iocp_activate_overlapped_(struct event_iocp_port *port,
struct event_overlapped *o,
ev_uintptr_t key, ev_uint32_t n_bytes);

struct event_base;

struct event_iocp_port *event_base_get_iocp_(struct event_base *base);


int event_base_start_iocp_(struct event_base *base, int n_cpus);
void event_base_stop_iocp_(struct event_base *base);


struct bufferevent *bufferevent_async_new_(struct event_base *base,
evutil_socket_t fd, int options);


void bufferevent_async_set_connected_(struct bufferevent *bev);
int bufferevent_async_can_connect_(struct bufferevent *bev);
int bufferevent_async_connect_(struct bufferevent *bev, evutil_socket_t fd,
const struct sockaddr *sa, int socklen);

#if defined(__cplusplus)
}
#endif

#endif
