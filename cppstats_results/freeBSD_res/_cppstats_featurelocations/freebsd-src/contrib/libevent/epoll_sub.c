

























#include "evconfig-private.h"
#include <stdint.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>

int
epoll_create(int size)
{
#if !defined(__NR_epoll_create) && defined(__NR_epoll_create1)
if (size <= 0) {
errno = EINVAL;
return -1;
}
return (syscall(__NR_epoll_create1, 0));
#else
return (syscall(__NR_epoll_create, size));
#endif
}

int
epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{

return (syscall(__NR_epoll_ctl, epfd, op, fd, event));
}

int
epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
#if !defined(__NR_epoll_wait) && defined(__NR_epoll_pwait)
return (syscall(__NR_epoll_pwait, epfd, events, maxevents, timeout, NULL, 0));
#else
return (syscall(__NR_epoll_wait, epfd, events, maxevents, timeout));
#endif
}
