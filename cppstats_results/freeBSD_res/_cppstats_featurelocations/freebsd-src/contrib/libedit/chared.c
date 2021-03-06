

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)chared.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: chared.c,v 1.59 2019/07/23 10:18:52 christos Exp $");
#endif
#endif




#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "el.h"
#include "common.h"
#include "fcns.h"


#define EL_LEAVE 2




libedit_private void
cv_undo(EditLine *el)
{
c_undo_t *vu = &el->el_chared.c_undo;
c_redo_t *r = &el->el_chared.c_redo;
size_t size;


size = (size_t)(el->el_line.lastchar - el->el_line.buffer);
vu->len = (ssize_t)size;
vu->cursor = (int)(el->el_line.cursor - el->el_line.buffer);
(void)memcpy(vu->buf, el->el_line.buffer, size * sizeof(*vu->buf));


r->count = el->el_state.doingarg ? el->el_state.argument : 0;
r->action = el->el_chared.c_vcmd.action;
r->pos = r->buf;
r->cmd = el->el_state.thiscmd;
r->ch = el->el_state.thisch;
}




libedit_private void
cv_yank(EditLine *el, const wchar_t *ptr, int size)
{
c_kill_t *k = &el->el_chared.c_kill;

(void)memcpy(k->buf, ptr, (size_t)size * sizeof(*k->buf));
k->last = k->buf + size;
}





libedit_private void
c_insert(EditLine *el, int num)
{
wchar_t *cp;

if (el->el_line.lastchar + num >= el->el_line.limit) {
if (!ch_enlargebufs(el, (size_t)num))
return;
}

if (el->el_line.cursor < el->el_line.lastchar) {

for (cp = el->el_line.lastchar; cp >= el->el_line.cursor; cp--)
cp[num] = *cp;
}
el->el_line.lastchar += num;
}





libedit_private void
c_delafter(EditLine *el, int num)
{

if (el->el_line.cursor + num > el->el_line.lastchar)
num = (int)(el->el_line.lastchar - el->el_line.cursor);

if (el->el_map.current != el->el_map.emacs) {
cv_undo(el);
cv_yank(el, el->el_line.cursor, num);
}

if (num > 0) {
wchar_t *cp;

for (cp = el->el_line.cursor; cp <= el->el_line.lastchar; cp++)
*cp = cp[num];

el->el_line.lastchar -= num;
}
}





libedit_private void
c_delafter1(EditLine *el)
{
wchar_t *cp;

for (cp = el->el_line.cursor; cp <= el->el_line.lastchar; cp++)
*cp = cp[1];

el->el_line.lastchar--;
}





libedit_private void
c_delbefore(EditLine *el, int num)
{

if (el->el_line.cursor - num < el->el_line.buffer)
num = (int)(el->el_line.cursor - el->el_line.buffer);

if (el->el_map.current != el->el_map.emacs) {
cv_undo(el);
cv_yank(el, el->el_line.cursor - num, num);
}

if (num > 0) {
wchar_t *cp;

for (cp = el->el_line.cursor - num;
&cp[num] <= el->el_line.lastchar;
cp++)
*cp = cp[num];

el->el_line.lastchar -= num;
}
}





libedit_private void
c_delbefore1(EditLine *el)
{
wchar_t *cp;

for (cp = el->el_line.cursor - 1; cp <= el->el_line.lastchar; cp++)
*cp = cp[1];

el->el_line.lastchar--;
}





libedit_private int
ce__isword(wint_t p)
{
return iswalnum(p) || wcschr(L"*?_-.[]~=", p) != NULL;
}





libedit_private int
cv__isword(wint_t p)
{
if (iswalnum(p) || p == L'_')
return 1;
if (iswgraph(p))
return 2;
return 0;
}





libedit_private int
cv__isWord(wint_t p)
{
return !iswspace(p);
}





libedit_private wchar_t *
c__prev_word(wchar_t *p, wchar_t *low, int n, int (*wtest)(wint_t))
{
p--;

while (n--) {
while ((p >= low) && !(*wtest)(*p))
p--;
while ((p >= low) && (*wtest)(*p))
p--;
}


p++;
if (p < low)
p = low;

return p;
}





libedit_private wchar_t *
c__next_word(wchar_t *p, wchar_t *high, int n, int (*wtest)(wint_t))
{
while (n--) {
while ((p < high) && !(*wtest)(*p))
p++;
while ((p < high) && (*wtest)(*p))
p++;
}
if (p > high)
p = high;

return p;
}




libedit_private wchar_t *
cv_next_word(EditLine *el, wchar_t *p, wchar_t *high, int n,
int (*wtest)(wint_t))
{
int test;

while (n--) {
test = (*wtest)(*p);
while ((p < high) && (*wtest)(*p) == test)
p++;




if (n || el->el_chared.c_vcmd.action != (DELETE|INSERT))
while ((p < high) && iswspace(*p))
p++;
}


if (p > high)
return high;
else
return p;
}





libedit_private wchar_t *
cv_prev_word(wchar_t *p, wchar_t *low, int n, int (*wtest)(wint_t))
{
int test;

p--;
while (n--) {
while ((p > low) && iswspace(*p))
p--;
test = (*wtest)(*p);
while ((p >= low) && (*wtest)(*p) == test)
p--;
}
p++;


if (p < low)
return low;
else
return p;
}





libedit_private void
cv_delfini(EditLine *el)
{
int size;
int action = el->el_chared.c_vcmd.action;

if (action & INSERT)
el->el_map.current = el->el_map.key;

if (el->el_chared.c_vcmd.pos == 0)

return;

size = (int)(el->el_line.cursor - el->el_chared.c_vcmd.pos);
if (size == 0)
size = 1;
el->el_line.cursor = el->el_chared.c_vcmd.pos;
if (action & YANK) {
if (size > 0)
cv_yank(el, el->el_line.cursor, size);
else
cv_yank(el, el->el_line.cursor + size, -size);
} else {
if (size > 0) {
c_delafter(el, size);
re_refresh_cursor(el);
} else {
c_delbefore(el, -size);
el->el_line.cursor += size;
}
}
el->el_chared.c_vcmd.action = NOP;
}





libedit_private wchar_t *
cv__endword(wchar_t *p, wchar_t *high, int n, int (*wtest)(wint_t))
{
int test;

p++;

while (n--) {
while ((p < high) && iswspace(*p))
p++;

test = (*wtest)(*p);
while ((p < high) && (*wtest)(*p) == test)
p++;
}
p--;
return p;
}




libedit_private int
ch_init(EditLine *el)
{
el->el_line.buffer = el_calloc(EL_BUFSIZ,
sizeof(*el->el_line.buffer));
if (el->el_line.buffer == NULL)
return -1;

el->el_line.cursor = el->el_line.buffer;
el->el_line.lastchar = el->el_line.buffer;
el->el_line.limit = &el->el_line.buffer[EL_BUFSIZ - EL_LEAVE];

el->el_chared.c_undo.buf = el_calloc(EL_BUFSIZ,
sizeof(*el->el_chared.c_undo.buf));
if (el->el_chared.c_undo.buf == NULL)
return -1;
el->el_chared.c_undo.len = -1;
el->el_chared.c_undo.cursor = 0;
el->el_chared.c_redo.buf = el_calloc(EL_BUFSIZ,
sizeof(*el->el_chared.c_redo.buf));
if (el->el_chared.c_redo.buf == NULL)
return -1;
el->el_chared.c_redo.pos = el->el_chared.c_redo.buf;
el->el_chared.c_redo.lim = el->el_chared.c_redo.buf + EL_BUFSIZ;
el->el_chared.c_redo.cmd = ED_UNASSIGNED;

el->el_chared.c_vcmd.action = NOP;
el->el_chared.c_vcmd.pos = el->el_line.buffer;

el->el_chared.c_kill.buf = el_calloc(EL_BUFSIZ,
sizeof(*el->el_chared.c_kill.buf));
if (el->el_chared.c_kill.buf == NULL)
return -1;
el->el_chared.c_kill.mark = el->el_line.buffer;
el->el_chared.c_kill.last = el->el_chared.c_kill.buf;
el->el_chared.c_resizefun = NULL;
el->el_chared.c_resizearg = NULL;
el->el_chared.c_aliasfun = NULL;
el->el_chared.c_aliasarg = NULL;

el->el_map.current = el->el_map.key;

el->el_state.inputmode = MODE_INSERT;
el->el_state.doingarg = 0;
el->el_state.metanext = 0;
el->el_state.argument = 1;
el->el_state.lastcmd = ED_UNASSIGNED;

return 0;
}




libedit_private void
ch_reset(EditLine *el)
{
el->el_line.cursor = el->el_line.buffer;
el->el_line.lastchar = el->el_line.buffer;

el->el_chared.c_undo.len = -1;
el->el_chared.c_undo.cursor = 0;

el->el_chared.c_vcmd.action = NOP;
el->el_chared.c_vcmd.pos = el->el_line.buffer;

el->el_chared.c_kill.mark = el->el_line.buffer;

el->el_map.current = el->el_map.key;

el->el_state.inputmode = MODE_INSERT;
el->el_state.doingarg = 0;
el->el_state.metanext = 0;
el->el_state.argument = 1;
el->el_state.lastcmd = ED_UNASSIGNED;

el->el_history.eventno = 0;
}





libedit_private int
ch_enlargebufs(EditLine *el, size_t addlen)
{
size_t sz, newsz;
wchar_t *newbuffer, *oldbuf, *oldkbuf;

sz = (size_t)(el->el_line.limit - el->el_line.buffer + EL_LEAVE);
newsz = sz * 2;




if (addlen > sz) {
while(newsz - sz < addlen)
newsz *= 2;
}




newbuffer = el_realloc(el->el_line.buffer, newsz * sizeof(*newbuffer));
if (!newbuffer)
return 0;


(void) memset(&newbuffer[sz], 0, (newsz - sz) * sizeof(*newbuffer));

oldbuf = el->el_line.buffer;

el->el_line.buffer = newbuffer;
el->el_line.cursor = newbuffer + (el->el_line.cursor - oldbuf);
el->el_line.lastchar = newbuffer + (el->el_line.lastchar - oldbuf);

el->el_line.limit = &newbuffer[sz - EL_LEAVE];




newbuffer = el_realloc(el->el_chared.c_kill.buf, newsz *
sizeof(*newbuffer));
if (!newbuffer)
return 0;


(void) memset(&newbuffer[sz], 0, (newsz - sz) * sizeof(*newbuffer));

oldkbuf = el->el_chared.c_kill.buf;

el->el_chared.c_kill.buf = newbuffer;
el->el_chared.c_kill.last = newbuffer +
(el->el_chared.c_kill.last - oldkbuf);
el->el_chared.c_kill.mark = el->el_line.buffer +
(el->el_chared.c_kill.mark - oldbuf);




newbuffer = el_realloc(el->el_chared.c_undo.buf,
newsz * sizeof(*newbuffer));
if (!newbuffer)
return 0;


(void) memset(&newbuffer[sz], 0, (newsz - sz) * sizeof(*newbuffer));
el->el_chared.c_undo.buf = newbuffer;

newbuffer = el_realloc(el->el_chared.c_redo.buf,
newsz * sizeof(*newbuffer));
if (!newbuffer)
return 0;
el->el_chared.c_redo.pos = newbuffer +
(el->el_chared.c_redo.pos - el->el_chared.c_redo.buf);
el->el_chared.c_redo.lim = newbuffer +
(el->el_chared.c_redo.lim - el->el_chared.c_redo.buf);
el->el_chared.c_redo.buf = newbuffer;

if (!hist_enlargebuf(el, sz, newsz))
return 0;


el->el_line.limit = &el->el_line.buffer[newsz - EL_LEAVE];
if (el->el_chared.c_resizefun)
(*el->el_chared.c_resizefun)(el, el->el_chared.c_resizearg);
return 1;
}




libedit_private void
ch_end(EditLine *el)
{
el_free(el->el_line.buffer);
el->el_line.buffer = NULL;
el->el_line.limit = NULL;
el_free(el->el_chared.c_undo.buf);
el->el_chared.c_undo.buf = NULL;
el_free(el->el_chared.c_redo.buf);
el->el_chared.c_redo.buf = NULL;
el->el_chared.c_redo.pos = NULL;
el->el_chared.c_redo.lim = NULL;
el->el_chared.c_redo.cmd = ED_UNASSIGNED;
el_free(el->el_chared.c_kill.buf);
el->el_chared.c_kill.buf = NULL;
ch_reset(el);
}





int
el_winsertstr(EditLine *el, const wchar_t *s)
{
size_t len;

if (s == NULL || (len = wcslen(s)) == 0)
return -1;
if (el->el_line.lastchar + len >= el->el_line.limit) {
if (!ch_enlargebufs(el, len))
return -1;
}

c_insert(el, (int)len);
while (*s)
*el->el_line.cursor++ = *s++;
return 0;
}





void
el_deletestr(EditLine *el, int n)
{
if (n <= 0)
return;

if (el->el_line.cursor < &el->el_line.buffer[n])
return;

c_delbefore(el, n);
el->el_line.cursor -= n;
if (el->el_line.cursor < el->el_line.buffer)
el->el_line.cursor = el->el_line.buffer;
}




int
el_cursor(EditLine *el, int n)
{
if (n == 0)
goto out;

el->el_line.cursor += n;

if (el->el_line.cursor < el->el_line.buffer)
el->el_line.cursor = el->el_line.buffer;
if (el->el_line.cursor > el->el_line.lastchar)
el->el_line.cursor = el->el_line.lastchar;
out:
return (int)(el->el_line.cursor - el->el_line.buffer);
}




libedit_private int
c_gets(EditLine *el, wchar_t *buf, const wchar_t *prompt)
{
ssize_t len;
wchar_t *cp = el->el_line.buffer, ch;

if (prompt) {
len = (ssize_t)wcslen(prompt);
(void)memcpy(cp, prompt, (size_t)len * sizeof(*cp));
cp += len;
}
len = 0;

for (;;) {
el->el_line.cursor = cp;
*cp = ' ';
el->el_line.lastchar = cp + 1;
re_refresh(el);

if (el_wgetc(el, &ch) != 1) {
ed_end_of_file(el, 0);
len = -1;
break;
}

switch (ch) {

case L'\b':
case 0177:
if (len == 0) {
len = -1;
break;
}
len--;
cp--;
continue;

case 0033:
case L'\r':
case L'\n':
buf[len] = ch;
break;

default:
if (len >= (ssize_t)(EL_BUFSIZ - 16))
terminal_beep(el);
else {
buf[len++] = ch;
*cp++ = ch;
}
continue;
}
break;
}

el->el_line.buffer[0] = '\0';
el->el_line.lastchar = el->el_line.buffer;
el->el_line.cursor = el->el_line.buffer;
return (int)len;
}





libedit_private int
c_hpos(EditLine *el)
{
wchar_t *ptr;




if (el->el_line.cursor == el->el_line.buffer)
return 0;
else {
for (ptr = el->el_line.cursor - 1;
ptr >= el->el_line.buffer && *ptr != '\n';
ptr--)
continue;
return (int)(el->el_line.cursor - ptr - 1);
}
}

libedit_private int
ch_resizefun(EditLine *el, el_zfunc_t f, void *a)
{
el->el_chared.c_resizefun = f;
el->el_chared.c_resizearg = a;
return 0;
}

libedit_private int
ch_aliasfun(EditLine *el, el_afunc_t f, void *a)
{
el->el_chared.c_aliasfun = f;
el->el_chared.c_aliasarg = a;
return 0;
}
