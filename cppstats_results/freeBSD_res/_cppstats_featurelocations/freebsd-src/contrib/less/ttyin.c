













#include "less.h"
#if OS2
#include "cmd.h"
#include "pckeys.h"
#endif
#if MSDOS_COMPILER==WIN32C
#define WIN32_LEAN_AND_MEAN
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x400
#endif
#include <windows.h>
public DWORD console_mode;
public HANDLE tty;
#else
public int tty;
#endif
#if LESSTEST
public char *ttyin_name = NULL;
public int rstat_file = -1;
#endif
extern int sigs;
extern int utf_mode;
extern int wheel_lines;




#if !MSDOS_COMPILER
public char *
tty_device(VOID_PARAM)
{
char *dev = NULL;
#if HAVE_TTYNAME
dev = ttyname(2);
#endif
if (dev == NULL)
dev = "/dev/tty";
#if LESSTEST
if (ttyin_name != NULL)
dev = ttyin_name;
#endif
return dev;
}
#endif




public void
open_getchr(VOID_PARAM)
{
#if MSDOS_COMPILER==WIN32C

SECURITY_ATTRIBUTES sa;
memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
sa.nLength = sizeof(SECURITY_ATTRIBUTES);
sa.bInheritHandle = TRUE;
tty = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE,
FILE_SHARE_READ, &sa,
OPEN_EXISTING, 0L, NULL);
GetConsoleMode(tty, &console_mode);

SetConsoleMode(tty, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
#else
#if MSDOS_COMPILER
extern int fd0;




fd0 = dup(0);
close(0);
tty = open("CON", OPEN_READ);
#if MSDOS_COMPILER==DJGPPC




(void) __djgpp_set_ctrl_c(1);
#endif
#else






#if OS2

tty = __open(tty_device(), OPEN_READ);
#else
tty = open(tty_device(), OPEN_READ);
#endif
if (tty < 0)
tty = 2;
#endif
#endif
}




public void
close_getchr(VOID_PARAM)
{
#if MSDOS_COMPILER==WIN32C
SetConsoleMode(tty, console_mode);
CloseHandle(tty);
#endif
}

#if MSDOS_COMPILER==WIN32C



int
pclose(f)
FILE *f;
{
int result;

result = _pclose(f);
SetConsoleMode(tty, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
return result;
}
#endif




public int
default_wheel_lines(VOID_PARAM)
{
int lines = 1;
#if MSDOS_COMPILER==WIN32C
if (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lines, 0))
{
if (lines == WHEEL_PAGESCROLL)
lines = 3;
}
#endif
return lines;
}

#if LESSTEST
public void
rstat(st)
char st;
{
if (rstat_file < 0)
return;
lseek(rstat_file, SEEK_SET, 0);
write(rstat_file, &st, 1);
}
#endif




public int
getchr(VOID_PARAM)
{
char c;
int result;

do
{
flush();
#if MSDOS_COMPILER && MSDOS_COMPILER != DJGPPC



#if MSDOS_COMPILER==WIN32C
if (ABORT_SIGS())
return (READ_INTR);
c = WIN32getch();
#else
c = getch();
#endif
result = 1;
if (c == '\003')
return (READ_INTR);
#else
#if LESSTEST
rstat('R');
#endif
{
unsigned char uc;
result = iread(tty, &uc, sizeof(char));
c = (char) uc;
}
#if LESSTEST
rstat('B');
#endif
if (result == READ_INTR)
return (READ_INTR);
if (result < 0)
{




quit(QUIT_ERROR);
}
#endif
#if 0

{
static int hex_in = 0;
static int hex_value = 0;
if (c == CONTROL('A'))
{
hex_in = 2;
result = 0;
continue;
}
if (hex_in > 0)
{
int v;
if (c >= '0' && c <= '9')
v = c - '0';
else if (c >= 'a' && c <= 'f')
v = c - 'a' + 10;
else if (c >= 'A' && c <= 'F')
v = c - 'A' + 10;
else
v = 0;
hex_value = (hex_value << 4) | v;
if (--hex_in > 0)
{
result = 0;
continue;
}
c = hex_value;
}
}
#endif





if (c == '\0')
c = '\340';
} while (result != 1);

return (c & 0xFF);
}
