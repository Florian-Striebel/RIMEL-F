

























#include <signal.h>
#include <stdlib.h>
#include <unistd.h>






extern int a;

int help(void);
int go(void);

int a = 100;

int
help(void)
{
return (a);
}

int
go(void)
{
return (help() + 1);
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
