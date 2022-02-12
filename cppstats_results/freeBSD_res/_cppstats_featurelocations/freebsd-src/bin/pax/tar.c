


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)tar.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "pax.h"
#include "extern.h"
#include "tar.h"





static u_long tar_chksm(char *, int);
static char *name_split(char *, int);
static int ul_oct(u_long, char *, int, int);
#if !defined(NET2_STAT)
static int uqd_oct(u_quad_t, char *, int, int);
#endif





static int tar_nodir;








int
tar_endwr(void)
{
return(wr_skip((off_t)(NULLCNT*BLKMULT)));
}








off_t
tar_endrd(void)
{
return((off_t)(NULLCNT*BLKMULT));
}












int
tar_trail(char *buf, int in_resync, int *cnt)
{
int i;




for (i = 0; i < BLKMULT; ++i) {
if (buf[i] != '\0')
break;
}




if (i != BLKMULT)
return(-1);









if (!in_resync && (++*cnt >= NULLCNT))
return(0);
return(1);
}












static int
ul_oct(u_long val, char *str, int len, int term)
{
char *pt;




pt = str + len - 1;
switch(term) {
case 3:
*pt-- = '\0';
break;
case 2:
*pt-- = ' ';
*pt-- = '\0';
break;
case 1:
*pt-- = ' ';
break;
case 0:
default:
*pt-- = '\0';
*pt-- = ' ';
break;
}




while (pt >= str) {
*pt-- = '0' + (char)(val & 0x7);
if ((val = val >> 3) == (u_long)0)
break;
}

while (pt >= str)
*pt-- = '0';
if (val != (u_long)0)
return(-1);
return(0);
}

#if !defined(NET2_STAT)











static int
uqd_oct(u_quad_t val, char *str, int len, int term)
{
char *pt;




pt = str + len - 1;
switch(term) {
case 3:
*pt-- = '\0';
break;
case 2:
*pt-- = ' ';
*pt-- = '\0';
break;
case 1:
*pt-- = ' ';
break;
case 0:
default:
*pt-- = '\0';
*pt-- = ' ';
break;
}




while (pt >= str) {
*pt-- = '0' + (char)(val & 0x7);
if ((val = val >> 3) == 0)
break;
}

while (pt >= str)
*pt-- = '0';
if (val != (u_quad_t)0)
return(-1);
return(0);
}
#endif











static u_long
tar_chksm(char *blk, int len)
{
char *stop;
char *pt;
u_long chksm = BLNKSUM;




pt = blk;
stop = blk + CHK_OFFSET;
while (pt < stop)
chksm += (u_long)(*pt++ & 0xff);







pt += CHK_LEN;
stop = blk + len;
while (pt < stop)
chksm += (u_long)(*pt++ & 0xff);
return(chksm);
}














int
tar_id(char *blk, int size)
{
HD_TAR *hd;
HD_USTAR *uhd;

if (size < BLKMULT)
return(-1);
hd = (HD_TAR *)blk;
uhd = (HD_USTAR *)blk;








if (hd->name[0] == '\0')
return(-1);
if (strncmp(uhd->magic, TMAGIC, TMAGLEN - 1) == 0)
return(-1);
if (asc_ul(hd->chksum,sizeof(hd->chksum),OCT) != tar_chksm(blk,BLKMULT))
return(-1);
return(0);
}








int
tar_opt(void)
{
OPLIST *opt;

while ((opt = opt_next()) != NULL) {
if (strcmp(opt->name, TAR_OPTION) ||
strcmp(opt->value, TAR_NODIR)) {
paxwarn(1, "Unknown tar format -o option/value pair %s=%s",
opt->name, opt->value);
paxwarn(1,"%s=%s is the only supported tar format option",
TAR_OPTION, TAR_NODIR);
return(-1);
}




if ((act != APPND) && (act != ARCHIVE)) {
paxwarn(1, "%s=%s is only supported when writing.",
opt->name, opt->value);
return(-1);
}
tar_nodir = 1;
}
return(0);
}










int
tar_rd(ARCHD *arcn, char *buf)
{
HD_TAR *hd;
char *pt;




if (tar_id(buf, BLKMULT) < 0)
return(-1);
arcn->org_name = arcn->name;
arcn->sb.st_nlink = 1;
arcn->pat = NULL;




hd = (HD_TAR *)buf;





arcn->nlen = l_strncpy(arcn->name, hd->name,
MIN(sizeof(hd->name), sizeof(arcn->name)) - 1);
arcn->name[arcn->nlen] = '\0';
arcn->sb.st_mode = (mode_t)(asc_ul(hd->mode,sizeof(hd->mode),OCT) &
0xfff);
arcn->sb.st_uid = (uid_t)asc_ul(hd->uid, sizeof(hd->uid), OCT);
arcn->sb.st_gid = (gid_t)asc_ul(hd->gid, sizeof(hd->gid), OCT);
#if defined(NET2_STAT)
arcn->sb.st_size = (off_t)asc_ul(hd->size, sizeof(hd->size), OCT);
arcn->sb.st_mtime = (time_t)asc_ul(hd->mtime, sizeof(hd->mtime), OCT);
#else
arcn->sb.st_size = (off_t)asc_uqd(hd->size, sizeof(hd->size), OCT);
arcn->sb.st_mtime = (time_t)asc_uqd(hd->mtime, sizeof(hd->mtime), OCT);
#endif
arcn->sb.st_ctime = arcn->sb.st_atime = arcn->sb.st_mtime;





pt = &(arcn->name[arcn->nlen - 1]);
arcn->pad = 0;
arcn->skip = 0;
switch(hd->linkflag) {
case SYMTYPE:




arcn->type = PAX_SLK;
arcn->ln_nlen = l_strncpy(arcn->ln_name, hd->linkname,
MIN(sizeof(hd->linkname), sizeof(arcn->ln_name)) - 1);
arcn->ln_name[arcn->ln_nlen] = '\0';
arcn->sb.st_mode |= S_IFLNK;
break;
case LNKTYPE:




arcn->type = PAX_HLK;
arcn->sb.st_nlink = 2;
arcn->ln_nlen = l_strncpy(arcn->ln_name, hd->linkname,
MIN(sizeof(hd->linkname), sizeof(arcn->ln_name)) - 1);
arcn->ln_name[arcn->ln_nlen] = '\0';





arcn->sb.st_mode |= S_IFREG;
break;
case DIRTYPE:



arcn->type = PAX_DIR;
arcn->sb.st_mode |= S_IFDIR;
arcn->sb.st_nlink = 2;
arcn->ln_name[0] = '\0';
arcn->ln_nlen = 0;
break;
case AREGTYPE:
case REGTYPE:
default:



arcn->ln_name[0] = '\0';
arcn->ln_nlen = 0;
if (*pt == '/') {



arcn->type = PAX_DIR;
arcn->sb.st_mode |= S_IFDIR;
arcn->sb.st_nlink = 2;
} else {





arcn->type = PAX_REG;
arcn->sb.st_mode |= S_IFREG;
arcn->pad = TAR_PAD(arcn->sb.st_size);
arcn->skip = arcn->sb.st_size;
}
break;
}




if (*pt == '/') {
*pt = '\0';
--arcn->nlen;
}
return(0);
}













int
tar_wr(ARCHD *arcn)
{
HD_TAR *hd;
int len;
HD_TAR hdblk;




switch(arcn->type) {
case PAX_DIR:



if (tar_nodir)
return(1);
break;
case PAX_CHR:
paxwarn(1, "Tar cannot archive a character device %s",
arcn->org_name);
return(1);
case PAX_BLK:
paxwarn(1, "Tar cannot archive a block device %s", arcn->org_name);
return(1);
case PAX_SCK:
paxwarn(1, "Tar cannot archive a socket %s", arcn->org_name);
return(1);
case PAX_FIF:
paxwarn(1, "Tar cannot archive a fifo %s", arcn->org_name);
return(1);
case PAX_SLK:
case PAX_HLK:
case PAX_HRG:
if (arcn->ln_nlen >= (int)sizeof(hd->linkname)) {
paxwarn(1,"Link name too long for tar %s", arcn->ln_name);
return(1);
}
break;
case PAX_REG:
case PAX_CTG:
default:
break;
}




len = arcn->nlen;
if (arcn->type == PAX_DIR)
++len;
if (len >= (int)sizeof(hd->name)) {
paxwarn(1, "File name too long for tar %s", arcn->name);
return(1);
}









hd = &hdblk;
l_strncpy(hd->name, arcn->name, sizeof(hd->name) - 1);
hd->name[sizeof(hd->name) - 1] = '\0';
arcn->pad = 0;

if (arcn->type == PAX_DIR) {





hd->linkflag = AREGTYPE;
memset(hd->linkname, 0, sizeof(hd->linkname));
hd->name[len-1] = '/';
if (ul_oct((u_long)0L, hd->size, sizeof(hd->size), 1))
goto out;
} else if (arcn->type == PAX_SLK) {



hd->linkflag = SYMTYPE;
l_strncpy(hd->linkname,arcn->ln_name, sizeof(hd->linkname) - 1);
hd->linkname[sizeof(hd->linkname) - 1] = '\0';
if (ul_oct((u_long)0L, hd->size, sizeof(hd->size), 1))
goto out;
} else if ((arcn->type == PAX_HLK) || (arcn->type == PAX_HRG)) {



hd->linkflag = LNKTYPE;
l_strncpy(hd->linkname,arcn->ln_name, sizeof(hd->linkname) - 1);
hd->linkname[sizeof(hd->linkname) - 1] = '\0';
if (ul_oct((u_long)0L, hd->size, sizeof(hd->size), 1))
goto out;
} else {



hd->linkflag = AREGTYPE;
memset(hd->linkname, 0, sizeof(hd->linkname));
#if defined(NET2_STAT)
if (ul_oct((u_long)arcn->sb.st_size, hd->size,
sizeof(hd->size), 1)) {
#else
if (uqd_oct((u_quad_t)arcn->sb.st_size, hd->size,
sizeof(hd->size), 1)) {
#endif
paxwarn(1,"File is too large for tar %s", arcn->org_name);
return(1);
}
arcn->pad = TAR_PAD(arcn->sb.st_size);
}




if (ul_oct((u_long)arcn->sb.st_mode, hd->mode, sizeof(hd->mode), 0) ||
ul_oct((u_long)arcn->sb.st_uid, hd->uid, sizeof(hd->uid), 0) ||
ul_oct((u_long)arcn->sb.st_gid, hd->gid, sizeof(hd->gid), 0) ||
ul_oct((u_long)arcn->sb.st_mtime, hd->mtime, sizeof(hd->mtime), 1))
goto out;






if (ul_oct(tar_chksm((char *)&hdblk, sizeof(HD_TAR)), hd->chksum,
sizeof(hd->chksum), 3))
goto out;
if (wr_rdbuf((char *)&hdblk, sizeof(HD_TAR)) < 0)
return(-1);
if (wr_skip((off_t)(BLKMULT - sizeof(HD_TAR))) < 0)
return(-1);
if ((arcn->type == PAX_CTG) || (arcn->type == PAX_REG))
return(0);
return(1);

out:



paxwarn(1, "Tar header field is too small for %s", arcn->org_name);
return(1);
}












int
ustar_strd(void)
{
if ((usrtb_start() < 0) || (grptb_start() < 0))
return(-1);
return(0);
}








int
ustar_stwr(void)
{
if ((uidtb_start() < 0) || (gidtb_start() < 0))
return(-1);
return(0);
}









int
ustar_id(char *blk, int size)
{
HD_USTAR *hd;

if (size < BLKMULT)
return(-1);
hd = (HD_USTAR *)blk;







if (hd->name[0] == '\0')
return(-1);
if (strncmp(hd->magic, TMAGIC, TMAGLEN - 1) != 0)
return(-1);
if (asc_ul(hd->chksum,sizeof(hd->chksum),OCT) != tar_chksm(blk,BLKMULT))
return(-1);
return(0);
}









int
ustar_rd(ARCHD *arcn, char *buf)
{
HD_USTAR *hd;
char *dest;
int cnt = 0;
dev_t devmajor;
dev_t devminor;




if (ustar_id(buf, BLKMULT) < 0)
return(-1);
arcn->org_name = arcn->name;
arcn->sb.st_nlink = 1;
arcn->pat = NULL;
arcn->nlen = 0;
hd = (HD_USTAR *)buf;





dest = arcn->name;
if (*(hd->prefix) != '\0') {
cnt = l_strncpy(dest, hd->prefix,
MIN(sizeof(hd->prefix), sizeof(arcn->name) - 2));
dest += cnt;
*dest++ = '/';
cnt++;
}





arcn->nlen = cnt + l_strncpy(dest, hd->name,
MIN(sizeof(hd->name), sizeof(arcn->name) - cnt - 1));
arcn->name[arcn->nlen] = '\0';





arcn->sb.st_mode = (mode_t)(asc_ul(hd->mode, sizeof(hd->mode), OCT) &
0xfff);
#if defined(NET2_STAT)
arcn->sb.st_size = (off_t)asc_ul(hd->size, sizeof(hd->size), OCT);
arcn->sb.st_mtime = (time_t)asc_ul(hd->mtime, sizeof(hd->mtime), OCT);
#else
arcn->sb.st_size = (off_t)asc_uqd(hd->size, sizeof(hd->size), OCT);
arcn->sb.st_mtime = (time_t)asc_uqd(hd->mtime, sizeof(hd->mtime), OCT);
#endif
arcn->sb.st_ctime = arcn->sb.st_atime = arcn->sb.st_mtime;







hd->gname[sizeof(hd->gname) - 1] = '\0';
if (gid_name(hd->gname, &(arcn->sb.st_gid)) < 0)
arcn->sb.st_gid = (gid_t)asc_ul(hd->gid, sizeof(hd->gid), OCT);
hd->uname[sizeof(hd->uname) - 1] = '\0';
if (uid_name(hd->uname, &(arcn->sb.st_uid)) < 0)
arcn->sb.st_uid = (uid_t)asc_ul(hd->uid, sizeof(hd->uid), OCT);




arcn->ln_name[0] = '\0';
arcn->ln_nlen = 0;
arcn->pad = 0;
arcn->skip = 0;
arcn->sb.st_rdev = (dev_t)0;




switch(hd->typeflag) {
case FIFOTYPE:
arcn->type = PAX_FIF;
arcn->sb.st_mode |= S_IFIFO;
break;
case DIRTYPE:
arcn->type = PAX_DIR;
arcn->sb.st_mode |= S_IFDIR;
arcn->sb.st_nlink = 2;






if (arcn->name[arcn->nlen - 1] == '/')
arcn->name[--arcn->nlen] = '\0';
break;
case BLKTYPE:
case CHRTYPE:



if (hd->typeflag == BLKTYPE) {
arcn->type = PAX_BLK;
arcn->sb.st_mode |= S_IFBLK;
} else {
arcn->type = PAX_CHR;
arcn->sb.st_mode |= S_IFCHR;
}
devmajor = (dev_t)asc_ul(hd->devmajor,sizeof(hd->devmajor),OCT);
devminor = (dev_t)asc_ul(hd->devminor,sizeof(hd->devminor),OCT);
arcn->sb.st_rdev = TODEV(devmajor, devminor);
break;
case SYMTYPE:
case LNKTYPE:
if (hd->typeflag == SYMTYPE) {
arcn->type = PAX_SLK;
arcn->sb.st_mode |= S_IFLNK;
} else {
arcn->type = PAX_HLK;



arcn->sb.st_mode |= S_IFREG;
arcn->sb.st_nlink = 2;
}



arcn->ln_nlen = l_strncpy(arcn->ln_name, hd->linkname,
MIN(sizeof(hd->linkname), sizeof(arcn->ln_name) - 1));
arcn->ln_name[arcn->ln_nlen] = '\0';
break;
case CONTTYPE:
case AREGTYPE:
case REGTYPE:
default:




arcn->type = PAX_REG;
arcn->pad = TAR_PAD(arcn->sb.st_size);
arcn->skip = arcn->sb.st_size;
arcn->sb.st_mode |= S_IFREG;
break;
}
return(0);
}













int
ustar_wr(ARCHD *arcn)
{
HD_USTAR *hd;
char *pt;
HD_USTAR hdblk;




if (arcn->type == PAX_SCK) {
paxwarn(1, "Ustar cannot archive a socket %s", arcn->org_name);
return(1);
}




if (((arcn->type == PAX_SLK) || (arcn->type == PAX_HLK) ||
(arcn->type == PAX_HRG)) &&
(arcn->ln_nlen > (int)sizeof(hd->linkname))) {
paxwarn(1, "Link name too long for ustar %s", arcn->ln_name);
return(1);
}





if ((pt = name_split(arcn->name, arcn->nlen)) == NULL) {
paxwarn(1, "File name too long for ustar %s", arcn->name);
return(1);
}
hd = &hdblk;
arcn->pad = 0L;




if (pt != arcn->name) {




*pt = '\0';
l_strncpy(hd->prefix, arcn->name, sizeof(hd->prefix));
*pt++ = '/';
} else
memset(hd->prefix, 0, sizeof(hd->prefix));





l_strncpy(hd->name, pt, sizeof(hd->name));




switch(arcn->type) {
case PAX_DIR:
hd->typeflag = DIRTYPE;
memset(hd->linkname, 0, sizeof(hd->linkname));
memset(hd->devmajor, 0, sizeof(hd->devmajor));
memset(hd->devminor, 0, sizeof(hd->devminor));
if (ul_oct((u_long)0L, hd->size, sizeof(hd->size), 3))
goto out;
break;
case PAX_CHR:
case PAX_BLK:
if (arcn->type == PAX_CHR)
hd->typeflag = CHRTYPE;
else
hd->typeflag = BLKTYPE;
memset(hd->linkname, 0, sizeof(hd->linkname));
if (ul_oct((u_long)MAJOR(arcn->sb.st_rdev), hd->devmajor,
sizeof(hd->devmajor), 3) ||
ul_oct((u_long)MINOR(arcn->sb.st_rdev), hd->devminor,
sizeof(hd->devminor), 3) ||
ul_oct((u_long)0L, hd->size, sizeof(hd->size), 3))
goto out;
break;
case PAX_FIF:
hd->typeflag = FIFOTYPE;
memset(hd->linkname, 0, sizeof(hd->linkname));
memset(hd->devmajor, 0, sizeof(hd->devmajor));
memset(hd->devminor, 0, sizeof(hd->devminor));
if (ul_oct((u_long)0L, hd->size, sizeof(hd->size), 3))
goto out;
break;
case PAX_SLK:
case PAX_HLK:
case PAX_HRG:
if (arcn->type == PAX_SLK)
hd->typeflag = SYMTYPE;
else
hd->typeflag = LNKTYPE;

l_strncpy(hd->linkname,arcn->ln_name, sizeof(hd->linkname));
memset(hd->devmajor, 0, sizeof(hd->devmajor));
memset(hd->devminor, 0, sizeof(hd->devminor));
if (ul_oct((u_long)0L, hd->size, sizeof(hd->size), 3))
goto out;
break;
case PAX_REG:
case PAX_CTG:
default:



if (arcn->type == PAX_CTG)
hd->typeflag = CONTTYPE;
else
hd->typeflag = REGTYPE;
memset(hd->linkname, 0, sizeof(hd->linkname));
memset(hd->devmajor, 0, sizeof(hd->devmajor));
memset(hd->devminor, 0, sizeof(hd->devminor));
arcn->pad = TAR_PAD(arcn->sb.st_size);
#if defined(NET2_STAT)
if (ul_oct((u_long)arcn->sb.st_size, hd->size,
sizeof(hd->size), 3)) {
#else
if (uqd_oct((u_quad_t)arcn->sb.st_size, hd->size,
sizeof(hd->size), 3)) {
#endif
paxwarn(1,"File is too long for ustar %s",arcn->org_name);
return(1);
}
break;
}

l_strncpy(hd->magic, TMAGIC, TMAGLEN);
l_strncpy(hd->version, TVERSION, TVERSLEN);





if (ul_oct((u_long)arcn->sb.st_mode, hd->mode, sizeof(hd->mode), 3) ||
ul_oct((u_long)arcn->sb.st_uid, hd->uid, sizeof(hd->uid), 3) ||
ul_oct((u_long)arcn->sb.st_gid, hd->gid, sizeof(hd->gid), 3) ||
ul_oct((u_long)arcn->sb.st_mtime,hd->mtime,sizeof(hd->mtime),3))
goto out;
l_strncpy(hd->uname,name_uid(arcn->sb.st_uid, 0),sizeof(hd->uname));
l_strncpy(hd->gname,name_gid(arcn->sb.st_gid, 0),sizeof(hd->gname));






if (ul_oct(tar_chksm((char *)&hdblk, sizeof(HD_USTAR)), hd->chksum,
sizeof(hd->chksum), 3))
goto out;
if (wr_rdbuf((char *)&hdblk, sizeof(HD_USTAR)) < 0)
return(-1);
if (wr_skip((off_t)(BLKMULT - sizeof(HD_USTAR))) < 0)
return(-1);
if ((arcn->type == PAX_CTG) || (arcn->type == PAX_REG))
return(0);
return(1);

out:



paxwarn(1, "Ustar header field is too small for %s", arcn->org_name);
return(1);
}













static char *
name_split(char *name, int len)
{
char *start;





if (len <= TNMSZ)
return(name);
if (len > TPFSZ + TNMSZ)
return(NULL);







start = name + len - TNMSZ;
while ((*start != '\0') && (*start != '/'))
++start;





if (*start == '\0')
return(NULL);
len = start - name;







if ((len > TPFSZ) || (len == 0))
return(NULL);




return(start);
}
