


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)sel_subs.c 8.1 (Berkeley) 5/31/93";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "pax.h"
#include "sel_subs.h"
#include "extern.h"

static int str_sec(char *, time_t *);
static int usr_match(ARCHD *);
static int grp_match(ARCHD *);
static int trng_match(ARCHD *);

static TIME_RNG *trhead = NULL;
static TIME_RNG *trtail = NULL;
static USRT **usrtb = NULL;
static GRPT **grptb = NULL;












int
sel_chk(ARCHD *arcn)
{
if (((usrtb != NULL) && usr_match(arcn)) ||
((grptb != NULL) && grp_match(arcn)) ||
((trhead != NULL) && trng_match(arcn)))
return(1);
return(0);
}
















int
usr_add(char *str)
{
u_int indx;
USRT *pt;
struct passwd *pw;
uid_t uid;




if ((str == NULL) || (*str == '\0'))
return(-1);
if ((usrtb == NULL) &&
((usrtb = (USRT **)calloc(USR_TB_SZ, sizeof(USRT *))) == NULL)) {
paxwarn(1, "Unable to allocate memory for user selection table");
return(-1);
}




if (str[0] != '#') {



if ((str[0] == '\\') && (str[1] == '#'))
++str;
if ((pw = getpwnam(str)) == NULL) {
paxwarn(1, "Unable to find uid for user: %s", str);
return(-1);
}
uid = (uid_t)pw->pw_uid;
} else
#if defined(NET2_STAT)
uid = (uid_t)atoi(str+1);
#else
uid = (uid_t)strtoul(str+1, NULL, 10);
#endif
endpwent();




indx = ((unsigned)uid) % USR_TB_SZ;
if ((pt = usrtb[indx]) != NULL) {
while (pt != NULL) {
if (pt->uid == uid)
return(0);
pt = pt->fow;
}
}




if ((pt = (USRT *)malloc(sizeof(USRT))) != NULL) {
pt->uid = uid;
pt->fow = usrtb[indx];
usrtb[indx] = pt;
return(0);
}
paxwarn(1, "User selection table out of memory");
return(-1);
}








static int
usr_match(ARCHD *arcn)
{
USRT *pt;




pt = usrtb[((unsigned)arcn->sb.st_uid) % USR_TB_SZ];
while (pt != NULL) {
if (pt->uid == arcn->sb.st_uid)
return(0);
pt = pt->fow;
}




return(1);
}








int
grp_add(char *str)
{
u_int indx;
GRPT *pt;
struct group *gr;
gid_t gid;




if ((str == NULL) || (*str == '\0'))
return(-1);
if ((grptb == NULL) &&
((grptb = (GRPT **)calloc(GRP_TB_SZ, sizeof(GRPT *))) == NULL)) {
paxwarn(1, "Unable to allocate memory fo group selection table");
return(-1);
}




if (str[0] != '#') {



if ((str[0] == '\\') && (str[1] == '#'))
++str;
if ((gr = getgrnam(str)) == NULL) {
paxwarn(1,"Cannot determine gid for group name: %s", str);
return(-1);
}
gid = gr->gr_gid;
} else
#if defined(NET2_STAT)
gid = (gid_t)atoi(str+1);
#else
gid = (gid_t)strtoul(str+1, NULL, 10);
#endif
endgrent();




indx = ((unsigned)gid) % GRP_TB_SZ;
if ((pt = grptb[indx]) != NULL) {
while (pt != NULL) {
if (pt->gid == gid)
return(0);
pt = pt->fow;
}
}




if ((pt = (GRPT *)malloc(sizeof(GRPT))) != NULL) {
pt->gid = gid;
pt->fow = grptb[indx];
grptb[indx] = pt;
return(0);
}
paxwarn(1, "Group selection table out of memory");
return(-1);
}








static int
grp_match(ARCHD *arcn)
{
GRPT *pt;




pt = grptb[((unsigned)arcn->sb.st_gid) % GRP_TB_SZ];
while (pt != NULL) {
if (pt->gid == arcn->sb.st_gid)
return(0);
pt = pt->fow;
}




return(1);
}






























int
trng_add(char *str)
{
TIME_RNG *pt;
char *up_pt = NULL;
char *stpt;
char *flgpt;
int dot = 0;




if ((str == NULL) || (*str == '\0')) {
paxwarn(1, "Empty time range string");
return(-1);
}




if ((flgpt = strrchr(str, '/')) != NULL)
*flgpt++ = '\0';

for (stpt = str; *stpt != '\0'; ++stpt) {
if ((*stpt >= '0') && (*stpt <= '9'))
continue;
if ((*stpt == ',') && (up_pt == NULL)) {
*stpt = '\0';
up_pt = stpt + 1;
dot = 0;
continue;
}




if ((*stpt == '.') && (!dot)) {
++dot;
continue;
}
paxwarn(1, "Improperly specified time range: %s", str);
goto out;
}




if ((pt = (TIME_RNG *)malloc(sizeof(TIME_RNG))) == NULL) {
paxwarn(1, "Unable to allocate memory for time range");
return(-1);
}





if ((flgpt == NULL) || (*flgpt == '\0'))
pt->flgs = CMPMTME;
else {
pt->flgs = 0;
while (*flgpt != '\0') {
switch(*flgpt) {
case 'M':
case 'm':
pt->flgs |= CMPMTME;
break;
case 'C':
case 'c':
pt->flgs |= CMPCTME;
break;
default:
paxwarn(1, "Bad option %c with time range %s",
*flgpt, str);
free(pt);
goto out;
}
++flgpt;
}
}




pt->low_time = pt->high_time = time(NULL);
if (*str != '\0') {



if (str_sec(str, &(pt->low_time)) < 0) {
paxwarn(1, "Illegal lower time range %s", str);
free(pt);
goto out;
}
pt->flgs |= HASLOW;
}

if ((up_pt != NULL) && (*up_pt != '\0')) {



if (str_sec(up_pt, &(pt->high_time)) < 0) {
paxwarn(1, "Illegal upper time range %s", up_pt);
free(pt);
goto out;
}
pt->flgs |= HASHIGH;




if (pt->flgs & HASLOW) {
if (pt->low_time > pt->high_time) {
paxwarn(1, "Upper %s and lower %s time overlap",
up_pt, str);
free(pt);
return(-1);
}
}
}

pt->fow = NULL;
if (trhead == NULL) {
trtail = trhead = pt;
return(0);
}
trtail->fow = pt;
trtail = pt;
return(0);

out:
paxwarn(1, "Time range format is: [yy[mm[dd[hh]]]]mm[.ss][/[c][m]]");
return(-1);
}








static int
trng_match(ARCHD *arcn)
{
TIME_RNG *pt;





pt = trhead;
while (pt != NULL) {
switch(pt->flgs & CMPBOTH) {
case CMPBOTH:




if (((pt->flgs & HASLOW) &&
(arcn->sb.st_mtime < pt->low_time) &&
(arcn->sb.st_ctime < pt->low_time)) ||
((pt->flgs & HASHIGH) &&
(arcn->sb.st_mtime > pt->high_time) &&
(arcn->sb.st_ctime > pt->high_time))) {
pt = pt->fow;
continue;
}
break;
case CMPCTME:



if (((pt->flgs & HASLOW) &&
(arcn->sb.st_ctime < pt->low_time)) ||
((pt->flgs & HASHIGH) &&
(arcn->sb.st_ctime > pt->high_time))) {
pt = pt->fow;
continue;
}
break;
case CMPMTME:
default:



if (((pt->flgs & HASLOW) &&
(arcn->sb.st_mtime < pt->low_time)) ||
((pt->flgs & HASHIGH) &&
(arcn->sb.st_mtime > pt->high_time))) {
pt = pt->fow;
continue;
}
break;
}
break;
}

if (pt == NULL)
return(1);
return(0);
}









static int
str_sec(char *str, time_t *tval)
{
struct tm *lt;
char *dot = NULL;

lt = localtime(tval);
if ((dot = strchr(str, '.')) != NULL) {



*dot++ = '\0';
if (strlen(dot) != 2)
return(-1);
if ((lt->tm_sec = ATOI2(dot)) > 61)
return(-1);
} else
lt->tm_sec = 0;

switch (strlen(str)) {
case 10:




if ((lt->tm_year = ATOI2(str)) < 69)
lt->tm_year += 100;
str += 2;

case 8:




if ((lt->tm_mon = ATOI2(str)) > 12)
return(-1);
--lt->tm_mon;
str += 2;

case 6:



if ((lt->tm_mday = ATOI2(str)) > 31)
return(-1);
str += 2;

case 4:



if ((lt->tm_hour = ATOI2(str)) > 23)
return(-1);
str += 2;

case 2:



if ((lt->tm_min = ATOI2(str)) > 59)
return(-1);
break;
default:
return(-1);
}



if ((*tval = mktime(lt)) == -1)
return(-1);
return(0);
}
