































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)mail.c 8.2 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");





#include "shell.h"
#include "mail.h"
#include "var.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>


#define MAXMBOXES 10


static int nmboxes;
static time_t mailtime[MAXMBOXES];









void
chkmail(int silent)
{
int i;
char *mpath;
char *p;
char *msg;
struct stackmark smark;
struct stat statb;

if (silent)
nmboxes = 10;
if (nmboxes == 0)
return;
setstackmark(&smark);
mpath = stsavestr(mpathset()? mpathval() : mailval());
for (i = 0 ; i < nmboxes ; i++) {
p = mpath;
if (*p == '\0')
break;
mpath = strchrnul(mpath, ':');
if (*mpath != '\0') {
*mpath++ = '\0';
if (p == mpath - 1)
continue;
}
msg = strchr(p, '%');
if (msg != NULL)
*msg++ = '\0';
#if defined(notdef)
if (stat(p, &statb) < 0)
statb.st_mtime = 0;
if (statb.st_mtime > mailtime[i] && ! silent) {
out2str(msg? msg : "you have mail");
out2c('\n');
}
mailtime[i] = statb.st_mtime;
#else
if (stat(p, &statb) < 0)
statb.st_size = 0;
if (statb.st_size > mailtime[i] && ! silent) {
out2str(msg? msg : "you have mail");
out2c('\n');
}
mailtime[i] = statb.st_size;
#endif
}
nmboxes = i;
popstackmark(&smark);
}
