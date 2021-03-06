









#include "less.h"

#define WHITESP(c) ((c)==' ' || (c)=='\t')

#if TAGS

public char ztags[] = "tags";
public char *tags = ztags;

static int total;
static int curseq;

extern int linenums;
extern int sigs;
extern int ctldisp;

enum tag_result {
TAG_FOUND,
TAG_NOFILE,
TAG_NOTAG,
TAG_NOTYPE,
TAG_INTR
};




enum {
T_CTAGS,
T_CTAGS_X,
T_GTAGS,
T_GRTAGS,
T_GSYMS,
T_GPATH
};

static enum tag_result findctag LESSPARAMS((char *tag));
static enum tag_result findgtag LESSPARAMS((char *tag, int type));
static char *nextgtag(VOID_PARAM);
static char *prevgtag(VOID_PARAM);
static POSITION ctagsearch(VOID_PARAM);
static POSITION gtagsearch(VOID_PARAM);
static int getentry LESSPARAMS((char *buf, char **tag, char **file, char **line));









struct taglist {
struct tag *tl_first;
struct tag *tl_last;
};
struct tag {
struct tag *next, *prev;
char *tag_file;
LINENUM tag_linenum;
char *tag_pattern;
char tag_endline;
};
#define TAG_END ((struct tag *) &taglist)
static struct taglist taglist = { TAG_END, TAG_END };
static struct tag *curtag;

#define TAG_INS(tp) (tp)->next = TAG_END; (tp)->prev = taglist.tl_last; taglist.tl_last->next = (tp); taglist.tl_last = (tp);





#define TAG_RM(tp) (tp)->next->prev = (tp)->prev; (tp)->prev->next = (tp)->next;






public void
cleantags(VOID_PARAM)
{
struct tag *tp;






while ((tp = taglist.tl_first) != TAG_END)
{
TAG_RM(tp);
free(tp->tag_file);
free(tp->tag_pattern);
free(tp);
}
curtag = NULL;
total = curseq = 0;
}




static struct tag *
maketagent(name, file, linenum, pattern, endline)
char *name;
char *file;
LINENUM linenum;
char *pattern;
int endline;
{
struct tag *tp;

tp = (struct tag *) ecalloc(sizeof(struct tag), 1);
tp->tag_file = (char *) ecalloc(strlen(file) + 1, sizeof(char));
strcpy(tp->tag_file, file);
tp->tag_linenum = linenum;
tp->tag_endline = endline;
if (pattern == NULL)
tp->tag_pattern = NULL;
else
{
tp->tag_pattern = (char *) ecalloc(strlen(pattern) + 1, sizeof(char));
strcpy(tp->tag_pattern, pattern);
}
return (tp);
}




public int
gettagtype(VOID_PARAM)
{
int f;

if (strcmp(tags, "GTAGS") == 0)
return T_GTAGS;
if (strcmp(tags, "GRTAGS") == 0)
return T_GRTAGS;
if (strcmp(tags, "GSYMS") == 0)
return T_GSYMS;
if (strcmp(tags, "GPATH") == 0)
return T_GPATH;
if (strcmp(tags, "-") == 0)
return T_CTAGS_X;
f = open(tags, OPEN_READ);
if (f >= 0)
{
close(f);
return T_CTAGS;
}
return T_GTAGS;
}








public void
findtag(tag)
char *tag;
{
int type = gettagtype();
enum tag_result result;

if (type == T_CTAGS)
result = findctag(tag);
else
result = findgtag(tag, type);
switch (result)
{
case TAG_FOUND:
case TAG_INTR:
break;
case TAG_NOFILE:
error("No tags file", NULL_PARG);
break;
case TAG_NOTAG:
error("No such tag in tags file", NULL_PARG);
break;
case TAG_NOTYPE:
error("unknown tag type", NULL_PARG);
break;
}
}




public POSITION
tagsearch(VOID_PARAM)
{
if (curtag == NULL)
return (NULL_POSITION);
if (curtag->tag_linenum != 0)
return gtagsearch();
else
return ctagsearch();
}




public char *
nexttag(n)
int n;
{
char *tagfile = (char *) NULL;

while (n-- > 0)
tagfile = nextgtag();
return tagfile;
}




public char *
prevtag(n)
int n;
{
char *tagfile = (char *) NULL;

while (n-- > 0)
tagfile = prevgtag();
return tagfile;
}




public int
ntags(VOID_PARAM)
{
return total;
}




public int
curr_tag(VOID_PARAM)
{
return curseq;
}









static enum tag_result
findctag(tag)
char *tag;
{
char *p;
char *q;
FILE *f;
int taglen;
LINENUM taglinenum;
char *tagfile;
char *tagpattern;
int tagendline;
int search_char;
int err;
char tline[TAGLINE_SIZE];
struct tag *tp;

p = shell_unquote(tags);
f = fopen(p, "r");
free(p);
if (f == NULL)
return TAG_NOFILE;

cleantags();
total = 0;
taglen = (int) strlen(tag);




while (fgets(tline, sizeof(tline), f) != NULL)
{
if (tline[0] == '!')

continue;
if (strncmp(tag, tline, taglen) != 0 || !WHITESP(tline[taglen]))
continue;









tagpattern = NULL;




p = skipsp(tline+taglen);
if (*p == '\0')

continue;





tagfile = p;
while (!WHITESP(*p) && *p != '\0')
p++;
*p++ = '\0';
p = skipsp(p);
if (*p == '\0')

continue;




tagendline = 0;
taglinenum = getnum(&p, 0, &err);
if (err)
{






taglinenum = 0;
search_char = *p++;
if (*p == '^')
p++;
tagpattern = q = p;
while (*p != search_char && *p != '\0')
{
if (*p == '\\')
p++;
if (q != p)
{
*q++ = *p++;
} else
{
q++;
p++;
}
}
tagendline = (q[-1] == '$');
if (tagendline)
q--;
*q = '\0';
}
tp = maketagent(tag, tagfile, taglinenum, tagpattern, tagendline);
TAG_INS(tp);
total++;
}
fclose(f);
if (total == 0)
return TAG_NOTAG;
curtag = taglist.tl_first;
curseq = 1;
return TAG_FOUND;
}




public int
edit_tagfile(VOID_PARAM)
{
if (curtag == NULL)
return (1);
return (edit(curtag->tag_file));
}

static int
curtag_match(char const *line, POSITION linepos)
{







int len = (int) strlen(curtag->tag_pattern);
if (strncmp(curtag->tag_pattern, line, len) == 0 &&
(!curtag->tag_endline || line[len] == '\0' || line[len] == '\r'))
{
curtag->tag_linenum = find_linenum(linepos);
return 1;
}
return 0;
}










static POSITION
ctagsearch(VOID_PARAM)
{
POSITION pos, linepos;
LINENUM linenum;
int line_len;
char *line;
int found;

pos = ch_zero();
linenum = find_linenum(pos);

for (found = 0; !found;)
{




if (ABORT_SIGS())
return (NULL_POSITION);





linepos = pos;
pos = forw_raw_line(pos, &line, &line_len);
if (linenum != 0)
linenum++;

if (pos == NULL_POSITION)
{



error("Tag not found", NULL_PARG);
return (NULL_POSITION);
}






if (linenums)
add_lnum(linenum, pos);

if (ctldisp != OPT_ONPLUS)
{
if (curtag_match(line, linepos))
found = 1;
} else
{
int cvt_ops = CVT_ANSI;
int cvt_len = cvt_length(line_len, cvt_ops);
int *chpos = cvt_alloc_chpos(cvt_len);
char *cline = (char *) ecalloc(1, cvt_len);
cvt_text(cline, line, chpos, &line_len, cvt_ops);
if (curtag_match(cline, linepos))
found = 1;
free(chpos);
free(cline);
}
}

return (linepos);
}












static enum tag_result
findgtag(tag, type)
char *tag;
int type;
{
char buf[256];
FILE *fp;
struct tag *tp;

if (type != T_CTAGS_X && tag == NULL)
return TAG_NOFILE;

cleantags();
total = 0;





if (type == T_CTAGS_X)
{
fp = stdin;

tags = ztags;
} else
{
#if !HAVE_POPEN
return TAG_NOFILE;
#else
char *command;
char *flag;
char *qtag;
char *cmd = lgetenv("LESSGLOBALTAGS");

if (isnullenv(cmd))
return TAG_NOFILE;

switch (type)
{
case T_GTAGS:
flag = "" ;
break;
case T_GRTAGS:
flag = "r";
break;
case T_GSYMS:
flag = "s";
break;
case T_GPATH:
flag = "P";
break;
default:
return TAG_NOTYPE;
}


qtag = shell_quote(tag);
if (qtag == NULL)
qtag = tag;
command = (char *) ecalloc(strlen(cmd) + strlen(flag) +
strlen(qtag) + 5, sizeof(char));
sprintf(command, "%s -x%s %s", cmd, flag, qtag);
if (qtag != tag)
free(qtag);
fp = popen(command, "r");
free(command);
#endif
}
if (fp != NULL)
{
while (fgets(buf, sizeof(buf), fp))
{
char *name, *file, *line;
int len;

if (sigs)
{
#if HAVE_POPEN
if (fp != stdin)
pclose(fp);
#endif
return TAG_INTR;
}
len = (int) strlen(buf);
if (len > 0 && buf[len-1] == '\n')
buf[len-1] = '\0';
else
{
int c;
do {
c = fgetc(fp);
} while (c != '\n' && c != EOF);
}

if (getentry(buf, &name, &file, &line))
{




break;
}


tp = maketagent(name, file, (LINENUM) atoi(line), NULL, 0);
TAG_INS(tp);
total++;
}
if (fp != stdin)
{
if (pclose(fp))
{
curtag = NULL;
total = curseq = 0;
return TAG_NOFILE;
}
}
}


tp = taglist.tl_first;
if (tp == TAG_END)
return TAG_NOTAG;
curtag = tp;
curseq = 1;
return TAG_FOUND;
}

static int circular = 0;






static char *
nextgtag(VOID_PARAM)
{
struct tag *tp;

if (curtag == NULL)

return NULL;

tp = curtag->next;
if (tp == TAG_END)
{
if (!circular)
return NULL;

curtag = taglist.tl_first;
curseq = 1;
} else
{
curtag = tp;
curseq++;
}
return (curtag->tag_file);
}






static char *
prevgtag(VOID_PARAM)
{
struct tag *tp;

if (curtag == NULL)

return NULL;

tp = curtag->prev;
if (tp == TAG_END)
{
if (!circular)
return NULL;

curtag = taglist.tl_last;
curseq = total;
} else
{
curtag = tp;
curseq--;
}
return (curtag->tag_file);
}






static POSITION
gtagsearch(VOID_PARAM)
{
if (curtag == NULL)
return (NULL_POSITION);
return (find_pos(curtag->tag_linenum));
}





























static int
getentry(buf, tag, file, line)
char *buf;
char **tag;
char **file;
char **line;
{
char *p = buf;

for (*tag = p; *p && !IS_SPACE(*p); p++)
;
if (*p == 0)
return (-1);
*p++ = 0;
for ( ; *p && IS_SPACE(*p); p++)
;
if (*p == 0)
return (-1);




if (!IS_DIGIT(*p))
{
for ( ; *p && !IS_SPACE(*p); p++)
;
for (; *p && IS_SPACE(*p); p++)
;
}
if (!IS_DIGIT(*p))
return (-1);
*line = p;
for (*line = p; *p && !IS_SPACE(*p); p++)
;
if (*p == 0)
return (-1);
*p++ = 0;
for ( ; *p && IS_SPACE(*p); p++)
;
if (*p == 0)
return (-1);
*file = p;
for (*file = p; *p && !IS_SPACE(*p); p++)
;
if (*p == 0)
return (-1);
*p = 0;


if (strlen(*tag) && strlen(*line) && strlen(*file) && atoi(*line) > 0)
return (0);
return (-1);
}

#endif
