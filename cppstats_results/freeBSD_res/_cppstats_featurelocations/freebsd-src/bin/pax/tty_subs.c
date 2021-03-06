


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)tty_subs.c 8.2 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pax.h"
#include "extern.h"
#include <stdarg.h>





#define DEVTTY "/dev/tty"
static FILE *ttyoutf = NULL;
static FILE *ttyinf = NULL;







int
tty_init(void)
{
int ttyfd;

if ((ttyfd = open(DEVTTY, O_RDWR)) >= 0) {
if ((ttyoutf = fdopen(ttyfd, "w")) != NULL) {
if ((ttyinf = fdopen(ttyfd, "r")) != NULL)
return(0);
(void)fclose(ttyoutf);
}
(void)close(ttyfd);
}

if (iflag) {
paxwarn(1, "Fatal error, cannot open %s", DEVTTY);
return(-1);
}
return(0);
}







void
tty_prnt(const char *fmt, ...)
{
va_list ap;
if (ttyoutf == NULL)
return;
va_start(ap, fmt);
(void)vfprintf(ttyoutf, fmt, ap);
va_end(ap);
(void)fflush(ttyoutf);
}









int
tty_read(char *str, int len)
{
char *pt;

if ((--len <= 0) || (ttyinf == NULL) || (fgets(str,len,ttyinf) == NULL))
return(-1);
*(str + len) = '\0';




if ((pt = strchr(str, '\n')) != NULL)
*pt = '\0';
return(0);
}







void
paxwarn(int set, const char *fmt, ...)
{
va_list ap;
va_start(ap, fmt);
if (set)
exit_val = 1;




if (vflag && vfpart) {
(void)fflush(listf);
(void)fputc('\n', stderr);
vfpart = 0;
}
(void)fprintf(stderr, "%s: ", argv0);
(void)vfprintf(stderr, fmt, ap);
va_end(ap);
(void)fputc('\n', stderr);
}







void
syswarn(int set, int errnum, const char *fmt, ...)
{
va_list ap;
va_start(ap, fmt);
if (set)
exit_val = 1;




if (vflag && vfpart) {
(void)fflush(listf);
(void)fputc('\n', stderr);
vfpart = 0;
}
(void)fprintf(stderr, "%s: ", argv0);
(void)vfprintf(stderr, fmt, ap);
va_end(ap);




if (errnum > 0)
(void)fprintf(stderr, " <%s>", strerror(errnum));
(void)fputc('\n', stderr);
}
