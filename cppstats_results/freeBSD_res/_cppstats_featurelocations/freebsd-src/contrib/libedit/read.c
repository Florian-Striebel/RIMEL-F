

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)read.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: read.c,v 1.107 2021/08/15 10:08:41 christos Exp $");
#endif
#endif




#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "el.h"
#include "fcns.h"
#include "read.h"

#define EL_MAXMACRO 10

struct macros {
wchar_t **macro;
int level;
int offset;
};

struct el_read_t {
struct macros macros;
el_rfunc_t read_char;
int read_errno;
};

static int read__fixio(int, int);
static int read_char(EditLine *, wchar_t *);
static int read_getcmd(EditLine *, el_action_t *, wchar_t *);
static void read_clearmacros(struct macros *);
static void read_pop(struct macros *);
static const wchar_t *noedit_wgets(EditLine *, int *);




libedit_private int
read_init(EditLine *el)
{
struct macros *ma;

if ((el->el_read = el_malloc(sizeof(*el->el_read))) == NULL)
return -1;

ma = &el->el_read->macros;
if ((ma->macro = el_calloc(EL_MAXMACRO, sizeof(*ma->macro))) == NULL) {
free(el->el_read);
return -1;
}
ma->level = -1;
ma->offset = 0;


el->el_read->read_char = read_char;
return 0;
}




libedit_private void
read_end(struct el_read_t *el_read)
{
read_clearmacros(&el_read->macros);
el_free(el_read->macros.macro);
el_read->macros.macro = NULL;
el_free(el_read);
}





libedit_private int
el_read_setfn(struct el_read_t *el_read, el_rfunc_t rc)
{
el_read->read_char = (rc == EL_BUILTIN_GETCFN) ? read_char : rc;
return 0;
}






libedit_private el_rfunc_t
el_read_getfn(struct el_read_t *el_read)
{
return el_read->read_char == read_char ?
EL_BUILTIN_GETCFN : el_read->read_char;
}






static int
read__fixio(int fd __attribute__((__unused__)), int e)
{

switch (e) {
case -1:

#if defined(EWOULDBLOCK)
case EWOULDBLOCK:
#if !defined(TRY_AGAIN)
#define TRY_AGAIN
#endif
#endif

#if defined(POSIX) && defined(EAGAIN)
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
case EAGAIN:
#if !defined(TRY_AGAIN)
#define TRY_AGAIN
#endif
#endif
#endif

e = 0;
#if defined(TRY_AGAIN)
#if defined(F_SETFL) && defined(O_NDELAY)
if ((e = fcntl(fd, F_GETFL, 0)) == -1)
return -1;

if (fcntl(fd, F_SETFL, e & ~O_NDELAY) == -1)
return -1;
else
e = 1;
#endif

#if defined(FIONBIO)
{
int zero = 0;

if (ioctl(fd, FIONBIO, &zero) == -1)
return -1;
else
e = 1;
}
#endif

#endif
return e ? 0 : -1;

case EINTR:
return 0;

default:
return -1;
}
}





void
el_wpush(EditLine *el, const wchar_t *str)
{
struct macros *ma = &el->el_read->macros;

if (str != NULL && ma->level + 1 < EL_MAXMACRO) {
ma->level++;
if ((ma->macro[ma->level] = wcsdup(str)) != NULL)
return;
ma->level--;
}
terminal_beep(el);
terminal__flush(el);
}







static int
read_getcmd(EditLine *el, el_action_t *cmdnum, wchar_t *ch)
{
static const wchar_t meta = (wchar_t)0x80;
el_action_t cmd;

do {
if (el_wgetc(el, ch) != 1)
return -1;

#if defined(KANJI)
if ((*ch & meta)) {
el->el_state.metanext = 0;
cmd = CcViMap[' '];
break;
} else
#endif

if (el->el_state.metanext) {
el->el_state.metanext = 0;
*ch |= meta;
}
if (*ch >= N_KEYS)
cmd = ED_INSERT;
else
cmd = el->el_map.current[(unsigned char) *ch];
if (cmd == ED_SEQUENCE_LEAD_IN) {
keymacro_value_t val;
switch (keymacro_get(el, ch, &val)) {
case XK_CMD:
cmd = val.cmd;
break;
case XK_STR:
el_wpush(el, val.str);
break;
case XK_NOD:
return -1;
default:
EL_ABORT((el->el_errfile, "Bad XK_ type \n"));
break;
}
}
} while (cmd == ED_SEQUENCE_LEAD_IN);
*cmdnum = cmd;
return 0;
}




static int
read_char(EditLine *el, wchar_t *cp)
{
ssize_t num_read;
int tried = (el->el_flags & FIXIO) == 0;
char cbuf[MB_LEN_MAX];
size_t cbp = 0;
int save_errno = errno;

again:
el->el_signal->sig_no = 0;
while ((num_read = read(el->el_infd, cbuf + cbp, (size_t)1)) == -1) {
int e = errno;
switch (el->el_signal->sig_no) {
case SIGCONT:
el_wset(el, EL_REFRESH);

case SIGWINCH:
sig_set(el);
goto again;
default:
break;
}
if (!tried && read__fixio(el->el_infd, e) == 0) {
errno = save_errno;
tried = 1;
} else {
errno = e;
*cp = L'\0';
return -1;
}
}


if (num_read == 0) {
*cp = L'\0';
return 0;
}

for (;;) {
mbstate_t mbs;

++cbp;

memset(&mbs, 0, sizeof(mbs));
switch (mbrtowc(cp, cbuf, cbp, &mbs)) {
case (size_t)-1:
if (cbp > 1) {




cbuf[0] = cbuf[cbp - 1];
cbp = 0;
break;
} else {

cbp = 0;
goto again;
}
case (size_t)-2:
if (cbp >= MB_LEN_MAX) {
errno = EILSEQ;
*cp = L'\0';
return -1;
}

goto again;
default:

return 1;
}
}
}




static void
read_pop(struct macros *ma)
{
int i;

el_free(ma->macro[0]);
for (i = 0; i < ma->level; i++)
ma->macro[i] = ma->macro[i + 1];
ma->level--;
ma->offset = 0;
}

static void
read_clearmacros(struct macros *ma)
{
while (ma->level >= 0)
el_free(ma->macro[ma->level--]);
ma->offset = 0;
}




int
el_wgetc(EditLine *el, wchar_t *cp)
{
struct macros *ma = &el->el_read->macros;
int num_read;

terminal__flush(el);
for (;;) {
if (ma->level < 0)
break;

if (ma->macro[0][ma->offset] == '\0') {
read_pop(ma);
continue;
}

*cp = ma->macro[0][ma->offset++];

if (ma->macro[0][ma->offset] == '\0') {

read_pop(ma);
}

return 1;
}

if (tty_rawmode(el) < 0)
return 0;

num_read = (*el->el_read->read_char)(el, cp);






if (num_read < 0)
el->el_read->read_errno = errno;

return num_read;
}

libedit_private void
read_prepare(EditLine *el)
{
if (el->el_flags & HANDLE_SIGNALS)
sig_set(el);
if (el->el_flags & NO_TTY)
return;
if ((el->el_flags & (UNBUFFERED|EDIT_DISABLED)) == UNBUFFERED)
tty_rawmode(el);



el_resize(el);
re_clear_display(el);
ch_reset(el);
re_refresh(el);

if (el->el_flags & UNBUFFERED)
terminal__flush(el);
}

libedit_private void
read_finish(EditLine *el)
{
if ((el->el_flags & UNBUFFERED) == 0)
(void) tty_cookedmode(el);
if (el->el_flags & HANDLE_SIGNALS)
sig_clr(el);
}

static const wchar_t *
noedit_wgets(EditLine *el, int *nread)
{
el_line_t *lp = &el->el_line;
int num;

while ((num = (*el->el_read->read_char)(el, lp->lastchar)) == 1) {
if (lp->lastchar + 1 >= lp->limit &&
!ch_enlargebufs(el, (size_t)2))
break;
lp->lastchar++;
if (el->el_flags & UNBUFFERED ||
lp->lastchar[-1] == '\r' ||
lp->lastchar[-1] == '\n')
break;
}
if (num == -1 && errno == EINTR)
lp->lastchar = lp->buffer;
lp->cursor = lp->lastchar;
*lp->lastchar = '\0';
*nread = (int)(lp->lastchar - lp->buffer);
return *nread ? lp->buffer : NULL;
}

const wchar_t *
el_wgets(EditLine *el, int *nread)
{
int retval;
el_action_t cmdnum = 0;
int num;
wchar_t ch;
int nrb;

if (nread == NULL)
nread = &nrb;
*nread = 0;
el->el_read->read_errno = 0;

if (el->el_flags & NO_TTY) {
el->el_line.lastchar = el->el_line.buffer;
return noedit_wgets(el, nread);
}

#if defined(FIONREAD)
if (el->el_tty.t_mode == EX_IO && el->el_read->macros.level < 0) {
int chrs = 0;

(void) ioctl(el->el_infd, FIONREAD, &chrs);
if (chrs == 0) {
if (tty_rawmode(el) < 0) {
errno = 0;
*nread = 0;
return NULL;
}
}
}
#endif

if ((el->el_flags & UNBUFFERED) == 0)
read_prepare(el);

if (el->el_flags & EDIT_DISABLED) {
if ((el->el_flags & UNBUFFERED) == 0)
el->el_line.lastchar = el->el_line.buffer;
terminal__flush(el);
return noedit_wgets(el, nread);
}

for (num = -1; num == -1;) {

if (read_getcmd(el, &cmdnum, &ch) == -1)
break;
if ((size_t)cmdnum >= el->el_map.nfunc)
continue;


el->el_state.thiscmd = cmdnum;
el->el_state.thisch = ch;
if (el->el_map.type == MAP_VI &&
el->el_map.current == el->el_map.key &&
el->el_chared.c_redo.pos < el->el_chared.c_redo.lim) {
if (cmdnum == VI_DELETE_PREV_CHAR &&
el->el_chared.c_redo.pos != el->el_chared.c_redo.buf
&& iswprint(el->el_chared.c_redo.pos[-1]))
el->el_chared.c_redo.pos--;
else
*el->el_chared.c_redo.pos++ = ch;
}
retval = (*el->el_map.func[cmdnum]) (el, ch);


el->el_state.lastcmd = cmdnum;


switch (retval) {
case CC_CURSOR:
re_refresh_cursor(el);
break;

case CC_REDISPLAY:
re_clear_lines(el);
re_clear_display(el);


case CC_REFRESH:
re_refresh(el);
break;

case CC_REFRESH_BEEP:
re_refresh(el);
terminal_beep(el);
break;

case CC_NORM:
break;

case CC_ARGHACK:

continue;

case CC_EOF:
if ((el->el_flags & UNBUFFERED) == 0)
num = 0;
else if (num == -1) {
*el->el_line.lastchar++ = CONTROL('d');
el->el_line.cursor = el->el_line.lastchar;
num = 1;
}
break;

case CC_NEWLINE:
num = (int)(el->el_line.lastchar - el->el_line.buffer);
break;

case CC_FATAL:

re_clear_display(el);
ch_reset(el);
read_clearmacros(&el->el_read->macros);
re_refresh(el);
break;

case CC_ERROR:
default:
terminal_beep(el);
terminal__flush(el);
break;
}
el->el_state.argument = 1;
el->el_state.doingarg = 0;
el->el_chared.c_vcmd.action = NOP;
if (el->el_flags & UNBUFFERED)
break;
}

terminal__flush(el);

if ((el->el_flags & UNBUFFERED) == 0) {
read_finish(el);
*nread = num != -1 ? num : 0;
} else
*nread = (int)(el->el_line.lastchar - el->el_line.buffer);

if (*nread == 0) {
if (num == -1) {
*nread = -1;
if (el->el_read->read_errno)
errno = el->el_read->read_errno;
}
return NULL;
} else
return el->el_line.buffer;
}
