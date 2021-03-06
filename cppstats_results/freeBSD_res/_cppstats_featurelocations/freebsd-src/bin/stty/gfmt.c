




























#if !defined(lint)
#if 0
static char sccsid[] = "@(#)gfmt.c 8.6 (Berkeley) 4/2/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stty.h"
#include "extern.h"

static void gerr(const char *s) __dead2;

static void
gerr(const char *s)
{
if (s)
errx(1, "illegal gfmt1 option -- %s", s);
else
errx(1, "illegal gfmt1 option");
}

void
gprint(struct termios *tp, struct winsize *wp __unused, int ldisc __unused)
{
struct cchar *cp;

(void)printf("gfmt1:cflag=%lx:iflag=%lx:lflag=%lx:oflag=%lx:",
(u_long)tp->c_cflag, (u_long)tp->c_iflag, (u_long)tp->c_lflag,
(u_long)tp->c_oflag);
for (cp = cchars1; cp->name; ++cp)
(void)printf("%s=%x:", cp->name, tp->c_cc[cp->sub]);
(void)printf("ispeed=%lu:ospeed=%lu\n",
(u_long)cfgetispeed(tp), (u_long)cfgetospeed(tp));
}

void
gread(struct termios *tp, char *s)
{
struct cchar *cp;
char *ep, *p;
long tmp;

if ((s = strchr(s, ':')) == NULL)
gerr(NULL);
for (++s; s != NULL;) {
p = strsep(&s, ":\0");
if (!p || !*p)
break;
if (!(ep = strchr(p, '=')))
gerr(p);
*ep++ = '\0';
tmp = strtoul(ep, NULL, 0x10);

#define CHK(s) (*p == s[0] && !strcmp(p, s))
if (CHK("cflag")) {
tp->c_cflag = tmp;
continue;
}
if (CHK("iflag")) {
tp->c_iflag = tmp;
continue;
}
if (CHK("ispeed")) {
tmp = strtoul(ep, NULL, 10);
tp->c_ispeed = tmp;
continue;
}
if (CHK("lflag")) {
tp->c_lflag = tmp;
continue;
}
if (CHK("oflag")) {
tp->c_oflag = tmp;
continue;
}
if (CHK("ospeed")) {
tmp = strtoul(ep, NULL, 10);
tp->c_ospeed = tmp;
continue;
}
for (cp = cchars1; cp->name != NULL; ++cp)
if (CHK(cp->name)) {
if (cp->sub == VMIN || cp->sub == VTIME)
tmp = strtoul(ep, NULL, 10);
tp->c_cc[cp->sub] = tmp;
break;
}
if (cp->name == NULL)
gerr(p);
}
}
