

































#include "config.h"
#if !defined(lint)
__COPYRIGHT("@(#) Copyright (c) 1992, 1993\n\
The Regents of the University of California. All rights reserved.\n");
#endif

#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)test.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: tc1.c,v 1.7 2016/02/17 19:47:49 christos Exp $");
#endif
#endif




#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "histedit.h"

static int continuation = 0;
volatile sig_atomic_t gotsig = 0;

static unsigned char complete(EditLine *, int);
int main(int, char **);
static char *prompt(EditLine *);
static void sig(int);

static char *
prompt(EditLine *el)
{
static char a[] = "\1\033[7m\1Edit$\1\033[0m\1 ";
static char b[] = "Edit> ";

return (continuation ? b : a);
}

static void
sig(int i)
{
gotsig = i;
}

static unsigned char
complete(EditLine *el, int ch)
{
DIR *dd = opendir(".");
struct dirent *dp;
const char* ptr;
const LineInfo *lf = el_line(el);
int len;
int res = CC_ERROR;




for (ptr = lf->cursor - 1;
!isspace((unsigned char)*ptr) && ptr > lf->buffer; ptr--)
continue;
len = lf->cursor - ++ptr;

for (dp = readdir(dd); dp != NULL; dp = readdir(dd)) {
if (len > strlen(dp->d_name))
continue;
if (strncmp(dp->d_name, ptr, len) == 0) {
if (el_insertstr(el, &dp->d_name[len]) == -1)
res = CC_ERROR;
else
res = CC_REFRESH;
break;
}
}

closedir(dd);
return res;
}

int
main(int argc, char *argv[])
{
EditLine *el = NULL;
int num;
const char *buf;
Tokenizer *tok;
#if 0
int lastevent = 0;
#endif
int ncontinuation;
History *hist;
HistEvent ev;

(void) setlocale(LC_CTYPE, "");
(void) signal(SIGINT, sig);
(void) signal(SIGQUIT, sig);
(void) signal(SIGHUP, sig);
(void) signal(SIGTERM, sig);

hist = history_init();

history(hist, &ev, H_SETSIZE, 100);

tok = tok_init(NULL);


el = el_init(*argv, stdin, stdout, stderr);

el_set(el, EL_EDITOR, "vi");
el_set(el, EL_SIGNAL, 1);
el_set(el, EL_PROMPT_ESC, prompt, '\1');


el_set(el, EL_HIST, history, hist);


el_set(el, EL_ADDFN, "ed-complete", "Complete argument", complete);


el_set(el, EL_BIND, "^I", "ed-complete", NULL);





el_set(el, EL_BIND, "-a", "k", "ed-prev-line", NULL);
el_set(el, EL_BIND, "-a", "j", "ed-next-line", NULL);




el_source(el, NULL);

while ((buf = el_gets(el, &num)) != NULL && num != 0) {
int ac, cc, co;
#if defined(DEBUG)
int i;
#endif
const char **av;
const LineInfo *li;
li = el_line(el);
#if defined(DEBUG)
(void) fprintf(stderr, "==> got %d %s", num, buf);
(void) fprintf(stderr, " > li `%.*s_%.*s'\n",
(li->cursor - li->buffer), li->buffer,
(li->lastchar - 1 - li->cursor),
(li->cursor >= li->lastchar) ? "" : li->cursor);

#endif
if (gotsig) {
(void) fprintf(stderr, "Got signal %d.\n", (int)gotsig);
gotsig = 0;
el_reset(el);
}

if (!continuation && num == 1)
continue;

ac = cc = co = 0;
ncontinuation = tok_line(tok, li, &ac, &av, &cc, &co);
if (ncontinuation < 0) {
(void) fprintf(stderr, "Internal error\n");
continuation = 0;
continue;
}
#if defined(DEBUG)
(void) fprintf(stderr, " > nc %d ac %d cc %d co %d\n",
ncontinuation, ac, cc, co);
#endif
#if 0
if (continuation) {




if (history(hist, &ev, H_SET, lastevent) == -1)
err(1, "%d: %s", lastevent, ev.str);
history(hist, &ev, H_ADD , buf);
} else {
history(hist, &ev, H_ENTER, buf);
lastevent = ev.num;
}
#else

history(hist, &ev, continuation ? H_APPEND : H_ENTER, buf);
#endif

continuation = ncontinuation;
ncontinuation = 0;
if (continuation)
continue;
#if defined(DEBUG)
for (i = 0; i < ac; i++) {
(void) fprintf(stderr, " > arg#%2d ", i);
if (i != cc)
(void) fprintf(stderr, "`%s'\n", av[i]);
else
(void) fprintf(stderr, "`%.*s_%s'\n",
co, av[i], av[i] + co);
}
#endif

if (strcmp(av[0], "history") == 0) {
int rv;

switch (ac) {
case 1:
for (rv = history(hist, &ev, H_LAST); rv != -1;
rv = history(hist, &ev, H_PREV))
(void) fprintf(stdout, "%4d %s",
ev.num, ev.str);
break;

case 2:
if (strcmp(av[1], "clear") == 0)
history(hist, &ev, H_CLEAR);
else
goto badhist;
break;

case 3:
if (strcmp(av[1], "load") == 0)
history(hist, &ev, H_LOAD, av[2]);
else if (strcmp(av[1], "save") == 0)
history(hist, &ev, H_SAVE, av[2]);
break;

badhist:
default:
(void) fprintf(stderr,
"Bad history arguments\n");
break;
}
} else if (el_parse(el, ac, av) == -1) {
switch (fork()) {
case 0:
execvp(av[0], (char *const *)__UNCONST(av));
perror(av[0]);
_exit(1);

break;

case -1:
perror("fork");
break;

default:
if (wait(&num) == -1)
perror("wait");
(void) fprintf(stderr, "Exit %x\n", num);
break;
}
}

tok_reset(tok);
}

el_end(el);
tok_end(tok);
history_end(hist);

return (0);
}
