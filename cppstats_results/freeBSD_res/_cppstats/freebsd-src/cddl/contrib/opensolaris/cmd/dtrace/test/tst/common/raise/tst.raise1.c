#include <unistd.h>
#include <signal.h>
int
main(void)
{
sigset_t ss;
(void) sigemptyset(&ss);
(void) sigaddset(&ss, SIGUSR1);
(void) sigprocmask(SIG_BLOCK, &ss, NULL);
do {
(void) getpid();
(void) sigpending(&ss);
} while (!sigismember(&ss, SIGUSR1));
return (0);
}
