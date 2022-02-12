































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)exec.c 8.4 (Berkeley) 6/8/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <paths.h>
#include <stdbool.h>
#include <stdlib.h>










#include "shell.h"
#include "main.h"
#include "nodes.h"
#include "parser.h"
#include "redir.h"
#include "eval.h"
#include "exec.h"
#include "builtins.h"
#include "var.h"
#include "options.h"
#include "input.h"
#include "output.h"
#include "syntax.h"
#include "memalloc.h"
#include "error.h"
#include "mystring.h"
#include "show.h"
#include "jobs.h"
#include "alias.h"


#define CMDTABLESIZE 31



struct tblentry {
struct tblentry *next;
union param param;
int special;
signed char cmdtype;
char cmdname[];
};


static struct tblentry *cmdtable[CMDTABLESIZE];
static int cmdtable_cd = 0;


static void tryexec(char *, char **, char **);
static void printentry(struct tblentry *, int);
static struct tblentry *cmdlookup(const char *, int);
static void delete_cmd_entry(void);
static void addcmdentry(const char *, struct cmdentry *);










void
shellexec(char **argv, char **envp, const char *path, int idx)
{
char *cmdname;
const char *opt;
int e;

if (strchr(argv[0], '/') != NULL) {
tryexec(argv[0], argv, envp);
e = errno;
} else {
e = ENOENT;
while ((cmdname = padvance(&path, &opt, argv[0])) != NULL) {
if (--idx < 0 && opt == NULL) {
tryexec(cmdname, argv, envp);
if (errno != ENOENT && errno != ENOTDIR)
e = errno;
if (e == ENOEXEC)
break;
}
stunalloc(cmdname);
}
}


if (e == ENOENT || e == ENOTDIR)
errorwithstatus(127, "%s: not found", argv[0]);
else
errorwithstatus(126, "%s: %s", argv[0], strerror(e));
}


static bool
isbinary(const char *data, size_t len)
{
const char *nul, *p;
bool hasletter;

nul = memchr(data, '\0', len);
if (nul == NULL)
return false;











hasletter = false;
for (p = data; *p != '\0'; p++) {
if ((*p >= 'a' && *p <= 'z') || *p == '$' || *p == '`')
hasletter = true;
if (hasletter && *p == '\n')
return false;
}
return true;
}


static void
tryexec(char *cmd, char **argv, char **envp)
{
int e, in;
ssize_t n;
char buf[256];

execve(cmd, argv, envp);
e = errno;
if (e == ENOEXEC) {
INTOFF;
in = open(cmd, O_RDONLY | O_NONBLOCK);
if (in != -1) {
n = pread(in, buf, sizeof buf, 0);
close(in);
if (n > 0 && isbinary(buf, n)) {
errno = ENOEXEC;
return;
}
}
*argv = cmd;
*--argv = __DECONST(char *, _PATH_BSHELL);
execve(_PATH_BSHELL, argv, envp);
}
errno = e;
}











char *
padvance(const char **path, const char **popt, const char *name)
{
const char *p, *start;
char *q;
size_t len, namelen;

if (*path == NULL)
return NULL;
start = *path;
if (popt != NULL)
for (p = start; *p && *p != ':' && *p != '%'; p++)
;
else
for (p = start; *p && *p != ':'; p++)
;
namelen = strlen(name);
len = p - start + namelen + 2;
STARTSTACKSTR(q);
CHECKSTRSPACE(len, q);
if (p != start) {
memcpy(q, start, p - start);
q += p - start;
*q++ = '/';
}
memcpy(q, name, namelen + 1);
if (popt != NULL) {
if (*p == '%') {
*popt = ++p;
while (*p && *p != ':') p++;
} else
*popt = NULL;
}
if (*p == ':')
*path = p + 1;
else
*path = NULL;
return stalloc(len);
}






int
hashcmd(int argc __unused, char **argv __unused)
{
struct tblentry **pp;
struct tblentry *cmdp;
int c;
int verbose;
struct cmdentry entry;
char *name;
int errors;

errors = 0;
verbose = 0;
while ((c = nextopt("rv")) != '\0') {
if (c == 'r') {
clearcmdentry();
} else if (c == 'v') {
verbose++;
}
}
if (*argptr == NULL) {
for (pp = cmdtable ; pp < &cmdtable[CMDTABLESIZE] ; pp++) {
for (cmdp = *pp ; cmdp ; cmdp = cmdp->next) {
if (cmdp->cmdtype == CMDNORMAL)
printentry(cmdp, verbose);
}
}
return 0;
}
while ((name = *argptr) != NULL) {
if ((cmdp = cmdlookup(name, 0)) != NULL
&& cmdp->cmdtype == CMDNORMAL)
delete_cmd_entry();
find_command(name, &entry, DO_ERR, pathval());
if (entry.cmdtype == CMDUNKNOWN)
errors = 1;
else if (verbose) {
cmdp = cmdlookup(name, 0);
if (cmdp != NULL)
printentry(cmdp, verbose);
else {
outfmt(out2, "%s: not found\n", name);
errors = 1;
}
flushall();
}
argptr++;
}
return errors;
}


static void
printentry(struct tblentry *cmdp, int verbose)
{
int idx;
const char *path, *opt;
char *name;

if (cmdp->cmdtype == CMDNORMAL) {
idx = cmdp->param.index;
path = pathval();
do {
name = padvance(&path, &opt, cmdp->cmdname);
stunalloc(name);
} while (--idx >= 0);
out1str(name);
} else if (cmdp->cmdtype == CMDBUILTIN) {
out1fmt("builtin %s", cmdp->cmdname);
} else if (cmdp->cmdtype == CMDFUNCTION) {
out1fmt("function %s", cmdp->cmdname);
if (verbose) {
INTOFF;
name = commandtext(getfuncnode(cmdp->param.func));
out1c(' ');
out1str(name);
ckfree(name);
INTON;
}
#if defined(DEBUG)
} else {
error("internal error: cmdtype %d", cmdp->cmdtype);
#endif
}
out1c('\n');
}








void
find_command(const char *name, struct cmdentry *entry, int act,
const char *path)
{
struct tblentry *cmdp, loc_cmd;
int idx;
const char *opt;
char *fullname;
struct stat statb;
int e;
int i;
int spec;
int cd;


if (strchr(name, '/') != NULL) {
entry->cmdtype = CMDNORMAL;
entry->u.index = 0;
entry->special = 0;
return;
}

cd = 0;


if ((cmdp = cmdlookup(name, 0)) != NULL) {
if (cmdp->cmdtype == CMDFUNCTION && act & DO_NOFUNC)
cmdp = NULL;
else
goto success;
}


if ((i = find_builtin(name, &spec)) >= 0) {
INTOFF;
cmdp = cmdlookup(name, 1);
if (cmdp->cmdtype == CMDFUNCTION)
cmdp = &loc_cmd;
cmdp->cmdtype = CMDBUILTIN;
cmdp->param.index = i;
cmdp->special = spec;
INTON;
goto success;
}



e = ENOENT;
idx = -1;
for (;(fullname = padvance(&path, &opt, name)) != NULL;
stunalloc(fullname)) {
idx++;
if (opt) {
if (strncmp(opt, "func", 4) == 0) {

} else {
continue;
}
}
if (fullname[0] != '/')
cd = 1;
if (stat(fullname, &statb) < 0) {
if (errno != ENOENT && errno != ENOTDIR)
e = errno;
continue;
}
e = EACCES;
if (!S_ISREG(statb.st_mode))
continue;
if (opt) {
readcmdfile(fullname, -1 );
if ((cmdp = cmdlookup(name, 0)) == NULL || cmdp->cmdtype != CMDFUNCTION)
error("%s not defined in %s", name, fullname);
stunalloc(fullname);
goto success;
}
#if defined(notdef)
if (statb.st_uid == geteuid()) {
if ((statb.st_mode & 0100) == 0)
goto loop;
} else if (statb.st_gid == getegid()) {
if ((statb.st_mode & 010) == 0)
goto loop;
} else {
if ((statb.st_mode & 01) == 0)
goto loop;
}
#endif
TRACE(("searchexec \"%s\" returns \"%s\"\n", name, fullname));
INTOFF;
stunalloc(fullname);
cmdp = cmdlookup(name, 1);
if (cmdp->cmdtype == CMDFUNCTION)
cmdp = &loc_cmd;
cmdp->cmdtype = CMDNORMAL;
cmdp->param.index = idx;
cmdp->special = 0;
INTON;
goto success;
}

if (act & DO_ERR) {
if (e == ENOENT || e == ENOTDIR)
outfmt(out2, "%s: not found\n", name);
else
outfmt(out2, "%s: %s\n", name, strerror(e));
}
entry->cmdtype = CMDUNKNOWN;
entry->u.index = 0;
entry->special = 0;
return;

success:
if (cd)
cmdtable_cd = 1;
entry->cmdtype = cmdp->cmdtype;
entry->u = cmdp->param;
entry->special = cmdp->special;
}







int
find_builtin(const char *name, int *special)
{
const unsigned char *bp;
size_t len;

len = strlen(name);
for (bp = builtincmd ; *bp ; bp += 2 + bp[0]) {
if (bp[0] == len && memcmp(bp + 2, name, len) == 0) {
*special = (bp[1] & BUILTIN_SPECIAL) != 0;
return bp[1] & ~BUILTIN_SPECIAL;
}
}
return -1;
}








void
hashcd(void)
{
if (cmdtable_cd)
clearcmdentry();
}









void
changepath(const char *newval __unused)
{
clearcmdentry();
}






void
clearcmdentry(void)
{
struct tblentry **tblp;
struct tblentry **pp;
struct tblentry *cmdp;

INTOFF;
for (tblp = cmdtable ; tblp < &cmdtable[CMDTABLESIZE] ; tblp++) {
pp = tblp;
while ((cmdp = *pp) != NULL) {
if (cmdp->cmdtype == CMDNORMAL) {
*pp = cmdp->next;
ckfree(cmdp);
} else {
pp = &cmdp->next;
}
}
}
cmdtable_cd = 0;
INTON;
}










static struct tblentry **lastcmdentry;


static struct tblentry *
cmdlookup(const char *name, int add)
{
unsigned int hashval;
const char *p;
struct tblentry *cmdp;
struct tblentry **pp;
size_t len;

p = name;
hashval = (unsigned char)*p << 4;
while (*p)
hashval += *p++;
pp = &cmdtable[hashval % CMDTABLESIZE];
for (cmdp = *pp ; cmdp ; cmdp = cmdp->next) {
if (equal(cmdp->cmdname, name))
break;
pp = &cmdp->next;
}
if (add && cmdp == NULL) {
INTOFF;
len = strlen(name);
cmdp = *pp = ckmalloc(sizeof (struct tblentry) + len + 1);
cmdp->next = NULL;
cmdp->cmdtype = CMDUNKNOWN;
memcpy(cmdp->cmdname, name, len + 1);
INTON;
}
lastcmdentry = pp;
return cmdp;
}





static void
delete_cmd_entry(void)
{
struct tblentry *cmdp;

INTOFF;
cmdp = *lastcmdentry;
*lastcmdentry = cmdp->next;
ckfree(cmdp);
INTON;
}








static void
addcmdentry(const char *name, struct cmdentry *entry)
{
struct tblentry *cmdp;

INTOFF;
cmdp = cmdlookup(name, 1);
if (cmdp->cmdtype == CMDFUNCTION) {
unreffunc(cmdp->param.func);
}
cmdp->cmdtype = entry->cmdtype;
cmdp->param = entry->u;
cmdp->special = entry->special;
INTON;
}






void
defun(const char *name, union node *func)
{
struct cmdentry entry;

INTOFF;
entry.cmdtype = CMDFUNCTION;
entry.u.func = copyfunc(func);
entry.special = 0;
addcmdentry(name, &entry);
INTON;
}







int
unsetfunc(const char *name)
{
struct tblentry *cmdp;

if ((cmdp = cmdlookup(name, 0)) != NULL && cmdp->cmdtype == CMDFUNCTION) {
unreffunc(cmdp->param.func);
delete_cmd_entry();
return (0);
}
return (0);
}





int
isfunc(const char *name)
{
struct tblentry *cmdp;
cmdp = cmdlookup(name, 0);
return (cmdp != NULL && cmdp->cmdtype == CMDFUNCTION);
}


static void
print_absolute_path(const char *name)
{
const char *pwd;

if (*name != '/' && (pwd = lookupvar("PWD")) != NULL && *pwd != '\0') {
out1str(pwd);
if (strcmp(pwd, "/") != 0)
outcslow('/', out1);
}
out1str(name);
outcslow('\n', out1);
}







int
typecmd_impl(int argc, char **argv, int cmd, const char *path)
{
struct cmdentry entry;
struct tblentry *cmdp;
const char *const *pp;
struct alias *ap;
int i;
int error1 = 0;

if (path != pathval())
clearcmdentry();

for (i = 1; i < argc; i++) {

for (pp = parsekwd; *pp; pp++)
if (**pp == *argv[i] && equal(*pp, argv[i]))
break;

if (*pp) {
if (cmd == TYPECMD_SMALLV)
out1fmt("%s\n", argv[i]);
else
out1fmt("%s is a shell keyword\n", argv[i]);
continue;
}


if ((ap = lookupalias(argv[i], 1)) != NULL) {
if (cmd == TYPECMD_SMALLV) {
out1fmt("alias %s=", argv[i]);
out1qstr(ap->val);
outcslow('\n', out1);
} else
out1fmt("%s is an alias for %s\n", argv[i],
ap->val);
continue;
}


if ((cmdp = cmdlookup(argv[i], 0)) != NULL) {
entry.cmdtype = cmdp->cmdtype;
entry.u = cmdp->param;
entry.special = cmdp->special;
}
else {

find_command(argv[i], &entry, 0, path);
}

switch (entry.cmdtype) {
case CMDNORMAL: {
if (strchr(argv[i], '/') == NULL) {
const char *path2 = path;
const char *opt2;
char *name;
int j = entry.u.index;
do {
name = padvance(&path2, &opt2, argv[i]);
stunalloc(name);
} while (--j >= 0);
if (cmd != TYPECMD_SMALLV)
out1fmt("%s is%s ", argv[i],
(cmdp && cmd == TYPECMD_TYPE) ?
" a tracked alias for" : "");
print_absolute_path(name);
} else {
if (eaccess(argv[i], X_OK) == 0) {
if (cmd != TYPECMD_SMALLV)
out1fmt("%s is ", argv[i]);
print_absolute_path(argv[i]);
} else {
if (cmd != TYPECMD_SMALLV)
outfmt(out2, "%s: %s\n",
argv[i], strerror(errno));
error1 |= 127;
}
}
break;
}
case CMDFUNCTION:
if (cmd == TYPECMD_SMALLV)
out1fmt("%s\n", argv[i]);
else
out1fmt("%s is a shell function\n", argv[i]);
break;

case CMDBUILTIN:
if (cmd == TYPECMD_SMALLV)
out1fmt("%s\n", argv[i]);
else if (entry.special)
out1fmt("%s is a special shell builtin\n",
argv[i]);
else
out1fmt("%s is a shell builtin\n", argv[i]);
break;

default:
if (cmd != TYPECMD_SMALLV)
outfmt(out2, "%s: not found\n", argv[i]);
error1 |= 127;
break;
}
}

if (path != pathval())
clearcmdentry();

return error1;
}





int
typecmd(int argc, char **argv)
{
if (argc > 2 && strcmp(argv[1], "--") == 0)
argc--, argv++;
return typecmd_impl(argc, argv, TYPECMD_TYPE, bltinlookup("PATH", 1));
}
