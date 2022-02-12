













#include "less.h"
#include "position.h"









public void
match_brac(obrac, cbrac, forwdir, n)
int obrac;
int cbrac;
int forwdir;
int n;
{
int c;
int nest;
POSITION pos;
int (*chget)();

extern int ch_forw_get(), ch_back_get();






pos = position((forwdir) ? TOP : BOTTOM);
if (pos == NULL_POSITION || ch_seek(pos))
{
if (forwdir)
error("Nothing in top line", NULL_PARG);
else
error("Nothing in bottom line", NULL_PARG);
return;
}




do
{
if ((c = ch_forw_get()) == '\n' || c == EOI)
{
if (forwdir)
error("No bracket in top line", NULL_PARG);
else
error("No bracket in bottom line", NULL_PARG);
return;
}
} while (c != obrac || --n > 0);







if (!forwdir)
(void) ch_back_get();




chget = (forwdir) ? ch_forw_get : ch_back_get;
nest = 0;
while ((c = (*chget)()) != EOI)
{
if (c == obrac)
nest++;
else if (c == cbrac && --nest < 0)
{





jump_line_loc(ch_tell(), forwdir ? -1 : 1);
return;
}
}
error("No matching bracket", NULL_PARG);
}
