









#include "less.h"
#include "position.h"

extern IFILE curr_ifile;
extern int sc_height;
extern int jump_sline;
extern int perma_marks;




struct mark
{






char m_letter;
IFILE m_ifile;
char *m_filename;
struct scrpos m_scrpos;
};






#define NMARKS ((2*26)+2)
#define NUMARKS ((2*26)+1)
#define MOUSEMARK (NMARKS-2)
#define LASTMARK (NMARKS-1)
static struct mark marks[NMARKS];
public int marks_modified = 0;





static void
cmark(m, ifile, pos, ln)
struct mark *m;
IFILE ifile;
POSITION pos;
int ln;
{
m->m_ifile = ifile;
m->m_scrpos.pos = pos;
m->m_scrpos.ln = ln;
m->m_filename = NULL;
}




public void
init_mark(VOID_PARAM)
{
int i;

for (i = 0; i < NMARKS; i++)
{
char letter;
switch (i) {
case MOUSEMARK: letter = '#'; break;
case LASTMARK: letter = '\''; break;
default: letter = (i < 26) ? 'a'+i : 'A'+i-26; break;
}
marks[i].m_letter = letter;
cmark(&marks[i], NULL_IFILE, NULL_POSITION, -1);
}
}




static void
mark_set_ifile(m, ifile)
struct mark *m;
IFILE ifile;
{
m->m_ifile = ifile;

free(m->m_filename);
m->m_filename = NULL;
}




static void
mark_get_ifile(m)
struct mark *m;
{
if (m->m_ifile != NULL_IFILE)
return;
mark_set_ifile(m, get_ifile(m->m_filename, prev_ifile(NULL_IFILE)));
}




static struct mark *
getumark(c)
int c;
{
PARG parg;
if (c >= 'a' && c <= 'z')
return (&marks[c-'a']);
if (c >= 'A' && c <= 'Z')
return (&marks[c-'A'+26]);
if (c == '\'')
return (&marks[LASTMARK]);
if (c == '#')
return (&marks[MOUSEMARK]);
parg.p_char = (char) c;
error("Invalid mark letter %c", &parg);
return (NULL);
}






static struct mark *
getmark(c)
int c;
{
struct mark *m;
static struct mark sm;

switch (c)
{
case '^':



m = &sm;
cmark(m, curr_ifile, ch_zero(), 0);
break;
case '$':



if (ch_end_seek())
{
error("Cannot seek to end of file", NULL_PARG);
return (NULL);
}
m = &sm;
cmark(m, curr_ifile, ch_tell(), sc_height);
break;
case '.':



m = &sm;
get_scrpos(&m->m_scrpos, TOP);
cmark(m, curr_ifile, m->m_scrpos.pos, m->m_scrpos.ln);
break;
case '\'':



m = &marks[LASTMARK];
break;
default:



m = getumark(c);
if (m == NULL)
break;
if (m->m_scrpos.pos == NULL_POSITION)
{
error("Mark not set", NULL_PARG);
return (NULL);
}
break;
}
return (m);
}




public int
badmark(c)
int c;
{
return (getmark(c) == NULL);
}




public void
setmark(c, where)
int c;
int where;
{
struct mark *m;
struct scrpos scrpos;

m = getumark(c);
if (m == NULL)
return;
get_scrpos(&scrpos, where);
if (scrpos.pos == NULL_POSITION)
{
bell();
return;
}
cmark(m, curr_ifile, scrpos.pos, scrpos.ln);
marks_modified = 1;
}




public void
clrmark(c)
int c;
{
struct mark *m;

m = getumark(c);
if (m == NULL)
return;
if (m->m_scrpos.pos == NULL_POSITION)
{
bell();
return;
}
m->m_scrpos.pos = NULL_POSITION;
marks_modified = 1;
}




public void
lastmark(VOID_PARAM)
{
struct scrpos scrpos;

if (ch_getflags() & CH_HELPFILE)
return;
get_scrpos(&scrpos, TOP);
if (scrpos.pos == NULL_POSITION)
return;
cmark(&marks[LASTMARK], curr_ifile, scrpos.pos, scrpos.ln);
marks_modified = 1;
}




public void
gomark(c)
int c;
{
struct mark *m;
struct scrpos scrpos;

m = getmark(c);
if (m == NULL)
return;







if (m == &marks[LASTMARK] && m->m_scrpos.pos == NULL_POSITION)
cmark(m, curr_ifile, ch_zero(), jump_sline);

mark_get_ifile(m);


scrpos = m->m_scrpos;
if (m->m_ifile != curr_ifile)
{



if (edit_ifile(m->m_ifile))
return;
}

jump_loc(scrpos.pos, scrpos.ln);
}








public POSITION
markpos(c)
int c;
{
struct mark *m;

m = getmark(c);
if (m == NULL)
return (NULL_POSITION);

if (m->m_ifile != curr_ifile)
{
error("Mark not in current file", NULL_PARG);
return (NULL_POSITION);
}
return (m->m_scrpos.pos);
}




public char
posmark(pos)
POSITION pos;
{
int i;


for (i = 0; i < NUMARKS; i++)
{
if (marks[i].m_ifile == curr_ifile && marks[i].m_scrpos.pos == pos)
{
if (i < 26) return 'a' + i;
if (i < 26*2) return 'A' + (i - 26);
return '#';
}
}
return 0;
}




public void
unmark(ifile)
IFILE ifile;
{
int i;

for (i = 0; i < NMARKS; i++)
if (marks[i].m_ifile == ifile)
marks[i].m_scrpos.pos = NULL_POSITION;
}





public void
mark_check_ifile(ifile)
IFILE ifile;
{
int i;
char *filename = get_real_filename(ifile);

for (i = 0; i < NMARKS; i++)
{
struct mark *m = &marks[i];
char *mark_filename = m->m_filename;
if (mark_filename != NULL)
{
mark_filename = lrealpath(mark_filename);
if (strcmp(filename, mark_filename) == 0)
mark_set_ifile(m, ifile);
free(mark_filename);
}
}
}

#if CMD_HISTORY




public void
save_marks(fout, hdr)
FILE *fout;
char *hdr;
{
int i;

if (!perma_marks)
return;

fprintf(fout, "%s\n", hdr);
for (i = 0; i < NMARKS; i++)
{
char *filename;
struct mark *m = &marks[i];
char pos_str[INT_STRLEN_BOUND(m->m_scrpos.pos) + 2];
if (m->m_scrpos.pos == NULL_POSITION)
continue;
postoa(m->m_scrpos.pos, pos_str);
filename = m->m_filename;
if (filename == NULL)
filename = get_real_filename(m->m_ifile);
if (strcmp(filename, "-") != 0)
fprintf(fout, "m %c %d %s %s\n",
m->m_letter, m->m_scrpos.ln, pos_str, filename);
}
}




public void
restore_mark(line)
char *line;
{
struct mark *m;
int ln;
POSITION pos;

#define skip_whitespace while (*line == ' ') line++
if (*line++ != 'm')
return;
skip_whitespace;
m = getumark(*line++);
if (m == NULL)
return;
skip_whitespace;
ln = lstrtoi(line, &line);
if (ln < 1)
ln = 1;
if (ln > sc_height)
ln = sc_height;
skip_whitespace;
pos = lstrtopos(line, &line);
skip_whitespace;
cmark(m, NULL_IFILE, pos, ln);
m->m_filename = save(line);
}

#endif
