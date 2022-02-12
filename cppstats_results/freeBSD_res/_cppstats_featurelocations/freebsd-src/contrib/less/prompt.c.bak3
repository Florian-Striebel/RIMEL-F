


















#include "less.h"
#include "position.h"

extern int pr_type;
extern int new_file;
extern int sc_width;
extern int so_s_width, so_e_width;
extern int linenums;
extern int hshift;
extern int sc_height;
extern int jump_sline;
extern int less_is_more;
extern IFILE curr_ifile;
#if EDITOR
extern char *editor;
extern char *editproto;
#endif





static constant char s_proto[] =
"?n?f%f .?m(%T %i of %m) ..?e(END) ?x- Next\\: %x..%t";
static constant char m_proto[] =
"?n?f%f .?m(%T %i of %m) ..?e(END) ?x- Next\\: %x.:?pB%pB\\%:byte %bB?s/%s...%t";
static constant char M_proto[] =
"?f%f .?n?m(%T %i of %m) ..?ltlines %lt-%lb?L/%L. :byte %bB?s/%s. .?e(END) ?x- Next\\: %x.:?pB%pB\\%..%t";
static constant char e_proto[] =
"?f%f .?m(%T %i of %m) .?ltlines %lt-%lb?L/%L. .byte %bB?s/%s. ?e(END) :?pB%pB\\%..%t";
static constant char h_proto[] =
"HELP -- ?eEND -- Press g to see it again:Press RETURN for more., or q when done";
static constant char w_proto[] =
"Waiting for data";
static constant char more_proto[] =
"--More--(?eEND ?x- Next\\: %x.:?pB%pB\\%:byte %bB?s/%s...%t)";

public char *prproto[3];
public char constant *eqproto = e_proto;
public char constant *hproto = h_proto;
public char constant *wproto = w_proto;

static char message[PROMPT_SIZE];
static char *mp;




public void
init_prompt(VOID_PARAM)
{
prproto[0] = save(s_proto);
prproto[1] = save(less_is_more ? more_proto : m_proto);
prproto[2] = save(M_proto);
eqproto = save(e_proto);
hproto = save(h_proto);
wproto = save(w_proto);
}




static void
ap_str(s)
char *s;
{
int len;

len = (int) strlen(s);
if (mp + len >= message + PROMPT_SIZE)
len = (int) (message + PROMPT_SIZE - mp - 1);
strncpy(mp, s, len);
mp += len;
*mp = '\0';
}




static void
ap_char(c)
char c;
{
char buf[2];

buf[0] = c;
buf[1] = '\0';
ap_str(buf);
}




static void
ap_pos(pos)
POSITION pos;
{
char buf[INT_STRLEN_BOUND(pos) + 2];

postoa(pos, buf);
ap_str(buf);
}




static void
ap_linenum(linenum)
LINENUM linenum;
{
char buf[INT_STRLEN_BOUND(linenum) + 2];

linenumtoa(linenum, buf);
ap_str(buf);
}




static void
ap_int(num)
int num;
{
char buf[INT_STRLEN_BOUND(num) + 2];

inttoa(num, buf);
ap_str(buf);
}




static void
ap_quest(VOID_PARAM)
{
ap_str("?");
}




static POSITION
curr_byte(where)
int where;
{
POSITION pos;

pos = position(where);
while (pos == NULL_POSITION && where >= 0 && where < sc_height-1)
pos = position(++where);
if (pos == NULL_POSITION)
pos = ch_length();
return (pos);
}







static int
cond(c, where)
char c;
int where;
{
POSITION len;

switch (c)
{
case 'a':
return (mp > message);
case 'b':
return (curr_byte(where) != NULL_POSITION);
case 'c':
return (hshift != 0);
case 'e':
return (eof_displayed());
case 'f':
case 'g':
return (strcmp(get_filename(curr_ifile), "-") != 0);
case 'l':
case 'd':
if (!linenums)
return 0;
return (currline(where) != 0);
case 'L':
case 'D':
return (linenums && ch_length() != NULL_POSITION);
case 'm':
#if TAGS
return (ntags() ? (ntags() > 1) : (nifile() > 1));
#else
return (nifile() > 1);
#endif
case 'n':
#if TAGS
return (ntags() ? 1 : new_file);
#else
return (new_file);
#endif
case 'p':
return (curr_byte(where) != NULL_POSITION &&
ch_length() > 0);
case 'P':
return (currline(where) != 0 &&
(len = ch_length()) > 0 &&
find_linenum(len) != 0);
case 's':
case 'B':
return (ch_length() != NULL_POSITION);
case 'x':
#if TAGS
if (ntags())
return (0);
#endif
return (next_ifile(curr_ifile) != NULL_IFILE);
}
return (0);
}








static void
protochar(c, where, iseditproto)
int c;
int where;
int iseditproto;
{
POSITION pos;
POSITION len;
int n;
LINENUM linenum;
LINENUM last_linenum;
IFILE h;
char *s;

#undef PAGE_NUM
#define PAGE_NUM(linenum) ((((linenum) - 1) / (sc_height - 1)) + 1)

switch (c)
{
case 'b':
pos = curr_byte(where);
if (pos != NULL_POSITION)
ap_pos(pos);
else
ap_quest();
break;
case 'c':
ap_int(hshift);
break;
case 'd':
linenum = currline(where);
if (linenum > 0 && sc_height > 1)
ap_linenum(PAGE_NUM(linenum));
else
ap_quest();
break;
case 'D':

len = ch_length();
if (len == NULL_POSITION)
ap_quest();
else if (len == 0)

ap_linenum(0);
else
{
linenum = find_linenum(len - 1);
if (linenum <= 0)
ap_quest();
else
ap_linenum(PAGE_NUM(linenum));
}
break;
#if EDITOR
case 'E':
ap_str(editor);
break;
#endif
case 'f':
ap_str(get_filename(curr_ifile));
break;
case 'F':
ap_str(last_component(get_filename(curr_ifile)));
break;
case 'g':
s = shell_quote(get_filename(curr_ifile));
ap_str(s);
free(s);
break;
case 'i':
#if TAGS
if (ntags())
ap_int(curr_tag());
else
#endif
ap_int(get_index(curr_ifile));
break;
case 'l':
linenum = currline(where);
if (linenum != 0)
ap_linenum(linenum);
else
ap_quest();
break;
case 'L':
len = ch_length();
if (len == NULL_POSITION || len == ch_zero() ||
(linenum = find_linenum(len)) <= 0)
ap_quest();
else
ap_linenum(linenum-1);
break;
case 'm':
#if TAGS
n = ntags();
if (n)
ap_int(n);
else
#endif
ap_int(nifile());
break;
case 'p':
pos = curr_byte(where);
len = ch_length();
if (pos != NULL_POSITION && len > 0)
ap_int(percentage(pos,len));
else
ap_quest();
break;
case 'P':
linenum = currline(where);
if (linenum == 0 ||
(len = ch_length()) == NULL_POSITION || len == ch_zero() ||
(last_linenum = find_linenum(len)) <= 0)
ap_quest();
else
ap_int(percentage(linenum, last_linenum));
break;
case 's':
case 'B':
len = ch_length();
if (len != NULL_POSITION)
ap_pos(len);
else
ap_quest();
break;
case 't':
while (mp > message && mp[-1] == ' ')
mp--;
*mp = '\0';
break;
case 'T':
#if TAGS
if (ntags())
ap_str("tag");
else
#endif
ap_str("file");
break;
case 'x':
h = next_ifile(curr_ifile);
if (h != NULL_IFILE)
ap_str(get_filename(h));
else
ap_quest();
break;
}
}








static constant char *
skipcond(p)
constant char *p;
{
int iflevel;





iflevel = 1;

for (;;) switch (*++p)
{
case '?':



iflevel++;
break;
case ':':





if (iflevel == 1)
return (p);
break;
case '.':





if (--iflevel == 0)
return (p);
break;
case '\\':



++p;
break;
case '\0':





return (p-1);
}

}




static constant char *
wherechar(p, wp)
char constant *p;
int *wp;
{
switch (*p)
{
case 'b': case 'd': case 'l': case 'p': case 'P':
switch (*++p)
{
case 't': *wp = TOP; break;
case 'm': *wp = MIDDLE; break;
case 'b': *wp = BOTTOM; break;
case 'B': *wp = BOTTOM_PLUS_ONE; break;
case 'j': *wp = sindex_from_sline(jump_sline); break;
default: *wp = TOP; p--; break;
}
}
return (p);
}




public char *
pr_expand(proto, maxwidth)
constant char *proto;
int maxwidth;
{
constant char *p;
int c;
int where;

mp = message;

if (*proto == '\0')
return ("");

for (p = proto; *p != '\0'; p++)
{
switch (*p)
{
default:
ap_char(*p);
break;
case '\\':
p++;
ap_char(*p);
break;
case '?':
if ((c = *++p) == '\0')
--p;
else
{
where = 0;
p = wherechar(p, &where);
if (!cond(c, where))
p = skipcond(p);
}
break;
case ':':
p = skipcond(p);
break;
case '.':
break;
case '%':
if ((c = *++p) == '\0')
--p;
else
{
where = 0;
p = wherechar(p, &where);
protochar(c, where,
#if EDITOR
(proto == editproto));
#else
0);
#endif

}
break;
}
}

if (mp == message)
return ("");
if (maxwidth > 0 && mp >= message + maxwidth)
{




return (mp - maxwidth);
}
return (message);
}




public char *
eq_message(VOID_PARAM)
{
return (pr_expand(eqproto, 0));
}







public char *
pr_string(VOID_PARAM)
{
char *prompt;
int type;

type = (!less_is_more) ? pr_type : pr_type ? 0 : 1;
prompt = pr_expand((ch_getflags() & CH_HELPFILE) ?
hproto : prproto[type],
sc_width-so_s_width-so_e_width-2);
new_file = 0;
return (prompt);
}




public char *
wait_message(VOID_PARAM)
{
return (pr_expand(wproto, sc_width-so_s_width-so_e_width-2));
}
