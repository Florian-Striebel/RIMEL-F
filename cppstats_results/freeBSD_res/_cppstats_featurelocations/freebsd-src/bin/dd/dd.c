


































#if 0
#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1991, 1993, 1994\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)dd.c 8.5 (Berkeley) 4/2/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/capsicum.h>
#include <sys/conf.h>
#include <sys/disklabel.h>
#include <sys/filio.h>
#include <sys/mtio.h>
#include <sys/time.h>

#include <assert.h>
#include <capsicum_helpers.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "dd.h"
#include "extern.h"

static void dd_close(void);
static void dd_in(void);
static void getfdtype(IO *);
static void setup(void);

IO in, out;
STAT st;
void (*cfunc)(void);
uintmax_t cpy_cnt;
static off_t pending = 0;
uint64_t ddflags = 0;
size_t cbsz;
uintmax_t files_cnt = 1;
const u_char *ctab;
char fill_char;
size_t speed = 0;
volatile sig_atomic_t need_summary;
volatile sig_atomic_t need_progress;

int
main(int argc __unused, char *argv[])
{
struct itimerval itv = { { 1, 0 }, { 1, 0 } };

(void)setlocale(LC_CTYPE, "");
jcl(argv);
setup();

caph_cache_catpages();
if (caph_enter() < 0)
err(1, "unable to enter capability mode");

(void)signal(SIGINFO, siginfo_handler);
if (ddflags & C_PROGRESS) {
(void)signal(SIGALRM, sigalarm_handler);
setitimer(ITIMER_REAL, &itv, NULL);
}
(void)signal(SIGINT, terminate);

atexit(summary);

while (files_cnt--)
dd_in();

dd_close();






if (close(out.fd) == -1 && errno != EINTR)
err(1, "close");
exit(0);
}

static int
parity(u_char c)
{
int i;

i = c ^ (c >> 1) ^ (c >> 2) ^ (c >> 3) ^
(c >> 4) ^ (c >> 5) ^ (c >> 6) ^ (c >> 7);
return (i & 1);
}

static void
setup(void)
{
u_int cnt;
int iflags, oflags;
cap_rights_t rights;
unsigned long cmds[] = { FIODTYPE, MTIOCTOP };

if (in.name == NULL) {
in.name = "stdin";
in.fd = STDIN_FILENO;
} else {
iflags = 0;
if (ddflags & C_IDIRECT)
iflags |= O_DIRECT;
in.fd = open(in.name, O_RDONLY | iflags, 0);
if (in.fd == -1)
err(1, "%s", in.name);
}

getfdtype(&in);

cap_rights_init(&rights, CAP_READ, CAP_SEEK);
if (caph_rights_limit(in.fd, &rights) == -1)
err(1, "unable to limit capability rights");

if (files_cnt > 1 && !(in.flags & ISTAPE))
errx(1, "files is not supported for non-tape devices");

cap_rights_set(&rights, CAP_FTRUNCATE, CAP_IOCTL, CAP_WRITE);
if (ddflags & (C_FDATASYNC | C_FSYNC))
cap_rights_set(&rights, CAP_FSYNC);
if (out.name == NULL) {

out.fd = STDOUT_FILENO;
out.name = "stdout";
if (ddflags & C_OFSYNC) {
oflags = fcntl(out.fd, F_GETFL);
if (oflags == -1)
err(1, "unable to get fd flags for stdout");
oflags |= O_FSYNC;
if (fcntl(out.fd, F_SETFL, oflags) == -1)
err(1, "unable to set fd flags for stdout");
}
} else {
oflags = O_CREAT;
if (!(ddflags & (C_SEEK | C_NOTRUNC)))
oflags |= O_TRUNC;
if (ddflags & C_OFSYNC)
oflags |= O_FSYNC;
if (ddflags & C_ODIRECT)
oflags |= O_DIRECT;
out.fd = open(out.name, O_RDWR | oflags, DEFFILEMODE);





if (out.fd == -1) {
out.fd = open(out.name, O_WRONLY | oflags, DEFFILEMODE);
out.flags |= NOREAD;
cap_rights_clear(&rights, CAP_READ);
}
if (out.fd == -1)
err(1, "%s", out.name);
}

getfdtype(&out);

if (caph_rights_limit(out.fd, &rights) == -1)
err(1, "unable to limit capability rights");
if (caph_ioctls_limit(out.fd, cmds, nitems(cmds)) == -1)
err(1, "unable to limit capability rights");

if (in.fd != STDIN_FILENO && out.fd != STDIN_FILENO) {
if (caph_limit_stdin() == -1)
err(1, "unable to limit capability rights");
}

if (in.fd != STDOUT_FILENO && out.fd != STDOUT_FILENO) {
if (caph_limit_stdout() == -1)
err(1, "unable to limit capability rights");
}

if (in.fd != STDERR_FILENO && out.fd != STDERR_FILENO) {
if (caph_limit_stderr() == -1)
err(1, "unable to limit capability rights");
}





if (!(ddflags & (C_BLOCK | C_UNBLOCK))) {
if ((in.db = malloc((size_t)out.dbsz + in.dbsz - 1)) == NULL)
err(1, "input buffer");
out.db = in.db;
} else if ((in.db = malloc(MAX((size_t)in.dbsz, cbsz) + cbsz)) == NULL ||
(out.db = malloc(out.dbsz + cbsz)) == NULL)
err(1, "output buffer");


in.dbp = in.db;
out.dbp = out.db;


if (in.offset)
pos_in();
if (out.offset)
pos_out();





if ((ddflags & (C_OF | C_SEEK | C_NOTRUNC)) == (C_OF | C_SEEK) &&
out.flags & ISTRUNC)
if (ftruncate(out.fd, out.offset * out.dbsz) == -1)
err(1, "truncating %s", out.name);

if (ddflags & (C_LCASE | C_UCASE | C_ASCII | C_EBCDIC | C_PARITY)) {
if (ctab != NULL) {
for (cnt = 0; cnt <= 0377; ++cnt)
casetab[cnt] = ctab[cnt];
} else {
for (cnt = 0; cnt <= 0377; ++cnt)
casetab[cnt] = cnt;
}
if ((ddflags & C_PARITY) && !(ddflags & C_ASCII)) {




for (cnt = 200; cnt <= 0377; ++cnt)
casetab[cnt] = casetab[cnt & 0x7f];
}
if (ddflags & C_LCASE) {
for (cnt = 0; cnt <= 0377; ++cnt)
casetab[cnt] = tolower(casetab[cnt]);
} else if (ddflags & C_UCASE) {
for (cnt = 0; cnt <= 0377; ++cnt)
casetab[cnt] = toupper(casetab[cnt]);
}
if ((ddflags & C_PARITY)) {




for (cnt = 0; cnt <= 0377; ++cnt)
casetab[cnt] = casetab[cnt] & 0x7f;
}
if ((ddflags & C_PARSET)) {
for (cnt = 0; cnt <= 0377; ++cnt)
casetab[cnt] = casetab[cnt] | 0x80;
}
if ((ddflags & C_PAREVEN)) {
for (cnt = 0; cnt <= 0377; ++cnt)
if (parity(casetab[cnt]))
casetab[cnt] = casetab[cnt] | 0x80;
}
if ((ddflags & C_PARODD)) {
for (cnt = 0; cnt <= 0377; ++cnt)
if (!parity(casetab[cnt]))
casetab[cnt] = casetab[cnt] | 0x80;
}

ctab = casetab;
}

if (clock_gettime(CLOCK_MONOTONIC, &st.start))
err(1, "clock_gettime");
}

static void
getfdtype(IO *io)
{
struct stat sb;
int type;

if (fstat(io->fd, &sb) == -1)
err(1, "%s", io->name);
if (S_ISREG(sb.st_mode))
io->flags |= ISTRUNC;
if (S_ISCHR(sb.st_mode) || S_ISBLK(sb.st_mode)) {
if (ioctl(io->fd, FIODTYPE, &type) == -1) {
err(1, "%s", io->name);
} else {
if (type & D_TAPE)
io->flags |= ISTAPE;
else if (type & (D_DISK | D_MEM))
io->flags |= ISSEEK;
if (S_ISCHR(sb.st_mode) && (type & D_TAPE) == 0)
io->flags |= ISCHR;
}
return;
}
errno = 0;
if (lseek(io->fd, (off_t)0, SEEK_CUR) == -1 && errno == ESPIPE)
io->flags |= ISPIPE;
else
io->flags |= ISSEEK;
}







static void
speed_limit(void)
{
static double t_prev, t_usleep;
double t_now, t_io, t_target;

t_now = secs_elapsed();
t_io = t_now - t_prev - t_usleep;
t_target = (double)in.dbsz / (double)speed;
t_usleep = t_target - t_io;
if (t_usleep > 0)
usleep(t_usleep * 1000000);
else
t_usleep = 0;
t_prev = t_now;
}

static void
swapbytes(void *v, size_t len)
{
unsigned char *p = v;
unsigned char t;

while (len > 1) {
t = p[0];
p[0] = p[1];
p[1] = t;
p += 2;
len -= 2;
}
}

static void
dd_in(void)
{
ssize_t n;

for (;;) {
switch (cpy_cnt) {
case -1:
return;
case 0:
break;
default:
if (st.in_full + st.in_part >= (uintmax_t)cpy_cnt)
return;
break;
}

if (speed > 0)
speed_limit();





if (ddflags & C_SYNC) {
if (ddflags & C_FILL)
memset(in.dbp, fill_char, in.dbsz);
else if (ddflags & (C_BLOCK | C_UNBLOCK))
memset(in.dbp, ' ', in.dbsz);
else
memset(in.dbp, 0, in.dbsz);
}

in.dbrcnt = 0;
fill:
n = read(in.fd, in.dbp + in.dbrcnt, in.dbsz - in.dbrcnt);


if (n == 0 && in.dbrcnt == 0)
return;


if (n == -1) {




if (!(ddflags & C_NOERROR))
err(1, "%s", in.name);
warn("%s", in.name);
summary();







if (in.flags & ISSEEK &&
lseek(in.fd, (off_t)in.dbsz, SEEK_CUR))
warn("%s", in.name);


if (!(ddflags & C_SYNC))
continue;
}


if (ddflags & C_SYNC)
n = in.dbsz;


in.dbrcnt += n;


if (in.dbrcnt == in.dbsz)
++st.in_full;
else if (ddflags & C_IFULLBLOCK && n != 0)
goto fill;
else
++st.in_part;


in.dbcnt += in.dbrcnt;






if ((ddflags & ~(C_NOERROR | C_NOTRUNC | C_SYNC)) == C_BS) {
out.dbcnt = in.dbcnt;
dd_out(1);
in.dbcnt = 0;
continue;
}

if (ddflags & C_SWAB) {
if ((n = in.dbrcnt) & 1) {
++st.swab;
--n;
}
swapbytes(in.dbp, (size_t)n);
}


in.dbp += in.dbrcnt;
(*cfunc)();
if (need_summary)
summary();
if (need_progress)
progress();
}
}





static void
dd_close(void)
{
if (cfunc == def)
def_close();
else if (cfunc == block)
block_close();
else if (cfunc == unblock)
unblock_close();
if (ddflags & C_OSYNC && out.dbcnt && out.dbcnt < out.dbsz) {
if (ddflags & C_FILL)
memset(out.dbp, fill_char, out.dbsz - out.dbcnt);
else if (ddflags & (C_BLOCK | C_UNBLOCK))
memset(out.dbp, ' ', out.dbsz - out.dbcnt);
else
memset(out.dbp, 0, out.dbsz - out.dbcnt);
out.dbcnt = out.dbsz;
}
if (out.dbcnt || pending)
dd_out(1);





if (out.seek_offset > 0 && (out.flags & ISTRUNC)) {
if (ftruncate(out.fd, out.seek_offset) == -1)
err(1, "truncating %s", out.name);
}

if (ddflags & C_FSYNC) {
if (fsync(out.fd) == -1)
err(1, "fsyncing %s", out.name);
} else if (ddflags & C_FDATASYNC) {
if (fdatasync(out.fd) == -1)
err(1, "fdatasyncing %s", out.name);
}
}

void
dd_out(int force)
{
u_char *outp;
size_t cnt, n;
ssize_t nw;
static int warned;
int sparse;

















outp = out.db;






for (n = force ? out.dbcnt : out.dbsz;; n = out.dbsz) {
cnt = n;
do {
sparse = 0;
if (ddflags & C_SPARSE) {

sparse = BISZERO(outp, cnt);
}
if (sparse && !force) {
pending += cnt;
nw = cnt;
} else {
if (pending != 0) {






out.seek_offset = lseek(out.fd, pending, SEEK_CUR);
if (out.seek_offset == -1)
err(2, "%s: seek error creating sparse file",
out.name);
pending = 0;
}
if (cnt) {
nw = write(out.fd, outp, cnt);
out.seek_offset = 0;
} else {
return;
}
}

if (nw <= 0) {
if (nw == 0)
errx(1, "%s: end of device", out.name);
if (errno != EINTR)
err(1, "%s", out.name);
nw = 0;
}

outp += nw;
st.bytes += nw;

if ((size_t)nw == n && n == (size_t)out.dbsz)
++st.out_full;
else
++st.out_part;

if ((size_t) nw != cnt) {
if (out.flags & ISTAPE)
errx(1, "%s: short write on tape device",
out.name);
if (out.flags & ISCHR && !warned) {
warned = 1;
warnx("%s: short write on character device",
out.name);
}
}

cnt -= nw;
} while (cnt != 0);

if ((out.dbcnt -= n) < out.dbsz)
break;
}


if (out.dbcnt)
(void)memmove(out.db, out.dbp - out.dbcnt, out.dbcnt);
out.dbp = out.db + out.dbcnt;
}
