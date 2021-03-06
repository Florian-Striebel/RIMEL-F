

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)refresh.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: refresh.c,v 1.58 2021/09/09 20:24:07 christos Exp $");
#endif
#endif




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "el.h"

static void re_nextline(EditLine *);
static void re_addc(EditLine *, wint_t);
static void re_update_line(EditLine *, wchar_t *, wchar_t *, int);
static void re_insert (EditLine *, wchar_t *, int, int, wchar_t *, int);
static void re_delete(EditLine *, wchar_t *, int, int, int);
static void re_fastputc(EditLine *, wint_t);
static void re_clear_eol(EditLine *, int, int, int);
static void re__strncopy(wchar_t *, wchar_t *, size_t);
static void re__copy_and_pad(wchar_t *, const wchar_t *, size_t);

#if defined(DEBUG_REFRESH)
static void re_printstr(EditLine *, const char *, wchar_t *, wchar_t *);
#define __F el->el_errfile
#define ELRE_ASSERT(a, b, c) do if ( a) { (void) fprintf b; c; } while (0)





#define ELRE_DEBUG(a, b) ELRE_ASSERT(a,b,;)




static void
re_printstr(EditLine *el, const char *str, wchar_t *f, wchar_t *t)
{

ELRE_DEBUG(1, (__F, "%s:\"", str));
while (f < t)
ELRE_DEBUG(1, (__F, "%c", *f++ & 0177));
ELRE_DEBUG(1, (__F, "\"\r\n"));
}
#else
#define ELRE_ASSERT(a, b, c)
#define ELRE_DEBUG(a, b)
#endif




static void
re_nextline(EditLine *el)
{
el->el_refresh.r_cursor.h = 0;







if (el->el_refresh.r_cursor.v + 1 >= el->el_terminal.t_size.v) {
int i, lins = el->el_terminal.t_size.v;
wint_t *firstline = el->el_vdisplay[0];

for(i = 1; i < lins; i++)
el->el_vdisplay[i - 1] = el->el_vdisplay[i];

firstline[0] = '\0';
el->el_vdisplay[i - 1] = firstline;
} else
el->el_refresh.r_cursor.v++;

ELRE_ASSERT(el->el_refresh.r_cursor.v >= el->el_terminal.t_size.v,
(__F, "\r\nre_putc: overflow! r_cursor.v == %d > %d\r\n",
el->el_refresh.r_cursor.v, el->el_terminal.t_size.v),
abort());
}




static void
re_addc(EditLine *el, wint_t c)
{
switch (ct_chr_class(c)) {
case CHTYPE_TAB:
for (;;) {
re_putc(el, ' ', 1);
if ((el->el_refresh.r_cursor.h & 07) == 0)
break;
}
break;
case CHTYPE_NL: {
int oldv = el->el_refresh.r_cursor.v;
re_putc(el, '\0', 0);
if (oldv == el->el_refresh.r_cursor.v)
re_nextline(el);
break;
}
case CHTYPE_PRINT:
re_putc(el, c, 1);
break;
default: {
wchar_t visbuf[VISUAL_WIDTH_MAX];
ssize_t i, n =
ct_visual_char(visbuf, VISUAL_WIDTH_MAX, c);
for (i = 0; n-- > 0; ++i)
re_putc(el, visbuf[i], 1);
break;
}
}
}




libedit_private void
re_putliteral(EditLine *el, const wchar_t *begin, const wchar_t *end)
{
coord_t *cur = &el->el_refresh.r_cursor;
wint_t c;
int sizeh = el->el_terminal.t_size.h;
int i, w;

c = literal_add(el, begin, end, &w);
if (c == 0 || w <= 0)
return;
el->el_vdisplay[cur->v][cur->h] = c;

i = w;
if (i > sizeh - cur->h)
i = sizeh - cur->h;
while (--i > 0)
el->el_vdisplay[cur->v][cur->h + i] = MB_FILL_CHAR;

cur->h += w;
if (cur->h >= sizeh) {

el->el_vdisplay[cur->v][sizeh] = '\0';
re_nextline(el);
}
}




libedit_private void
re_putc(EditLine *el, wint_t c, int shift)
{
coord_t *cur = &el->el_refresh.r_cursor;
int i, w = wcwidth(c);
int sizeh = el->el_terminal.t_size.h;

ELRE_DEBUG(1, (__F, "printing %5x '%lc'\r\n", c, c));
if (w == -1)
w = 0;

while (shift && (cur->h + w > sizeh))
re_putc(el, ' ', 1);

el->el_vdisplay[cur->v][cur->h] = c;

i = w;
while (--i > 0)
el->el_vdisplay[cur->v][cur->h + i] = MB_FILL_CHAR;

if (!shift)
return;

cur->h += w;
if (cur->h >= sizeh) {

el->el_vdisplay[cur->v][sizeh] = '\0';
re_nextline(el);
}
}








libedit_private void
re_refresh(EditLine *el)
{
int i, rhdiff;
wchar_t *cp, *st;
coord_t cur;
#if defined(notyet)
size_t termsz;
#endif

ELRE_DEBUG(1, (__F, "el->el_line.buffer = :%ls:\r\n",
el->el_line.buffer));

literal_clear(el);

el->el_refresh.r_cursor.h = 0;
el->el_refresh.r_cursor.v = 0;

terminal_move_to_char(el, 0);


prompt_print(el, EL_RPROMPT);


el->el_refresh.r_cursor.h = 0;
el->el_refresh.r_cursor.v = 0;

if (el->el_line.cursor >= el->el_line.lastchar) {
if (el->el_map.current == el->el_map.alt
&& el->el_line.lastchar != el->el_line.buffer)
el->el_line.cursor = el->el_line.lastchar - 1;
else
el->el_line.cursor = el->el_line.lastchar;
}

cur.h = -1;
cur.v = 0;

prompt_print(el, EL_PROMPT);


#if notyet
termsz = el->el_terminal.t_size.h * el->el_terminal.t_size.v;
if (el->el_line.lastchar - el->el_line.buffer > termsz) {




size_t rem = (el->el_line.lastchar-el->el_line.buffer)%termsz;

st = el->el_line.lastchar - rem
- (termsz - (((rem / el->el_terminal.t_size.v) - 1)
* el->el_terminal.t_size.v));
} else
#endif
st = el->el_line.buffer;

for (cp = st; cp < el->el_line.lastchar; cp++) {
if (cp == el->el_line.cursor) {
int w = wcwidth(*cp);

cur.h = el->el_refresh.r_cursor.h;
cur.v = el->el_refresh.r_cursor.v;

if (w > 1 && el->el_refresh.r_cursor.h + w >
el->el_terminal.t_size.h) {
cur.h = 0;
cur.v++;
}
}
re_addc(el, *cp);
}

if (cur.h == -1) {
cur.h = el->el_refresh.r_cursor.h;
cur.v = el->el_refresh.r_cursor.v;
}
rhdiff = el->el_terminal.t_size.h - el->el_refresh.r_cursor.h -
el->el_rprompt.p_pos.h;
if (el->el_rprompt.p_pos.h && !el->el_rprompt.p_pos.v &&
!el->el_refresh.r_cursor.v && rhdiff > 1) {





while (--rhdiff > 0)
re_putc(el, ' ', 1);
prompt_print(el, EL_RPROMPT);
} else {
el->el_rprompt.p_pos.h = 0;
el->el_rprompt.p_pos.v = 0;
}

re_putc(el, '\0', 0);

el->el_refresh.r_newcv = el->el_refresh.r_cursor.v;

ELRE_DEBUG(1, (__F,
"term.h=%d vcur.h=%d vcur.v=%d vdisplay[0]=\r\n:%80.80s:\r\n",
el->el_terminal.t_size.h, el->el_refresh.r_cursor.h,
el->el_refresh.r_cursor.v, ct_encode_string(el->el_vdisplay[0],
&el->el_scratch)));

ELRE_DEBUG(1, (__F, "updating %d lines.\r\n", el->el_refresh.r_newcv));
for (i = 0; i <= el->el_refresh.r_newcv; i++) {

re_update_line(el, (wchar_t *)el->el_display[i],
(wchar_t *)el->el_vdisplay[i], i);








re__copy_and_pad((wchar_t *)el->el_display[i],
(wchar_t *)el->el_vdisplay[i],
(size_t) el->el_terminal.t_size.h);
}
ELRE_DEBUG(1, (__F,
"\r\nel->el_refresh.r_cursor.v=%d,el->el_refresh.r_oldcv=%d i=%d\r\n",
el->el_refresh.r_cursor.v, el->el_refresh.r_oldcv, i));

if (el->el_refresh.r_oldcv > el->el_refresh.r_newcv)
for (; i <= el->el_refresh.r_oldcv; i++) {
terminal_move_to_line(el, i);
terminal_move_to_char(el, 0);

terminal_clear_EOL(el,
(int) wcslen((const wchar_t *)el->el_display[i]));
#if defined(DEBUG_REFRESH)
terminal_overwrite(el, L"C\b", 2);
#endif
el->el_display[i][0] = '\0';
}

el->el_refresh.r_oldcv = el->el_refresh.r_newcv;
ELRE_DEBUG(1, (__F,
"\r\ncursor.h = %d, cursor.v = %d, cur.h = %d, cur.v = %d\r\n",
el->el_refresh.r_cursor.h, el->el_refresh.r_cursor.v,
cur.h, cur.v));
terminal_move_to_line(el, cur.v);
terminal_move_to_char(el, cur.h);
}





libedit_private void
re_goto_bottom(EditLine *el)
{

terminal_move_to_line(el, el->el_refresh.r_oldcv);
terminal__putc(el, '\n');
re_clear_display(el);
terminal__flush(el);
}






static void

re_insert(EditLine *el __attribute__((__unused__)),
wchar_t *d, int dat, int dlen, wchar_t *s, int num)
{
wchar_t *a, *b;

if (num <= 0)
return;
if (num > dlen - dat)
num = dlen - dat;

ELRE_DEBUG(1,
(__F, "re_insert() starting: %d at %d max %d, d == \"%s\"\n",
num, dat, dlen, ct_encode_string(d, &el->el_scratch)));
ELRE_DEBUG(1, (__F, "s == \"%s\"\n", ct_encode_string(s,
&el->el_scratch)));


if (num > 0) {
b = d + dlen - 1;
a = b - num;
while (a >= &d[dat])
*b-- = *a--;
d[dlen] = '\0';
}

ELRE_DEBUG(1, (__F,
"re_insert() after insert: %d at %d max %d, d == \"%s\"\n",
num, dat, dlen, ct_encode_string(d, &el->el_scratch)));
ELRE_DEBUG(1, (__F, "s == \"%s\"\n", ct_encode_string(s,
&el->el_scratch)));


for (a = d + dat; (a < d + dlen) && (num > 0); num--)
*a++ = *s++;

#if defined(notyet)


ELRE_DEBUG(1,
(__F, "re_insert() after copy: %d at %d max %d, %s == \"%s\"\n",
num, dat, dlen, d, s));
ELRE_DEBUG(1, (__F, "s == \"%s\"\n", s));
#endif
}





static void

re_delete(EditLine *el __attribute__((__unused__)),
wchar_t *d, int dat, int dlen, int num)
{
wchar_t *a, *b;

if (num <= 0)
return;
if (dat + num >= dlen) {
d[dat] = '\0';
return;
}
ELRE_DEBUG(1,
(__F, "re_delete() starting: %d at %d max %d, d == \"%s\"\n",
num, dat, dlen, ct_encode_string(d, &el->el_scratch)));


if (num > 0) {
b = d + dat;
a = b + num;
while (a < &d[dlen])
*b++ = *a++;
d[dlen] = '\0';
}
ELRE_DEBUG(1,
(__F, "re_delete() after delete: %d at %d max %d, d == \"%s\"\n",
num, dat, dlen, ct_encode_string(d, &el->el_scratch)));
}





static void
re__strncopy(wchar_t *a, wchar_t *b, size_t n)
{

while (n-- && *b)
*a++ = *b++;
}








static void
re_clear_eol(EditLine *el, int fx, int sx, int diff)
{

ELRE_DEBUG(1, (__F, "re_clear_eol sx %d, fx %d, diff %d\n",
sx, fx, diff));

if (fx < 0)
fx = -fx;
if (sx < 0)
sx = -sx;
if (fx > diff)
diff = fx;
if (sx > diff)
diff = sx;

ELRE_DEBUG(1, (__F, "re_clear_eol %d\n", diff));
terminal_clear_EOL(el, diff);
}























#define MIN_END_KEEP 4

static void
re_update_line(EditLine *el, wchar_t *old, wchar_t *new, int i)
{
wchar_t *o, *n, *p, c;
wchar_t *ofd, *ols, *oe, *nfd, *nls, *ne;
wchar_t *osb, *ose, *nsb, *nse;
int fx, sx;
size_t len;




for (o = old, n = new; *o && (*o == *n); o++, n++)
continue;
ofd = o;
nfd = n;




while (*o)
o++;




while (ofd < o) {
if (o[-1] != ' ')
break;
o--;
}
oe = o;
*oe = '\0';

while (*n)
n++;


while (nfd < n) {
if (n[-1] != ' ')
break;
n--;
}
ne = n;
*ne = '\0';




if (*ofd == '\0' && *nfd == '\0') {
ELRE_DEBUG(1, (__F, "no difference.\r\n"));
return;
}



while ((o > ofd) && (n > nfd) && (*--o == *--n))
continue;
ols = ++o;
nls = ++n;




osb = ols;
nsb = nls;
ose = ols;
nse = nls;




if (*ofd) {
for (c = *ofd, n = nfd; n < nls; n++) {
if (c == *n) {
for (o = ofd, p = n;
p < nls && o < ols && *o == *p;
o++, p++)
continue;




if (((nse - nsb) < (p - n)) &&
(2 * (p - n) > n - nfd)) {
nsb = n;
nse = p;
osb = ofd;
ose = o;
}
}
}
}



if (*nfd) {
for (c = *nfd, o = ofd; o < ols; o++) {
if (c == *o) {
for (n = nfd, p = o;
p < ols && n < nls && *p == *n;
p++, n++)
continue;




if (((ose - osb) < (p - o)) &&
(2 * (p - o) > o - ofd)) {
nsb = nfd;
nse = n;
osb = o;
ose = p;
}
}
}
}




if ((oe - ols) < MIN_END_KEEP) {
ols = oe;
nls = ne;
}









fx = (int)((nsb - nfd) - (osb - ofd));




sx = (int)((nls - nse) - (ols - ose));

if (!EL_CAN_INSERT) {
if (fx > 0) {
osb = ols;
ose = ols;
nsb = nls;
nse = nls;
}
if (sx > 0) {
ols = oe;
nls = ne;
}
if ((ols - ofd) < (nls - nfd)) {
ols = oe;
nls = ne;
}
}
if (!EL_CAN_DELETE) {
if (fx < 0) {
osb = ols;
ose = ols;
nsb = nls;
nse = nls;
}
if (sx < 0) {
ols = oe;
nls = ne;
}
if ((ols - ofd) > (nls - nfd)) {
ols = oe;
nls = ne;
}
}






if ((ose - osb) < MIN_END_KEEP) {
osb = ols;
ose = ols;
nsb = nls;
nse = nls;
}



fx = (int)((nsb - nfd) - (osb - ofd));
sx = (int)((nls - nse) - (ols - ose));

ELRE_DEBUG(1, (__F, "fx %d, sx %d\n", fx, sx));
ELRE_DEBUG(1, (__F, "ofd %td, osb %td, ose %td, ols %td, oe %td\n",
ofd - old, osb - old, ose - old, ols - old, oe - old));
ELRE_DEBUG(1, (__F, "nfd %td, nsb %td, nse %td, nls %td, ne %td\n",
nfd - new, nsb - new, nse - new, nls - new, ne - new));
ELRE_DEBUG(1, (__F,
"xxx-xxx:\"00000000001111111111222222222233333333334\"\r\n"));
ELRE_DEBUG(1, (__F,
"xxx-xxx:\"01234567890123456789012345678901234567890\"\r\n"));
#if defined(DEBUG_REFRESH)
re_printstr(el, "old- oe", old, oe);
re_printstr(el, "new- ne", new, ne);
re_printstr(el, "old-ofd", old, ofd);
re_printstr(el, "new-nfd", new, nfd);
re_printstr(el, "ofd-osb", ofd, osb);
re_printstr(el, "nfd-nsb", nfd, nsb);
re_printstr(el, "osb-ose", osb, ose);
re_printstr(el, "nsb-nse", nsb, nse);
re_printstr(el, "ose-ols", ose, ols);
re_printstr(el, "nse-nls", nse, nls);
re_printstr(el, "ols- oe", ols, oe);
re_printstr(el, "nls- ne", nls, ne);
#endif






terminal_move_to_line(el, i);
































p = (ols != oe) ? oe : ose;











if ((nsb != nfd) && fx > 0 &&
((p - old) + fx <= el->el_terminal.t_size.h)) {
ELRE_DEBUG(1,
(__F, "first diff insert at %td...\r\n", nfd - new));



terminal_move_to_char(el, (int)(nfd - new));



if (nsb != ne) {
ELRE_DEBUG(1, (__F, "with stuff to keep at end\r\n"));



if (fx > 0) {
ELRE_DEBUG(!EL_CAN_INSERT, (__F,
"ERROR: cannot insert in early first diff\n"));
terminal_insertwrite(el, nfd, fx);
re_insert(el, old, (int)(ofd - old),
el->el_terminal.t_size.h, nfd, fx);
}




len = (size_t) ((nsb - nfd) - fx);
terminal_overwrite(el, (nfd + fx), len);
re__strncopy(ofd + fx, nfd + fx, len);
} else {
ELRE_DEBUG(1, (__F, "without anything to save\r\n"));
len = (size_t)(nsb - nfd);
terminal_overwrite(el, nfd, len);
re__strncopy(ofd, nfd, len);



return;
}
} else if (fx < 0) {
ELRE_DEBUG(1,
(__F, "first diff delete at %td...\r\n", ofd - old));



terminal_move_to_char(el, (int)(ofd - old));



if (osb != oe) {
ELRE_DEBUG(1, (__F, "with stuff to save at end\r\n"));




if (fx < 0) {
ELRE_DEBUG(!EL_CAN_DELETE, (__F,
"ERROR: cannot delete in first diff\n"));
terminal_deletechars(el, -fx);
re_delete(el, old, (int)(ofd - old),
el->el_terminal.t_size.h, -fx);
}



len = (size_t) (nsb - nfd);
terminal_overwrite(el, nfd, len);
re__strncopy(ofd, nfd, len);

} else {
ELRE_DEBUG(1, (__F,
"but with nothing left to save\r\n"));



terminal_overwrite(el, nfd, (size_t)(nsb - nfd));
re_clear_eol(el, fx, sx,
(int)((oe - old) - (ne - new)));



return;
}
} else
fx = 0;

if (sx < 0 && (ose - old) + fx < el->el_terminal.t_size.h) {
ELRE_DEBUG(1, (__F,
"second diff delete at %td...\r\n", (ose - old) + fx));







terminal_move_to_char(el, (int)((ose - old) + fx));



if (ols != oe) {
ELRE_DEBUG(1, (__F, "with stuff to save at end\r\n"));



if (sx < 0) {
ELRE_DEBUG(!EL_CAN_DELETE, (__F,
"ERROR: cannot delete in second diff\n"));
terminal_deletechars(el, -sx);
}



terminal_overwrite(el, nse, (size_t)(nls - nse));
} else {
ELRE_DEBUG(1, (__F,
"but with nothing left to save\r\n"));
terminal_overwrite(el, nse, (size_t)(nls - nse));
re_clear_eol(el, fx, sx,
(int)((oe - old) - (ne - new)));
}
}



if ((nsb != nfd) && (osb - ofd) <= (nsb - nfd) && (fx == 0)) {
ELRE_DEBUG(1, (__F, "late first diff insert at %td...\r\n",
nfd - new));

terminal_move_to_char(el, (int)(nfd - new));



if (nsb != ne) {
ELRE_DEBUG(1, (__F, "with stuff to keep at end\r\n"));





fx = (int)((nsb - nfd) - (osb - ofd));
if (fx > 0) {



ELRE_DEBUG(!EL_CAN_INSERT, (__F,
"ERROR: cannot insert in late first diff\n"));
terminal_insertwrite(el, nfd, fx);
re_insert(el, old, (int)(ofd - old),
el->el_terminal.t_size.h, nfd, fx);
}




len = (size_t) ((nsb - nfd) - fx);
terminal_overwrite(el, (nfd + fx), len);
re__strncopy(ofd + fx, nfd + fx, len);
} else {
ELRE_DEBUG(1, (__F, "without anything to save\r\n"));
len = (size_t) (nsb - nfd);
terminal_overwrite(el, nfd, len);
re__strncopy(ofd, nfd, len);
}
}



if (sx >= 0) {
ELRE_DEBUG(1, (__F,
"second diff insert at %d...\r\n", (int)(nse - new)));
terminal_move_to_char(el, (int)(nse - new));
if (ols != oe) {
ELRE_DEBUG(1, (__F, "with stuff to keep at end\r\n"));
if (sx > 0) {

ELRE_DEBUG(!EL_CAN_INSERT, (__F,
"ERROR: cannot insert in second diff\n"));
terminal_insertwrite(el, nse, sx);
}




terminal_overwrite(el, (nse + sx),
(size_t)((nls - nse) - sx));
} else {
ELRE_DEBUG(1, (__F, "without anything to save\r\n"));
terminal_overwrite(el, nse, (size_t)(nls - nse));






}
}
ELRE_DEBUG(1, (__F, "done.\r\n"));
}





static void
re__copy_and_pad(wchar_t *dst, const wchar_t *src, size_t width)
{
size_t i;

for (i = 0; i < width; i++) {
if (*src == '\0')
break;
*dst++ = *src++;
}

for (; i < width; i++)
*dst++ = ' ';

*dst = '\0';
}





libedit_private void
re_refresh_cursor(EditLine *el)
{
wchar_t *cp;
int h, v, th, w;

if (el->el_line.cursor >= el->el_line.lastchar) {
if (el->el_map.current == el->el_map.alt
&& el->el_line.lastchar != el->el_line.buffer)
el->el_line.cursor = el->el_line.lastchar - 1;
else
el->el_line.cursor = el->el_line.lastchar;
}


h = el->el_prompt.p_pos.h;
v = el->el_prompt.p_pos.v;
th = el->el_terminal.t_size.h;


for (cp = el->el_line.buffer; cp < el->el_line.cursor; cp++) {
switch (ct_chr_class(*cp)) {
case CHTYPE_NL:
h = 0;
v++;
break;
case CHTYPE_TAB:
while (++h & 07)
continue;
break;
default:
w = wcwidth(*cp);
if (w > 1 && h + w > th) {
h = 0;
v++;
}
h += ct_visual_width(*cp);
break;
}

if (h >= th) {
h -= th;
v++;
}
}


if (cp < el->el_line.lastchar && (w = wcwidth(*cp)) > 1)
if (h + w > th) {
h = 0;
v++;
}


terminal_move_to_line(el, v);
terminal_move_to_char(el, h);
terminal__flush(el);
}





static void
re_fastputc(EditLine *el, wint_t c)
{
wint_t *lastline;
int w;

w = wcwidth(c);
while (w > 1 && el->el_cursor.h + w > el->el_terminal.t_size.h)
re_fastputc(el, ' ');

terminal__putc(el, c);
el->el_display[el->el_cursor.v][el->el_cursor.h++] = c;
while (--w > 0)
el->el_display[el->el_cursor.v][el->el_cursor.h++]
= MB_FILL_CHAR;

if (el->el_cursor.h >= el->el_terminal.t_size.h) {

el->el_cursor.h = 0;







if (el->el_cursor.v + 1 >= el->el_terminal.t_size.v) {
int i, lins = el->el_terminal.t_size.v;

lastline = el->el_display[0];
for(i = 1; i < lins; i++)
el->el_display[i - 1] = el->el_display[i];

el->el_display[i - 1] = lastline;
} else {
el->el_cursor.v++;
lastline = el->el_display[++el->el_refresh.r_oldcv];
}
re__copy_and_pad((wchar_t *)lastline, L"",
(size_t)el->el_terminal.t_size.h);

if (EL_HAS_AUTO_MARGINS) {
if (EL_HAS_MAGIC_MARGINS) {
terminal__putc(el, ' ');
terminal__putc(el, '\b');
}
} else {
terminal__putc(el, '\r');
terminal__putc(el, '\n');
}
}
}






libedit_private void
re_fastaddc(EditLine *el)
{
wchar_t c;
int rhdiff;

c = el->el_line.cursor[-1];

if (c == '\t' || el->el_line.cursor != el->el_line.lastchar) {
re_refresh(el);
return;
}
rhdiff = el->el_terminal.t_size.h - el->el_cursor.h -
el->el_rprompt.p_pos.h;
if (el->el_rprompt.p_pos.h && rhdiff < 3) {
re_refresh(el);
return;
}
switch (ct_chr_class(c)) {
case CHTYPE_TAB:
break;
case CHTYPE_NL:
case CHTYPE_PRINT:
re_fastputc(el, c);
break;
case CHTYPE_ASCIICTL:
case CHTYPE_NONPRINT: {
wchar_t visbuf[VISUAL_WIDTH_MAX];
ssize_t i, n =
ct_visual_char(visbuf, VISUAL_WIDTH_MAX, c);
for (i = 0; n-- > 0; ++i)
re_fastputc(el, visbuf[i]);
break;
}
}
terminal__flush(el);
}





libedit_private void
re_clear_display(EditLine *el)
{
int i;

el->el_cursor.v = 0;
el->el_cursor.h = 0;
for (i = 0; i < el->el_terminal.t_size.v; i++)
el->el_display[i][0] = '\0';
el->el_refresh.r_oldcv = 0;
}





libedit_private void
re_clear_lines(EditLine *el)
{

if (EL_CAN_CEOL) {
int i;
for (i = el->el_refresh.r_oldcv; i >= 0; i--) {

terminal_move_to_line(el, i);
terminal_move_to_char(el, 0);
terminal_clear_EOL(el, el->el_terminal.t_size.h);
}
} else {
terminal_move_to_line(el, el->el_refresh.r_oldcv);

terminal__putc(el, '\r');
terminal__putc(el, '\n');
}
}
