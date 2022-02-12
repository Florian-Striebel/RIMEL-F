


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)ar_subs.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "pax.h"
#include "extern.h"

static void wr_archive(ARCHD *, int is_app);
static int get_arc(void);
static int next_head(ARCHD *);






static char hdbuf[BLKMULT];
u_long flcnt;







void
list(void)
{
ARCHD *arcn;
int res;
ARCHD archd;
time_t now;

arcn = &archd;







if ((get_arc() < 0) || ((*frmt->options)() < 0) ||
((*frmt->st_rd)() < 0))
return;

if (vflag && ((uidtb_start() < 0) || (gidtb_start() < 0)))
return;

now = time(NULL);




while (next_head(arcn) == 0) {




if ((res = pat_match(arcn)) < 0)
break;

if ((res == 0) && (sel_chk(arcn) == 0)) {



if (pat_sel(arcn) < 0)
break;





if ((res = mod_name(arcn)) < 0)
break;
if (res == 0)
ls_list(arcn, now, stdout);
}





if (rd_skip(arcn->skip + arcn->pad) == 1)
break;
}





(void)(*frmt->end_rd)();
(void)sigprocmask(SIG_BLOCK, &s_mask, NULL);
ar_close();
pat_chk();
}







void
extract(void)
{
ARCHD *arcn;
int res;
off_t cnt;
ARCHD archd;
struct stat sb;
int fd;
time_t now;

arcn = &archd;





if ((get_arc() < 0) || ((*frmt->options)() < 0) ||
((*frmt->st_rd)() < 0) || (dir_start() < 0))
return;





if (iflag && (name_start() < 0))
return;

now = time(NULL);





while (next_head(arcn) == 0) {





if ((res = pat_match(arcn)) < 0)
break;

if ((res > 0) || (sel_chk(arcn) != 0)) {




(void)rd_skip(arcn->skip + arcn->pad);
continue;
}











if ((uflag || Dflag) && ((lstat(arcn->name, &sb) == 0))) {
if (uflag && Dflag) {
if ((arcn->sb.st_mtime <= sb.st_mtime) &&
(arcn->sb.st_ctime <= sb.st_ctime)) {
(void)rd_skip(arcn->skip + arcn->pad);
continue;
}
} else if (Dflag) {
if (arcn->sb.st_ctime <= sb.st_ctime) {
(void)rd_skip(arcn->skip + arcn->pad);
continue;
}
} else if (arcn->sb.st_mtime <= sb.st_mtime) {
(void)rd_skip(arcn->skip + arcn->pad);
continue;
}
}




if ((pat_sel(arcn) < 0) || ((res = mod_name(arcn)) < 0))
break;
if (res > 0) {



purg_lnk(arcn);
(void)rd_skip(arcn->skip + arcn->pad);
continue;
}





if ((Yflag || Zflag) && ((lstat(arcn->name, &sb) == 0))) {
if (Yflag && Zflag) {
if ((arcn->sb.st_mtime <= sb.st_mtime) &&
(arcn->sb.st_ctime <= sb.st_ctime)) {
(void)rd_skip(arcn->skip + arcn->pad);
continue;
}
} else if (Yflag) {
if (arcn->sb.st_ctime <= sb.st_ctime) {
(void)rd_skip(arcn->skip + arcn->pad);
continue;
}
} else if (arcn->sb.st_mtime <= sb.st_mtime) {
(void)rd_skip(arcn->skip + arcn->pad);
continue;
}
}

if (vflag) {
if (vflag > 1)
ls_list(arcn, now, listf);
else {
(void)fputs(arcn->name, listf);
vfpart = 1;
}
}




if ((arcn->pat != NULL) && (arcn->pat->chdname != NULL))
if (chdir(arcn->pat->chdname) != 0)
syswarn(1, errno, "Cannot chdir to %s",
arcn->pat->chdname);




if ((arcn->type != PAX_REG) && (arcn->type != PAX_CTG)) {





if ((arcn->type == PAX_HLK) || (arcn->type == PAX_HRG))
res = lnk_creat(arcn);
else
res = node_creat(arcn);

(void)rd_skip(arcn->skip + arcn->pad);
if (res < 0)
purg_lnk(arcn);

if (vflag && vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}
continue;
}




if ((fd = file_creat(arcn)) < 0) {
(void)rd_skip(arcn->skip + arcn->pad);
purg_lnk(arcn);
continue;
}




res = (*frmt->rd_data)(arcn, fd, &cnt);
file_close(arcn, fd);
if (vflag && vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}
if (!res)
(void)rd_skip(cnt + arcn->pad);




if ((arcn->pat != NULL) && (arcn->pat->chdname != NULL))
if (fchdir(cwdfd) != 0)
syswarn(1, errno,
"Can't fchdir to starting directory");
}






(void)(*frmt->end_rd)();
(void)sigprocmask(SIG_BLOCK, &s_mask, NULL);
ar_close();
proc_dir();
pat_chk();
}







static void
wr_archive(ARCHD *arcn, int is_app)
{
int res;
int hlk;
int wr_one;
off_t cnt;
int (*wrf)(ARCHD *);
int fd = -1;
time_t now;





if (((hlk = frmt->hlk) == 1) && (lnk_start() < 0))
return;




if ((ftree_start() < 0) || ((*frmt->st_wr)() < 0))
return;
wrf = frmt->wr;





if (iflag && (name_start() < 0))
return;




wr_one = is_app;

now = time(NULL);




while (next_file(arcn) == 0) {



if (sel_chk(arcn) != 0) {
ftree_notsel();
continue;
}
fd = -1;
if (uflag) {




if ((res = chk_ftime(arcn)) < 0)
break;
if (res > 0)
continue;
}





ftree_sel(arcn);
if (hlk && (chk_lnk(arcn) < 0))
break;

if ((arcn->type == PAX_REG) || (arcn->type == PAX_HRG) ||
(arcn->type == PAX_CTG)) {






if ((fd = open(arcn->org_name, O_RDONLY, 0)) < 0) {
syswarn(1,errno, "Unable to open %s to read",
arcn->org_name);
purg_lnk(arcn);
continue;
}
}




if ((res = mod_name(arcn)) < 0) {




rdfile_close(arcn, &fd);
purg_lnk(arcn);
break;
}

if ((res > 0) || (docrc && (set_crc(arcn, fd) < 0))) {




rdfile_close(arcn, &fd);
purg_lnk(arcn);
continue;
}

if (vflag) {
if (vflag > 1)
ls_list(arcn, now, listf);
else {
(void)fputs(arcn->name, listf);
vfpart = 1;
}
}
++flcnt;





if ((res = (*wrf)(arcn)) < 0) {
rdfile_close(arcn, &fd);
break;
}
wr_one = 1;
if (res > 0) {




if (vflag && vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}
rdfile_close(arcn, &fd);
continue;
}









res = (*frmt->wr_data)(arcn, fd, &cnt);
rdfile_close(arcn, &fd);
if (vflag && vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}
if (res < 0)
break;




if (((cnt > 0) && (wr_skip(cnt) < 0)) ||
((arcn->pad > 0) && (wr_skip(arcn->pad) < 0)))
break;
}







if (wr_one) {
(*frmt->end_wr)();
wr_fin();
}
(void)sigprocmask(SIG_BLOCK, &s_mask, NULL);
ar_close();
if (tflag)
proc_dir();
ftree_chk();
}























void
append(void)
{
ARCHD *arcn;
int res;
ARCHD archd;
FSUB *orgfrmt;
int udev;
off_t tlen;

arcn = &archd;
orgfrmt = frmt;





if (get_arc() < 0)
return;
if ((orgfrmt != NULL) && (orgfrmt != frmt)) {
paxwarn(1, "Cannot mix current archive format %s with %s",
frmt->name, orgfrmt->name);
return;
}




if (((*frmt->options)() < 0) || ((*frmt->st_rd)() < 0))
return;





if (uflag && (ftime_start() < 0))
return;















if ((udev = frmt->udev) && (dev_start() < 0))
return;




if (vflag) {
(void)fprintf(listf,
"%s: Reading archive to position at the end...", argv0);
vfpart = 1;
}




while (next_head(arcn) == 0) {



if (sel_chk(arcn) != 0) {
if (rd_skip(arcn->skip + arcn->pad) == 1)
break;
continue;
}

if (uflag) {




if ((res = chk_ftime(arcn)) < 0)
break;
if (res > 0) {
if (rd_skip(arcn->skip + arcn->pad) == 1)
break;
continue;
}
}







if ((udev && (add_dev(arcn) < 0)) ||
(rd_skip(arcn->skip + arcn->pad) == 1))
break;
}






tlen = (*frmt->end_rd)();
lnk_end();





if (appnd_start(tlen) < 0)
return;




if (vflag && vfpart) {
(void)fputs("done.\n", listf);
vfpart = 0;
}




wr_archive(arcn, 1);
}






void
archive(void)
{
ARCHD archd;






if ((uflag && (ftime_start() < 0)) || (wr_start() < 0))
return;
if ((*frmt->options)() < 0)
return;

wr_archive(&archd, 0);
}









void
copy(void)
{
ARCHD *arcn;
int res;
int fddest;
char *dest_pt;
int dlen;
int drem;
int fdsrc = -1;
struct stat sb;
ARCHD archd;
char dirbuf[PAXPATHLEN+1];

arcn = &archd;




dlen = l_strncpy(dirbuf, dirptr, sizeof(dirbuf) - 1);
dest_pt = dirbuf + dlen;
if (*(dest_pt-1) != '/') {
*dest_pt++ = '/';
++dlen;
}
*dest_pt = '\0';
drem = PAXPATHLEN - dlen;

if (stat(dirptr, &sb) < 0) {
syswarn(1, errno, "Cannot access destination directory %s",
dirptr);
return;
}
if (!S_ISDIR(sb.st_mode)) {
paxwarn(1, "Destination is not a directory %s", dirptr);
return;
}





if ((lnk_start() < 0) || (ftree_start() < 0) || (dir_start() < 0))
return;





if (iflag && (name_start() < 0))
return;




cp_start();




while (next_file(arcn) == 0) {
fdsrc = -1;




if (sel_chk(arcn) != 0) {
ftree_notsel();
continue;
}











if (uflag || Dflag) {



if (*(arcn->name) == '/')
res = 1;
else
res = 0;
if ((arcn->nlen - res) > drem) {
paxwarn(1, "Destination pathname too long %s",
arcn->name);
continue;
}
(void)strncpy(dest_pt, arcn->name + res, drem);
dirbuf[PAXPATHLEN] = '\0';




res = lstat(dirbuf, &sb);
*dest_pt = '\0';

if (res == 0) {
if (uflag && Dflag) {
if ((arcn->sb.st_mtime<=sb.st_mtime) &&
(arcn->sb.st_ctime<=sb.st_ctime))
continue;
} else if (Dflag) {
if (arcn->sb.st_ctime <= sb.st_ctime)
continue;
} else if (arcn->sb.st_mtime <= sb.st_mtime)
continue;
}
}






ftree_sel(arcn);
if ((chk_lnk(arcn) < 0) || ((res = mod_name(arcn)) < 0))
break;
if ((res > 0) || (set_dest(arcn, dirbuf, dlen) < 0)) {



purg_lnk(arcn);
continue;
}





if ((Yflag || Zflag) && ((lstat(arcn->name, &sb) == 0))) {
if (Yflag && Zflag) {
if ((arcn->sb.st_mtime <= sb.st_mtime) &&
(arcn->sb.st_ctime <= sb.st_ctime))
continue;
} else if (Yflag) {
if (arcn->sb.st_ctime <= sb.st_ctime)
continue;
} else if (arcn->sb.st_mtime <= sb.st_mtime)
continue;
}

if (vflag) {
(void)fputs(arcn->name, listf);
vfpart = 1;
}
++flcnt;





if (lflag)
res = cross_lnk(arcn);
else
res = chk_same(arcn);
if (res <= 0) {
if (vflag && vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}
continue;
}




if ((arcn->type != PAX_REG) && (arcn->type != PAX_CTG)) {



if ((arcn->type == PAX_HLK) || (arcn->type == PAX_HRG))
res = lnk_creat(arcn);
else
res = node_creat(arcn);
if (res < 0)
purg_lnk(arcn);
if (vflag && vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}
continue;
}





if ((fdsrc = open(arcn->org_name, O_RDONLY, 0)) < 0) {
syswarn(1, errno, "Unable to open %s to read",
arcn->org_name);
purg_lnk(arcn);
continue;
}
if ((fddest = file_creat(arcn)) < 0) {
rdfile_close(arcn, &fdsrc);
purg_lnk(arcn);
continue;
}




cp_file(arcn, fdsrc, fddest);
file_close(arcn, fddest);
rdfile_close(arcn, &fdsrc);

if (vflag && vfpart) {
(void)putc('\n', listf);
vfpart = 0;
}
}






(void)sigprocmask(SIG_BLOCK, &s_mask, NULL);
ar_close();
proc_dir();
ftree_chk();
}




















static int
next_head(ARCHD *arcn)
{
int ret;
char *hdend;
int res;
int shftsz;
int hsz;
int in_resync = 0;
int cnt = 0;
int first = 1;





res = hsz = frmt->hsz;
hdend = hdbuf;
shftsz = hsz - 1;
for(;;) {




for (;;) {
if ((ret = rd_wrbuf(hdend, res)) == res)
break;








if (first && ret == 0)
return(-1);
first = 0;





if ((ret == 0) || (rd_sync() < 0)) {
paxwarn(1,"Premature end of file on archive read");
return(-1);
}
if (!in_resync) {
if (act == APPND) {
paxwarn(1,
"Archive I/O error, cannot continue");
return(-1);
}
paxwarn(1,"Archive I/O error. Trying to recover.");
++in_resync;
}




res = hsz;
hdend = hdbuf;
}













if ((*frmt->rd)(arcn, hdbuf) == 0)
break;

if (!frmt->inhead) {



if ((ret = (*frmt->trail_tar)(hdbuf,in_resync,&cnt)) == 0){



ar_drain();
return(-1);
}

if (ret == 1) {






res = hsz;
hdend = hdbuf;
continue;
}
}









if (!in_resync) {
if (act == APPND) {
paxwarn(1,"Unable to append, archive header flaw");
return(-1);
}
paxwarn(1,"Invalid header, starting valid header search.");
++in_resync;
}
memmove(hdbuf, hdbuf+1, shftsz);
res = 1;
hdend = hdbuf + shftsz;
}





if (frmt->inhead && ((*frmt->trail_cpio)(arcn) == 0)) {



ar_drain();
return(-1);
}

++flcnt;
return(0);
}











static int
get_arc(void)
{
int i;
int hdsz = 0;
int res;
int minhd = BLKMULT;
char *hdend;
int notice = 0;





for (i = 0; ford[i] >= 0; ++i) {
if (fsub[ford[i]].hsz < minhd)
minhd = fsub[ford[i]].hsz;
}
if (rd_start() < 0)
return(-1);
res = BLKMULT;
hdsz = 0;
hdend = hdbuf;
for(;;) {
for (;;) {



i = rd_wrbuf(hdend, res);
if (i > 0)
hdsz += i;
if (hdsz >= minhd)
break;




if ((i == 0) || (rd_sync() < 0))
goto out;







res = BLKMULT;
hdsz = 0;
hdend = hdbuf;
if (!notice) {
if (act == APPND)
return(-1);
paxwarn(1,"Cannot identify format. Searching...");
++notice;
}
}









for (i = 0; ford[i] >= 0; ++i) {
if ((*fsub[ford[i]].id)(hdbuf, hdsz) < 0)
continue;
frmt = &(fsub[ford[i]]);







pback(hdbuf, hdsz);
return(0);
}





if (!notice) {
if (act == APPND)
return(-1);
paxwarn(1, "Cannot identify format. Searching...");
++notice;
}







if (--hdsz > 0) {
memmove(hdbuf, hdbuf+1, hdsz);
res = BLKMULT - hdsz;
hdend = hdbuf + hdsz;
} else {
res = BLKMULT;
hdend = hdbuf;
hdsz = 0;
}
}

out:



paxwarn(1, "Sorry, unable to determine archive format.");
return(-1);
}
