


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)tables.c 8.1 (Berkeley) 5/31/93";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pax.h"
#include "tables.h"
#include "extern.h"
















static HRDLNK **ltab = NULL;
static FTM **ftab = NULL;
static NAMT **ntab = NULL;
static DEVT **dtab = NULL;
static ATDIR **atab = NULL;
static int dirfd = -1;
static u_long dircnt;
static int ffd = -1;

static DEVT *chk_dev(dev_t, int);
























int
lnk_start(void)
{
if (ltab != NULL)
return(0);
if ((ltab = (HRDLNK **)calloc(L_TAB_SZ, sizeof(HRDLNK *))) == NULL) {
paxwarn(1, "Cannot allocate memory for hard link table");
return(-1);
}
return(0);
}













int
chk_lnk(ARCHD *arcn)
{
HRDLNK *pt;
HRDLNK **ppt;
u_int indx;

if (ltab == NULL)
return(-1);



if ((arcn->type == PAX_DIR) || (arcn->sb.st_nlink <= 1))
return(0);




indx = ((unsigned)arcn->sb.st_ino) % L_TAB_SZ;
if ((pt = ltab[indx]) != NULL) {



ppt = &(ltab[indx]);
while (pt != NULL) {
if ((pt->ino == arcn->sb.st_ino) &&
(pt->dev == arcn->sb.st_dev))
break;
ppt = &(pt->fow);
pt = pt->fow;
}

if (pt != NULL) {






arcn->ln_nlen = l_strncpy(arcn->ln_name, pt->name,
sizeof(arcn->ln_name) - 1);
arcn->ln_name[arcn->ln_nlen] = '\0';
if (arcn->type == PAX_REG)
arcn->type = PAX_HRG;
else
arcn->type = PAX_HLK;





if (--pt->nlink <= 1) {
*ppt = pt->fow;
free(pt->name);
free(pt);
}
return(1);
}
}





if ((pt = (HRDLNK *)malloc(sizeof(HRDLNK))) != NULL) {
if ((pt->name = strdup(arcn->name)) != NULL) {
pt->dev = arcn->sb.st_dev;
pt->ino = arcn->sb.st_ino;
pt->nlink = arcn->sb.st_nlink;
pt->fow = ltab[indx];
ltab[indx] = pt;
return(0);
}
free(pt);
}

paxwarn(1, "Hard link table out of memory");
return(-1);
}








void
purg_lnk(ARCHD *arcn)
{
HRDLNK *pt;
HRDLNK **ppt;
u_int indx;

if (ltab == NULL)
return;



if ((arcn->sb.st_nlink <= 1) || (arcn->type == PAX_DIR) ||
(arcn->type == PAX_HLK) || (arcn->type == PAX_HRG))
return;




indx = ((unsigned)arcn->sb.st_ino) % L_TAB_SZ;
if ((pt = ltab[indx]) == NULL)
return;





ppt = &(ltab[indx]);
while (pt != NULL) {
if ((pt->ino == arcn->sb.st_ino) &&
(pt->dev == arcn->sb.st_dev))
break;
ppt = &(pt->fow);
pt = pt->fow;
}
if (pt == NULL)
return;




*ppt = pt->fow;
free(pt->name);
free(pt);
}









void
lnk_end(void)
{
int i;
HRDLNK *pt;
HRDLNK *ppt;

if (ltab == NULL)
return;

for (i = 0; i < L_TAB_SZ; ++i) {
if (ltab[i] == NULL)
continue;
pt = ltab[i];
ltab[i] = NULL;




while (pt != NULL) {
ppt = pt;
pt = ppt->fow;
free(ppt->name);
free(ppt);
}
}
return;
}
































int
ftime_start(void)
{

if (ftab != NULL)
return(0);
if ((ftab = (FTM **)calloc(F_TAB_SZ, sizeof(FTM *))) == NULL) {
paxwarn(1, "Cannot allocate memory for file time table");
return(-1);
}





memcpy(tempbase, _TFILE_BASE, sizeof(_TFILE_BASE));
if ((ffd = mkstemp(tempfile)) < 0) {
syswarn(1, errno, "Unable to create temporary file: %s",
tempfile);
return(-1);
}
(void)unlink(tempfile);

return(0);
}













int
chk_ftime(ARCHD *arcn)
{
FTM *pt;
int namelen;
u_int indx;
char ckname[PAXPATHLEN+1];




if (ftab == NULL)
return(0);




namelen = arcn->nlen;
indx = st_hash(arcn->name, namelen, F_TAB_SZ);
if ((pt = ftab[indx]) != NULL) {





while (pt != NULL) {
if (pt->namelen == namelen) {




if (lseek(ffd,pt->seek,SEEK_SET) != pt->seek) {
syswarn(1, errno,
"Failed ftime table seek");
return(-1);
}
if (read(ffd, ckname, namelen) != namelen) {
syswarn(1, errno,
"Failed ftime table read");
return(-1);
}




if (!strncmp(ckname, arcn->name, namelen))
break;
}




pt = pt->fow;
}

if (pt != NULL) {



if (arcn->sb.st_mtime > pt->mtime) {



pt->mtime = arcn->sb.st_mtime;
return(0);
}



return(1);
}
}




if ((pt = (FTM *)malloc(sizeof(FTM))) != NULL) {




if ((pt->seek = lseek(ffd, (off_t)0, SEEK_END)) >= 0) {
if (write(ffd, arcn->name, namelen) == namelen) {
pt->mtime = arcn->sb.st_mtime;
pt->namelen = namelen;
pt->fow = ftab[indx];
ftab[indx] = pt;
return(0);
}
syswarn(1, errno, "Failed write to file time table");
} else
syswarn(1, errno, "Failed seek on file time table");
} else
paxwarn(1, "File time table ran out of memory");

if (pt != NULL)
free(pt);
return(-1);
}




















int
name_start(void)
{
if (ntab != NULL)
return(0);
if ((ntab = (NAMT **)calloc(N_TAB_SZ, sizeof(NAMT *))) == NULL) {
paxwarn(1, "Cannot allocate memory for interactive rename table");
return(-1);
}
return(0);
}










int
add_name(char *oname, int onamelen, char *nname)
{
NAMT *pt;
u_int indx;

if (ntab == NULL) {



paxwarn(0, "No interactive rename table, links may fail\n");
return(0);
}





indx = st_hash(oname, onamelen, N_TAB_SZ);
if ((pt = ntab[indx]) != NULL) {



while ((pt != NULL) && (strcmp(oname, pt->oname) != 0))
pt = pt->fow;

if (pt != NULL) {




if (strcmp(nname, pt->nname) == 0)
return(0);

free(pt->nname);
if ((pt->nname = strdup(nname)) == NULL) {
paxwarn(1, "Cannot update rename table");
return(-1);
}
return(0);
}
}




if ((pt = (NAMT *)malloc(sizeof(NAMT))) != NULL) {
if ((pt->oname = strdup(oname)) != NULL) {
if ((pt->nname = strdup(nname)) != NULL) {
pt->fow = ntab[indx];
ntab[indx] = pt;
return(0);
}
free(pt->oname);
}
free(pt);
}
paxwarn(1, "Interactive rename table out of memory");
return(-1);
}








void
sub_name(char *oname, int *onamelen, size_t onamesize)
{
NAMT *pt;
u_int indx;

if (ntab == NULL)
return;



indx = st_hash(oname, *onamelen, N_TAB_SZ);
if ((pt = ntab[indx]) == NULL)
return;

while (pt != NULL) {



if (strcmp(oname, pt->oname) == 0) {




*onamelen = l_strncpy(oname, pt->nname, onamesize - 1);
oname[*onamelen] = '\0';
return;
}
pt = pt->fow;
}




return;
}
















































int
dev_start(void)
{
if (dtab != NULL)
return(0);
if ((dtab = (DEVT **)calloc(D_TAB_SZ, sizeof(DEVT *))) == NULL) {
paxwarn(1, "Cannot allocate memory for device mapping table");
return(-1);
}
return(0);
}











int
add_dev(ARCHD *arcn)
{
if (chk_dev(arcn->sb.st_dev, 1) == NULL)
return(-1);
return(0);
}














static DEVT *
chk_dev(dev_t dev, int add)
{
DEVT *pt;
u_int indx;

if (dtab == NULL)
return(NULL);



indx = ((unsigned)dev) % D_TAB_SZ;
if ((pt = dtab[indx]) != NULL) {
while ((pt != NULL) && (pt->dev != dev))
pt = pt->fow;




if (pt != NULL)
return(pt);
}





if (add == 0)
return(NULL);






if ((pt = (DEVT *)malloc(sizeof(DEVT))) == NULL) {
paxwarn(1, "Device map table out of memory");
return(NULL);
}
pt->dev = dev;
pt->list = NULL;
pt->fow = dtab[indx];
dtab[indx] = pt;
return(pt);
}













int
map_dev(ARCHD *arcn, u_long dev_mask, u_long ino_mask)
{
DEVT *pt;
DLIST *dpt;
static dev_t lastdev = 0;
int trc_ino = 0;
int trc_dev = 0;
ino_t trunc_bits = 0;
ino_t nino;

if (dtab == NULL)
return(0);




if ((arcn->sb.st_dev & (dev_t)dev_mask) != arcn->sb.st_dev)
++trc_dev;
if ((nino = arcn->sb.st_ino & (ino_t)ino_mask) != arcn->sb.st_ino) {
++trc_ino;
trunc_bits = arcn->sb.st_ino & (ino_t)(~ino_mask);
}





if ((pt = chk_dev(arcn->sb.st_dev, 0)) != NULL) {



for (dpt = pt->list; dpt != NULL; dpt = dpt->fow)
if (dpt->trunc_bits == trunc_bits)
break;

if (dpt != NULL) {




arcn->sb.st_dev = dpt->dev;
arcn->sb.st_ino = nino;
return(0);
}
} else {




if (!trc_ino && !trc_dev)
return(0);




if ((pt = chk_dev(arcn->sb.st_dev, 1)) == NULL)
goto bad;










if (!trc_dev && (trunc_bits != 0)) {
if ((dpt = (DLIST *)malloc(sizeof(DLIST))) == NULL)
goto bad;
dpt->trunc_bits = 0;
dpt->dev = arcn->sb.st_dev;
dpt->fow = pt->list;
pt->list = dpt;
}
}





while (++lastdev > 0) {
if (chk_dev(lastdev, 0) != NULL)
continue;





if (((lastdev & ((dev_t)dev_mask)) != lastdev) ||
(chk_dev(lastdev, 1) == NULL))
goto bad;
break;
}

if ((lastdev <= 0) || ((dpt = (DLIST *)malloc(sizeof(DLIST))) == NULL))
goto bad;





dpt->trunc_bits = trunc_bits;
dpt->dev = lastdev;
dpt->fow = pt->list;
pt->list = dpt;
arcn->sb.st_dev = lastdev;
arcn->sb.st_ino = nino;
return(0);

bad:
paxwarn(1, "Unable to fix truncated inode/device field when storing %s",
arcn->name);
paxwarn(0, "Archive may create improper hard links when extracted");
return(0);
}

























int
atdir_start(void)
{
if (atab != NULL)
return(0);
if ((atab = (ATDIR **)calloc(A_TAB_SZ, sizeof(ATDIR *))) == NULL) {
paxwarn(1,"Cannot allocate space for directory access time table");
return(-1);
}
return(0);
}









void
atdir_end(void)
{
ATDIR *pt;
int i;

if (atab == NULL)
return;




for (i = 0; i < A_TAB_SZ; ++i) {
if ((pt = atab[i]) == NULL)
continue;





for (; pt != NULL; pt = pt->fow)
set_ftime(pt->name, pt->mtime, pt->atime, 1);
}
}







void
add_atdir(char *fname, dev_t dev, ino_t ino, time_t mtime, time_t atime)
{
ATDIR *pt;
u_int indx;

if (atab == NULL)
return;








indx = ((unsigned)ino) % A_TAB_SZ;
if ((pt = atab[indx]) != NULL) {
while (pt != NULL) {
if ((pt->ino == ino) && (pt->dev == dev))
break;
pt = pt->fow;
}




if (pt != NULL)
return;
}




if ((pt = (ATDIR *)malloc(sizeof(ATDIR))) != NULL) {
if ((pt->name = strdup(fname)) != NULL) {
pt->dev = dev;
pt->ino = ino;
pt->mtime = mtime;
pt->atime = atime;
pt->fow = atab[indx];
atab[indx] = pt;
return;
}
free(pt);
}

paxwarn(1, "Directory access time reset table ran out of memory");
return;
}












int
get_atdir(dev_t dev, ino_t ino, time_t *mtime, time_t *atime)
{
ATDIR *pt;
ATDIR **ppt;
u_int indx;

if (atab == NULL)
return(-1);



indx = ((unsigned)ino) % A_TAB_SZ;
if ((pt = atab[indx]) == NULL)
return(-1);

ppt = &(atab[indx]);
while (pt != NULL) {
if ((pt->ino == ino) && (pt->dev == dev))
break;



ppt = &(pt->fow);
pt = pt->fow;
}




if (pt == NULL)
return(-1);




*ppt = pt->fow;
*mtime = pt->mtime;
*atime = pt->atime;
free(pt->name);
free(pt);
return(0);
}
































int
dir_start(void)
{

if (dirfd != -1)
return(0);




memcpy(tempbase, _TFILE_BASE, sizeof(_TFILE_BASE));
if ((dirfd = mkstemp(tempfile)) >= 0) {
(void)unlink(tempfile);
return(0);
}
paxwarn(1, "Unable to create temporary file for directory times: %s",
tempfile);
return(-1);
}














void
add_dir(char *name, int nlen, struct stat *psb, int frc_mode)
{
DIRDATA dblk;

if (dirfd < 0)
return;





if ((dblk.npos = lseek(dirfd, 0L, SEEK_CUR)) < 0) {
paxwarn(1,"Unable to store mode and times for directory: %s",name);
return;
}




dblk.nlen = nlen + 1;
dblk.mode = psb->st_mode & 0xffff;
dblk.mtime = psb->st_mtime;
dblk.atime = psb->st_atime;
dblk.frc_mode = frc_mode;
if ((write(dirfd, name, dblk.nlen) == dblk.nlen) &&
(write(dirfd, (char *)&dblk, sizeof(dblk)) == sizeof(dblk))) {
++dircnt;
return;
}

paxwarn(1,"Unable to store mode and times for created directory: %s",name);
return;
}







void
proc_dir(void)
{
char name[PAXPATHLEN+1];
DIRDATA dblk;
u_long cnt;

if (dirfd < 0)
return;



for (cnt = 0; cnt < dircnt; ++cnt) {




if (lseek(dirfd, -((off_t)sizeof(dblk)), SEEK_CUR) < 0)
break;
if (read(dirfd,(char *)&dblk, sizeof(dblk)) != sizeof(dblk))
break;
if (lseek(dirfd, dblk.npos, SEEK_SET) < 0)
break;
if (read(dirfd, name, dblk.nlen) != dblk.nlen)
break;
if (lseek(dirfd, dblk.npos, SEEK_SET) < 0)
break;





if (pmode || dblk.frc_mode)
set_pmode(name, dblk.mode);
if (patime || pmtime)
set_ftime(name, dblk.mtime, dblk.atime, 0);
}

(void)close(dirfd);
dirfd = -1;
if (cnt != dircnt)
paxwarn(1,"Unable to set mode and times for created directories");
return;
}



















u_int
st_hash(char *name, int len, int tabsz)
{
char *pt;
char *dest;
char *end;
int i;
u_int key = 0;
int steps;
int res;
u_int val;






if (len > MAXKEYLEN) {
pt = &(name[len - MAXKEYLEN]);
len = MAXKEYLEN;
} else
pt = name;





steps = len/sizeof(u_int);
res = len % sizeof(u_int);






for (i = 0; i < steps; ++i) {
end = pt + sizeof(u_int);
dest = (char *)&val;
while (pt < end)
*dest++ = *pt++;
key += val;
}




if (res) {
val = 0;
end = pt + res;
dest = (char *)&val;
while (pt < end)
*dest++ = *pt++;
key += val;
}




return(key % tabsz);
}
