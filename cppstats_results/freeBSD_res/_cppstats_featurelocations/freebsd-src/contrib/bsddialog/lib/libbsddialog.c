


























#if defined(PORTNCURSES)
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bsddialog.h"
#include "lib_util.h"
#include "bsddialog_theme.h"















extern struct bsddialog_theme t;

int bsddialog_init(void)
{
int i, j, c, error;

set_error_string("");

if(initscr() == NULL)
RETURN_ERROR("Cannot init ncurses (initscr)");

error = OK;
error += keypad(stdscr, TRUE);
nl();
error += cbreak();
error += noecho();
curs_set(0);
if(error != OK) {
bsddialog_end();
RETURN_ERROR("Cannot init ncurses (keypad and cursor)");
}

c = 1;
error += start_color();
for (i=0; i<8; i++)
for(j=0; j<8; j++) {
error += init_pair(c, i, j);
c++;
}
if(error != OK) {
bsddialog_end();
RETURN_ERROR("Cannot init ncurses (colors)");
}

if (bsddialog_set_default_theme(BSDDIALOG_THEME_DEFAULT) != 0) {
bsddialog_end();
return (BSDDIALOG_ERROR);
}

return (BSDDIALOG_OK);
}

int bsddialog_end(void)
{
if (endwin() != OK)
RETURN_ERROR("Cannot end ncurses (endwin)");

return (BSDDIALOG_OK);
}

int bsddialog_backtitle(struct bsddialog_conf *conf, char *backtitle)
{
mvaddstr(0, 1, backtitle);
if (conf->no_lines != true)
mvhline(1, 1, conf->ascii_lines ? '-' : ACS_HLINE, COLS-2);

refresh();

return (BSDDIALOG_OK);
}

const char *bsddialog_geterror(void)
{
return (get_error_string());
}

int bsddialog_initconf(struct bsddialog_conf *conf)
{
if (conf == NULL)
RETURN_ERROR("conf is NULL");
if (sizeof(*conf) != sizeof(struct bsddialog_conf))
RETURN_ERROR("Bad conf size");

memset(conf, 0, sizeof(struct bsddialog_conf));
conf->y = BSDDIALOG_CENTER;
conf->x = BSDDIALOG_CENTER;
conf->shadow = true;

return (BSDDIALOG_OK);
}

int bsddialog_clearterminal(void)
{
if (clear() != OK)
RETURN_ERROR("Cannot clear the terminal");
refresh();

return (BSDDIALOG_OK);
}
