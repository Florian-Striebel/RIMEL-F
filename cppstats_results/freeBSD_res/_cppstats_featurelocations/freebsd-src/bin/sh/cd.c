































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)cd.c 8.2 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>





#include "shell.h"
#include "var.h"
#include "nodes.h"
#include "jobs.h"
#include "options.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "exec.h"
#include "redir.h"
#include "mystring.h"
#include "show.h"
#include "cd.h"
#include "builtins.h"

static int cdlogical(char *);
static int cdphysical(char *);
static int docd(char *, int, int);
static char *getcomponent(char **);
static char *findcwd(char *);
static void updatepwd(char *);
static char *getpwd(void);
static char *getpwd2(void);

static char *curdir = NULL;

int
cdcmd(int argc __unused, char **argv __unused)
{
const char *dest;
const char *path;
char *p;
struct stat statb;
int ch, phys, print = 0, getcwderr = 0;
int rc;
int errno1 = ENOENT;

phys = Pflag;
while ((ch = nextopt("eLP")) != '\0') {
switch (ch) {
case 'e':
getcwderr = 1;
break;
case 'L':
phys = 0;
break;
case 'P':
phys = 1;
break;
}
}

if (*argptr != NULL && argptr[1] != NULL)
error("too many arguments");

if ((dest = *argptr) == NULL && (dest = bltinlookup("HOME", 1)) == NULL)
error("HOME not set");
if (*dest == '\0')
dest = ".";
if (dest[0] == '-' && dest[1] == '\0') {
dest = bltinlookup("OLDPWD", 1);
if (dest == NULL)
error("OLDPWD not set");
print = 1;
}
if (dest[0] == '/' ||
(dest[0] == '.' && (dest[1] == '/' || dest[1] == '\0')) ||
(dest[0] == '.' && dest[1] == '.' && (dest[2] == '/' || dest[2] == '\0')) ||
(path = bltinlookup("CDPATH", 1)) == NULL)
path = "";
while ((p = padvance(&path, NULL, dest)) != NULL) {
if (stat(p, &statb) < 0) {
if (errno != ENOENT)
errno1 = errno;
} else if (!S_ISDIR(statb.st_mode))
errno1 = ENOTDIR;
else {
if (!print) {



if (p[0] == '.' && p[1] == '/' && p[2] != '\0')
print = strcmp(p + 2, dest);
else
print = strcmp(p, dest);
}
rc = docd(p, print, phys);
if (rc >= 0)
return getcwderr ? rc : 0;
if (errno != ENOENT)
errno1 = errno;
}
}
error("%s: %s", dest, strerror(errno1));

return 0;
}






static int
docd(char *dest, int print, int phys)
{
int rc;

TRACE(("docd(\"%s\", %d, %d) called\n", dest, print, phys));


if ((phys || (rc = cdlogical(dest)) < 0) && (rc = cdphysical(dest)) < 0)
return (-1);

if (print && iflag && curdir) {
out1fmt("%s\n", curdir);






flushout(out1);
outclearerror(out1);
}

return (rc);
}

static int
cdlogical(char *dest)
{
char *p;
char *q;
char *component;
char *path;
struct stat statb;
int first;
int badstat;






badstat = 0;
path = stsavestr(dest);
STARTSTACKSTR(p);
if (*dest == '/') {
STPUTC('/', p);
path++;
}
first = 1;
while ((q = getcomponent(&path)) != NULL) {
if (q[0] == '\0' || (q[0] == '.' && q[1] == '\0'))
continue;
if (! first)
STPUTC('/', p);
first = 0;
component = q;
STPUTS(q, p);
if (equal(component, ".."))
continue;
STACKSTRNUL(p);
if (lstat(stackblock(), &statb) < 0) {
badstat = 1;
break;
}
}

INTOFF;
if ((p = findcwd(badstat ? NULL : dest)) == NULL || chdir(p) < 0) {
INTON;
return (-1);
}
updatepwd(p);
INTON;
return (0);
}

static int
cdphysical(char *dest)
{
char *p;
int rc = 0;

INTOFF;
if (chdir(dest) < 0) {
INTON;
return (-1);
}
p = findcwd(NULL);
if (p == NULL) {
warning("warning: failed to get name of current directory");
rc = 1;
}
updatepwd(p);
INTON;
return (rc);
}





static char *
getcomponent(char **path)
{
char *p;
char *start;

if ((p = *path) == NULL)
return NULL;
start = *path;
while (*p != '/' && *p != '\0')
p++;
if (*p == '\0') {
*path = NULL;
} else {
*p++ = '\0';
*path = p;
}
return start;
}


static char *
findcwd(char *dir)
{
char *new;
char *p;
char *path;






if (dir == NULL || curdir == NULL)
return getpwd2();
path = stsavestr(dir);
STARTSTACKSTR(new);
if (*dir != '/') {
STPUTS(curdir, new);
if (STTOPC(new) == '/')
STUNPUTC(new);
}
while ((p = getcomponent(&path)) != NULL) {
if (equal(p, "..")) {
while (new > stackblock() && (STUNPUTC(new), *new) != '/');
} else if (*p != '\0' && ! equal(p, ".")) {
STPUTC('/', new);
STPUTS(p, new);
}
}
if (new == stackblock())
STPUTC('/', new);
STACKSTRNUL(new);
return stackblock();
}






static void
updatepwd(char *dir)
{
char *prevdir;

hashcd();

setvar("PWD", dir, VEXPORT);
setvar("OLDPWD", curdir, VEXPORT);
prevdir = curdir;
curdir = dir ? savestr(dir) : NULL;
ckfree(prevdir);
}

int
pwdcmd(int argc __unused, char **argv __unused)
{
char *p;
int ch, phys;

phys = Pflag;
while ((ch = nextopt("LP")) != '\0') {
switch (ch) {
case 'L':
phys = 0;
break;
case 'P':
phys = 1;
break;
}
}

if (*argptr != NULL)
error("too many arguments");

if (!phys && getpwd()) {
out1str(curdir);
out1c('\n');
} else {
if ((p = getpwd2()) == NULL)
error(".: %s", strerror(errno));
out1str(p);
out1c('\n');
}

return 0;
}




static char *
getpwd(void)
{
char *p;

if (curdir)
return curdir;

p = getpwd2();
if (p != NULL) {
INTOFF;
curdir = savestr(p);
INTON;
}

return curdir;
}

#define MAXPWD 256




static char *
getpwd2(void)
{
char *pwd;
int i;

for (i = MAXPWD;; i *= 2) {
pwd = stalloc(i);
if (getcwd(pwd, i) != NULL)
return pwd;
stunalloc(pwd);
if (errno != ERANGE)
break;
}

return NULL;
}





void
pwd_init(int warn)
{
char *pwd;
struct stat stdot, stpwd;

pwd = lookupvar("PWD");
if (pwd && *pwd == '/' && stat(".", &stdot) != -1 &&
stat(pwd, &stpwd) != -1 &&
stdot.st_dev == stpwd.st_dev &&
stdot.st_ino == stpwd.st_ino) {
if (curdir)
ckfree(curdir);
curdir = savestr(pwd);
}
if (getpwd() == NULL && warn)
out2fmt_flush("sh: cannot determine working directory\n");
setvar("PWD", curdir, VEXPORT);
}
