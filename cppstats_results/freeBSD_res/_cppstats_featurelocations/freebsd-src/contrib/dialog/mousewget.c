






















#include <dialog.h>
#include <dlg_keys.h>

static int
mouse_wgetch(WINDOW *win, int *fkey, bool ignore_errs)
{
int mouse_err = FALSE;
int key;

do {

key = dlg_getc(win, fkey);

#if USE_MOUSE

mouse_err = FALSE;
if (key == KEY_MOUSE) {
MEVENT event;

if (getmouse(&event) != ERR) {
mseRegion *p;

DLG_TRACE(("#mouse-click abs %d,%d (rel %d,%d)\n",
event.y, event.x,
event.y - getbegy(win),
event.x - getbegx(win)));
if ((p = dlg_mouse_region(event.y, event.x)) != 0) {
key = DLGK_MOUSE(p->code);
} else if ((p = dlg_mouse_bigregion(event.y, event.x)) != 0) {
int x = event.x - p->x;
int y = event.y - p->y;
int row = (p->X - p->x) / p->step_x;

key = -(p->code);
switch (p->mode) {
case 1:
key += y;
break;
case 2:
key += (x / p->step_x);
break;
default:
case 3:
key += (x / p->step_x) + (y * row);
break;
}
} else {
(void) beep();
mouse_err = TRUE;
}
} else {
(void) beep();
mouse_err = TRUE;
}
}
#endif

} while (ignore_errs && mouse_err);

return key;
}

int
dlg_mouse_wgetch(WINDOW *win, int *fkey)
{
return mouse_wgetch(win, fkey, TRUE);
}

int
dlg_mouse_wgetch_nowait(WINDOW *win, int *fkey)
{
return mouse_wgetch(win, fkey, FALSE);
}
