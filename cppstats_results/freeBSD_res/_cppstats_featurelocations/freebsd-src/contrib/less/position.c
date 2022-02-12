



















#include "less.h"
#include "position.h"

static POSITION *table = NULL;
static int table_size = 0;

extern int sc_width, sc_height;










public POSITION
position(sindex)
int sindex;
{
switch (sindex)
{
case BOTTOM:
sindex = sc_height - 2;
break;
case BOTTOM_PLUS_ONE:
sindex = sc_height - 1;
break;
case MIDDLE:
sindex = (sc_height - 1) / 2;
break;
}
return (table[sindex]);
}




public void
add_forw_pos(pos)
POSITION pos;
{
int i;




for (i = 1; i < sc_height; i++)
table[i-1] = table[i];
table[sc_height - 1] = pos;
}




public void
add_back_pos(pos)
POSITION pos;
{
int i;




for (i = sc_height - 1; i > 0; i--)
table[i] = table[i-1];
table[0] = pos;
}




public void
pos_clear(VOID_PARAM)
{
int i;

for (i = 0; i < sc_height; i++)
table[i] = NULL_POSITION;
}




public void
pos_init(VOID_PARAM)
{
struct scrpos scrpos;

if (sc_height <= table_size)
return;




if (table != NULL)
{
get_scrpos(&scrpos, TOP);
free((char*)table);
} else
scrpos.pos = NULL_POSITION;
table = (POSITION *) ecalloc(sc_height, sizeof(POSITION));
table_size = sc_height;
pos_clear();
if (scrpos.pos != NULL_POSITION)
table[scrpos.ln-1] = scrpos.pos;
}






public int
onscreen(pos)
POSITION pos;
{
int i;

if (pos < table[0])
return (-1);
for (i = 1; i < sc_height; i++)
if (pos < table[i])
return (i-1);
return (-1);
}




public int
empty_screen(VOID_PARAM)
{
return (empty_lines(0, sc_height-1));
}

public int
empty_lines(s, e)
int s;
int e;
{
int i;

for (i = s; i <= e; i++)
if (table[i] != NULL_POSITION && table[i] != 0)
return (0);
return (1);
}









public void
get_scrpos(scrpos, where)
struct scrpos *scrpos;
int where;
{
int i;
int dir;
int last;

switch (where)
{
case TOP:
i = 0; dir = +1; last = sc_height-2;
break;
case BOTTOM: case BOTTOM_PLUS_ONE:
i = sc_height-2; dir = -1; last = 0;
break;
default:
i = where;
if (table[i] == NULL_POSITION) {
scrpos->pos = NULL_POSITION;
return;
}

break;
}





for (;; i += dir)
{
if (table[i] != NULL_POSITION)
{
scrpos->ln = i+1;
scrpos->pos = table[i];
return;
}
if (i == last) break;
}



scrpos->pos = NULL_POSITION;
}










public int
sindex_from_sline(sline)
int sline;
{




if (sline < 0)
sline += sc_height;



if (sline <= 0)
sline = 1;
if (sline > sc_height)
sline = sc_height;



return (sline-1);
}
