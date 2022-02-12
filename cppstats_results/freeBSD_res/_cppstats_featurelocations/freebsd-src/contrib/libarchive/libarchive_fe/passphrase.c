


















































#include "lafe_platform.h"
__FBSDID("$FreeBSD$");

#include <errno.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_READPASSPHRASE_H)
#include <readpassphrase.h>
#endif

#include "err.h"
#include "passphrase.h"

#if !defined(HAVE_READPASSPHRASE)

#define RPP_ECHO_OFF 0x00
#define RPP_ECHO_ON 0x01
#define RPP_REQUIRE_TTY 0x02
#define RPP_FORCELOWER 0x04
#define RPP_FORCEUPPER 0x08
#define RPP_SEVENBIT 0x10
#define RPP_STDIN 0x20


#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>

static char *
readpassphrase(const char *prompt, char *buf, size_t bufsiz, int flags)
{
HANDLE hStdin, hStdout;
DWORD mode, rbytes;
BOOL success;

(void)flags;

hStdin = GetStdHandle(STD_INPUT_HANDLE);
if (hStdin == INVALID_HANDLE_VALUE)
return (NULL);
hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
if (hStdout == INVALID_HANDLE_VALUE)
return (NULL);

success = GetConsoleMode(hStdin, &mode);
if (!success)
return (NULL);
mode &= ~ENABLE_ECHO_INPUT;
mode |= ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT;
success = SetConsoleMode(hStdin, mode);
if (!success)
return (NULL);

success = WriteFile(hStdout, prompt, (DWORD)strlen(prompt),
NULL, NULL);
if (!success)
return (NULL);
success = ReadFile(hStdin, buf, (DWORD)bufsiz - 1, &rbytes, NULL);
if (!success)
return (NULL);
WriteFile(hStdout, "\r\n", 2, NULL, NULL);
buf[rbytes] = '\0';

if (rbytes > 2 && buf[rbytes - 2] == '\r' && buf[rbytes - 1] == '\n')
buf[rbytes - 2] = '\0';

return (buf);
}

#else

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#if defined(HAVE_PATHS_H)
#include <paths.h>
#endif
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#if !defined(_PATH_TTY)
#define _PATH_TTY "/dev/tty"
#endif

#if defined(TCSASOFT)
#define _T_FLUSH (TCSAFLUSH|TCSASOFT)
#else
#define _T_FLUSH (TCSAFLUSH)
#endif


#if !defined(_POSIX_VDISABLE) && defined(VDISABLE)
#define _POSIX_VDISABLE VDISABLE
#endif

#define M(a,b) (a > b ? a : b)
#define MAX_SIGNO M(M(M(SIGALRM, SIGHUP), M(SIGINT, SIGPIPE)), M(M(SIGQUIT, SIGTERM), M(M(SIGTSTP, SIGTTIN), SIGTTOU)))




static volatile sig_atomic_t signo[MAX_SIGNO + 1];

static void
handler(int s)
{
assert(s <= MAX_SIGNO);
signo[s] = 1;
}

static char *
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
for (i = 0; i <= MAX_SIGNO; i++)
signo[i] = 0;
nr = -1;
save_errno = 0;
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
(void)tcsetattr(input, _T_FLUSH, &term);
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

if (!(flags & RPP_STDIN)) {
int r = write(output, prompt, strlen(prompt));
(void)r;
}
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
if (!(term.c_lflag & ECHO)) {
int r = write(output, "\n", 1);
(void)r;
}


if (memcmp(&term, &oterm, sizeof(term)) != 0) {
const int sigttou = signo[SIGTTOU];


while (tcsetattr(input, _T_FLUSH, &oterm) == -1 &&
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





for (i = 0; i <= MAX_SIGNO; i++) {
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
#endif
#endif

char *
lafe_readpassphrase(const char *prompt, char *buf, size_t bufsiz)
{
char *p;

p = readpassphrase(prompt, buf, bufsiz, RPP_ECHO_OFF);
if (p == NULL) {
switch (errno) {
case EINTR:
break;
default:
lafe_errc(1, errno, "Couldn't read passphrase");
break;
}
}
return (p);
}

