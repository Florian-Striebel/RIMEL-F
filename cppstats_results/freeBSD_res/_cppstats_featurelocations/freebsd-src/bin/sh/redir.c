

































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)redir.c 8.2 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>





#include "shell.h"
#include "nodes.h"
#include "jobs.h"
#include "expand.h"
#include "redir.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "options.h"


#define EMPTY -2
#define CLOSED -1


struct redirtab {
struct redirtab *next;
int renamed[10];
int fd0_redirected;
unsigned int empty_redirs;
};


static struct redirtab *redirlist;






static int fd0_redirected = 0;


static unsigned int empty_redirs = 0;

static void openredirect(union node *, char[10 ]);
static int openhere(union node *);

















void
redirect(union node *redir, int flags)
{
union node *n;
struct redirtab *sv = NULL;
int i;
int fd;
char memory[10];

INTOFF;
for (i = 10 ; --i >= 0 ; )
memory[i] = 0;
memory[1] = flags & REDIR_BACKQ;
if (flags & REDIR_PUSH) {
empty_redirs++;
if (redir != NULL) {
sv = ckmalloc(sizeof (struct redirtab));
for (i = 0 ; i < 10 ; i++)
sv->renamed[i] = EMPTY;
sv->fd0_redirected = fd0_redirected;
sv->empty_redirs = empty_redirs - 1;
sv->next = redirlist;
redirlist = sv;
empty_redirs = 0;
}
}
for (n = redir ; n ; n = n->nfile.next) {
fd = n->nfile.fd;
if (fd == 0)
fd0_redirected = 1;
if ((n->nfile.type == NTOFD || n->nfile.type == NFROMFD) &&
n->ndup.dupfd == fd)
continue;

if ((flags & REDIR_PUSH) && sv->renamed[fd] == EMPTY) {
INTOFF;
if ((i = fcntl(fd, F_DUPFD_CLOEXEC, 10)) == -1) {
switch (errno) {
case EBADF:
i = CLOSED;
break;
default:
INTON;
error("%d: %s", fd, strerror(errno));
break;
}
}
sv->renamed[fd] = i;
INTON;
}
openredirect(n, memory);
INTON;
INTOFF;
}
if (memory[1])
out1 = &memout;
if (memory[2])
out2 = &memout;
INTON;
}


static void
openredirect(union node *redir, char memory[10])
{
struct stat sb;
int fd = redir->nfile.fd;
const char *fname;
int f;
int e;

memory[fd] = 0;
switch (redir->nfile.type) {
case NFROM:
fname = redir->nfile.expfname;
if ((f = open(fname, O_RDONLY)) < 0)
error("cannot open %s: %s", fname, strerror(errno));
break;
case NFROMTO:
fname = redir->nfile.expfname;
if ((f = open(fname, O_RDWR|O_CREAT, 0666)) < 0)
error("cannot create %s: %s", fname, strerror(errno));
break;
case NTO:
if (Cflag) {
fname = redir->nfile.expfname;
if (stat(fname, &sb) == -1) {
if ((f = open(fname, O_WRONLY|O_CREAT|O_EXCL, 0666)) < 0)
error("cannot create %s: %s", fname, strerror(errno));
} else if (!S_ISREG(sb.st_mode)) {
if ((f = open(fname, O_WRONLY, 0666)) < 0)
error("cannot create %s: %s", fname, strerror(errno));
if (fstat(f, &sb) != -1 && S_ISREG(sb.st_mode)) {
close(f);
error("cannot create %s: %s", fname,
strerror(EEXIST));
}
} else
error("cannot create %s: %s", fname,
strerror(EEXIST));
break;
}

case NCLOBBER:
fname = redir->nfile.expfname;
if ((f = open(fname, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
error("cannot create %s: %s", fname, strerror(errno));
break;
case NAPPEND:
fname = redir->nfile.expfname;
if ((f = open(fname, O_WRONLY|O_CREAT|O_APPEND, 0666)) < 0)
error("cannot create %s: %s", fname, strerror(errno));
break;
case NTOFD:
case NFROMFD:
if (redir->ndup.dupfd >= 0) {
if (memory[redir->ndup.dupfd])
memory[fd] = 1;
else {
if (dup2(redir->ndup.dupfd, fd) < 0)
error("%d: %s", redir->ndup.dupfd,
strerror(errno));
}
} else {
close(fd);
}
return;
case NHERE:
case NXHERE:
f = openhere(redir);
break;
default:
abort();
}
if (f != fd) {
if (dup2(f, fd) == -1) {
e = errno;
close(f);
error("%d: %s", fd, strerror(e));
}
close(f);
}
}








static int
openhere(union node *redir)
{
const char *p;
int pip[2];
size_t len = 0;
int flags;
ssize_t written = 0;

if (pipe(pip) < 0)
error("Pipe call failed: %s", strerror(errno));

if (redir->type == NXHERE)
p = redir->nhere.expdoc;
else
p = redir->nhere.doc->narg.text;
len = strlen(p);
if (len == 0)
goto out;
flags = fcntl(pip[1], F_GETFL, 0);
if (flags != -1 && fcntl(pip[1], F_SETFL, flags | O_NONBLOCK) != -1) {
written = write(pip[1], p, len);
if (written < 0)
written = 0;
if ((size_t)written == len)
goto out;
fcntl(pip[1], F_SETFL, flags);
}

if (forkshell((struct job *)NULL, (union node *)NULL, FORK_NOJOB) == 0) {
close(pip[0]);
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
signal(SIGHUP, SIG_IGN);
signal(SIGTSTP, SIG_IGN);
signal(SIGPIPE, SIG_DFL);
xwrite(pip[1], p + written, len - written);
_exit(0);
}
out:
close(pip[1]);
return pip[0];
}







void
popredir(void)
{
struct redirtab *rp = redirlist;
int i;

INTOFF;
if (empty_redirs > 0) {
empty_redirs--;
INTON;
return;
}
for (i = 0 ; i < 10 ; i++) {
if (rp->renamed[i] != EMPTY) {
if (rp->renamed[i] >= 0) {
dup2(rp->renamed[i], i);
close(rp->renamed[i]);
} else {
close(i);
}
}
}
fd0_redirected = rp->fd0_redirected;
empty_redirs = rp->empty_redirs;
redirlist = rp->next;
ckfree(rp);
INTON;
}


int
fd0_redirected_p(void)
{
return fd0_redirected != 0;
}





void
clearredir(void)
{
struct redirtab *rp;
int i;

for (rp = redirlist ; rp ; rp = rp->next) {
for (i = 0 ; i < 10 ; i++) {
if (rp->renamed[i] >= 0) {
close(rp->renamed[i]);
}
rp->renamed[i] = EMPTY;
}
}
}
