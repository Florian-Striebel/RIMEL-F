

























#include <signal.h>
#include <stdlib.h>
#include <unistd.h>






extern int a;

int go(void);

int a = 100;

int
go(void)
{
return (a);
}

static void
handle(int sig __unused)
{
go();
exit(0);
}

int
main(void)
{
(void) signal(SIGUSR1, handle);
for (;;)
getpid();
}
