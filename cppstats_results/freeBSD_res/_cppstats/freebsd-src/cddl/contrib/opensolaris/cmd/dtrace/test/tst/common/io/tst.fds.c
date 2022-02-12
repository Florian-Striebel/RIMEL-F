#include <sys/ioctl.h>
#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
static sigjmp_buf env;
static void
interrupt(int sig)
{
siglongjmp(env, sig);
}
int
main(int argc, char *argv[])
{
const char *file = "/dev/null";
size_t i, n;
int fds[10];
struct sigaction act;
if (argc > 1) {
(void) fprintf(stderr, "Usage: %s\n", argv[0]);
return (EXIT_FAILURE);
}
act.sa_handler = interrupt;
act.sa_flags = 0;
(void) sigemptyset(&act.sa_mask);
(void) sigaction(SIGUSR1, &act, NULL);
closefrom(0);
n = 0;
if (sigsetjmp(env, 1) == 0) {
for (;;)
(void) ioctl(-1, 0, NULL);
}
fds[n++] = open(file, O_RDONLY);
fds[n++] = open(file, O_WRONLY);
fds[n++] = open(file, O_RDWR);
fds[n++] = open(file, O_RDWR | O_APPEND | O_CREAT |
O_NOCTTY | O_NONBLOCK | O_NDELAY | O_SYNC | O_TRUNC | 0666);
fds[n++] = open(file, O_RDWR);
(void) lseek(fds[n - 1], 123, SEEK_SET);
for (i = 0; i < n; i++)
(void) ioctl(fds[i], 0, NULL);
assert(n <= sizeof (fds) / sizeof (fds[0]));
exit(0);
}
