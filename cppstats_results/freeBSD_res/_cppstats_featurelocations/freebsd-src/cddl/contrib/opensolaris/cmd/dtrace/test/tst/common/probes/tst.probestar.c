
























#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int
main(void)
{

sigset_t set;
siginfo_t info;
struct timespec timeout;

(void)sigemptyset(&set);
(void)sigaddset(&set, SIGHUP);
timeout.tv_sec = 1;
timeout.tv_nsec = 0;

for (;;)
(void)sigtimedwait(&set, &info, &timeout);

return (0);
}
