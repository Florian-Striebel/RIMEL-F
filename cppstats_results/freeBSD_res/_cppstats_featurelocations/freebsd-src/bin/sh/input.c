































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)input.c 8.3 (Berkeley) 6/9/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>





#include "shell.h"
#include "redir.h"
#include "syntax.h"
#include "input.h"
#include "output.h"
#include "options.h"
#include "memalloc.h"
#include "error.h"
#include "alias.h"
#include "parser.h"
#if !defined(NO_HISTORY)
#include "myhistedit.h"
#endif
#include "trap.h"

#define EOF_NLEFT -99

struct strpush {
struct strpush *prev;
const char *prevstring;
int prevnleft;
int prevlleft;
struct alias *ap;
};






struct parsefile {
struct parsefile *prev;
int linno;
int fd;
int nleft;
int lleft;
const char *nextc;
char *buf;
struct strpush *strpush;
struct strpush basestrpush;
};


int plinno = 1;
int parsenleft;
static int parselleft;
const char *parsenextc;
static char basebuf[BUFSIZ + 1];
static struct parsefile basepf = {
.nextc = basebuf,
.buf = basebuf
};
static struct parsefile *parsefile = &basepf;
int whichprompt;

static void pushfile(void);
static int preadfd(void);
static void popstring(void);

void
resetinput(void)
{
popallfiles();
parselleft = parsenleft = 0;
}








int
pgetc(void)
{
return pgetc_macro();
}


static int
preadfd(void)
{
int nr;
parsenextc = parsefile->buf;

retry:
#if !defined(NO_HISTORY)
if (parsefile->fd == 0 && el) {
static const char *rl_cp;
static int el_len;

if (rl_cp == NULL) {
el_resize(el);
rl_cp = el_gets(el, &el_len);
}
if (rl_cp == NULL)
nr = el_len == 0 ? 0 : -1;
else {
nr = el_len;
if (nr > BUFSIZ)
nr = BUFSIZ;
memcpy(parsefile->buf, rl_cp, nr);
if (nr != el_len) {
el_len -= nr;
rl_cp += nr;
} else
rl_cp = NULL;
}
} else
#endif
nr = read(parsefile->fd, parsefile->buf, BUFSIZ);

if (nr <= 0) {
if (nr < 0) {
if (errno == EINTR)
goto retry;
if (parsefile->fd == 0 && errno == EWOULDBLOCK) {
int flags = fcntl(0, F_GETFL, 0);
if (flags >= 0 && flags & O_NONBLOCK) {
flags &=~ O_NONBLOCK;
if (fcntl(0, F_SETFL, flags) >= 0) {
out2fmt_flush("sh: turning off NDELAY mode\n");
goto retry;
}
}
}
}
nr = -1;
}
return nr;
}











int
preadbuffer(void)
{
char *p, *q, *r, *end;
char savec;

while (parsefile->strpush) {





if (parsenleft == -1 && parsefile->strpush->ap != NULL)
return ' ';
popstring();
if (--parsenleft >= 0)
return (*parsenextc++);
}
if (parsenleft == EOF_NLEFT || parsefile->buf == NULL)
return PEOF;

again:
if (parselleft <= 0) {
if ((parselleft = preadfd()) == -1) {
parselleft = parsenleft = EOF_NLEFT;
return PEOF;
}
}

p = parsefile->buf + (parsenextc - parsefile->buf);
end = p + parselleft;
*end = '\0';
q = strchrnul(p, '\n');
if (q != end && *q == '\0') {

for (r = q; q != end; q++) {
if (*q != '\0')
*r++ = *q;
}
parselleft -= end - r;
if (parselleft == 0)
goto again;
end = p + parselleft;
*end = '\0';
q = strchrnul(p, '\n');
}
if (q == end) {
parsenleft = parselleft;
parselleft = 0;
} else {
q++;
parsenleft = q - parsenextc;
parselleft -= parsenleft;
}
parsenleft--;

savec = *q;
*q = '\0';

#if !defined(NO_HISTORY)
if (parsefile->fd == 0 && hist &&
parsenextc[strspn(parsenextc, " \t\n")] != '\0') {
HistEvent he;
INTOFF;
history(hist, &he, whichprompt == 1 ? H_ENTER : H_ADD,
parsenextc);
INTON;
}
#endif

if (vflag) {
out2str(parsenextc);
flushout(out2);
}

*q = savec;

return *parsenextc++;
}






int
preadateof(void)
{
if (parsenleft > 0)
return 0;
if (parsefile->strpush)
return 0;
if (parsenleft == EOF_NLEFT || parsefile->buf == NULL)
return 1;
return 0;
}






void
pungetc(void)
{
parsenleft++;
parsenextc--;
}





void
pushstring(const char *s, int len, struct alias *ap)
{
struct strpush *sp;

INTOFF;

if (parsefile->strpush) {
sp = ckmalloc(sizeof (struct strpush));
sp->prev = parsefile->strpush;
parsefile->strpush = sp;
} else
sp = parsefile->strpush = &(parsefile->basestrpush);
sp->prevstring = parsenextc;
sp->prevnleft = parsenleft;
sp->prevlleft = parselleft;
sp->ap = ap;
if (ap)
ap->flag |= ALIASINUSE;
parsenextc = s;
parsenleft = len;
INTON;
}

static void
popstring(void)
{
struct strpush *sp = parsefile->strpush;

INTOFF;
if (sp->ap) {
if (parsenextc != sp->ap->val &&
(parsenextc[-1] == ' ' || parsenextc[-1] == '\t'))
forcealias();
sp->ap->flag &= ~ALIASINUSE;
}
parsenextc = sp->prevstring;
parsenleft = sp->prevnleft;
parselleft = sp->prevlleft;

parsefile->strpush = sp->prev;
if (sp != &(parsefile->basestrpush))
ckfree(sp);
INTON;
}










void
setinputfile(const char *fname, int push, int verify)
{
int e;
int fd;
int fd2;
int oflags = O_RDONLY | O_CLOEXEC;

if (verify == 1 || (verify == -1 && verifyflag))
oflags |= O_VERIFY;

INTOFF;
if ((fd = open(fname, oflags)) < 0) {
e = errno;
errorwithstatus(e == ENOENT || e == ENOTDIR ? 127 : 126,
"cannot open %s: %s", fname, strerror(e));
}
if (fd < 10) {
fd2 = fcntl(fd, F_DUPFD_CLOEXEC, 10);
close(fd);
if (fd2 < 0)
error("Out of file descriptors");
fd = fd2;
}
setinputfd(fd, push);
INTON;
}







void
setinputfd(int fd, int push)
{
if (push) {
pushfile();
parsefile->buf = ckmalloc(BUFSIZ + 1);
}
if (parsefile->fd > 0)
close(parsefile->fd);
parsefile->fd = fd;
if (parsefile->buf == NULL)
parsefile->buf = ckmalloc(BUFSIZ + 1);
parselleft = parsenleft = 0;
plinno = 1;
}






void
setinputstring(const char *string, int push)
{
INTOFF;
if (push)
pushfile();
parsenextc = string;
parselleft = parsenleft = strlen(string);
parsefile->buf = NULL;
plinno = 1;
INTON;
}








static void
pushfile(void)
{
struct parsefile *pf;

parsefile->nleft = parsenleft;
parsefile->lleft = parselleft;
parsefile->nextc = parsenextc;
parsefile->linno = plinno;
pf = (struct parsefile *)ckmalloc(sizeof (struct parsefile));
pf->prev = parsefile;
pf->fd = -1;
pf->strpush = NULL;
pf->basestrpush.prev = NULL;
parsefile = pf;
}


void
popfile(void)
{
struct parsefile *pf = parsefile;

INTOFF;
if (pf->fd >= 0)
close(pf->fd);
if (pf->buf)
ckfree(pf->buf);
while (pf->strpush)
popstring();
parsefile = pf->prev;
ckfree(pf);
parsenleft = parsefile->nleft;
parselleft = parsefile->lleft;
parsenextc = parsefile->nextc;
plinno = parsefile->linno;
INTON;
}






struct parsefile *
getcurrentfile(void)
{
return parsefile;
}








void
popfilesupto(struct parsefile *file)
{
while (parsefile != file && parsefile != &basepf)
popfile();
if (parsefile != file)
error("popfilesupto() misused");
}





void
popallfiles(void)
{
while (parsefile != &basepf)
popfile();
}








void
closescript(void)
{
popallfiles();
if (parsefile->fd > 0) {
close(parsefile->fd);
parsefile->fd = 0;
}
}
