













#include "less.h"
#include "position.h"
#include "charset.h"

#define MINPOS(a,b) (((a) < (b)) ? (a) : (b))
#define MAXPOS(a,b) (((a) > (b)) ? (a) : (b))

extern int sigs;
extern int how_search;
extern int caseless;
extern int linenums;
extern int sc_height;
extern int jump_sline;
extern int bs_mode;
extern int ctldisp;
extern int status_col;
extern void *ml_search;
extern POSITION start_attnpos;
extern POSITION end_attnpos;
extern int utf_mode;
extern int screen_trashed;
extern int sc_width;
extern int sc_height;
extern int chopline;
extern int hshift;
#if HILITE_SEARCH
extern int hilite_search;
extern int size_linebuf;
extern int squished;
extern int can_goto_line;
static int hide_hilite;
static POSITION prep_startpos;
static POSITION prep_endpos;
extern POSITION xxpos;

















struct hilite
{
POSITION hl_startpos;
POSITION hl_endpos;
};
struct hilite_node
{
struct hilite_node *parent;
struct hilite_node *left;
struct hilite_node *right;
struct hilite_node *prev;
struct hilite_node *next;
int red;
struct hilite r;
};
struct hilite_storage
{
int capacity;
int used;
struct hilite_storage *next;
struct hilite_node *nodes;
};
struct hilite_tree
{
struct hilite_storage *first;
struct hilite_storage *current;
struct hilite_node *root;
struct hilite_node *lookaside;
};
#define HILITE_INITIALIZER() { NULL, NULL, NULL, NULL }
#define HILITE_LOOKASIDE_STEPS 2

static struct hilite_tree hilite_anchor = HILITE_INITIALIZER();
static struct hilite_tree filter_anchor = HILITE_INITIALIZER();
static struct pattern_info *filter_infos = NULL;

#endif





struct pattern_info {
PATTERN_TYPE compiled;
char* text;
int search_type;
struct pattern_info *next;
};

#if NO_REGEX
#define info_compiled(info) ((void*)0)
#else
#define info_compiled(info) ((info)->compiled)
#endif

static struct pattern_info search_info;
static int is_ucase_pattern;
static int is_caseless;




static int
is_ucase(str)
char *str;
{
char *str_end = str + strlen(str);
LWCHAR ch;

while (str < str_end)
{
ch = step_char(&str, +1, str_end);
if (IS_UPPER(ch))
return (1);
}
return (0);
}




static void
clear_pattern(info)
struct pattern_info *info;
{
if (info->text != NULL)
free(info->text);
info->text = NULL;
#if !NO_REGEX
uncompile_pattern(&info->compiled);
#endif
}




static int
set_pattern(info, pattern, search_type, show_error)
struct pattern_info *info;
char *pattern;
int search_type;
int show_error;
{
#if !NO_REGEX
if (pattern == NULL)
SET_NULL_PATTERN(info->compiled);
else if (compile_pattern(pattern, search_type, show_error, &info->compiled) < 0)
return -1;
#endif

if (info->text != NULL)
free(info->text);
info->text = NULL;
if (pattern != NULL)
{
info->text = (char *) ecalloc(1, strlen(pattern)+1);
strcpy(info->text, pattern);
}
info->search_type = search_type;





is_ucase_pattern = is_ucase(pattern);
if (is_ucase_pattern && caseless != OPT_ONPLUS)
is_caseless = 0;
else
is_caseless = caseless;
return 0;
}




static void
init_pattern(info)
struct pattern_info *info;
{
SET_NULL_PATTERN(info->compiled);
info->text = NULL;
info->search_type = 0;
info->next = NULL;
}




public void
init_search(VOID_PARAM)
{
init_pattern(&search_info);
}




static int
get_cvt_ops(VOID_PARAM)
{
int ops = 0;

if (is_caseless)
ops |= CVT_TO_LC;
if (bs_mode == BS_SPECIAL)
ops |= CVT_BS;
if (bs_mode != BS_CONTROL)
ops |= CVT_CRLF;
if (ctldisp == OPT_ONPLUS)
ops |= CVT_ANSI;
return (ops);
}




static int
prev_pattern(info)
struct pattern_info *info;
{
#if !NO_REGEX
if ((info->search_type & SRCH_NO_REGEX) == 0)
return (!is_null_pattern(info->compiled));
#endif
return (info->text != NULL);
}

#if HILITE_SEARCH





public void
repaint_hilite(on)
int on;
{
int sindex;
POSITION pos;
int save_hide_hilite;

if (squished)
repaint();

save_hide_hilite = hide_hilite;
if (!on)
{
if (hide_hilite)
return;
hide_hilite = 1;
}

if (!can_goto_line)
{
repaint();
hide_hilite = save_hide_hilite;
return;
}

for (sindex = TOP; sindex < TOP + sc_height-1; sindex++)
{
pos = position(sindex);
if (pos == NULL_POSITION)
continue;
(void) forw_line(pos);
goto_line(sindex);
put_line();
}
lower_left();
hide_hilite = save_hide_hilite;
}
#endif




public void
clear_attn(VOID_PARAM)
{
#if HILITE_SEARCH
int sindex;
POSITION old_start_attnpos;
POSITION old_end_attnpos;
POSITION pos;
POSITION epos;
int moved = 0;

if (start_attnpos == NULL_POSITION)
return;
old_start_attnpos = start_attnpos;
old_end_attnpos = end_attnpos;
start_attnpos = end_attnpos = NULL_POSITION;

if (!can_goto_line)
{
repaint();
return;
}
if (squished)
repaint();

for (sindex = TOP; sindex < TOP + sc_height-1; sindex++)
{
pos = position(sindex);
if (pos == NULL_POSITION)
continue;
epos = position(sindex+1);
if (pos <= old_end_attnpos &&
(epos == NULL_POSITION || epos > old_start_attnpos))
{
(void) forw_line(pos);
goto_line(sindex);
put_line();
moved = 1;
}
}
if (moved)
lower_left();
#endif
}




public void
undo_search(clear)
int clear;
{
clear_pattern(&search_info);
#if HILITE_SEARCH
if (clear)
{
clr_hilite();
} else
{
if (hilite_anchor.first == NULL)
{
error("No previous regular expression", NULL_PARG);
return;
}
hide_hilite = !hide_hilite;
}
repaint_hilite(1);
#endif
}

#if HILITE_SEARCH



public void
clr_hlist(anchor)
struct hilite_tree *anchor;
{
struct hilite_storage *hls;
struct hilite_storage *nexthls;

for (hls = anchor->first; hls != NULL; hls = nexthls)
{
nexthls = hls->next;
free((void*)hls->nodes);
free((void*)hls);
}
anchor->first = NULL;
anchor->current = NULL;
anchor->root = NULL;

anchor->lookaside = NULL;

prep_startpos = prep_endpos = NULL_POSITION;
}

public void
clr_hilite(VOID_PARAM)
{
clr_hlist(&hilite_anchor);
}

public void
clr_filter(VOID_PARAM)
{
clr_hlist(&filter_anchor);
}

struct hilite_node*
hlist_last(anchor)
struct hilite_tree *anchor;
{
struct hilite_node *n = anchor->root;
while (n != NULL && n->right != NULL)
n = n->right;
return n;
}

struct hilite_node*
hlist_next(n)
struct hilite_node *n;
{
return n->next;
}

struct hilite_node*
hlist_prev(n)
struct hilite_node *n;
{
return n->prev;
}







struct hilite_node*
hlist_find(anchor, pos)
struct hilite_tree *anchor;
POSITION pos;
{
struct hilite_node *n, *m;

if (anchor->lookaside)
{
int steps = 0;
int hit = 0;

n = anchor->lookaside;

for (;;)
{
if (pos < n->r.hl_endpos)
{
if (n->prev == NULL || pos >= n->prev->r.hl_endpos)
{
hit = 1;
break;
}
} else if (n->next == NULL)
{
n = NULL;
hit = 1;
break;
}





if (steps >= HILITE_LOOKASIDE_STEPS)
break;
steps++;

if (pos < n->r.hl_endpos)
anchor->lookaside = n = n->prev;
else
anchor->lookaside = n = n->next;
}

if (hit)
return n;
}

n = anchor->root;
m = NULL;

while (n != NULL)
{
if (pos < n->r.hl_startpos)
{
if (n->left != NULL)
{
m = n;
n = n->left;
continue;
}
break;
}
if (pos >= n->r.hl_endpos)
{
if (n->right != NULL)
{
n = n->right;
continue;
}
if (m != NULL)
{
n = m;
} else
{
m = n;
n = NULL;
}
}
break;
}

if (n != NULL)
anchor->lookaside = n;
else if (m != NULL)
anchor->lookaside = m;

return n;
}




static int
is_hilited_range(pos, epos)
POSITION pos;
POSITION epos;
{
struct hilite_node *n = hlist_find(&hilite_anchor, pos);
return (n != NULL && (epos == NULL_POSITION || epos > n->r.hl_startpos));
}




public int
is_filtered(pos)
POSITION pos;
{
struct hilite_node *n;

if (ch_getflags() & CH_HELPFILE)
return (0);

n = hlist_find(&filter_anchor, pos);
return (n != NULL && pos >= n->r.hl_startpos);
}





public POSITION
next_unfiltered(pos)
POSITION pos;
{
struct hilite_node *n;

if (ch_getflags() & CH_HELPFILE)
return (pos);

n = hlist_find(&filter_anchor, pos);
while (n != NULL && pos >= n->r.hl_startpos)
{
pos = n->r.hl_endpos;
n = n->next;
}
return (pos);
}





public POSITION
prev_unfiltered(pos)
POSITION pos;
{
struct hilite_node *n;

if (ch_getflags() & CH_HELPFILE)
return (pos);

n = hlist_find(&filter_anchor, pos);
while (n != NULL && pos >= n->r.hl_startpos)
{
pos = n->r.hl_startpos;
if (pos == 0)
break;
pos--;
n = n->prev;
}
return (pos);
}






public int
is_hilited_attr(pos, epos, nohide, p_matches)
POSITION pos;
POSITION epos;
int nohide;
int *p_matches;
{
int match;

if (p_matches != NULL)
*p_matches = 0;

if (!status_col &&
start_attnpos != NULL_POSITION &&
pos <= end_attnpos &&
(epos == NULL_POSITION || epos >= start_attnpos))



return (AT_HILITE|AT_COLOR_ATTN);

match = is_hilited_range(pos, epos);
if (!match)
return (0);

if (p_matches == NULL)





return (AT_HILITE|AT_COLOR_SEARCH);




*p_matches = 1;

if (hilite_search == 0)



return (0);

if (!nohide && hide_hilite)



return (0);

return (AT_HILITE|AT_COLOR_SEARCH);
}





static struct hilite_storage*
hlist_getstorage(anchor)
struct hilite_tree *anchor;
{
int capacity = 1;
struct hilite_storage *s;

if (anchor->current)
{
if (anchor->current->used < anchor->current->capacity)
return anchor->current;
capacity = anchor->current->capacity * 2;
}

s = (struct hilite_storage *) ecalloc(1, sizeof(struct hilite_storage));
s->nodes = (struct hilite_node *) ecalloc(capacity, sizeof(struct hilite_node));
s->capacity = capacity;
s->used = 0;
s->next = NULL;
if (anchor->current)
anchor->current->next = s;
else
anchor->first = s;
anchor->current = s;
return s;
}





static struct hilite_node*
hlist_getnode(anchor)
struct hilite_tree *anchor;
{
struct hilite_storage *s = hlist_getstorage(anchor);
return &s->nodes[s->used++];
}




static void
hlist_rotate_left(anchor, n)
struct hilite_tree *anchor;
struct hilite_node *n;
{
struct hilite_node *np = n->parent;
struct hilite_node *nr = n->right;
struct hilite_node *nrl = n->right->left;

if (np != NULL)
{
if (n == np->left)
np->left = nr;
else
np->right = nr;
} else
{
anchor->root = nr;
}
nr->left = n;
n->right = nrl;

nr->parent = np;
n->parent = nr;
if (nrl != NULL)
nrl->parent = n;
}




static void
hlist_rotate_right(anchor, n)
struct hilite_tree *anchor;
struct hilite_node *n;
{
struct hilite_node *np = n->parent;
struct hilite_node *nl = n->left;
struct hilite_node *nlr = n->left->right;

if (np != NULL)
{
if (n == np->right)
np->right = nl;
else
np->left = nl;
} else
{
anchor->root = nl;
}
nl->right = n;
n->left = nlr;

nl->parent = np;
n->parent = nl;
if (nlr != NULL)
nlr->parent = n;
}





static void
add_hilite(anchor, hl)
struct hilite_tree *anchor;
struct hilite *hl;
{
struct hilite_node *p, *n, *u;


if (hl->hl_startpos >= hl->hl_endpos)
return;

p = anchor->root;


if (p == NULL)
{
n = hlist_getnode(anchor);
n->r = *hl;
anchor->root = n;
anchor->lookaside = n;
return;
}






for (;;)
{
if (hl->hl_startpos < p->r.hl_startpos)
{
if (hl->hl_endpos > p->r.hl_startpos)
hl->hl_endpos = p->r.hl_startpos;
if (p->left != NULL)
{
p = p->left;
continue;
}
break;
}
if (hl->hl_startpos < p->r.hl_endpos) {
hl->hl_startpos = p->r.hl_endpos;
if (hl->hl_startpos >= hl->hl_endpos)
return;
}
if (p->right != NULL)
{
p = p->right;
continue;
}
break;
}






if (hl->hl_startpos < p->r.hl_startpos) {
if (hl->hl_endpos == p->r.hl_startpos)
{
p->r.hl_startpos = hl->hl_startpos;
return;
}
if (p->prev != NULL && p->prev->r.hl_endpos == hl->hl_startpos)
{
p->prev->r.hl_endpos = hl->hl_endpos;
return;
}

p->left = n = hlist_getnode(anchor);
n->next = p;
if (p->prev != NULL)
{
n->prev = p->prev;
p->prev->next = n;
}
p->prev = n;
} else {
if (p->r.hl_endpos == hl->hl_startpos)
{
p->r.hl_endpos = hl->hl_endpos;
return;
}
if (p->next != NULL && hl->hl_endpos == p->next->r.hl_startpos) {
p->next->r.hl_startpos = hl->hl_startpos;
return;
}

p->right = n = hlist_getnode(anchor);
n->prev = p;
if (p->next != NULL)
{
n->next = p->next;
p->next->prev = n;
}
p->next = n;
}
n->parent = p;
n->red = 1;
n->r = *hl;






for (;;)
{

if (n->parent == NULL)
{
n->red = 0;
break;
}


if (!n->parent->red)
break;











u = n->parent->parent->left;
if (n->parent == u)
u = n->parent->parent->right;
if (u != NULL && u->red)
{
n->parent->red = 0;
u->red = 0;
n = n->parent->parent;
n->red = 1;
continue;
}








if (n == n->parent->right &&
n->parent == n->parent->parent->left)
{
hlist_rotate_left(anchor, n->parent);
n = n->left;
} else if (n == n->parent->left &&
n->parent == n->parent->parent->right)
{
hlist_rotate_right(anchor, n->parent);
n = n->right;
}





n->parent->red = 0;
n->parent->parent->red = 1;
if (n == n->parent->left)
hlist_rotate_right(anchor, n->parent->parent);
else
hlist_rotate_left(anchor, n->parent->parent);
break;
}
}




static void
create_hilites(linepos, start_index, end_index, chpos)
POSITION linepos;
int start_index;
int end_index;
int *chpos;
{
struct hilite hl;
int i;


hl.hl_startpos = linepos + chpos[start_index];









for (i = start_index+1; i <= end_index; i++)
{
if (chpos[i] != chpos[i-1] + 1 || i == end_index)
{
hl.hl_endpos = linepos + chpos[i-1] + 1;
add_hilite(&hilite_anchor, &hl);

if (i < end_index)
{
hl.hl_startpos = linepos + chpos[i];
}
}
}
}






static void
hilite_line(linepos, line, line_len, chpos, sp, ep, cvt_ops)
POSITION linepos;
char *line;
int line_len;
int *chpos;
char *sp;
char *ep;
int cvt_ops;
{
char *searchp;
char *line_end = line + line_len;











searchp = line;
do {
if (sp == NULL || ep == NULL)
return;
create_hilites(linepos, sp-line, ep-line, chpos);





if (ep > searchp)
searchp = ep;
else if (searchp != line_end)
searchp++;
else
break;
} while (match_pattern(info_compiled(&search_info), search_info.text,
searchp, line_end - searchp, &sp, &ep, 1, search_info.search_type));
}
#endif

#if HILITE_SEARCH



static void
hilite_screen(VOID_PARAM)
{
struct scrpos scrpos;

get_scrpos(&scrpos, TOP);
if (scrpos.pos == NULL_POSITION)
return;
prep_hilite(scrpos.pos, position(BOTTOM_PLUS_ONE), -1);
repaint_hilite(1);
}




public void
chg_hilite(VOID_PARAM)
{



clr_hilite();
hide_hilite = 0;

if (hilite_search == OPT_ONPLUS)



hilite_screen();
}
#endif




static POSITION
search_pos(search_type)
int search_type;
{
POSITION pos;
int sindex;

if (empty_screen())
{







if (search_type & SRCH_FORW)
{
pos = ch_zero();
} else
{
pos = ch_length();
if (pos == NULL_POSITION)
{
(void) ch_end_seek();
pos = ch_length();
}
}
sindex = 0;
} else
{
int add_one = 0;

if (how_search == OPT_ON)
{



if (search_type & SRCH_FORW)
sindex = sc_height-1;
else
sindex = 0;
} else if (how_search == OPT_ONPLUS && !(search_type & SRCH_AFTER_TARGET))
{



if (search_type & SRCH_FORW)
sindex = 0;
else
sindex = sc_height-1;
} else
{





sindex = sindex_from_sline(jump_sline);
if (search_type & SRCH_FORW)
add_one = 1;
}
pos = position(sindex);
if (add_one)
pos = forw_raw_line(pos, (char **)NULL, (int *)NULL);
}




if (search_type & SRCH_FORW)
{
while (pos == NULL_POSITION)
{
if (++sindex >= sc_height)
break;
pos = position(sindex);
}
} else
{
while (pos == NULL_POSITION)
{
if (--sindex < 0)
break;
pos = position(sindex);
}
}
return (pos);
}





#if HILITE_SEARCH
static int
matches_filters(pos, cline, line_len, chpos, linepos, sp, ep)
POSITION pos;
char *cline;
int line_len;
int *chpos;
POSITION linepos;
char **sp;
char **ep;
{
struct pattern_info *filter;

for (filter = filter_infos; filter != NULL; filter = filter->next)
{
int line_filter = match_pattern(info_compiled(filter), filter->text,
cline, line_len, sp, ep, 0, filter->search_type);
if (line_filter)
{
struct hilite hl;
hl.hl_startpos = linepos;
hl.hl_endpos = pos;
add_hilite(&filter_anchor, &hl);
free(cline);
free(chpos);
return (1);
}
}
return (0);
}
#endif





static POSITION
get_lastlinepos(pos, tpos, sheight)
POSITION pos;
POSITION tpos;
int sheight;
{
int nlines;

for (nlines = 0;; nlines++)
{
POSITION npos = forw_line(pos);
if (npos > tpos)
{
if (nlines < sheight)
return NULL_POSITION;
return pos;
}
pos = npos;
}
}





static int
get_seg(pos, tpos)
POSITION pos;
POSITION tpos;
{
int seg;

for (seg = 0;; seg++)
{
POSITION npos = forw_line_seg(pos, TRUE);
if (npos > tpos)
return seg;
pos = npos;
}
}




static int
search_range(pos, endpos, search_type, matches, maxlines, plinepos, pendpos, plastlinepos)
POSITION pos;
POSITION endpos;
int search_type;
int matches;
int maxlines;
POSITION *plinepos;
POSITION *pendpos;
POSITION *plastlinepos;
{
char *line;
char *cline;
int line_len;
LINENUM linenum;
char *sp, *ep;
int line_match;
int cvt_ops;
int cvt_len;
int *chpos;
POSITION linepos, oldpos;
int swidth = sc_width - line_pfx_width();
int sheight = sc_height - sindex_from_sline(jump_sline);

linenum = find_linenum(pos);
oldpos = pos;

if ((search_type & SRCH_WRAP) && endpos == NULL_POSITION)
endpos = pos;
for (;;)
{





if (ABORT_SIGS())
{



return (-1);
}

if ((endpos != NULL_POSITION && !(search_type & SRCH_WRAP) &&
(((search_type & SRCH_FORW) && pos >= endpos) ||
((search_type & SRCH_BACK) && pos <= endpos))) || maxlines == 0)
{



if (pendpos != NULL)
*pendpos = pos;
return (matches);
}
if (maxlines > 0)
maxlines--;

if (search_type & SRCH_FORW)
{




linepos = pos;
pos = forw_raw_line(pos, &line, &line_len);
if (linenum != 0)
linenum++;
} else
{




pos = back_raw_line(pos, &line, &line_len);
linepos = pos;
if (linenum != 0)
linenum--;
}

if (pos == NULL_POSITION)
{



if (search_type & SRCH_WRAP)
{




if (search_type & SRCH_FORW)
{
pos = ch_zero();
} else
{
pos = ch_length();
if (pos == NULL_POSITION)
{
(void) ch_end_seek();
pos = ch_length();
}
}
if (pos != NULL_POSITION) {





search_type &= ~SRCH_WRAP;
linenum = find_linenum(pos);
continue;
}
}
if (pendpos != NULL)
*pendpos = oldpos;
return (matches);
}









if (linenums && abs((int)(pos - oldpos)) > 2048)
add_lnum(linenum, pos);
oldpos = pos;

#if HILITE_SEARCH
if (is_filtered(linepos))
continue;
#endif





cvt_ops = get_cvt_ops();
cvt_len = cvt_length(line_len, cvt_ops);
cline = (char *) ecalloc(1, cvt_len);
chpos = cvt_alloc_chpos(cvt_len);
cvt_text(cline, line, chpos, &line_len, cvt_ops);

#if HILITE_SEARCH



if (filter_infos != NULL &&
((search_type & SRCH_FIND_ALL) ||
prep_startpos == NULL_POSITION ||
linepos < prep_startpos || linepos >= prep_endpos)) {
if (matches_filters(pos, cline, line_len, chpos, linepos, &sp, &ep))
continue;
}
#endif






if (prev_pattern(&search_info))
{
line_match = match_pattern(info_compiled(&search_info), search_info.text,
cline, line_len, &sp, &ep, 0, search_type);
if (line_match)
{



if (search_type & SRCH_FIND_ALL)
{
#if HILITE_SEARCH





hilite_line(linepos, cline, line_len, chpos, sp, ep, cvt_ops);
#endif
} else if (--matches <= 0)
{




#if HILITE_SEARCH
if (hilite_search == OPT_ON)
{




clr_hilite();
hilite_line(linepos, cline, line_len, chpos, sp, ep, cvt_ops);
}
#endif
if (chopline)
{




if (sp != NULL && ep != NULL)
{
int start_off = sp - cline;
int end_off = ep - cline;
int save_hshift = hshift;
int sshift;
int eshift;
hshift = 0;
chopline = FALSE;
sshift = swidth * get_seg(linepos, linepos + chpos[start_off]);
eshift = swidth * get_seg(linepos, linepos + chpos[end_off]);
chopline = TRUE;
if (sshift >= save_hshift && eshift <= save_hshift)
{
hshift = save_hshift;
} else
{
hshift = sshift;
screen_trashed = 1;
}
}
} else if (plastlinepos != NULL)
{








if (ep != NULL)
{
int end_off = ep - cline;
if (end_off >= swidth * sheight / 4)
*plastlinepos = get_lastlinepos(linepos, linepos + chpos[end_off], sheight);
}
}
free(cline);
free(chpos);
if (plinepos != NULL)
*plinepos = linepos;
return (0);
}
}
}
free(cline);
free(chpos);
}
}




static int
hist_pattern(search_type)
int search_type;
{
#if CMD_HISTORY
char *pattern;

set_mlist(ml_search, 0);
pattern = cmd_lastpattern();
if (pattern == NULL)
return (0);

if (set_pattern(&search_info, pattern, search_type, 1) < 0)
return (-1);

#if HILITE_SEARCH
if (hilite_search == OPT_ONPLUS && !hide_hilite)
hilite_screen();
#endif

return (1);
#else
return (0);
#endif
}





public void
chg_caseless(VOID_PARAM)
{
if (!is_ucase_pattern)




is_caseless = caseless;
else
{




clear_pattern(&search_info);
(void) hist_pattern(search_info.search_type);
}
}










public int
search(search_type, pattern, n)
int search_type;
char *pattern;
int n;
{
POSITION pos;
POSITION opos;
POSITION lastlinepos = NULL_POSITION;

if (pattern == NULL || *pattern == '\0')
{



search_type |= SRCH_AFTER_TARGET;
if (!prev_pattern(&search_info))
{
int r = hist_pattern(search_type);
if (r == 0)
error("No previous regular expression", NULL_PARG);
if (r <= 0)
return (-1);
}
if ((search_type & SRCH_NO_REGEX) !=
(search_info.search_type & SRCH_NO_REGEX))
{
error("Please re-enter search pattern", NULL_PARG);
return -1;
}
#if HILITE_SEARCH
if (hilite_search == OPT_ON || status_col)
{




repaint_hilite(0);
}
if (hilite_search == OPT_ONPLUS && hide_hilite)
{




hide_hilite = 0;
hilite_screen();
}
hide_hilite = 0;
#endif
} else
{



int show_error = !(search_type & SRCH_INCR);
if (set_pattern(&search_info, pattern, search_type, show_error) < 0)
return (-1);
#if HILITE_SEARCH
if (hilite_search || status_col)
{




repaint_hilite(0);
hide_hilite = 0;
clr_hilite();
}
if (hilite_search == OPT_ONPLUS || status_col)
{




hilite_screen();
}
#endif
}




pos = search_pos(search_type);
opos = position(sindex_from_sline(jump_sline));
if (pos == NULL_POSITION)
{



if (search_type & SRCH_PAST_EOF)
return (n);
#if HILITE_SEARCH
if (hilite_search == OPT_ON || status_col)
repaint_hilite(1);
#endif
error("Nothing to search", NULL_PARG);
return (-1);
}

n = search_range(pos, NULL_POSITION, search_type, n, -1,
&pos, (POSITION*)NULL, &lastlinepos);
if (n != 0)
{



#if HILITE_SEARCH
if ((hilite_search == OPT_ON || status_col) && n > 0)



repaint_hilite(1);
#endif
return (n);
}

if (!(search_type & SRCH_NO_MOVE))
{



if (lastlinepos != NULL_POSITION)
jump_loc(lastlinepos, BOTTOM);
else if (pos != opos)
jump_loc(pos, jump_sline);
}

#if HILITE_SEARCH
if (hilite_search == OPT_ON || status_col)



repaint_hilite(1);
#endif
return (0);
}

#if HILITE_SEARCH










public void
prep_hilite(spos, epos, maxlines)
POSITION spos;
POSITION epos;
int maxlines;
{
POSITION nprep_startpos = prep_startpos;
POSITION nprep_endpos = prep_endpos;
POSITION new_epos;
POSITION max_epos;
int result;
int i;





#define SEARCH_MORE (3*size_linebuf)

if (!prev_pattern(&search_info) && !is_filtering())
return;





spos = back_raw_line(spos+1, (char **)NULL, (int *)NULL);





if (maxlines < 0)
max_epos = NULL_POSITION;
else
{
max_epos = spos;
for (i = 0; i < maxlines; i++)
max_epos = forw_raw_line(max_epos, (char **)NULL, (int *)NULL);
}







if (prep_startpos == NULL_POSITION ||
(epos != NULL_POSITION && epos < prep_startpos) ||
spos > prep_endpos)
{




clr_hilite();
clr_filter();
if (epos != NULL_POSITION)
epos += SEARCH_MORE;
nprep_startpos = spos;
} else
{



if (epos == NULL_POSITION)
{



;
} else if (epos > prep_endpos)
{




epos += SEARCH_MORE;
} else
{




epos = prep_startpos;
}

if (spos < prep_startpos)
{





if (spos < SEARCH_MORE)
spos = 0;
else
spos -= SEARCH_MORE;
nprep_startpos = spos;
} else
{




spos = prep_endpos;
}
}

if (epos != NULL_POSITION && max_epos != NULL_POSITION &&
epos > max_epos)



epos = max_epos;

if (epos == NULL_POSITION || epos > spos)
{
int search_type = SRCH_FORW | SRCH_FIND_ALL;
search_type |= (search_info.search_type & SRCH_NO_REGEX);
for (;;)
{
result = search_range(spos, epos, search_type, 0, maxlines, (POSITION*)NULL, &new_epos, (POSITION*)NULL);
if (result < 0)
return;
if (prep_endpos == NULL_POSITION || new_epos > prep_endpos)
nprep_endpos = new_epos;







if (prep_endpos == NULL_POSITION || nprep_endpos > prep_endpos)
{
if (new_epos >= nprep_endpos && is_filtered(new_epos-1))
{
spos = nprep_endpos;
epos = forw_raw_line(nprep_endpos, (char **)NULL, (int *)NULL);
if (epos == NULL_POSITION)
break;
maxlines = 1;
continue;
}
}

if (prep_startpos == NULL_POSITION || nprep_startpos < prep_startpos)
{
if (nprep_startpos > 0 && is_filtered(nprep_startpos))
{
epos = nprep_startpos;
spos = back_raw_line(nprep_startpos, (char **)NULL, (int *)NULL);
if (spos == NULL_POSITION)
break;
nprep_startpos = spos;
maxlines = 1;
continue;
}
}
break;
}
}
prep_startpos = nprep_startpos;
prep_endpos = nprep_endpos;
}




public void
set_filter_pattern(pattern, search_type)
char *pattern;
int search_type;
{
struct pattern_info *filter;

clr_filter();
if (pattern == NULL || *pattern == '\0')
{

for (filter = filter_infos; filter != NULL; )
{
struct pattern_info *next_filter = filter->next;
clear_pattern(filter);
free(filter);
filter = next_filter;
}
filter_infos = NULL;
} else
{

filter = ecalloc(1, sizeof(struct pattern_info));
init_pattern(filter);
set_pattern(filter, pattern, search_type, 1);
filter->next = filter_infos;
filter_infos = filter;
}
screen_trashed = 1;
}




public int
is_filtering(VOID_PARAM)
{
if (ch_getflags() & CH_HELPFILE)
return (0);
return (filter_infos != NULL);
}
#endif

#if HAVE_V8_REGCOMP




public int reg_show_error = 1;

void
regerror(s)
char *s;
{
PARG parg;

if (!reg_show_error)
return;
parg.p_string = s;
error("%s", &parg);
}
#endif

