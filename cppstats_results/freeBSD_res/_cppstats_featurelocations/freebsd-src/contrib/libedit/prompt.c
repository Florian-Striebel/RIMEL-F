

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)prompt.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: prompt.c,v 1.27 2017/06/27 23:25:13 christos Exp $");
#endif
#endif




#include <stdio.h>
#include "el.h"

static wchar_t *prompt_default(EditLine *);
static wchar_t *prompt_default_r(EditLine *);




static wchar_t *

prompt_default(EditLine *el __attribute__((__unused__)))
{
static wchar_t a[3] = L"? ";

return a;
}





static wchar_t *

prompt_default_r(EditLine *el __attribute__((__unused__)))
{
static wchar_t a[1] = L"";

return a;
}





libedit_private void
prompt_print(EditLine *el, int op)
{
el_prompt_t *elp;
wchar_t *p;

if (op == EL_PROMPT)
elp = &el->el_prompt;
else
elp = &el->el_rprompt;

if (elp->p_wide)
p = (*elp->p_func)(el);
else
p = ct_decode_string((char *)(void *)(*elp->p_func)(el),
&el->el_scratch);

for (; *p; p++) {
if (elp->p_ignore == *p) {
wchar_t *litstart = ++p;
while (*p && *p != elp->p_ignore)
p++;
if (!*p || !p[1]) {

break;
}
re_putliteral(el, litstart, p++);
continue;
}
re_putc(el, *p, 1);
}

elp->p_pos.v = el->el_refresh.r_cursor.v;
elp->p_pos.h = el->el_refresh.r_cursor.h;
}





libedit_private int
prompt_init(EditLine *el)
{

el->el_prompt.p_func = prompt_default;
el->el_prompt.p_pos.v = 0;
el->el_prompt.p_pos.h = 0;
el->el_prompt.p_ignore = '\0';
el->el_rprompt.p_func = prompt_default_r;
el->el_rprompt.p_pos.v = 0;
el->el_rprompt.p_pos.h = 0;
el->el_rprompt.p_ignore = '\0';
return 0;
}





libedit_private void

prompt_end(EditLine *el __attribute__((__unused__)))
{
}





libedit_private int
prompt_set(EditLine *el, el_pfunc_t prf, wchar_t c, int op, int wide)
{
el_prompt_t *p;

if (op == EL_PROMPT || op == EL_PROMPT_ESC)
p = &el->el_prompt;
else
p = &el->el_rprompt;

if (prf == NULL) {
if (op == EL_PROMPT || op == EL_PROMPT_ESC)
p->p_func = prompt_default;
else
p->p_func = prompt_default_r;
} else {
p->p_func = prf;
}

p->p_ignore = c;

p->p_pos.v = 0;
p->p_pos.h = 0;
p->p_wide = wide;

return 0;
}





libedit_private int
prompt_get(EditLine *el, el_pfunc_t *prf, wchar_t *c, int op)
{
el_prompt_t *p;

if (prf == NULL)
return -1;

if (op == EL_PROMPT)
p = &el->el_prompt;
else
p = &el->el_rprompt;

if (prf)
*prf = p->p_func;
if (c)
*c = p->p_ignore;

return 0;
}
