


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)file_subs.c 8.1 (Berkeley) 5/31/93";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/uio.h>
#include "pax.h"
#include "options.h"
#include "extern.h"

static int
mk_link(char *,struct stat *,char *, int);






#define FILEBITS (S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)
#define SETBITS (S_ISUID | S_ISGID)
#define ABITS (FILEBITS | SETBITS)








int
file_creat(ARCHD *arcn)
{
int fd = -1;
mode_t file_mode;
int oerrno;












file_mode = arcn->sb.st_mode & FILEBITS;
if ((fd = open(arcn->name, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL,
file_mode)) >= 0)
return(fd);






if (unlnk_exist(arcn->name, arcn->type) != 0)
return(-1);

for (;;) {





if ((fd = open(arcn->name, O_WRONLY | O_CREAT | O_TRUNC,
file_mode)) >= 0)
break;
oerrno = errno;
if (nodirs || chk_path(arcn->name,arcn->sb.st_uid,arcn->sb.st_gid) < 0) {
syswarn(1, oerrno, "Unable to create %s", arcn->name);
return(-1);
}
}
return(fd);
}









void
file_close(ARCHD *arcn, int fd)
{
int res = 0;

if (fd < 0)
return;
if (close(fd) < 0)
syswarn(0, errno, "Unable to close file descriptor on %s",
arcn->name);






if (pids)
res = set_ids(arcn->name, arcn->sb.st_uid, arcn->sb.st_gid);






if (!pmode || res)
arcn->sb.st_mode &= ~(SETBITS);
if (pmode)
set_pmode(arcn->name, arcn->sb.st_mode);
if (patime || pmtime)
set_ftime(arcn->name, arcn->sb.st_mtime, arcn->sb.st_atime, 0);
}









int
lnk_creat(ARCHD *arcn)
{
struct stat sb;





if (lstat(arcn->ln_name, &sb) < 0) {
syswarn(1,errno,"Unable to link to %s from %s", arcn->ln_name,
arcn->name);
return(-1);
}

if (S_ISDIR(sb.st_mode)) {
paxwarn(1, "A hard link to the directory %s is not allowed",
arcn->ln_name);
return(-1);
}

return(mk_link(arcn->ln_name, &sb, arcn->name, 0));
}











int
cross_lnk(ARCHD *arcn)
{





if (arcn->type == PAX_DIR)
return(1);
return(mk_link(arcn->org_name, &(arcn->sb), arcn->name, 1));
}












int
chk_same(ARCHD *arcn)
{
struct stat sb;





if (lstat(arcn->name, &sb) < 0)
return(1);
if (kflag)
return(0);




if ((arcn->sb.st_dev == sb.st_dev) && (arcn->sb.st_ino == sb.st_ino)) {
paxwarn(1, "Unable to copy %s, file would overwrite itself",
arcn->name);
return(0);
}
return(1);
}













static int
mk_link(char *to, struct stat *to_sb, char *from,
int ign)
{
struct stat sb;
int oerrno;





if (lstat(from, &sb) == 0) {
if (kflag)
return(0);




if ((to_sb->st_dev==sb.st_dev)&&(to_sb->st_ino == sb.st_ino)) {
paxwarn(1, "Unable to link file %s to itself", to);
return(-1);
}




if (S_ISDIR(sb.st_mode)) {
if (rmdir(from) < 0) {
syswarn(1, errno, "Unable to remove %s", from);
return(-1);
}
} else if (unlink(from) < 0) {
if (!ign) {
syswarn(1, errno, "Unable to remove %s", from);
return(-1);
}
return(1);
}
}






for (;;) {
if (link(to, from) == 0)
break;
oerrno = errno;
if (!nodirs && chk_path(from, to_sb->st_uid, to_sb->st_gid) == 0)
continue;
if (!ign) {
syswarn(1, oerrno, "Could not link to %s from %s", to,
from);
return(-1);
}
return(1);
}




return(0);
}









int
node_creat(ARCHD *arcn)
{
int res;
int ign = 0;
int oerrno;
int pass = 0;
mode_t file_mode;
struct stat sb;







file_mode = arcn->sb.st_mode & FILEBITS;

for (;;) {
switch(arcn->type) {
case PAX_DIR:
res = mkdir(arcn->name, file_mode);
if (ign)
res = 0;
break;
case PAX_CHR:
file_mode |= S_IFCHR;
res = mknod(arcn->name, file_mode, arcn->sb.st_rdev);
break;
case PAX_BLK:
file_mode |= S_IFBLK;
res = mknod(arcn->name, file_mode, arcn->sb.st_rdev);
break;
case PAX_FIF:
res = mkfifo(arcn->name, file_mode);
break;
case PAX_SCK:



paxwarn(0,
"%s skipped. Sockets cannot be copied or extracted",
arcn->name);
return(-1);
case PAX_SLK:
res = symlink(arcn->ln_name, arcn->name);
break;
case PAX_CTG:
case PAX_HLK:
case PAX_HRG:
case PAX_REG:
default:



paxwarn(0, "%s has an unknown file type, skipping",
arcn->name);
return(-1);
}






if (res == 0)
break;




oerrno = errno;
if ((ign = unlnk_exist(arcn->name, arcn->type)) < 0)
return(-1);

if (++pass <= 1)
continue;

if (nodirs || chk_path(arcn->name,arcn->sb.st_uid,arcn->sb.st_gid) < 0) {
syswarn(1, oerrno, "Could not create: %s", arcn->name);
return(-1);
}
}




if (pids)
res = set_ids(arcn->name, arcn->sb.st_uid, arcn->sb.st_gid);
else
res = 0;






if (!pmode || res)
arcn->sb.st_mode &= ~(SETBITS);
if (pmode)
set_pmode(arcn->name, arcn->sb.st_mode);

if (arcn->type == PAX_DIR && strcmp(NM_CPIO, argv0) != 0) {









if (access(arcn->name, R_OK | W_OK | X_OK) < 0) {
if (lstat(arcn->name, &sb) < 0) {
syswarn(0, errno,"Could not access %s (stat)",
arcn->name);
set_pmode(arcn->name,file_mode | S_IRWXU);
} else {






set_pmode(arcn->name,
((sb.st_mode & FILEBITS) | S_IRWXU));
if (!pmode)
arcn->sb.st_mode = sb.st_mode;
}





add_dir(arcn->name, arcn->nlen, &(arcn->sb), 1);
} else if (pmode || patime || pmtime)
add_dir(arcn->name, arcn->nlen, &(arcn->sb), 0);
}

if (patime || pmtime)
set_ftime(arcn->name, arcn->sb.st_mtime, arcn->sb.st_atime, 0);
return(0);
}













int
unlnk_exist(char *name, int type)
{
struct stat sb;




if (lstat(name, &sb) < 0)
return(0);
if (kflag)
return(-1);

if (S_ISDIR(sb.st_mode)) {




if (rmdir(name) < 0) {
if (type == PAX_DIR)
return(1);
syswarn(1,errno,"Unable to remove directory %s", name);
return(-1);
}
return(0);
}




if (unlink(name) < 0) {
syswarn(1, errno, "Could not unlink %s", name);
return(-1);
}
return(0);
}















int
chk_path( char *name, uid_t st_uid, gid_t st_gid)
{
char *spt = name;
struct stat sb;
int retval = -1;




if (*spt == '/')
++spt;

for(;;) {



spt = strchr(spt, '/');
if (spt == NULL)
break;
*spt = '\0';










if (lstat(name, &sb) == 0) {
*(spt++) = '/';
continue;
}





if (mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
*spt = '/';
retval = -1;
break;
}






retval = 0;
if (pids)
(void)set_ids(name, st_uid, st_gid);







if ((access(name, R_OK | W_OK | X_OK) < 0) &&
(lstat(name, &sb) == 0)) {
set_pmode(name, ((sb.st_mode & FILEBITS) | S_IRWXU));
add_dir(name, spt - name, &sb, 1);
}
*(spt++) = '/';
continue;
}
return(retval);
}













void
set_ftime(char *fnm, time_t mtime, time_t atime, int frc)
{
static struct timeval tv[2] = {{0L, 0L}, {0L, 0L}};
struct stat sb;

tv[0].tv_sec = atime;
tv[1].tv_sec = mtime;
if (!frc && (!patime || !pmtime)) {




if (lstat(fnm, &sb) == 0) {
if (!patime)
tv[0].tv_sec = sb.st_atime;
if (!pmtime)
tv[1].tv_sec = sb.st_mtime;
} else
syswarn(0,errno,"Unable to obtain file stats %s", fnm);
}




if (lutimes(fnm, tv) < 0)
syswarn(1, errno, "Access/modification time set failed on: %s",
fnm);
return;
}








int
set_ids(char *fnm, uid_t uid, gid_t gid)
{
if (lchown(fnm, uid, gid) < 0) {




if (strcmp(NM_PAX, argv0) == 0 || errno != EPERM || vflag ||
geteuid() == 0)
syswarn(1, errno, "Unable to set file uid/gid of %s",
fnm);
return(-1);
}
return(0);
}






void
set_pmode(char *fnm, mode_t mode)
{
mode &= ABITS;
if (lchmod(fnm, mode) < 0)
syswarn(1, errno, "Could not set permissions on %s", fnm);
return;
}

















































int
file_write(int fd, char *str, int cnt, int *rem, int *isempt, int sz,
char *name)
{
char *pt;
char *end;
int wcnt;
char *st = str;




while (cnt) {
if (!*rem) {





*isempt = 1;
*rem = sz;
}





wcnt = MIN(cnt, *rem);
cnt -= wcnt;
*rem -= wcnt;
if (*isempt) {




pt = st;
end = st + wcnt;




while ((pt < end) && (*pt == '\0'))
++pt;

if (pt == end) {



if (lseek(fd, (off_t)wcnt, SEEK_CUR) < 0) {
syswarn(1,errno,"File seek on %s",
name);
return(-1);
}
st = pt;
continue;
}



*isempt = 0;
}




if (write(fd, st, wcnt) != wcnt) {
syswarn(1, errno, "Failed write to file %s", name);
return(-1);
}
st += wcnt;
}
return(st - str);
}








void
file_flush(int fd, char *fname, int isempt)
{
static char blnk[] = "\0";





if (!isempt)
return;




if (lseek(fd, (off_t)-1, SEEK_CUR) < 0) {
syswarn(1, errno, "Failed seek on file %s", fname);
return;
}

if (write(fd, blnk, 1) < 0)
syswarn(1, errno, "Failed write to file %s", fname);
return;
}







void
rdfile_close(ARCHD *arcn, int *fd)
{



if (*fd < 0)
return;

(void)close(*fd);
*fd = -1;
if (!tflag)
return;




set_ftime(arcn->org_name, arcn->sb.st_mtime, arcn->sb.st_atime, 1);
return;
}










int
set_crc(ARCHD *arcn, int fd)
{
int i;
int res;
off_t cpcnt = 0L;
u_long size;
unsigned long crc = 0L;
char tbuf[FILEBLK];
struct stat sb;

if (fd < 0) {



arcn->crc = 0L;
return(0);
}

if ((size = (u_long)arcn->sb.st_blksize) > (u_long)sizeof(tbuf))
size = (u_long)sizeof(tbuf);





for(;;) {
if ((res = read(fd, tbuf, size)) <= 0)
break;
cpcnt += res;
for (i = 0; i < res; ++i)
crc += (tbuf[i] & 0xff);
}





if (cpcnt != arcn->sb.st_size)
paxwarn(1, "File changed size %s", arcn->org_name);
else if (fstat(fd, &sb) < 0)
syswarn(1, errno, "Failed stat on %s", arcn->org_name);
else if (arcn->sb.st_mtime != sb.st_mtime)
paxwarn(1, "File %s was modified during read", arcn->org_name);
else if (lseek(fd, (off_t)0L, SEEK_SET) < 0)
syswarn(1, errno, "File rewind failed on: %s", arcn->org_name);
else {
arcn->crc = crc;
return(0);
}
return(-1);
}
