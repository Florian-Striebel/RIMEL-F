































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)histedit.c 8.2 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



#include "shell.h"
#include "parser.h"
#include "var.h"
#include "options.h"
#include "main.h"
#include "output.h"
#include "mystring.h"
#include "builtins.h"
#if !defined(NO_HISTORY)
#include "myhistedit.h"
#include "error.h"
#include "eval.h"
#include "memalloc.h"

#define MAXHISTLOOPS 4
#define DEFEDITOR "ed"

History *hist;
EditLine *el;
int displayhist;
static int savehist;
static FILE *el_in, *el_out;

static char *fc_replace(const char *, char *, char *);
static int not_fcnumber(const char *);
static int str_to_event(const char *, int);
static int comparator(const void *, const void *, void *);
static char **sh_matches(const char *, int, int);
static unsigned char sh_complete(EditLine *, int);

static const char *
get_histfile(void)
{
const char *histfile;


if (hist == NULL || histsizeval() == 0)
return (NULL);
histfile = expandstr("${HISTFILE-${HOME-}/.sh_history}");

if (histfile[0] == '\0')
return (NULL);
return (histfile);
}

void
histsave(void)
{
HistEvent he;
char *histtmpname = NULL;
const char *histfile;
int fd;
FILE *f;

if (!savehist || (histfile = get_histfile()) == NULL)
return;
INTOFF;
asprintf(&histtmpname, "%s.XXXXXXXXXX", histfile);
if (histtmpname == NULL) {
INTON;
return;
}
fd = mkstemp(histtmpname);
if (fd == -1 || (f = fdopen(fd, "w")) == NULL) {
free(histtmpname);
INTON;
return;
}
if (history(hist, &he, H_SAVE_FP, f) < 1 ||
rename(histtmpname, histfile) == -1)
unlink(histtmpname);
fclose(f);
free(histtmpname);
INTON;

}

void
histload(void)
{
const char *histfile;
HistEvent he;

if ((histfile = get_histfile()) == NULL)
return;
errno = 0;
if (history(hist, &he, H_LOAD, histfile) != -1 || errno == ENOENT)
savehist = 1;
}





void
histedit(void)
{

#define editing (Eflag || Vflag)

if (iflag) {
if (!hist) {



INTOFF;
hist = history_init();
INTON;

if (hist != NULL)
sethistsize(histsizeval());
else
out2fmt_flush("sh: can't initialize history\n");
}
if (editing && !el && isatty(0)) {



char *term;

INTOFF;
if (el_in == NULL)
el_in = fdopen(0, "r");
if (el_out == NULL)
el_out = fdopen(2, "w");
if (el_in == NULL || el_out == NULL)
goto bad;
term = lookupvar("TERM");
if (term)
setenv("TERM", term, 1);
else
unsetenv("TERM");
el = el_init(arg0, el_in, el_out, el_out);
if (el != NULL) {
if (hist)
el_set(el, EL_HIST, history, hist);
el_set(el, EL_PROMPT, getprompt);
el_set(el, EL_ADDFN, "sh-complete",
"Filename completion",
sh_complete);
} else {
bad:
out2fmt_flush("sh: can't initialize editing\n");
}
INTON;
} else if (!editing && el) {
INTOFF;
el_end(el);
el = NULL;
INTON;
}
if (el) {
if (Vflag)
el_set(el, EL_EDITOR, "vi");
else if (Eflag) {
el_set(el, EL_EDITOR, "emacs");
}
el_set(el, EL_BIND, "^I", "sh-complete", NULL);
el_source(el, NULL);
}
} else {
INTOFF;
if (el) {
el_end(el);
el = NULL;
}
if (hist) {
history_end(hist);
hist = NULL;
}
INTON;
}
}


void
sethistsize(const char *hs)
{
int histsize;
HistEvent he;

if (hist != NULL) {
if (hs == NULL || !is_number(hs))
histsize = 100;
else
histsize = atoi(hs);
history(hist, &he, H_SETSIZE, histsize);
history(hist, &he, H_SETUNIQUE, 1);
}
}

void
setterm(const char *term)
{
if (rootshell && el != NULL && term != NULL)
el_set(el, EL_TERMINAL, term);
}

int
histcmd(int argc, char **argv __unused)
{
int ch;
const char *editor = NULL;
HistEvent he;
int lflg = 0, nflg = 0, rflg = 0, sflg = 0;
int i, retval;
const char *firststr, *laststr;
int first, last, direction;
char *pat = NULL, *repl = NULL;
static int active = 0;
struct jmploc jmploc;
struct jmploc *savehandler;
char editfilestr[PATH_MAX];
char *volatile editfile;
FILE *efp = NULL;
int oldhistnum;

if (hist == NULL)
error("history not active");

if (argc == 1)
error("missing history argument");

while (not_fcnumber(*argptr) && (ch = nextopt("e:lnrs")) != '\0')
switch ((char)ch) {
case 'e':
editor = shoptarg;
break;
case 'l':
lflg = 1;
break;
case 'n':
nflg = 1;
break;
case 'r':
rflg = 1;
break;
case 's':
sflg = 1;
break;
}

savehandler = handler;



if (lflg == 0 || editor || sflg) {
lflg = 0;
editfile = NULL;




if (setjmp(jmploc.loc)) {
active = 0;
if (editfile)
unlink(editfile);
handler = savehandler;
longjmp(handler->loc, 1);
}
handler = &jmploc;
if (++active > MAXHISTLOOPS) {
active = 0;
displayhist = 0;
error("called recursively too many times");
}



if (sflg == 0) {
if (editor == NULL &&
(editor = bltinlookup("FCEDIT", 1)) == NULL &&
(editor = bltinlookup("EDITOR", 1)) == NULL)
editor = DEFEDITOR;
if (editor[0] == '-' && editor[1] == '\0') {
sflg = 1;
editor = NULL;
}
}
}




if (lflg == 0 && *argptr != NULL &&
((repl = strchr(*argptr, '=')) != NULL)) {
pat = *argptr;
*repl++ = '\0';
argptr++;
}



if (*argptr == NULL) {
firststr = lflg ? "-16" : "-1";
laststr = "-1";
} else if (argptr[1] == NULL) {
firststr = argptr[0];
laststr = lflg ? "-1" : argptr[0];
} else if (argptr[2] == NULL) {
firststr = argptr[0];
laststr = argptr[1];
} else
error("too many arguments");



first = str_to_event(firststr, 0);
last = str_to_event(laststr, 1);

if (rflg) {
i = last;
last = first;
first = i;
}





direction = first < last ? H_PREV : H_NEXT;




if (editor) {
int fd;
INTOFF;
sprintf(editfilestr, "%s/_shXXXXXX", _PATH_TMP);
if ((fd = mkstemp(editfilestr)) < 0)
error("can't create temporary file %s", editfile);
editfile = editfilestr;
if ((efp = fdopen(fd, "w")) == NULL) {
close(fd);
error("Out of space");
}
}









history(hist, &he, H_FIRST);
retval = history(hist, &he, H_NEXT_EVENT, first);
for (;retval != -1; retval = history(hist, &he, direction)) {
if (lflg) {
if (!nflg)
out1fmt("%5d ", he.num);
out1str(he.str);
} else {
const char *s = pat ?
fc_replace(he.str, pat, repl) : he.str;

if (sflg) {
if (displayhist) {
out2str(s);
flushout(out2);
}
evalstring(s, 0);
if (displayhist && hist) {




oldhistnum = he.num;
history(hist, &he, H_ENTER, s);





history(hist, &he,
H_NEXT_EVENT, oldhistnum);
}
} else
fputs(s, efp);
}




if (he.num == last)
break;
}
if (editor) {
char *editcmd;

fclose(efp);
INTON;
editcmd = stalloc(strlen(editor) + strlen(editfile) + 2);
sprintf(editcmd, "%s %s", editor, editfile);
evalstring(editcmd, 0);
readcmdfile(editfile, 0 );
unlink(editfile);
}

if (lflg == 0 && active > 0)
--active;
if (displayhist)
displayhist = 0;
handler = savehandler;
return 0;
}

static char *
fc_replace(const char *s, char *p, char *r)
{
char *dest;
int plen = strlen(p);

STARTSTACKSTR(dest);
while (*s) {
if (*s == *p && strncmp(s, p, plen) == 0) {
STPUTS(r, dest);
s += plen;
*p = '\0';
} else
STPUTC(*s++, dest);
}
STPUTC('\0', dest);
dest = grabstackstr(dest);

return (dest);
}

static int
not_fcnumber(const char *s)
{
if (s == NULL)
return (0);
if (*s == '-')
s++;
return (!is_number(s));
}

static int
str_to_event(const char *str, int last)
{
HistEvent he;
const char *s = str;
int relative = 0;
int i, retval;

retval = history(hist, &he, H_FIRST);
switch (*s) {
case '-':
relative = 1;

case '+':
s++;
}
if (is_number(s)) {
i = atoi(s);
if (relative) {
while (retval != -1 && i--) {
retval = history(hist, &he, H_NEXT);
}
if (retval == -1)
retval = history(hist, &he, H_LAST);
} else {
retval = history(hist, &he, H_NEXT_EVENT, i);
if (retval == -1) {




retval = history(hist, &he, last ? H_FIRST : H_LAST);
}
}
if (retval == -1)
error("history number %s not found (internal error)",
str);
} else {



retval = history(hist, &he, H_PREV_STR, str);
if (retval == -1)
error("history pattern not found: %s", str);
}
return (he.num);
}

int
bindcmd(int argc, char **argv)
{
int ret;
FILE *old;
FILE *out;

if (el == NULL)
error("line editing is disabled");

INTOFF;

out = out1fp();
if (out == NULL)
error("Out of space");

el_get(el, EL_GETFP, 1, &old);
el_set(el, EL_SETFP, 1, out);

ret = el_parse(el, argc, __DECONST(const char **, argv));

el_set(el, EL_SETFP, 1, old);

fclose(out);

if (argc > 1 && argv[1][0] == '-' &&
memchr("ve", argv[1][1], 2) != NULL) {
Vflag = argv[1][1] == 'v';
Eflag = !Vflag;
histedit();
}

INTON;

return ret;
}





static int
comparator(const void *a, const void *b, void *thunk)
{
size_t curpos = (intptr_t)thunk;
return (strcmp(*(char *const *)a + curpos,
*(char *const *)b + curpos));
}







static char
**sh_matches(const char *text, int start, int end)
{
char *free_path = NULL, *path;
const char *dirname;
char **matches = NULL;
size_t i = 0, size = 16, uniq;
size_t curpos = end - start, lcstring = -1;

if (start > 0 || memchr("/.~", text[0], 3) != NULL)
return (NULL);
if ((free_path = path = strdup(pathval())) == NULL)
goto out;
if ((matches = malloc(size * sizeof(matches[0]))) == NULL)
goto out;
while ((dirname = strsep(&path, ":")) != NULL) {
struct dirent *entry;
DIR *dir;
int dfd;

dir = opendir(dirname[0] == '\0' ? "." : dirname);
if (dir == NULL)
continue;
if ((dfd = dirfd(dir)) == -1) {
closedir(dir);
continue;
}
while ((entry = readdir(dir)) != NULL) {
struct stat statb;
char **rmatches;

if (strncmp(entry->d_name, text, curpos) != 0)
continue;
if (entry->d_type == DT_UNKNOWN || entry->d_type == DT_LNK) {
if (fstatat(dfd, entry->d_name, &statb, 0) == -1)
continue;
if (!S_ISREG(statb.st_mode))
continue;
} else if (entry->d_type != DT_REG)
continue;
matches[++i] = strdup(entry->d_name);
if (i < size - 1)
continue;
size *= 2;
rmatches = reallocarray(matches, size, sizeof(matches[0]));
if (rmatches == NULL) {
closedir(dir);
goto out;
}
matches = rmatches;
}
closedir(dir);
}
out:
free(free_path);
if (i == 0) {
free(matches);
return (NULL);
}
uniq = 1;
if (i > 1) {
qsort_s(matches + 1, i, sizeof(matches[0]), comparator,
(void *)(intptr_t)curpos);
for (size_t k = 2; k <= i; k++) {
const char *l = matches[uniq] + curpos;
const char *r = matches[k] + curpos;
size_t common = 0;

while (*l != '\0' && *r != '\0' && *l == *r)
(void)l++, r++, common++;
if (common < lcstring)
lcstring = common;
if (*l == *r)
free(matches[k]);
else
matches[++uniq] = matches[k];
}
}
matches[uniq + 1] = NULL;









if (uniq == 1)
matches[0] = strdup(matches[1]);
else if (lcstring != (size_t)-1)
matches[0] = strndup(matches[1], curpos + lcstring);
else
matches[0] = strdup(text);
if (matches[0] == NULL) {
for (size_t k = 1; k <= uniq; k++)
free(matches[k]);
free(matches);
return (NULL);
}
return (matches);
}





unsigned char
sh_complete(EditLine *sel, int ch __unused)
{
return (unsigned char)fn_complete2(sel, NULL, sh_matches,
L" \t\n\"\\'`@$><=;|&{(", NULL, NULL, (size_t)100,
NULL, &((int) {0}), NULL, NULL, FN_QUOTE_MATCH);
}

#else
#include "error.h"

int
histcmd(int argc __unused, char **argv __unused)
{

error("not compiled with history support");

return (0);
}

int
bindcmd(int argc __unused, char **argv __unused)
{

error("not compiled with line editing support");
return (0);
}
#endif
