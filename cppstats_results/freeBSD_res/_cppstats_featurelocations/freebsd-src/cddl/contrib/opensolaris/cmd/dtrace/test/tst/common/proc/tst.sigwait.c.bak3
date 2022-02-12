

























#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define NANOSEC 1000000000

int
main(int argc __unused, char **argv)
{
struct sigevent ev;
struct itimerspec ts;
sigset_t set;
timer_t tid;
char *cmd = argv[0];
int sig;

ev.sigev_notify = SIGEV_SIGNAL;
ev.sigev_signo = SIGUSR1;

if (timer_create(CLOCK_REALTIME, &ev, &tid) == -1) {
(void) fprintf(stderr, "%s: cannot create CLOCK_HIGHRES "
"timer: %s\n", cmd, strerror(errno));
exit(EXIT_FAILURE);
}

(void) sigemptyset(&set);
(void) sigaddset(&set, SIGUSR1);
(void) sigprocmask(SIG_BLOCK, &set, NULL);

ts.it_value.tv_sec = 1;
ts.it_value.tv_nsec = 0;
ts.it_interval.tv_sec = 0;
ts.it_interval.tv_nsec = NANOSEC / 2;

if (timer_settime(tid, TIMER_RELTIME, &ts, NULL) == -1) {
(void) fprintf(stderr, "%s: timer_settime() failed: %s\n",
cmd, strerror(errno));
exit(EXIT_FAILURE);
}

do {
(void) sigwait(&set, &sig);
} while(sig != SIGUSR1);


return (0);
}
