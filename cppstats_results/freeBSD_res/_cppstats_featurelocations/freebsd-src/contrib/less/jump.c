













#include "less.h"
#include "position.h"

extern int jump_sline;
extern int squished;
extern int screen_trashed;
extern int sc_width, sc_height;
extern int show_attn;
extern int top_scroll;




public void
jump_forw(VOID_PARAM)
{
POSITION pos;
POSITION end_pos;

if (ch_end_seek())
{
error("Cannot seek to end of file", NULL_PARG);
return;
}





lastmark();





pos_clear();
end_pos = ch_tell();
pos = back_line(end_pos);
if (pos == NULL_POSITION)
jump_loc(ch_zero(), sc_height-1);
else
{
jump_loc(pos, sc_height-1);
if (position(sc_height-1) != end_pos)
repaint();
}
}




public void
jump_forw_buffered(VOID_PARAM)
{
POSITION end;

if (ch_end_buffer_seek())
{
error("Cannot seek to end of buffers", NULL_PARG);
return;
}
end = ch_tell();
if (end != NULL_POSITION && end > 0)
jump_line_loc(end-1, sc_height-1);
}




public void
jump_back(linenum)
LINENUM linenum;
{
POSITION pos;
PARG parg;







pos = find_pos(linenum);
if (pos != NULL_POSITION && ch_seek(pos) == 0)
{
if (show_attn)
set_attnpos(pos);
jump_loc(pos, jump_sline);
} else if (linenum <= 1 && ch_beg_seek() == 0)
{
jump_loc(ch_tell(), jump_sline);
error("Cannot seek to beginning of file", NULL_PARG);
} else
{
parg.p_linenum = linenum;
error("Cannot seek to line number %n", &parg);
}
}




public void
repaint(VOID_PARAM)
{
struct scrpos scrpos;




get_scrpos(&scrpos, TOP);
pos_clear();
if (scrpos.pos == NULL_POSITION)

jump_loc(ch_zero(), 1);
else
jump_loc(scrpos.pos, scrpos.ln);
}




public void
jump_percent(percent, fraction)
int percent;
long fraction;
{
POSITION pos, len;





if ((len = ch_length()) == NULL_POSITION)
{
ierror("Determining length of file", NULL_PARG);
ch_end_seek();
}
if ((len = ch_length()) == NULL_POSITION)
{
error("Don't know length of file", NULL_PARG);
return;
}
pos = percent_pos(len, percent, fraction);
if (pos >= len)
pos = len-1;

jump_line_loc(pos, jump_sline);
}






public void
jump_line_loc(pos, sline)
POSITION pos;
int sline;
{
int c;

if (ch_seek(pos) == 0)
{



while ((c = ch_back_get()) != '\n' && c != EOI)
;
if (c == '\n')
(void) ch_forw_get();
pos = ch_tell();
}
if (show_attn)
set_attnpos(pos);
jump_loc(pos, sline);
}






public void
jump_loc(pos, sline)
POSITION pos;
int sline;
{
int nline;
int sindex;
POSITION tpos;
POSITION bpos;




sindex = sindex_from_sline(sline);

if ((nline = onscreen(pos)) >= 0)
{




nline -= sindex;
if (nline > 0)
forw(nline, position(BOTTOM_PLUS_ONE), 1, 0, 0);
else
back(-nline, position(TOP), 1, 0);
#if HILITE_SEARCH
if (show_attn)
repaint_hilite(1);
#endif
return;
}





if (ch_seek(pos))
{
error("Cannot seek to that file position", NULL_PARG);
return;
}





tpos = position(TOP);
bpos = position(BOTTOM_PLUS_ONE);
if (tpos == NULL_POSITION || pos >= tpos)
{






for (nline = 0; nline < sindex; nline++)
{
if (bpos != NULL_POSITION && pos <= bpos)
{





forw(sc_height-sindex+nline-1, bpos, 1, 0, 0);
#if HILITE_SEARCH
if (show_attn)
repaint_hilite(1);
#endif
return;
}
pos = back_line(pos);
if (pos == NULL_POSITION)
{






break;
}
}
lastmark();
squished = 0;
screen_trashed = 0;
forw(sc_height-1, pos, 1, 0, sindex-nline);
} else
{






for (nline = sindex; nline < sc_height - 1; nline++)
{
pos = forw_line(pos);
if (pos == NULL_POSITION)
{





break;
}
#if HILITE_SEARCH
pos = next_unfiltered(pos);
#endif
if (pos >= tpos)
{





back(nline+1, tpos, 1, 0);
#if HILITE_SEARCH
if (show_attn)
repaint_hilite(1);
#endif
return;
}
}
lastmark();
if (!top_scroll)
clear();
else
home();
screen_trashed = 0;
add_back_pos(pos);
back(sc_height-1, pos, 1, 0);
}
}
