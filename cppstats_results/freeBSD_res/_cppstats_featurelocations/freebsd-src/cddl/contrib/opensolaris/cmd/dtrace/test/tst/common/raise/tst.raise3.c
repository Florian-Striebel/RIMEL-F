

























#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

static void
handle(int sig __unused)
{
exit(0);
}

int
main(void)
{
struct sigaction sa;

sa.sa_handler = handle;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;

(void) sigaction(SIGUSR1, &sa, NULL);

for (;;) {
(void) getpid();
}
}
