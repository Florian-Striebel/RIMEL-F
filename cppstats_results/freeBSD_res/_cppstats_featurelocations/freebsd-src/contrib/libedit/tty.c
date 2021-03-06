

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)tty.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: tty.c,v 1.70 2021/07/14 07:47:23 christos Exp $");
#endif
#endif




#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "el.h"
#include "fcns.h"
#include "parse.h"

typedef struct ttymodes_t {
const char *m_name;
unsigned int m_value;
int m_type;
} ttymodes_t;

typedef struct ttymap_t {
wint_t nch, och;
el_action_t bind[3];
} ttymap_t;


static const ttyperm_t ttyperm = {
{
{"iflag:", ICRNL, (INLCR | IGNCR)},
{"oflag:", (OPOST | ONLCR), ONLRET},
{"cflag:", 0, 0},
{"lflag:", (ISIG | ICANON | ECHO | ECHOE | ECHOCTL | IEXTEN),
(NOFLSH | ECHONL | EXTPROC | FLUSHO)},
{"chars:", 0, 0},
},
{
{"iflag:", (INLCR | ICRNL), IGNCR},
{"oflag:", (OPOST | ONLCR), ONLRET},
{"cflag:", 0, 0},
{"lflag:", ISIG,
(NOFLSH | ICANON | ECHO | ECHOK | ECHONL | EXTPROC | IEXTEN | FLUSHO)},
{"chars:", (C_SH(C_MIN) | C_SH(C_TIME) | C_SH(C_SWTCH) | C_SH(C_DSWTCH) |
C_SH(C_SUSP) | C_SH(C_DSUSP) | C_SH(C_EOL) | C_SH(C_DISCARD) |
C_SH(C_PGOFF) | C_SH(C_PAGE) | C_SH(C_STATUS)), 0}
},
{
{"iflag:", 0, IXON | IXOFF | INLCR | ICRNL},
{"oflag:", 0, 0},
{"cflag:", 0, 0},
{"lflag:", 0, ISIG | IEXTEN},
{"chars:", 0, 0},
}
};

static const ttychar_t ttychar = {
{
CINTR, CQUIT, CERASE, CKILL,
CEOF, CEOL, CEOL2, CSWTCH,
CDSWTCH, CERASE2, CSTART, CSTOP,
CWERASE, CSUSP, CDSUSP, CREPRINT,
CDISCARD, CLNEXT, CSTATUS, CPAGE,
CPGOFF, CKILL2, CBRK, CMIN,
CTIME
},
{
CINTR, CQUIT, CERASE, CKILL,
_POSIX_VDISABLE, _POSIX_VDISABLE, _POSIX_VDISABLE, _POSIX_VDISABLE,
_POSIX_VDISABLE, CERASE2, CSTART, CSTOP,
_POSIX_VDISABLE, CSUSP, _POSIX_VDISABLE, _POSIX_VDISABLE,
CDISCARD, _POSIX_VDISABLE, _POSIX_VDISABLE, _POSIX_VDISABLE,
_POSIX_VDISABLE, _POSIX_VDISABLE, _POSIX_VDISABLE, 1,
0
},
{
0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
0
}
};

static const ttymap_t tty_map[] = {
#if defined(VERASE)
{C_ERASE, VERASE,
{EM_DELETE_PREV_CHAR, VI_DELETE_PREV_CHAR, ED_PREV_CHAR}},
#endif
#if defined(VERASE2)
{C_ERASE2, VERASE2,
{EM_DELETE_PREV_CHAR, VI_DELETE_PREV_CHAR, ED_PREV_CHAR}},
#endif
#if defined(VKILL)
{C_KILL, VKILL,
{EM_KILL_LINE, VI_KILL_LINE_PREV, ED_UNASSIGNED}},
#endif
#if defined(VKILL2)
{C_KILL2, VKILL2,
{EM_KILL_LINE, VI_KILL_LINE_PREV, ED_UNASSIGNED}},
#endif
#if defined(VEOF)
{C_EOF, VEOF,
{EM_DELETE_OR_LIST, VI_LIST_OR_EOF, ED_UNASSIGNED}},
#endif
#if defined(VWERASE)
{C_WERASE, VWERASE,
{ED_DELETE_PREV_WORD, ED_DELETE_PREV_WORD, ED_PREV_WORD}},
#endif
#if defined(VREPRINT)
{C_REPRINT, VREPRINT,
{ED_REDISPLAY, ED_INSERT, ED_REDISPLAY}},
#endif
#if defined(VLNEXT)
{C_LNEXT, VLNEXT,
{ED_QUOTED_INSERT, ED_QUOTED_INSERT, ED_UNASSIGNED}},
#endif
{(wint_t)-1, (wint_t)-1,
{ED_UNASSIGNED, ED_UNASSIGNED, ED_UNASSIGNED}}
};

static const ttymodes_t ttymodes[] = {
#if defined(IGNBRK)
{"ignbrk", IGNBRK, MD_INP},
#endif
#if defined(BRKINT)
{"brkint", BRKINT, MD_INP},
#endif
#if defined(IGNPAR)
{"ignpar", IGNPAR, MD_INP},
#endif
#if defined(PARMRK)
{"parmrk", PARMRK, MD_INP},
#endif
#if defined(INPCK)
{"inpck", INPCK, MD_INP},
#endif
#if defined(ISTRIP)
{"istrip", ISTRIP, MD_INP},
#endif
#if defined(INLCR)
{"inlcr", INLCR, MD_INP},
#endif
#if defined(IGNCR)
{"igncr", IGNCR, MD_INP},
#endif
#if defined(ICRNL)
{"icrnl", ICRNL, MD_INP},
#endif
#if defined(IUCLC)
{"iuclc", IUCLC, MD_INP},
#endif
#if defined(IXON)
{"ixon", IXON, MD_INP},
#endif
#if defined(IXANY)
{"ixany", IXANY, MD_INP},
#endif
#if defined(IXOFF)
{"ixoff", IXOFF, MD_INP},
#endif
#if defined(IMAXBEL)
{"imaxbel", IMAXBEL, MD_INP},
#endif

#if defined(OPOST)
{"opost", OPOST, MD_OUT},
#endif
#if defined(OLCUC)
{"olcuc", OLCUC, MD_OUT},
#endif
#if defined(ONLCR)
{"onlcr", ONLCR, MD_OUT},
#endif
#if defined(OCRNL)
{"ocrnl", OCRNL, MD_OUT},
#endif
#if defined(ONOCR)
{"onocr", ONOCR, MD_OUT},
#endif
#if defined(ONOEOT)
{"onoeot", ONOEOT, MD_OUT},
#endif
#if defined(ONLRET)
{"onlret", ONLRET, MD_OUT},
#endif
#if defined(OFILL)
{"ofill", OFILL, MD_OUT},
#endif
#if defined(OFDEL)
{"ofdel", OFDEL, MD_OUT},
#endif
#if defined(NLDLY)
{"nldly", NLDLY, MD_OUT},
#endif
#if defined(CRDLY)
{"crdly", CRDLY, MD_OUT},
#endif
#if defined(TABDLY)
{"tabdly", TABDLY, MD_OUT},
#endif
#if defined(XTABS)
{"xtabs", XTABS, MD_OUT},
#endif
#if defined(BSDLY)
{"bsdly", BSDLY, MD_OUT},
#endif
#if defined(VTDLY)
{"vtdly", VTDLY, MD_OUT},
#endif
#if defined(FFDLY)
{"ffdly", FFDLY, MD_OUT},
#endif
#if defined(PAGEOUT)
{"pageout", PAGEOUT, MD_OUT},
#endif
#if defined(WRAP)
{"wrap", WRAP, MD_OUT},
#endif

#if defined(CIGNORE)
{"cignore", CIGNORE, MD_CTL},
#endif
#if defined(CBAUD)
{"cbaud", CBAUD, MD_CTL},
#endif
#if defined(CSTOPB)
{"cstopb", CSTOPB, MD_CTL},
#endif
#if defined(CREAD)
{"cread", CREAD, MD_CTL},
#endif
#if defined(PARENB)
{"parenb", PARENB, MD_CTL},
#endif
#if defined(PARODD)
{"parodd", PARODD, MD_CTL},
#endif
#if defined(HUPCL)
{"hupcl", HUPCL, MD_CTL},
#endif
#if defined(CLOCAL)
{"clocal", CLOCAL, MD_CTL},
#endif
#if defined(LOBLK)
{"loblk", LOBLK, MD_CTL},
#endif
#if defined(CIBAUD)
{"cibaud", CIBAUD, MD_CTL},
#endif
#if defined(CRTSCTS)
#if defined(CCTS_OFLOW)
{"ccts_oflow", CCTS_OFLOW, MD_CTL},
#else
{"crtscts", CRTSCTS, MD_CTL},
#endif
#endif
#if defined(CRTS_IFLOW)
{"crts_iflow", CRTS_IFLOW, MD_CTL},
#endif
#if defined(CDTRCTS)
{"cdtrcts", CDTRCTS, MD_CTL},
#endif
#if defined(MDMBUF)
{"mdmbuf", MDMBUF, MD_CTL},
#endif
#if defined(RCV1EN)
{"rcv1en", RCV1EN, MD_CTL},
#endif
#if defined(XMT1EN)
{"xmt1en", XMT1EN, MD_CTL},
#endif

#if defined(ISIG)
{"isig", ISIG, MD_LIN},
#endif
#if defined(ICANON)
{"icanon", ICANON, MD_LIN},
#endif
#if defined(XCASE)
{"xcase", XCASE, MD_LIN},
#endif
#if defined(ECHO)
{"echo", ECHO, MD_LIN},
#endif
#if defined(ECHOE)
{"echoe", ECHOE, MD_LIN},
#endif
#if defined(ECHOK)
{"echok", ECHOK, MD_LIN},
#endif
#if defined(ECHONL)
{"echonl", ECHONL, MD_LIN},
#endif
#if defined(NOFLSH)
{"noflsh", NOFLSH, MD_LIN},
#endif
#if defined(TOSTOP)
{"tostop", TOSTOP, MD_LIN},
#endif
#if defined(ECHOCTL)
{"echoctl", ECHOCTL, MD_LIN},
#endif
#if defined(ECHOPRT)
{"echoprt", ECHOPRT, MD_LIN},
#endif
#if defined(ECHOKE)
{"echoke", ECHOKE, MD_LIN},
#endif
#if defined(DEFECHO)
{"defecho", DEFECHO, MD_LIN},
#endif
#if defined(FLUSHO)
{"flusho", FLUSHO, MD_LIN},
#endif
#if defined(PENDIN)
{"pendin", PENDIN, MD_LIN},
#endif
#if defined(IEXTEN)
{"iexten", IEXTEN, MD_LIN},
#endif
#if defined(NOKERNINFO)
{"nokerninfo", NOKERNINFO, MD_LIN},
#endif
#if defined(ALTWERASE)
{"altwerase", ALTWERASE, MD_LIN},
#endif
#if defined(EXTPROC)
{"extproc", EXTPROC, MD_LIN},
#endif

#if defined(VINTR)
{"intr", C_SH(C_INTR), MD_CHAR},
#endif
#if defined(VQUIT)
{"quit", C_SH(C_QUIT), MD_CHAR},
#endif
#if defined(VERASE)
{"erase", C_SH(C_ERASE), MD_CHAR},
#endif
#if defined(VKILL)
{"kill", C_SH(C_KILL), MD_CHAR},
#endif
#if defined(VEOF)
{"eof", C_SH(C_EOF), MD_CHAR},
#endif
#if defined(VEOL)
{"eol", C_SH(C_EOL), MD_CHAR},
#endif
#if defined(VEOL2)
{"eol2", C_SH(C_EOL2), MD_CHAR},
#endif
#if defined(VSWTCH)
{"swtch", C_SH(C_SWTCH), MD_CHAR},
#endif
#if defined(VDSWTCH)
{"dswtch", C_SH(C_DSWTCH), MD_CHAR},
#endif
#if defined(VERASE2)
{"erase2", C_SH(C_ERASE2), MD_CHAR},
#endif
#if defined(VSTART)
{"start", C_SH(C_START), MD_CHAR},
#endif
#if defined(VSTOP)
{"stop", C_SH(C_STOP), MD_CHAR},
#endif
#if defined(VWERASE)
{"werase", C_SH(C_WERASE), MD_CHAR},
#endif
#if defined(VSUSP)
{"susp", C_SH(C_SUSP), MD_CHAR},
#endif
#if defined(VDSUSP)
{"dsusp", C_SH(C_DSUSP), MD_CHAR},
#endif
#if defined(VREPRINT)
{"reprint", C_SH(C_REPRINT), MD_CHAR},
#endif
#if defined(VDISCARD)
{"discard", C_SH(C_DISCARD), MD_CHAR},
#endif
#if defined(VLNEXT)
{"lnext", C_SH(C_LNEXT), MD_CHAR},
#endif
#if defined(VSTATUS)
{"status", C_SH(C_STATUS), MD_CHAR},
#endif
#if defined(VPAGE)
{"page", C_SH(C_PAGE), MD_CHAR},
#endif
#if defined(VPGOFF)
{"pgoff", C_SH(C_PGOFF), MD_CHAR},
#endif
#if defined(VKILL2)
{"kill2", C_SH(C_KILL2), MD_CHAR},
#endif
#if defined(VBRK)
{"brk", C_SH(C_BRK), MD_CHAR},
#endif
#if defined(VMIN)
{"min", C_SH(C_MIN), MD_CHAR},
#endif
#if defined(VTIME)
{"time", C_SH(C_TIME), MD_CHAR},
#endif
{NULL, 0, -1},
};



#define tty__gettabs(td) ((((td)->c_oflag & TAB3) == TAB3) ? 0 : 1)
#define tty__geteightbit(td) (((td)->c_cflag & CSIZE) == CS8)
#define tty__cooked_mode(td) ((td)->c_lflag & ICANON)

static int tty_getty(EditLine *, struct termios *);
static int tty_setty(EditLine *, int, const struct termios *);
static int tty__getcharindex(int);
static void tty__getchar(struct termios *, unsigned char *);
static void tty__setchar(struct termios *, unsigned char *);
static speed_t tty__getspeed(struct termios *);
static int tty_setup(EditLine *);
static void tty_setup_flags(EditLine *, struct termios *, int);

#define t_qu t_ts




static int
tty_getty(EditLine *el, struct termios *t)
{
int rv;
while ((rv = tcgetattr(el->el_infd, t)) == -1 && errno == EINTR)
continue;
return rv;
}




static int
tty_setty(EditLine *el, int action, const struct termios *t)
{
int rv;
while ((rv = tcsetattr(el->el_infd, action, t)) == -1 && errno == EINTR)
continue;
return rv;
}




static int
tty_setup(EditLine *el)
{
int rst = (el->el_flags & NO_RESET) == 0;

if (el->el_flags & EDIT_DISABLED)
return 0;

if (el->el_tty.t_initialized)
return -1;

if (!isatty(el->el_outfd)) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: isatty: %s\n", __func__,
strerror(errno));
#endif
return -1;
}
if (tty_getty(el, &el->el_tty.t_or) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_getty: %s\n", __func__,
strerror(errno));
#endif
return -1;
}
el->el_tty.t_ts = el->el_tty.t_ex = el->el_tty.t_ed = el->el_tty.t_or;

el->el_tty.t_speed = tty__getspeed(&el->el_tty.t_ex);
el->el_tty.t_tabs = tty__gettabs(&el->el_tty.t_ex);
el->el_tty.t_eight = tty__geteightbit(&el->el_tty.t_ex);

tty_setup_flags(el, &el->el_tty.t_ex, EX_IO);





if (rst) {
if (tty__cooked_mode(&el->el_tty.t_ts)) {
tty__getchar(&el->el_tty.t_ts, el->el_tty.t_c[TS_IO]);



for (rst = 0; rst < C_NCC - 2; rst++)
if (el->el_tty.t_c[TS_IO][rst] !=
el->el_tty.t_vdisable
&& el->el_tty.t_c[ED_IO][rst] !=
el->el_tty.t_vdisable)
el->el_tty.t_c[ED_IO][rst] =
el->el_tty.t_c[TS_IO][rst];
for (rst = 0; rst < C_NCC; rst++)
if (el->el_tty.t_c[TS_IO][rst] !=
el->el_tty.t_vdisable)
el->el_tty.t_c[EX_IO][rst] =
el->el_tty.t_c[TS_IO][rst];
}
tty__setchar(&el->el_tty.t_ex, el->el_tty.t_c[EX_IO]);
if (tty_setty(el, TCSADRAIN, &el->el_tty.t_ex) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_setty: %s\n",
__func__, strerror(errno));
#endif
return -1;
}
}

tty_setup_flags(el, &el->el_tty.t_ed, ED_IO);

tty__setchar(&el->el_tty.t_ed, el->el_tty.t_c[ED_IO]);
tty_bind_char(el, 1);
el->el_tty.t_initialized = 1;
return 0;
}

libedit_private int
tty_init(EditLine *el)
{

el->el_tty.t_mode = EX_IO;
el->el_tty.t_vdisable = _POSIX_VDISABLE;
el->el_tty.t_initialized = 0;
(void) memcpy(el->el_tty.t_t, ttyperm, sizeof(ttyperm_t));
(void) memcpy(el->el_tty.t_c, ttychar, sizeof(ttychar_t));
return tty_setup(el);
}





libedit_private void

tty_end(EditLine *el, int how)
{
if (el->el_flags & EDIT_DISABLED)
return;

if (!el->el_tty.t_initialized)
return;

if (tty_setty(el, how, &el->el_tty.t_or) == -1)
{
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile,
"%s: tty_setty: %s\n", __func__, strerror(errno));
#endif
}
}





static speed_t
tty__getspeed(struct termios *td)
{
speed_t spd;

if ((spd = cfgetispeed(td)) == 0)
spd = cfgetospeed(td);
return spd;
}




static int
tty__getcharindex(int i)
{
switch (i) {
#if defined(VINTR)
case C_INTR:
return VINTR;
#endif
#if defined(VQUIT)
case C_QUIT:
return VQUIT;
#endif
#if defined(VERASE)
case C_ERASE:
return VERASE;
#endif
#if defined(VKILL)
case C_KILL:
return VKILL;
#endif
#if defined(VEOF)
case C_EOF:
return VEOF;
#endif
#if defined(VEOL)
case C_EOL:
return VEOL;
#endif
#if defined(VEOL2)
case C_EOL2:
return VEOL2;
#endif
#if defined(VSWTCH)
case C_SWTCH:
return VSWTCH;
#endif
#if defined(VDSWTCH)
case C_DSWTCH:
return VDSWTCH;
#endif
#if defined(VERASE2)
case C_ERASE2:
return VERASE2;
#endif
#if defined(VSTART)
case C_START:
return VSTART;
#endif
#if defined(VSTOP)
case C_STOP:
return VSTOP;
#endif
#if defined(VWERASE)
case C_WERASE:
return VWERASE;
#endif
#if defined(VSUSP)
case C_SUSP:
return VSUSP;
#endif
#if defined(VDSUSP)
case C_DSUSP:
return VDSUSP;
#endif
#if defined(VREPRINT)
case C_REPRINT:
return VREPRINT;
#endif
#if defined(VDISCARD)
case C_DISCARD:
return VDISCARD;
#endif
#if defined(VLNEXT)
case C_LNEXT:
return VLNEXT;
#endif
#if defined(VSTATUS)
case C_STATUS:
return VSTATUS;
#endif
#if defined(VPAGE)
case C_PAGE:
return VPAGE;
#endif
#if defined(VPGOFF)
case C_PGOFF:
return VPGOFF;
#endif
#if defined(VKILL2)
case C_KILL2:
return VKILL2;
#endif
#if defined(VMIN)
case C_MIN:
return VMIN;
#endif
#if defined(VTIME)
case C_TIME:
return VTIME;
#endif
default:
return -1;
}
}




static void
tty__getchar(struct termios *td, unsigned char *s)
{

#if defined(VINTR)
s[C_INTR] = td->c_cc[VINTR];
#endif
#if defined(VQUIT)
s[C_QUIT] = td->c_cc[VQUIT];
#endif
#if defined(VERASE)
s[C_ERASE] = td->c_cc[VERASE];
#endif
#if defined(VKILL)
s[C_KILL] = td->c_cc[VKILL];
#endif
#if defined(VEOF)
s[C_EOF] = td->c_cc[VEOF];
#endif
#if defined(VEOL)
s[C_EOL] = td->c_cc[VEOL];
#endif
#if defined(VEOL2)
s[C_EOL2] = td->c_cc[VEOL2];
#endif
#if defined(VSWTCH)
s[C_SWTCH] = td->c_cc[VSWTCH];
#endif
#if defined(VDSWTCH)
s[C_DSWTCH] = td->c_cc[VDSWTCH];
#endif
#if defined(VERASE2)
s[C_ERASE2] = td->c_cc[VERASE2];
#endif
#if defined(VSTART)
s[C_START] = td->c_cc[VSTART];
#endif
#if defined(VSTOP)
s[C_STOP] = td->c_cc[VSTOP];
#endif
#if defined(VWERASE)
s[C_WERASE] = td->c_cc[VWERASE];
#endif
#if defined(VSUSP)
s[C_SUSP] = td->c_cc[VSUSP];
#endif
#if defined(VDSUSP)
s[C_DSUSP] = td->c_cc[VDSUSP];
#endif
#if defined(VREPRINT)
s[C_REPRINT] = td->c_cc[VREPRINT];
#endif
#if defined(VDISCARD)
s[C_DISCARD] = td->c_cc[VDISCARD];
#endif
#if defined(VLNEXT)
s[C_LNEXT] = td->c_cc[VLNEXT];
#endif
#if defined(VSTATUS)
s[C_STATUS] = td->c_cc[VSTATUS];
#endif
#if defined(VPAGE)
s[C_PAGE] = td->c_cc[VPAGE];
#endif
#if defined(VPGOFF)
s[C_PGOFF] = td->c_cc[VPGOFF];
#endif
#if defined(VKILL2)
s[C_KILL2] = td->c_cc[VKILL2];
#endif
#if defined(VMIN)
s[C_MIN] = td->c_cc[VMIN];
#endif
#if defined(VTIME)
s[C_TIME] = td->c_cc[VTIME];
#endif
}





static void
tty__setchar(struct termios *td, unsigned char *s)
{

#if defined(VINTR)
td->c_cc[VINTR] = s[C_INTR];
#endif
#if defined(VQUIT)
td->c_cc[VQUIT] = s[C_QUIT];
#endif
#if defined(VERASE)
td->c_cc[VERASE] = s[C_ERASE];
#endif
#if defined(VKILL)
td->c_cc[VKILL] = s[C_KILL];
#endif
#if defined(VEOF)
td->c_cc[VEOF] = s[C_EOF];
#endif
#if defined(VEOL)
td->c_cc[VEOL] = s[C_EOL];
#endif
#if defined(VEOL2)
td->c_cc[VEOL2] = s[C_EOL2];
#endif
#if defined(VSWTCH)
td->c_cc[VSWTCH] = s[C_SWTCH];
#endif
#if defined(VDSWTCH)
td->c_cc[VDSWTCH] = s[C_DSWTCH];
#endif
#if defined(VERASE2)
td->c_cc[VERASE2] = s[C_ERASE2];
#endif
#if defined(VSTART)
td->c_cc[VSTART] = s[C_START];
#endif
#if defined(VSTOP)
td->c_cc[VSTOP] = s[C_STOP];
#endif
#if defined(VWERASE)
td->c_cc[VWERASE] = s[C_WERASE];
#endif
#if defined(VSUSP)
td->c_cc[VSUSP] = s[C_SUSP];
#endif
#if defined(VDSUSP)
td->c_cc[VDSUSP] = s[C_DSUSP];
#endif
#if defined(VREPRINT)
td->c_cc[VREPRINT] = s[C_REPRINT];
#endif
#if defined(VDISCARD)
td->c_cc[VDISCARD] = s[C_DISCARD];
#endif
#if defined(VLNEXT)
td->c_cc[VLNEXT] = s[C_LNEXT];
#endif
#if defined(VSTATUS)
td->c_cc[VSTATUS] = s[C_STATUS];
#endif
#if defined(VPAGE)
td->c_cc[VPAGE] = s[C_PAGE];
#endif
#if defined(VPGOFF)
td->c_cc[VPGOFF] = s[C_PGOFF];
#endif
#if defined(VKILL2)
td->c_cc[VKILL2] = s[C_KILL2];
#endif
#if defined(VMIN)
td->c_cc[VMIN] = s[C_MIN];
#endif
#if defined(VTIME)
td->c_cc[VTIME] = s[C_TIME];
#endif
}





libedit_private void
tty_bind_char(EditLine *el, int force)
{

unsigned char *t_n = el->el_tty.t_c[ED_IO];
unsigned char *t_o = el->el_tty.t_ed.c_cc;
wchar_t new[2], old[2];
const ttymap_t *tp;
el_action_t *map, *alt;
const el_action_t *dmap, *dalt;
new[1] = old[1] = '\0';

map = el->el_map.key;
alt = el->el_map.alt;
if (el->el_map.type == MAP_VI) {
dmap = el->el_map.vii;
dalt = el->el_map.vic;
} else {
dmap = el->el_map.emacs;
dalt = NULL;
}

for (tp = tty_map; tp->nch != (wint_t)-1; tp++) {
new[0] = (wchar_t)t_n[tp->nch];
old[0] = (wchar_t)t_o[tp->och];
if (new[0] == old[0] && !force)
continue;

keymacro_clear(el, map, old);
map[(unsigned char)old[0]] = dmap[(unsigned char)old[0]];
keymacro_clear(el, map, new);

map[(unsigned char)new[0]] = tp->bind[el->el_map.type];
if (dalt) {
keymacro_clear(el, alt, old);
alt[(unsigned char)old[0]] =
dalt[(unsigned char)old[0]];
keymacro_clear(el, alt, new);
alt[(unsigned char)new[0]] =
tp->bind[el->el_map.type + 1];
}
}
}


static tcflag_t *
tty__get_flag(struct termios *t, int kind) {
switch (kind) {
case MD_INP:
return &t->c_iflag;
case MD_OUT:
return &t->c_oflag;
case MD_CTL:
return &t->c_cflag;
case MD_LIN:
return &t->c_lflag;
default:
abort();

}
}


static tcflag_t
tty_update_flag(EditLine *el, tcflag_t f, int mode, int kind)
{
f &= ~el->el_tty.t_t[mode][kind].t_clrmask;
f |= el->el_tty.t_t[mode][kind].t_setmask;
return f;
}


static void
tty_update_flags(EditLine *el, int kind)
{
tcflag_t *tt, *ed, *ex;
tt = tty__get_flag(&el->el_tty.t_ts, kind);
ed = tty__get_flag(&el->el_tty.t_ed, kind);
ex = tty__get_flag(&el->el_tty.t_ex, kind);

if (*tt != *ex && (kind != MD_CTL || *tt != *ed)) {
*ed = tty_update_flag(el, *tt, ED_IO, kind);
*ex = tty_update_flag(el, *tt, EX_IO, kind);
}
}


static void
tty_update_char(EditLine *el, int mode, int c) {
if (!((el->el_tty.t_t[mode][MD_CHAR].t_setmask & C_SH(c)))
&& (el->el_tty.t_c[TS_IO][c] != el->el_tty.t_c[EX_IO][c]))
el->el_tty.t_c[mode][c] = el->el_tty.t_c[TS_IO][c];
if (el->el_tty.t_t[mode][MD_CHAR].t_clrmask & C_SH(c))
el->el_tty.t_c[mode][c] = el->el_tty.t_vdisable;
}





libedit_private int
tty_rawmode(EditLine *el)
{

if (el->el_tty.t_mode == ED_IO || el->el_tty.t_mode == QU_IO)
return 0;

if (el->el_flags & EDIT_DISABLED)
return 0;

if (tty_getty(el, &el->el_tty.t_ts) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_getty: %s\n", __func__,
strerror(errno));
#endif
return -1;
}




el->el_tty.t_eight = tty__geteightbit(&el->el_tty.t_ts);
el->el_tty.t_speed = tty__getspeed(&el->el_tty.t_ts);

if (tty__getspeed(&el->el_tty.t_ex) != el->el_tty.t_speed ||
tty__getspeed(&el->el_tty.t_ed) != el->el_tty.t_speed) {
(void) cfsetispeed(&el->el_tty.t_ex, el->el_tty.t_speed);
(void) cfsetospeed(&el->el_tty.t_ex, el->el_tty.t_speed);
(void) cfsetispeed(&el->el_tty.t_ed, el->el_tty.t_speed);
(void) cfsetospeed(&el->el_tty.t_ed, el->el_tty.t_speed);
}
if (tty__cooked_mode(&el->el_tty.t_ts)) {
int i;

for (i = MD_INP; i <= MD_LIN; i++)
tty_update_flags(el, i);

if (tty__gettabs(&el->el_tty.t_ex) == 0)
el->el_tty.t_tabs = 0;
else
el->el_tty.t_tabs = EL_CAN_TAB ? 1 : 0;

tty__getchar(&el->el_tty.t_ts, el->el_tty.t_c[TS_IO]);





for (i = 0; i < C_NCC; i++)
if (el->el_tty.t_c[TS_IO][i] !=
el->el_tty.t_c[EX_IO][i])
break;

if (i != C_NCC) {




for (i = 0; i < C_NCC; i++)
tty_update_char(el, ED_IO, i);

tty_bind_char(el, 0);
tty__setchar(&el->el_tty.t_ed, el->el_tty.t_c[ED_IO]);

for (i = 0; i < C_NCC; i++)
tty_update_char(el, EX_IO, i);

tty__setchar(&el->el_tty.t_ex, el->el_tty.t_c[EX_IO]);
}
}
if (tty_setty(el, TCSADRAIN, &el->el_tty.t_ed) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_setty: %s\n", __func__,
strerror(errno));
#endif
return -1;
}
el->el_tty.t_mode = ED_IO;
return 0;
}





libedit_private int
tty_cookedmode(EditLine *el)
{

if (el->el_tty.t_mode == EX_IO)
return 0;

if (el->el_flags & EDIT_DISABLED)
return 0;

if (tty_setty(el, TCSADRAIN, &el->el_tty.t_ex) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_setty: %s\n", __func__,
strerror(errno));
#endif
return -1;
}
el->el_tty.t_mode = EX_IO;
return 0;
}





libedit_private int
tty_quotemode(EditLine *el)
{
if (el->el_tty.t_mode == QU_IO)
return 0;

el->el_tty.t_qu = el->el_tty.t_ed;

tty_setup_flags(el, &el->el_tty.t_qu, QU_IO);

if (tty_setty(el, TCSADRAIN, &el->el_tty.t_qu) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_setty: %s\n", __func__,
strerror(errno));
#endif
return -1;
}
el->el_tty.t_mode = QU_IO;
return 0;
}





libedit_private int
tty_noquotemode(EditLine *el)
{

if (el->el_tty.t_mode != QU_IO)
return 0;
if (tty_setty(el, TCSADRAIN, &el->el_tty.t_ed) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_setty: %s\n", __func__,
strerror(errno));
#endif
return -1;
}
el->el_tty.t_mode = ED_IO;
return 0;
}





libedit_private int

tty_stty(EditLine *el, int argc __attribute__((__unused__)),
const wchar_t **argv)
{
const ttymodes_t *m;
char x;
int aflag = 0;
const wchar_t *s, *d;
char name[EL_BUFSIZ];
struct termios *tios = &el->el_tty.t_ex;
int z = EX_IO;

if (argv == NULL)
return -1;
strlcpy(name, ct_encode_string(*argv++, &el->el_scratch), sizeof(name));

while (argv && *argv && argv[0][0] == '-' && argv[0][2] == '\0')
switch (argv[0][1]) {
case 'a':
aflag++;
argv++;
break;
case 'd':
argv++;
tios = &el->el_tty.t_ed;
z = ED_IO;
break;
case 'x':
argv++;
tios = &el->el_tty.t_ex;
z = EX_IO;
break;
case 'q':
argv++;
tios = &el->el_tty.t_ts;
z = QU_IO;
break;
default:
(void) fprintf(el->el_errfile,
"%s: Unknown switch `%lc'.\n",
name, (wint_t)argv[0][1]);
return -1;
}

if (!argv || !*argv) {
int i = -1;
size_t len = 0, st = 0, cu;
for (m = ttymodes; m->m_name; m++) {
if (m->m_type != i) {
(void) fprintf(el->el_outfile, "%s%s",
i != -1 ? "\n" : "",
el->el_tty.t_t[z][m->m_type].t_name);
i = m->m_type;
st = len =
strlen(el->el_tty.t_t[z][m->m_type].t_name);
}
if (i != -1) {
x = (el->el_tty.t_t[z][i].t_setmask & m->m_value)
? '+' : '\0';

if (el->el_tty.t_t[z][i].t_clrmask & m->m_value)
x = '-';
} else {
x = '\0';
}

if (x != '\0' || aflag) {

cu = strlen(m->m_name) + (x != '\0') + 1;

if (len + cu >=
(size_t)el->el_terminal.t_size.h) {
(void) fprintf(el->el_outfile, "\n%*s",
(int)st, "");
len = st + cu;
} else
len += cu;

if (x != '\0')
(void) fprintf(el->el_outfile, "%c%s ",
x, m->m_name);
else
(void) fprintf(el->el_outfile, "%s ",
m->m_name);
}
}
(void) fprintf(el->el_outfile, "\n");
return 0;
}
while (argv && (s = *argv++)) {
const wchar_t *p;
switch (*s) {
case '+':
case '-':
x = (char)*s++;
break;
default:
x = '\0';
break;
}
d = s;
p = wcschr(s, L'=');
for (m = ttymodes; m->m_name; m++)
if ((p ? strncmp(m->m_name, ct_encode_string(d,
&el->el_scratch), (size_t)(p - d)) :
strcmp(m->m_name, ct_encode_string(d,
&el->el_scratch))) == 0 &&
(p == NULL || m->m_type == MD_CHAR))
break;

if (!m->m_name) {
(void) fprintf(el->el_errfile,
"%s: Invalid argument `%ls'.\n", name, d);
return -1;
}
if (p) {
int c = ffs((int)m->m_value);
int v = *++p ? parse__escape(&p) :
el->el_tty.t_vdisable;
assert(c != 0);
c--;
c = tty__getcharindex(c);
assert(c != -1);
tios->c_cc[c] = (cc_t)v;
continue;
}
switch (x) {
case '+':
el->el_tty.t_t[z][m->m_type].t_setmask |= m->m_value;
el->el_tty.t_t[z][m->m_type].t_clrmask &= ~m->m_value;
break;
case '-':
el->el_tty.t_t[z][m->m_type].t_setmask &= ~m->m_value;
el->el_tty.t_t[z][m->m_type].t_clrmask |= m->m_value;
break;
default:
el->el_tty.t_t[z][m->m_type].t_setmask &= ~m->m_value;
el->el_tty.t_t[z][m->m_type].t_clrmask &= ~m->m_value;
break;
}
}

tty_setup_flags(el, tios, z);
if (el->el_tty.t_mode == z) {
if (tty_setty(el, TCSADRAIN, tios) == -1) {
#if defined(DEBUG_TTY)
(void) fprintf(el->el_errfile, "%s: tty_setty: %s\n",
__func__, strerror(errno));
#endif
return -1;
}
}

return 0;
}


#if defined(notyet)



static void
tty_printchar(EditLine *el, unsigned char *s)
{
ttyperm_t *m;
int i;

for (i = 0; i < C_NCC; i++) {
for (m = el->el_tty.t_t; m->m_name; m++)
if (m->m_type == MD_CHAR && C_SH(i) == m->m_value)
break;
if (m->m_name)
(void) fprintf(el->el_errfile, "%s ^%c ",
m->m_name, s[i] + 'A' - 1);
if (i % 5 == 0)
(void) fprintf(el->el_errfile, "\n");
}
(void) fprintf(el->el_errfile, "\n");
}
#endif


static void
tty_setup_flags(EditLine *el, struct termios *tios, int mode)
{
int kind;
for (kind = MD_INP; kind <= MD_LIN; kind++) {
tcflag_t *f = tty__get_flag(tios, kind);
*f = tty_update_flag(el, *f, mode, kind);
}
}

libedit_private int
tty_get_signal_character(EditLine *el, int sig)
{
#if defined(ECHOCTL)
tcflag_t *ed = tty__get_flag(&el->el_tty.t_ed, MD_INP);
if ((*ed & ECHOCTL) == 0)
return -1;
#endif
switch (sig) {
#if defined(SIGINT) && defined(VINTR)
case SIGINT:
return el->el_tty.t_c[ED_IO][VINTR];
#endif
#if defined(SIGQUIT) && defined(VQUIT)
case SIGQUIT:
return el->el_tty.t_c[ED_IO][VQUIT];
#endif
#if defined(SIGINFO) && defined(VSTATUS)
case SIGINFO:
return el->el_tty.t_c[ED_IO][VSTATUS];
#endif
#if defined(SIGTSTP) && defined(VSUSP)
case SIGTSTP:
return el->el_tty.t_c[ED_IO][VSUSP];
#endif
default:
return -1;
}
}
