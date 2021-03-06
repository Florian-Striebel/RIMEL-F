


























#include <sys/param.h>

#if defined(PORTNCURSES)
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include "bsddialog.h"
#include "lib_util.h"
#include "bsddialog_theme.h"



#define MIN_HEIGHT 3

extern struct bsddialog_theme t;

static int
infobox_autosize(struct bsddialog_conf *conf, int rows, int cols, int *h, int *w,
char *text)
{
int maxword, maxline, nlines;

if (get_text_properties(conf, text, &maxword, &maxline, &nlines) != 0)
return BSDDIALOG_ERROR;

if (cols == BSDDIALOG_AUTOSIZE) {

*w = maxline + VBORDERS + t.text.hmargin * 2;

*w = MAX(*w, (int)conf->auto_minwidth);

*w = MIN(*w, widget_max_width(conf));
}

if (rows == BSDDIALOG_AUTOSIZE) {
*h = MIN_HEIGHT - 1;
if (maxword > 0)
*h += MIN(nlines, (int)(*w / GET_ASPECT_RATIO(conf)));
*h = MAX(*h, MIN_HEIGHT);

*h = MAX(*h, (int)conf->auto_minheight);

*h = MIN(*h, widget_max_height(conf));
}

return 0;
}

static int infobox_checksize(int rows, int cols)
{

if (cols < HBORDERS + 1 + (int) t.text.hmargin * 2)
RETURN_ERROR("Few cols, infobox needs at least width 3 + text "\
"margins");

if (rows < 3)
RETURN_ERROR("Infobox needs at least height 3");

return 0;
}

int
bsddialog_infobox(struct bsddialog_conf *conf, char* text, int rows, int cols)
{
WINDOW *shadow, *widget, *textpad;
int y, x, h, w, htextpad;

if (set_widget_size(conf, rows, cols, &h, &w) != 0)
return BSDDIALOG_ERROR;
if (infobox_autosize(conf, rows, cols, &h, &w, text) != 0)
return BSDDIALOG_ERROR;
if (infobox_checksize(h, w) != 0)
return BSDDIALOG_ERROR;
if (set_widget_position(conf, &y, &x, h, w) != 0)
return BSDDIALOG_ERROR;

if (new_widget_withtextpad(conf, &shadow, &widget, y, x, h, w, RAISED,
&textpad, &htextpad, text, false) != 0)
return BSDDIALOG_ERROR;

pnoutrefresh(textpad, 0, 0, y+1, x+1+t.text.hmargin, y+h-2, x+w-t.text.hmargin);

doupdate();

end_widget_withtextpad(conf, widget, h, w, textpad, shadow);

return (BSDDIALOG_OK);
}

