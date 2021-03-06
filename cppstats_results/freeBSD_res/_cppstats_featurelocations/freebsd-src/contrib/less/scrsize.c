









































#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>

static int get_winsize(dpy, window, p_width, p_height)
Display *dpy;
Window window;
int *p_width;
int *p_height;
{
XWindowAttributes win_attributes;
XSizeHints hints;
long longjunk;

if (!XGetWindowAttributes(dpy, window, &win_attributes))
return 1;
if (!XGetWMNormalHints(dpy, window, &hints, &longjunk))
return 1;
if (!(hints.flags & PResizeInc))
return 1;
if (hints.width_inc == 0 || hints.height_inc == 0)
return 1;
if (!(hints.flags & (PBaseSize|PMinSize)))
return 1;
if (hints.flags & PBaseSize)
{
win_attributes.width -= hints.base_width;
win_attributes.height -= hints.base_height;
} else
{
win_attributes.width -= hints.min_width;
win_attributes.height -= hints.min_height;
}
*p_width = win_attributes.width / hints.width_inc;
*p_height = win_attributes.height / hints.height_inc;
return 0;
}

int main(argc, argv)
int argc;
char *argv[];
{
char *cp;
Display *dpy;
int size[2];

_scrsize(size);
cp = getenv("WINDOWID");
if (cp != NULL)
{
dpy = XOpenDisplay(NULL);
if (dpy != NULL)
{
get_winsize(dpy, (Window) atol(cp), &size[0], &size[1]);
XCloseDisplay(dpy);
}
}
printf("%i %i\n", size[0], size[1]);
return (0);
}
