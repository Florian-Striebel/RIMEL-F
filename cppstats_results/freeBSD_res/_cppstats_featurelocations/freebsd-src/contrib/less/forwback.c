















#include "less.h"
#include "position.h"

public int screen_trashed;
public int squished;
public int no_back_scroll = 0;
public int forw_prompt;

extern int sigs;
extern int top_scroll;
extern int quiet;
extern int sc_width, sc_height;
extern int less_is_more;
extern int plusoption;
extern int forw_scroll;
extern int back_scroll;
extern int ignore_eoi;
extern int clear_bg;
extern int final_attr;
extern int oldbot;
#if HILITE_SEARCH
extern int size_linebuf;
extern int hilite_search;
extern int status_col;
#endif
#if TAGS
extern char *tagoption;
#endif




static void
eof_bell(VOID_PARAM)
{
#if HAVE_TIME
static time_type last_eof_bell = 0;
time_type now = get_time();
if (now == last_eof_bell)
return;
last_eof_bell = now;
#endif
if (quiet == NOT_QUIET)
bell();
else
vbell();
}




public int
eof_displayed(VOID_PARAM)
{
POSITION pos;

if (ignore_eoi)
return (0);

if (ch_length() == NULL_POSITION)




return (0);






pos = position(BOTTOM_PLUS_ONE);
return (pos == NULL_POSITION || pos == ch_length());
}




public int
entire_file_displayed(VOID_PARAM)
{
POSITION pos;


if (!eof_displayed())
return (0);


pos = position(0);
return (pos == NULL_POSITION || pos == 0);
}







public void
squish_check(VOID_PARAM)
{
if (!squished)
return;
squished = 0;
repaint();
}










public void
forw(n, pos, force, only_last, nblank)
int n;
POSITION pos;
int force;
int only_last;
int nblank;
{
int nlines = 0;
int do_repaint;
static int first_time = 1;

squish_check();










do_repaint = (only_last && n > sc_height-1) ||
(forw_scroll >= 0 && n > forw_scroll && n != sc_height-1);

#if HILITE_SEARCH
if (hilite_search == OPT_ONPLUS || is_filtering() || status_col) {
prep_hilite(pos, pos + 4*size_linebuf, ignore_eoi ? 1 : -1);
pos = next_unfiltered(pos);
}
#endif

if (!do_repaint)
{
if (top_scroll && n >= sc_height - 1 && pos != ch_length())
{






pos_clear();
add_forw_pos(pos);
force = 1;
if (less_is_more == 0) {
clear();
home();
}
}

if (pos != position(BOTTOM_PLUS_ONE) || empty_screen())
{





pos_clear();
add_forw_pos(pos);
force = 1;
if (top_scroll)
{
clear();
home();
} else if (!first_time && !is_filtering())
{
putstr("...skipping...\n");
}
}
}

while (--n >= 0)
{



if (nblank > 0)
{






if (--nblank == 0)
pos = ch_zero();
} else
{



pos = forw_line(pos);
#if HILITE_SEARCH
pos = next_unfiltered(pos);
#endif
if (pos == NULL_POSITION)
{






if (!force && position(TOP) != NULL_POSITION)
break;
if (!empty_lines(0, 0) &&
!empty_lines(1, 1) &&
empty_lines(2, sc_height-1))
break;
}
}




add_forw_pos(pos);
nlines++;
if (do_repaint)
continue;










if ((first_time || less_is_more) &&
pos == NULL_POSITION && !top_scroll &&
#if TAGS
tagoption == NULL &&
#endif
!plusoption)
{
squished = 1;
continue;
}
put_line();
#if 0








if (clear_bg && apply_at_specials(final_attr) != AT_NORMAL)
{







clear_eol();
}
#endif
forw_prompt = 1;
}

if (nlines == 0 && !ignore_eoi)
eof_bell();
else if (do_repaint)
repaint();
first_time = 0;
(void) currline(BOTTOM);
}




public void
back(n, pos, force, only_last)
int n;
POSITION pos;
int force;
int only_last;
{
int nlines = 0;
int do_repaint;

squish_check();
do_repaint = (n > get_back_scroll() || (only_last && n > sc_height-1));
#if HILITE_SEARCH
if (hilite_search == OPT_ONPLUS || is_filtering() || status_col) {
prep_hilite((pos < 3*size_linebuf) ? 0 : pos - 3*size_linebuf, pos, -1);
}
#endif
while (--n >= 0)
{



#if HILITE_SEARCH
pos = prev_unfiltered(pos);
#endif

pos = back_line(pos);
if (pos == NULL_POSITION)
{



if (!force)
break;
}




add_back_pos(pos);
nlines++;
if (!do_repaint)
{
home();
add_line();
put_line();
}
}

if (nlines == 0)
eof_bell();
else if (do_repaint)
repaint();
else if (!oldbot)
lower_left();
(void) currline(BOTTOM);
}





public void
forward(n, force, only_last)
int n;
int force;
int only_last;
{
POSITION pos;

if (get_quit_at_eof() && eof_displayed() && !(ch_getflags() & CH_HELPFILE))
{




if (edit_next(1))
quit(QUIT_OK);
return;
}

pos = position(BOTTOM_PLUS_ONE);
if (pos == NULL_POSITION && (!force || empty_lines(2, sc_height-1)))
{
if (ignore_eoi)
{





if (empty_screen())
pos = ch_zero();
else
{
do
{
back(1, position(TOP), 1, 0);
pos = position(BOTTOM_PLUS_ONE);
} while (pos == NULL_POSITION);
}
} else
{
eof_bell();
return;
}
}
forw(n, pos, force, only_last, 0);
}





public void
backward(n, force, only_last)
int n;
int force;
int only_last;
{
POSITION pos;

pos = position(TOP);
if (pos == NULL_POSITION && (!force || position(BOTTOM) == 0))
{
eof_bell();
return;
}
back(n, pos, force, only_last);
}







public int
get_back_scroll(VOID_PARAM)
{
if (no_back_scroll)
return (0);
if (back_scroll >= 0)
return (back_scroll);
if (top_scroll)
return (sc_height - 2);
return (10000);
}




public int
get_one_screen(VOID_PARAM)
{
int nlines;
POSITION pos = ch_zero();

for (nlines = 0; nlines < sc_height; nlines++)
{
pos = forw_line(pos);
if (pos == NULL_POSITION) break;
}
return (nlines < sc_height);
}
