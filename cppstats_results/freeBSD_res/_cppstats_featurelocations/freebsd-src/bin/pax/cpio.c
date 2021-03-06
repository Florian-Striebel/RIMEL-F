


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)cpio.c 8.1 (Berkeley) 5/31/93";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "pax.h"
#include "cpio.h"
#include "extern.h"

static int rd_nm(ARCHD *, int);
static int rd_ln_nm(ARCHD *);
static int com_rd(ARCHD *);





static int swp_head;












int
cpio_strd(void)
{
return(lnk_start());
}











int
cpio_trail(ARCHD *arcn)
{



if ((strcmp(arcn->name, TRAILER) == 0) && (arcn->sb.st_size == 0))
return(0);
return(-1);
}








static int
com_rd(ARCHD *arcn)
{
arcn->skip = 0;
arcn->pat = NULL;
arcn->org_name = arcn->name;
switch(arcn->sb.st_mode & C_IFMT) {
case C_ISFIFO:
arcn->type = PAX_FIF;
break;
case C_ISDIR:
arcn->type = PAX_DIR;
break;
case C_ISBLK:
arcn->type = PAX_BLK;
break;
case C_ISCHR:
arcn->type = PAX_CHR;
break;
case C_ISLNK:
arcn->type = PAX_SLK;
break;
case C_ISOCK:
arcn->type = PAX_SCK;
break;
case C_ISCTG:
case C_ISREG:
default:




arcn->sb.st_mode = (arcn->sb.st_mode & 0xfff) | C_ISREG;
arcn->type = PAX_REG;
arcn->skip = arcn->sb.st_size;
break;
}
if (chk_lnk(arcn) < 0)
return(-1);
return(0);
}








int
cpio_endwr(void)
{
ARCHD last;




memset(&last, 0, sizeof(last));
last.nlen = sizeof(TRAILER) - 1;
last.type = PAX_REG;
last.sb.st_nlink = 1;
(void)strcpy(last.name, TRAILER);
return((*frmt->wr)(&last));
}








static int
rd_nm(ARCHD *arcn, int nsz)
{



if ((nsz == 0) || (nsz > (int)sizeof(arcn->name))) {
paxwarn(1, "Cpio file name length %d is out of range", nsz);
return(-1);
}




if ((rd_wrbuf(arcn->name,nsz) != nsz) || (arcn->name[nsz-1] != '\0') ||
(arcn->name[0] == '\0')) {
paxwarn(1, "Cpio file name in header is corrupted");
return(-1);
}
return(0);
}









static int
rd_ln_nm(ARCHD *arcn)
{



if ((arcn->sb.st_size == 0) ||
((size_t)arcn->sb.st_size >= sizeof(arcn->ln_name))) {
#if defined(NET2_STAT)
paxwarn(1, "Cpio link name length is invalid: %lu",
arcn->sb.st_size);
#else
paxwarn(1, "Cpio link name length is invalid: %ju",
(uintmax_t)arcn->sb.st_size);
#endif
return(-1);
}




if (rd_wrbuf(arcn->ln_name, (int)arcn->sb.st_size) !=
(int)arcn->sb.st_size) {
paxwarn(1, "Cpio link name read error");
return(-1);
}
arcn->ln_nlen = arcn->sb.st_size;
arcn->ln_name[arcn->ln_nlen] = '\0';




if (arcn->ln_name[0] == '\0') {
paxwarn(1, "Cpio link name is corrupt");
return(-1);
}
return(0);
}













int
cpio_id(char *blk, int size)
{
if ((size < (int)sizeof(HD_CPIO)) ||
(strncmp(blk, AMAGIC, sizeof(AMAGIC) - 1) != 0))
return(-1);
return(0);
}









int
cpio_rd(ARCHD *arcn, char *buf)
{
int nsz;
HD_CPIO *hd;




if (cpio_id(buf, sizeof(HD_CPIO)) < 0)
return(-1);
hd = (HD_CPIO *)buf;





arcn->pad = 0L;
arcn->sb.st_dev = (dev_t)asc_ul(hd->c_dev, sizeof(hd->c_dev), OCT);
arcn->sb.st_ino = (ino_t)asc_ul(hd->c_ino, sizeof(hd->c_ino), OCT);
arcn->sb.st_mode = (mode_t)asc_ul(hd->c_mode, sizeof(hd->c_mode), OCT);
arcn->sb.st_uid = (uid_t)asc_ul(hd->c_uid, sizeof(hd->c_uid), OCT);
arcn->sb.st_gid = (gid_t)asc_ul(hd->c_gid, sizeof(hd->c_gid), OCT);
arcn->sb.st_nlink = (nlink_t)asc_ul(hd->c_nlink, sizeof(hd->c_nlink),
OCT);
arcn->sb.st_rdev = (dev_t)asc_ul(hd->c_rdev, sizeof(hd->c_rdev), OCT);
#if defined(NET2_STAT)
arcn->sb.st_mtime = (time_t)asc_ul(hd->c_mtime, sizeof(hd->c_mtime),
OCT);
#else
arcn->sb.st_mtime = (time_t)asc_uqd(hd->c_mtime, sizeof(hd->c_mtime),
OCT);
#endif
arcn->sb.st_ctime = arcn->sb.st_atime = arcn->sb.st_mtime;
#if defined(NET2_STAT)
arcn->sb.st_size = (off_t)asc_ul(hd->c_filesize,sizeof(hd->c_filesize),
OCT);
#else
arcn->sb.st_size = (off_t)asc_uqd(hd->c_filesize,sizeof(hd->c_filesize),
OCT);
#endif





if ((nsz = (int)asc_ul(hd->c_namesize,sizeof(hd->c_namesize),OCT)) < 2)
return(-1);
arcn->nlen = nsz - 1;
if (rd_nm(arcn, nsz) < 0)
return(-1);

if (((arcn->sb.st_mode&C_IFMT) != C_ISLNK)||(arcn->sb.st_size == 0)) {



arcn->ln_nlen = 0;
arcn->ln_name[0] = '\0';
return(com_rd(arcn));
}





if (rd_ln_nm(arcn) < 0)
return(-1);




return(com_rd(arcn));
}








off_t
cpio_endrd(void)
{
return((off_t)(sizeof(HD_CPIO) + sizeof(TRAILER)));
}








int
cpio_stwr(void)
{
return(dev_start());
}










int
cpio_wr(ARCHD *arcn)
{
HD_CPIO *hd;
int nsz;
HD_CPIO hdblk;




if (map_dev(arcn, (u_long)CPIO_MASK, (u_long)CPIO_MASK) < 0)
return(-1);

arcn->pad = 0L;
nsz = arcn->nlen + 1;
hd = &hdblk;
if ((arcn->type != PAX_BLK) && (arcn->type != PAX_CHR))
arcn->sb.st_rdev = 0;

switch(arcn->type) {
case PAX_CTG:
case PAX_REG:
case PAX_HRG:



#if defined(NET2_STAT)
if (ul_asc((u_long)arcn->sb.st_size, hd->c_filesize,
sizeof(hd->c_filesize), OCT)) {
#else
if (uqd_asc((u_quad_t)arcn->sb.st_size, hd->c_filesize,
sizeof(hd->c_filesize), OCT)) {
#endif
paxwarn(1,"File is too large for cpio format %s",
arcn->org_name);
return(1);
}
break;
case PAX_SLK:



if (ul_asc((u_long)arcn->ln_nlen, hd->c_filesize,
sizeof(hd->c_filesize), OCT))
goto out;
break;
default:



if (ul_asc((u_long)0, hd->c_filesize, sizeof(hd->c_filesize),
OCT))
goto out;
break;
}




if (ul_asc((u_long)MAGIC, hd->c_magic, sizeof(hd->c_magic), OCT) ||
ul_asc((u_long)arcn->sb.st_dev, hd->c_dev, sizeof(hd->c_dev),
OCT) ||
ul_asc((u_long)arcn->sb.st_ino, hd->c_ino, sizeof(hd->c_ino),
OCT) ||
ul_asc((u_long)arcn->sb.st_mode, hd->c_mode, sizeof(hd->c_mode),
OCT) ||
ul_asc((u_long)arcn->sb.st_uid, hd->c_uid, sizeof(hd->c_uid),
OCT) ||
ul_asc((u_long)arcn->sb.st_gid, hd->c_gid, sizeof(hd->c_gid),
OCT) ||
ul_asc((u_long)arcn->sb.st_nlink, hd->c_nlink, sizeof(hd->c_nlink),
OCT) ||
ul_asc((u_long)arcn->sb.st_rdev, hd->c_rdev, sizeof(hd->c_rdev),
OCT) ||
ul_asc((u_long)arcn->sb.st_mtime,hd->c_mtime,sizeof(hd->c_mtime),
OCT) ||
ul_asc((u_long)nsz, hd->c_namesize, sizeof(hd->c_namesize), OCT))
goto out;




if ((wr_rdbuf((char *)&hdblk, (int)sizeof(HD_CPIO)) < 0) ||
(wr_rdbuf(arcn->name, nsz) < 0)) {
paxwarn(1, "Unable to write cpio header for %s", arcn->org_name);
return(-1);
}





if ((arcn->type == PAX_CTG) || (arcn->type == PAX_REG) ||
(arcn->type == PAX_HRG))
return(0);
if (arcn->type != PAX_SLK)
return(1);





if (wr_rdbuf(arcn->ln_name, arcn->ln_nlen) < 0) {
paxwarn(1,"Unable to write cpio link name for %s",arcn->org_name);
return(-1);
}
return(1);

out:



paxwarn(1, "Cpio header field is too small to store file %s",
arcn->org_name);
return(1);
}














int
vcpio_id(char *blk, int size)
{
if ((size < (int)sizeof(HD_VCPIO)) ||
(strncmp(blk, AVMAGIC, sizeof(AVMAGIC) - 1) != 0))
return(-1);
return(0);
}









int
crc_id(char *blk, int size)
{
if ((size < (int)sizeof(HD_VCPIO)) ||
(strncmp(blk, AVCMAGIC, (int)sizeof(AVCMAGIC) - 1) != 0))
return(-1);
return(0);
}








int
crc_strd(void)
{
docrc = 1;
return(lnk_start());
}









int
vcpio_rd(ARCHD *arcn, char *buf)
{
HD_VCPIO *hd;
dev_t devminor;
dev_t devmajor;
int nsz;





if (docrc) {
if (crc_id(buf, sizeof(HD_VCPIO)) < 0)
return(-1);
} else {
if (vcpio_id(buf, sizeof(HD_VCPIO)) < 0)
return(-1);
}

hd = (HD_VCPIO *)buf;
arcn->pad = 0L;




arcn->sb.st_ino = (ino_t)asc_ul(hd->c_ino, sizeof(hd->c_ino), HEX);
arcn->sb.st_mode = (mode_t)asc_ul(hd->c_mode, sizeof(hd->c_mode), HEX);
arcn->sb.st_uid = (uid_t)asc_ul(hd->c_uid, sizeof(hd->c_uid), HEX);
arcn->sb.st_gid = (gid_t)asc_ul(hd->c_gid, sizeof(hd->c_gid), HEX);
#if defined(NET2_STAT)
arcn->sb.st_mtime = (time_t)asc_ul(hd->c_mtime,sizeof(hd->c_mtime),HEX);
#else
arcn->sb.st_mtime = (time_t)asc_uqd(hd->c_mtime,sizeof(hd->c_mtime),HEX);
#endif
arcn->sb.st_ctime = arcn->sb.st_atime = arcn->sb.st_mtime;
#if defined(NET2_STAT)
arcn->sb.st_size = (off_t)asc_ul(hd->c_filesize,
sizeof(hd->c_filesize), HEX);
#else
arcn->sb.st_size = (off_t)asc_uqd(hd->c_filesize,
sizeof(hd->c_filesize), HEX);
#endif
arcn->sb.st_nlink = (nlink_t)asc_ul(hd->c_nlink, sizeof(hd->c_nlink),
HEX);
devmajor = (dev_t)asc_ul(hd->c_maj, sizeof(hd->c_maj), HEX);
devminor = (dev_t)asc_ul(hd->c_min, sizeof(hd->c_min), HEX);
arcn->sb.st_dev = TODEV(devmajor, devminor);
devmajor = (dev_t)asc_ul(hd->c_rmaj, sizeof(hd->c_maj), HEX);
devminor = (dev_t)asc_ul(hd->c_rmin, sizeof(hd->c_min), HEX);
arcn->sb.st_rdev = TODEV(devmajor, devminor);
arcn->crc = asc_ul(hd->c_chksum, sizeof(hd->c_chksum), HEX);





if ((nsz = (int)asc_ul(hd->c_namesize,sizeof(hd->c_namesize),HEX)) < 2)
return(-1);
arcn->nlen = nsz - 1;
if (rd_nm(arcn, nsz) < 0)
return(-1);




if (rd_skip((off_t)(VCPIO_PAD(sizeof(HD_VCPIO) + nsz))) < 0)
return(-1);





if (((arcn->sb.st_mode&C_IFMT) != C_ISLNK)||(arcn->sb.st_size == 0)) {



arcn->ln_nlen = 0;
arcn->ln_name[0] = '\0';
arcn->pad = VCPIO_PAD(arcn->sb.st_size);
return(com_rd(arcn));
}




if ((rd_ln_nm(arcn) < 0) ||
(rd_skip((off_t)(VCPIO_PAD(arcn->sb.st_size))) < 0))
return(-1);




return(com_rd(arcn));
}








off_t
vcpio_endrd(void)
{
return((off_t)(sizeof(HD_VCPIO) + sizeof(TRAILER) +
(VCPIO_PAD(sizeof(HD_VCPIO) + sizeof(TRAILER)))));
}








int
crc_stwr(void)
{
docrc = 1;
return(dev_start());
}










int
vcpio_wr(ARCHD *arcn)
{
HD_VCPIO *hd;
unsigned int nsz;
HD_VCPIO hdblk;





if (map_dev(arcn, (u_long)VCPIO_MASK, (u_long)VCPIO_MASK) < 0)
return(-1);
nsz = arcn->nlen + 1;
hd = &hdblk;
if ((arcn->type != PAX_BLK) && (arcn->type != PAX_CHR))
arcn->sb.st_rdev = 0;





if (docrc) {
if (ul_asc((u_long)VCMAGIC, hd->c_magic, sizeof(hd->c_magic),
OCT) ||
ul_asc((u_long)arcn->crc,hd->c_chksum,sizeof(hd->c_chksum),
HEX))
goto out;
} else {
if (ul_asc((u_long)VMAGIC, hd->c_magic, sizeof(hd->c_magic),
OCT) ||
ul_asc((u_long)0L, hd->c_chksum, sizeof(hd->c_chksum),HEX))
goto out;
}

switch(arcn->type) {
case PAX_CTG:
case PAX_REG:
case PAX_HRG:




arcn->pad = VCPIO_PAD(arcn->sb.st_size);
#if defined(NET2_STAT)
if (ul_asc((u_long)arcn->sb.st_size, hd->c_filesize,
sizeof(hd->c_filesize), HEX)) {
#else
if (uqd_asc((u_quad_t)arcn->sb.st_size, hd->c_filesize,
sizeof(hd->c_filesize), HEX)) {
#endif
paxwarn(1,"File is too large for sv4cpio format %s",
arcn->org_name);
return(1);
}
break;
case PAX_SLK:




arcn->pad = 0L;
if (ul_asc((u_long)arcn->ln_nlen, hd->c_filesize,
sizeof(hd->c_filesize), HEX))
goto out;
break;
default:



arcn->pad = 0L;
if (ul_asc((u_long)0L, hd->c_filesize, sizeof(hd->c_filesize),
HEX))
goto out;
break;
}




if (ul_asc((u_long)arcn->sb.st_ino, hd->c_ino, sizeof(hd->c_ino),
HEX) ||
ul_asc((u_long)arcn->sb.st_mode, hd->c_mode, sizeof(hd->c_mode),
HEX) ||
ul_asc((u_long)arcn->sb.st_uid, hd->c_uid, sizeof(hd->c_uid),
HEX) ||
ul_asc((u_long)arcn->sb.st_gid, hd->c_gid, sizeof(hd->c_gid),
HEX) ||
ul_asc((u_long)arcn->sb.st_mtime, hd->c_mtime, sizeof(hd->c_mtime),
HEX) ||
ul_asc((u_long)arcn->sb.st_nlink, hd->c_nlink, sizeof(hd->c_nlink),
HEX) ||
ul_asc((u_long)MAJOR(arcn->sb.st_dev),hd->c_maj, sizeof(hd->c_maj),
HEX) ||
ul_asc((u_long)MINOR(arcn->sb.st_dev),hd->c_min, sizeof(hd->c_min),
HEX) ||
ul_asc((u_long)MAJOR(arcn->sb.st_rdev),hd->c_rmaj,sizeof(hd->c_maj),
HEX) ||
ul_asc((u_long)MINOR(arcn->sb.st_rdev),hd->c_rmin,sizeof(hd->c_min),
HEX) ||
ul_asc((u_long)nsz, hd->c_namesize, sizeof(hd->c_namesize), HEX))
goto out;




if ((wr_rdbuf((char *)&hdblk, (int)sizeof(HD_VCPIO)) < 0) ||
(wr_rdbuf(arcn->name, (int)nsz) < 0) ||
(wr_skip((off_t)(VCPIO_PAD(sizeof(HD_VCPIO) + nsz))) < 0)) {
paxwarn(1,"Could not write sv4cpio header for %s",arcn->org_name);
return(-1);
}




if ((arcn->type == PAX_CTG) || (arcn->type == PAX_REG) ||
(arcn->type == PAX_HRG))
return(0);




if (arcn->type != PAX_SLK)
return(1);




if ((wr_rdbuf(arcn->ln_name, arcn->ln_nlen) < 0) ||
(wr_skip((off_t)(VCPIO_PAD(arcn->ln_nlen))) < 0)) {
paxwarn(1,"Could not write sv4cpio link name for %s",
arcn->org_name);
return(-1);
}
return(1);

out:



paxwarn(1,"Sv4cpio header field is too small for file %s",arcn->org_name);
return(1);
}













int
bcpio_id(char *blk, int size)
{
if (size < (int)sizeof(HD_BCPIO))
return(-1);




if (((u_short)SHRT_EXT(blk)) == MAGIC)
return(0);
if (((u_short)RSHRT_EXT(blk)) == MAGIC) {
if (!swp_head)
++swp_head;
return(0);
}
return(-1);
}










int
bcpio_rd(ARCHD *arcn, char *buf)
{
HD_BCPIO *hd;
int nsz;




if (bcpio_id(buf, sizeof(HD_BCPIO)) < 0)
return(-1);

arcn->pad = 0L;
hd = (HD_BCPIO *)buf;
if (swp_head) {



arcn->sb.st_dev = (dev_t)(RSHRT_EXT(hd->h_dev));
arcn->sb.st_ino = (ino_t)(RSHRT_EXT(hd->h_ino));
arcn->sb.st_mode = (mode_t)(RSHRT_EXT(hd->h_mode));
arcn->sb.st_uid = (uid_t)(RSHRT_EXT(hd->h_uid));
arcn->sb.st_gid = (gid_t)(RSHRT_EXT(hd->h_gid));
arcn->sb.st_nlink = (nlink_t)(RSHRT_EXT(hd->h_nlink));
arcn->sb.st_rdev = (dev_t)(RSHRT_EXT(hd->h_rdev));
arcn->sb.st_mtime = (time_t)(RSHRT_EXT(hd->h_mtime_1));
arcn->sb.st_mtime = (arcn->sb.st_mtime << 16) |
((time_t)(RSHRT_EXT(hd->h_mtime_2)));
arcn->sb.st_size = (off_t)(RSHRT_EXT(hd->h_filesize_1));
arcn->sb.st_size = (arcn->sb.st_size << 16) |
((off_t)(RSHRT_EXT(hd->h_filesize_2)));
nsz = (int)(RSHRT_EXT(hd->h_namesize));
} else {
arcn->sb.st_dev = (dev_t)(SHRT_EXT(hd->h_dev));
arcn->sb.st_ino = (ino_t)(SHRT_EXT(hd->h_ino));
arcn->sb.st_mode = (mode_t)(SHRT_EXT(hd->h_mode));
arcn->sb.st_uid = (uid_t)(SHRT_EXT(hd->h_uid));
arcn->sb.st_gid = (gid_t)(SHRT_EXT(hd->h_gid));
arcn->sb.st_nlink = (nlink_t)(SHRT_EXT(hd->h_nlink));
arcn->sb.st_rdev = (dev_t)(SHRT_EXT(hd->h_rdev));
arcn->sb.st_mtime = (time_t)(SHRT_EXT(hd->h_mtime_1));
arcn->sb.st_mtime = (arcn->sb.st_mtime << 16) |
((time_t)(SHRT_EXT(hd->h_mtime_2)));
arcn->sb.st_size = (off_t)(SHRT_EXT(hd->h_filesize_1));
arcn->sb.st_size = (arcn->sb.st_size << 16) |
((off_t)(SHRT_EXT(hd->h_filesize_2)));
nsz = (int)(SHRT_EXT(hd->h_namesize));
}
arcn->sb.st_ctime = arcn->sb.st_atime = arcn->sb.st_mtime;





if (nsz < 2)
return(-1);
arcn->nlen = nsz - 1;
if (rd_nm(arcn, nsz) < 0)
return(-1);




if (rd_skip((off_t)(BCPIO_PAD(sizeof(HD_BCPIO) + nsz))) < 0)
return(-1);





if (((arcn->sb.st_mode & C_IFMT) != C_ISLNK)||(arcn->sb.st_size == 0)){



arcn->ln_nlen = 0;
arcn->ln_name[0] = '\0';
arcn->pad = BCPIO_PAD(arcn->sb.st_size);
return(com_rd(arcn));
}

if ((rd_ln_nm(arcn) < 0) ||
(rd_skip((off_t)(BCPIO_PAD(arcn->sb.st_size))) < 0))
return(-1);




return(com_rd(arcn));
}








off_t
bcpio_endrd(void)
{
return((off_t)(sizeof(HD_BCPIO) + sizeof(TRAILER) +
(BCPIO_PAD(sizeof(HD_BCPIO) + sizeof(TRAILER)))));
}












int
bcpio_wr(ARCHD *arcn)
{
HD_BCPIO *hd;
int nsz;
HD_BCPIO hdblk;
off_t t_offt;
int t_int;
time_t t_timet;





if (map_dev(arcn, (u_long)BCPIO_MASK, (u_long)BCPIO_MASK) < 0)
return(-1);

if ((arcn->type != PAX_BLK) && (arcn->type != PAX_CHR))
arcn->sb.st_rdev = 0;
hd = &hdblk;

switch(arcn->type) {
case PAX_CTG:
case PAX_REG:
case PAX_HRG:




arcn->pad = BCPIO_PAD(arcn->sb.st_size);
hd->h_filesize_1[0] = CHR_WR_0(arcn->sb.st_size);
hd->h_filesize_1[1] = CHR_WR_1(arcn->sb.st_size);
hd->h_filesize_2[0] = CHR_WR_2(arcn->sb.st_size);
hd->h_filesize_2[1] = CHR_WR_3(arcn->sb.st_size);
t_offt = (off_t)(SHRT_EXT(hd->h_filesize_1));
t_offt = (t_offt<<16) | ((off_t)(SHRT_EXT(hd->h_filesize_2)));
if (arcn->sb.st_size != t_offt) {
paxwarn(1,"File is too large for bcpio format %s",
arcn->org_name);
return(1);
}
break;
case PAX_SLK:




arcn->pad = 0L;
hd->h_filesize_1[0] = CHR_WR_0(arcn->ln_nlen);
hd->h_filesize_1[1] = CHR_WR_1(arcn->ln_nlen);
hd->h_filesize_2[0] = CHR_WR_2(arcn->ln_nlen);
hd->h_filesize_2[1] = CHR_WR_3(arcn->ln_nlen);
t_int = (int)(SHRT_EXT(hd->h_filesize_1));
t_int = (t_int << 16) | ((int)(SHRT_EXT(hd->h_filesize_2)));
if (arcn->ln_nlen != t_int)
goto out;
break;
default:



arcn->pad = 0L;
hd->h_filesize_1[0] = (char)0;
hd->h_filesize_1[1] = (char)0;
hd->h_filesize_2[0] = (char)0;
hd->h_filesize_2[1] = (char)0;
break;
}




hd->h_magic[0] = CHR_WR_2(MAGIC);
hd->h_magic[1] = CHR_WR_3(MAGIC);
hd->h_dev[0] = CHR_WR_2(arcn->sb.st_dev);
hd->h_dev[1] = CHR_WR_3(arcn->sb.st_dev);
if (arcn->sb.st_dev != (dev_t)(SHRT_EXT(hd->h_dev)))
goto out;
hd->h_ino[0] = CHR_WR_2(arcn->sb.st_ino);
hd->h_ino[1] = CHR_WR_3(arcn->sb.st_ino);
if (arcn->sb.st_ino != (ino_t)(SHRT_EXT(hd->h_ino)))
goto out;
hd->h_mode[0] = CHR_WR_2(arcn->sb.st_mode);
hd->h_mode[1] = CHR_WR_3(arcn->sb.st_mode);
if (arcn->sb.st_mode != (mode_t)(SHRT_EXT(hd->h_mode)))
goto out;
hd->h_uid[0] = CHR_WR_2(arcn->sb.st_uid);
hd->h_uid[1] = CHR_WR_3(arcn->sb.st_uid);
if (arcn->sb.st_uid != (uid_t)(SHRT_EXT(hd->h_uid)))
goto out;
hd->h_gid[0] = CHR_WR_2(arcn->sb.st_gid);
hd->h_gid[1] = CHR_WR_3(arcn->sb.st_gid);
if (arcn->sb.st_gid != (gid_t)(SHRT_EXT(hd->h_gid)))
goto out;
hd->h_nlink[0] = CHR_WR_2(arcn->sb.st_nlink);
hd->h_nlink[1] = CHR_WR_3(arcn->sb.st_nlink);
if (arcn->sb.st_nlink != (nlink_t)(SHRT_EXT(hd->h_nlink)))
goto out;
hd->h_rdev[0] = CHR_WR_2(arcn->sb.st_rdev);
hd->h_rdev[1] = CHR_WR_3(arcn->sb.st_rdev);
if (arcn->sb.st_rdev != (dev_t)(SHRT_EXT(hd->h_rdev)))
goto out;
hd->h_mtime_1[0] = CHR_WR_0(arcn->sb.st_mtime);
hd->h_mtime_1[1] = CHR_WR_1(arcn->sb.st_mtime);
hd->h_mtime_2[0] = CHR_WR_2(arcn->sb.st_mtime);
hd->h_mtime_2[1] = CHR_WR_3(arcn->sb.st_mtime);
t_timet = (time_t)(SHRT_EXT(hd->h_mtime_1));
t_timet = (t_timet << 16) | ((time_t)(SHRT_EXT(hd->h_mtime_2)));
if (arcn->sb.st_mtime != t_timet)
goto out;
nsz = arcn->nlen + 1;
hd->h_namesize[0] = CHR_WR_2(nsz);
hd->h_namesize[1] = CHR_WR_3(nsz);
if (nsz != (int)(SHRT_EXT(hd->h_namesize)))
goto out;




if ((wr_rdbuf((char *)&hdblk, (int)sizeof(HD_BCPIO)) < 0) ||
(wr_rdbuf(arcn->name, nsz) < 0) ||
(wr_skip((off_t)(BCPIO_PAD(sizeof(HD_BCPIO) + nsz))) < 0)) {
paxwarn(1, "Could not write bcpio header for %s", arcn->org_name);
return(-1);
}




if ((arcn->type == PAX_CTG) || (arcn->type == PAX_REG) ||
(arcn->type == PAX_HRG))
return(0);




if (arcn->type != PAX_SLK)
return(1);




if ((wr_rdbuf(arcn->ln_name, arcn->ln_nlen) < 0) ||
(wr_skip((off_t)(BCPIO_PAD(arcn->ln_nlen))) < 0)) {
paxwarn(1,"Could not write bcpio link name for %s",arcn->org_name);
return(-1);
}
return(1);

out:



paxwarn(1,"Bcpio header field is too small for file %s", arcn->org_name);
return(1);
}
