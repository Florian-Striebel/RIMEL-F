


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)ftree.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fts.h>
#include "pax.h"
#include "ftree.h"
#include "extern.h"















static FTS *ftsp = NULL;
static int ftsopts;
static char *farray[2];
static FTREE *fthead = NULL;
static FTREE *fttail = NULL;
static FTREE *ftcur = NULL;
static FTSENT *ftent = NULL;
static int ftree_skip;

static int ftree_arg(void);











int
ftree_start(void)
{






ftsopts = FTS_NOCHDIR;











if (Lflag)
ftsopts |= FTS_LOGICAL;
else
ftsopts |= FTS_PHYSICAL;
if (Hflag)
#if defined(NET2_FTS)
paxwarn(0, "The -H flag is not supported on this version");
#else
ftsopts |= FTS_COMFOLLOW;
#endif
if (Xflag)
ftsopts |= FTS_XDEV;

if ((fthead == NULL) && ((farray[0] = malloc(PAXPATHLEN+2)) == NULL)) {
paxwarn(1, "Unable to allocate memory for file name buffer");
return(-1);
}

if (ftree_arg() < 0)
return(-1);
if (tflag && (atdir_start() < 0))
return(-1);
return(0);
}









int
ftree_add(char *str, int chflg)
{
FTREE *ft;
int len;




if ((str == NULL) || (*str == '\0')) {
paxwarn(0, "Invalid file name argument");
return(-1);
}






if ((ft = (FTREE *)malloc(sizeof(FTREE))) == NULL) {
paxwarn(0, "Unable to allocate memory for filename");
return(-1);
}

if (((len = strlen(str) - 1) > 0) && (str[len] == '/'))
str[len] = '\0';
ft->fname = str;
ft->refcnt = 0;
ft->chflg = chflg;
ft->fow = NULL;
if (fthead == NULL) {
fttail = fthead = ft;
return(0);
}
fttail->fow = ft;
fttail = ft;
return(0);
}







void
ftree_sel(ARCHD *arcn)
{





if (ftcur != NULL)
ftcur->refcnt = 1;







if (nflag)
ftree_skip = 1;

if (!dflag || (arcn->type != PAX_DIR))
return;

if (ftent != NULL)
(void)fts_set(ftsp, ftent, FTS_SKIP);
}






void
ftree_notsel(void)
{
if (ftent != NULL)
(void)fts_set(ftsp, ftent, FTS_SKIP);
}







void
ftree_chk(void)
{
FTREE *ft;
int wban = 0;




if (tflag)
atdir_end();





for (ft = fthead; ft != NULL; ft = ft->fow) {
if ((ft->refcnt > 0) || ft->chflg)
continue;
if (wban == 0) {
paxwarn(1,"WARNING! These file names were not selected:");
++wban;
}
(void)fprintf(stderr, "%s\n", ft->fname);
}
}











static int
ftree_arg(void)
{
char *pt;




if (ftsp != NULL) {
(void)fts_close(ftsp);
ftsp = NULL;
}





for(;;) {
if (fthead == NULL) {




if (fgets(farray[0], PAXPATHLEN+1, stdin) == NULL)
return(-1);
if ((pt = strchr(farray[0], '\n')) != NULL)
*pt = '\0';
} else {



if (ftcur == NULL)
ftcur = fthead;
else if ((ftcur = ftcur->fow) == NULL)
return(-1);
if (ftcur->chflg) {

if (fchdir(cwdfd) < 0) {
syswarn(1, errno,
"Can't fchdir to starting directory");
return(-1);
}
if (chdir(ftcur->fname) < 0) {
syswarn(1, errno, "Can't chdir to %s",
ftcur->fname);
return(-1);
}
continue;
} else
farray[0] = ftcur->fname;
}










if ((ftsp = fts_open(farray, ftsopts, NULL)) != NULL)
break;
}
return(0);
}








int
next_file(ARCHD *arcn)
{
int cnt;
time_t atime;
time_t mtime;







if (ftree_skip) {



ftree_skip = 0;
if (ftree_arg() < 0)
return(-1);
}




for(;;) {
if ((ftent = fts_read(ftsp)) == NULL) {




if (ftree_arg() < 0)
return(-1);
continue;
}




switch(ftent->fts_info) {
case FTS_D:
case FTS_DEFAULT:
case FTS_F:
case FTS_SL:
case FTS_SLNONE:



break;
case FTS_DP:








#if defined(NET2_FTS)
if (!tflag || (get_atdir(ftent->fts_statb.st_dev,
ftent->fts_statb.st_ino, &mtime, &atime) < 0))
#else
if (!tflag || (get_atdir(ftent->fts_statp->st_dev,
ftent->fts_statp->st_ino, &mtime, &atime) < 0))
#endif
continue;
set_ftime(ftent->fts_path, mtime, atime, 1);
continue;
case FTS_DC:



paxwarn(1,"File system cycle found at %s",ftent->fts_path);
continue;
case FTS_DNR:
#if defined(NET2_FTS)
syswarn(1, errno,
#else
syswarn(1, ftent->fts_errno,
#endif
"Unable to read directory %s", ftent->fts_path);
continue;
case FTS_ERR:
#if defined(NET2_FTS)
syswarn(1, errno,
#else
syswarn(1, ftent->fts_errno,
#endif
"File system traversal error");
continue;
case FTS_NS:
case FTS_NSOK:
#if defined(NET2_FTS)
syswarn(1, errno,
#else
syswarn(1, ftent->fts_errno,
#endif
"Unable to access %s", ftent->fts_path);
continue;
}





arcn->skip = 0;
arcn->pad = 0;
arcn->ln_nlen = 0;
arcn->ln_name[0] = '\0';
#if defined(NET2_FTS)
arcn->sb = ftent->fts_statb;
#else
arcn->sb = *(ftent->fts_statp);
#endif











switch(S_IFMT & arcn->sb.st_mode) {
case S_IFDIR:
arcn->type = PAX_DIR;
if (!tflag)
break;
add_atdir(ftent->fts_path, arcn->sb.st_dev,
arcn->sb.st_ino, arcn->sb.st_mtime,
arcn->sb.st_atime);
break;
case S_IFCHR:
arcn->type = PAX_CHR;
break;
case S_IFBLK:
arcn->type = PAX_BLK;
break;
case S_IFREG:






arcn->type = PAX_REG;
arcn->skip = arcn->sb.st_size;
break;
case S_IFLNK:
arcn->type = PAX_SLK;



if ((cnt = readlink(ftent->fts_path, arcn->ln_name,
PAXPATHLEN - 1)) < 0) {
syswarn(1, errno, "Unable to read symlink %s",
ftent->fts_path);
continue;
}




arcn->ln_name[cnt] = '\0';
arcn->ln_nlen = cnt;
break;
case S_IFSOCK:





arcn->type = PAX_SCK;
break;
case S_IFIFO:
arcn->type = PAX_FIF;
break;
}
break;
}




arcn->nlen = l_strncpy(arcn->name, ftent->fts_path, sizeof(arcn->name) - 1);
arcn->name[arcn->nlen] = '\0';
arcn->org_name = ftent->fts_path;
return(0);
}
