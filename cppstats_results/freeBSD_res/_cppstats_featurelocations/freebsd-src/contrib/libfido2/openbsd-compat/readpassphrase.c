
























#include "openbsd-compat.h"

#if !defined(HAVE_READPASSPHRASE)

#include <termios.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <paths.h>

#if !defined(_PATH_TTY)
#define _PATH_TTY "/dev/tty"
#endif

#if !defined(TCSASOFT)

#define TCSASOFT 0
#endif


#if !defined(_POSIX_VDISABLE) && defined(VDISABLE)
#define _POSIX_VDISABLE VDISABLE
#endif

static volatile sig_atomic_t signo[NSIG];

static void handler(int);

char *
readpassphrase(const char *prompt, char *buf, size_t bufsiz, int flags)
{
ssize_t nr;
int input, output, save_errno, i, need_restart;
char ch, *p, *end;
struct termios term, oterm;
struct sigaction sa, savealrm, saveint, savehup, savequit, saveterm;
struct sigaction savetstp, savettin, savettou, savepipe;


if (bufsiz == 0) {
errno = EINVAL;
return(NULL);
}

restart:
for (i = 0; i < NSIG; i++)
signo[i] = 0;
need_restart = 0;




if ((flags & RPP_STDIN) ||
(input = output = open(_PATH_TTY, O_RDWR)) == -1) {
if (flags & RPP_REQUIRE_TTY) {
errno = ENOTTY;
return(NULL);
}
input = STDIN_FILENO;
output = STDERR_FILENO;
}






if (input != STDIN_FILENO && tcgetattr(input, &oterm) == 0) {
memcpy(&term, &oterm, sizeof(term));
if (!(flags & RPP_ECHO_ON))
term.c_lflag &= ~(ECHO | ECHONL);
#if defined(VSTATUS)
if (term.c_cc[VSTATUS] != _POSIX_VDISABLE)
term.c_cc[VSTATUS] = _POSIX_VDISABLE;
#endif
(void)tcsetattr(input, TCSAFLUSH|TCSASOFT, &term);
} else {
memset(&term, 0, sizeof(term));
term.c_lflag |= ECHO;
memset(&oterm, 0, sizeof(oterm));
oterm.c_lflag |= ECHO;
}






sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sa.sa_handler = handler;
(void)sigaction(SIGALRM, &sa, &savealrm);
(void)sigaction(SIGHUP, &sa, &savehup);
(void)sigaction(SIGINT, &sa, &saveint);
(void)sigaction(SIGPIPE, &sa, &savepipe);
(void)sigaction(SIGQUIT, &sa, &savequit);
(void)sigaction(SIGTERM, &sa, &saveterm);
(void)sigaction(SIGTSTP, &sa, &savetstp);
(void)sigaction(SIGTTIN, &sa, &savettin);
(void)sigaction(SIGTTOU, &sa, &savettou);

if (!(flags & RPP_STDIN))
(void)write(output, prompt, strlen(prompt));
end = buf + bufsiz - 1;
p = buf;
while ((nr = read(input, &ch, 1)) == 1 && ch != '\n' && ch != '\r') {
if (p < end) {
if ((flags & RPP_SEVENBIT))
ch &= 0x7f;
if (isalpha((unsigned char)ch)) {
if ((flags & RPP_FORCELOWER))
ch = (char)tolower((unsigned char)ch);
if ((flags & RPP_FORCEUPPER))
ch = (char)toupper((unsigned char)ch);
}
*p++ = ch;
}
}
*p = '\0';
save_errno = errno;
if (!(term.c_lflag & ECHO))
(void)write(output, "\n", 1);


if (memcmp(&term, &oterm, sizeof(term)) != 0) {
const int sigttou = signo[SIGTTOU];


while (tcsetattr(input, TCSAFLUSH|TCSASOFT, &oterm) == -1 &&
errno == EINTR && !signo[SIGTTOU])
continue;
signo[SIGTTOU] = sigttou;
}
(void)sigaction(SIGALRM, &savealrm, NULL);
(void)sigaction(SIGHUP, &savehup, NULL);
(void)sigaction(SIGINT, &saveint, NULL);
(void)sigaction(SIGQUIT, &savequit, NULL);
(void)sigaction(SIGPIPE, &savepipe, NULL);
(void)sigaction(SIGTERM, &saveterm, NULL);
(void)sigaction(SIGTSTP, &savetstp, NULL);
(void)sigaction(SIGTTIN, &savettin, NULL);
(void)sigaction(SIGTTOU, &savettou, NULL);
if (input != STDIN_FILENO)
(void)close(input);





for (i = 0; i < NSIG; i++) {
if (signo[i]) {
kill(getpid(), i);
switch (i) {
case SIGTSTP:
case SIGTTIN:
case SIGTTOU:
need_restart = 1;
}
}
}
if (need_restart)
goto restart;

if (save_errno)
errno = save_errno;
return(nr == -1 ? NULL : buf);
}

#if 0
char *
getpass(const char *prompt)
{
static char buf[_PASSWORD_LEN + 1];

return(readpassphrase(prompt, buf, sizeof(buf), RPP_ECHO_OFF));
}
#endif

static void handler(int s)
{

signo[s] = 1;
}
#endif
