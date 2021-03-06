













#include "less.h"
#if MSDOS_COMPILER==WIN32C
#include "windows.h"
#if !defined(COMMON_LVB_UNDERSCORE)
#define COMMON_LVB_UNDERSCORE 0x8000
#endif
#endif

public int errmsgs;
public int need_clr;
public int final_attr;
public int at_prompt;

extern int sigs;
extern int sc_width;
extern int so_s_width, so_e_width;
extern int screen_trashed;
extern int is_tty;
extern int oldbot;

#if MSDOS_COMPILER==WIN32C || MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
extern int ctldisp;
extern int nm_fg_color, nm_bg_color;
extern int bo_fg_color, bo_bg_color;
extern int ul_fg_color, ul_bg_color;
extern int so_fg_color, so_bg_color;
extern int bl_fg_color, bl_bg_color;
extern int sgr_mode;
#if MSDOS_COMPILER==WIN32C
extern int vt_enabled;
#endif
#endif




public void
put_line(VOID_PARAM)
{
int c;
int i;
int a;

if (ABORT_SIGS())
{



screen_trashed = 1;
return;
}

final_attr = AT_NORMAL;

for (i = 0; (c = gline(i, &a)) != '\0'; i++)
{
at_switch(a);
final_attr = a;
if (c == '\b')
putbs();
else
putchr(c);
}

at_exit();
}

static char obuf[OUTBUF_SIZE];
static char *ob = obuf;
static int outfd = 2;

#if MSDOS_COMPILER==WIN32C || MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
static void
win_flush(VOID_PARAM)
{
if (ctldisp != OPT_ONPLUS || (vt_enabled && sgr_mode))
WIN32textout(obuf, ob - obuf);
else
{






char *anchor, *p, *p_next;
static int fg, fgi, bg, bgi;
static int at;
int f, b;
#if MSDOS_COMPILER==WIN32C

static unsigned char screen_color[] = {
0,
FOREGROUND_RED,
FOREGROUND_GREEN,
FOREGROUND_RED|FOREGROUND_GREEN,
FOREGROUND_BLUE,
FOREGROUND_BLUE|FOREGROUND_RED,
FOREGROUND_BLUE|FOREGROUND_GREEN,
FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED
};
#else
static enum COLORS screen_color[] = {
BLACK, RED, GREEN, BROWN,
BLUE, MAGENTA, CYAN, LIGHTGRAY
};
#endif

if (fg == 0 && bg == 0)
{
fg = nm_fg_color & 7;
fgi = nm_fg_color & 8;
bg = nm_bg_color & 7;
bgi = nm_bg_color & 8;
}
for (anchor = p_next = obuf;
(p_next = memchr(p_next, ESC, ob - p_next)) != NULL; )
{
p = p_next;
if (p[1] == '[')
{
if (p > anchor)
{





WIN32textout(anchor, p-anchor);
anchor = p;
}
p += 2;
if (is_ansi_end(*p))
{





p++;
anchor = p_next = p;
fg = nm_fg_color & 7;
fgi = nm_fg_color & 8;
bg = nm_bg_color & 7;
bgi = nm_bg_color & 8;
at = 0;
WIN32setcolors(nm_fg_color, nm_bg_color);
continue;
}
p_next = p;
at &= ~32;





while (!is_ansi_end(*p))
{
char *q;
long code = strtol(p, &q, 10);

if (*q == '\0')
{





int slop = (int) (q - anchor);

strcpy(obuf, anchor);
ob = &obuf[slop];
return;
}

if (q == p ||
code > 49 || code < 0 ||
(!is_ansi_end(*q) && *q != ';'))
{
p_next = q;
break;
}
if (*q == ';')
{
q++;
at |= 32;
}

switch (code)
{
default:

fg = nm_fg_color & 7;
bg = nm_bg_color & 7;
at &= 32;





if (at & 32)
{
fgi = 0;
bgi = 0;
} else
{
fgi = nm_fg_color & 8;
bgi = nm_bg_color & 8;
}
break;
case 1:
fgi = 8;
at |= 1;
break;
case 3:
case 7:
at |= 2;
break;
case 4:
bgi = 8;
at |= 4;
break;
case 5:
case 6:
bgi = 8;
at |= 8;
break;
case 8:
at |= 16;
break;
case 22:
fgi = 0;
at &= ~1;
break;
case 23:
case 27:
at &= ~2;
break;
case 24:
bgi = 0;
at &= ~4;
break;
case 28:
at &= ~16;
break;
case 30: case 31: case 32:
case 33: case 34: case 35:
case 36: case 37:
fg = screen_color[code - 30];
at |= 32;
break;
case 39:
fg = nm_fg_color & 7;
at |= 32;
break;
case 40: case 41: case 42:
case 43: case 44: case 45:
case 46: case 47:
bg = screen_color[code - 40];
at |= 32;
break;
case 49:
bg = nm_bg_color & 7;
at |= 32;
break;
}
p = q;
}
if (!is_ansi_end(*p) || p == p_next)
break;







if (sgr_mode || (at & 32))
{
if (at & 2)
{
f = bg | bgi;
b = fg | fgi;
} else
{
f = fg | fgi;
b = bg | bgi;
}
} else
{
if (at & 1)
{
f = bo_fg_color;
b = bo_bg_color;
} else if (at & 2)
{
f = so_fg_color;
b = so_bg_color;
} else if (at & 4)
{
f = ul_fg_color;
b = ul_bg_color;
} else if (at & 8)
{
f = bl_fg_color;
b = bl_bg_color;
} else
{
f = nm_fg_color;
b = nm_bg_color;
}
}
if (at & 16)
f = b ^ 8;
#if MSDOS_COMPILER==WIN32C
f &= 0xf | COMMON_LVB_UNDERSCORE;
#else
f &= 0xf;
#endif
b &= 0xf;
WIN32setcolors(f, b);
p_next = anchor = p + 1;
} else
p_next++;
}


WIN32textout(anchor, ob - anchor);
}
ob = obuf;
}
#endif

















public void
flush(VOID_PARAM)
{
int n;

n = (int) (ob - obuf);
if (n == 0)
return;
ob = obuf;

#if MSDOS_COMPILER==MSOFTC
if (interactive())
{
obuf[n] = '\0';
_outtext(obuf);
return;
}
#else
#if MSDOS_COMPILER==WIN32C || MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
if (interactive())
{
ob = obuf + n;
*ob = '\0';
win_flush();
return;
}
#endif
#endif

if (write(outfd, obuf, n) != n)
screen_trashed = 1;
}




public void
set_output(fd)
int fd;
{
flush();
outfd = fd;
}




public int
putchr(c)
int c;
{
#if 0
extern int utf_mode;
if (utf_mode)
{
static char ubuf[MAX_UTF_CHAR_LEN];
static int ubuf_len = 0;
static int ubuf_index = 0;
if (ubuf_len == 0)
{
ubuf_len = utf_len(c);
ubuf_index = 0;
}
ubuf[ubuf_index++] = c;
if (ubuf_index < ubuf_len)
return c;
c = get_wchar(ubuf) & 0xFF;
ubuf_len = 0;
}
#endif
clear_bot_if_needed();
#if MSDOS_COMPILER
if (c == '\n' && is_tty)
{

putchr('\r');
}
#else
#if defined(_OSK)
if (c == '\n' && is_tty)
putchr(0x0A);
#endif
#endif




if (ob >= &obuf[sizeof(obuf)-1])
flush();
*ob++ = c;
at_prompt = 0;
return (c);
}

public void
clear_bot_if_needed(VOID_PARAM)
{
if (!need_clr)
return;
need_clr = 0;
clear_bot();
}




public void
putstr(s)
constant char *s;
{
while (*s != '\0')
putchr(*s++);
}





#define TYPE_TO_A_FUNC(funcname, type) void funcname(num, buf) type num; char *buf; { int neg = (num < 0); char tbuf[INT_STRLEN_BOUND(num)+2]; char *s = tbuf + sizeof(tbuf); if (neg) num = -num; *--s = '\0'; do { *--s = (num % 10) + '0'; } while ((num /= 10) != 0); if (neg) *--s = '-'; strcpy(buf, s); }
















TYPE_TO_A_FUNC(postoa, POSITION)
TYPE_TO_A_FUNC(linenumtoa, LINENUM)
TYPE_TO_A_FUNC(inttoa, int)




#define STR_TO_TYPE_FUNC(funcname, type) type funcname(buf, ebuf) char *buf; char **ebuf; { type val = 0; for (;; buf++) { char c = *buf; if (c < '0' || c > '9') break; val = 10 * val + c - '0'; } if (ebuf != NULL) *ebuf = buf; return val; }














STR_TO_TYPE_FUNC(lstrtopos, POSITION)
STR_TO_TYPE_FUNC(lstrtoi, int)




static int
iprint_int(num)
int num;
{
char buf[INT_STRLEN_BOUND(num)];

inttoa(num, buf);
putstr(buf);
return ((int) strlen(buf));
}




static int
iprint_linenum(num)
LINENUM num;
{
char buf[INT_STRLEN_BOUND(num)];

linenumtoa(num, buf);
putstr(buf);
return ((int) strlen(buf));
}








public int
less_printf(fmt, parg)
char *fmt;
PARG *parg;
{
char *s;
int col;

col = 0;
while (*fmt != '\0')
{
if (*fmt != '%')
{
putchr(*fmt++);
col++;
} else
{
++fmt;
switch (*fmt++)
{
case 's':
s = parg->p_string;
parg++;
while (*s != '\0')
{
putchr(*s++);
col++;
}
break;
case 'd':
col += iprint_int(parg->p_int);
parg++;
break;
case 'n':
col += iprint_linenum(parg->p_linenum);
parg++;
break;
case 'c':
putchr(parg->p_char);
col++;
break;
case '%':
putchr('%');
break;
}
}
}
return (col);
}






public void
get_return(VOID_PARAM)
{
int c;

#if ONLY_RETURN
while ((c = getchr()) != '\n' && c != '\r')
bell();
#else
c = getchr();
if (c != '\n' && c != '\r' && c != ' ' && c != READ_INTR)
ungetcc(c);
#endif
}





public void
error(fmt, parg)
char *fmt;
PARG *parg;
{
int col = 0;
static char return_to_continue[] = " (press RETURN)";

errmsgs++;

if (!interactive())
{
less_printf(fmt, parg);
putchr('\n');
return;
}

if (!oldbot)
squish_check();
at_exit();
clear_bot();
at_enter(AT_STANDOUT|AT_COLOR_ERROR);
col += so_s_width;
col += less_printf(fmt, parg);
putstr(return_to_continue);
at_exit();
col += sizeof(return_to_continue) + so_e_width;

get_return();
lower_left();
clear_eol();

if (col >= sc_width)





screen_trashed = 1;

flush();
}

static char intr_to_abort[] = "... (interrupt to abort)";







public void
ierror(fmt, parg)
char *fmt;
PARG *parg;
{
at_exit();
clear_bot();
at_enter(AT_STANDOUT|AT_COLOR_ERROR);
(void) less_printf(fmt, parg);
putstr(intr_to_abort);
at_exit();
flush();
need_clr = 1;
}





public int
query(fmt, parg)
char *fmt;
PARG *parg;
{
int c;
int col = 0;

if (interactive())
clear_bot();

(void) less_printf(fmt, parg);
c = getchr();

if (interactive())
{
lower_left();
if (col >= sc_width)
screen_trashed = 1;
flush();
} else
{
putchr('\n');
}

if (c == 'Q')
quit(QUIT_OK);
return (c);
}
