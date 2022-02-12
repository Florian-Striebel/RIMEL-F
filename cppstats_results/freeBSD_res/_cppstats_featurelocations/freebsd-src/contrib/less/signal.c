




















#include "less.h"
#include <signal.h>




public int sigs;

extern int sc_width, sc_height;
extern int screen_trashed;
extern int lnloop;
extern int linenums;
extern int wscroll;
extern int reading;
extern int quit_on_intr;
extern int secure;
extern long jump_sline_fraction;

extern int less_is_more;




#if MSDOS_COMPILER!=WIN32C

static RETSIGTYPE
u_interrupt(type)
int type;
{
bell();
#if OS2
LSIGNAL(SIGINT, SIG_ACK);
#endif
LSIGNAL(SIGINT, u_interrupt);
sigs |= S_INTERRUPT;
#if MSDOS_COMPILER==DJGPPC





if (kbhit())
getkey();
#endif
if (less_is_more)
quit(0);
#if HILITE_SEARCH
set_filter_pattern(NULL, 0);
#endif
if (reading)
intread();
}
#endif

#if defined(SIGTSTP)




static RETSIGTYPE
stop(type)
int type;
{
LSIGNAL(SIGTSTP, stop);
sigs |= S_STOP;
if (reading)
intread();
}
#endif

#undef SIG_LESSWINDOW
#if defined(SIGWINCH)
#define SIG_LESSWINDOW SIGWINCH
#else
#if defined(SIGWIND)
#define SIG_LESSWINDOW SIGWIND
#endif
#endif

#if defined(SIG_LESSWINDOW)




public RETSIGTYPE
winch(type)
int type;
{
LSIGNAL(SIG_LESSWINDOW, winch);
sigs |= S_WINCH;
if (reading)
intread();
}
#endif

#if MSDOS_COMPILER==WIN32C



#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static BOOL WINAPI
wbreak_handler(dwCtrlType)
DWORD dwCtrlType;
{
switch (dwCtrlType)
{
case CTRL_C_EVENT:
case CTRL_BREAK_EVENT:
sigs |= S_INTERRUPT;
#if HILITE_SEARCH
set_filter_pattern(NULL, 0);
#endif
return (TRUE);
default:
break;
}
return (FALSE);
}
#endif

static RETSIGTYPE
terminate(type)
int type;
{
quit(15);
}




public void
init_signals(on)
int on;
{
if (on)
{



#if MSDOS_COMPILER==WIN32C
SetConsoleCtrlHandler(wbreak_handler, TRUE);
#else
(void) LSIGNAL(SIGINT, u_interrupt);
#endif
#if defined(SIGTSTP)
(void) LSIGNAL(SIGTSTP, secure ? SIG_IGN : stop);
#endif
#if defined(SIGWINCH)
(void) LSIGNAL(SIGWINCH, winch);
#endif
#if defined(SIGWIND)
(void) LSIGNAL(SIGWIND, winch);
#endif
#if defined(SIGQUIT)
(void) LSIGNAL(SIGQUIT, SIG_IGN);
#endif
#if defined(SIGTERM)
(void) LSIGNAL(SIGTERM, terminate);
#endif
} else
{



#if MSDOS_COMPILER==WIN32C
SetConsoleCtrlHandler(wbreak_handler, FALSE);
#else
(void) LSIGNAL(SIGINT, SIG_DFL);
#endif
#if defined(SIGTSTP)
(void) LSIGNAL(SIGTSTP, SIG_DFL);
#endif
#if defined(SIGWINCH)
(void) LSIGNAL(SIGWINCH, SIG_IGN);
#endif
#if defined(SIGWIND)
(void) LSIGNAL(SIGWIND, SIG_IGN);
#endif
#if defined(SIGQUIT)
(void) LSIGNAL(SIGQUIT, SIG_DFL);
#endif
#if defined(SIGTERM)
(void) LSIGNAL(SIGTERM, SIG_DFL);
#endif
}
}





public void
psignals(VOID_PARAM)
{
int tsignals;

if ((tsignals = sigs) == 0)
return;
sigs = 0;

#if defined(SIGTSTP)
if (tsignals & S_STOP)
{



#if defined(SIGTTOU)
LSIGNAL(SIGTTOU, SIG_IGN);
#endif
clear_bot();
deinit();
flush();
raw_mode(0);
#if defined(SIGTTOU)
LSIGNAL(SIGTTOU, SIG_DFL);
#endif
LSIGNAL(SIGTSTP, SIG_DFL);
kill(getpid(), SIGTSTP);






LSIGNAL(SIGTSTP, stop);
raw_mode(1);
init();
screen_trashed = 1;
tsignals |= S_WINCH;
}
#endif
#if defined(S_WINCH)
if (tsignals & S_WINCH)
{
int old_width, old_height;



old_width = sc_width;
old_height = sc_height;
get_term();
if (sc_width != old_width || sc_height != old_height)
{
wscroll = (sc_height + 1) / 2;
calc_jump_sline();
calc_shift_count();
}
screen_trashed = 1;
}
#endif
if (tsignals & S_INTERRUPT)
{
if (quit_on_intr)
quit(QUIT_INTERRUPT);
}
}
