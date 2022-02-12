


























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "ed.h"


#define USIZE 100
static undo_t *ustack = NULL;
static long usize = 0;
static long u_p = 0;


undo_t *
push_undo_stack(int type, long from, long to)
{
undo_t *t;

#if defined(sun) || defined(NO_REALLOC_NULL)
if (ustack == NULL &&
(ustack = (undo_t *) malloc((usize = USIZE) * sizeof(undo_t))) == NULL) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "out of memory";
return NULL;
}
#endif
t = ustack;
if (u_p < usize ||
(t = (undo_t *) realloc(ustack, (usize += USIZE) * sizeof(undo_t))) != NULL) {
ustack = t;
ustack[u_p].type = type;
ustack[u_p].t = get_addressed_line_node(to);
ustack[u_p].h = get_addressed_line_node(from);
return ustack + u_p++;
}

fprintf(stderr, "%s\n", strerror(errno));
errmsg = "out of memory";
clear_undo_stack();
free(ustack);
ustack = NULL;
usize = 0;
return NULL;
}



#define USWAP(x,y) { undo_t utmp; utmp = x, x = y, y = utmp; }





long u_current_addr = -1;
long u_addr_last = -1;


int
pop_undo_stack(void)
{
long n;
long o_current_addr = current_addr;
long o_addr_last = addr_last;

if (u_current_addr == -1 || u_addr_last == -1) {
errmsg = "nothing to undo";
return ERR;
} else if (u_p)
modified = 1;
get_addressed_line_node(0);
SPL1();
for (n = u_p; n-- > 0;) {
switch(ustack[n].type) {
case UADD:
REQUE(ustack[n].h->q_back, ustack[n].t->q_forw);
break;
case UDEL:
REQUE(ustack[n].h->q_back, ustack[n].h);
REQUE(ustack[n].t, ustack[n].t->q_forw);
break;
case UMOV:
case VMOV:
REQUE(ustack[n - 1].h, ustack[n].h->q_forw);
REQUE(ustack[n].t->q_back, ustack[n - 1].t);
REQUE(ustack[n].h, ustack[n].t);
n--;
break;
default:

;
}
ustack[n].type ^= 1;
}

for (n = u_p; n-- > (u_p + 1)/ 2;)
USWAP(ustack[n], ustack[u_p - 1 - n]);
if (isglobal)
clear_active_list();
current_addr = u_current_addr, u_current_addr = o_current_addr;
addr_last = u_addr_last, u_addr_last = o_addr_last;
SPL0();
return 0;
}



void
clear_undo_stack(void)
{
line_t *lp, *ep, *tl;

while (u_p--)
if (ustack[u_p].type == UDEL) {
ep = ustack[u_p].t->q_forw;
for (lp = ustack[u_p].h; lp != ep; lp = tl) {
unmark_line_node(lp);
tl = lp->q_forw;
free(lp);
}
}
u_p = 0;
u_current_addr = current_addr;
u_addr_last = addr_last;
}
