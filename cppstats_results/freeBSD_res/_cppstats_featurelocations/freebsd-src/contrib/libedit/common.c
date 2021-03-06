

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)common.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: common.c,v 1.49 2020/03/30 06:54:37 ryo Exp $");
#endif
#endif




#include <ctype.h>
#include <string.h>

#include "el.h"
#include "common.h"
#include "fcns.h"
#include "parse.h"
#include "vi.h"





libedit_private el_action_t

ed_end_of_file(EditLine *el, wint_t c __attribute__((__unused__)))
{

re_goto_bottom(el);
*el->el_line.lastchar = '\0';
return CC_EOF;
}






libedit_private el_action_t
ed_insert(EditLine *el, wint_t c)
{
int count = el->el_state.argument;

if (c == '\0')
return CC_ERROR;

if (el->el_line.lastchar + el->el_state.argument >=
el->el_line.limit) {

if (!ch_enlargebufs(el, (size_t) count))
return CC_ERROR;
}

if (count == 1) {
if (el->el_state.inputmode == MODE_INSERT
|| el->el_line.cursor >= el->el_line.lastchar)
c_insert(el, 1);

*el->el_line.cursor++ = c;
re_fastaddc(el);
} else {
if (el->el_state.inputmode != MODE_REPLACE_1)
c_insert(el, el->el_state.argument);

while (count-- && el->el_line.cursor < el->el_line.lastchar)
*el->el_line.cursor++ = c;
re_refresh(el);
}

if (el->el_state.inputmode == MODE_REPLACE_1)
return vi_command_mode(el, 0);

return CC_NORM;
}






libedit_private el_action_t

ed_delete_prev_word(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *cp, *p, *kp;

if (el->el_line.cursor == el->el_line.buffer)
return CC_ERROR;

cp = c__prev_word(el->el_line.cursor, el->el_line.buffer,
el->el_state.argument, ce__isword);

for (p = cp, kp = el->el_chared.c_kill.buf; p < el->el_line.cursor; p++)
*kp++ = *p;
el->el_chared.c_kill.last = kp;

c_delbefore(el, (int)(el->el_line.cursor - cp));
el->el_line.cursor = cp;
if (el->el_line.cursor < el->el_line.buffer)
el->el_line.cursor = el->el_line.buffer;
return CC_REFRESH;
}






libedit_private el_action_t

ed_delete_next_char(EditLine *el, wint_t c __attribute__((__unused__)))
{
#if defined(DEBUG_EDIT)
#define EL el->el_line
(void) fprintf(el->el_errfile,
"\nD(b: %p(%ls) c: %p(%ls) last: %p(%ls) limit: %p(%ls)\n",
EL.buffer, EL.buffer, EL.cursor, EL.cursor, EL.lastchar,
EL.lastchar, EL.limit, EL.limit);
#endif
if (el->el_line.cursor == el->el_line.lastchar) {

if (el->el_map.type == MAP_VI) {
if (el->el_line.cursor == el->el_line.buffer) {

#if defined(KSHVI)
return CC_ERROR;
#else

terminal_writec(el, c);
return CC_EOF;
#endif
} else {
#if defined(KSHVI)
el->el_line.cursor--;
#else
return CC_ERROR;
#endif
}
} else
return CC_ERROR;
}
c_delafter(el, el->el_state.argument);
if (el->el_map.type == MAP_VI &&
el->el_line.cursor >= el->el_line.lastchar &&
el->el_line.cursor > el->el_line.buffer)

el->el_line.cursor = el->el_line.lastchar - 1;
return CC_REFRESH;
}






libedit_private el_action_t

ed_kill_line(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *kp, *cp;

cp = el->el_line.cursor;
kp = el->el_chared.c_kill.buf;
while (cp < el->el_line.lastchar)
*kp++ = *cp++;
el->el_chared.c_kill.last = kp;

el->el_line.lastchar = el->el_line.cursor;
return CC_REFRESH;
}






libedit_private el_action_t

ed_move_to_end(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_line.cursor = el->el_line.lastchar;
if (el->el_map.type == MAP_VI) {
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
#if defined(VI_MOVE)
el->el_line.cursor--;
#endif
}
return CC_CURSOR;
}






libedit_private el_action_t

ed_move_to_beg(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_line.cursor = el->el_line.buffer;

if (el->el_map.type == MAP_VI) {

while (iswspace(*el->el_line.cursor))
el->el_line.cursor++;
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
}
return CC_CURSOR;
}






libedit_private el_action_t
ed_transpose_chars(EditLine *el, wint_t c)
{

if (el->el_line.cursor < el->el_line.lastchar) {
if (el->el_line.lastchar <= &el->el_line.buffer[1])
return CC_ERROR;
else
el->el_line.cursor++;
}
if (el->el_line.cursor > &el->el_line.buffer[1]) {

c = el->el_line.cursor[-2];
el->el_line.cursor[-2] = el->el_line.cursor[-1];
el->el_line.cursor[-1] = c;
return CC_REFRESH;
} else
return CC_ERROR;
}






libedit_private el_action_t

ed_next_char(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *lim = el->el_line.lastchar;

if (el->el_line.cursor >= lim ||
(el->el_line.cursor == lim - 1 &&
el->el_map.type == MAP_VI &&
el->el_chared.c_vcmd.action == NOP))
return CC_ERROR;

el->el_line.cursor += el->el_state.argument;
if (el->el_line.cursor > lim)
el->el_line.cursor = lim;

if (el->el_map.type == MAP_VI)
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

ed_prev_word(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor == el->el_line.buffer)
return CC_ERROR;

el->el_line.cursor = c__prev_word(el->el_line.cursor,
el->el_line.buffer,
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

ed_prev_char(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor > el->el_line.buffer) {
el->el_line.cursor -= el->el_state.argument;
if (el->el_line.cursor < el->el_line.buffer)
el->el_line.cursor = el->el_line.buffer;

if (el->el_map.type == MAP_VI)
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
} else
return CC_ERROR;
}






libedit_private el_action_t

ed_quoted_insert(EditLine *el, wint_t c __attribute__((__unused__)))
{
int num;
wchar_t ch;

tty_quotemode(el);
num = el_wgetc(el, &ch);
tty_noquotemode(el);
if (num == 1)
return ed_insert(el, ch);
else
return ed_end_of_file(el, 0);
}





libedit_private el_action_t
ed_digit(EditLine *el, wint_t c)
{

if (!iswdigit(c))
return CC_ERROR;

if (el->el_state.doingarg) {

if (el->el_state.lastcmd == EM_UNIVERSAL_ARGUMENT)
el->el_state.argument = c - '0';
else {
if (el->el_state.argument > 1000000)
return CC_ERROR;
el->el_state.argument =
(el->el_state.argument * 10) + (c - '0');
}
return CC_ARGHACK;
}

return ed_insert(el, c);
}






libedit_private el_action_t
ed_argument_digit(EditLine *el, wint_t c)
{

if (!iswdigit(c))
return CC_ERROR;

if (el->el_state.doingarg) {
if (el->el_state.argument > 1000000)
return CC_ERROR;
el->el_state.argument = (el->el_state.argument * 10) +
(c - '0');
} else {
el->el_state.argument = c - '0';
el->el_state.doingarg = 1;
}
return CC_ARGHACK;
}






libedit_private el_action_t

ed_unassigned(EditLine *el __attribute__((__unused__)),
wint_t c __attribute__((__unused__)))
{

return CC_ERROR;
}






libedit_private el_action_t

ed_ignore(EditLine *el __attribute__((__unused__)),
wint_t c __attribute__((__unused__)))
{

return CC_NORM;
}






libedit_private el_action_t

ed_newline(EditLine *el, wint_t c __attribute__((__unused__)))
{

re_goto_bottom(el);
*el->el_line.lastchar++ = '\n';
*el->el_line.lastchar = '\0';
return CC_NEWLINE;
}






libedit_private el_action_t

ed_delete_prev_char(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor <= el->el_line.buffer)
return CC_ERROR;

c_delbefore(el, el->el_state.argument);
el->el_line.cursor -= el->el_state.argument;
if (el->el_line.cursor < el->el_line.buffer)
el->el_line.cursor = el->el_line.buffer;
return CC_REFRESH;
}






libedit_private el_action_t

ed_clear_screen(EditLine *el, wint_t c __attribute__((__unused__)))
{

terminal_clear_screen(el);
re_clear_display(el);
return CC_REFRESH;
}






libedit_private el_action_t

ed_redisplay(EditLine *el __attribute__((__unused__)),
wint_t c __attribute__((__unused__)))
{

return CC_REDISPLAY;
}






libedit_private el_action_t

ed_start_over(EditLine *el, wint_t c __attribute__((__unused__)))
{

ch_reset(el);
return CC_REFRESH;
}






libedit_private el_action_t

ed_sequence_lead_in(EditLine *el __attribute__((__unused__)),
wint_t c __attribute__((__unused__)))
{

return CC_NORM;
}






libedit_private el_action_t

ed_prev_history(EditLine *el, wint_t c __attribute__((__unused__)))
{
char beep = 0;
int sv_event = el->el_history.eventno;

el->el_chared.c_undo.len = -1;
*el->el_line.lastchar = '\0';

if (el->el_history.eventno == 0) {

(void) wcsncpy(el->el_history.buf, el->el_line.buffer,
EL_BUFSIZ);
el->el_history.last = el->el_history.buf +
(el->el_line.lastchar - el->el_line.buffer);
}
el->el_history.eventno += el->el_state.argument;

if (hist_get(el) == CC_ERROR) {
if (el->el_map.type == MAP_VI) {
el->el_history.eventno = sv_event;
}
beep = 1;

(void) hist_get(el);
}
if (beep)
return CC_REFRESH_BEEP;
return CC_REFRESH;
}






libedit_private el_action_t

ed_next_history(EditLine *el, wint_t c __attribute__((__unused__)))
{
el_action_t beep = CC_REFRESH, rval;

el->el_chared.c_undo.len = -1;
*el->el_line.lastchar = '\0';

el->el_history.eventno -= el->el_state.argument;

if (el->el_history.eventno < 0) {
el->el_history.eventno = 0;
beep = CC_REFRESH_BEEP;
}
rval = hist_get(el);
if (rval == CC_REFRESH)
return beep;
return rval;

}






libedit_private el_action_t

ed_search_prev_history(EditLine *el, wint_t c __attribute__((__unused__)))
{
const wchar_t *hp;
int h;
int found = 0;

el->el_chared.c_vcmd.action = NOP;
el->el_chared.c_undo.len = -1;
*el->el_line.lastchar = '\0';
if (el->el_history.eventno < 0) {
#if defined(DEBUG_EDIT)
(void) fprintf(el->el_errfile,
"e_prev_search_hist(): eventno < 0;\n");
#endif
el->el_history.eventno = 0;
return CC_ERROR;
}
if (el->el_history.eventno == 0) {
(void) wcsncpy(el->el_history.buf, el->el_line.buffer,
EL_BUFSIZ);
el->el_history.last = el->el_history.buf +
(el->el_line.lastchar - el->el_line.buffer);
}
if (el->el_history.ref == NULL)
return CC_ERROR;

hp = HIST_FIRST(el);
if (hp == NULL)
return CC_ERROR;

c_setpat(el);

for (h = 1; h <= el->el_history.eventno; h++)
hp = HIST_NEXT(el);

while (hp != NULL) {
#if defined(SDEBUG)
(void) fprintf(el->el_errfile, "Comparing with \"%ls\"\n", hp);
#endif
if ((wcsncmp(hp, el->el_line.buffer, (size_t)
(el->el_line.lastchar - el->el_line.buffer)) ||
hp[el->el_line.lastchar - el->el_line.buffer]) &&
c_hmatch(el, hp)) {
found = 1;
break;
}
h++;
hp = HIST_NEXT(el);
}

if (!found) {
#if defined(SDEBUG)
(void) fprintf(el->el_errfile, "not found\n");
#endif
return CC_ERROR;
}
el->el_history.eventno = h;

return hist_get(el);
}






libedit_private el_action_t

ed_search_next_history(EditLine *el, wint_t c __attribute__((__unused__)))
{
const wchar_t *hp;
int h;
int found = 0;

el->el_chared.c_vcmd.action = NOP;
el->el_chared.c_undo.len = -1;
*el->el_line.lastchar = '\0';

if (el->el_history.eventno == 0)
return CC_ERROR;

if (el->el_history.ref == NULL)
return CC_ERROR;

hp = HIST_FIRST(el);
if (hp == NULL)
return CC_ERROR;

c_setpat(el);

for (h = 1; h < el->el_history.eventno && hp; h++) {
#if defined(SDEBUG)
(void) fprintf(el->el_errfile, "Comparing with \"%ls\"\n", hp);
#endif
if ((wcsncmp(hp, el->el_line.buffer, (size_t)
(el->el_line.lastchar - el->el_line.buffer)) ||
hp[el->el_line.lastchar - el->el_line.buffer]) &&
c_hmatch(el, hp))
found = h;
hp = HIST_NEXT(el);
}

if (!found) {
if (!c_hmatch(el, el->el_history.buf)) {
#if defined(SDEBUG)
(void) fprintf(el->el_errfile, "not found\n");
#endif
return CC_ERROR;
}
}
el->el_history.eventno = found;

return hist_get(el);
}






libedit_private el_action_t

ed_prev_line(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *ptr;
int nchars = c_hpos(el);




if (*(ptr = el->el_line.cursor) == '\n')
ptr--;

for (; ptr >= el->el_line.buffer; ptr--)
if (*ptr == '\n' && --el->el_state.argument <= 0)
break;

if (el->el_state.argument > 0)
return CC_ERROR;




for (ptr--; ptr >= el->el_line.buffer && *ptr != '\n'; ptr--)
continue;




for (ptr++;
nchars-- > 0 && ptr < el->el_line.lastchar && *ptr != '\n';
ptr++)
continue;

el->el_line.cursor = ptr;
return CC_CURSOR;
}






libedit_private el_action_t

ed_next_line(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *ptr;
int nchars = c_hpos(el);




for (ptr = el->el_line.cursor; ptr < el->el_line.lastchar; ptr++)
if (*ptr == '\n' && --el->el_state.argument <= 0)
break;

if (el->el_state.argument > 0)
return CC_ERROR;




for (ptr++;
nchars-- > 0 && ptr < el->el_line.lastchar && *ptr != '\n';
ptr++)
continue;

el->el_line.cursor = ptr;
return CC_CURSOR;
}






libedit_private el_action_t

ed_command(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t tmpbuf[EL_BUFSIZ];
int tmplen;

tmplen = c_gets(el, tmpbuf, L"\n: ");
terminal__putc(el, '\n');

if (tmplen < 0 || (tmpbuf[tmplen] = 0, parse_line(el, tmpbuf)) == -1)
terminal_beep(el);

el->el_map.current = el->el_map.key;
re_clear_display(el);
return CC_REFRESH;
}
