






























#include "filemon.h"

#include <sys/ioctl.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(HAVE_FILEMON_H)
#include <filemon.h>
#endif

#if !defined(_PATH_FILEMON)
#define _PATH_FILEMON "/dev/filemon"
#endif

struct filemon {
int fd;
};

const char *
filemon_path(void)
{

return _PATH_FILEMON;
}

struct filemon *
filemon_open(void)
{
struct filemon *F;
unsigned i;
int error;


F = calloc(1, sizeof *F);
if (F == NULL)
return NULL;


for (i = 0; (F->fd = open(_PATH_FILEMON, O_RDWR|O_CLOEXEC)) == -1; i++) {
if (i == 5) {
error = errno;
goto fail0;
}
}


return F;

fail0: free(F);
errno = error;
return NULL;
}

int
filemon_setfd(struct filemon *F, int fd)
{


if (ioctl(F->fd, FILEMON_SET_FD, &fd) == -1)
return -1;


(void)close(fd);


return 0;
}

void
filemon_setpid_parent(struct filemon *F, pid_t pid)
{

}

int
filemon_setpid_child(const struct filemon *F, pid_t pid)
{


return ioctl(F->fd, FILEMON_SET_PID, &pid);
}

int
filemon_close(struct filemon *F)
{
int error = 0;


if (close(F->fd) == -1 && error == 0)
error = errno;


free(F);


if (error != 0) {
errno = error;
return -1;
}


return 0;
}

int
filemon_readfd(const struct filemon *F)
{

return -1;
}

int
filemon_process(struct filemon *F)
{

return 0;
}
