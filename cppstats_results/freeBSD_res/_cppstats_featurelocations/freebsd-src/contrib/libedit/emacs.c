

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)emacs.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: emacs.c,v 1.36 2016/05/09 21:46:56 christos Exp $");
#endif
#endif




#include <ctype.h>

#include "el.h"
#include "emacs.h"
#include "fcns.h"





libedit_private el_action_t

em_delete_or_list(EditLine *el, wint_t c)
{

if (el->el_line.cursor == el->el_line.lastchar) {

if (el->el_line.cursor == el->el_line.buffer) {

terminal_writec(el, c);
return CC_EOF;
} else {




terminal_beep(el);
return CC_ERROR;
}
} else {
if (el->el_state.doingarg)
c_delafter(el, el->el_state.argument);
else
c_delafter1(el);
if (el->el_line.cursor > el->el_line.lastchar)
el->el_line.cursor = el->el_line.lastchar;

return CC_REFRESH;
}
}






libedit_private el_action_t

em_delete_next_word(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *cp, *p, *kp;

if (el->el_line.cursor == el->el_line.lastchar)
return CC_ERROR;

cp = c__next_word(el->el_line.cursor, el->el_line.lastchar,
el->el_state.argument, ce__isword);

for (p = el->el_line.cursor, kp = el->el_chared.c_kill.buf; p < cp; p++)

*kp++ = *p;
el->el_chared.c_kill.last = kp;

c_delafter(el, (int)(cp - el->el_line.cursor));
if (el->el_line.cursor > el->el_line.lastchar)
el->el_line.cursor = el->el_line.lastchar;

return CC_REFRESH;
}






libedit_private el_action_t

em_yank(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *kp, *cp;

if (el->el_chared.c_kill.last == el->el_chared.c_kill.buf)
return CC_NORM;

if (el->el_line.lastchar +
(el->el_chared.c_kill.last - el->el_chared.c_kill.buf) >=
el->el_line.limit)
return CC_ERROR;

el->el_chared.c_kill.mark = el->el_line.cursor;
cp = el->el_line.cursor;


c_insert(el,
(int)(el->el_chared.c_kill.last - el->el_chared.c_kill.buf));

for (kp = el->el_chared.c_kill.buf; kp < el->el_chared.c_kill.last; kp++)
*cp++ = *kp;


if (el->el_state.argument == 1)
el->el_line.cursor = cp;

return CC_REFRESH;
}






libedit_private el_action_t

em_kill_line(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *kp, *cp;

cp = el->el_line.buffer;
kp = el->el_chared.c_kill.buf;
while (cp < el->el_line.lastchar)
*kp++ = *cp++;
el->el_chared.c_kill.last = kp;

el->el_line.lastchar = el->el_line.buffer;
el->el_line.cursor = el->el_line.buffer;
return CC_REFRESH;
}






libedit_private el_action_t

em_kill_region(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *kp, *cp;

if (!el->el_chared.c_kill.mark)
return CC_ERROR;

if (el->el_chared.c_kill.mark > el->el_line.cursor) {
cp = el->el_line.cursor;
kp = el->el_chared.c_kill.buf;
while (cp < el->el_chared.c_kill.mark)
*kp++ = *cp++;
el->el_chared.c_kill.last = kp;
c_delafter(el, (int)(cp - el->el_line.cursor));
} else {
cp = el->el_chared.c_kill.mark;
kp = el->el_chared.c_kill.buf;
while (cp < el->el_line.cursor)
*kp++ = *cp++;
el->el_chared.c_kill.last = kp;
c_delbefore(el, (int)(cp - el->el_chared.c_kill.mark));
el->el_line.cursor = el->el_chared.c_kill.mark;
}
return CC_REFRESH;
}






libedit_private el_action_t

em_copy_region(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *kp, *cp;

if (!el->el_chared.c_kill.mark)
return CC_ERROR;

if (el->el_chared.c_kill.mark > el->el_line.cursor) {
cp = el->el_line.cursor;
kp = el->el_chared.c_kill.buf;
while (cp < el->el_chared.c_kill.mark)
*kp++ = *cp++;
el->el_chared.c_kill.last = kp;
} else {
cp = el->el_chared.c_kill.mark;
kp = el->el_chared.c_kill.buf;
while (cp < el->el_line.cursor)
*kp++ = *cp++;
el->el_chared.c_kill.last = kp;
}
return CC_NORM;
}






libedit_private el_action_t
em_gosmacs_transpose(EditLine *el, wint_t c)
{

if (el->el_line.cursor > &el->el_line.buffer[1]) {

c = el->el_line.cursor[-2];
el->el_line.cursor[-2] = el->el_line.cursor[-1];
el->el_line.cursor[-1] = c;
return CC_REFRESH;
} else
return CC_ERROR;
}






libedit_private el_action_t

em_next_word(EditLine *el, wint_t c __attribute__((__unused__)))
{
if (el->el_line.cursor == el->el_line.lastchar)
return CC_ERROR;

el->el_line.cursor = c__next_word(el->el_line.cursor,
el->el_line.lastchar,
el->el_state.argument,
ce__isword);

if (el->el_map.type == MAP_VI)
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

em_upper_case(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *cp, *ep;

ep = c__next_word(el->el_line.cursor, el->el_line.lastchar,
el->el_state.argument, ce__isword);

for (cp = el->el_line.cursor; cp < ep; cp++)
if (iswlower(*cp))
*cp = towupper(*cp);

el->el_line.cursor = ep;
if (el->el_line.cursor > el->el_line.lastchar)
el->el_line.cursor = el->el_line.lastchar;
return CC_REFRESH;
}






libedit_private el_action_t

em_capitol_case(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *cp, *ep;

ep = c__next_word(el->el_line.cursor, el->el_line.lastchar,
el->el_state.argument, ce__isword);

for (cp = el->el_line.cursor; cp < ep; cp++) {
if (iswalpha(*cp)) {
if (iswlower(*cp))
*cp = towupper(*cp);
cp++;
break;
}
}
for (; cp < ep; cp++)
if (iswupper(*cp))
*cp = towlower(*cp);

el->el_line.cursor = ep;
if (el->el_line.cursor > el->el_line.lastchar)
el->el_line.cursor = el->el_line.lastchar;
return CC_REFRESH;
}






libedit_private el_action_t

em_lower_case(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *cp, *ep;

ep = c__next_word(el->el_line.cursor, el->el_line.lastchar,
el->el_state.argument, ce__isword);

for (cp = el->el_line.cursor; cp < ep; cp++)
if (iswupper(*cp))
*cp = towlower(*cp);

el->el_line.cursor = ep;
if (el->el_line.cursor > el->el_line.lastchar)
el->el_line.cursor = el->el_line.lastchar;
return CC_REFRESH;
}






libedit_private el_action_t

em_set_mark(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_chared.c_kill.mark = el->el_line.cursor;
return CC_NORM;
}






libedit_private el_action_t

em_exchange_mark(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *cp;

cp = el->el_line.cursor;
el->el_line.cursor = el->el_chared.c_kill.mark;
el->el_chared.c_kill.mark = cp;
return CC_CURSOR;
}






libedit_private el_action_t

em_universal_argument(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_state.argument > 1000000)
return CC_ERROR;
el->el_state.doingarg = 1;
el->el_state.argument *= 4;
return CC_ARGHACK;
}






libedit_private el_action_t

em_meta_next(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_state.metanext = 1;
return CC_ARGHACK;
}





libedit_private el_action_t

em_toggle_overwrite(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_state.inputmode = (el->el_state.inputmode == MODE_INSERT) ?
MODE_REPLACE : MODE_INSERT;
return CC_NORM;
}





libedit_private el_action_t

em_copy_prev_word(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *cp, *oldc, *dp;

if (el->el_line.cursor == el->el_line.buffer)
return CC_ERROR;

oldc = el->el_line.cursor;

cp = c__prev_word(el->el_line.cursor, el->el_line.buffer,
el->el_state.argument, ce__isword);

c_insert(el, (int)(oldc - cp));
for (dp = oldc; cp < oldc && dp < el->el_line.lastchar; cp++)
*dp++ = *cp;

el->el_line.cursor = dp;

return CC_REFRESH;
}





libedit_private el_action_t

em_inc_search_next(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_search.patlen = 0;
return ce_inc_search(el, ED_SEARCH_NEXT_HISTORY);
}





libedit_private el_action_t

em_inc_search_prev(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_search.patlen = 0;
return ce_inc_search(el, ED_SEARCH_PREV_HISTORY);
}






libedit_private el_action_t

em_delete_prev_char(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor <= el->el_line.buffer)
return CC_ERROR;

if (el->el_state.doingarg)
c_delbefore(el, el->el_state.argument);
else
c_delbefore1(el);
el->el_line.cursor -= el->el_state.argument;
if (el->el_line.cursor < el->el_line.buffer)
el->el_line.cursor = el->el_line.buffer;
return CC_REFRESH;
}
