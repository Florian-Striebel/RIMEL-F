


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)gen_subs.c 8.1 (Berkeley) 5/31/93";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <langinfo.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pax.h"
#include "extern.h"








#define MODELEN 20
#define DATELEN 64
#define SIXMONTHS ((365 / 2) * 86400)
#define CURFRMTM "%b %e %H:%M"
#define OLDFRMTM "%b %e %Y"
#define CURFRMTD "%e %b %H:%M"
#define OLDFRMTD "%e %b %Y"

static int d_first = -1;






void
ls_list(ARCHD *arcn, time_t now, FILE *fp)
{
struct stat *sbp;
char f_mode[MODELEN];
char f_date[DATELEN];
const char *timefrmt;




if (!vflag) {
(void)fprintf(fp, "%s\n", arcn->name);
(void)fflush(fp);
return;
}

if (d_first < 0)
d_first = (*nl_langinfo(D_MD_ORDER) == 'd');



sbp = &(arcn->sb);
strmode(sbp->st_mode, f_mode);




if ((sbp->st_mtime + SIXMONTHS) <= now)
timefrmt = d_first ? OLDFRMTD : OLDFRMTM;
else
timefrmt = d_first ? CURFRMTD : CURFRMTM;




if (strftime(f_date,DATELEN,timefrmt,localtime(&(sbp->st_mtime))) == 0)
f_date[0] = '\0';
(void)fprintf(fp, "%s%2ju %-12s %-12s ", f_mode,
(uintmax_t)sbp->st_nlink,
name_uid(sbp->st_uid, 1), name_gid(sbp->st_gid, 1));




if ((arcn->type == PAX_CHR) || (arcn->type == PAX_BLK))
#if defined(NET2_STAT)
(void)fprintf(fp, "%4u,%4u ", MAJOR(sbp->st_rdev),
MINOR(sbp->st_rdev));
#else
(void)fprintf(fp, "%4lu,%4lu ", (unsigned long)MAJOR(sbp->st_rdev),
(unsigned long)MINOR(sbp->st_rdev));
#endif
else {
#if defined(NET2_STAT)
(void)fprintf(fp, "%9lu ", sbp->st_size);
#else
(void)fprintf(fp, "%9ju ", (uintmax_t)sbp->st_size);
#endif
}




(void)fprintf(fp, "%s %s", f_date, arcn->name);
if ((arcn->type == PAX_HLK) || (arcn->type == PAX_HRG))
(void)fprintf(fp, " == %s\n", arcn->ln_name);
else if (arcn->type == PAX_SLK)
(void)fprintf(fp, " => %s\n", arcn->ln_name);
else
(void)putc('\n', fp);
(void)fflush(fp);
return;
}






void
ls_tty(ARCHD *arcn)
{
char f_date[DATELEN];
char f_mode[MODELEN];
const char *timefrmt;

if (d_first < 0)
d_first = (*nl_langinfo(D_MD_ORDER) == 'd');

if ((arcn->sb.st_mtime + SIXMONTHS) <= time(NULL))
timefrmt = d_first ? OLDFRMTD : OLDFRMTM;
else
timefrmt = d_first ? CURFRMTD : CURFRMTM;




if (strftime(f_date, DATELEN, timefrmt,
localtime(&(arcn->sb.st_mtime))) == 0)
f_date[0] = '\0';
strmode(arcn->sb.st_mode, f_mode);
tty_prnt("%s%s %s\n", f_mode, f_date, arcn->name);
return;
}










int
l_strncpy(char *dest, const char *src, int len)
{
char *stop;
char *start;

stop = dest + len;
start = dest;
while ((dest < stop) && (*src != '\0'))
*dest++ = *src++;
len = dest - start;
while (dest < stop)
*dest++ = '\0';
return(len);
}











u_long
asc_ul(char *str, int len, int base)
{
char *stop;
u_long tval = 0;

stop = str + len;




while ((str < stop) && ((*str == ' ') || (*str == '0')))
++str;





if (base == HEX) {
while (str < stop) {
if ((*str >= '0') && (*str <= '9'))
tval = (tval << 4) + (*str++ - '0');
else if ((*str >= 'A') && (*str <= 'F'))
tval = (tval << 4) + 10 + (*str++ - 'A');
else if ((*str >= 'a') && (*str <= 'f'))
tval = (tval << 4) + 10 + (*str++ - 'a');
else
break;
}
} else {
while ((str < stop) && (*str >= '0') && (*str <= '7'))
tval = (tval << 3) + (*str++ - '0');
}
return(tval);
}








int
ul_asc(u_long val, char *str, int len, int base)
{
char *pt;
u_long digit;




pt = str + len - 1;






if (base == HEX) {
while (pt >= str) {
if ((digit = (val & 0xf)) < 10)
*pt-- = '0' + (char)digit;
else
*pt-- = 'a' + (char)(digit - 10);
if ((val = (val >> 4)) == (u_long)0)
break;
}
} else {
while (pt >= str) {
*pt-- = '0' + (char)(val & 0x7);
if ((val = (val >> 3)) == (u_long)0)
break;
}
}




while (pt >= str)
*pt-- = '0';
if (val != (u_long)0)
return(-1);
return(0);
}

#if !defined(NET2_STAT)










u_quad_t
asc_uqd(char *str, int len, int base)
{
char *stop;
u_quad_t tval = 0;

stop = str + len;




while ((str < stop) && ((*str == ' ') || (*str == '0')))
++str;





if (base == HEX) {
while (str < stop) {
if ((*str >= '0') && (*str <= '9'))
tval = (tval << 4) + (*str++ - '0');
else if ((*str >= 'A') && (*str <= 'F'))
tval = (tval << 4) + 10 + (*str++ - 'A');
else if ((*str >= 'a') && (*str <= 'f'))
tval = (tval << 4) + 10 + (*str++ - 'a');
else
break;
}
} else {
while ((str < stop) && (*str >= '0') && (*str <= '7'))
tval = (tval << 3) + (*str++ - '0');
}
return(tval);
}








int
uqd_asc(u_quad_t val, char *str, int len, int base)
{
char *pt;
u_quad_t digit;




pt = str + len - 1;






if (base == HEX) {
while (pt >= str) {
if ((digit = (val & 0xf)) < 10)
*pt-- = '0' + (char)digit;
else
*pt-- = 'a' + (char)(digit - 10);
if ((val = (val >> 4)) == (u_quad_t)0)
break;
}
} else {
while (pt >= str) {
*pt-- = '0' + (char)(val & 0x7);
if ((val = (val >> 3)) == (u_quad_t)0)
break;
}
}




while (pt >= str)
*pt-- = '0';
if (val != (u_quad_t)0)
return(-1);
return(0);
}
#endif
