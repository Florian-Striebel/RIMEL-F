


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)buf_subs.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "pax.h"
#include "extern.h"





#define MINFBSZ 512
#define MAXFLT 10






static char bufmem[MAXBLK+BLKMULT];
static char *buf;
static char *bufend;
static char *bufpt;
int blksz = MAXBLK;
int wrblksz;
int maxflt = MAXFLT;
int rdblksz;
off_t wrlimit;
off_t wrcnt;
off_t rdcnt;








int
wr_start(void)
{
buf = &(bufmem[BLKMULT]);







if (!wrblksz)
wrblksz = frmt->bsz;
if (wrblksz > MAXBLK) {
paxwarn(1, "Write block size of %d too large, maximum is: %d",
wrblksz, MAXBLK);
return(-1);
}
if (wrblksz % BLKMULT) {
paxwarn(1, "Write block size of %d is not a %d byte multiple",
wrblksz, BLKMULT);
return(-1);
}
if (wrblksz > MAXBLK_POSIX) {
paxwarn(0, "Write block size of %d larger than POSIX max %d, archive may not be portable",
wrblksz, MAXBLK_POSIX);
return(-1);
}




blksz = rdblksz = wrblksz;
if ((ar_open(arcname) < 0) && (ar_next() < 0))
return(-1);
wrcnt = 0;
bufend = buf + wrblksz;
bufpt = buf;
return(0);
}








int
rd_start(void)
{





buf = &(bufmem[BLKMULT]);
if ((act == APPND) && wrblksz) {
if (wrblksz > MAXBLK) {
paxwarn(1,"Write block size %d too large, maximum is: %d",
wrblksz, MAXBLK);
return(-1);
}
if (wrblksz % BLKMULT) {
paxwarn(1, "Write block size %d is not a %d byte multiple",
wrblksz, BLKMULT);
return(-1);
}
}




if ((ar_open(arcname) < 0) && (ar_next() < 0))
return(-1);
bufend = buf + rdblksz;
bufpt = bufend;
rdcnt = 0;
return(0);
}






void
cp_start(void)
{
buf = &(bufmem[BLKMULT]);
rdblksz = blksz = MAXBLK;
}

































int
appnd_start(off_t skcnt)
{
int res;
off_t cnt;

if (exit_val != 0) {
paxwarn(0, "Cannot append to an archive that may have flaws.");
return(-1);
}





if (!wrblksz)
wrblksz = blksz = rdblksz;
else
blksz = rdblksz;




if (ar_app_ok() < 0)
return(-1);







skcnt += bufend - bufpt;
if ((cnt = (skcnt/blksz) * blksz) < skcnt)
cnt += blksz;
if (ar_rev((off_t)cnt) < 0)
goto out;






if ((cnt -= skcnt) > 0) {






bufpt = buf;
bufend = buf + blksz;
while (bufpt < bufend) {
if ((res = ar_read(bufpt, rdblksz)) <= 0)
goto out;
bufpt += res;
}
if (ar_rev((off_t)(bufpt - buf)) < 0)
goto out;
bufpt = buf + cnt;
bufend = buf + blksz;
} else {



bufend = buf + blksz;
bufpt = buf;
}
rdblksz = blksz;
rdcnt -= skcnt;
wrcnt = 0;







if (ar_set_wr() < 0)
return(-1);
act = ARCHIVE;
return(0);

out:
paxwarn(1, "Unable to rewrite archive trailer, cannot append.");
return(-1);
}












int
rd_sync(void)
{
int errcnt = 0;
int res;




if (maxflt == 0)
return(-1);
if (act == APPND) {
paxwarn(1, "Unable to append when there are archive read errors.");
return(-1);
}




if (ar_rdsync() < 0) {
if (ar_next() < 0)
return(-1);
else
rdcnt = 0;
}

for (;;) {
if ((res = ar_read(buf, blksz)) > 0) {



bufpt = buf;
bufend = buf + res;
rdcnt += res;
return(0);
}









if ((maxflt > 0) && (++errcnt > maxflt))
paxwarn(0,"Archive read error limit (%d) reached",maxflt);
else if (ar_rdsync() == 0)
continue;
if (ar_next() < 0)
break;
rdcnt = 0;
errcnt = 0;
}
return(-1);
}












void
pback(char *pt, int cnt)
{
bufpt -= cnt;
memcpy(bufpt, pt, cnt);
return;
}









int
rd_skip(off_t skcnt)
{
off_t res;
off_t cnt;
off_t skipped = 0;







if (skcnt == 0)
return(0);
res = MIN((bufend - bufpt), skcnt);
bufpt += res;
skcnt -= res;




if (skcnt == 0)
return(0);





res = skcnt%rdblksz;
cnt = (skcnt/rdblksz) * rdblksz;





if (ar_fow(cnt, &skipped) < 0)
return(-1);
res += cnt - skipped;
rdcnt += skipped;





while (res > 0L) {
cnt = bufend - bufpt;



if ((cnt <= 0) && ((cnt = buf_fill()) < 0))
return(-1);
if (cnt == 0)
return(1);
cnt = MIN(cnt, res);
bufpt += cnt;
res -= cnt;
}
return(0);
}









void
wr_fin(void)
{
if (bufpt > buf) {
memset(bufpt, 0, bufend - bufpt);
bufpt = bufend;
(void)buf_flush(blksz);
}
}












int
wr_rdbuf(char *out, int outcnt)
{
int cnt;





while (outcnt > 0) {
cnt = bufend - bufpt;
if ((cnt <= 0) && ((cnt = buf_flush(blksz)) < 0))
return(-1);



cnt = MIN(cnt, outcnt);
memcpy(bufpt, out, cnt);
bufpt += cnt;
out += cnt;
outcnt -= cnt;
}
return(0);
}












int
rd_wrbuf(char *in, int cpcnt)
{
int res;
int cnt;
int incnt = cpcnt;




while (incnt > 0) {
cnt = bufend - bufpt;
if ((cnt <= 0) && ((cnt = buf_fill()) <= 0)) {






if ((res = cpcnt - incnt) > 0)
return(res);
return(cnt);
}





cnt = MIN(cnt, incnt);
memcpy(in, bufpt, cnt);
bufpt += cnt;
incnt -= cnt;
in += cnt;
}
return(cpcnt);
}












int
wr_skip(off_t skcnt)
{
int cnt;




while (skcnt > 0L) {
cnt = bufend - bufpt;
if ((cnt <= 0) && ((cnt = buf_flush(blksz)) < 0))
return(-1);
cnt = MIN(cnt, skcnt);
memset(bufpt, 0, cnt);
bufpt += cnt;
skcnt -= cnt;
}
return(0);
}



















int
wr_rdfile(ARCHD *arcn, int ifd, off_t *left)
{
int cnt;
int res = 0;
off_t size = arcn->sb.st_size;
struct stat sb;




while (size > 0L) {
cnt = bufend - bufpt;
if ((cnt <= 0) && ((cnt = buf_flush(blksz)) < 0)) {
*left = size;
return(-1);
}
cnt = MIN(cnt, size);
if ((res = read(ifd, bufpt, cnt)) <= 0)
break;
size -= res;
bufpt += res;
}





if (res < 0)
syswarn(1, errno, "Read fault on %s", arcn->org_name);
else if (size != 0L)
paxwarn(1, "File changed size during read %s", arcn->org_name);
else if (fstat(ifd, &sb) < 0)
syswarn(1, errno, "Failed stat on %s", arcn->org_name);
else if (arcn->sb.st_mtime != sb.st_mtime)
paxwarn(1, "File %s was modified during copy to archive",
arcn->org_name);
*left = size;
return(0);
}





















int
rd_wrfile(ARCHD *arcn, int ofd, off_t *left)
{
int cnt = 0;
off_t size = arcn->sb.st_size;
int res = 0;
char *fnm = arcn->name;
int isem = 1;
int rem;
int sz = MINFBSZ;
struct stat sb;
u_long crc = 0L;





if (fstat(ofd, &sb) == 0) {
if (sb.st_blksize > 0)
sz = (int)sb.st_blksize;
} else
syswarn(0,errno,"Unable to obtain block size for file %s",fnm);
rem = sz;
*left = 0L;






while (size > 0L) {
cnt = bufend - bufpt;





if ((cnt <= 0) && ((cnt = buf_fill()) <= 0))
break;
cnt = MIN(cnt, size);
if ((res = file_write(ofd,bufpt,cnt,&rem,&isem,sz,fnm)) <= 0) {
*left = size;
break;
}

if (docrc) {



cnt = res;
while (--cnt >= 0)
crc += *bufpt++ & 0xff;
} else
bufpt += res;
size -= res;
}







if (isem && (arcn->sb.st_size > 0L))
file_flush(ofd, fnm, isem);




if ((size > 0L) && (*left == 0L))
return(-1);





if (docrc && (size == 0L) && (arcn->crc != crc))
paxwarn(1,"Actual crc does not match expected crc %s",arcn->name);
return(0);
}








void
cp_file(ARCHD *arcn, int fd1, int fd2)
{
int cnt;
off_t cpcnt = 0L;
int res = 0;
char *fnm = arcn->name;
int no_hole = 0;
int isem = 1;
int rem;
int sz = MINFBSZ;
struct stat sb;





if (((off_t)(arcn->sb.st_blocks * BLKMULT)) >= arcn->sb.st_size)
++no_hole;





if (fstat(fd2, &sb) == 0) {
if (sb.st_blksize > 0)
sz = sb.st_blksize;
} else
syswarn(0,errno,"Unable to obtain block size for file %s",fnm);
rem = sz;




for(;;) {
if ((cnt = read(fd1, buf, blksz)) <= 0)
break;
if (no_hole)
res = write(fd2, buf, cnt);
else
res = file_write(fd2, buf, cnt, &rem, &isem, sz, fnm);
if (res != cnt)
break;
cpcnt += cnt;
}




if (res < 0)
syswarn(1, errno, "Failed write during copy of %s to %s",
arcn->org_name, arcn->name);
else if (cpcnt != arcn->sb.st_size)
paxwarn(1, "File %s changed size during copy to %s",
arcn->org_name, arcn->name);
else if (fstat(fd1, &sb) < 0)
syswarn(1, errno, "Failed stat of %s", arcn->org_name);
else if (arcn->sb.st_mtime != sb.st_mtime)
paxwarn(1, "File %s was modified during copy to %s",
arcn->org_name, arcn->name);







if (!no_hole && isem && (arcn->sb.st_size > 0L))
file_flush(fd2, fnm, isem);
return;
}










int
buf_fill(void)
{
int cnt;
static int fini = 0;

if (fini)
return(0);

for(;;) {




if ((cnt = ar_read(buf, blksz)) > 0) {
bufpt = buf;
bufend = buf + cnt;
rdcnt += cnt;
return(cnt);
}







if (cnt < 0)
break;
if (frmt == NULL || ar_next() < 0) {
fini = 1;
return(0);
}
rdcnt = 0;
}
exit_val = 1;
return(-1);
}










int
buf_flush(int bufcnt)
{
int cnt;
int push = 0;
int totcnt = 0;







if ((wrlimit > 0) && (wrcnt > wrlimit)) {
paxwarn(0, "User specified archive volume byte limit reached.");
if (ar_next() < 0) {
wrcnt = 0;
exit_val = 1;
return(-1);
}
wrcnt = 0;










bufend = buf + blksz;
if (blksz > bufcnt)
return(0);
if (blksz < bufcnt)
push = bufcnt - blksz;
}




for (;;) {



cnt = ar_write(buf, blksz);
if (cnt == blksz) {



wrcnt += cnt;
totcnt += cnt;
if (push > 0) {




memcpy(buf, bufend, push);
bufpt = buf + push;
if (push >= blksz) {
push -= blksz;
continue;
}
} else
bufpt = buf;
return(totcnt);
} else if (cnt > 0) {






totcnt += cnt;
wrcnt += cnt;
bufpt = buf + cnt;
cnt = bufcnt - cnt;
memcpy(buf, bufpt, cnt);
bufpt = buf + cnt;
if (!frmt->blkalgn || ((cnt % frmt->blkalgn) == 0))
return(totcnt);
break;
}




wrcnt = 0;
if (ar_next() < 0)
break;






bufend = buf + blksz;
if (blksz > bufcnt)
return(0);
if (blksz < bufcnt)
push = bufcnt - blksz;
}




exit_val = 1;
return(-1);
}
