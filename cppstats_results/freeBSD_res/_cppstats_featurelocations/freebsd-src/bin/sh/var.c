

































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)var.c 8.3 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <unistd.h>
#include <stdlib.h>
#include <paths.h>





#include <locale.h>
#include <langinfo.h>

#include "shell.h"
#include "output.h"
#include "expand.h"
#include "nodes.h"
#include "eval.h"
#include "exec.h"
#include "syntax.h"
#include "options.h"
#include "mail.h"
#include "var.h"
#include "memalloc.h"
#include "error.h"
#include "mystring.h"
#include "parser.h"
#include "builtins.h"
#if !defined(NO_HISTORY)
#include "myhistedit.h"
#endif


#if !defined(VTABSIZE)
#define VTABSIZE 39
#endif


struct varinit {
struct var *var;
int flags;
const char *text;
void (*func)(const char *);
};


#if !defined(NO_HISTORY)
struct var vhistsize;
struct var vterm;
#endif
struct var vifs;
struct var vmail;
struct var vmpath;
struct var vpath;
struct var vps1;
struct var vps2;
struct var vps4;
static struct var voptind;
struct var vdisvfork;

struct localvar *localvars;
int forcelocal;

static const struct varinit varinit[] = {
#if !defined(NO_HISTORY)
{ &vhistsize, VUNSET, "HISTSIZE=",
sethistsize },
#endif
{ &vifs, 0, "IFS= \t\n",
NULL },
{ &vmail, VUNSET, "MAIL=",
NULL },
{ &vmpath, VUNSET, "MAILPATH=",
NULL },
{ &vpath, 0, "PATH=" _PATH_DEFPATH,
changepath },



{ &vps2, 0, "PS2=> ",
NULL },
{ &vps4, 0, "PS4=+ ",
NULL },
#if !defined(NO_HISTORY)
{ &vterm, VUNSET, "TERM=",
setterm },
#endif
{ &voptind, 0, "OPTIND=1",
getoptsreset },
{ &vdisvfork, VUNSET, "SH_DISABLE_VFORK=",
NULL },
{ NULL, 0, NULL,
NULL }
};

static struct var *vartab[VTABSIZE];

static const char *const locale_names[7] = {
"LC_COLLATE", "LC_CTYPE", "LC_MONETARY",
"LC_NUMERIC", "LC_TIME", "LC_MESSAGES", NULL
};
static const int locale_categories[7] = {
LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, LC_MESSAGES, 0
};

static int varequal(const char *, const char *);
static struct var *find_var(const char *, struct var ***, int *);
static int localevar(const char *);
static void setvareq_const(const char *s, int flags);

extern char **environ;






void
initvar(void)
{
char ppid[20];
const struct varinit *ip;
struct var *vp;
struct var **vpp;
char **envp;

for (ip = varinit ; (vp = ip->var) != NULL ; ip++) {
if (find_var(ip->text, &vpp, &vp->name_len) != NULL)
continue;
vp->next = *vpp;
*vpp = vp;
vp->text = __DECONST(char *, ip->text);
vp->flags = ip->flags | VSTRFIXED | VTEXTFIXED;
vp->func = ip->func;
}



if (find_var("PS1", &vpp, &vps1.name_len) == NULL) {
vps1.next = *vpp;
*vpp = &vps1;
vps1.text = __DECONST(char *, geteuid() ? "PS1=$ " : "PS1=#");
vps1.flags = VSTRFIXED|VTEXTFIXED;
}
fmtstr(ppid, sizeof(ppid), "%d", (int)getppid());
setvarsafe("PPID", ppid, 0);
for (envp = environ ; *envp ; envp++) {
if (strchr(*envp, '=')) {
setvareq(*envp, VEXPORT|VTEXTFIXED);
}
}
setvareq_const("OPTIND=1", 0);
setvareq_const("IFS= \t\n", 0);
}





int
setvarsafe(const char *name, const char *val, int flags)
{
struct jmploc jmploc;
struct jmploc *const savehandler = handler;
int err = 0;
int inton;

inton = is_int_on();
if (setjmp(jmploc.loc))
err = 1;
else {
handler = &jmploc;
setvar(name, val, flags);
}
handler = savehandler;
SETINTON(inton);
return err;
}






void
setvar(const char *name, const char *val, int flags)
{
const char *p;
size_t len;
size_t namelen;
size_t vallen;
char *nameeq;
int isbad;

isbad = 0;
p = name;
if (! is_name(*p))
isbad = 1;
p++;
for (;;) {
if (! is_in_name(*p)) {
if (*p == '\0' || *p == '=')
break;
isbad = 1;
}
p++;
}
namelen = p - name;
if (isbad)
error("%.*s: bad variable name", (int)namelen, name);
len = namelen + 2;
if (val == NULL) {
flags |= VUNSET;
vallen = 0;
} else {
vallen = strlen(val);
len += vallen;
}
INTOFF;
nameeq = ckmalloc(len);
memcpy(nameeq, name, namelen);
nameeq[namelen] = '=';
if (val)
memcpy(nameeq + namelen + 1, val, vallen + 1);
else
nameeq[namelen + 1] = '\0';
setvareq(nameeq, flags);
INTON;
}

static int
localevar(const char *s)
{
const char *const *ss;

if (*s != 'L')
return 0;
if (varequal(s + 1, "ANG"))
return 1;
if (strncmp(s + 1, "C_", 2) != 0)
return 0;
if (varequal(s + 3, "ALL"))
return 1;
for (ss = locale_names; *ss ; ss++)
if (varequal(s + 3, *ss + 3))
return 1;
return 0;
}






static void
change_env(const char *s, int set)
{
char *eqp;
char *ss;

INTOFF;
ss = savestr(s);
if ((eqp = strchr(ss, '=')) != NULL)
*eqp = '\0';
if (set && eqp != NULL)
(void) setenv(ss, eqp + 1, 1);
else
(void) unsetenv(ss);
ckfree(ss);
INTON;

return;
}









void
setvareq(char *s, int flags)
{
struct var *vp, **vpp;
int nlen;

if (aflag)
flags |= VEXPORT;
if (forcelocal && !(flags & (VNOSET | VNOLOCAL)))
mklocal(s);
vp = find_var(s, &vpp, &nlen);
if (vp != NULL) {
if (vp->flags & VREADONLY) {
if ((flags & (VTEXTFIXED|VSTACK)) == 0)
ckfree(s);
error("%.*s: is read only", vp->name_len, vp->text);
}
if (flags & VNOSET) {
if ((flags & (VTEXTFIXED|VSTACK)) == 0)
ckfree(s);
return;
}
INTOFF;

if (vp->func && (flags & VNOFUNC) == 0)
(*vp->func)(s + vp->name_len + 1);

if ((vp->flags & (VTEXTFIXED|VSTACK)) == 0)
ckfree(vp->text);

vp->flags &= ~(VTEXTFIXED|VSTACK|VUNSET);
vp->flags |= flags;
vp->text = s;









if ((vp == &vmpath || (vp == &vmail && ! mpathset())) &&
iflag == 1)
chkmail(1);
if ((vp->flags & VEXPORT) && localevar(s)) {
change_env(s, 1);
(void) setlocale(LC_ALL, "");
updatecharset();
}
INTON;
return;
}

if (flags & VNOSET) {
if ((flags & (VTEXTFIXED|VSTACK)) == 0)
ckfree(s);
return;
}
INTOFF;
vp = ckmalloc(sizeof (*vp));
vp->flags = flags;
vp->text = s;
vp->name_len = nlen;
vp->next = *vpp;
vp->func = NULL;
*vpp = vp;
if ((vp->flags & VEXPORT) && localevar(s)) {
change_env(s, 1);
(void) setlocale(LC_ALL, "");
updatecharset();
}
INTON;
}


static void
setvareq_const(const char *s, int flags)
{
setvareq(__DECONST(char *, s), flags | VTEXTFIXED);
}






void
listsetvar(struct arglist *list, int flags)
{
int i;

INTOFF;
for (i = 0; i < list->count; i++)
setvareq(savestr(list->args[i]), flags);
INTON;
}







char *
lookupvar(const char *name)
{
struct var *v;

v = find_var(name, NULL, NULL);
if (v == NULL || v->flags & VUNSET)
return NULL;
return v->text + v->name_len + 1;
}









char *
bltinlookup(const char *name, int doall)
{
struct var *v;
char *result;
int i;

result = NULL;
if (cmdenviron) for (i = 0; i < cmdenviron->count; i++) {
if (varequal(cmdenviron->args[i], name))
result = strchr(cmdenviron->args[i], '=') + 1;
}
if (result != NULL)
return result;

v = find_var(name, NULL, NULL);
if (v == NULL || v->flags & VUNSET ||
(!doall && (v->flags & VEXPORT) == 0))
return NULL;
return v->text + v->name_len + 1;
}





void
bltinsetlocale(void)
{
int act = 0;
char *loc, *locdef;
int i;

if (cmdenviron) for (i = 0; i < cmdenviron->count; i++) {
if (localevar(cmdenviron->args[i])) {
act = 1;
break;
}
}
if (!act)
return;
loc = bltinlookup("LC_ALL", 0);
INTOFF;
if (loc != NULL) {
setlocale(LC_ALL, loc);
INTON;
updatecharset();
return;
}
locdef = bltinlookup("LANG", 0);
for (i = 0; locale_names[i] != NULL; i++) {
loc = bltinlookup(locale_names[i], 0);
if (loc == NULL)
loc = locdef;
if (loc != NULL)
setlocale(locale_categories[i], loc);
}
INTON;
updatecharset();
}




void
bltinunsetlocale(void)
{
int i;

INTOFF;
if (cmdenviron) for (i = 0; i < cmdenviron->count; i++) {
if (localevar(cmdenviron->args[i])) {
setlocale(LC_ALL, "");
updatecharset();
break;
}
}
INTON;
}




void
updatecharset(void)
{
char *charset;

charset = nl_langinfo(CODESET);
localeisutf8 = !strcmp(charset, "UTF-8");
}

void
initcharset(void)
{
updatecharset();
initial_localeisutf8 = localeisutf8;
}






char **
environment(void)
{
int nenv;
struct var **vpp;
struct var *vp;
char **env, **ep;

nenv = 0;
for (vpp = vartab ; vpp < vartab + VTABSIZE ; vpp++) {
for (vp = *vpp ; vp ; vp = vp->next)
if ((vp->flags & (VEXPORT|VUNSET)) == VEXPORT)
nenv++;
}
ep = env = stalloc((nenv + 1) * sizeof *env);
for (vpp = vartab ; vpp < vartab + VTABSIZE ; vpp++) {
for (vp = *vpp ; vp ; vp = vp->next)
if ((vp->flags & (VEXPORT|VUNSET)) == VEXPORT)
*ep++ = vp->text;
}
*ep = NULL;
return env;
}


static int
var_compare(const void *a, const void *b)
{
const char *const *sa, *const *sb;

sa = a;
sb = b;





return strcoll(*sa, *sb);
}







int
showvarscmd(int argc __unused, char **argv __unused)
{
struct var **vpp;
struct var *vp;
const char *s;
const char **vars;
int i, n;




n = 0;
for (vpp = vartab; vpp < vartab + VTABSIZE; vpp++) {
for (vp = *vpp; vp; vp = vp->next) {
if (!(vp->flags & VUNSET))
n++;
}
}

INTOFF;
vars = ckmalloc(n * sizeof(*vars));
i = 0;
for (vpp = vartab; vpp < vartab + VTABSIZE; vpp++) {
for (vp = *vpp; vp; vp = vp->next) {
if (!(vp->flags & VUNSET))
vars[i++] = vp->text;
}
}

qsort(vars, n, sizeof(*vars), var_compare);
for (i = 0; i < n; i++) {




if (!isassignment(vars[i]))
continue;
s = strchr(vars[i], '=');
s++;
outbin(vars[i], s - vars[i], out1);
out1qstr(s);
out1c('\n');
}
ckfree(vars);
INTON;

return 0;
}







int
exportcmd(int argc __unused, char **argv)
{
struct var **vpp;
struct var *vp;
char **ap;
char *name;
char *p;
char *cmdname;
int ch, values;
int flag = argv[0][0] == 'r'? VREADONLY : VEXPORT;

cmdname = argv[0];
values = 0;
while ((ch = nextopt("p")) != '\0') {
switch (ch) {
case 'p':
values = 1;
break;
}
}

if (values && *argptr != NULL)
error("-p requires no arguments");
if (*argptr != NULL) {
for (ap = argptr; (name = *ap) != NULL; ap++) {
if ((p = strchr(name, '=')) != NULL) {
p++;
} else {
vp = find_var(name, NULL, NULL);
if (vp != NULL) {
vp->flags |= flag;
if ((vp->flags & VEXPORT) && localevar(vp->text)) {
change_env(vp->text, 1);
(void) setlocale(LC_ALL, "");
updatecharset();
}
continue;
}
}
setvar(name, p, flag);
}
} else {
for (vpp = vartab ; vpp < vartab + VTABSIZE ; vpp++) {
for (vp = *vpp ; vp ; vp = vp->next) {
if (vp->flags & flag) {
if (values) {





if (!isassignment(vp->text))
continue;
out1str(cmdname);
out1c(' ');
}
if (values && !(vp->flags & VUNSET)) {
outbin(vp->text,
vp->name_len + 1, out1);
out1qstr(vp->text +
vp->name_len + 1);
} else
outbin(vp->text, vp->name_len,
out1);
out1c('\n');
}
}
}
}
return 0;
}






int
localcmd(int argc __unused, char **argv __unused)
{
char *name;

nextopt("");
if (! in_function())
error("Not in a function");
while ((name = *argptr++) != NULL) {
mklocal(name);
}
return 0;
}









void
mklocal(char *name)
{
struct localvar *lvp;
struct var **vpp;
struct var *vp;

INTOFF;
lvp = ckmalloc(sizeof (struct localvar));
if (name[0] == '-' && name[1] == '\0') {
lvp->text = ckmalloc(sizeof optval);
memcpy(lvp->text, optval, sizeof optval);
vp = NULL;
} else {
vp = find_var(name, &vpp, NULL);
if (vp == NULL) {
if (strchr(name, '='))
setvareq(savestr(name), VSTRFIXED | VNOLOCAL);
else
setvar(name, NULL, VSTRFIXED | VNOLOCAL);
vp = *vpp;
lvp->text = NULL;
lvp->flags = VUNSET;
} else {
lvp->text = vp->text;
lvp->flags = vp->flags;
vp->flags |= VSTRFIXED|VTEXTFIXED;
if (name[vp->name_len] == '=')
setvareq(savestr(name), VNOLOCAL);
}
}
lvp->vp = vp;
lvp->next = localvars;
localvars = lvp;
INTON;
}






void
poplocalvars(void)
{
struct localvar *lvp;
struct var *vp;
int islocalevar;

INTOFF;
while ((lvp = localvars) != NULL) {
localvars = lvp->next;
vp = lvp->vp;
if (vp == NULL) {
memcpy(optval, lvp->text, sizeof optval);
ckfree(lvp->text);
optschanged();
} else if ((lvp->flags & (VUNSET|VSTRFIXED)) == VUNSET) {
vp->flags &= ~VREADONLY;
(void)unsetvar(vp->text);
} else {
islocalevar = (vp->flags | lvp->flags) & VEXPORT &&
localevar(lvp->text);
if ((vp->flags & VTEXTFIXED) == 0)
ckfree(vp->text);
vp->flags = lvp->flags;
vp->text = lvp->text;
if (vp->func)
(*vp->func)(vp->text + vp->name_len + 1);
if (islocalevar) {
change_env(vp->text, vp->flags & VEXPORT &&
(vp->flags & VUNSET) == 0);
setlocale(LC_ALL, "");
updatecharset();
}
}
ckfree(lvp);
}
INTON;
}


int
setvarcmd(int argc, char **argv)
{
if (argc <= 2)
return unsetcmd(argc, argv);
else if (argc == 3)
setvar(argv[1], argv[2], 0);
else
error("too many arguments");
return 0;
}






int
unsetcmd(int argc __unused, char **argv __unused)
{
char **ap;
int i;
int flg_func = 0;
int flg_var = 0;
int ret = 0;

while ((i = nextopt("vf")) != '\0') {
if (i == 'f')
flg_func = 1;
else
flg_var = 1;
}
if (flg_func == 0 && flg_var == 0)
flg_var = 1;

INTOFF;
for (ap = argptr; *ap ; ap++) {
if (flg_func)
ret |= unsetfunc(*ap);
if (flg_var)
ret |= unsetvar(*ap);
}
INTON;
return ret;
}







int
unsetvar(const char *s)
{
struct var **vpp;
struct var *vp;

vp = find_var(s, &vpp, NULL);
if (vp == NULL)
return (0);
if (vp->flags & VREADONLY)
return (1);
if (vp->text[vp->name_len + 1] != '\0')
setvar(s, "", 0);
if ((vp->flags & VEXPORT) && localevar(vp->text)) {
change_env(s, 0);
setlocale(LC_ALL, "");
updatecharset();
}
vp->flags &= ~VEXPORT;
vp->flags |= VUNSET;
if ((vp->flags & VSTRFIXED) == 0) {
if ((vp->flags & VTEXTFIXED) == 0)
ckfree(vp->text);
*vpp = vp->next;
ckfree(vp);
}
return (0);
}









static int
varequal(const char *p, const char *q)
{
while (*p == *q++) {
if (*p++ == '=')
return 1;
}
if (*p == '=' && *(q - 1) == '\0')
return 1;
return 0;
}








static struct var *
find_var(const char *name, struct var ***vppp, int *lenp)
{
unsigned int hashval;
int len;
struct var *vp, **vpp;
const char *p = name;

hashval = 0;
while (*p && *p != '=')
hashval = 2 * hashval + (unsigned char)*p++;
len = p - name;

if (lenp)
*lenp = len;
vpp = &vartab[hashval % VTABSIZE];
if (vppp)
*vppp = vpp;

for (vp = *vpp ; vp ; vpp = &vp->next, vp = *vpp) {
if (vp->name_len != len)
continue;
if (memcmp(vp->text, name, len) != 0)
continue;
if (vppp)
*vppp = vpp;
return vp;
}
return NULL;
}
