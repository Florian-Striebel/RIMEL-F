














#include "less.h"
#include "cmd.h"

#if MSDOS_COMPILER
#include "pckeys.h"
#if MSDOS_COMPILER==MSOFTC
#include <graph.h>
#else
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
#include <conio.h>
#if MSDOS_COMPILER==DJGPPC
#include <pc.h>
extern int fd0;
#endif
#else
#if MSDOS_COMPILER==WIN32C
#include <windows.h>
#endif
#endif
#endif
#include <time.h>

#if !defined(FOREGROUND_BLUE)
#define FOREGROUND_BLUE 0x0001
#endif
#if !defined(FOREGROUND_GREEN)
#define FOREGROUND_GREEN 0x0002
#endif
#if !defined(FOREGROUND_RED)
#define FOREGROUND_RED 0x0004
#endif
#if !defined(FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSITY 0x0008
#endif

#else

#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if HAVE_TERMIOS_H && HAVE_TERMIOS_FUNCS
#include <termios.h>
#else
#if HAVE_TERMIO_H
#include <termio.h>
#else
#if HAVE_SGSTAT_H
#include <sgstat.h>
#else
#include <sgtty.h>
#endif
#endif
#endif

#if HAVE_TERMCAP_H
#include <termcap.h>
#endif
#if defined(_OSK)
#include <signal.h>
#endif
#if OS2
#include <sys/signal.h>
#include "pckeys.h"
#endif
#if HAVE_SYS_STREAM_H
#include <sys/stream.h>
#endif
#if HAVE_SYS_PTEM_H
#include <sys/ptem.h>
#endif

#endif





#if defined(__ultrix__)
#define MUST_SET_LINE_DISCIPLINE 1
#else
#define MUST_SET_LINE_DISCIPLINE 0
#endif

#if OS2
#define DEFAULT_TERM "ansi"
static char *windowid;
#else
#define DEFAULT_TERM "unknown"
#endif

#if MSDOS_COMPILER==MSOFTC
static int videopages;
static long msec_loops;
static int flash_created = 0;
#define SET_FG_COLOR(fg) _settextcolor(fg)
#define SET_BG_COLOR(bg) _setbkcolor(bg)
#define SETCOLORS(fg,bg) { SET_FG_COLOR(fg); SET_BG_COLOR(bg); }
#endif

#if MSDOS_COMPILER==BORLANDC
static unsigned short *whitescreen;
static int flash_created = 0;
#endif
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
#define _settextposition(y,x) gotoxy(x,y)
#define _clearscreen(m) clrscr()
#define _outtext(s) cputs(s)
#define SET_FG_COLOR(fg) textcolor(fg)
#define SET_BG_COLOR(bg) textbackground(bg)
#define SETCOLORS(fg,bg) { SET_FG_COLOR(fg); SET_BG_COLOR(bg); }
extern int sc_height;
#endif

#if MSDOS_COMPILER==WIN32C
struct keyRecord
{
int ascii;
int scan;
} currentKey;

static int keyCount = 0;
static WORD curr_attr;
static int pending_scancode = 0;
static char x11mousebuf[] = "[M???";
static int x11mousePos, x11mouseCount;

static HANDLE con_out_save = INVALID_HANDLE_VALUE;
static HANDLE con_out_ours = INVALID_HANDLE_VALUE;
HANDLE con_out = INVALID_HANDLE_VALUE;

extern int utf_mode;
extern int quitting;
static void win32_init_term();
static void win32_deinit_term();

#if !defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 4
#endif

#define FG_COLORS (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define BG_COLORS (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define MAKEATTR(fg,bg) ((WORD)((fg)|((bg)<<4)))
#define APPLY_COLORS() { if (SetConsoleTextAttribute(con_out, curr_attr) == 0) error("SETCOLORS failed", NULL_PARG); }

#define SET_FG_COLOR(fg) { curr_attr |= (fg); APPLY_COLORS(); }
#define SET_BG_COLOR(bg) { curr_attr |= ((bg)<<4); APPLY_COLORS(); }
#define SETCOLORS(fg,bg) { curr_attr = MAKEATTR(fg,bg); APPLY_COLORS(); }
#endif

#if MSDOS_COMPILER
public int nm_fg_color;
public int nm_bg_color;
public int bo_fg_color;
public int bo_bg_color;
public int ul_fg_color;
public int ul_bg_color;
public int so_fg_color;
public int so_bg_color;
public int bl_fg_color;
public int bl_bg_color;
static int sy_fg_color;
static int sy_bg_color;
public int sgr_mode;
#if MSDOS_COMPILER==WIN32C
static DWORD init_output_mode;
public int vt_enabled = -1;
#endif
#else




static char
*sc_pad,
*sc_home,
*sc_addline,
*sc_lower_left,
*sc_return,
*sc_move,
*sc_clear,
*sc_eol_clear,
*sc_eos_clear,
*sc_s_in,
*sc_s_out,
*sc_u_in,
*sc_u_out,
*sc_b_in,
*sc_b_out,
*sc_bl_in,
*sc_bl_out,
*sc_visual_bell,
*sc_backspace,
*sc_s_keypad,
*sc_e_keypad,
*sc_s_mousecap,
*sc_e_mousecap,
*sc_init,
*sc_deinit;

static int attrcolor = -1;
#endif

static int init_done = 0;

public int auto_wrap;
public int ignaw;
public int erase_char;
public int erase2_char;
public int kill_char;
public int werase_char;
public int sc_width, sc_height;
public int bo_s_width, bo_e_width;
public int ul_s_width, ul_e_width;
public int so_s_width, so_e_width;
public int bl_s_width, bl_e_width;
public int above_mem, below_mem;
public int can_goto_line;
public int clear_bg;
public int missing_cap = 0;
public char *kent = NULL;

static int attrmode = AT_NORMAL;
static int termcap_debug = -1;
extern int binattr;
extern int one_screen;
#if LESSTEST
extern char *ttyin_name;
#endif

#if !MSDOS_COMPILER
static char *cheaper LESSPARAMS((char *t1, char *t2, char *def));
static void tmodes LESSPARAMS((char *incap, char *outcap, char **instr,
char **outstr, char *def_instr, char *def_outstr, char **spp));
#endif





#if MUST_DEFINE_OSPEED
extern short ospeed;
extern char PC;
#endif
#if defined(_OSK)
short ospeed;
char PC_, *UP, *BC;
#endif

extern int quiet;
extern int no_back_scroll;
extern int swindow;
extern int no_init;
extern int no_keypad;
extern int sigs;
extern int wscroll;
extern int screen_trashed;
extern int top_scroll;
extern int quit_if_one_screen;
extern int oldbot;
extern int mousecap;
extern int is_tty;
extern int use_color;
#if HILITE_SEARCH
extern int hilite_search;
#endif
#if MSDOS_COMPILER==WIN32C
extern HANDLE tty;
extern DWORD console_mode;
#if !defined(ENABLE_EXTENDED_FLAGS)
#define ENABLE_EXTENDED_FLAGS 0x80
#define ENABLE_QUICK_EDIT_MODE 0x40
#endif
#else
extern int tty;
#endif

extern char *tgetstr();
extern char *tgoto();













public void
raw_mode(on)
int on;
{
static int curr_on = 0;

if (on == curr_on)
return;
erase2_char = '\b';
#if HAVE_TERMIOS_H && HAVE_TERMIOS_FUNCS
{
struct termios s;
static struct termios save_term;
static int saved_term = 0;

if (on)
{



tcgetattr(tty, &s);




if (!saved_term)
{
save_term = s;
saved_term = 1;
}
#if HAVE_OSPEED
switch (cfgetospeed(&s))
{
#if defined(B0)
case B0: ospeed = 0; break;
#endif
#if defined(B50)
case B50: ospeed = 1; break;
#endif
#if defined(B75)
case B75: ospeed = 2; break;
#endif
#if defined(B110)
case B110: ospeed = 3; break;
#endif
#if defined(B134)
case B134: ospeed = 4; break;
#endif
#if defined(B150)
case B150: ospeed = 5; break;
#endif
#if defined(B200)
case B200: ospeed = 6; break;
#endif
#if defined(B300)
case B300: ospeed = 7; break;
#endif
#if defined(B600)
case B600: ospeed = 8; break;
#endif
#if defined(B1200)
case B1200: ospeed = 9; break;
#endif
#if defined(B1800)
case B1800: ospeed = 10; break;
#endif
#if defined(B2400)
case B2400: ospeed = 11; break;
#endif
#if defined(B4800)
case B4800: ospeed = 12; break;
#endif
#if defined(B9600)
case B9600: ospeed = 13; break;
#endif
#if defined(EXTA)
case EXTA: ospeed = 14; break;
#endif
#if defined(EXTB)
case EXTB: ospeed = 15; break;
#endif
#if defined(B57600)
case B57600: ospeed = 16; break;
#endif
#if defined(B115200)
case B115200: ospeed = 17; break;
#endif
default: ;
}
#endif
erase_char = s.c_cc[VERASE];
#if defined(VERASE2)
erase2_char = s.c_cc[VERASE2];
#endif
kill_char = s.c_cc[VKILL];
#if defined(VWERASE)
werase_char = s.c_cc[VWERASE];
#else
werase_char = CONTROL('W');
#endif




s.c_lflag &= ~(0
#if defined(ICANON)
| ICANON
#endif
#if defined(ECHO)
| ECHO
#endif
#if defined(ECHOE)
| ECHOE
#endif
#if defined(ECHOK)
| ECHOK
#endif
#if ECHONL
| ECHONL
#endif
);

s.c_oflag |= (0
#if defined(OXTABS)
| OXTABS
#else
#if defined(TAB3)
| TAB3
#else
#if defined(XTABS)
| XTABS
#endif
#endif
#endif
#if defined(OPOST)
| OPOST
#endif
#if defined(ONLCR)
| ONLCR
#endif
);

s.c_oflag &= ~(0
#if defined(ONOEOT)
| ONOEOT
#endif
#if defined(OCRNL)
| OCRNL
#endif
#if defined(ONOCR)
| ONOCR
#endif
#if defined(ONLRET)
| ONLRET
#endif
);
s.c_cc[VMIN] = 1;
s.c_cc[VTIME] = 0;
#if defined(VLNEXT)
s.c_cc[VLNEXT] = 0;
#endif
#if defined(VDSUSP)
s.c_cc[VDSUSP] = 0;
#endif
#if MUST_SET_LINE_DISCIPLINE




s.c_line = TERMIODISC;
#endif
} else
{



s = save_term;
}
#if HAVE_FSYNC
fsync(tty);
#endif
tcsetattr(tty, TCSADRAIN, &s);
#if MUST_SET_LINE_DISCIPLINE
if (!on)
{






ioctl(tty, TIOCSETD, &save_term.c_line);
}
#endif
}
#else
#if defined(TCGETA)
{
struct termio s;
static struct termio save_term;
static int saved_term = 0;

if (on)
{



ioctl(tty, TCGETA, &s);




if (!saved_term)
{
save_term = s;
saved_term = 1;
}
#if HAVE_OSPEED
ospeed = s.c_cflag & CBAUD;
#endif
erase_char = s.c_cc[VERASE];
kill_char = s.c_cc[VKILL];
#if defined(VWERASE)
werase_char = s.c_cc[VWERASE];
#else
werase_char = CONTROL('W');
#endif




s.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL);
s.c_oflag |= (OPOST|ONLCR|TAB3);
s.c_oflag &= ~(OCRNL|ONOCR|ONLRET);
s.c_cc[VMIN] = 1;
s.c_cc[VTIME] = 0;
} else
{



s = save_term;
}
ioctl(tty, TCSETAW, &s);
}
#else
#if defined(TIOCGETP)
{
struct sgttyb s;
static struct sgttyb save_term;
static int saved_term = 0;

if (on)
{



ioctl(tty, TIOCGETP, &s);




if (!saved_term)
{
save_term = s;
saved_term = 1;
}
#if HAVE_OSPEED
ospeed = s.sg_ospeed;
#endif
erase_char = s.sg_erase;
kill_char = s.sg_kill;
werase_char = CONTROL('W');




s.sg_flags |= CBREAK;
s.sg_flags &= ~(ECHO|XTABS);
} else
{



s = save_term;
}
ioctl(tty, TIOCSETN, &s);
}
#else
#if defined(_OSK)
{
struct sgbuf s;
static struct sgbuf save_term;
static int saved_term = 0;

if (on)
{



_gs_opt(tty, &s);




if (!saved_term)
{
save_term = s;
saved_term = 1;
}
erase_char = s.sg_bspch;
kill_char = s.sg_dlnch;
werase_char = CONTROL('W');




s.sg_echo = 0;
s.sg_eofch = 0;
s.sg_pause = 0;
s.sg_psch = 0;
} else
{



s = save_term;
}
_ss_opt(tty, &s);
}
#else

#if OS2

LSIGNAL(SIGINT, SIG_IGN);
#endif
erase_char = '\b';
#if MSDOS_COMPILER==DJGPPC
kill_char = CONTROL('U');







if (fd0 != 0)
setmode(0, on ? O_BINARY : O_TEXT);
#else
kill_char = ESC;
#endif
werase_char = CONTROL('W');
#endif
#endif
#endif
#endif
curr_on = on;
}

#if !MSDOS_COMPILER



static int hardcopy;

static char *
ltget_env(capname)
char *capname;
{
char name[64];

if (termcap_debug)
{
struct env { struct env *next; char *name; char *value; };
static struct env *envs = NULL;
struct env *p;
for (p = envs; p != NULL; p = p->next)
if (strcmp(p->name, capname) == 0)
return p->value;
p = (struct env *) ecalloc(1, sizeof(struct env));
p->name = save(capname);
p->value = (char *) ecalloc(strlen(capname)+3, sizeof(char));
sprintf(p->value, "<%s>", capname);
p->next = envs;
envs = p;
return p->value;
}
SNPRINTF1(name, sizeof(name), "LESS_TERMCAP_%s", capname);
return (lgetenv(name));
}

static int
ltgetflag(capname)
char *capname;
{
char *s;

if ((s = ltget_env(capname)) != NULL)
return (*s != '\0' && *s != '0');
if (hardcopy)
return (0);
return (tgetflag(capname));
}

static int
ltgetnum(capname)
char *capname;
{
char *s;

if ((s = ltget_env(capname)) != NULL)
return (atoi(s));
if (hardcopy)
return (-1);
return (tgetnum(capname));
}

static char *
ltgetstr(capname, pp)
char *capname;
char **pp;
{
char *s;

if ((s = ltget_env(capname)) != NULL)
return (s);
if (hardcopy)
return (NULL);
return (tgetstr(capname, pp));
}
#endif




public void
scrsize(VOID_PARAM)
{
char *s;
int sys_height;
int sys_width;
#if !MSDOS_COMPILER
int n;
#endif

#define DEF_SC_WIDTH 80
#if MSDOS_COMPILER
#define DEF_SC_HEIGHT 25
#else
#define DEF_SC_HEIGHT 24
#endif


sys_width = sys_height = 0;

#if MSDOS_COMPILER==MSOFTC
{
struct videoconfig w;
_getvideoconfig(&w);
sys_height = w.numtextrows;
sys_width = w.numtextcols;
}
#else
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
{
struct text_info w;
gettextinfo(&w);
sys_height = w.screenheight;
sys_width = w.screenwidth;
}
#else
#if MSDOS_COMPILER==WIN32C
{
CONSOLE_SCREEN_BUFFER_INFO scr;
GetConsoleScreenBufferInfo(con_out, &scr);
sys_height = scr.srWindow.Bottom - scr.srWindow.Top + 1;
sys_width = scr.srWindow.Right - scr.srWindow.Left + 1;
}
#else
#if OS2
{
int s[2];
_scrsize(s);
sys_width = s[0];
sys_height = s[1];





windowid = getenv("WINDOWID");
if (windowid != NULL)
{
FILE *fd = popen("scrsize", "rt");
if (fd != NULL)
{
int w, h;
fscanf(fd, "%i %i", &w, &h);
if (w > 0 && h > 0)
{
sys_width = w;
sys_height = h;
}
pclose(fd);
}
}
}
#else
#if defined(TIOCGWINSZ)
{
struct winsize w;
if (ioctl(2, TIOCGWINSZ, &w) == 0)
{
if (w.ws_row > 0)
sys_height = w.ws_row;
if (w.ws_col > 0)
sys_width = w.ws_col;
}
}
#else
#if defined(WIOCGETD)
{
struct uwdata w;
if (ioctl(2, WIOCGETD, &w) == 0)
{
if (w.uw_height > 0)
sys_height = w.uw_height / w.uw_vs;
if (w.uw_width > 0)
sys_width = w.uw_width / w.uw_hs;
}
}
#endif
#endif
#endif
#endif
#endif
#endif

if (sys_height > 0)
sc_height = sys_height;
else if ((s = lgetenv("LINES")) != NULL)
sc_height = atoi(s);
#if !MSDOS_COMPILER
else if ((n = ltgetnum("li")) > 0)
sc_height = n;
#endif
if (sc_height <= 0)
sc_height = DEF_SC_HEIGHT;

if (sys_width > 0)
sc_width = sys_width;
else if ((s = lgetenv("COLUMNS")) != NULL)
sc_width = atoi(s);
#if !MSDOS_COMPILER
else if ((n = ltgetnum("co")) > 0)
sc_width = n;
#endif
if (sc_width <= 0)
sc_width = DEF_SC_WIDTH;
}

#if MSDOS_COMPILER==MSOFTC



static void
get_clock(VOID_PARAM)
{
clock_t start;




start = clock();
while (clock() == start)
;



start = clock();
msec_loops = 0;
while (clock() == start)
msec_loops++;



msec_loops *= CLOCKS_PER_SEC;
msec_loops /= 1000;
}




static void
delay(msec)
int msec;
{
long i;

while (msec-- > 0)
{
for (i = 0; i < msec_loops; i++)
(void) clock();
}
}
#endif




public char *
special_key_str(key)
int key;
{
static char tbuf[40];
char *s;
#if MSDOS_COMPILER || OS2
static char k_right[] = { '\340', PCK_RIGHT, 0 };
static char k_left[] = { '\340', PCK_LEFT, 0 };
static char k_ctl_right[] = { '\340', PCK_CTL_RIGHT, 0 };
static char k_ctl_left[] = { '\340', PCK_CTL_LEFT, 0 };
static char k_insert[] = { '\340', PCK_INSERT, 0 };
static char k_delete[] = { '\340', PCK_DELETE, 0 };
static char k_ctl_delete[] = { '\340', PCK_CTL_DELETE, 0 };
static char k_ctl_backspace[] = { '\177', 0 };
static char k_home[] = { '\340', PCK_HOME, 0 };
static char k_end[] = { '\340', PCK_END, 0 };
static char k_up[] = { '\340', PCK_UP, 0 };
static char k_down[] = { '\340', PCK_DOWN, 0 };
static char k_backtab[] = { '\340', PCK_SHIFT_TAB, 0 };
static char k_pagedown[] = { '\340', PCK_PAGEDOWN, 0 };
static char k_pageup[] = { '\340', PCK_PAGEUP, 0 };
static char k_f1[] = { '\340', PCK_F1, 0 };
#endif
#if !MSDOS_COMPILER
char *sp = tbuf;
#endif

switch (key)
{
#if OS2




case SK_RIGHT_ARROW:
s = windowid ? ltgetstr("kr", &sp) : k_right;
break;
case SK_LEFT_ARROW:
s = windowid ? ltgetstr("kl", &sp) : k_left;
break;
case SK_UP_ARROW:
s = windowid ? ltgetstr("ku", &sp) : k_up;
break;
case SK_DOWN_ARROW:
s = windowid ? ltgetstr("kd", &sp) : k_down;
break;
case SK_PAGE_UP:
s = windowid ? ltgetstr("kP", &sp) : k_pageup;
break;
case SK_PAGE_DOWN:
s = windowid ? ltgetstr("kN", &sp) : k_pagedown;
break;
case SK_HOME:
s = windowid ? ltgetstr("kh", &sp) : k_home;
break;
case SK_END:
s = windowid ? ltgetstr("@7", &sp) : k_end;
break;
case SK_DELETE:
s = windowid ? ltgetstr("kD", &sp) : k_delete;
if (s == NULL)
{
tbuf[0] = '\177';
tbuf[1] = '\0';
s = tbuf;
}
break;
#endif
#if MSDOS_COMPILER
case SK_RIGHT_ARROW:
s = k_right;
break;
case SK_LEFT_ARROW:
s = k_left;
break;
case SK_UP_ARROW:
s = k_up;
break;
case SK_DOWN_ARROW:
s = k_down;
break;
case SK_PAGE_UP:
s = k_pageup;
break;
case SK_PAGE_DOWN:
s = k_pagedown;
break;
case SK_HOME:
s = k_home;
break;
case SK_END:
s = k_end;
break;
case SK_DELETE:
s = k_delete;
break;
#endif
#if MSDOS_COMPILER || OS2
case SK_INSERT:
s = k_insert;
break;
case SK_CTL_LEFT_ARROW:
s = k_ctl_left;
break;
case SK_CTL_RIGHT_ARROW:
s = k_ctl_right;
break;
case SK_CTL_BACKSPACE:
s = k_ctl_backspace;
break;
case SK_CTL_DELETE:
s = k_ctl_delete;
break;
case SK_F1:
s = k_f1;
break;
case SK_BACKTAB:
s = k_backtab;
break;
#else
case SK_RIGHT_ARROW:
s = ltgetstr("kr", &sp);
break;
case SK_LEFT_ARROW:
s = ltgetstr("kl", &sp);
break;
case SK_UP_ARROW:
s = ltgetstr("ku", &sp);
break;
case SK_DOWN_ARROW:
s = ltgetstr("kd", &sp);
break;
case SK_PAGE_UP:
s = ltgetstr("kP", &sp);
break;
case SK_PAGE_DOWN:
s = ltgetstr("kN", &sp);
break;
case SK_HOME:
s = ltgetstr("kh", &sp);
break;
case SK_END:
s = ltgetstr("@7", &sp);
break;
case SK_DELETE:
s = ltgetstr("kD", &sp);
if (s == NULL)
{
tbuf[0] = '\177';
tbuf[1] = '\0';
s = tbuf;
}
break;
#endif
case SK_CONTROL_K:
tbuf[0] = CONTROL('K');
tbuf[1] = '\0';
s = tbuf;
break;
default:
return (NULL);
}
return (s);
}




public void
get_term(VOID_PARAM)
{
termcap_debug = !isnullenv(lgetenv("LESS_TERMCAP_DEBUG"));
#if MSDOS_COMPILER
auto_wrap = 1;
ignaw = 0;
can_goto_line = 1;
clear_bg = 1;




#if MSDOS_COMPILER==MSOFTC
sy_bg_color = _getbkcolor();
sy_fg_color = _gettextcolor();
get_clock();
#else
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
{
struct text_info w;
gettextinfo(&w);
sy_bg_color = (w.attribute >> 4) & 0x0F;
sy_fg_color = (w.attribute >> 0) & 0x0F;
}
#else
#if MSDOS_COMPILER==WIN32C
{
CONSOLE_SCREEN_BUFFER_INFO scr;

con_out_save = con_out = GetStdHandle(STD_OUTPUT_HANDLE);




SET_BINARY(0);
GetConsoleMode(con_out, &init_output_mode);
GetConsoleScreenBufferInfo(con_out, &scr);
curr_attr = scr.wAttributes;
sy_bg_color = (curr_attr & BG_COLORS) >> 4;
sy_fg_color = curr_attr & FG_COLORS;
}
#endif
#endif
#endif
nm_fg_color = sy_fg_color;
nm_bg_color = sy_bg_color;
bo_fg_color = 11;
bo_bg_color = 0;
ul_fg_color = 9;
ul_bg_color = 0;
so_fg_color = 15;
so_bg_color = 9;
bl_fg_color = 15;
bl_bg_color = 0;
sgr_mode = 0;




scrsize();
pos_init();


#else
{
char *sp;
char *t1, *t2;
char *term;




static char termbuf[TERMBUF_SIZE];
static char sbuf[TERMSBUF_SIZE];

#if OS2



sp = lgetenv("TERMCAP");
if (isnullenv(sp))
{
char *termcap;
if ((sp = homefile("termcap.dat")) != NULL)
{
termcap = (char *) ecalloc(strlen(sp)+9, sizeof(char));
sprintf(termcap, "TERMCAP=%s", sp);
free(sp);
putenv(termcap);
}
}
#endif



if ((term = lgetenv("TERM")) == NULL)
term = DEFAULT_TERM;
hardcopy = 0;

if (tgetent(termbuf, term) != TGETENT_OK)
hardcopy = 1;
if (ltgetflag("hc"))
hardcopy = 1;




scrsize();
pos_init();

auto_wrap = ltgetflag("am");
ignaw = ltgetflag("xn");
above_mem = ltgetflag("da");
below_mem = ltgetflag("db");
clear_bg = ltgetflag("ut");







if ((so_s_width = ltgetnum("sg")) < 0)
so_s_width = 0;
so_e_width = so_s_width;

bo_s_width = bo_e_width = so_s_width;
ul_s_width = ul_e_width = so_s_width;
bl_s_width = bl_e_width = so_s_width;

#if HILITE_SEARCH
if (so_s_width > 0 || so_e_width > 0)







hilite_search = 0;
#endif




sp = sbuf;

#if HAVE_OSPEED
sc_pad = ltgetstr("pc", &sp);
if (sc_pad != NULL)
PC = *sc_pad;
#endif

sc_s_keypad = ltgetstr("ks", &sp);
if (sc_s_keypad == NULL)
sc_s_keypad = "";
sc_e_keypad = ltgetstr("ke", &sp);
if (sc_e_keypad == NULL)
sc_e_keypad = "";
kent = ltgetstr("@8", &sp);

sc_s_mousecap = ltgetstr("MOUSE_START", &sp);
if (sc_s_mousecap == NULL)
sc_s_mousecap = ESCS "[?1000h" ESCS "[?1006h";
sc_e_mousecap = ltgetstr("MOUSE_END", &sp);
if (sc_e_mousecap == NULL)
sc_e_mousecap = ESCS "[?1006l" ESCS "[?1000l";

sc_init = ltgetstr("ti", &sp);
if (sc_init == NULL)
sc_init = "";

sc_deinit= ltgetstr("te", &sp);
if (sc_deinit == NULL)
sc_deinit = "";

sc_eol_clear = ltgetstr("ce", &sp);
if (sc_eol_clear == NULL || *sc_eol_clear == '\0')
{
missing_cap = 1;
sc_eol_clear = "";
}

sc_eos_clear = ltgetstr("cd", &sp);
if (below_mem && (sc_eos_clear == NULL || *sc_eos_clear == '\0'))
{
missing_cap = 1;
sc_eos_clear = "";
}

sc_clear = ltgetstr("cl", &sp);
if (sc_clear == NULL || *sc_clear == '\0')
{
missing_cap = 1;
sc_clear = "\n\n";
}

sc_move = ltgetstr("cm", &sp);
if (sc_move == NULL || *sc_move == '\0')
{





sc_move = "";
can_goto_line = 0;
} else
can_goto_line = 1;

tmodes("so", "se", &sc_s_in, &sc_s_out, "", "", &sp);
tmodes("us", "ue", &sc_u_in, &sc_u_out, sc_s_in, sc_s_out, &sp);
tmodes("md", "me", &sc_b_in, &sc_b_out, sc_s_in, sc_s_out, &sp);
tmodes("mb", "me", &sc_bl_in, &sc_bl_out, sc_s_in, sc_s_out, &sp);

sc_visual_bell = ltgetstr("vb", &sp);
if (sc_visual_bell == NULL)
sc_visual_bell = "";

if (ltgetflag("bs"))
sc_backspace = "\b";
else
{
sc_backspace = ltgetstr("bc", &sp);
if (sc_backspace == NULL || *sc_backspace == '\0')
sc_backspace = "\b";
}





t1 = ltgetstr("ho", &sp);
if (t1 == NULL)
t1 = "";
if (*sc_move == '\0')
t2 = "";
else
{
strcpy(sp, tgoto(sc_move, 0, 0));
t2 = sp;
sp += strlen(sp) + 1;
}
sc_home = cheaper(t1, t2, "|\b^");





t1 = ltgetstr("ll", &sp);
if (t1 == NULL)
t1 = "";
if (*sc_move == '\0')
t2 = "";
else
{
strcpy(sp, tgoto(sc_move, 0, sc_height-1));
t2 = sp;
sp += strlen(sp) + 1;
}
sc_lower_left = cheaper(t1, t2, "\r");




sc_return = ltgetstr("cr", &sp);
if (sc_return == NULL)
sc_return = "\r";





t1 = ltgetstr("al", &sp);
if (t1 == NULL)
t1 = "";
t2 = ltgetstr("sr", &sp);
if (t2 == NULL)
t2 = "";
#if OS2
if (*t1 == '\0' && *t2 == '\0')
sc_addline = "";
else
#endif
if (above_mem)
sc_addline = t1;
else
sc_addline = cheaper(t1, t2, "");
if (*sc_addline == '\0')
{



no_back_scroll = 1;
}
}
#endif
}

#if !MSDOS_COMPILER







static int costcount;


static int
inc_costcount(c)
int c;
{
costcount++;
return (c);
}

static int
cost(t)
char *t;
{
costcount = 0;
tputs(t, sc_height, inc_costcount);
return (costcount);
}






static char *
cheaper(t1, t2, def)
char *t1, *t2;
char *def;
{
if (*t1 == '\0' && *t2 == '\0')
{
missing_cap = 1;
return (def);
}
if (*t1 == '\0')
return (t2);
if (*t2 == '\0')
return (t1);
if (cost(t1) < cost(t2))
return (t1);
return (t2);
}

static void
tmodes(incap, outcap, instr, outstr, def_instr, def_outstr, spp)
char *incap;
char *outcap;
char **instr;
char **outstr;
char *def_instr;
char *def_outstr;
char **spp;
{
*instr = ltgetstr(incap, spp);
if (*instr == NULL)
{

*instr = def_instr;
*outstr = def_outstr;
return;
}

*outstr = ltgetstr(outcap, spp);
if (*outstr == NULL)

*outstr = ltgetstr("me", spp);
if (*outstr == NULL)

*outstr = "";
}

#endif








#if MSDOS_COMPILER

#if MSDOS_COMPILER==WIN32C
static void
_settextposition(int row, int col)
{
COORD cpos;
CONSOLE_SCREEN_BUFFER_INFO csbi;

GetConsoleScreenBufferInfo(con_out, &csbi);
cpos.X = csbi.srWindow.Left + (col - 1);
cpos.Y = csbi.srWindow.Top + (row - 1);
SetConsoleCursorPosition(con_out, cpos);
}
#endif




static void
initcolor(VOID_PARAM)
{
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
intensevideo();
#endif
SETCOLORS(nm_fg_color, nm_bg_color);
#if 0




char *blanks;
int row;
int col;




SETCOLORS(nm_fg_color, nm_bg_color);
blanks = (char *) ecalloc(width+1, sizeof(char));
for (col = 0; col < sc_width; col++)
blanks[col] = ' ';
blanks[sc_width] = '\0';
for (row = 0; row < sc_height; row++)
_outtext(blanks);
free(blanks);
#endif
}
#endif

#if MSDOS_COMPILER==WIN32C




static void
win32_init_vt_term(VOID_PARAM)
{
DWORD output_mode;

if (vt_enabled == 0 || (vt_enabled == 1 && con_out == con_out_ours))
return;

GetConsoleMode(con_out, &output_mode);
vt_enabled = SetConsoleMode(con_out,
output_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
if (vt_enabled)
{
auto_wrap = 0;
ignaw = 1;
}
}

static void
win32_deinit_vt_term(VOID_PARAM)
{
if (vt_enabled == 1 && con_out == con_out_save)
SetConsoleMode(con_out, init_output_mode);
}




static void
win32_init_term(VOID_PARAM)
{
CONSOLE_SCREEN_BUFFER_INFO scr;
COORD size;

if (con_out_save == INVALID_HANDLE_VALUE)
return;

GetConsoleScreenBufferInfo(con_out_save, &scr);

if (con_out_ours == INVALID_HANDLE_VALUE)
{




con_out_ours = CreateConsoleScreenBuffer(
GENERIC_WRITE | GENERIC_READ,
FILE_SHARE_WRITE | FILE_SHARE_READ,
(LPSECURITY_ATTRIBUTES) NULL,
CONSOLE_TEXTMODE_BUFFER,
(LPVOID) NULL);
}

size.X = scr.srWindow.Right - scr.srWindow.Left + 1;
size.Y = scr.srWindow.Bottom - scr.srWindow.Top + 1;
SetConsoleScreenBufferSize(con_out_ours, size);
SetConsoleActiveScreenBuffer(con_out_ours);
con_out = con_out_ours;
}




static void
win32_deinit_term(VOID_PARAM)
{
if (con_out_save == INVALID_HANDLE_VALUE)
return;
if (quitting)
(void) CloseHandle(con_out_ours);
SetConsoleActiveScreenBuffer(con_out_save);
con_out = con_out_save;
}

#endif

#if !MSDOS_COMPILER
static void
do_tputs(str, affcnt, f_putc)
char *str;
int affcnt;
int (*f_putc)(int);
{
#if LESSTEST
if (ttyin_name != NULL)
putstr(str);
else
#endif
tputs(str, affcnt, f_putc);
}





static void
ltputs(str, affcnt, f_putc)
char *str;
int affcnt;
int (*f_putc)(int);
{
while (str != NULL && *str != '\0')
{
#if HAVE_STRSTR
char *obrac = strstr(str, "$<");
if (obrac != NULL)
{
char str2[64];
int slen = obrac - str;
if (slen < sizeof(str2))
{
int delay;

memcpy(str2, str, slen);
str2[slen] = '\0';
do_tputs(str2, affcnt, f_putc);
str += slen + 2;

delay = lstrtoi(str, &str);
if (*str == '*')
delay *= affcnt;
flush();
sleep_ms(delay);

str = strstr(str, ">");
if (str != NULL)
str++;
continue;
}
}
#endif

do_tputs(str, affcnt, f_putc);
break;
}
}
#endif





public void
init_mouse(VOID_PARAM)
{
#if !MSDOS_COMPILER
ltputs(sc_s_mousecap, sc_height, putchr);
#else
#if MSDOS_COMPILER==WIN32C
SetConsoleMode(tty, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT
| ENABLE_EXTENDED_FLAGS );

#endif
#endif
}





public void
deinit_mouse(VOID_PARAM)
{
#if !MSDOS_COMPILER
ltputs(sc_e_mousecap, sc_height, putchr);
#else
#if MSDOS_COMPILER==WIN32C
SetConsoleMode(tty, ENABLE_PROCESSED_INPUT | ENABLE_EXTENDED_FLAGS
| (console_mode & ENABLE_QUICK_EDIT_MODE));
#endif
#endif
}




public void
init(VOID_PARAM)
{
clear_bot_if_needed();
#if !MSDOS_COMPILER
if (!(quit_if_one_screen && one_screen))
{
if (!no_init)
ltputs(sc_init, sc_height, putchr);
if (!no_keypad)
ltputs(sc_s_keypad, sc_height, putchr);
if (mousecap)
init_mouse();
}
init_done = 1;
if (top_scroll)
{
int i;







for (i = 1; i < sc_height; i++)
putchr('\n');
} else
line_left();
#else
#if MSDOS_COMPILER==WIN32C
if (!(quit_if_one_screen && one_screen))
{
if (!no_init)
win32_init_term();
if (mousecap)
init_mouse();

}
win32_init_vt_term();
#endif
init_done = 1;
initcolor();
flush();
#endif
}




public void
deinit(VOID_PARAM)
{
if (!init_done)
return;
#if !MSDOS_COMPILER
if (!(quit_if_one_screen && one_screen))
{
if (mousecap)
deinit_mouse();
if (!no_keypad)
ltputs(sc_e_keypad, sc_height, putchr);
if (!no_init)
ltputs(sc_deinit, sc_height, putchr);
}
#else

SETCOLORS(sy_fg_color, sy_bg_color);
#if MSDOS_COMPILER==WIN32C
win32_deinit_vt_term();
if (!(quit_if_one_screen && one_screen))
{
if (mousecap)
deinit_mouse();
if (!no_init)
win32_deinit_term();
}
#else

clreol();
#endif
#endif
init_done = 0;
}




public int
interactive(VOID_PARAM)
{
return (is_tty && init_done);
}

static void
assert_interactive(VOID_PARAM)
{
if (interactive()) return;

}




public void
home(VOID_PARAM)
{
assert_interactive();
#if !MSDOS_COMPILER
ltputs(sc_home, 1, putchr);
#else
flush();
_settextposition(1,1);
#endif
}





public void
add_line(VOID_PARAM)
{
assert_interactive();
#if !MSDOS_COMPILER
ltputs(sc_addline, sc_height, putchr);
#else
flush();
#if MSDOS_COMPILER==MSOFTC
_scrolltextwindow(_GSCROLLDOWN);
_settextposition(1,1);
#else
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
movetext(1,1, sc_width,sc_height-1, 1,2);
gotoxy(1,1);
clreol();
#else
#if MSDOS_COMPILER==WIN32C
{
CHAR_INFO fillchar;
SMALL_RECT rcSrc, rcClip;
COORD new_org;
CONSOLE_SCREEN_BUFFER_INFO csbi;

GetConsoleScreenBufferInfo(con_out,&csbi);


rcClip.Left = csbi.srWindow.Left;
rcClip.Top = csbi.srWindow.Top;
rcClip.Right = csbi.srWindow.Right;
rcClip.Bottom = csbi.srWindow.Bottom;


rcSrc = rcClip;
rcSrc.Bottom--;


new_org.X = rcSrc.Left;
new_org.Y = rcSrc.Top + 1;


fillchar.Char.AsciiChar = ' ';
curr_attr = MAKEATTR(nm_fg_color, nm_bg_color);
fillchar.Attributes = curr_attr;
ScrollConsoleScreenBuffer(con_out, &rcSrc, &rcClip, new_org, &fillchar);
_settextposition(1,1);
}
#endif
#endif
#endif
#endif
}

#if 0





public void
remove_top(n)
int n;
{
#if MSDOS_COMPILER==WIN32C
SMALL_RECT rcSrc, rcClip;
CHAR_INFO fillchar;
COORD new_org;
CONSOLE_SCREEN_BUFFER_INFO csbi;

if (n >= sc_height - 1)
{
clear();
home();
return;
}

flush();

GetConsoleScreenBufferInfo(con_out, &csbi);


rcSrc.Left = csbi.srWindow.Left;
rcSrc.Top = csbi.srWindow.Top + n;
rcSrc.Right = csbi.srWindow.Right;
rcSrc.Bottom = csbi.srWindow.Bottom;


rcClip.Left = rcSrc.Left;
rcClip.Top = csbi.srWindow.Top;
rcClip.Right = rcSrc.Right;
rcClip.Bottom = rcSrc.Bottom ;


new_org.X = rcSrc.Left;
new_org.Y = rcSrc.Top - n;


fillchar.Char.AsciiChar = ' ';
curr_attr = MAKEATTR(nm_fg_color, nm_bg_color);
fillchar.Attributes = curr_attr;

ScrollConsoleScreenBuffer(con_out, &rcSrc, &rcClip, new_org, &fillchar);


goto_line(sc_height - n - 1);
#endif
}
#endif

#if MSDOS_COMPILER==WIN32C



static void
win32_clear(VOID_PARAM)
{








COORD topleft;
DWORD nchars;
DWORD winsz;
CONSOLE_SCREEN_BUFFER_INFO csbi;


GetConsoleScreenBufferInfo(con_out, &csbi);
winsz = csbi.dwSize.X * (csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
topleft.X = 0;
topleft.Y = csbi.srWindow.Top;

curr_attr = MAKEATTR(nm_fg_color, nm_bg_color);
FillConsoleOutputCharacter(con_out, ' ', winsz, topleft, &nchars);
FillConsoleOutputAttribute(con_out, curr_attr, winsz, topleft, &nchars);
}





public void
win32_scroll_up(n)
int n;
{
SMALL_RECT rcSrc, rcClip;
CHAR_INFO fillchar;
COORD topleft;
COORD new_org;
DWORD nchars;
DWORD size;
CONSOLE_SCREEN_BUFFER_INFO csbi;

if (n <= 0)
return;

if (n >= sc_height - 1)
{
win32_clear();
_settextposition(1,1);
return;
}


GetConsoleScreenBufferInfo(con_out, &csbi);
rcSrc.Left = csbi.srWindow.Left;
rcSrc.Top = csbi.srWindow.Top + n;
rcSrc.Right = csbi.srWindow.Right;
rcSrc.Bottom = csbi.srWindow.Bottom;


rcClip.Left = rcSrc.Left;
rcClip.Top = csbi.srWindow.Top;
rcClip.Right = rcSrc.Right;
rcClip.Bottom = rcSrc.Bottom ;


new_org.X = rcSrc.Left;
new_org.Y = rcClip.Top;


fillchar.Char.AsciiChar = ' ';
fillchar.Attributes = MAKEATTR(nm_fg_color, nm_bg_color);


SetConsoleTextAttribute(con_out, fillchar.Attributes);
ScrollConsoleScreenBuffer(con_out, &rcSrc, &rcClip, new_org, &fillchar);


topleft.X = csbi.dwCursorPosition.X;
topleft.Y = rcSrc.Bottom - n;
size = (n * csbi.dwSize.X) + (rcSrc.Right - topleft.X);
FillConsoleOutputCharacter(con_out, ' ', size, topleft,
&nchars);
FillConsoleOutputAttribute(con_out, fillchar.Attributes, size, topleft,
&nchars);
SetConsoleTextAttribute(con_out, curr_attr);


csbi.dwCursorPosition.Y -= n;
SetConsoleCursorPosition(con_out, csbi.dwCursorPosition);
}
#endif




public void
lower_left(VOID_PARAM)
{
assert_interactive();
#if !MSDOS_COMPILER
ltputs(sc_lower_left, 1, putchr);
#else
flush();
_settextposition(sc_height, 1);
#endif
}




public void
line_left(VOID_PARAM)
{
assert_interactive();
#if !MSDOS_COMPILER
ltputs(sc_return, 1, putchr);
#else
{
int row;
flush();
#if MSDOS_COMPILER==WIN32C
{
CONSOLE_SCREEN_BUFFER_INFO scr;
GetConsoleScreenBufferInfo(con_out, &scr);
row = scr.dwCursorPosition.Y - scr.srWindow.Top + 1;
}
#else
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
row = wherey();
#else
{
struct rccoord tpos = _gettextposition();
row = tpos.row;
}
#endif
#endif
_settextposition(row, 1);
}
#endif
}





public void
check_winch(VOID_PARAM)
{
#if MSDOS_COMPILER==WIN32C
CONSOLE_SCREEN_BUFFER_INFO scr;
COORD size;

if (con_out == INVALID_HANDLE_VALUE)
return;

flush();
GetConsoleScreenBufferInfo(con_out, &scr);
size.Y = scr.srWindow.Bottom - scr.srWindow.Top + 1;
size.X = scr.srWindow.Right - scr.srWindow.Left + 1;
if (size.Y != sc_height || size.X != sc_width)
{
sc_height = size.Y;
sc_width = size.X;
if (!no_init && con_out_ours == con_out)
SetConsoleScreenBufferSize(con_out, size);
pos_init();
wscroll = (sc_height + 1) / 2;
screen_trashed = 1;
}
#endif
}




public void
goto_line(sindex)
int sindex;
{
assert_interactive();
#if !MSDOS_COMPILER
ltputs(tgoto(sc_move, 0, sindex), 1, putchr);
#else
flush();
_settextposition(sindex+1, 1);
#endif
}

#if MSDOS_COMPILER==MSOFTC || MSDOS_COMPILER==BORLANDC






static void
create_flash(VOID_PARAM)
{
#if MSDOS_COMPILER==MSOFTC
struct videoconfig w;
char *blanks;
int row, col;

_getvideoconfig(&w);
videopages = w.numvideopages;
if (videopages < 2)
{
at_enter(AT_STANDOUT);
at_exit();
} else
{
_setactivepage(1);
at_enter(AT_STANDOUT);
blanks = (char *) ecalloc(w.numtextcols, sizeof(char));
for (col = 0; col < w.numtextcols; col++)
blanks[col] = ' ';
for (row = w.numtextrows; row > 0; row--)
_outmem(blanks, w.numtextcols);
_setactivepage(0);
_setvisualpage(0);
free(blanks);
at_exit();
}
#else
#if MSDOS_COMPILER==BORLANDC
int n;

whitescreen = (unsigned short *)
malloc(sc_width * sc_height * sizeof(short));
if (whitescreen == NULL)
return;
for (n = 0; n < sc_width * sc_height; n++)
whitescreen[n] = 0x7020;
#endif
#endif
flash_created = 1;
}
#endif




public void
vbell(VOID_PARAM)
{
#if !MSDOS_COMPILER
if (*sc_visual_bell == '\0')
return;
ltputs(sc_visual_bell, sc_height, putchr);
#else
#if MSDOS_COMPILER==DJGPPC
ScreenVisualBell();
#else
#if MSDOS_COMPILER==MSOFTC




if (!flash_created)
create_flash();
if (videopages < 2)
return;
_setvisualpage(1);
delay(100);
_setvisualpage(0);
#else
#if MSDOS_COMPILER==BORLANDC
unsigned short *currscreen;






if (!flash_created)
create_flash();
if (whitescreen == NULL)
return;
currscreen = (unsigned short *)
malloc(sc_width * sc_height * sizeof(short));
if (currscreen == NULL) return;
gettext(1, 1, sc_width, sc_height, currscreen);
puttext(1, 1, sc_width, sc_height, whitescreen);
delay(100);
puttext(1, 1, sc_width, sc_height, currscreen);
free(currscreen);
#else
#if MSDOS_COMPILER==WIN32C

clear();


Sleep(100);


repaint();
#endif
#endif
#endif
#endif
#endif
}




static void
beep(VOID_PARAM)
{
#if !MSDOS_COMPILER
putchr(CONTROL('G'));
#else
#if MSDOS_COMPILER==WIN32C
MessageBeep(0);
#else
write(1, "\7", 1);
#endif
#endif
}




public void
bell(VOID_PARAM)
{
if (quiet == VERY_QUIET)
vbell();
else
beep();
}




public void
clear(VOID_PARAM)
{
assert_interactive();
#if !MSDOS_COMPILER
ltputs(sc_clear, sc_height, putchr);
#else
flush();
#if MSDOS_COMPILER==WIN32C
win32_clear();
#else
_clearscreen(_GCLEARSCREEN);
#endif
#endif
}





public void
clear_eol(VOID_PARAM)
{

#if !MSDOS_COMPILER
ltputs(sc_eol_clear, 1, putchr);
#else
#if MSDOS_COMPILER==MSOFTC
short top, left;
short bot, right;
struct rccoord tpos;

flush();



tpos = _gettextposition();
_gettextwindow(&top, &left, &bot, &right);





_settextwindow(tpos.row, tpos.col, tpos.row, sc_width);
_clearscreen(_GWINDOW);



_settextwindow(top, left, bot, right);
_settextposition(tpos.row, tpos.col);
#else
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
flush();
clreol();
#else
#if MSDOS_COMPILER==WIN32C
DWORD nchars;
COORD cpos;
CONSOLE_SCREEN_BUFFER_INFO scr;

flush();
memset(&scr, 0, sizeof(scr));
GetConsoleScreenBufferInfo(con_out, &scr);
cpos.X = scr.dwCursorPosition.X;
cpos.Y = scr.dwCursorPosition.Y;
curr_attr = MAKEATTR(nm_fg_color, nm_bg_color);
FillConsoleOutputAttribute(con_out, curr_attr,
scr.dwSize.X - cpos.X, cpos, &nchars);
FillConsoleOutputCharacter(con_out, ' ',
scr.dwSize.X - cpos.X, cpos, &nchars);
#endif
#endif
#endif
#endif
}





static void
clear_eol_bot(VOID_PARAM)
{
assert_interactive();
#if MSDOS_COMPILER
clear_eol();
#else
if (below_mem)
ltputs(sc_eos_clear, 1, putchr);
else
ltputs(sc_eol_clear, 1, putchr);
#endif
}





public void
clear_bot(VOID_PARAM)
{





if (oldbot)
lower_left();
else
line_left();

if (attrmode == AT_NORMAL)
clear_eol_bot();
else
{
int saved_attrmode = attrmode;

at_exit();
clear_eol_bot();
at_enter(saved_attrmode);
}
}










static int
parse_color4(ch)
char ch;
{
switch (ch)
{
case 'k': return 0;
case 'r': return CV_RED;
case 'g': return CV_GREEN;
case 'y': return CV_RED|CV_GREEN;
case 'b': return CV_BLUE;
case 'm': return CV_RED|CV_BLUE;
case 'c': return CV_GREEN|CV_BLUE;
case 'w': return CV_RED|CV_GREEN|CV_BLUE;
case 'K': return 0|CV_BRIGHT;
case 'R': return CV_RED|CV_BRIGHT;
case 'G': return CV_GREEN|CV_BRIGHT;
case 'Y': return CV_RED|CV_GREEN|CV_BRIGHT;
case 'B': return CV_BLUE|CV_BRIGHT;
case 'M': return CV_RED|CV_BLUE|CV_BRIGHT;
case 'C': return CV_GREEN|CV_BLUE|CV_BRIGHT;
case 'W': return CV_RED|CV_GREEN|CV_BLUE|CV_BRIGHT;
case '-': return CV_NOCHANGE;
default: return CV_ERROR;
}
}




static int
parse_color6(ps)
char **ps;
{
if (**ps == '-')
{
(*ps)++;
return CV_NOCHANGE;
} else
{
char *ops = *ps;
int color = lstrtoi(ops, ps);
if (*ps == ops)
return CV_ERROR;
return color;
}
}







public COLOR_TYPE
parse_color(str, p_fg, p_bg)
char *str;
int *p_fg;
int *p_bg;
{
int fg;
int bg;
COLOR_TYPE type = CT_NULL;

if (str == NULL || *str == '\0')
return CT_NULL;
if (*str == '+')
str++;

fg = parse_color4(str[0]);
bg = parse_color4((strlen(str) < 2) ? '-' : str[1]);
if (fg != CV_ERROR && bg != CV_ERROR)
type = CT_4BIT;
else
{
fg = parse_color6(&str);
bg = (fg != CV_ERROR && *str++ == '.') ? parse_color6(&str) : CV_NOCHANGE;
if (fg != CV_ERROR && bg != CV_ERROR)
type = CT_6BIT;
}
if (p_fg != NULL) *p_fg = fg;
if (p_bg != NULL) *p_bg = bg;
return type;
}

#if !MSDOS_COMPILER

static int
sgr_color(color)
int color;
{
switch (color)
{
case 0: return 30;
case CV_RED: return 31;
case CV_GREEN: return 32;
case CV_RED|CV_GREEN: return 33;
case CV_BLUE: return 34;
case CV_RED|CV_BLUE: return 35;
case CV_GREEN|CV_BLUE: return 36;
case CV_RED|CV_GREEN|CV_BLUE: return 37;

case CV_BRIGHT: return 90;
case CV_RED|CV_BRIGHT: return 91;
case CV_GREEN|CV_BRIGHT: return 92;
case CV_RED|CV_GREEN|CV_BRIGHT: return 93;
case CV_BLUE|CV_BRIGHT: return 94;
case CV_RED|CV_BLUE|CV_BRIGHT: return 95;
case CV_GREEN|CV_BLUE|CV_BRIGHT: return 96;
case CV_RED|CV_GREEN|CV_BLUE|CV_BRIGHT: return 97;

default: return color;
}
}

static void
tput_fmt(fmt, color, f_putc)
char *fmt;
int color;
int (*f_putc)(int);
{
char buf[32];
if (color == attrcolor)
return;
SNPRINTF1(buf, sizeof(buf), fmt, color);
ltputs(buf, 1, f_putc);
attrcolor = color;
}

static void
tput_color(str, f_putc)
char *str;
int (*f_putc)(int);
{
int fg;
int bg;

if (str != NULL && strcmp(str, "*") == 0)
{

tput_fmt(ESCS"[m", -1, f_putc);
return;
}
switch (parse_color(str, &fg, &bg))
{
case CT_4BIT:
if (fg >= 0)
tput_fmt(ESCS"[%dm", sgr_color(fg), f_putc);
if (bg >= 0)
tput_fmt(ESCS"[%dm", sgr_color(bg)+10, f_putc);
break;
case CT_6BIT:
if (fg >= 0)
tput_fmt(ESCS"[38;5;%dm", fg, f_putc);
if (bg >= 0)
tput_fmt(ESCS"[48;5;%dm", bg, f_putc);
break;
default:
break;
}
}

static void
tput_inmode(mode_str, attr, attr_bit, f_putc)
char *mode_str;
int attr;
int attr_bit;
int (*f_putc)(int);
{
char *color_str;
if ((attr & attr_bit) == 0)
return;
color_str = get_color_map(attr_bit);
if (color_str == NULL || *color_str == '\0' || *color_str == '+')
{
ltputs(mode_str, 1, f_putc);
if (color_str == NULL || *color_str++ != '+')
return;
}

tput_color(color_str, f_putc);
}

static void
tput_outmode(mode_str, attr_bit, f_putc)
char *mode_str;
int attr_bit;
int (*f_putc)(int);
{
if ((attrmode & attr_bit) == 0)
return;
ltputs(mode_str, 1, f_putc);
}

#else

#if MSDOS_COMPILER==WIN32C
static int
WIN32put_fmt(fmt, color)
char *fmt;
int color;
{
char buf[16];
int len = SNPRINTF1(buf, sizeof(buf), fmt, color);
WIN32textout(buf, len);
return TRUE;
}
#endif

static int
win_set_color(attr)
int attr;
{
int fg;
int bg;
int out = FALSE;
char *str = get_color_map(attr);
if (str == NULL || str[0] == '\0')
return FALSE;
switch (parse_color(str, &fg, &bg))
{
case CT_4BIT:
if (fg >= 0 && bg >= 0)
{
SETCOLORS(fg, bg);
out = TRUE;
} else if (fg >= 0)
{
SET_FG_COLOR(fg);
out = TRUE;
} else if (bg >= 0)
{
SET_BG_COLOR(bg);
out = TRUE;
}
break;
#if MSDOS_COMPILER==WIN32C
case CT_6BIT:
if (vt_enabled)
{
if (fg > 0)
out = WIN32put_fmt(ESCS"[38;5;%dm", fg);
if (bg > 0)
out = WIN32put_fmt(ESCS"[48;5;%dm", bg);
}
break;
#endif
default:
break;
}
return out;
}

#endif

public void
at_enter(attr)
int attr;
{
attr = apply_at_specials(attr);
#if !MSDOS_COMPILER

tput_inmode(sc_u_in, attr, AT_UNDERLINE, putchr);
tput_inmode(sc_b_in, attr, AT_BOLD, putchr);
tput_inmode(sc_bl_in, attr, AT_BLINK, putchr);

if (use_color && (attr & AT_COLOR))
tput_color(get_color_map(attr), putchr);
else
tput_inmode(sc_s_in, attr, AT_STANDOUT, putchr);
#else
flush();

if ((attr & AT_COLOR) && use_color)
{
win_set_color(attr);
} else if (attr & AT_STANDOUT)
{
SETCOLORS(so_fg_color, so_bg_color);
} else if (attr & AT_BLINK)
{
SETCOLORS(bl_fg_color, bl_bg_color);
} else if (attr & AT_BOLD)
{
SETCOLORS(bo_fg_color, bo_bg_color);
} else if (attr & AT_UNDERLINE)
{
SETCOLORS(ul_fg_color, ul_bg_color);
}
#endif
attrmode = attr;
}

public void
at_exit(VOID_PARAM)
{
#if !MSDOS_COMPILER

tput_color("*", putchr);
tput_outmode(sc_s_out, AT_STANDOUT, putchr);
tput_outmode(sc_bl_out, AT_BLINK, putchr);
tput_outmode(sc_b_out, AT_BOLD, putchr);
tput_outmode(sc_u_out, AT_UNDERLINE, putchr);
#else
flush();
SETCOLORS(nm_fg_color, nm_bg_color);
#endif
attrmode = AT_NORMAL;
}

public void
at_switch(attr)
int attr;
{
int new_attrmode = apply_at_specials(attr);
int ignore_modes = AT_ANSI;

if ((new_attrmode & ~ignore_modes) != (attrmode & ~ignore_modes))
{
at_exit();
at_enter(attr);
}
}

public int
is_at_equiv(attr1, attr2)
int attr1;
int attr2;
{
attr1 = apply_at_specials(attr1);
attr2 = apply_at_specials(attr2);

return (attr1 == attr2);
}

public int
apply_at_specials(attr)
int attr;
{
if (attr & AT_BINARY)
attr |= binattr;
if (attr & AT_HILITE)
attr |= AT_STANDOUT;
attr &= ~(AT_BINARY|AT_HILITE);

return attr;
}




public void
putbs(VOID_PARAM)
{
if (termcap_debug)
putstr("<bs>");
else
{
#if !MSDOS_COMPILER
ltputs(sc_backspace, 1, putchr);
#else
int row, col;

flush();
{
#if MSDOS_COMPILER==MSOFTC
struct rccoord tpos;
tpos = _gettextposition();
row = tpos.row;
col = tpos.col;
#else
#if MSDOS_COMPILER==BORLANDC || MSDOS_COMPILER==DJGPPC
row = wherey();
col = wherex();
#else
#if MSDOS_COMPILER==WIN32C
CONSOLE_SCREEN_BUFFER_INFO scr;
GetConsoleScreenBufferInfo(con_out, &scr);
row = scr.dwCursorPosition.Y - scr.srWindow.Top + 1;
col = scr.dwCursorPosition.X - scr.srWindow.Left + 1;
#endif
#endif
#endif
}
if (col <= 1)
return;
_settextposition(row, col-1);
#endif
}
}

#if MSDOS_COMPILER==WIN32C



public int
win32_kbhit(VOID_PARAM)
{
INPUT_RECORD ip;
DWORD read;

if (keyCount > 0)
return (TRUE);

currentKey.ascii = 0;
currentKey.scan = 0;

if (x11mouseCount > 0)
{
currentKey.ascii = x11mousebuf[x11mousePos++];
--x11mouseCount;
keyCount = 1;
return (TRUE);
}





do
{
PeekConsoleInput(tty, &ip, 1, &read);
if (read == 0)
return (FALSE);
ReadConsoleInput(tty, &ip, 1, &read);

if (mousecap && ip.EventType == MOUSE_EVENT &&
ip.Event.MouseEvent.dwEventFlags != MOUSE_MOVED)
{
x11mousebuf[3] = X11MOUSE_OFFSET + ip.Event.MouseEvent.dwMousePosition.X + 1;
x11mousebuf[4] = X11MOUSE_OFFSET + ip.Event.MouseEvent.dwMousePosition.Y + 1;
switch (ip.Event.MouseEvent.dwEventFlags)
{
case 0:
if (ip.Event.MouseEvent.dwButtonState == 0)
x11mousebuf[2] = X11MOUSE_OFFSET + X11MOUSE_BUTTON_REL;
else if (ip.Event.MouseEvent.dwButtonState & (FROM_LEFT_3RD_BUTTON_PRESSED | FROM_LEFT_4TH_BUTTON_PRESSED))
continue;
else
x11mousebuf[2] = X11MOUSE_OFFSET + X11MOUSE_BUTTON1 + ((int)ip.Event.MouseEvent.dwButtonState << 1);
break;
case MOUSE_WHEELED:
x11mousebuf[2] = X11MOUSE_OFFSET + (((int)ip.Event.MouseEvent.dwButtonState < 0) ? X11MOUSE_WHEEL_DOWN : X11MOUSE_WHEEL_UP);
break;
default:
continue;
}
x11mousePos = 0;
x11mouseCount = 5;
currentKey.ascii = ESC;
keyCount = 1;
return (TRUE);
}
} while (ip.EventType != KEY_EVENT ||
ip.Event.KeyEvent.bKeyDown != TRUE ||
ip.Event.KeyEvent.wVirtualScanCode == 0 ||
ip.Event.KeyEvent.wVirtualKeyCode == VK_SHIFT ||
ip.Event.KeyEvent.wVirtualKeyCode == VK_CONTROL ||
ip.Event.KeyEvent.wVirtualKeyCode == VK_MENU);

currentKey.ascii = ip.Event.KeyEvent.uChar.AsciiChar;
currentKey.scan = ip.Event.KeyEvent.wVirtualScanCode;
keyCount = ip.Event.KeyEvent.wRepeatCount;

if (ip.Event.KeyEvent.dwControlKeyState &
(LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
{
switch (currentKey.scan)
{
case PCK_ALT_E:
currentKey.ascii = 0;
break;
}
} else if (ip.Event.KeyEvent.dwControlKeyState &
(LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
{
switch (currentKey.scan)
{
case PCK_RIGHT:
currentKey.scan = PCK_CTL_RIGHT;
break;
case PCK_LEFT:
currentKey.scan = PCK_CTL_LEFT;
break;
case PCK_DELETE:
currentKey.scan = PCK_CTL_DELETE;
break;
}
} else if (ip.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED)
{
switch (currentKey.scan)
{
case PCK_SHIFT_TAB:
currentKey.ascii = 0;
break;
}
}

return (TRUE);
}




public char
WIN32getch(VOID_PARAM)
{
int ascii;

if (pending_scancode)
{
pending_scancode = 0;
return ((char)(currentKey.scan & 0x00FF));
}

do {
while (win32_kbhit() == FALSE)
{
Sleep(20);
if (ABORT_SIGS())
return ('\003');
continue;
}
keyCount --;
ascii = currentKey.ascii;





pending_scancode = (ascii == 0x00);
} while (pending_scancode &&
(currentKey.scan == PCK_CAPS_LOCK || currentKey.scan == PCK_NUM_LOCK));

return ((char)ascii);
}
#endif

#if MSDOS_COMPILER


public void
WIN32setcolors(fg, bg)
int fg;
int bg;
{
SETCOLORS(fg, bg);
}



public void
WIN32textout(text, len)
char *text;
int len;
{
#if MSDOS_COMPILER==WIN32C
DWORD written;
if (utf_mode == 2)
{




WCHAR wtext[1024];
len = MultiByteToWideChar(CP_UTF8, 0, text, len, wtext,
sizeof(wtext)/sizeof(*wtext));
WriteConsoleW(con_out, wtext, len, &written, NULL);
} else
WriteConsole(con_out, text, len, &written, NULL);
#else
char c = text[len];
text[len] = '\0';
cputs(text);
text[len] = c;
#endif
}
#endif

