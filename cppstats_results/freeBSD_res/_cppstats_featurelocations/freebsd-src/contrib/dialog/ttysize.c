

























#include <dialog.h>






#if defined(HAVE_TERMIOS_H)
#include <termios.h>
#endif

#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif

#if defined(NEED_PTEM_H)



#include <sys/stream.h>
#include <sys/ptem.h>
#endif





#if defined(TIOCGSIZE)
#define IOCTL_WINSIZE TIOCGSIZE
#define STRUCT_WINSIZE struct ttysize
#define WINSIZE_ROWS(n) (int)n.ts_lines
#define WINSIZE_COLS(n) (int)n.ts_cols
#elif defined(TIOCGWINSZ)
#define IOCTL_WINSIZE TIOCGWINSZ
#define STRUCT_WINSIZE struct winsize
#define WINSIZE_ROWS(n) (int)n.ws_row
#define WINSIZE_COLS(n) (int)n.ws_col
#else
#undef HAVE_SIZECHANGE
#endif

int
dlg_ttysize(int fd, int *high, int *wide)
{
int rc = -1;
#if defined(HAVE_SIZECHANGE)
if (isatty(fd)) {
STRUCT_WINSIZE size;

if (ioctl(fd, IOCTL_WINSIZE, &size) >= 0) {
*high = WINSIZE_ROWS(size);
*wide = WINSIZE_COLS(size);
rc = 0;
}
}
#else
*high = 24;
*wide = 80;
#endif
return rc;
}
