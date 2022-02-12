

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)vi.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: vi.c,v 1.64 2021/08/28 17:17:47 christos Exp $");
#endif
#endif




#include <sys/wait.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "el.h"
#include "common.h"
#include "emacs.h"
#include "fcns.h"
#include "vi.h"

static el_action_t cv_action(EditLine *, wint_t);
static el_action_t cv_paste(EditLine *, wint_t);




static el_action_t
cv_action(EditLine *el, wint_t c)
{

if (el->el_chared.c_vcmd.action != NOP) {

if (c != (wint_t)el->el_chared.c_vcmd.action)
return CC_ERROR;

if (!(c & YANK))
cv_undo(el);
cv_yank(el, el->el_line.buffer,
(int)(el->el_line.lastchar - el->el_line.buffer));
el->el_chared.c_vcmd.action = NOP;
el->el_chared.c_vcmd.pos = 0;
if (!(c & YANK)) {
el->el_line.lastchar = el->el_line.buffer;
el->el_line.cursor = el->el_line.buffer;
}
if (c & INSERT)
el->el_map.current = el->el_map.key;

return CC_REFRESH;
}
el->el_chared.c_vcmd.pos = el->el_line.cursor;
el->el_chared.c_vcmd.action = c;
return CC_ARGHACK;
}




static el_action_t
cv_paste(EditLine *el, wint_t c)
{
c_kill_t *k = &el->el_chared.c_kill;
size_t len = (size_t)(k->last - k->buf);

if (k->buf == NULL || len == 0)
return CC_ERROR;
#if defined(DEBUG_PASTE)
(void) fprintf(el->el_errfile, "Paste: \"%.*ls\"\n", (int)len,
k->buf);
#endif

cv_undo(el);

if (!c && el->el_line.cursor < el->el_line.lastchar)
el->el_line.cursor++;

c_insert(el, (int)len);
if (el->el_line.cursor + len > el->el_line.lastchar)
return CC_ERROR;
(void) memcpy(el->el_line.cursor, k->buf, len *
sizeof(*el->el_line.cursor));

return CC_REFRESH;
}






libedit_private el_action_t

vi_paste_next(EditLine *el, wint_t c __attribute__((__unused__)))
{

return cv_paste(el, 0);
}






libedit_private el_action_t

vi_paste_prev(EditLine *el, wint_t c __attribute__((__unused__)))
{

return cv_paste(el, 1);
}






libedit_private el_action_t

vi_prev_big_word(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor == el->el_line.buffer)
return CC_ERROR;

el->el_line.cursor = cv_prev_word(el->el_line.cursor,
el->el_line.buffer,
el->el_state.argument,
cv__isWord);

if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

vi_prev_word(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor == el->el_line.buffer)
return CC_ERROR;

el->el_line.cursor = cv_prev_word(el->el_line.cursor,
el->el_line.buffer,
el->el_state.argument,
cv__isword);

if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

vi_next_big_word(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor >= el->el_line.lastchar - 1)
return CC_ERROR;

el->el_line.cursor = cv_next_word(el, el->el_line.cursor,
el->el_line.lastchar, el->el_state.argument, cv__isWord);

if (el->el_map.type == MAP_VI)
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

vi_next_word(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor >= el->el_line.lastchar - 1)
return CC_ERROR;

el->el_line.cursor = cv_next_word(el, el->el_line.cursor,
el->el_line.lastchar, el->el_state.argument, cv__isword);

if (el->el_map.type == MAP_VI)
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t
vi_change_case(EditLine *el, wint_t c)
{
int i;

if (el->el_line.cursor >= el->el_line.lastchar)
return CC_ERROR;
cv_undo(el);
for (i = 0; i < el->el_state.argument; i++) {

c = *el->el_line.cursor;
if (iswupper(c))
*el->el_line.cursor = towlower(c);
else if (iswlower(c))
*el->el_line.cursor = towupper(c);

if (++el->el_line.cursor >= el->el_line.lastchar) {
el->el_line.cursor--;
re_fastaddc(el);
break;
}
re_fastaddc(el);
}
return CC_NORM;
}






libedit_private el_action_t

vi_change_meta(EditLine *el, wint_t c __attribute__((__unused__)))
{





return cv_action(el, DELETE | INSERT);
}






libedit_private el_action_t

vi_insert_at_bol(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_line.cursor = el->el_line.buffer;
cv_undo(el);
el->el_map.current = el->el_map.key;
return CC_CURSOR;
}






libedit_private el_action_t

vi_replace_char(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor >= el->el_line.lastchar)
return CC_ERROR;

el->el_map.current = el->el_map.key;
el->el_state.inputmode = MODE_REPLACE_1;
cv_undo(el);
return CC_ARGHACK;
}






libedit_private el_action_t

vi_replace_mode(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_map.current = el->el_map.key;
el->el_state.inputmode = MODE_REPLACE;
cv_undo(el);
return CC_NORM;
}






libedit_private el_action_t

vi_substitute_char(EditLine *el, wint_t c __attribute__((__unused__)))
{

c_delafter(el, el->el_state.argument);
el->el_map.current = el->el_map.key;
return CC_REFRESH;
}






libedit_private el_action_t

vi_substitute_line(EditLine *el, wint_t c __attribute__((__unused__)))
{

cv_undo(el);
cv_yank(el, el->el_line.buffer,
(int)(el->el_line.lastchar - el->el_line.buffer));
(void) em_kill_line(el, 0);
el->el_map.current = el->el_map.key;
return CC_REFRESH;
}






libedit_private el_action_t

vi_change_to_eol(EditLine *el, wint_t c __attribute__((__unused__)))
{

cv_undo(el);
cv_yank(el, el->el_line.cursor,
(int)(el->el_line.lastchar - el->el_line.cursor));
(void) ed_kill_line(el, 0);
el->el_map.current = el->el_map.key;
return CC_REFRESH;
}






libedit_private el_action_t

vi_insert(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_map.current = el->el_map.key;
cv_undo(el);
return CC_NORM;
}






libedit_private el_action_t

vi_add(EditLine *el, wint_t c __attribute__((__unused__)))
{
int ret;

el->el_map.current = el->el_map.key;
if (el->el_line.cursor < el->el_line.lastchar) {
el->el_line.cursor++;
if (el->el_line.cursor > el->el_line.lastchar)
el->el_line.cursor = el->el_line.lastchar;
ret = CC_CURSOR;
} else
ret = CC_NORM;

cv_undo(el);

return (el_action_t)ret;
}






libedit_private el_action_t

vi_add_at_eol(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_map.current = el->el_map.key;
el->el_line.cursor = el->el_line.lastchar;
cv_undo(el);
return CC_CURSOR;
}






libedit_private el_action_t

vi_delete_meta(EditLine *el, wint_t c __attribute__((__unused__)))
{

return cv_action(el, DELETE);
}






libedit_private el_action_t

vi_end_big_word(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor == el->el_line.lastchar)
return CC_ERROR;

el->el_line.cursor = cv__endword(el->el_line.cursor,
el->el_line.lastchar, el->el_state.argument, cv__isWord);

if (el->el_chared.c_vcmd.action != NOP) {
el->el_line.cursor++;
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

vi_end_word(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor == el->el_line.lastchar)
return CC_ERROR;

el->el_line.cursor = cv__endword(el->el_line.cursor,
el->el_line.lastchar, el->el_state.argument, cv__isword);

if (el->el_chared.c_vcmd.action != NOP) {
el->el_line.cursor++;
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

vi_undo(EditLine *el, wint_t c __attribute__((__unused__)))
{
c_undo_t un = el->el_chared.c_undo;

if (un.len == -1)
return CC_ERROR;


el->el_chared.c_undo.buf = el->el_line.buffer;
el->el_chared.c_undo.len = el->el_line.lastchar - el->el_line.buffer;
el->el_chared.c_undo.cursor =
(int)(el->el_line.cursor - el->el_line.buffer);
el->el_line.limit = un.buf + (el->el_line.limit - el->el_line.buffer);
el->el_line.buffer = un.buf;
el->el_line.cursor = un.buf + un.cursor;
el->el_line.lastchar = un.buf + un.len;

return CC_REFRESH;
}






libedit_private el_action_t

vi_command_mode(EditLine *el, wint_t c __attribute__((__unused__)))
{


el->el_chared.c_vcmd.action = NOP;
el->el_chared.c_vcmd.pos = 0;

el->el_state.doingarg = 0;

el->el_state.inputmode = MODE_INSERT;
el->el_map.current = el->el_map.alt;
#if defined(VI_MOVE)
if (el->el_line.cursor > el->el_line.buffer)
el->el_line.cursor--;
#endif
return CC_CURSOR;
}






libedit_private el_action_t
vi_zero(EditLine *el, wint_t c)
{

if (el->el_state.doingarg)
return ed_argument_digit(el, c);

el->el_line.cursor = el->el_line.buffer;
if (el->el_chared.c_vcmd.action != NOP) {
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}






libedit_private el_action_t

vi_delete_prev_char(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_line.cursor <= el->el_line.buffer)
return CC_ERROR;

c_delbefore1(el);
el->el_line.cursor--;
return CC_REFRESH;
}






libedit_private el_action_t

vi_list_or_eof(EditLine *el, wint_t c)
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
#if defined(notyet)
re_goto_bottom(el);
*el->el_line.lastchar = '\0';
return CC_LIST_CHOICES;
#else



terminal_beep(el);
return CC_ERROR;
#endif
}
}






libedit_private el_action_t

vi_kill_line_prev(EditLine *el, wint_t c __attribute__((__unused__)))
{
wchar_t *kp, *cp;

cp = el->el_line.buffer;
kp = el->el_chared.c_kill.buf;
while (cp < el->el_line.cursor)
*kp++ = *cp++;
el->el_chared.c_kill.last = kp;
c_delbefore(el, (int)(el->el_line.cursor - el->el_line.buffer));
el->el_line.cursor = el->el_line.buffer;
return CC_REFRESH;
}






libedit_private el_action_t

vi_search_prev(EditLine *el, wint_t c __attribute__((__unused__)))
{

return cv_search(el, ED_SEARCH_PREV_HISTORY);
}






libedit_private el_action_t

vi_search_next(EditLine *el, wint_t c __attribute__((__unused__)))
{

return cv_search(el, ED_SEARCH_NEXT_HISTORY);
}






libedit_private el_action_t

vi_repeat_search_next(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_search.patlen == 0)
return CC_ERROR;
else
return cv_repeat_srch(el, el->el_search.patdir);
}







libedit_private el_action_t
vi_repeat_search_prev(EditLine *el, wint_t c __attribute__((__unused__)))
{

if (el->el_search.patlen == 0)
return CC_ERROR;
else
return (cv_repeat_srch(el,
el->el_search.patdir == ED_SEARCH_PREV_HISTORY ?
ED_SEARCH_NEXT_HISTORY : ED_SEARCH_PREV_HISTORY));
}






libedit_private el_action_t

vi_next_char(EditLine *el, wint_t c __attribute__((__unused__)))
{
return cv_csearch(el, CHAR_FWD, -1, el->el_state.argument, 0);
}






libedit_private el_action_t

vi_prev_char(EditLine *el, wint_t c __attribute__((__unused__)))
{
return cv_csearch(el, CHAR_BACK, -1, el->el_state.argument, 0);
}






libedit_private el_action_t

vi_to_next_char(EditLine *el, wint_t c __attribute__((__unused__)))
{
return cv_csearch(el, CHAR_FWD, -1, el->el_state.argument, 1);
}






libedit_private el_action_t

vi_to_prev_char(EditLine *el, wint_t c __attribute__((__unused__)))
{
return cv_csearch(el, CHAR_BACK, -1, el->el_state.argument, 1);
}






libedit_private el_action_t

vi_repeat_next_char(EditLine *el, wint_t c __attribute__((__unused__)))
{

return cv_csearch(el, el->el_search.chadir, el->el_search.chacha,
el->el_state.argument, el->el_search.chatflg);
}






libedit_private el_action_t

vi_repeat_prev_char(EditLine *el, wint_t c __attribute__((__unused__)))
{
el_action_t r;
int dir = el->el_search.chadir;

r = cv_csearch(el, -dir, el->el_search.chacha,
el->el_state.argument, el->el_search.chatflg);
el->el_search.chadir = dir;
return r;
}






libedit_private el_action_t

vi_match(EditLine *el, wint_t c __attribute__((__unused__)))
{
const wchar_t match_chars[] = L"()[]{}";
wchar_t *cp;
size_t delta, i, count;
wchar_t o_ch, c_ch;

*el->el_line.lastchar = '\0';

i = wcscspn(el->el_line.cursor, match_chars);
o_ch = el->el_line.cursor[i];
if (o_ch == 0)
return CC_ERROR;
delta = (size_t)(wcschr(match_chars, o_ch) - match_chars);
c_ch = match_chars[delta ^ 1];
count = 1;
delta = 1 - (delta & 1) * 2;

for (cp = &el->el_line.cursor[i]; count; ) {
cp += delta;
if (cp < el->el_line.buffer || cp >= el->el_line.lastchar)
return CC_ERROR;
if (*cp == o_ch)
count++;
else if (*cp == c_ch)
count--;
}

el->el_line.cursor = cp;

if (el->el_chared.c_vcmd.action != NOP) {


if (delta > 0)
el->el_line.cursor++;
cv_delfini(el);
return CC_REFRESH;
}
return CC_CURSOR;
}





libedit_private el_action_t

vi_undo_line(EditLine *el, wint_t c __attribute__((__unused__)))
{

cv_undo(el);
return hist_get(el);
}






libedit_private el_action_t

vi_to_column(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_line.cursor = el->el_line.buffer;
el->el_state.argument--;
return ed_next_char(el, 0);
}





libedit_private el_action_t

vi_yank_end(EditLine *el, wint_t c __attribute__((__unused__)))
{

cv_yank(el, el->el_line.cursor,
(int)(el->el_line.lastchar - el->el_line.cursor));
return CC_REFRESH;
}





libedit_private el_action_t

vi_yank(EditLine *el, wint_t c __attribute__((__unused__)))
{

return cv_action(el, YANK);
}





libedit_private el_action_t

vi_comment_out(EditLine *el, wint_t c __attribute__((__unused__)))
{

el->el_line.cursor = el->el_line.buffer;
c_insert(el, 1);
*el->el_line.cursor = '#';
re_refresh(el);
return ed_newline(el, 0);
}







libedit_private el_action_t

vi_alias(EditLine *el, wint_t c __attribute__((__unused__)))
{
char alias_name[3];
const char *alias_text;

if (el->el_chared.c_aliasfun == NULL)
return CC_ERROR;

alias_name[0] = '_';
alias_name[2] = 0;
if (el_getc(el, &alias_name[1]) != 1)
return CC_ERROR;

alias_text = (*el->el_chared.c_aliasfun)(el->el_chared.c_aliasarg,
alias_name);
if (alias_text != NULL)
el_wpush(el, ct_decode_string(alias_text, &el->el_scratch));
return CC_NORM;
}





libedit_private el_action_t

vi_to_history_line(EditLine *el, wint_t c __attribute__((__unused__)))
{
int sv_event_no = el->el_history.eventno;
el_action_t rval;


if (el->el_history.eventno == 0) {
(void) wcsncpy(el->el_history.buf, el->el_line.buffer,
EL_BUFSIZ);
el->el_history.last = el->el_history.buf +
(el->el_line.lastchar - el->el_line.buffer);
}


if (!el->el_state.doingarg) {
el->el_history.eventno = 0x7fffffff;
hist_get(el);
} else {





el->el_history.eventno = 1;
if (hist_get(el) == CC_ERROR)
return CC_ERROR;
el->el_history.eventno = 1 + el->el_history.ev.num
- el->el_state.argument;
if (el->el_history.eventno < 0) {
el->el_history.eventno = sv_event_no;
return CC_ERROR;
}
}
rval = hist_get(el);
if (rval == CC_ERROR)
el->el_history.eventno = sv_event_no;
return rval;
}





libedit_private el_action_t

vi_histedit(EditLine *el, wint_t c __attribute__((__unused__)))
{
int fd;
pid_t pid;
ssize_t st;
int status;
char tempfile[] = "/tmp/histedit.XXXXXXXXXX";
char *cp = NULL;
size_t len;
wchar_t *line = NULL;
const char *editor;

if (el->el_state.doingarg) {
if (vi_to_history_line(el, 0) == CC_ERROR)
return CC_ERROR;
}

if ((editor = getenv("EDITOR")) == NULL)
editor = "vi";
fd = mkstemp(tempfile);
if (fd < 0)
return CC_ERROR;
len = (size_t)(el->el_line.lastchar - el->el_line.buffer);
#define TMP_BUFSIZ (EL_BUFSIZ * MB_LEN_MAX)
cp = el_calloc(TMP_BUFSIZ, sizeof(*cp));
if (cp == NULL)
goto error;
line = el_calloc(len + 1, sizeof(*line));
if (line == NULL)
goto error;
wcsncpy(line, el->el_line.buffer, len);
line[len] = '\0';
wcstombs(cp, line, TMP_BUFSIZ - 1);
cp[TMP_BUFSIZ - 1] = '\0';
len = strlen(cp);
write(fd, cp, len);
write(fd, "\n", (size_t)1);
pid = fork();
switch (pid) {
case -1:
goto error;
case 0:
close(fd);
execlp(editor, editor, tempfile, (char *)NULL);
exit(0);

default:
while (waitpid(pid, &status, 0) != pid)
continue;
lseek(fd, (off_t)0, SEEK_SET);
st = read(fd, cp, TMP_BUFSIZ - 1);
if (st > 0) {
cp[st] = '\0';
len = (size_t)(el->el_line.limit - el->el_line.buffer);
len = mbstowcs(el->el_line.buffer, cp, len);
if (len > 0 && el->el_line.buffer[len - 1] == '\n')
--len;
}
else
len = 0;
el->el_line.cursor = el->el_line.buffer;
el->el_line.lastchar = el->el_line.buffer + len;
el_free(cp);
el_free(line);
break;
}

close(fd);
unlink(tempfile);

return ed_newline(el, 0);
error:
el_free(line);
el_free(cp);
close(fd);
unlink(tempfile);
return CC_ERROR;
}







libedit_private el_action_t

vi_history_word(EditLine *el, wint_t c __attribute__((__unused__)))
{
const wchar_t *wp = HIST_FIRST(el);
const wchar_t *wep, *wsp;
int len;
wchar_t *cp;
const wchar_t *lim;

if (wp == NULL)
return CC_ERROR;

wep = wsp = NULL;
do {
while (iswspace(*wp))
wp++;
if (*wp == 0)
break;
wsp = wp;
while (*wp && !iswspace(*wp))
wp++;
wep = wp;
} while ((!el->el_state.doingarg || --el->el_state.argument > 0)
&& *wp != 0);

if (wsp == NULL || (el->el_state.doingarg && el->el_state.argument != 0))
return CC_ERROR;

cv_undo(el);
len = (int)(wep - wsp);
if (el->el_line.cursor < el->el_line.lastchar)
el->el_line.cursor++;
c_insert(el, len + 1);
cp = el->el_line.cursor;
lim = el->el_line.limit;
if (cp < lim)
*cp++ = ' ';
while (wsp < wep && cp < lim)
*cp++ = *wsp++;
el->el_line.cursor = cp;

el->el_map.current = el->el_map.key;
return CC_REFRESH;
}





libedit_private el_action_t

vi_redo(EditLine *el, wint_t c __attribute__((__unused__)))
{
c_redo_t *r = &el->el_chared.c_redo;

if (!el->el_state.doingarg && r->count) {
el->el_state.doingarg = 1;
el->el_state.argument = r->count;
}

el->el_chared.c_vcmd.pos = el->el_line.cursor;
el->el_chared.c_vcmd.action = r->action;
if (r->pos != r->buf) {
if (r->pos + 1 > r->lim)

r->pos = r->lim - 1;
r->pos[0] = 0;
el_wpush(el, r->buf);
}

el->el_state.thiscmd = r->cmd;
el->el_state.thisch = r->ch;
return (*el->el_map.func[r->cmd])(el, r->ch);
}
