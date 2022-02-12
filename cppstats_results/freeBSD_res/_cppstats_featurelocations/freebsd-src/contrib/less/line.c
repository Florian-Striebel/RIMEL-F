














#include "less.h"
#include "charset.h"
#include "position.h"

#if MSDOS_COMPILER==WIN32C
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define MAX_PFX_WIDTH (MAX_LINENUM_WIDTH + MAX_STATUSCOL_WIDTH + 1)
static struct {
char *buf;
int *attr;
int print;
int end;
char pfx[MAX_PFX_WIDTH];
int pfx_attr[MAX_PFX_WIDTH];
int pfx_end;
} linebuf;

struct xbuffer shifted_ansi;
struct xbuffer last_ansi;

public int size_linebuf = 0;
static struct ansi_state *line_ansi = NULL;
static int cshift;
public int hshift;
public int tabstops[TABSTOP_MAX] = { 0 };
public int ntabstops = 1;
public int tabdefault = 8;
public POSITION highest_hilite;

static int end_column;
static int right_curr;
static int right_column;
static int overstrike;
static int last_overstrike = AT_NORMAL;
static int is_null_line;
static LWCHAR pendc;
static POSITION pendpos;
static char *end_ansi_chars;
static char *mid_ansi_chars;
static int in_hilite;

static int attr_swidth LESSPARAMS ((int a));
static int attr_ewidth LESSPARAMS ((int a));
static int do_append LESSPARAMS ((LWCHAR ch, char *rep, POSITION pos));

extern int sigs;
extern int bs_mode;
extern int linenums;
extern int ctldisp;
extern int twiddle;
extern int binattr;
extern int status_col;
extern int status_col_width;
extern int linenum_width;
extern int auto_wrap, ignaw;
extern int bo_s_width, bo_e_width;
extern int ul_s_width, ul_e_width;
extern int bl_s_width, bl_e_width;
extern int so_s_width, so_e_width;
extern int sc_width, sc_height;
extern int utf_mode;
extern POSITION start_attnpos;
extern POSITION end_attnpos;
extern char rscroll_char;
extern int rscroll_attr;
extern int use_color;

static char mbc_buf[MAX_UTF_CHAR_LEN];
static int mbc_buf_len = 0;
static int mbc_buf_index = 0;
static POSITION mbc_pos;


static char color_map[AT_NUM_COLORS][12] = {
"Wm",
"kR",
"kR",
"kY",
"c",
"Wb",
"kC",
"kc",
"kG",
"",
"",
"",
"",
};


struct ansi_state {
int hindex;
int hlink;
int prev_esc;
};




public void
init_line(VOID_PARAM)
{
end_ansi_chars = lgetenv("LESSANSIENDCHARS");
if (isnullenv(end_ansi_chars))
end_ansi_chars = "m";

mid_ansi_chars = lgetenv("LESSANSIMIDCHARS");
if (isnullenv(mid_ansi_chars))
mid_ansi_chars = "0123456789:;[?!\"'#%()*+ ";

linebuf.buf = (char *) ecalloc(LINEBUF_SIZE, sizeof(char));
linebuf.attr = (int *) ecalloc(LINEBUF_SIZE, sizeof(int));
size_linebuf = LINEBUF_SIZE;
xbuf_init(&shifted_ansi);
xbuf_init(&last_ansi);
}




static int
expand_linebuf(VOID_PARAM)
{

int new_size = size_linebuf * 2;


#if HAVE_REALLOC
char *new_buf = (char *) realloc(linebuf.buf, new_size);
int *new_attr = (int *) realloc(linebuf.attr, new_size*sizeof(int));
#else
char *new_buf = (char *) calloc(new_size, sizeof(char));
int *new_attr = (int *) calloc(new_size, sizeof(int));
#endif
if (new_buf == NULL || new_attr == NULL)
{
if (new_attr != NULL)
free(new_attr);
if (new_buf != NULL)
free(new_buf);
return 1;
}
#if !HAVE_REALLOC



memcpy(new_buf, linebuf.buf, size_linebuf * sizeof(char));
memcpy(new_attr, linebuf.attr, size_linebuf * sizeof(int));
free(linebuf.attr);
free(linebuf.buf);
#endif
linebuf.buf = new_buf;
linebuf.attr = new_attr;
size_linebuf = new_size;
return 0;
}




public int
is_ascii_char(ch)
LWCHAR ch;
{
return (ch <= 0x7F);
}



static void
inc_end_column(w)
int w;
{
if (end_column > right_column && w > 0)
{
right_column = end_column;
right_curr = linebuf.end;
}
end_column += w;
}




public void
prewind(VOID_PARAM)
{
linebuf.print = 6;
linebuf.pfx_end = 0;
for (linebuf.end = 0; linebuf.end < linebuf.print; linebuf.end++)
{
linebuf.buf[linebuf.end] = '\0';
linebuf.attr[linebuf.end] = 0;
}

end_column = 0;
right_curr = 0;
right_column = 0;
cshift = 0;
overstrike = 0;
last_overstrike = AT_NORMAL;
mbc_buf_len = 0;
is_null_line = 0;
pendc = '\0';
in_hilite = 0;
xbuf_reset(&shifted_ansi);
xbuf_reset(&last_ansi);
}




static void
set_linebuf(n, ch, attr)
int n;
char ch;
int attr;
{
linebuf.buf[n] = ch;
linebuf.attr[n] = attr;
}




static void
add_linebuf(ch, attr, w)
char ch;
int attr;
int w;
{
set_linebuf(linebuf.end++, ch, attr);
inc_end_column(w);
}




static void
set_pfx(n, ch, attr)
int n;
char ch;
int attr;
{
linebuf.pfx[n] = ch;
linebuf.pfx_attr[n] = attr;
}




static void
add_pfx(ch, attr)
char ch;
int attr;
{
set_pfx(linebuf.pfx_end++, ch, attr);
}




public void
plinestart(pos)
POSITION pos;
{
LINENUM linenum = 0;
int i;

if (linenums == OPT_ONPLUS)
{








linenum = find_linenum(pos);
}




if (status_col)
{
int a = AT_NORMAL;
char c = posmark(pos);
if (c != 0)
a |= AT_HILITE|AT_COLOR_MARK;
else
{
c = ' ';
if (start_attnpos != NULL_POSITION &&
pos >= start_attnpos && pos <= end_attnpos)
a |= AT_HILITE|AT_COLOR_ATTN;
}
add_pfx(c, a);
while (linebuf.pfx_end < status_col_width)
add_pfx(' ', AT_NORMAL);
}





if (linenums == OPT_ONPLUS)
{
char buf[INT_STRLEN_BOUND(linenum) + 2];
int len;

linenumtoa(linenum, buf);
len = (int) strlen(buf);
for (i = 0; i < linenum_width - len; i++)
add_pfx(' ', AT_NORMAL);
for (i = 0; i < len; i++)
add_pfx(buf[i], AT_NORMAL|AT_COLOR_LINENUM);
add_pfx(' ', AT_NORMAL);
}
end_column = linebuf.pfx_end;
}





public int
line_pfx_width(VOID_PARAM)
{
int width = 0;
if (status_col)
width += status_col_width;
if (linenums == OPT_ONPLUS)
width += linenum_width + 1;
return width;
}





public void
pshift_all(VOID_PARAM)
{
int i;
for (i = linebuf.print; i < linebuf.end; i++)
if (linebuf.attr[i] == AT_ANSI)
xbuf_add(&shifted_ansi, linebuf.buf[i]);
linebuf.end = linebuf.print;
end_column = linebuf.pfx_end;
}





static int
attr_swidth(a)
int a;
{
int w = 0;

a = apply_at_specials(a);

if (a & AT_UNDERLINE)
w += ul_s_width;
if (a & AT_BOLD)
w += bo_s_width;
if (a & AT_BLINK)
w += bl_s_width;
if (a & AT_STANDOUT)
w += so_s_width;

return w;
}





static int
attr_ewidth(a)
int a;
{
int w = 0;

a = apply_at_specials(a);

if (a & AT_UNDERLINE)
w += ul_e_width;
if (a & AT_BOLD)
w += bo_e_width;
if (a & AT_BLINK)
w += bl_e_width;
if (a & AT_STANDOUT)
w += so_e_width;

return w;
}







public int
pwidth(ch, a, prev_ch, prev_a)
LWCHAR ch;
int a;
LWCHAR prev_ch;
int prev_a;
{
int w;

if (ch == '\b')
{



if (prev_a & (AT_ANSI|AT_BINARY))
return strlen(prchar('\b'));
return (utf_mode && is_wide_char(prev_ch)) ? -2 : -1;
}

if (!utf_mode || is_ascii_char(ch))
{
if (control_char((char)ch))
{





return (0);
}
} else
{
if (is_composing_char(ch) || is_combining_char(prev_ch, ch))
{











return (0);
}
}





w = 1;
if (is_wide_char(ch))
w++;
if (linebuf.end > 0 && !is_at_equiv(linebuf.attr[linebuf.end-1], a))
w += attr_ewidth(linebuf.attr[linebuf.end-1]);
if (apply_at_specials(a) != AT_NORMAL &&
(linebuf.end == 0 || !is_at_equiv(linebuf.attr[linebuf.end-1], a)))
w += attr_swidth(a);
return (w);
}




static int
backc(VOID_PARAM)
{
LWCHAR ch;
char *p;

if (linebuf.end == 0)
return (0);
p = &linebuf.buf[linebuf.end];
ch = step_char(&p, -1, linebuf.buf);

while (p > linebuf.buf)
{
LWCHAR prev_ch;
int width;
linebuf.end = (int) (p - linebuf.buf);
prev_ch = step_char(&p, -1, linebuf.buf);
width = pwidth(ch, linebuf.attr[linebuf.end], prev_ch, linebuf.attr[linebuf.end-1]);
end_column -= width;

if (width > 0)
break;
ch = prev_ch;
}
return (1);
}




public int
is_ansi_end(ch)
LWCHAR ch;
{
if (!is_ascii_char(ch))
return (0);
return (strchr(end_ansi_chars, (char) ch) != NULL);
}




public int
is_ansi_middle(ch)
LWCHAR ch;
{
if (!is_ascii_char(ch))
return (0);
if (is_ansi_end(ch))
return (0);
return (strchr(mid_ansi_chars, (char) ch) != NULL);
}





public void
skip_ansi(pansi, pp, limit)
struct ansi_state *pansi;
char **pp;
constant char *limit;
{
LWCHAR c;
do {
c = step_char(pp, +1, limit);
} while (*pp < limit && ansi_step(pansi, c) == ANSI_MID);

}





public struct ansi_state *
ansi_start(ch)
LWCHAR ch;
{
struct ansi_state *pansi;

if (!IS_CSI_START(ch))
return NULL;
pansi = ecalloc(1, sizeof(struct ansi_state));
pansi->hindex = 0;
pansi->hlink = 0;
pansi->prev_esc = 0;
return pansi;
}





public int
ansi_step(pansi, ch)
struct ansi_state *pansi;
LWCHAR ch;
{
if (pansi->hlink)
{

if (ch == '\7')
return ANSI_END;
if (pansi->prev_esc && ch == '\\')
return ANSI_END;
pansi->prev_esc = (ch == ESC);
return ANSI_MID;
}
if (pansi->hindex >= 0)
{
static char hlink_prefix[] = ESCS "]8;";
if (ch == hlink_prefix[pansi->hindex] ||
(pansi->hindex == 0 && IS_CSI_START(ch)))
{
pansi->hindex++;
if (hlink_prefix[pansi->hindex] == '\0')
pansi->hlink = 1;
return ANSI_MID;
}
pansi->hindex = -1;
}

if (is_ansi_middle(ch))
return ANSI_MID;
if (is_ansi_end(ch))
return ANSI_END;
return ANSI_ERR;
}




public void
ansi_done(pansi)
struct ansi_state *pansi;
{
free(pansi);
}




#define STORE_CHAR(ch,a,rep,pos) do { if (store_char((ch),(a),(rep),(pos))) return (1); } while (0)




static int
store_char(ch, a, rep, pos)
LWCHAR ch;
int a;
char *rep;
POSITION pos;
{
int w;
int i;
int replen;
char cs;

i = (a & (AT_UNDERLINE|AT_BOLD));
if (i != AT_NORMAL)
last_overstrike = i;

#if HILITE_SEARCH
{
int matches;
int resend_last = 0;
int hl_attr = is_hilited_attr(pos, pos+1, 0, &matches);
if (hl_attr)
{




if (a != AT_ANSI)
{
if (highest_hilite != NULL_POSITION && pos > highest_hilite)
highest_hilite = pos;
a |= hl_attr;
}
in_hilite = 1;
} else
{
if (in_hilite)
{




resend_last = 1;
}
in_hilite = 0;
}
if (resend_last)
{
for (i = 0; i < last_ansi.end; i++)
STORE_CHAR(last_ansi.data[i], AT_ANSI, NULL, pos);
}
}
#endif

if (a == AT_ANSI) {
w = 0;
} else {
char *p = &linebuf.buf[linebuf.end];
LWCHAR prev_ch = (linebuf.end > 0) ? step_char(&p, -1, linebuf.buf) : 0;
int prev_a = (linebuf.end > 0) ? linebuf.attr[linebuf.end-1] : 0;
w = pwidth(ch, a, prev_ch, prev_a);
}

if (ctldisp != OPT_ON && end_column - cshift + w + attr_ewidth(a) > sc_width)



return (1);

if (rep == NULL)
{
cs = (char) ch;
rep = &cs;
replen = 1;
} else
{
replen = utf_len(rep[0]);
}
if (linebuf.end + replen >= size_linebuf-6)
{




if (expand_linebuf())
return (1);
}

if (cshift == hshift && shifted_ansi.end > 0)
{

for (i = 0; i < shifted_ansi.end; i++)
add_linebuf(shifted_ansi.data[i], AT_ANSI, 0);
xbuf_reset(&shifted_ansi);
}

inc_end_column(w);
for (i = 0; i < replen; i++)
add_linebuf(*rep++, a, 0);

if (cshift < hshift)
{

if (a == AT_ANSI)
xbuf_add(&shifted_ansi, ch);
if (linebuf.end > linebuf.print)
{

int i;
for (i = 0; i < linebuf.print; i++)
{
linebuf.buf[i] = linebuf.buf[i+replen];
linebuf.attr[i] = linebuf.attr[i+replen];
}
linebuf.end -= replen;
cshift += w;





while (cshift > hshift)
{
add_linebuf(' ', rscroll_attr, 0);
cshift--;
}
}
}
return (0);
}





#define STORE_TAB(a,pos) do { if (store_tab((a),(pos))) return (1); } while (0)


static int
store_tab(attr, pos)
int attr;
POSITION pos;
{
int to_tab = end_column - linebuf.pfx_end;

if (ntabstops < 2 || to_tab >= tabstops[ntabstops-1])
to_tab = tabdefault -
((to_tab - tabstops[ntabstops-1]) % tabdefault);
else
{
int i;
for (i = ntabstops - 2; i >= 0; i--)
if (to_tab >= tabstops[i])
break;
to_tab = tabstops[i+1] - to_tab;
}

do {
STORE_CHAR(' ', attr, " ", pos);
} while (--to_tab > 0);
return 0;
}

#define STORE_PRCHAR(c, pos) do { if (store_prchar((c), (pos))) return 1; } while (0)


static int
store_prchar(c, pos)
LWCHAR c;
POSITION pos;
{
char *s;




s = prchar(c);
for ( ; *s != 0; s++)
STORE_CHAR(*s, AT_BINARY|AT_COLOR_CTRL, NULL, pos);
return 0;
}

static int
flush_mbc_buf(pos)
POSITION pos;
{
int i;

for (i = 0; i < mbc_buf_index; i++)
if (store_prchar(mbc_buf[i], pos))
return mbc_buf_index - i;
return 0;
}






public int
pappend(c, pos)
int c;
POSITION pos;
{
int r;

if (pendc)
{
if (c == '\r' && pendc == '\r')
return (0);
if (do_append(pendc, NULL, pendpos))




return (1);
pendc = '\0';
}

if (c == '\r' && bs_mode == BS_SPECIAL)
{
if (mbc_buf_len > 0)
{

r = flush_mbc_buf(mbc_pos);
mbc_buf_index = r + 1;
mbc_buf_len = 0;
if (r)
return (mbc_buf_index);
}






pendc = c;
pendpos = pos;
return (0);
}

if (!utf_mode)
{
r = do_append(c, NULL, pos);
} else
{

if (mbc_buf_len == 0)
{
retry:
mbc_buf_index = 1;
*mbc_buf = c;
if (IS_ASCII_OCTET(c))
r = do_append(c, NULL, pos);
else if (IS_UTF8_LEAD(c))
{
mbc_buf_len = utf_len(c);
mbc_pos = pos;
return (0);
} else

r = flush_mbc_buf(pos);
} else if (IS_UTF8_TRAIL(c))
{
mbc_buf[mbc_buf_index++] = c;
if (mbc_buf_index < mbc_buf_len)
return (0);
if (is_utf8_well_formed(mbc_buf, mbc_buf_index))
r = do_append(get_wchar(mbc_buf), mbc_buf, mbc_pos);
else

mbc_buf_index = r = flush_mbc_buf(mbc_pos);
mbc_buf_len = 0;
} else
{

r = flush_mbc_buf(mbc_pos);
mbc_buf_index = r + 1;
mbc_buf_len = 0;

if (!r)
goto retry;
}
}
if (r)
{

r = (!utf_mode) ? 1 : mbc_buf_index;
}
return (r);
}

static int
store_control_char(ch, rep, pos)
LWCHAR ch;
char *rep;
POSITION pos;
{
if (ctldisp == OPT_ON)
{

STORE_CHAR(ch, AT_NORMAL, rep, pos);
} else
{

STORE_PRCHAR((char) ch, pos);
}
return (0);
}

static int
store_ansi(ch, rep, pos)
LWCHAR ch;
char *rep;
POSITION pos;
{
switch (ansi_step(line_ansi, ch))
{
case ANSI_MID:
if (!in_hilite)
STORE_CHAR(ch, AT_ANSI, rep, pos);
break;
case ANSI_END:
if (!in_hilite)
STORE_CHAR(ch, AT_ANSI, rep, pos);
ansi_done(line_ansi);
line_ansi = NULL;
break;
case ANSI_ERR: {

char *start = (cshift < hshift) ? shifted_ansi.data : linebuf.buf;
int *end = (cshift < hshift) ? &shifted_ansi.end : &linebuf.end;
char *p = start + *end;
LWCHAR bch;
do {
bch = step_char(&p, -1, start);
} while (p > start && !IS_CSI_START(bch));
*end = (int) (p - start);
ansi_done(line_ansi);
line_ansi = NULL;
break; }
}
return (0);
}

static int
store_bs(ch, rep, pos)
LWCHAR ch;
char *rep;
POSITION pos;
{
if (bs_mode == BS_CONTROL)
return store_control_char(ch, rep, pos);
if (linebuf.end > 0 &&
((linebuf.end <= linebuf.print && linebuf.buf[linebuf.end-1] == '\0') ||
(linebuf.end > 0 && linebuf.attr[linebuf.end - 1] & (AT_ANSI|AT_BINARY))))
STORE_PRCHAR('\b', pos);
else if (bs_mode == BS_NORMAL)
STORE_CHAR(ch, AT_NORMAL, NULL, pos);
else if (bs_mode == BS_SPECIAL)
overstrike = backc();
return 0;
}

static int
do_append(ch, rep, pos)
LWCHAR ch;
char *rep;
POSITION pos;
{
int a = AT_NORMAL;

if (ctldisp == OPT_ONPLUS && line_ansi == NULL)
{
line_ansi = ansi_start(ch);
if (line_ansi != NULL)
xbuf_reset(&last_ansi);
}

if (line_ansi != NULL)
{
xbuf_add(&last_ansi, ch);
return store_ansi(ch, rep, pos);
}

if (ch == '\b')
return store_bs(ch, rep, pos);

if (overstrike > 0)
{







LWCHAR prev_ch;
overstrike = utf_mode ? -1 : 0;
if (utf_mode)
{

prev_ch = get_wchar(&linebuf.buf[linebuf.end]);
} else
{
prev_ch = (unsigned char) linebuf.buf[linebuf.end];
}
a = linebuf.attr[linebuf.end];
if (ch == prev_ch)
{







if (ch == '_')
{
if ((a & (AT_BOLD|AT_UNDERLINE)) != AT_NORMAL)
a |= (AT_BOLD|AT_UNDERLINE);
else if (last_overstrike != AT_NORMAL)
a |= last_overstrike;
else
a |= AT_BOLD;
} else
a |= AT_BOLD;
} else if (ch == '_')
{
a |= AT_UNDERLINE;
ch = prev_ch;
rep = &linebuf.buf[linebuf.end];
} else if (prev_ch == '_')
{
a |= AT_UNDERLINE;
}

} else if (overstrike < 0)
{
if ( is_composing_char(ch)
|| is_combining_char(get_wchar(&linebuf.buf[linebuf.end]), ch))

a = last_overstrike;
else
overstrike = 0;
}

if (ch == '\t')
{



switch (bs_mode)
{
case BS_CONTROL:
return store_control_char(ch, rep, pos);
case BS_NORMAL:
case BS_SPECIAL:
STORE_TAB(a, pos);
break;
}
return (0);
}
if ((!utf_mode || is_ascii_char(ch)) && control_char((char)ch))
{
return store_control_char(ch, rep, pos);
} else if (utf_mode && ctldisp != OPT_ON && is_ubin_char(ch))
{
char *s = prutfchar(ch);
for ( ; *s != 0; s++)
STORE_CHAR(*s, AT_BINARY, NULL, pos);
} else
{
STORE_CHAR(ch, a, rep, pos);
}
return (0);
}




public int
pflushmbc(VOID_PARAM)
{
int r = 0;

if (mbc_buf_len > 0)
{

r = flush_mbc_buf(mbc_pos);
mbc_buf_len = 0;
}
return r;
}




static void
add_attr_normal(VOID_PARAM)
{
char *p = "\033[m";

if (ctldisp != OPT_ONPLUS || !is_ansi_end('m'))
return;
for ( ; *p != '\0'; p++)
add_linebuf(*p, AT_ANSI, 0);
}




public void
pdone(endline, chopped, forw)
int endline;
int chopped;
int forw;
{
(void) pflushmbc();

if (pendc && (pendc != '\r' || !endline))





(void) do_append(pendc, NULL, pendpos);

if (chopped && rscroll_char)
{





if (end_column >= sc_width + cshift)
{

end_column = right_column;
linebuf.end = right_curr;
}
add_attr_normal();
while (end_column < sc_width-1 + cshift)
{





add_linebuf(' ', rscroll_attr, 1);
}

add_linebuf(rscroll_char, rscroll_attr, 1);
} else
{
add_attr_normal();
}














if (end_column < sc_width + cshift || !auto_wrap || (endline && ignaw) || ctldisp == OPT_ON)
{
add_linebuf('\n', AT_NORMAL, 0);
}
else if (ignaw && end_column >= sc_width + cshift && forw)
{














add_linebuf(' ', AT_NORMAL, 1);
add_linebuf('\b', AT_NORMAL, -1);
}
set_linebuf(linebuf.end, '\0', AT_NORMAL);
}




public void
set_status_col(c, attr)
int c;
int attr;
{
set_pfx(0, c, attr);
}






public int
gline(i, ap)
int i;
int *ap;
{
if (is_null_line)
{




if (twiddle)
{
if (i == 0)
{
*ap = AT_BOLD;
return '~';
}
--i;
}

*ap = AT_NORMAL;
return i ? '\0' : '\n';
}

if (i < linebuf.pfx_end)
{
*ap = linebuf.pfx_attr[i];
return linebuf.pfx[i];
}
i += linebuf.print - linebuf.pfx_end;
*ap = linebuf.attr[i];
return (linebuf.buf[i] & 0xFF);
}




public void
null_line(VOID_PARAM)
{
is_null_line = 1;
cshift = 0;
}






public POSITION
forw_raw_line(curr_pos, linep, line_lenp)
POSITION curr_pos;
char **linep;
int *line_lenp;
{
int n;
int c;
POSITION new_pos;

if (curr_pos == NULL_POSITION || ch_seek(curr_pos) ||
(c = ch_forw_get()) == EOI)
return (NULL_POSITION);

n = 0;
for (;;)
{
if (c == '\n' || c == EOI || ABORT_SIGS())
{
new_pos = ch_tell();
break;
}
if (n >= size_linebuf-1)
{
if (expand_linebuf())
{




new_pos = ch_tell() - 1;
break;
}
}
linebuf.buf[n++] = c;
c = ch_forw_get();
}
linebuf.buf[n] = '\0';
if (linep != NULL)
*linep = linebuf.buf;
if (line_lenp != NULL)
*line_lenp = n;
return (new_pos);
}





public POSITION
back_raw_line(curr_pos, linep, line_lenp)
POSITION curr_pos;
char **linep;
int *line_lenp;
{
int n;
int c;
POSITION new_pos;

if (curr_pos == NULL_POSITION || curr_pos <= ch_zero() ||
ch_seek(curr_pos-1))
return (NULL_POSITION);

n = size_linebuf;
linebuf.buf[--n] = '\0';
for (;;)
{
c = ch_back_get();
if (c == '\n' || ABORT_SIGS())
{




new_pos = ch_tell() + 1;
break;
}
if (c == EOI)
{





new_pos = ch_zero();
break;
}
if (n <= 0)
{
int old_size_linebuf = size_linebuf;
char *fm;
char *to;
if (expand_linebuf())
{




new_pos = ch_tell() + 1;
break;
}



for (fm = linebuf.buf + old_size_linebuf - 1,
to = linebuf.buf + size_linebuf - 1;
fm >= linebuf.buf; fm--, to--)
*to = *fm;
n = size_linebuf - old_size_linebuf;
}
linebuf.buf[--n] = c;
}
if (linep != NULL)
*linep = &linebuf.buf[n];
if (line_lenp != NULL)
*line_lenp = size_linebuf - 1 - n;
return (new_pos);
}




public int
rrshift(VOID_PARAM)
{
POSITION pos;
int save_width;
int line;
int longest = 0;

save_width = sc_width;
sc_width = INT_MAX;
pos = position(TOP);
for (line = 0; line < sc_height && pos != NULL_POSITION; line++)
{
pos = forw_line(pos);
if (end_column > longest)
longest = end_column;
}
sc_width = save_width;
if (longest < sc_width)
return 0;
return longest - sc_width;
}




static int
color_index(attr)
int attr;
{
if (use_color)
{
switch (attr & AT_COLOR)
{
case AT_COLOR_ATTN: return 0;
case AT_COLOR_BIN: return 1;
case AT_COLOR_CTRL: return 2;
case AT_COLOR_ERROR: return 3;
case AT_COLOR_LINENUM: return 4;
case AT_COLOR_MARK: return 5;
case AT_COLOR_PROMPT: return 6;
case AT_COLOR_RSCROLL: return 7;
case AT_COLOR_SEARCH: return 8;
}
}
if (attr & AT_UNDERLINE)
return 9;
if (attr & AT_BOLD)
return 10;
if (attr & AT_BLINK)
return 11;
if (attr & AT_STANDOUT)
return 12;
return -1;
}




public int
set_color_map(attr, colorstr)
int attr;
char *colorstr;
{
int cx = color_index(attr);
if (cx < 0)
return -1;
if (strlen(colorstr)+1 > sizeof(color_map[cx]))
return -1;
if (*colorstr != '\0' && parse_color(colorstr, NULL, NULL) == CT_NULL)
return -1;
strcpy(color_map[cx], colorstr);
return 0;
}




public char *
get_color_map(attr)
int attr;
{
int cx = color_index(attr);
if (cx < 0)
return NULL;
return color_map[cx];
}
