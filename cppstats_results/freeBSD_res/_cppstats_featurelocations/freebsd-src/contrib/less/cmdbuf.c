














#include "less.h"
#include "cmd.h"
#include "charset.h"
#if HAVE_STAT
#include <sys/stat.h>
#endif

extern int sc_width;
extern int utf_mode;
extern int no_hist_dups;
extern int marks_modified;

static char cmdbuf[CMDBUF_SIZE];
static int cmd_col;
static int prompt_col;
static char *cp;
static int cmd_offset;
static int literal;
static int updown_match = -1;

#if TAB_COMPLETE_FILENAME
static int cmd_complete LESSPARAMS((int action));



static int in_completion = 0;
static char *tk_text;
static char *tk_original;
static char *tk_ipoint;
static char *tk_trial = NULL;
static struct textlist tk_tlist;
#endif

static int cmd_left();
static int cmd_right();

#if SPACES_IN_FILENAMES
public char openquote = '"';
public char closequote = '"';
#endif

#if CMD_HISTORY


#define HISTFILE_FIRST_LINE ".less-history-file:"
#define HISTFILE_SEARCH_SECTION ".search"
#define HISTFILE_SHELL_SECTION ".shell"
#define HISTFILE_MARK_SECTION ".mark"




struct mlist
{
struct mlist *next;
struct mlist *prev;
struct mlist *curr_mp;
char *string;
int modified;
};




struct mlist mlist_search =
{ &mlist_search, &mlist_search, &mlist_search, NULL, 0 };
public void *ml_search = (void *) &mlist_search;

struct mlist mlist_examine =
{ &mlist_examine, &mlist_examine, &mlist_examine, NULL, 0 };
public void *ml_examine = (void *) &mlist_examine;

#if SHELL_ESCAPE || PIPEC
struct mlist mlist_shell =
{ &mlist_shell, &mlist_shell, &mlist_shell, NULL, 0 };
public void *ml_shell = (void *) &mlist_shell;
#endif

#else


public void *ml_search = (void *)1;
public void *ml_examine = (void *)2;
#if SHELL_ESCAPE || PIPEC
public void *ml_shell = (void *)3;
#endif

#endif




static struct mlist *curr_mlist = NULL;
static int curr_cmdflags;

static char cmd_mbc_buf[MAX_UTF_CHAR_LEN];
static int cmd_mbc_buf_len;
static int cmd_mbc_buf_index;





public void
cmd_reset(VOID_PARAM)
{
cp = cmdbuf;
*cp = '\0';
cmd_col = 0;
cmd_offset = 0;
literal = 0;
cmd_mbc_buf_len = 0;
updown_match = -1;
}




public void
clear_cmd(VOID_PARAM)
{
cmd_col = prompt_col = 0;
cmd_mbc_buf_len = 0;
updown_match = -1;
}




public void
cmd_putstr(s)
constant char *s;
{
LWCHAR prev_ch = 0;
LWCHAR ch;
constant char *endline = s + strlen(s);
while (*s != '\0')
{
char *ns = (char *) s;
int width;
ch = step_char(&ns, +1, endline);
while (s < ns)
putchr(*s++);
if (!utf_mode)
width = 1;
else if (is_composing_char(ch) || is_combining_char(prev_ch, ch))
width = 0;
else
width = is_wide_char(ch) ? 2 : 1;
cmd_col += width;
prompt_col += width;
prev_ch = ch;
}
}




public int
len_cmdbuf(VOID_PARAM)
{
char *s = cmdbuf;
char *endline = s + strlen(s);
int len = 0;

while (*s != '\0')
{
step_char(&s, +1, endline);
len++;
}
return (len);
}






static char *
cmd_step_common(p, ch, len, pwidth, bswidth)
char *p;
LWCHAR ch;
int len;
int *pwidth;
int *bswidth;
{
char *pr;
int width;

if (len == 1)
{
pr = prchar((int) ch);
width = (int) strlen(pr);
} else
{
pr = prutfchar(ch);
if (is_composing_char(ch))
width = 0;
else if (is_ubin_char(ch))
width = (int) strlen(pr);
else
{
LWCHAR prev_ch = step_char(&p, -1, cmdbuf);
if (is_combining_char(prev_ch, ch))
width = 0;
else
width = is_wide_char(ch) ? 2 : 1;
}
}
if (pwidth != NULL)
*pwidth = width;
if (bswidth != NULL)
*bswidth = width;
return (pr);
}




static char *
cmd_step_right(pp, pwidth, bswidth)
char **pp;
int *pwidth;
int *bswidth;
{
char *p = *pp;
LWCHAR ch = step_char(pp, +1, p + strlen(p));

return cmd_step_common(p, ch, *pp - p, pwidth, bswidth);
}




static char *
cmd_step_left(pp, pwidth, bswidth)
char **pp;
int *pwidth;
int *bswidth;
{
char *p = *pp;
LWCHAR ch = step_char(pp, -1, cmdbuf);

return cmd_step_common(*pp, ch, p - *pp, pwidth, bswidth);
}





static void
cmd_home(VOID_PARAM)
{
while (cmd_col > prompt_col)
{
int width, bswidth;

cmd_step_left(&cp, &width, &bswidth);
while (bswidth-- > 0)
putbs();
cmd_col -= width;
}

cp = &cmdbuf[cmd_offset];
}





public void
cmd_repaint(old_cp)
constant char *old_cp;
{



if (old_cp == NULL)
{
old_cp = cp;
cmd_home();
}
clear_eol();
while (*cp != '\0')
{
char *np = cp;
int width;
char *pr = cmd_step_right(&np, &width, NULL);
if (cmd_col + width >= sc_width)
break;
cp = np;
putstr(pr);
cmd_col += width;
}
while (*cp != '\0')
{
char *np = cp;
int width;
char *pr = cmd_step_right(&np, &width, NULL);
if (width > 0)
break;
cp = np;
putstr(pr);
}




while (cp > old_cp)
cmd_left();
}




static void
cmd_lshift(VOID_PARAM)
{
char *s;
char *save_cp;
int cols;





s = cmdbuf + cmd_offset;
cols = 0;
while (cols < (sc_width - prompt_col) / 2 && *s != '\0')
{
int width;
cmd_step_right(&s, &width, NULL);
cols += width;
}
while (*s != '\0')
{
int width;
char *ns = s;
cmd_step_right(&ns, &width, NULL);
if (width > 0)
break;
s = ns;
}

cmd_offset = (int) (s - cmdbuf);
save_cp = cp;
cmd_home();
cmd_repaint(save_cp);
}




static void
cmd_rshift(VOID_PARAM)
{
char *s;
char *save_cp;
int cols;






s = cmdbuf + cmd_offset;
cols = 0;
while (cols < (sc_width - prompt_col) / 2 && s > cmdbuf)
{
int width;
cmd_step_left(&s, &width, NULL);
cols += width;
}

cmd_offset = (int) (s - cmdbuf);
save_cp = cp;
cmd_home();
cmd_repaint(save_cp);
}




static int
cmd_right(VOID_PARAM)
{
char *pr;
char *ncp;
int width;

if (*cp == '\0')
{

return (CC_OK);
}
ncp = cp;
pr = cmd_step_right(&ncp, &width, NULL);
if (cmd_col + width >= sc_width)
cmd_lshift();
else if (cmd_col + width == sc_width - 1 && cp[1] != '\0')
cmd_lshift();
cp = ncp;
cmd_col += width;
putstr(pr);
while (*cp != '\0')
{
pr = cmd_step_right(&ncp, &width, NULL);
if (width > 0)
break;
putstr(pr);
cp = ncp;
}
return (CC_OK);
}




static int
cmd_left(VOID_PARAM)
{
char *ncp;
int width = 0;
int bswidth = 0;

if (cp <= cmdbuf)
{

return (CC_OK);
}
ncp = cp;
while (ncp > cmdbuf)
{
cmd_step_left(&ncp, &width, &bswidth);
if (width > 0)
break;
}
if (cmd_col < prompt_col + width)
cmd_rshift();
cp = ncp;
cmd_col -= width;
while (bswidth-- > 0)
putbs();
return (CC_OK);
}




static int
cmd_ichar(cs, clen)
char *cs;
int clen;
{
char *s;

if (strlen(cmdbuf) + clen >= sizeof(cmdbuf)-1)
{

bell();
return (CC_ERROR);
}




for (s = &cmdbuf[strlen(cmdbuf)]; s >= cp; s--)
s[clen] = s[0];



for (s = cp; s < cp + clen; s++)
*s = *cs++;



updown_match = -1;
cmd_repaint(cp);
cmd_right();
return (CC_OK);
}





static int
cmd_erase(VOID_PARAM)
{
char *s;
int clen;

if (cp == cmdbuf)
{




return (CC_QUIT);
}



s = cp;
cmd_left();
clen = (int) (s - cp);




for (s = cp; ; s++)
{
s[0] = s[clen];
if (s[0] == '\0')
break;
}




updown_match = -1;
cmd_repaint(cp);





if ((curr_cmdflags & CF_QUIT_ON_ERASE) && cp == cmdbuf && *cp == '\0')
return (CC_QUIT);
return (CC_OK);
}




static int
cmd_delete(VOID_PARAM)
{
if (*cp == '\0')
{

return (CC_OK);
}



cmd_right();
cmd_erase();
return (CC_OK);
}




static int
cmd_werase(VOID_PARAM)
{
if (cp > cmdbuf && cp[-1] == ' ')
{




while (cp > cmdbuf && cp[-1] == ' ')
(void) cmd_erase();
} else
{




while (cp > cmdbuf && cp[-1] != ' ')
(void) cmd_erase();
}
return (CC_OK);
}




static int
cmd_wdelete(VOID_PARAM)
{
if (*cp == ' ')
{




while (*cp == ' ')
(void) cmd_delete();
} else
{




while (*cp != ' ' && *cp != '\0')
(void) cmd_delete();
}
return (CC_OK);
}




static int
cmd_kill(VOID_PARAM)
{
if (cmdbuf[0] == '\0')
{

return (CC_QUIT);
}
cmd_offset = 0;
cmd_home();
*cp = '\0';
updown_match = -1;
cmd_repaint(cp);





if (curr_cmdflags & CF_QUIT_ON_ERASE)
return (CC_QUIT);
return (CC_OK);
}




public void
set_mlist(mlist, cmdflags)
void *mlist;
int cmdflags;
{
#if CMD_HISTORY
curr_mlist = (struct mlist *) mlist;
curr_cmdflags = cmdflags;


if (curr_mlist != NULL)
curr_mlist->curr_mp = curr_mlist;
#endif
}

#if CMD_HISTORY





static int
cmd_updown(action)
int action;
{
constant char *s;
struct mlist *ml;

if (curr_mlist == NULL)
{



bell();
return (CC_OK);
}

if (updown_match < 0)
{
updown_match = (int) (cp - cmdbuf);
}




for (ml = curr_mlist->curr_mp;;)
{
ml = (action == EC_UP) ? ml->prev : ml->next;
if (ml == curr_mlist)
{



break;
}
if (strncmp(cmdbuf, ml->string, updown_match) == 0)
{




curr_mlist->curr_mp = ml;
s = ml->string;
if (s == NULL)
s = "";
cmd_offset = 0;
cmd_home();
clear_eol();
strcpy(cmdbuf, s);
for (cp = cmdbuf; *cp != '\0'; )
cmd_right();
return (CC_OK);
}
}



bell();
return (CC_OK);
}
#endif




static void
ml_link(mlist, ml)
struct mlist *mlist;
struct mlist *ml;
{
ml->next = mlist;
ml->prev = mlist->prev;
mlist->prev->next = ml;
mlist->prev = ml;
}




static void
ml_unlink(ml)
struct mlist *ml;
{
ml->prev->next = ml->next;
ml->next->prev = ml->prev;
}




public void
cmd_addhist(mlist, cmd, modified)
struct mlist *mlist;
constant char *cmd;
int modified;
{
#if CMD_HISTORY
struct mlist *ml;




if (strlen(cmd) == 0)
return;

if (no_hist_dups)
{
struct mlist *next = NULL;
for (ml = mlist->next; ml->string != NULL; ml = next)
{
next = ml->next;
if (strcmp(ml->string, cmd) == 0)
{
ml_unlink(ml);
free(ml->string);
free(ml);
}
}
}





ml = mlist->prev;
if (ml == mlist || strcmp(ml->string, cmd) != 0)
{




ml = (struct mlist *) ecalloc(1, sizeof(struct mlist));
ml->string = save(cmd);
ml->modified = modified;
ml_link(mlist, ml);
}




mlist->curr_mp = ml->next;
#endif
}





public void
cmd_accept(VOID_PARAM)
{
#if CMD_HISTORY



if (curr_mlist == NULL || curr_mlist == ml_examine)
return;
cmd_addhist(curr_mlist, cmdbuf, 1);
curr_mlist->modified = 1;
#endif
}









static int
cmd_edit(c)
int c;
{
int action;
int flags;

#if TAB_COMPLETE_FILENAME
#define not_in_completion() in_completion = 0
#else
#define not_in_completion(VOID_PARAM)
#endif




flags = 0;
#if CMD_HISTORY
if (curr_mlist == NULL)



flags |= ECF_NOHISTORY;
#endif
#if TAB_COMPLETE_FILENAME
if (curr_mlist == ml_search)



flags |= ECF_NOCOMPLETE;
#endif

action = editchar(c, flags);

switch (action)
{
case A_NOACTION:
return (CC_OK);
case EC_RIGHT:
not_in_completion();
return (cmd_right());
case EC_LEFT:
not_in_completion();
return (cmd_left());
case EC_W_RIGHT:
not_in_completion();
while (*cp != '\0' && *cp != ' ')
cmd_right();
while (*cp == ' ')
cmd_right();
return (CC_OK);
case EC_W_LEFT:
not_in_completion();
while (cp > cmdbuf && cp[-1] == ' ')
cmd_left();
while (cp > cmdbuf && cp[-1] != ' ')
cmd_left();
return (CC_OK);
case EC_HOME:
not_in_completion();
cmd_offset = 0;
cmd_home();
cmd_repaint(cp);
return (CC_OK);
case EC_END:
not_in_completion();
while (*cp != '\0')
cmd_right();
return (CC_OK);
case EC_INSERT:
not_in_completion();
return (CC_OK);
case EC_BACKSPACE:
not_in_completion();
return (cmd_erase());
case EC_LINEKILL:
not_in_completion();
return (cmd_kill());
case EC_ABORT:
not_in_completion();
(void) cmd_kill();
return (CC_QUIT);
case EC_W_BACKSPACE:
not_in_completion();
return (cmd_werase());
case EC_DELETE:
not_in_completion();
return (cmd_delete());
case EC_W_DELETE:
not_in_completion();
return (cmd_wdelete());
case EC_LITERAL:
literal = 1;
return (CC_OK);
#if CMD_HISTORY
case EC_UP:
case EC_DOWN:
not_in_completion();
return (cmd_updown(action));
#endif
#if TAB_COMPLETE_FILENAME
case EC_F_COMPLETE:
case EC_B_COMPLETE:
case EC_EXPAND:
return (cmd_complete(action));
#endif
default:
not_in_completion();
return (CC_PASS);
}
}

#if TAB_COMPLETE_FILENAME



static int
cmd_istr(str)
char *str;
{
char *s;
int action;
char *endline = str + strlen(str);

for (s = str; *s != '\0'; )
{
char *os = s;
step_char(&s, +1, endline);
action = cmd_ichar(os, s - os);
if (action != CC_OK)
return (action);
}
return (CC_OK);
}







static char *
delimit_word(VOID_PARAM)
{
char *word;
#if SPACES_IN_FILENAMES
char *p;
int delim_quoted = 0;
int meta_quoted = 0;
constant char *esc = get_meta_escape();
int esclen = (int) strlen(esc);
#endif




if (*cp != ' ' && *cp != '\0')
{




while (*cp != ' ' && *cp != '\0')
cmd_right();
} else if (cp > cmdbuf && cp[-1] != ' ')
{




;
#if 0
} else
{




return (NULL);
#endif
}



if (cp == cmdbuf)
return (NULL);
#if SPACES_IN_FILENAMES





for (word = cmdbuf; word < cp; word++)
if (*word != ' ')
break;
if (word >= cp)
return (cp);
for (p = cmdbuf; p < cp; p++)
{
if (meta_quoted)
{
meta_quoted = 0;
} else if (esclen > 0 && p + esclen < cp &&
strncmp(p, esc, esclen) == 0)
{
meta_quoted = 1;
p += esclen - 1;
} else if (delim_quoted)
{
if (*p == closequote)
delim_quoted = 0;
} else
{
if (*p == openquote)
delim_quoted = 1;
else if (*p == ' ')
word = p+1;
}
}
#endif
return (word);
}






static void
init_compl(VOID_PARAM)
{
char *word;
char c;




if (tk_text != NULL)
{
free(tk_text);
tk_text = NULL;
}



word = delimit_word();
if (word == NULL)
return;




tk_ipoint = word;



if (tk_original != NULL)
free(tk_original);
tk_original = (char *) ecalloc(cp-word+1, sizeof(char));
strncpy(tk_original, word, cp-word);





c = *cp;
*cp = '\0';
if (*word != openquote)
{
tk_text = fcomplete(word);
} else
{
#if MSDOS_COMPILER
char *qword = NULL;
#else
char *qword = shell_quote(word+1);
#endif
if (qword == NULL)
tk_text = fcomplete(word+1);
else
{
tk_text = fcomplete(qword);
free(qword);
}
}
*cp = c;
}




static char *
next_compl(action, prev)
int action;
char *prev;
{
switch (action)
{
case EC_F_COMPLETE:
return (forw_textlist(&tk_tlist, prev));
case EC_B_COMPLETE:
return (back_textlist(&tk_tlist, prev));
}

return ("?");
}







static int
cmd_complete(action)
int action;
{
char *s;

if (!in_completion || action == EC_EXPAND)
{





init_compl();
if (tk_text == NULL)
{
bell();
return (CC_OK);
}
if (action == EC_EXPAND)
{



tk_trial = tk_text;
} else
{



in_completion = 1;
init_textlist(&tk_tlist, tk_text);
tk_trial = next_compl(action, (char*)NULL);
}
} else
{




tk_trial = next_compl(action, tk_trial);
}




while (cp > tk_ipoint)
(void) cmd_erase();

if (tk_trial == NULL)
{




in_completion = 0;
if (cmd_istr(tk_original) != CC_OK)
goto fail;
} else
{



if (cmd_istr(tk_trial) != CC_OK)
goto fail;



if (is_dir(tk_trial))
{
if (cp > cmdbuf && cp[-1] == closequote)
(void) cmd_erase();
s = lgetenv("LESSSEPARATOR");
if (s == NULL)
s = PATHNAME_SEP;
if (cmd_istr(s) != CC_OK)
goto fail;
}
}

return (CC_OK);

fail:
in_completion = 0;
bell();
return (CC_OK);
}

#endif









public int
cmd_char(c)
int c;
{
int action;
int len;

if (!utf_mode)
{
cmd_mbc_buf[0] = c;
len = 1;
} else
{

if (cmd_mbc_buf_len == 0)
{
retry:
cmd_mbc_buf_index = 1;
*cmd_mbc_buf = c;
if (IS_ASCII_OCTET(c))
cmd_mbc_buf_len = 1;
#if MSDOS_COMPILER || OS2
else if (c == (unsigned char) '\340' && IS_ASCII_OCTET(peekcc()))
{

cmd_mbc_buf_len = 1;
}
#endif
else if (IS_UTF8_LEAD(c))
{
cmd_mbc_buf_len = utf_len(c);
return (CC_OK);
} else
{

bell();
return (CC_ERROR);
}
} else if (IS_UTF8_TRAIL(c))
{
cmd_mbc_buf[cmd_mbc_buf_index++] = c;
if (cmd_mbc_buf_index < cmd_mbc_buf_len)
return (CC_OK);
if (!is_utf8_well_formed(cmd_mbc_buf, cmd_mbc_buf_index))
{

cmd_mbc_buf_len = 0;
bell();
return (CC_ERROR);
}
} else
{

cmd_mbc_buf_len = 0;
bell();

goto retry;
}

len = cmd_mbc_buf_len;
cmd_mbc_buf_len = 0;
}

if (literal)
{



literal = 0;
return (cmd_ichar(cmd_mbc_buf, len));
}




if (in_mca() && len == 1)
{
action = cmd_edit(c);
switch (action)
{
case CC_OK:
case CC_QUIT:
return (action);
case CC_PASS:
break;
}
}




return (cmd_ichar(cmd_mbc_buf, len));
}




public LINENUM
cmd_int(frac)
long *frac;
{
char *p;
LINENUM n = 0;
int err;

for (p = cmdbuf; *p >= '0' && *p <= '9'; p++)
n = (n * 10) + (*p - '0');
*frac = 0;
if (*p++ == '.')
{
*frac = getfraction(&p, NULL, &err);

}
return (n);
}




public char *
get_cmdbuf(VOID_PARAM)
{
return (cmdbuf);
}

#if CMD_HISTORY



public char *
cmd_lastpattern(VOID_PARAM)
{
if (curr_mlist == NULL)
return (NULL);
return (curr_mlist->curr_mp->prev->string);
}
#endif

#if CMD_HISTORY


static int
mlist_size(ml)
struct mlist *ml;
{
int size = 0;
for (ml = ml->next; ml->string != NULL; ml = ml->next)
++size;
return size;
}




static char *
histfile_name(must_exist)
int must_exist;
{
char *home;
char *xdg;
char *name;


name = lgetenv("LESSHISTFILE");
if (!isnullenv(name))
{
if (strcmp(name, "-") == 0 || strcmp(name, "/dev/null") == 0)

return (NULL);
return (save(name));
}


if (strcmp(LESSHISTFILE, "") == 0 || strcmp(LESSHISTFILE, "-") == 0)
return (NULL);


xdg = lgetenv("XDG_DATA_HOME");
home = lgetenv("HOME");
#if OS2
if (isnullenv(home))
home = lgetenv("INIT");
#endif
name = NULL;
if (!must_exist)
{

name = dirfile(xdg, &LESSHISTFILE[1], 1);
if (name == NULL)
name = dirfile(home, LESSHISTFILE, 1);
}
if (name == NULL)
name = dirfile(xdg, &LESSHISTFILE[1], must_exist);
if (name == NULL)
name = dirfile(home, LESSHISTFILE, must_exist);
return (name);
}




static void
read_cmdhist2(action, uparam, skip_search, skip_shell)
void (*action)(void*,struct mlist*,char*);
void *uparam;
int skip_search;
int skip_shell;
{
struct mlist *ml = NULL;
char line[CMDBUF_SIZE];
char *filename;
FILE *f;
char *p;
int *skip = NULL;

filename = histfile_name(1);
if (filename == NULL)
return;
f = fopen(filename, "r");
free(filename);
if (f == NULL)
return;
if (fgets(line, sizeof(line), f) == NULL ||
strncmp(line, HISTFILE_FIRST_LINE, strlen(HISTFILE_FIRST_LINE)) != 0)
{
fclose(f);
return;
}
while (fgets(line, sizeof(line), f) != NULL)
{
for (p = line; *p != '\0'; p++)
{
if (*p == '\n' || *p == '\r')
{
*p = '\0';
break;
}
}
if (strcmp(line, HISTFILE_SEARCH_SECTION) == 0)
{
ml = &mlist_search;
skip = &skip_search;
} else if (strcmp(line, HISTFILE_SHELL_SECTION) == 0)
{
#if SHELL_ESCAPE || PIPEC
ml = &mlist_shell;
skip = &skip_shell;
#else
ml = NULL;
skip = NULL;
#endif
} else if (strcmp(line, HISTFILE_MARK_SECTION) == 0)
{
ml = NULL;
} else if (*line == '"')
{
if (ml != NULL)
{
if (skip != NULL && *skip > 0)
--(*skip);
else
(*action)(uparam, ml, line+1);
}
} else if (*line == 'm')
{
(*action)(uparam, NULL, line);
}
}
fclose(f);
}

static void
read_cmdhist(action, uparam, skip_search, skip_shell)
void (*action)(void*,struct mlist*,char*);
void *uparam;
int skip_search;
int skip_shell;
{
read_cmdhist2(action, uparam, skip_search, skip_shell);
(*action)(uparam, NULL, NULL);
}

static void
addhist_init(void *uparam, struct mlist *ml, char *string)
{
if (ml != NULL)
cmd_addhist(ml, string, 0);
else if (string != NULL)
restore_mark(string);
}
#endif




public void
init_cmdhist(VOID_PARAM)
{
#if CMD_HISTORY
read_cmdhist(&addhist_init, NULL, 0, 0);
#endif
}




#if CMD_HISTORY
static void
write_mlist_header(ml, f)
struct mlist *ml;
FILE *f;
{
if (ml == &mlist_search)
fprintf(f, "%s\n", HISTFILE_SEARCH_SECTION);
#if SHELL_ESCAPE || PIPEC
else if (ml == &mlist_shell)
fprintf(f, "%s\n", HISTFILE_SHELL_SECTION);
#endif
}




static void
write_mlist(ml, f)
struct mlist *ml;
FILE *f;
{
for (ml = ml->next; ml->string != NULL; ml = ml->next)
{
if (!ml->modified)
continue;
fprintf(f, "\"%s\n", ml->string);
ml->modified = 0;
}
ml->modified = 0;
}




static char *
make_tempname(filename)
char *filename;
{
char lastch;
char *tempname = ecalloc(1, strlen(filename)+1);
strcpy(tempname, filename);
lastch = tempname[strlen(tempname)-1];
tempname[strlen(tempname)-1] = (lastch == 'Q') ? 'Z' : 'Q';
return tempname;
}

struct save_ctx
{
struct mlist *mlist;
FILE *fout;
};






static void
copy_hist(void *uparam, struct mlist *ml, char *string)
{
struct save_ctx *ctx = (struct save_ctx *) uparam;

if (ml != NULL && ml != ctx->mlist) {

if (ctx->mlist)

write_mlist(ctx->mlist, ctx->fout);

ctx->mlist = ml;
write_mlist_header(ctx->mlist, ctx->fout);
}

if (string == NULL)
{

if (mlist_search.modified)
{
write_mlist_header(&mlist_search, ctx->fout);
write_mlist(&mlist_search, ctx->fout);
}
#if SHELL_ESCAPE || PIPEC
if (mlist_shell.modified)
{
write_mlist_header(&mlist_shell, ctx->fout);
write_mlist(&mlist_shell, ctx->fout);
}
#endif
} else if (ml != NULL)
{

fprintf(ctx->fout, "\"%s\n", string);
}

}
#endif




static void
make_file_private(f)
FILE *f;
{
#if HAVE_FCHMOD
int do_chmod = 1;
#if HAVE_STAT
struct stat statbuf;
int r = fstat(fileno(f), &statbuf);
if (r < 0 || !S_ISREG(statbuf.st_mode))

do_chmod = 0;
#endif
if (do_chmod)
fchmod(fileno(f), 0600);
#endif
}




static int
histfile_modified(VOID_PARAM)
{
if (mlist_search.modified)
return 1;
#if SHELL_ESCAPE || PIPEC
if (mlist_shell.modified)
return 1;
#endif
#if CMD_HISTORY
if (marks_modified)
return 1;
#endif
return 0;
}




public void
save_cmdhist(VOID_PARAM)
{
#if CMD_HISTORY
char *histname;
char *tempname;
int skip_search;
int skip_shell;
struct save_ctx ctx;
char *s;
FILE *fout = NULL;
int histsize = 0;

if (!histfile_modified())
return;
histname = histfile_name(0);
if (histname == NULL)
return;
tempname = make_tempname(histname);
fout = fopen(tempname, "w");
if (fout != NULL)
{
make_file_private(fout);
s = lgetenv("LESSHISTSIZE");
if (s != NULL)
histsize = atoi(s);
if (histsize <= 0)
histsize = 100;
skip_search = mlist_size(&mlist_search) - histsize;
#if SHELL_ESCAPE || PIPEC
skip_shell = mlist_size(&mlist_shell) - histsize;
#endif
fprintf(fout, "%s\n", HISTFILE_FIRST_LINE);
ctx.fout = fout;
ctx.mlist = NULL;
read_cmdhist(&copy_hist, &ctx, skip_search, skip_shell);
save_marks(fout, HISTFILE_MARK_SECTION);
fclose(fout);
#if MSDOS_COMPILER==WIN32C




remove(histname);
#endif
rename(tempname, histname);
}
free(tempname);
free(histname);
#endif
}
