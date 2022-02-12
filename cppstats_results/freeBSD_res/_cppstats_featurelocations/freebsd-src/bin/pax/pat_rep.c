


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)pat_rep.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(NET2_REGEX)
#include <regexp.h>
#else
#include <regex.h>
#endif
#include "pax.h"
#include "pat_rep.h"
#include "extern.h"








#define MAXSUBEXP 10
static PATTERN *pathead = NULL;
static PATTERN *pattail = NULL;
static REPLACE *rephead = NULL;
static REPLACE *reptail = NULL;

static int rep_name(char *, int *, int);
static int tty_rename(ARCHD *);
static int fix_path(char *, int *, char *, int);
static int fn_match(char *, char *, char **);
static char * range_match(char *, int);
#if defined(NET2_REGEX)
static int resub(regexp *, char *, char *, char *);
#else
static int resub(regex_t *, regmatch_t *, char *, char *, char *, char *);
#endif

















int
rep_add(char *str)
{
char *pt1;
char *pt2;
REPLACE *rep;
#if !defined(NET2_REGEX)
int res;
char rebuf[BUFSIZ];
#endif




if ((str == NULL) || (*str == '\0')) {
paxwarn(1, "Empty replacement string");
return(-1);
}





if ((pt1 = strchr(str+1, *str)) == NULL) {
paxwarn(1, "Invalid replacement string %s", str);
return(-1);
}





if ((rep = (REPLACE *)malloc(sizeof(REPLACE))) == NULL) {
paxwarn(1, "Unable to allocate memory for replacement string");
return(-1);
}

*pt1 = '\0';
#if defined(NET2_REGEX)
if ((rep->rcmp = regcomp(str+1)) == NULL) {
#else
if ((res = regcomp(&(rep->rcmp), str+1, 0)) != 0) {
regerror(res, &(rep->rcmp), rebuf, sizeof(rebuf));
paxwarn(1, "%s while compiling regular expression %s", rebuf, str);
#endif
free(rep);
return(-1);
}






*pt1++ = *str;
if ((pt2 = strchr(pt1, *str)) == NULL) {
#if defined(NET2_REGEX)
free(rep->rcmp);
#else
regfree(&rep->rcmp);
#endif
free(rep);
paxwarn(1, "Invalid replacement string %s", str);
return(-1);
}

*pt2 = '\0';
rep->nstr = pt1;
pt1 = pt2++;
rep->flgs = 0;




while (*pt2 != '\0') {
switch(*pt2) {
case 'g':
case 'G':
rep->flgs |= GLOB;
break;
case 'p':
case 'P':
rep->flgs |= PRNT;
break;
default:
#if defined(NET2_REGEX)
free(rep->rcmp);
#else
regfree(&rep->rcmp);
#endif
free(rep);
*pt1 = *str;
paxwarn(1, "Invalid replacement string option %s", str);
return(-1);
}
++pt2;
}




rep->fow = NULL;
if (rephead == NULL) {
reptail = rephead = rep;
return(0);
}
reptail->fow = rep;
reptail = rep;
return(0);
}












int
pat_add(char *str, char *chdnam)
{
PATTERN *pt;




if ((str == NULL) || (*str == '\0')) {
paxwarn(1, "Empty pattern string");
return(-1);
}






if ((pt = (PATTERN *)malloc(sizeof(PATTERN))) == NULL) {
paxwarn(1, "Unable to allocate memory for pattern string");
return(-1);
}

pt->pstr = str;
pt->pend = NULL;
pt->plen = strlen(str);
pt->fow = NULL;
pt->flgs = 0;
pt->chdname = chdnam;

if (pathead == NULL) {
pattail = pathead = pt;
return(0);
}
pattail->fow = pt;
pattail = pt;
return(0);
}







void
pat_chk(void)
{
PATTERN *pt;
int wban = 0;





for (pt = pathead; pt != NULL; pt = pt->fow) {
if (pt->flgs & MTCH)
continue;
if (!wban) {
paxwarn(1, "WARNING! These patterns were not matched:");
++wban;
}
(void)fprintf(stderr, "%s\n", pt->pstr);
}
}

















int
pat_sel(ARCHD *arcn)
{
PATTERN *pt;
PATTERN **ppt;
int len;




if ((pathead == NULL) || ((pt = arcn->pat) == NULL))
return(0);





if (!nflag) {
pt->flgs |= MTCH;
return(0);
}








if (pt->flgs & DIR_MTCH)
return(0);

if (!dflag && ((pt->pend != NULL) || (arcn->type == PAX_DIR))) {













if (pt->pend != NULL)
*pt->pend = '\0';

if ((pt->pstr = strdup(arcn->name)) == NULL) {
paxwarn(1, "Pattern select out of memory");
if (pt->pend != NULL)
*pt->pend = '/';
pt->pend = NULL;
return(-1);
}




if (pt->pend != NULL) {
*pt->pend = '/';
pt->pend = NULL;
}
pt->plen = strlen(pt->pstr);




len = pt->plen - 1;
if (*(pt->pstr + len) == '/') {
*(pt->pstr + len) = '\0';
pt->plen = len;
}
pt->flgs = DIR_MTCH | MTCH;
arcn->pat = pt;
return(0);
}









pt = pathead;
ppt = &pathead;
while ((pt != NULL) && (pt != arcn->pat)) {
ppt = &(pt->fow);
pt = pt->fow;
}

if (pt == NULL) {



paxwarn(1, "Pattern list inconsistent");
return(-1);
}
*ppt = pt->fow;
free(pt);
arcn->pat = NULL;
return(0);
}













int
pat_match(ARCHD *arcn)
{
PATTERN *pt;

arcn->pat = NULL;





if (pathead == NULL) {
if (nflag && !cflag)
return(-1);
return(0);
}




pt = pathead;
while (pt != NULL) {




if (pt->flgs & DIR_MTCH) {






if ((arcn->name[pt->plen] == '/') &&
(strncmp(pt->pstr, arcn->name, pt->plen) == 0))
break;
} else if (fn_match(pt->pstr, arcn->name, &pt->pend) == 0)
break;
pt = pt->fow;
}





if (pt == NULL)
return(cflag ? 0 : 1);






arcn->pat = pt;
if (!cflag)
return(0);

if (pat_sel(arcn) < 0)
return(-1);
arcn->pat = NULL;
return(1);
}










static int
fn_match(char *pattern, char *string, char **pend)
{
char c;
char test;

*pend = NULL;
for (;;) {
switch (c = *pattern++) {
case '\0':



if (*string == '\0')
return(0);




if ((dflag == 1) || (*string != '/'))
return(-1);





*pend = string;
return(0);
case '?':
if ((test = *string++) == '\0')
return (-1);
break;
case '*':
c = *pattern;



while (c == '*')
c = *++pattern;




if (c == '\0')
return (0);




while ((test = *string) != '\0') {
if (!fn_match(pattern, string, pend))
return (0);
++string;
}
return (-1);
case '[':



if (((test = *string++) == '\0') ||
((pattern = range_match(pattern, test)) == NULL))
return (-1);
break;
case '\\':
default:
if (c != *string++)
return (-1);
break;
}
}

}

static char *
range_match(char *pattern, int test)
{
char c;
char c2;
int negate;
int ok = 0;

if ((negate = (*pattern == '!')) != 0)
++pattern;

while ((c = *pattern++) != ']') {



if (c == '\0')
return (NULL);

if ((*pattern == '-') && ((c2 = pattern[1]) != '\0') &&
(c2 != ']')) {
if ((c <= test) && (test <= c2))
ok = 1;
pattern += 2;
} else if (c == test)
ok = 1;
}
return (ok == negate ? NULL : pattern);
}















int
mod_name(ARCHD *arcn)
{
int res = 0;





if (rmleadslash && arcn->name[0] == '/') {
if (arcn->name[1] == '\0') {
arcn->name[0] = '.';
} else {
(void)memmove(arcn->name, &arcn->name[1],
strlen(arcn->name));
arcn->nlen--;
}
if (rmleadslash < 2) {
rmleadslash = 2;
paxwarn(0, "Removing leading / from absolute path names in the archive");
}
}
if (rmleadslash && arcn->ln_name[0] == '/' &&
(arcn->type == PAX_HLK || arcn->type == PAX_HRG)) {
if (arcn->ln_name[1] == '\0') {
arcn->ln_name[0] = '.';
} else {
(void)memmove(arcn->ln_name, &arcn->ln_name[1],
strlen(arcn->ln_name));
arcn->ln_nlen--;
}
if (rmleadslash < 2) {
rmleadslash = 2;
paxwarn(0, "Removing leading / from absolute path names in the archive");
}
}




















if (rephead != NULL) {




if ((res = rep_name(arcn->name, &(arcn->nlen), 1)) != 0)
return(res);

if (((arcn->type == PAX_SLK) || (arcn->type == PAX_HLK) ||
(arcn->type == PAX_HRG)) &&
((res = rep_name(arcn->ln_name, &(arcn->ln_nlen), 0)) != 0))
return(res);
}

if (iflag) {



if ((res = tty_rename(arcn)) != 0)
return(res);
if ((arcn->type == PAX_SLK) || (arcn->type == PAX_HLK) ||
(arcn->type == PAX_HRG))
sub_name(arcn->ln_name, &(arcn->ln_nlen), sizeof(arcn->ln_name));
}
return(res);
}











static int
tty_rename(ARCHD *arcn)
{
char tmpname[PAXPATHLEN+2];
int res;







tty_prnt("\nATTENTION: %s interactive file rename operation.\n", argv0);

for (;;) {
ls_tty(arcn);
tty_prnt("Input new name, or a \".\" to keep the old name, ");
tty_prnt("or a \"return\" to skip this file.\n");
tty_prnt("Input > ");
if (tty_read(tmpname, sizeof(tmpname)) < 0)
return(-1);
if (strcmp(tmpname, "..") == 0) {
tty_prnt("Try again, illegal file name: ..\n");
continue;
}
if (strlen(tmpname) > PAXPATHLEN) {
tty_prnt("Try again, file name too long\n");
continue;
}
break;
}




if (tmpname[0] == '\0') {
tty_prnt("Skipping file.\n");
return(1);
}
if ((tmpname[0] == '.') && (tmpname[1] == '\0')) {
tty_prnt("Processing continues, name unchanged.\n");
return(0);
}






tty_prnt("Processing continues, name changed to: %s\n", tmpname);
res = add_name(arcn->name, arcn->nlen, tmpname);
arcn->nlen = l_strncpy(arcn->name, tmpname, sizeof(arcn->name) - 1);
arcn->name[arcn->nlen] = '\0';
if (res < 0)
return(-1);
return(0);
}









int
set_dest(ARCHD *arcn, char *dest_dir, int dir_len)
{
if (fix_path(arcn->name, &(arcn->nlen), dest_dir, dir_len) < 0)
return(-1);






if ((arcn->type != PAX_HLK) && (arcn->type != PAX_HRG))
return(0);

if (fix_path(arcn->ln_name, &(arcn->ln_nlen), dest_dir, dir_len) < 0)
return(-1);
return(0);
}









static int
fix_path( char *or_name, int *or_len, char *dir_name, int dir_len)
{
char *src;
char *dest;
char *start;
int len;







start = or_name;
src = start + *or_len;
dest = src + dir_len;
if (*start == '/') {
++start;
--dest;
}
if ((len = dest - or_name) > PAXPATHLEN) {
paxwarn(1, "File name %s/%s, too long", dir_name, start);
return(-1);
}
*or_len = len;




while (src >= start)
*dest-- = *src--;
src = dir_name + dir_len - 1;




while (src >= dir_name)
*dest-- = *src--;

*(or_name + len) = '\0';
return(0);
}




















static int
rep_name(char *name, int *nlen, int prnt)
{
REPLACE *pt;
char *inpt;
char *outpt;
char *endpt;
char *rpt;
int found = 0;
int res;
#if !defined(NET2_REGEX)
regmatch_t pm[MAXSUBEXP];
#endif
char nname[PAXPATHLEN+1];
char buf1[PAXPATHLEN+1];









pt = rephead;
(void)strlcpy(buf1, name, sizeof(buf1));
inpt = buf1;
outpt = nname;
endpt = outpt + PAXPATHLEN;




while (pt != NULL) {
do {




#if defined(NET2_REGEX)
if (regexec(pt->rcmp, inpt) == 0)
#else
if (regexec(&(pt->rcmp), inpt, MAXSUBEXP, pm, 0) != 0)
#endif
break;








found = 1;
#if defined(NET2_REGEX)
rpt = pt->rcmp->startp[0];
#else
rpt = inpt + pm[0].rm_so;
#endif

while ((inpt < rpt) && (outpt < endpt))
*outpt++ = *inpt++;
if (outpt == endpt)
break;







#if defined(NET2_REGEX)
if ((res = resub(pt->rcmp,pt->nstr,outpt,endpt)) < 0) {
#else
if ((res = resub(&(pt->rcmp),pm,inpt,pt->nstr,outpt,endpt))
< 0) {
#endif
if (prnt)
paxwarn(1, "Replacement name error %s",
name);
return(1);
}
outpt += res;











#if defined(NET2_REGEX)
inpt = pt->rcmp->endp[0];
#else
inpt += pm[0].rm_eo - pm[0].rm_so;
#endif

if ((outpt == endpt) || (*inpt == '\0'))
break;





} while (pt->flgs & GLOB);

if (found)
break;




pt = pt->fow;
}

if (found) {




while ((outpt < endpt) && (*inpt != '\0'))
*outpt++ = *inpt++;

*outpt = '\0';
if ((outpt == endpt) && (*inpt != '\0')) {
if (prnt)
paxwarn(1,"Replacement name too long %s >> %s",
name, nname);
return(1);
}




if (prnt && (pt->flgs & PRNT)) {
if (*nname == '\0')
(void)fprintf(stderr,"%s >> <empty string>\n",
name);
else
(void)fprintf(stderr,"%s >> %s\n", name, nname);
}





if (*nname == '\0')
return(1);
*nlen = l_strncpy(name, nname, PAXPATHLEN + 1);
name[PAXPATHLEN] = '\0';
}
return(0);
}

#if defined(NET2_REGEX)








static int
resub(regexp *prog, char *src, char *dest, char *destend)
{
char *spt;
char *dpt;
char c;
int no;
int len;

spt = src;
dpt = dest;
while ((dpt < destend) && ((c = *spt++) != '\0')) {
if (c == '&')
no = 0;
else if ((c == '\\') && (*spt >= '0') && (*spt <= '9'))
no = *spt++ - '0';
else {
if ((c == '\\') && ((*spt == '\\') || (*spt == '&')))
c = *spt++;
*dpt++ = c;
continue;
}
if ((prog->startp[no] == NULL) || (prog->endp[no] == NULL) ||
((len = prog->endp[no] - prog->startp[no]) <= 0))
continue;





if (len > (destend - dpt))
len = destend - dpt;
if (l_strncpy(dpt, prog->startp[no], len) != len)
return(-1);
dpt += len;
}
return(dpt - dest);
}

#else









static int
resub(regex_t *rp, regmatch_t *pm, char *orig, char *src, char *dest,
char *destend)
{
char *spt;
char *dpt;
char c;
regmatch_t *pmpt;
int len;
int subexcnt;

spt = src;
dpt = dest;
subexcnt = rp->re_nsub;
while ((dpt < destend) && ((c = *spt++) != '\0')) {




if (c == '&') {
pmpt = pm;
} else if ((c == '\\') && (*spt >= '0') && (*spt <= '9')) {



if ((len = *spt++ - '0') > subexcnt)
return(-1);
pmpt = pm + len;
} else {



if ((c == '\\') && ((*spt == '\\') || (*spt == '&')))
c = *spt++;
*dpt++ = c;
continue;
}




if ((pmpt->rm_so < 0) || (pmpt->rm_eo < 0) ||
((len = pmpt->rm_eo - pmpt->rm_so) <= 0))
continue;





if (len > (destend - dpt))
len = destend - dpt;
if (l_strncpy(dpt, orig + pmpt->rm_so, len) != len)
return(-1);
dpt += len;
}
return(dpt - dest);
}
#endif
