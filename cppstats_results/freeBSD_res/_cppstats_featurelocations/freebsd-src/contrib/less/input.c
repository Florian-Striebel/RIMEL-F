


















#include "less.h"

extern int squeeze;
extern int chopline;
extern int hshift;
extern int quit_if_one_screen;
extern int sigs;
extern int ignore_eoi;
extern int status_col;
extern POSITION start_attnpos;
extern POSITION end_attnpos;
#if HILITE_SEARCH
extern int hilite_search;
extern int size_linebuf;
extern int show_attn;
#endif








public POSITION
forw_line_seg(curr_pos, get_segpos)
POSITION curr_pos;
int get_segpos;
{
POSITION base_pos;
POSITION new_pos;
int c;
int blankline;
int endline;
int chopped;
int backchars;

get_forw_line:
if (curr_pos == NULL_POSITION)
{
null_line();
return (NULL_POSITION);
}
#if HILITE_SEARCH
if (hilite_search == OPT_ONPLUS || is_filtering() || status_col)
{







prep_hilite(curr_pos, curr_pos + 3*size_linebuf,
ignore_eoi ? 1 : -1);
curr_pos = next_unfiltered(curr_pos);
}
#endif
if (ch_seek(curr_pos))
{
null_line();
return (NULL_POSITION);
}




base_pos = curr_pos;
for (;;)
{
if (ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
c = ch_back_get();
if (c == EOI)
break;
if (c == '\n')
{
(void) ch_forw_get();
break;
}
--base_pos;
}




prewind();
plinestart(base_pos);
(void) ch_seek(base_pos);
new_pos = base_pos;
while (new_pos < curr_pos)
{
if (ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
c = ch_forw_get();
backchars = pappend(c, new_pos);
new_pos++;
if (backchars > 0)
{
pshift_all();
new_pos -= backchars;
while (--backchars >= 0)
(void) ch_back_get();
}
}
(void) pflushmbc();
pshift_all();




c = ch_forw_get();
if (c == EOI)
{
null_line();
return (NULL_POSITION);
}
blankline = (c == '\n' || c == '\r');




chopped = FALSE;
for (;;)
{
if (ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
if (c == '\n' || c == EOI)
{



backchars = pflushmbc();
new_pos = ch_tell();
if (backchars > 0 && !chopline && hshift == 0)
{
new_pos -= backchars + 1;
endline = FALSE;
} else
endline = TRUE;
break;
}
if (c != '\r')
blankline = 0;




backchars = pappend(c, ch_tell()-1);
if (backchars > 0)
{





if ((chopline || hshift > 0) && !get_segpos)
{

do
{
if (ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
c = ch_forw_get();
} while (c != '\n' && c != EOI);
new_pos = ch_tell();
endline = TRUE;
quit_if_one_screen = FALSE;
chopped = TRUE;
} else
{
new_pos = ch_tell() - backchars;
endline = FALSE;
}
break;
}
c = ch_forw_get();
}

#if HILITE_SEARCH
if (blankline && show_attn)
{

pappend(' ', ch_tell()-1);
}
#endif
pdone(endline, chopped, 1);

#if HILITE_SEARCH
if (is_filtered(base_pos))
{




curr_pos = new_pos;
goto get_forw_line;
}

if (status_col)
{
int attr = is_hilited_attr(base_pos, ch_tell()-1, 1, NULL);
if (attr)
set_status_col('*', attr);
}
#endif

if (squeeze && blankline)
{





while ((c = ch_forw_get()) == '\n' || c == '\r')
if (ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
if (c != EOI)
(void) ch_back_get();
new_pos = ch_tell();
}

return (new_pos);
}

public POSITION
forw_line(curr_pos)
POSITION curr_pos;
{
return forw_line_seg(curr_pos, FALSE);
}








public POSITION
back_line(curr_pos)
POSITION curr_pos;
{
POSITION new_pos, begin_new_pos, base_pos;
int c;
int endline;
int chopped;
int backchars;

get_back_line:
if (curr_pos == NULL_POSITION || curr_pos <= ch_zero())
{
null_line();
return (NULL_POSITION);
}
#if HILITE_SEARCH
if (hilite_search == OPT_ONPLUS || is_filtering() || status_col)
prep_hilite((curr_pos < 3*size_linebuf) ?
0 : curr_pos - 3*size_linebuf, curr_pos, -1);
#endif
if (ch_seek(curr_pos-1))
{
null_line();
return (NULL_POSITION);
}

if (squeeze)
{



(void) ch_forw_get();
c = ch_forw_get();
(void) ch_back_get();
(void) ch_back_get();

if (c == '\n' || c == '\r')
{





while ((c = ch_back_get()) == '\n' || c == '\r')
if (ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
if (c == EOI)
{
null_line();
return (NULL_POSITION);
}
(void) ch_forw_get();
}
}




for (;;)
{
if (ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
c = ch_back_get();
if (c == '\n')
{




base_pos = ch_tell() + 1;
break;
}
if (c == EOI)
{





base_pos = ch_tell();
break;
}
}










new_pos = base_pos;
if (ch_seek(new_pos))
{
null_line();
return (NULL_POSITION);
}
endline = FALSE;
prewind();
plinestart(new_pos);
loop:
begin_new_pos = new_pos;
(void) ch_seek(new_pos);
chopped = FALSE;

do
{
c = ch_forw_get();
if (c == EOI || ABORT_SIGS())
{
null_line();
return (NULL_POSITION);
}
new_pos++;
if (c == '\n')
{
backchars = pflushmbc();
if (backchars > 0 && !chopline && hshift == 0)
{
backchars++;
goto shift;
}
endline = TRUE;
break;
}
backchars = pappend(c, ch_tell()-1);
if (backchars > 0)
{





if (chopline || hshift > 0)
{
endline = TRUE;
chopped = TRUE;
quit_if_one_screen = FALSE;
break;
}
shift:
pshift_all();
while (backchars-- > 0)
{
(void) ch_back_get();
new_pos--;
}
goto loop;
}
} while (new_pos < curr_pos);

pdone(endline, chopped, 0);

#if HILITE_SEARCH
if (is_filtered(base_pos))
{




curr_pos = begin_new_pos;
goto get_back_line;
}

if (status_col && curr_pos > 0)
{
int attr = is_hilited_attr(base_pos, curr_pos-1, 1, NULL);
if (attr)
set_status_col('*', attr);
}
#endif

return (begin_new_pos);
}




public void
set_attnpos(pos)
POSITION pos;
{
int c;

if (pos != NULL_POSITION)
{
if (ch_seek(pos))
return;
for (;;)
{
c = ch_forw_get();
if (c == EOI)
break;
if (c == '\n' || c == '\r')
{
(void) ch_back_get();
break;
}
pos++;
}
end_attnpos = pos;
for (;;)
{
c = ch_back_get();
if (c == EOI || c == '\n' || c == '\r')
break;
pos--;
}
}
start_attnpos = pos;
}
