#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
int go(long, long, long, long, long, long, long, long, long, long);
int
go(long arg0 __unused, long arg1 __unused, long arg2 __unused,
long arg3 __unused, long arg4 __unused, long arg5 __unused,
long arg6 __unused, long arg7 __unused, long arg8 __unused,
long arg9 __unused)
{
return (arg1);
}
static void
handle(int sig __unused)
{
go(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
exit(0);
}
int
main(void)
{
(void) signal(SIGUSR1, handle);
for (;;)
getpid();
}
