






































#if !defined(_h_el_tty)
#define _h_el_tty

#include <termios.h>
#include <unistd.h>


#define CONTROL(A) ((A) & 037)




#if defined(VWERSE) && !defined(VWERASE)
#define VWERASE VWERSE
#endif

#if defined(VDISCRD) && !defined(VDISCARD)
#define VDISCARD VDISCRD
#endif

#if defined(VFLUSHO) && !defined(VDISCARD)
#define VDISCARD VFLUSHO
#endif

#if defined(VSTRT) && !defined(VSTART)
#define VSTART VSTRT
#endif

#if defined(VSTAT) && !defined(VSTATUS)
#define VSTATUS VSTAT
#endif

#if !defined(ONLRET)
#define ONLRET 0
#endif

#if !defined(TAB3)
#if defined(OXTABS)
#define TAB3 OXTABS
#else
#define TAB3 0
#endif
#endif

#if defined(OXTABS) && !defined(XTABS)
#define XTABS OXTABS
#endif

#if !defined(ONLCR)
#define ONLCR 0
#endif

#if !defined(IEXTEN)
#define IEXTEN 0
#endif

#if !defined(ECHOCTL)
#define ECHOCTL 0
#endif

#if !defined(PARENB)
#define PARENB 0
#endif

#if !defined(EXTPROC)
#define EXTPROC 0
#endif

#if !defined(FLUSHO)
#define FLUSHO 0
#endif


#if defined(VDISABLE) && !defined(_POSIX_VDISABLE)
#define _POSIX_VDISABLE VDISABLE
#endif





#if defined(ISC)
#if defined(IEXTEN) && defined(XCASE)
#if IEXTEN == XCASE
#undef IEXTEN
#define IEXTEN 0
#endif
#endif
#if defined(IEXTEN) && !defined(XCASE)
#define XCASE IEXTEN
#undef IEXTEN
#define IEXTEN 0
#endif
#endif





#if defined(convex) || defined(__convex__)
#if defined(IEXTEN) && IEXTEN != 0
#undef IEXTEN
#define IEXTEN 0
#endif
#endif




#if defined(__SVR4)
#undef CSWTCH
#endif

#if !defined(_POSIX_VDISABLE)
#define _POSIX_VDISABLE ((unsigned char) -1)
#endif

#if !defined(CREPRINT) && defined(CRPRNT)
#define CREPRINT CRPRNT
#endif
#if !defined(CDISCARD) && defined(CFLUSH)
#define CDISCARD CFLUSH
#endif

#if !defined(CINTR)
#define CINTR CONTROL('c')
#endif
#if !defined(CQUIT)
#define CQUIT 034
#endif
#if !defined(CERASE)
#define CERASE 0177
#endif
#if !defined(CKILL)
#define CKILL CONTROL('u')
#endif
#if !defined(CEOF)
#define CEOF CONTROL('d')
#endif
#if !defined(CEOL)
#define CEOL _POSIX_VDISABLE
#endif
#if !defined(CEOL2)
#define CEOL2 _POSIX_VDISABLE
#endif
#if !defined(CSWTCH)
#define CSWTCH _POSIX_VDISABLE
#endif
#if !defined(CDSWTCH)
#define CDSWTCH _POSIX_VDISABLE
#endif
#if !defined(CERASE2)
#define CERASE2 _POSIX_VDISABLE
#endif
#if !defined(CSTART)
#define CSTART CONTROL('q')
#endif
#if !defined(CSTOP)
#define CSTOP CONTROL('s')
#endif
#if !defined(CSUSP)
#define CSUSP CONTROL('z')
#endif
#if !defined(CDSUSP)
#define CDSUSP CONTROL('y')
#endif

#if defined(hpux)

#if !defined(CREPRINT)
#define CREPRINT _POSIX_VDISABLE
#endif
#if !defined(CDISCARD)
#define CDISCARD _POSIX_VDISABLE
#endif
#if !defined(CLNEXT)
#define CLNEXT _POSIX_VDISABLE
#endif
#if !defined(CWERASE)
#define CWERASE _POSIX_VDISABLE
#endif

#else

#if !defined(CREPRINT)
#define CREPRINT CONTROL('r')
#endif
#if !defined(CDISCARD)
#define CDISCARD CONTROL('o')
#endif
#if !defined(CLNEXT)
#define CLNEXT CONTROL('v')
#endif
#if !defined(CWERASE)
#define CWERASE CONTROL('w')
#endif

#endif

#if !defined(CSTATUS)
#define CSTATUS CONTROL('t')
#endif
#if !defined(CPAGE)
#define CPAGE ' '
#endif
#if !defined(CPGOFF)
#define CPGOFF CONTROL('m')
#endif
#if !defined(CKILL2)
#define CKILL2 _POSIX_VDISABLE
#endif
#if !defined(CBRK)
#if !defined(masscomp)
#define CBRK 0377
#else
#define CBRK '\0'
#endif
#endif
#if !defined(CMIN)
#define CMIN CEOF
#endif
#if !defined(CTIME)
#define CTIME CEOL
#endif





#if defined(TERMIO) || defined(POSIX)
#if defined(POSIX) && defined(NCCS)
#define NUMCC NCCS
#else
#if defined(NCC)
#define NUMCC NCC
#endif
#endif
#if defined(NUMCC)
#if defined(VINTR)
#if NUMCC <= VINTR
#undef VINTR
#endif
#endif
#if defined(VQUIT)
#if NUMCC <= VQUIT
#undef VQUIT
#endif
#endif
#if defined(VERASE)
#if NUMCC <= VERASE
#undef VERASE
#endif
#endif
#if defined(VKILL)
#if NUMCC <= VKILL
#undef VKILL
#endif
#endif
#if defined(VEOF)
#if NUMCC <= VEOF
#undef VEOF
#endif
#endif
#if defined(VEOL)
#if NUMCC <= VEOL
#undef VEOL
#endif
#endif
#if defined(VEOL2)
#if NUMCC <= VEOL2
#undef VEOL2
#endif
#endif
#if defined(VSWTCH)
#if NUMCC <= VSWTCH
#undef VSWTCH
#endif
#endif
#if defined(VDSWTCH)
#if NUMCC <= VDSWTCH
#undef VDSWTCH
#endif
#endif
#if defined(VERASE2)
#if NUMCC <= VERASE2
#undef VERASE2
#endif
#endif
#if defined(VSTART)
#if NUMCC <= VSTART
#undef VSTART
#endif
#endif
#if defined(VSTOP)
#if NUMCC <= VSTOP
#undef VSTOP
#endif
#endif
#if defined(VWERASE)
#if NUMCC <= VWERASE
#undef VWERASE
#endif
#endif
#if defined(VSUSP)
#if NUMCC <= VSUSP
#undef VSUSP
#endif
#endif
#if defined(VDSUSP)
#if NUMCC <= VDSUSP
#undef VDSUSP
#endif
#endif
#if defined(VREPRINT)
#if NUMCC <= VREPRINT
#undef VREPRINT
#endif
#endif
#if defined(VDISCARD)
#if NUMCC <= VDISCARD
#undef VDISCARD
#endif
#endif
#if defined(VLNEXT)
#if NUMCC <= VLNEXT
#undef VLNEXT
#endif
#endif
#if defined(VSTATUS)
#if NUMCC <= VSTATUS
#undef VSTATUS
#endif
#endif
#if defined(VPAGE)
#if NUMCC <= VPAGE
#undef VPAGE
#endif
#endif
#if defined(VPGOFF)
#if NUMCC <= VPGOFF
#undef VPGOFF
#endif
#endif
#if defined(VKILL2)
#if NUMCC <= VKILL2
#undef VKILL2
#endif
#endif
#if defined(VBRK)
#if NUMCC <= VBRK
#undef VBRK
#endif
#endif
#if defined(VMIN)
#if NUMCC <= VMIN
#undef VMIN
#endif
#endif
#if defined(VTIME)
#if NUMCC <= VTIME
#undef VTIME
#endif
#endif
#endif
#endif

#define C_INTR 0
#define C_QUIT 1
#define C_ERASE 2
#define C_KILL 3
#define C_EOF 4
#define C_EOL 5
#define C_EOL2 6
#define C_SWTCH 7
#define C_DSWTCH 8
#define C_ERASE2 9
#define C_START 10
#define C_STOP 11
#define C_WERASE 12
#define C_SUSP 13
#define C_DSUSP 14
#define C_REPRINT 15
#define C_DISCARD 16
#define C_LNEXT 17
#define C_STATUS 18
#define C_PAGE 19
#define C_PGOFF 20
#define C_KILL2 21
#define C_BRK 22
#define C_MIN 23
#define C_TIME 24
#define C_NCC 25
#define C_SH(A) ((unsigned int)(1 << (A)))




#define EX_IO 0
#define ED_IO 1
#define TS_IO 2
#define QU_IO 2
#define NN_IO 3


#define MD_INP 0
#define MD_OUT 1
#define MD_CTL 2
#define MD_LIN 3
#define MD_CHAR 4
#define MD_NN 5

typedef struct {
const char *t_name;
unsigned int t_setmask;
unsigned int t_clrmask;
} ttyperm_t[NN_IO][MD_NN];

typedef unsigned char ttychar_t[NN_IO][C_NCC];

libedit_private int tty_init(EditLine *);
libedit_private void tty_end(EditLine *, int);
libedit_private int tty_stty(EditLine *, int, const wchar_t **);
libedit_private int tty_rawmode(EditLine *);
libedit_private int tty_cookedmode(EditLine *);
libedit_private int tty_quotemode(EditLine *);
libedit_private int tty_noquotemode(EditLine *);
libedit_private void tty_bind_char(EditLine *, int);
libedit_private int tty_get_signal_character(EditLine *, int);

typedef struct {
ttyperm_t t_t;
ttychar_t t_c;
struct termios t_or, t_ex, t_ed, t_ts;
int t_tabs;
int t_eight;
speed_t t_speed;
unsigned char t_mode;
unsigned char t_vdisable;
unsigned char t_initialized;
} el_tty_t;


#endif
