


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)ar_io.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mtio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "pax.h"
#include "options.h"
#include "extern.h"





#define DMOD 0666
#define EXT_MODE O_RDONLY
#define AR_MODE (O_WRONLY | O_CREAT | O_TRUNC)
#define APP_MODE O_RDWR

static char none[] = "<NONE>";
static char stdo[] = "<STDOUT>";
static char stdn[] = "<STDIN>";
static int arfd = -1;
static int artyp = ISREG;
static int arvol = 1;
static int lstrval = -1;
static int io_ok;
static int did_io;
static int done;
static struct stat arsb;
static int invld_rec;
static int wr_trail = 1;
static int can_unlnk = 0;
const char *arcname;
const char *gzip_program;
static pid_t zpid = -1;

static int get_phys(void);
static void ar_start_gzip(int, const char *, int);










int
ar_open(const char *name)
{
struct mtget mb;

if (arfd != -1)
(void)close(arfd);
arfd = -1;
can_unlnk = did_io = io_ok = invld_rec = 0;
artyp = ISREG;
flcnt = 0;




switch (act) {
case LIST:
case EXTRACT:
if (name == NULL) {
arfd = STDIN_FILENO;
arcname = stdn;
} else if ((arfd = open(name, EXT_MODE, DMOD)) < 0)
syswarn(0, errno, "Failed open to read on %s", name);
if (arfd != -1 && gzip_program != NULL)
ar_start_gzip(arfd, gzip_program, 0);
break;
case ARCHIVE:
if (name == NULL) {
arfd = STDOUT_FILENO;
arcname = stdo;
} else if ((arfd = open(name, AR_MODE, DMOD)) < 0)
syswarn(0, errno, "Failed open to write on %s", name);
else
can_unlnk = 1;
if (arfd != -1 && gzip_program != NULL)
ar_start_gzip(arfd, gzip_program, 1);
break;
case APPND:
if (name == NULL) {
arfd = STDOUT_FILENO;
arcname = stdo;
} else if ((arfd = open(name, APP_MODE, DMOD)) < 0)
syswarn(0, errno, "Failed open to read/write on %s",
name);
break;
case COPY:



arcname = none;
lstrval = 1;
return(0);
}
if (arfd < 0)
return(-1);

if (chdname != NULL)
if (chdir(chdname) != 0) {
syswarn(1, errno, "Failed chdir to %s", chdname);
return(-1);
}



if (fstat(arfd, &arsb) < 0) {
syswarn(0, errno, "Failed stat on %s", arcname);
(void)close(arfd);
arfd = -1;
can_unlnk = 0;
return(-1);
}
if (S_ISDIR(arsb.st_mode)) {
paxwarn(0, "Cannot write an archive on top of a directory %s",
arcname);
(void)close(arfd);
arfd = -1;
can_unlnk = 0;
return(-1);
}

if (S_ISCHR(arsb.st_mode))
artyp = ioctl(arfd, MTIOCGET, &mb) ? ISCHR : ISTAPE;
else if (S_ISBLK(arsb.st_mode))
artyp = ISBLK;
else if ((lseek(arfd, (off_t)0L, SEEK_CUR) == -1) && (errno == ESPIPE))
artyp = ISPIPE;
else
artyp = ISREG;





if (artyp != ISREG)
can_unlnk = 0;



if (act == ARCHIVE) {
blksz = rdblksz = wrblksz;
lstrval = 1;
return(0);
}








switch(artyp) {
case ISTAPE:














blksz = rdblksz = MAXBLK;
break;
case ISPIPE:
case ISBLK:
case ISCHR:








if ((act == APPND) && wrblksz) {
blksz = rdblksz = wrblksz;
break;
}

if ((arsb.st_blksize > 0) && (arsb.st_blksize < MAXBLK) &&
((arsb.st_blksize % BLKMULT) == 0))
rdblksz = arsb.st_blksize;
else
rdblksz = DEVBLK;



if ((act == APPND) || (artyp == ISCHR))
blksz = rdblksz;
else
blksz = MAXBLK;
break;
case ISREG:




if ((act == APPND) && wrblksz && ((arsb.st_size%wrblksz)==0)){
blksz = rdblksz = wrblksz;
break;
}



for (rdblksz = MAXBLK; rdblksz > 0; rdblksz -= BLKMULT)
if ((arsb.st_size % rdblksz) == 0)
break;



if (rdblksz <= 0)
rdblksz = FILEBLK;



if (act == APPND)
blksz = rdblksz;
else
blksz = MAXBLK;
break;
default:



blksz = rdblksz = BLKMULT;
break;
}
lstrval = 1;
return(0);
}





void
ar_close(void)
{
int status;

if (arfd < 0) {
did_io = io_ok = flcnt = 0;
return;
}







if (vflag && (artyp == ISTAPE)) {
if (vfpart)
(void)putc('\n', listf);
(void)fprintf(listf,
"%s: Waiting for tape drive close to complete...",
argv0);
(void)fflush(listf);
}





if (can_unlnk && (fstat(arfd, &arsb) == 0) && (S_ISREG(arsb.st_mode)) &&
(arsb.st_size == 0)) {
(void)unlink(arcname);
can_unlnk = 0;
}





if ((act == LIST || act == EXTRACT) && nflag && zpid > 0)
kill(zpid, SIGINT);

(void)close(arfd);


if (zpid > 0)
waitpid(zpid, &status, 0);

if (vflag && (artyp == ISTAPE)) {
(void)fputs("done.\n", listf);
vfpart = 0;
(void)fflush(listf);
}
arfd = -1;

if (!io_ok && !did_io) {
flcnt = 0;
return;
}
did_io = io_ok = 0;





if (frmt != NULL)
++arvol;

if (!vflag) {
flcnt = 0;
return;
}




if (vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}






if (frmt == NULL) {
#if defined(NET2_STAT)
(void)fprintf(listf, "%s: unknown format, %lu bytes skipped.\n",
argv0, rdcnt);
#else
(void)fprintf(listf, "%s: unknown format, %ju bytes skipped.\n",
argv0, (uintmax_t)rdcnt);
#endif
(void)fflush(listf);
flcnt = 0;
return;
}

if (strcmp(NM_CPIO, argv0) == 0)
(void)fprintf(listf, "%llu blocks\n",
(unsigned long long)((rdcnt ? rdcnt : wrcnt) / 5120));
else if (strcmp(NM_TAR, argv0) != 0)
(void)fprintf(listf,
#if defined(NET2_STAT)
"%s: %s vol %d, %lu files, %lu bytes read, %lu bytes written.\n",
argv0, frmt->name, arvol-1, flcnt, rdcnt, wrcnt);
#else
"%s: %s vol %d, %ju files, %ju bytes read, %ju bytes written.\n",
argv0, frmt->name, arvol-1, (uintmax_t)flcnt,
(uintmax_t)rdcnt, (uintmax_t)wrcnt);
#endif
(void)fflush(listf);
flcnt = 0;
}








void
ar_drain(void)
{
int res;
char drbuf[MAXBLK];






if ((artyp != ISPIPE) || (lstrval <= 0))
return;




while ((res = read(arfd, drbuf, sizeof(drbuf))) > 0)
;
lstrval = res;
}











int
ar_set_wr(void)
{
off_t cpos;





wr_trail = 0;




if (artyp != ISREG)
return(0);





if (((cpos = lseek(arfd, (off_t)0L, SEEK_CUR)) < 0) ||
(ftruncate(arfd, cpos) < 0)) {
syswarn(1, errno, "Unable to truncate archive file");
return(-1);
}
return(0);
}










int
ar_app_ok(void)
{
if (artyp == ISPIPE) {
paxwarn(1, "Cannot append to an archive obtained from a pipe.");
return(-1);
}

if (!invld_rec)
return(0);
paxwarn(1,"Cannot append, device record size %d does not support %s spec",
rdblksz, argv0);
return(-1);
}










int
ar_read(char *buf, int cnt)
{
int res = 0;




if (lstrval <= 0)
return(lstrval);




switch (artyp) {
case ISTAPE:
if ((res = read(arfd, buf, cnt)) > 0) {







io_ok = 1;
if (res != rdblksz) {









rdblksz = res;
if (rdblksz % BLKMULT)
invld_rec = 1;
}
return(res);
}
break;
case ISREG:
case ISBLK:
case ISCHR:
case ISPIPE:
default:







if ((res = read(arfd, buf, cnt)) > 0) {
io_ok = 1;
return(res);
}
break;
}




lstrval = res;
if (res < 0)
syswarn(1, errno, "Failed read on archive volume %d", arvol);
else
paxwarn(0, "End of archive volume %d reached", arvol);
return(res);
}












int
ar_write(char *buf, int bsz)
{
int res;
off_t cpos;





if (lstrval <= 0)
return(lstrval);

if ((res = write(arfd, buf, bsz)) == bsz) {
wr_trail = 1;
io_ok = 1;
return(bsz);
}




if (res < 0)
lstrval = res;
else
lstrval = 0;

switch (artyp) {
case ISREG:
if ((res > 0) && (res % BLKMULT)) {





if ((cpos = lseek(arfd, (off_t)0L, SEEK_CUR)) < 0)
break;
cpos -= (off_t)res;
if (ftruncate(arfd, cpos) < 0)
break;
res = lstrval = 0;
break;
}
if (res >= 0)
break;



if ((errno == ENOSPC) || (errno == EFBIG) || (errno == EDQUOT))
res = lstrval = 0;
break;
case ISTAPE:
case ISCHR:
case ISBLK:
if (res >= 0)
break;
if (errno == EACCES) {
paxwarn(0, "Write failed, archive is write protected.");
res = lstrval = 0;
return(0);
}




if ((errno == ENOSPC) || (errno == EIO) || (errno == ENXIO))
res = lstrval = 0;
break;
case ISPIPE:
default:



break;
}









if (res >= 0) {
if (res > 0)
wr_trail = 1;
io_ok = 1;
}





if (!wr_trail && (res <= 0)) {
paxwarn(1,"Unable to append, trailer re-write failed. Quitting.");
return(res);
}

if (res == 0)
paxwarn(0, "End of archive volume %d reached", arvol);
else if (res < 0)
syswarn(1, errno, "Failed write to archive volume: %d", arvol);
else if (!frmt->blkalgn || ((res % frmt->blkalgn) == 0))
paxwarn(0,"WARNING: partial archive write. Archive MAY BE FLAWED");
else
paxwarn(1,"WARNING: partial archive write. Archive IS FLAWED");
return(res);
}









int
ar_rdsync(void)
{
long fsbz;
off_t cpos;
off_t mpos;
struct mtop mb;






if ((done > 0) || (lstrval == 0))
return(-1);

if ((act == APPND) || (act == ARCHIVE)) {
paxwarn(1, "Cannot allow updates to an archive with flaws.");
return(-1);
}
if (io_ok)
did_io = 1;

switch(artyp) {
case ISTAPE:








if (io_ok) {
io_ok = 0;
lstrval = 1;
break;
}
mb.mt_op = MTFSR;
mb.mt_count = 1;
if (ioctl(arfd, MTIOCTOP, &mb) < 0)
break;
lstrval = 1;
break;
case ISREG:
case ISCHR:
case ISBLK:



io_ok = 0;
if (((fsbz = arsb.st_blksize) <= 0) || (artyp != ISREG))
fsbz = BLKMULT;
if ((cpos = lseek(arfd, (off_t)0L, SEEK_CUR)) < 0)
break;
mpos = fsbz - (cpos % (off_t)fsbz);
if (lseek(arfd, mpos, SEEK_CUR) < 0)
break;
lstrval = 1;
break;
case ISPIPE:
default:



io_ok = 0;
break;
}
if (lstrval <= 0) {
paxwarn(1, "Unable to recover from an archive read failure.");
return(-1);
}
paxwarn(0, "Attempting to recover from an archive read failure.");
return(0);
}











int
ar_fow(off_t sksz, off_t *skipped)
{
off_t cpos;
off_t mpos;

*skipped = 0;
if (sksz <= 0)
return(0);




if (lstrval <= 0)
return(lstrval);







if (artyp != ISREG)
return(0);




if ((cpos = lseek(arfd, (off_t)0L, SEEK_CUR)) >= 0) {






if ((mpos = cpos + sksz) > arsb.st_size) {
*skipped = arsb.st_size - cpos;
mpos = arsb.st_size;
} else
*skipped = sksz;
if (lseek(arfd, mpos, SEEK_SET) >= 0)
return(0);
}
syswarn(1, errno, "Forward positioning operation on archive failed");
lstrval = -1;
return(-1);
}












int
ar_rev(off_t sksz)
{
off_t cpos;
struct mtop mb;
int phyblk;




if (lstrval < 0)
return(lstrval);

switch(artyp) {
case ISPIPE:
if (sksz <= 0)
break;



paxwarn(1, "Reverse positioning on pipes is not supported.");
lstrval = -1;
return(-1);
case ISREG:
case ISBLK:
case ISCHR:
default:
if (sksz <= 0)
break;








if ((cpos = lseek(arfd, (off_t)0L, SEEK_CUR)) < 0) {
syswarn(1, errno,
"Unable to obtain current archive byte offset");
lstrval = -1;
return(-1);
}








if ((cpos -= sksz) < (off_t)0L) {
if (arvol > 1) {



paxwarn(1,"Reverse position on previous volume.");
lstrval = -1;
return(-1);
}
cpos = (off_t)0L;
}
if (lseek(arfd, cpos, SEEK_SET) < 0) {
syswarn(1, errno, "Unable to seek archive backwards");
lstrval = -1;
return(-1);
}
break;
case ISTAPE:







if ((phyblk = get_phys()) <= 0) {
lstrval = -1;
return(-1);
}





rdblksz = phyblk;





if (sksz <= 0)
break;




if (sksz % phyblk) {
paxwarn(1,
"Tape drive unable to backspace requested amount");
lstrval = -1;
return(-1);
}




mb.mt_op = MTBSR;
mb.mt_count = sksz/phyblk;
if (ioctl(arfd, MTIOCTOP, &mb) < 0) {
syswarn(1,errno, "Unable to backspace tape %d blocks.",
mb.mt_count);
lstrval = -1;
return(-1);
}
break;
}
lstrval = 1;
return(0);
}












static int
get_phys(void)
{
int padsz = 0;
int res;
int phyblk;
struct mtop mb;
char scbuf[MAXBLK];





if (lstrval == 1) {




while ((res = read(arfd, scbuf, sizeof(scbuf))) > 0)
padsz += res;
if (res < 0) {
syswarn(1, errno, "Unable to locate tape filemark.");
return(-1);
}
}





mb.mt_op = MTBSF;
mb.mt_count = 1;
if (ioctl(arfd, MTIOCTOP, &mb) < 0) {
syswarn(1, errno, "Unable to backspace over tape filemark.");
return(-1);
}





mb.mt_op = MTBSR;
mb.mt_count = 1;
if (ioctl(arfd, MTIOCTOP, &mb) < 0) {
syswarn(1, errno, "Unable to backspace over last tape block.");
return(-1);
}
if ((phyblk = read(arfd, scbuf, sizeof(scbuf))) <= 0) {
syswarn(1, errno, "Cannot determine archive tape blocksize.");
return(-1);
}





while ((res = read(arfd, scbuf, sizeof(scbuf))) > 0)
;
if (res < 0) {
syswarn(1, errno, "Unable to locate tape filemark.");
return(-1);
}
mb.mt_op = MTBSF;
mb.mt_count = 1;
if (ioctl(arfd, MTIOCTOP, &mb) < 0) {
syswarn(1, errno, "Unable to backspace over tape filemark.");
return(-1);
}




lstrval = 1;




if (padsz == 0)
return(phyblk);





if (padsz % phyblk) {
paxwarn(1, "Tape drive unable to backspace requested amount");
return(-1);
}





mb.mt_op = MTBSR;
mb.mt_count = padsz/phyblk;
if (ioctl(arfd, MTIOCTOP, &mb) < 0) {
syswarn(1,errno,"Unable to backspace tape over %d pad blocks",
mb.mt_count);
return(-1);
}
return(phyblk);
}











int
ar_next(void)
{
static char *arcbuf;
char buf[PAXPATHLEN+2];
sigset_t o_mask;






if (sigprocmask(SIG_BLOCK, &s_mask, &o_mask) < 0)
syswarn(0, errno, "Unable to set signal mask");
ar_close();
if (sigprocmask(SIG_SETMASK, &o_mask, NULL) < 0)
syswarn(0, errno, "Unable to restore signal mask");

if (done || !wr_trail || Oflag || strcmp(NM_TAR, argv0) == 0)
return(-1);

tty_prnt("\nATTENTION! %s archive volume change required.\n", argv0);





if (strcmp(arcname, stdo) && strcmp(arcname, stdn) && (artyp != ISREG)
&& (artyp != ISPIPE)) {
if (artyp == ISTAPE) {
tty_prnt("%s ready for archive tape volume: %d\n",
arcname, arvol);
tty_prnt("Load the NEXT TAPE on the tape drive");
} else {
tty_prnt("%s ready for archive volume: %d\n",
arcname, arvol);
tty_prnt("Load the NEXT STORAGE MEDIA (if required)");
}

if ((act == ARCHIVE) || (act == APPND))
tty_prnt(" and make sure it is WRITE ENABLED.\n");
else
tty_prnt("\n");

for(;;) {
tty_prnt("Type \"y\" to continue, \".\" to quit %s,",
argv0);
tty_prnt(" or \"s\" to switch to new device.\nIf you");
tty_prnt(" cannot change storage media, type \"s\"\n");
tty_prnt("Is the device ready and online? > ");

if ((tty_read(buf,sizeof(buf))<0) || !strcmp(buf,".")){
done = 1;
lstrval = -1;
tty_prnt("Quitting %s!\n", argv0);
vfpart = 0;
return(-1);
}

if ((buf[0] == '\0') || (buf[1] != '\0')) {
tty_prnt("%s unknown command, try again\n",buf);
continue;
}

switch (buf[0]) {
case 'y':
case 'Y':



if (ar_open(arcname) >= 0)
return(0);
tty_prnt("Cannot re-open %s, try again\n",
arcname);
continue;
case 's':
case 'S':



tty_prnt("Switching to a different archive\n");
break;
default:
tty_prnt("%s unknown command, try again\n",buf);
continue;
}
break;
}
} else
tty_prnt("Ready for archive volume: %d\n", arvol);




for (;;) {
tty_prnt("Input archive name or \".\" to quit %s.\n", argv0);
tty_prnt("Archive name > ");

if ((tty_read(buf, sizeof(buf)) < 0) || !strcmp(buf, ".")) {
done = 1;
lstrval = -1;
tty_prnt("Quitting %s!\n", argv0);
vfpart = 0;
return(-1);
}
if (buf[0] == '\0') {
tty_prnt("Empty file name, try again\n");
continue;
}
if (!strcmp(buf, "..")) {
tty_prnt("Illegal file name: .. try again\n");
continue;
}
if (strlen(buf) > PAXPATHLEN) {
tty_prnt("File name too long, try again\n");
continue;
}




if (ar_open(buf) >= 0) {
free(arcbuf);
if ((arcbuf = strdup(buf)) == NULL) {
done = 1;
lstrval = -1;
paxwarn(0, "Cannot save archive name.");
return(-1);
}
arcname = arcbuf;
break;
}
tty_prnt("Cannot open %s, try again\n", buf);
continue;
}
return(0);
}






void
ar_start_gzip(int fd, const char *gzip_prog, int wr)
{
int fds[2];
const char *gzip_flags;

if (pipe(fds) < 0)
err(1, "could not pipe");
zpid = fork();
if (zpid < 0)
err(1, "could not fork");


if (zpid) {
if (wr)
dup2(fds[1], fd);
else
dup2(fds[0], fd);
close(fds[0]);
close(fds[1]);
} else {
if (wr) {
dup2(fds[0], STDIN_FILENO);
dup2(fd, STDOUT_FILENO);
gzip_flags = "-c";
} else {
dup2(fds[1], STDOUT_FILENO);
dup2(fd, STDIN_FILENO);
gzip_flags = "-dc";
}
close(fds[0]);
close(fds[1]);
if (execlp(gzip_prog, gzip_prog, gzip_flags,
(char *)NULL) < 0)
err(1, "could not exec");

}
}
