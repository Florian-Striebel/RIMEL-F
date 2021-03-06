








#define NEWBOT 1








#define MSOFTC 1
#define BORLANDC 2
#define WIN32C 3
#define DJGPPC 4





#include <defines.h>

#if defined(_SEQUENT_)





#undef HAVE_SIGSETMASK
#endif




#if HAVE_ANSI_PROTOS
#define LESSPARAMS(a) a
#else
#define LESSPARAMS(a) ()
#endif
#if HAVE_VOID
#define VOID_POINTER void *
#define VOID_PARAM void
#else
#define VOID_POINTER char *
#define VOID_PARAM
#define void int
#endif
#if HAVE_CONST
#define constant const
#else
#define constant
#endif

#define public



#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_STDIO_H
#include <stdio.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_CTYPE_H
#include <ctype.h>
#endif
#if HAVE_WCTYPE_H
#include <wctype.h>
#endif
#if HAVE_LIMITS_H
#include <limits.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif


#if defined(_OSK)
#include <modes.h>
#include <strings.h>
#endif

#if defined(__TANDEM)
#include <floss.h>
#endif

#if MSDOS_COMPILER==WIN32C || OS2
#include <io.h>
#endif

#if MSDOS_COMPILER==DJGPPC
#include <io.h>
#include <sys/exceptn.h>
#include <conio.h>
#include <pc.h>
#endif

#if !HAVE_STDLIB_H
char *getenv();
off_t lseek();
VOID_POINTER calloc();
void free();
#endif





#define ASCII_IS_UPPER(c) ((c) >= 'A' && (c) <= 'Z')
#define ASCII_IS_LOWER(c) ((c) >= 'a' && (c) <= 'z')
#define ASCII_TO_UPPER(c) ((c) - 'a' + 'A')
#define ASCII_TO_LOWER(c) ((c) - 'A' + 'a')

#undef IS_UPPER
#undef IS_LOWER
#undef TO_UPPER
#undef TO_LOWER
#undef IS_SPACE
#undef IS_DIGIT

#if HAVE_WCTYPE
#define IS_UPPER(c) iswupper(c)
#define IS_LOWER(c) iswlower(c)
#define TO_UPPER(c) towupper(c)
#define TO_LOWER(c) towlower(c)
#else
#if HAVE_UPPER_LOWER
#define IS_UPPER(c) isupper((unsigned char) (c))
#define IS_LOWER(c) islower((unsigned char) (c))
#define TO_UPPER(c) toupper((unsigned char) (c))
#define TO_LOWER(c) tolower((unsigned char) (c))
#else
#define IS_UPPER(c) ASCII_IS_UPPER(c)
#define IS_LOWER(c) ASCII_IS_LOWER(c)
#define TO_UPPER(c) ASCII_TO_UPPER(c)
#define TO_LOWER(c) ASCII_TO_LOWER(c)
#endif
#endif

#if defined(isspace)
#define IS_SPACE(c) isspace((unsigned char)(c))
#else
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f')
#endif

#if defined(isdigit)
#define IS_DIGIT(c) isdigit((unsigned char)(c))
#else
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#endif

#define IS_CSI_START(c) (((LWCHAR)(c)) == ESC || (((LWCHAR)(c)) == CSI))

#if !defined(NULL)
#define NULL 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif
#if !defined(FALSE)
#define FALSE 0
#endif

#define OPT_OFF 0
#define OPT_ON 1
#define OPT_ONPLUS 2

#if !HAVE_MEMCPY
#if !defined(memcpy)
#define memcpy(to,from,len) bcopy((from),(to),(len))
#endif
#endif

#if HAVE_SNPRINTF
#define SNPRINTF1(str, size, fmt, v1) snprintf((str), (size), (fmt), (v1))
#define SNPRINTF2(str, size, fmt, v1, v2) snprintf((str), (size), (fmt), (v1), (v2))
#define SNPRINTF3(str, size, fmt, v1, v2, v3) snprintf((str), (size), (fmt), (v1), (v2), (v3))
#define SNPRINTF4(str, size, fmt, v1, v2, v3, v4) snprintf((str), (size), (fmt), (v1), (v2), (v3), (v4))
#else

#define SNPRINTF1(str, size, fmt, v1) sprintf((str), (fmt), (v1))
#define SNPRINTF2(str, size, fmt, v1, v2) sprintf((str), (fmt), (v1), (v2))
#define SNPRINTF3(str, size, fmt, v1, v2, v3) sprintf((str), (fmt), (v1), (v2), (v3))
#define SNPRINTF4(str, size, fmt, v1, v2, v3, v4) sprintf((str), (fmt), (v1), (v2), (v3), (v4))
#endif

#define BAD_LSEEK ((off_t)-1)

#if !defined(SEEK_SET)
#define SEEK_SET 0
#endif
#if !defined(SEEK_END)
#define SEEK_END 2
#endif

#if !defined(CHAR_BIT)
#define CHAR_BIT 8
#endif






#define INT_STRLEN_BOUND(t) ((sizeof(t) * CHAR_BIT - 1) * 302 / 1000 + 1 + 1)




typedef unsigned long LWCHAR;
typedef off_t POSITION;
typedef off_t LINENUM;
#define MIN_LINENUM_WIDTH 7
#define MAX_LINENUM_WIDTH 16
#define MAX_STATUSCOL_WIDTH 4
#define MAX_UTF_CHAR_LEN 6

#define NULL_POSITION ((POSITION)(-1))




#if MSDOS_COMPILER || OS2
#define OPEN_READ (O_RDONLY|O_BINARY)
#else
#if defined(_OSK)
#define OPEN_READ (S_IREAD)
#else
#if defined(O_RDONLY)
#define OPEN_READ (O_RDONLY)
#else
#define OPEN_READ (0)
#endif
#endif
#endif

#if defined(O_WRONLY) && defined(O_APPEND)
#define OPEN_APPEND (O_APPEND|O_WRONLY)
#else
#if defined(_OSK)
#define OPEN_APPEND (S_IWRITE)
#else
#define OPEN_APPEND (1)
#endif
#endif




#if MSDOS_COMPILER==MSOFTC
#define SET_BINARY(f) _setmode(f, _O_BINARY);
#else
#if MSDOS_COMPILER || OS2
#define SET_BINARY(f) setmode(f, O_BINARY)
#else
#define SET_BINARY(f)
#endif
#endif




#if MSDOS_COMPILER || OS2 || _OSK
#define SHELL_META_QUEST 0
#else
#define SHELL_META_QUEST 1
#endif

#define SPACES_IN_FILENAMES 1




#define IFILE VOID_POINTER
#define NULL_IFILE ((IFILE)NULL)








struct scrpos
{
POSITION pos;
int ln;
};

typedef union parg
{
char *p_string;
int p_int;
LINENUM p_linenum;
char p_char;
} PARG;

#define NULL_PARG ((PARG *)NULL)

struct textlist
{
char *string;
char *endstring;
};

struct wchar_range
{
LWCHAR first, last;
};

struct wchar_range_table
{
struct wchar_range *table;
int count;
};

#define EOI (-1)

#define READ_INTR (-2)


#define NUM_FRAC_DENOM 1000000
#define NUM_LOG_FRAC_DENOM 6


#define NOT_QUIET 0
#define LITTLE_QUIET 1
#define VERY_QUIET 2


#define PR_SHORT 0
#define PR_MEDIUM 1
#define PR_LONG 2


#define BS_SPECIAL 0
#define BS_NORMAL 1
#define BS_CONTROL 2


#define SRCH_FORW (1 << 0)
#define SRCH_BACK (1 << 1)
#define SRCH_NO_MOVE (1 << 2)
#define SRCH_INCR (1 << 3)
#define SRCH_FIND_ALL (1 << 4)
#define SRCH_NO_MATCH (1 << 8)
#define SRCH_PAST_EOF (1 << 9)
#define SRCH_FIRST_FILE (1 << 10)
#define SRCH_NO_REGEX (1 << 12)
#define SRCH_FILTER (1 << 13)
#define SRCH_AFTER_TARGET (1 << 14)
#define SRCH_WRAP (1 << 15)

#define SRCH_REVERSE(t) (((t) & SRCH_FORW) ? (((t) & ~SRCH_FORW) | SRCH_BACK) : (((t) & ~SRCH_BACK) | SRCH_FORW))




#define NO_MCA 0
#define MCA_DONE 1
#define MCA_MORE 2

#define CC_OK 0
#define CC_QUIT 1
#define CC_ERROR 2
#define CC_PASS 3

#define CF_QUIT_ON_ERASE 0001


#define AT_NORMAL (0)
#define AT_UNDERLINE (1 << 0)
#define AT_BOLD (1 << 1)
#define AT_BLINK (1 << 2)
#define AT_STANDOUT (1 << 3)
#define AT_ANSI (1 << 4)
#define AT_BINARY (1 << 5)
#define AT_HILITE (1 << 6)

#define AT_COLOR_SHIFT 8
#define AT_NUM_COLORS 16
#define AT_COLOR ((AT_NUM_COLORS-1) << AT_COLOR_SHIFT)
#define AT_COLOR_ATTN (1 << AT_COLOR_SHIFT)
#define AT_COLOR_BIN (2 << AT_COLOR_SHIFT)
#define AT_COLOR_CTRL (3 << AT_COLOR_SHIFT)
#define AT_COLOR_ERROR (4 << AT_COLOR_SHIFT)
#define AT_COLOR_LINENUM (5 << AT_COLOR_SHIFT)
#define AT_COLOR_MARK (6 << AT_COLOR_SHIFT)
#define AT_COLOR_PROMPT (7 << AT_COLOR_SHIFT)
#define AT_COLOR_RSCROLL (8 << AT_COLOR_SHIFT)
#define AT_COLOR_SEARCH (9 << AT_COLOR_SHIFT)

typedef enum { CT_NULL, CT_4BIT, CT_6BIT } COLOR_TYPE;

typedef enum {
CV_BLUE = 1,
CV_GREEN = 2,
CV_RED = 4,
CV_BRIGHT = 8,
CV_NOCHANGE = -2,
CV_ERROR = -1
} COLOR_VALUE;


#define ANSI_MID 1
#define ANSI_ERR 2
#define ANSI_END 3

#if '0' == 240
#define IS_EBCDIC_HOST 1
#endif

#if IS_EBCDIC_HOST





#define CONTROL(c) ( (c)=='[' ? '\047' : (c)=='a' ? '\001' : (c)=='b' ? '\002' : (c)=='c' ? '\003' : (c)=='d' ? '\067' : (c)=='e' ? '\055' : (c)=='f' ? '\056' : (c)=='g' ? '\057' : (c)=='h' ? '\026' : (c)=='i' ? '\005' : (c)=='j' ? '\025' : (c)=='k' ? '\013' : (c)=='l' ? '\014' : (c)=='m' ? '\015' : (c)=='n' ? '\016' : (c)=='o' ? '\017' : (c)=='p' ? '\020' : (c)=='q' ? '\021' : (c)=='r' ? '\022' : (c)=='s' ? '\023' : (c)=='t' ? '\074' : (c)=='u' ? '\075' : (c)=='v' ? '\062' : (c)=='w' ? '\046' : (c)=='x' ? '\030' : (c)=='y' ? '\031' : (c)=='z' ? '\077' : (c)=='A' ? '\001' : (c)=='B' ? '\002' : (c)=='C' ? '\003' : (c)=='D' ? '\067' : (c)=='E' ? '\055' : (c)=='F' ? '\056' : (c)=='G' ? '\057' : (c)=='H' ? '\026' : (c)=='I' ? '\005' : (c)=='J' ? '\025' : (c)=='K' ? '\013' : (c)=='L' ? '\014' : (c)=='M' ? '\015' : (c)=='N' ? '\016' : (c)=='O' ? '\017' : (c)=='P' ? '\020' : (c)=='Q' ? '\021' : (c)=='R' ? '\022' : (c)=='S' ? '\023' : (c)=='T' ? '\074' : (c)=='U' ? '\075' : (c)=='V' ? '\062' : (c)=='W' ? '\046' : (c)=='X' ? '\030' : (c)=='Y' ? '\031' : (c)=='Z' ? '\077' : (c)=='|' ? '\031' : (c)=='\\' ? '\034' : (c)=='^' ? '\036' : (c)&077)

























































#else
#define CONTROL(c) ((c)&037)
#endif

#define ESC CONTROL('[')
#define ESCS "\33"
#define CSI ((unsigned char)'\233')
#define CHAR_END_COMMAND 0x40000000

#if _OSK_MWC32
#define LSIGNAL(sig,func) os9_signal(sig,func)
#else
#define LSIGNAL(sig,func) signal(sig,func)
#endif

#if HAVE_SIGPROCMASK
#if HAVE_SIGSET_T
#else
#undef HAVE_SIGPROCMASK
#endif
#endif
#if HAVE_SIGPROCMASK
#if HAVE_SIGEMPTYSET
#else
#undef sigemptyset
#define sigemptyset(mp) *(mp) = 0
#endif
#endif

#define S_INTERRUPT 01
#define S_STOP 02
#define S_WINCH 04
#define ABORT_SIGS() (sigs & (S_INTERRUPT|S_STOP))

#define QUIT_OK 0
#define QUIT_ERROR 1
#define QUIT_INTERRUPT 2
#define QUIT_SAVED_STATUS (-1)

#define FOLLOW_DESC 0
#define FOLLOW_NAME 1


#define CH_CANSEEK 001
#define CH_KEEPOPEN 002
#define CH_POPENED 004
#define CH_HELPFILE 010
#define CH_NODATA 020

#define ch_zero() ((POSITION)0)

#define FAKE_HELPFILE "@/\\less/\\help/\\file/\\@"
#define FAKE_EMPTYFILE "@/\\less/\\empty/\\file/\\@"


#define CVT_TO_LC 01
#define CVT_BS 02
#define CVT_CRLF 04
#define CVT_ANSI 010

#if HAVE_TIME_T
#define time_type time_t
#else
#define time_type long
#endif


#define X11MOUSE_BUTTON1 0
#define X11MOUSE_BUTTON2 1
#define X11MOUSE_BUTTON3 2
#define X11MOUSE_BUTTON_REL 3
#define X11MOUSE_WHEEL_UP 0x40
#define X11MOUSE_WHEEL_DOWN 0x41
#define X11MOUSE_OFFSET 0x20

struct mlist;
struct loption;
struct hilite_tree;
struct ansi_state;
#include "pattern.h"
#include "xbuf.h"
#include "funcs.h"


void postoa LESSPARAMS ((POSITION, char*));
void linenumtoa LESSPARAMS ((LINENUM, char*));
void inttoa LESSPARAMS ((int, char*));
int lstrtoi LESSPARAMS ((char*, char**));
POSITION lstrtopos LESSPARAMS ((char*, char**));
#if MSDOS_COMPILER==WIN32C
int pclose LESSPARAMS ((FILE*));
#endif
