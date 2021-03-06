

























#include <sys/types.h>
#include <sys/wait.h>
#include <spawn.h>
#include <signal.h>
#include <stdio.h>

void go(void);
void intr(int);

void
go(void)
{
pid_t pid;

(void) posix_spawn(&pid, "/bin/ls", NULL, NULL, NULL, NULL);

(void) waitpid(pid, NULL, 0);
}

void
intr(int sig __unused)
{
}

int
main(void)
{
struct sigaction sa;

sa.sa_handler = intr;
sigfillset(&sa.sa_mask);
sa.sa_flags = 0;

(void) sigaction(SIGUSR1, &sa, NULL);

for (;;) {
go();
}

return (0);
}
