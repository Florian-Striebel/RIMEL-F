

























#include <signal.h>
#include <stdlib.h>
#include <unistd.h>






extern int _go(int);

static int __unused
go(int a)
{
return (a + 1);
}

static void
handle(int sig __unused)
{
_go(1);
exit(0);
}

int
main(void)
{
(void) signal(SIGUSR1, handle);
for (;;)
getpid();
}

#pragma weak _go = go
