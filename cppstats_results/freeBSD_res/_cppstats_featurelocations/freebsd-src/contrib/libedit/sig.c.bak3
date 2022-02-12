

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)sig.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: sig.c,v 1.26 2016/05/09 21:46:56 christos Exp $");
#endif
#endif






#include <errno.h>
#include <stdlib.h>

#include "el.h"
#include "common.h"

static EditLine *sel = NULL;

static const int sighdl[] = {
#define _DO(a) (a),
ALLSIGS
#undef _DO
- 1
};

static void sig_handler(int);






static void
sig_handler(int signo)
{
int i, save_errno;
sigset_t nset, oset;

save_errno = errno;
(void) sigemptyset(&nset);
(void) sigaddset(&nset, signo);
(void) sigprocmask(SIG_BLOCK, &nset, &oset);

sel->el_signal->sig_no = signo;

switch (signo) {
case SIGCONT:
tty_rawmode(sel);
if (ed_redisplay(sel, 0) == CC_REFRESH)
re_refresh(sel);
terminal__flush(sel);
break;

case SIGWINCH:
el_resize(sel);
break;

default:
tty_cookedmode(sel);
break;
}

for (i = 0; sighdl[i] != -1; i++)
if (signo == sighdl[i])
break;

(void) sigaction(signo, &sel->el_signal->sig_action[i], NULL);
sel->el_signal->sig_action[i].sa_handler = SIG_ERR;
sel->el_signal->sig_action[i].sa_flags = 0;
sigemptyset(&sel->el_signal->sig_action[i].sa_mask);
(void) sigprocmask(SIG_SETMASK, &oset, NULL);
(void) kill(0, signo);
errno = save_errno;
}





libedit_private int
sig_init(EditLine *el)
{
size_t i;
sigset_t *nset, oset;

el->el_signal = el_malloc(sizeof(*el->el_signal));
if (el->el_signal == NULL)
return -1;

nset = &el->el_signal->sig_set;
(void) sigemptyset(nset);
#define _DO(a) (void) sigaddset(nset, a);
ALLSIGS
#undef _DO
(void) sigprocmask(SIG_BLOCK, nset, &oset);

for (i = 0; sighdl[i] != -1; i++) {
el->el_signal->sig_action[i].sa_handler = SIG_ERR;
el->el_signal->sig_action[i].sa_flags = 0;
sigemptyset(&el->el_signal->sig_action[i].sa_mask);
}

(void) sigprocmask(SIG_SETMASK, &oset, NULL);

return 0;
}





libedit_private void
sig_end(EditLine *el)
{

el_free(el->el_signal);
el->el_signal = NULL;
}





libedit_private void
sig_set(EditLine *el)
{
size_t i;
sigset_t oset;
struct sigaction osa, nsa;

nsa.sa_handler = sig_handler;
nsa.sa_flags = 0;
sigemptyset(&nsa.sa_mask);

(void) sigprocmask(SIG_BLOCK, &el->el_signal->sig_set, &oset);

for (i = 0; sighdl[i] != -1; i++) {

if (sigaction(sighdl[i], &nsa, &osa) != -1 &&
osa.sa_handler != sig_handler)
el->el_signal->sig_action[i] = osa;
}
sel = el;
(void) sigprocmask(SIG_SETMASK, &oset, NULL);
}





libedit_private void
sig_clr(EditLine *el)
{
size_t i;
sigset_t oset;

(void) sigprocmask(SIG_BLOCK, &el->el_signal->sig_set, &oset);

for (i = 0; sighdl[i] != -1; i++)
if (el->el_signal->sig_action[i].sa_handler != SIG_ERR)
(void)sigaction(sighdl[i],
&el->el_signal->sig_action[i], NULL);

sel = NULL;

(void)sigprocmask(SIG_SETMASK, &oset, NULL);
}
