

































#include "less.h"





struct linenum_info
{
struct linenum_info *next;
struct linenum_info *prev;
POSITION pos;
POSITION gap;
LINENUM line;
};









#define NPOOL 200

#define LONGTIME (2)

static struct linenum_info anchor;
static struct linenum_info *freelist;
static struct linenum_info pool[NPOOL];
static struct linenum_info *spare;

extern int linenums;
extern int sigs;
extern int sc_height;
extern int screen_trashed;




public void
clr_linenum(VOID_PARAM)
{
struct linenum_info *p;





for (p = pool; p < &pool[NPOOL-2]; p++)
p->next = p+1;
pool[NPOOL-2].next = NULL;
freelist = pool;

spare = &pool[NPOOL-1];




anchor.next = anchor.prev = &anchor;
anchor.gap = 0;
anchor.pos = (POSITION)0;
anchor.line = 1;
}




static void
calcgap(p)
struct linenum_info *p;
{






if (p == &anchor || p->next == &anchor)
return;
p->gap = p->next->pos - p->prev->pos;
}






public void
add_lnum(linenum, pos)
LINENUM linenum;
POSITION pos;
{
struct linenum_info *p;
struct linenum_info *new;
struct linenum_info *nextp;
struct linenum_info *prevp;
POSITION mingap;





for (p = anchor.next; p != &anchor && p->pos < pos; p = p->next)
if (p->line == linenum)

return;
nextp = p;
prevp = p->prev;

if (freelist != NULL)
{




new = freelist;
freelist = freelist->next;
} else
{




new = spare;
spare = NULL;
}





new->next = nextp;
new->prev = prevp;
new->pos = pos;
new->line = linenum;

nextp->prev = new;
prevp->next = new;




calcgap(new);
calcgap(nextp);
calcgap(prevp);

if (spare == NULL)
{









mingap = anchor.next->gap;
for (p = anchor.next; p->next != &anchor; p = p->next)
{
if (p->gap <= mingap)
{
spare = p;
mingap = p->gap;
}
}
spare->next->prev = spare->prev;
spare->prev->next = spare->next;
}
}





static void
longloopmessage(VOID_PARAM)
{
ierror("Calculating line numbers", NULL_PARG);
}

static int loopcount;
#if HAVE_TIME
static time_type startime;
#endif

static void
longish(VOID_PARAM)
{
#if HAVE_TIME
if (loopcount >= 0 && ++loopcount > 100)
{
loopcount = 0;
if (get_time() >= startime + LONGTIME)
{
longloopmessage();
loopcount = -1;
}
}
#else
if (loopcount >= 0 && ++loopcount > LONGLOOP)
{
longloopmessage();
loopcount = -1;
}
#endif
}





static void
abort_long(VOID_PARAM)
{
if (loopcount >= 0)
return;
if (linenums == OPT_ONPLUS)



screen_trashed = 1;
linenums = 0;
error("Line numbers turned off", NULL_PARG);
}





public LINENUM
find_linenum(pos)
POSITION pos;
{
struct linenum_info *p;
LINENUM linenum;
POSITION cpos;

if (!linenums)



return (0);
if (pos == NULL_POSITION)



return (0);
if (pos <= ch_zero())



return (1);




for (p = anchor.next; p != &anchor && p->pos < pos; p = p->next)
continue;
if (p->pos == pos)

return (p->line);












#if HAVE_TIME
startime = get_time();
#endif
loopcount = 0;
if (p == &anchor || pos - p->prev->pos < p->pos - pos)
{



p = p->prev;
if (ch_seek(p->pos))
return (0);
for (linenum = p->line, cpos = p->pos; cpos < pos; linenum++)
{



cpos = forw_raw_line(cpos, (char **)NULL, (int *)NULL);
if (ABORT_SIGS()) {
abort_long();
return (0);
}
if (cpos == NULL_POSITION)
return (0);
longish();
}



add_lnum(linenum, cpos);




if (cpos > pos)
linenum--;
} else
{



if (ch_seek(p->pos))
return (0);
for (linenum = p->line, cpos = p->pos; cpos > pos; linenum--)
{



cpos = back_raw_line(cpos, (char **)NULL, (int *)NULL);
if (ABORT_SIGS()) {
abort_long();
return (0);
}
if (cpos == NULL_POSITION)
return (0);
longish();
}



add_lnum(linenum, cpos);
}
loopcount = 0;
return (linenum);
}





public POSITION
find_pos(linenum)
LINENUM linenum;
{
struct linenum_info *p;
POSITION cpos;
LINENUM clinenum;

if (linenum <= 1)



return (ch_zero());




for (p = anchor.next; p != &anchor && p->line < linenum; p = p->next)
continue;
if (p->line == linenum)

return (p->pos);

if (p == &anchor || linenum - p->prev->line < p->line - linenum)
{



p = p->prev;
if (ch_seek(p->pos))
return (NULL_POSITION);
for (clinenum = p->line, cpos = p->pos; clinenum < linenum; clinenum++)
{



cpos = forw_raw_line(cpos, (char **)NULL, (int *)NULL);
if (ABORT_SIGS())
return (NULL_POSITION);
if (cpos == NULL_POSITION)
return (NULL_POSITION);
}
} else
{



if (ch_seek(p->pos))
return (NULL_POSITION);
for (clinenum = p->line, cpos = p->pos; clinenum > linenum; clinenum--)
{



cpos = back_raw_line(cpos, (char **)NULL, (int *)NULL);
if (ABORT_SIGS())
return (NULL_POSITION);
if (cpos == NULL_POSITION)
return (NULL_POSITION);
}
}



add_lnum(clinenum, cpos);
return (cpos);
}






public LINENUM
currline(where)
int where;
{
POSITION pos;
POSITION len;
LINENUM linenum;

pos = position(where);
len = ch_length();
while (pos == NULL_POSITION && where >= 0 && where < sc_height)
pos = position(++where);
if (pos == NULL_POSITION)
pos = len;
linenum = find_linenum(pos);
if (pos == len)
linenum--;
return (linenum);
}




public void
scan_eof(VOID_PARAM)
{
POSITION pos = 0;
LINENUM linenum = 0;

if (ch_seek(0))
return;
ierror("Determining length of file", NULL_PARG);
while (pos != NULL_POSITION)
{

if ((linenum++ % 256) == 0)
add_lnum(linenum, pos);
pos = forw_raw_line(pos, (char **)NULL, (int *)NULL);
if (ABORT_SIGS())
break;
}
}
