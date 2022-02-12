
































#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

static void
usage(void)
{

fprintf(stderr, "usage: pwait [-t timeout] [-ov] pid ...\n");
exit(EX_USAGE);
}




int
main(int argc, char *argv[])
{
struct itimerval itv;
struct kevent *e;
int oflag, tflag, verbose;
int i, kq, n, nleft, opt, status;
long pid;
char *end, *s;
double timeout;

oflag = 0;
tflag = 0;
verbose = 0;
memset(&itv, 0, sizeof(itv));

while ((opt = getopt(argc, argv, "ot:v")) != -1) {
switch (opt) {
case 'o':
oflag = 1;
break;
case 't':
tflag = 1;
errno = 0;
timeout = strtod(optarg, &end);
if (end == optarg || errno == ERANGE || timeout < 0) {
errx(EX_DATAERR, "timeout value");
}
switch(*end) {
case 0:
case 's':
break;
case 'h':
timeout *= 60;

case 'm':
timeout *= 60;
break;
default:
errx(EX_DATAERR, "timeout unit");
}
if (timeout > 100000000L) {
errx(EX_DATAERR, "timeout value");
}
itv.it_value.tv_sec = (time_t)timeout;
timeout -= (time_t)timeout;
itv.it_value.tv_usec =
(suseconds_t)(timeout * 1000000UL);
break;
case 'v':
verbose = 1;
break;
default:
usage();

}
}

argc -= optind;
argv += optind;

if (argc == 0) {
usage();
}

kq = kqueue();
if (kq == -1) {
err(EX_OSERR, "kqueue");
}

e = malloc((argc + tflag) * sizeof(struct kevent));
if (e == NULL) {
err(EX_OSERR, "malloc");
}
nleft = 0;
for (n = 0; n < argc; n++) {
s = argv[n];

if (!strncmp(s, "/proc/", 6)) {
s += 6;
}
errno = 0;
pid = strtol(s, &end, 10);
if (pid < 0 || *end != '\0' || errno != 0) {
warnx("%s: bad process id", s);
continue;
}
if (pid == getpid()) {
warnx("%s: skipping my own pid", s);
continue;
}
for (i = 0; i < nleft; i++) {
if (e[i].ident == (uintptr_t)pid) {
break;
}
}
if (i < nleft) {

continue;
}
EV_SET(e + nleft, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, NULL);
if (kevent(kq, e + nleft, 1, NULL, 0, NULL) == -1) {
warn("%ld", pid);
if (oflag) {
exit(EX_OK);
}
} else {
nleft++;
}
}

if (nleft > 0 && tflag) {




EV_SET(e + nleft, SIGALRM, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
if (kevent(kq, e + nleft, 1, NULL, 0, NULL) == -1) {
err(EX_OSERR, "kevent");
}

signal(SIGALRM, SIG_IGN);
if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
err(EX_OSERR, "setitimer");
}
}
while (nleft > 0) {
n = kevent(kq, NULL, 0, e, nleft + tflag, NULL);
if (n == -1) {
err(EX_OSERR, "kevent");
}
for (i = 0; i < n; i++) {
if (e[i].filter == EVFILT_SIGNAL) {
if (verbose) {
printf("timeout\n");
}
exit(124);
}
if (verbose) {
status = e[i].data;
if (WIFEXITED(status)) {
printf("%ld: exited with status %d.\n",
(long)e[i].ident,
WEXITSTATUS(status));
} else if (WIFSIGNALED(status)) {
printf("%ld: killed by signal %d.\n",
(long)e[i].ident,
WTERMSIG(status));
} else {
printf("%ld: terminated.\n",
(long)e[i].ident);
}
}
if (oflag) {
exit(EX_OK);
}
--nleft;
}
}

exit(EX_OK);
}
