






























#include "less.h"
#include "cmd.h"
#include "lesskey.h"

extern int erase_char, erase2_char, kill_char;
extern int secure;
extern int mousecap;
extern int screen_trashed;
extern int sc_height;

#define SK(k) SK_SPECIAL_KEY, (k), 6, 1, 1, 1






static unsigned char cmdtable[] =
{
'\r',0, A_F_LINE,
'\n',0, A_F_LINE,
'e',0, A_F_LINE,
'j',0, A_F_LINE,
SK(SK_DOWN_ARROW),0, A_F_LINE,
CONTROL('E'),0, A_F_LINE,
CONTROL('N'),0, A_F_LINE,
'k',0, A_B_LINE,
'y',0, A_B_LINE,
CONTROL('Y'),0, A_B_LINE,
SK(SK_CONTROL_K),0, A_B_LINE,
CONTROL('P'),0, A_B_LINE,
SK(SK_UP_ARROW),0, A_B_LINE,
'J',0, A_FF_LINE,
'K',0, A_BF_LINE,
'Y',0, A_BF_LINE,
'd',0, A_F_SCROLL,
CONTROL('D'),0, A_F_SCROLL,
'u',0, A_B_SCROLL,
CONTROL('U'),0, A_B_SCROLL,
ESC,'[','M',0, A_X11MOUSE_IN,
ESC,'[','<',0, A_X116MOUSE_IN,
' ',0, A_F_SCREEN,
'f',0, A_F_SCREEN,
CONTROL('F'),0, A_F_SCREEN,
CONTROL('V'),0, A_F_SCREEN,
SK(SK_PAGE_DOWN),0, A_F_SCREEN,
'b',0, A_B_SCREEN,
CONTROL('B'),0, A_B_SCREEN,
ESC,'v',0, A_B_SCREEN,
SK(SK_PAGE_UP),0, A_B_SCREEN,
'z',0, A_F_WINDOW,
'w',0, A_B_WINDOW,
ESC,' ',0, A_FF_SCREEN,
'F',0, A_F_FOREVER,
ESC,'F',0, A_F_UNTIL_HILITE,
'R',0, A_FREPAINT,
'r',0, A_REPAINT,
CONTROL('R'),0, A_REPAINT,
CONTROL('L'),0, A_REPAINT,
ESC,'u',0, A_UNDO_SEARCH,
ESC,'U',0, A_CLR_SEARCH,
'g',0, A_GOLINE,
SK(SK_HOME),0, A_GOLINE,
'<',0, A_GOLINE,
ESC,'<',0, A_GOLINE,
'p',0, A_PERCENT,
'%',0, A_PERCENT,
ESC,'[',0, A_LSHIFT,
ESC,']',0, A_RSHIFT,
ESC,'(',0, A_LSHIFT,
ESC,')',0, A_RSHIFT,
ESC,'{',0, A_LLSHIFT,
ESC,'}',0, A_RRSHIFT,
SK(SK_RIGHT_ARROW),0, A_RSHIFT,
SK(SK_LEFT_ARROW),0, A_LSHIFT,
SK(SK_CTL_RIGHT_ARROW),0, A_RRSHIFT,
SK(SK_CTL_LEFT_ARROW),0, A_LLSHIFT,
'{',0, A_F_BRACKET|A_EXTRA, '{','}',0,
'}',0, A_B_BRACKET|A_EXTRA, '{','}',0,
'(',0, A_F_BRACKET|A_EXTRA, '(',')',0,
')',0, A_B_BRACKET|A_EXTRA, '(',')',0,
'[',0, A_F_BRACKET|A_EXTRA, '[',']',0,
']',0, A_B_BRACKET|A_EXTRA, '[',']',0,
ESC,CONTROL('F'),0, A_F_BRACKET,
ESC,CONTROL('B'),0, A_B_BRACKET,
'G',0, A_GOEND,
ESC,'G',0, A_GOEND_BUF,
ESC,'>',0, A_GOEND,
'>',0, A_GOEND,
SK(SK_END),0, A_GOEND,
'P',0, A_GOPOS,

'0',0, A_DIGIT,
'1',0, A_DIGIT,
'2',0, A_DIGIT,
'3',0, A_DIGIT,
'4',0, A_DIGIT,
'5',0, A_DIGIT,
'6',0, A_DIGIT,
'7',0, A_DIGIT,
'8',0, A_DIGIT,
'9',0, A_DIGIT,
'.',0, A_DIGIT,

'=',0, A_STAT,
CONTROL('G'),0, A_STAT,
':','f',0, A_STAT,
'/',0, A_F_SEARCH,
'?',0, A_B_SEARCH,
ESC,'/',0, A_F_SEARCH|A_EXTRA, '*',0,
ESC,'?',0, A_B_SEARCH|A_EXTRA, '*',0,
'n',0, A_AGAIN_SEARCH,
ESC,'n',0, A_T_AGAIN_SEARCH,
'N',0, A_REVERSE_SEARCH,
ESC,'N',0, A_T_REVERSE_SEARCH,
'&',0, A_FILTER,
'm',0, A_SETMARK,
'M',0, A_SETMARKBOT,
ESC,'m',0, A_CLRMARK,
'\'',0, A_GOMARK,
CONTROL('X'),CONTROL('X'),0, A_GOMARK,
'E',0, A_EXAMINE,
':','e',0, A_EXAMINE,
CONTROL('X'),CONTROL('V'),0, A_EXAMINE,
':','n',0, A_NEXT_FILE,
':','p',0, A_PREV_FILE,
't',0, A_NEXT_TAG,
'T',0, A_PREV_TAG,
':','x',0, A_INDEX_FILE,
':','d',0, A_REMOVE_FILE,
'-',0, A_OPT_TOGGLE,
':','t',0, A_OPT_TOGGLE|A_EXTRA, 't',0,
's',0, A_OPT_TOGGLE|A_EXTRA, 'o',0,
'_',0, A_DISP_OPTION,
'|',0, A_PIPE,
'v',0, A_VISUAL,
'!',0, A_SHELL,
'+',0, A_FIRSTCMD,

'H',0, A_HELP,
'h',0, A_HELP,
SK(SK_F1),0, A_HELP,
'V',0, A_VERSION,
'q',0, A_QUIT,
'Q',0, A_QUIT,
':','q',0, A_QUIT,
':','Q',0, A_QUIT,
'Z','Z',0, A_QUIT
};

static unsigned char edittable[] =
{
'\t',0, EC_F_COMPLETE,
'\17',0, EC_B_COMPLETE,
SK(SK_BACKTAB),0, EC_B_COMPLETE,
ESC,'\t',0, EC_B_COMPLETE,
CONTROL('L'),0, EC_EXPAND,
CONTROL('V'),0, EC_LITERAL,
CONTROL('A'),0, EC_LITERAL,
ESC,'l',0, EC_RIGHT,
SK(SK_RIGHT_ARROW),0, EC_RIGHT,
ESC,'h',0, EC_LEFT,
SK(SK_LEFT_ARROW),0, EC_LEFT,
ESC,'b',0, EC_W_LEFT,
ESC,SK(SK_LEFT_ARROW),0, EC_W_LEFT,
SK(SK_CTL_LEFT_ARROW),0, EC_W_LEFT,
ESC,'w',0, EC_W_RIGHT,
ESC,SK(SK_RIGHT_ARROW),0, EC_W_RIGHT,
SK(SK_CTL_RIGHT_ARROW),0, EC_W_RIGHT,
ESC,'i',0, EC_INSERT,
SK(SK_INSERT),0, EC_INSERT,
ESC,'x',0, EC_DELETE,
SK(SK_DELETE),0, EC_DELETE,
ESC,'X',0, EC_W_DELETE,
ESC,SK(SK_DELETE),0, EC_W_DELETE,
SK(SK_CTL_DELETE),0, EC_W_DELETE,
SK(SK_CTL_BACKSPACE),0, EC_W_BACKSPACE,
ESC,'\b',0, EC_W_BACKSPACE,
ESC,'0',0, EC_HOME,
SK(SK_HOME),0, EC_HOME,
ESC,'$',0, EC_END,
SK(SK_END),0, EC_END,
ESC,'k',0, EC_UP,
SK(SK_UP_ARROW),0, EC_UP,
ESC,'j',0, EC_DOWN,
SK(SK_DOWN_ARROW),0, EC_DOWN,
CONTROL('G'),0, EC_ABORT,
ESC,'[','M',0, EC_X11MOUSE,
ESC,'[','<',0, EC_X116MOUSE,
};




struct tablelist
{
struct tablelist *t_next;
char *t_start;
char *t_end;
};




static struct tablelist *list_fcmd_tables = NULL;
static struct tablelist *list_ecmd_tables = NULL;
static struct tablelist *list_var_tables = NULL;
static struct tablelist *list_sysvar_tables = NULL;





static void
expand_special_keys(table, len)
char *table;
int len;
{
char *fm;
char *to;
int a;
char *repl;
int klen;

for (fm = table; fm < table + len; )
{




for (to = fm; *fm != '\0'; )
{
if (*fm != SK_SPECIAL_KEY)
{
*to++ = *fm++;
continue;
}









repl = special_key_str(fm[1]);
klen = fm[2] & 0377;
fm += klen;
if (repl == NULL || (int) strlen(repl) > klen)
repl = "\377";
while (*repl != '\0')
*to++ = *repl++;
}
*to++ = '\0';




while (to <= fm)
*to++ = A_SKIP;
fm++;
a = *fm++ & 0377;
if (a & A_EXTRA)
{
while (*fm++ != '\0')
continue;
}
}
}




static void
expand_cmd_table(tlist)
struct tablelist *tlist;
{
struct tablelist *t;
for (t = tlist; t != NULL; t = t->t_next)
{
expand_special_keys(t->t_start, t->t_end - t->t_start);
}
}




public void
expand_cmd_tables(VOID_PARAM)
{
expand_cmd_table(list_fcmd_tables);
expand_cmd_table(list_ecmd_tables);
expand_cmd_table(list_var_tables);
expand_cmd_table(list_sysvar_tables);
}





public void
init_cmds(VOID_PARAM)
{



add_fcmd_table((char*)cmdtable, sizeof(cmdtable));
add_ecmd_table((char*)edittable, sizeof(edittable));
#if USERFILE
#if defined(BINDIR)

add_hometable(lesskey, NULL, BINDIR "/.sysless", 1);
#endif










#if HAVE_LESSKEYSRC
if (add_hometable(lesskey_src, "LESSKEYIN_SYSTEM", LESSKEYINFILE_SYS, 1) != 0)
#endif
{



add_hometable(lesskey, "LESSKEY_SYSTEM", LESSKEYFILE_SYS, 1);
}



#if HAVE_LESSKEYSRC
if (add_hometable(lesskey_src, "LESSKEYIN", DEF_LESSKEYINFILE, 0) != 0)
#endif
{



add_hometable(lesskey, "LESSKEY", LESSKEYFILE, 0);
}
#endif
}




static int
add_cmd_table(tlist, buf, len)
struct tablelist **tlist;
char *buf;
int len;
{
struct tablelist *t;

if (len == 0)
return (0);




if ((t = (struct tablelist *)
calloc(1, sizeof(struct tablelist))) == NULL)
{
return (-1);
}
t->t_start = buf;
t->t_end = buf + len;
t->t_next = *tlist;
*tlist = t;
return (0);
}




public void
add_fcmd_table(buf, len)
char *buf;
int len;
{
if (add_cmd_table(&list_fcmd_tables, buf, len) < 0)
error("Warning: some commands disabled", NULL_PARG);
}




public void
add_ecmd_table(buf, len)
char *buf;
int len;
{
if (add_cmd_table(&list_ecmd_tables, buf, len) < 0)
error("Warning: some edit commands disabled", NULL_PARG);
}




static void
add_var_table(tlist, buf, len)
struct tablelist **tlist;
char *buf;
int len;
{
if (add_cmd_table(tlist, buf, len) < 0)
error("Warning: environment variables from lesskey file unavailable", NULL_PARG);
}




static int
mouse_wheel_down(VOID_PARAM)
{
return ((mousecap == OPT_ONPLUS) ? A_B_MOUSE : A_F_MOUSE);
}




static int
mouse_wheel_up(VOID_PARAM)
{
return ((mousecap == OPT_ONPLUS) ? A_F_MOUSE : A_B_MOUSE);
}




static int
mouse_button_rel(x, y)
int x;
int y;
{




if (y < sc_height-1)
{
setmark('#', y);
screen_trashed = 1;
}
return (A_NOACTION);
}




static int
getcc_int(pterm)
char* pterm;
{
int num = 0;
int digits = 0;
for (;;)
{
char ch = getcc();
if (ch < '0' || ch > '9')
{
if (pterm != NULL) *pterm = ch;
if (digits == 0)
return (-1);
return (num);
}
num = (10 * num) + (ch - '0');
++digits;
}
}





static int
x11mouse_action(skip)
int skip;
{
int b = getcc() - X11MOUSE_OFFSET;
int x = getcc() - X11MOUSE_OFFSET-1;
int y = getcc() - X11MOUSE_OFFSET-1;
if (skip)
return (A_NOACTION);
switch (b) {
default:
return (A_NOACTION);
case X11MOUSE_WHEEL_DOWN:
return mouse_wheel_down();
case X11MOUSE_WHEEL_UP:
return mouse_wheel_up();
case X11MOUSE_BUTTON_REL:
return mouse_button_rel(x, y);
}
}





static int
x116mouse_action(skip)
int skip;
{
char ch;
int x, y;
int b = getcc_int(&ch);
if (b < 0 || ch != ';') return (A_NOACTION);
x = getcc_int(&ch) - 1;
if (x < 0 || ch != ';') return (A_NOACTION);
y = getcc_int(&ch) - 1;
if (y < 0) return (A_NOACTION);
if (skip)
return (A_NOACTION);
switch (b) {
case X11MOUSE_WHEEL_DOWN:
return mouse_wheel_down();
case X11MOUSE_WHEEL_UP:
return mouse_wheel_up();
default:
if (ch != 'm') return (A_NOACTION);
return mouse_button_rel(x, y);
}
}




static int
cmd_search(cmd, table, endtable, sp)
char *cmd;
char *table;
char *endtable;
char **sp;
{
char *p;
char *q;
int a;

*sp = NULL;
for (p = table, q = cmd; p < endtable; p++, q++)
{
if (*p == *q)
{







if (*p == '\0')
{
a = *++p & 0377;
while (a == A_SKIP)
a = *++p & 0377;
if (a == A_END_LIST)
{






return (A_UINVALID);
}



if (a & A_EXTRA)
{
*sp = ++p;
a &= ~A_EXTRA;
}
if (a == A_X11MOUSE_IN)
a = x11mouse_action(0);
else if (a == A_X116MOUSE_IN)
a = x116mouse_action(0);
return (a);
}
} else if (*q == '\0')
{





return (A_PREFIX);
} else
{






if (*p == '\0' && p[1] == A_END_LIST)
{




return (A_UINVALID);
}
while (*p++ != '\0')
continue;
while (*p == A_SKIP)
p++;
if (*p & A_EXTRA)
while (*++p != '\0')
continue;
q = cmd-1;
}
}



return (A_INVALID);
}





static int
cmd_decode(tlist, cmd, sp)
struct tablelist *tlist;
char *cmd;
char **sp;
{
struct tablelist *t;
int action = A_INVALID;





for (t = tlist; t != NULL; t = t->t_next)
{
action = cmd_search(cmd, t->t_start, t->t_end, sp);
if (action != A_INVALID)
break;
}
if (action == A_UINVALID)
action = A_INVALID;
return (action);
}




public int
fcmd_decode(cmd, sp)
char *cmd;
char **sp;
{
return (cmd_decode(list_fcmd_tables, cmd, sp));
}




public int
ecmd_decode(cmd, sp)
char *cmd;
char **sp;
{
return (cmd_decode(list_ecmd_tables, cmd, sp));
}





public char *
lgetenv(var)
char *var;
{
int a;
char *s;

a = cmd_decode(list_var_tables, var, &s);
if (a == EV_OK)
return (s);
s = getenv(var);
if (s != NULL && *s != '\0')
return (s);
a = cmd_decode(list_sysvar_tables, var, &s);
if (a == EV_OK)
return (s);
return (NULL);
}




public int
isnullenv(s)
char* s;
{
return (s == NULL || *s == '\0');
}

#if USERFILE





static int
gint(sp)
char **sp;
{
int n;

n = *(*sp)++;
n += *(*sp)++ * KRADIX;
return (n);
}




static int
old_lesskey(buf, len)
char *buf;
int len;
{







if (buf[len-1] != '\0' && buf[len-2] != '\0')
return (-1);
add_fcmd_table(buf, len);
return (0);
}




static int
new_lesskey(buf, len, sysvar)
char *buf;
int len;
int sysvar;
{
char *p;
int c;
int n;





if (buf[len-3] != C0_END_LESSKEY_MAGIC ||
buf[len-2] != C1_END_LESSKEY_MAGIC ||
buf[len-1] != C2_END_LESSKEY_MAGIC)
return (-1);
p = buf + 4;
for (;;)
{
c = *p++;
switch (c)
{
case CMD_SECTION:
n = gint(&p);
add_fcmd_table(p, n);
p += n;
break;
case EDIT_SECTION:
n = gint(&p);
add_ecmd_table(p, n);
p += n;
break;
case VAR_SECTION:
n = gint(&p);
add_var_table((sysvar) ?
&list_sysvar_tables : &list_var_tables, p, n);
p += n;
break;
case END_SECTION:
return (0);
default:



return (-1);
}
}
}




public int
lesskey(filename, sysvar)
char *filename;
int sysvar;
{
char *buf;
POSITION len;
long n;
int f;

if (secure)
return (1);



f = open(filename, OPEN_READ);
if (f < 0)
return (1);









len = filesize(f);
if (len == NULL_POSITION || len < 3)
{



close(f);
return (-1);
}
if ((buf = (char *) calloc((int)len, sizeof(char))) == NULL)
{
close(f);
return (-1);
}
if (lseek(f, (off_t)0, SEEK_SET) == BAD_LSEEK)
{
free(buf);
close(f);
return (-1);
}
n = read(f, buf, (unsigned int) len);
close(f);
if (n != len)
{
free(buf);
return (-1);
}





if (buf[0] != C0_LESSKEY_MAGIC || buf[1] != C1_LESSKEY_MAGIC ||
buf[2] != C2_LESSKEY_MAGIC || buf[3] != C3_LESSKEY_MAGIC)
return (old_lesskey(buf, (int)len));
return (new_lesskey(buf, (int)len, sysvar));
}

#if HAVE_LESSKEYSRC
public int
lesskey_src(filename, sysvar)
char *filename;
int sysvar;
{
static struct lesskey_tables tables;
int r = parse_lesskey(filename, &tables);
if (r != 0)
return (r);
add_fcmd_table(tables.cmdtable.buf.data, tables.cmdtable.buf.end);
add_ecmd_table(tables.edittable.buf.data, tables.edittable.buf.end);
add_var_table(sysvar ? &list_sysvar_tables : &list_var_tables,
tables.vartable.buf.data, tables.vartable.buf.end);
return (0);
}

void
lesskey_parse_error(s)
char *s;
{
PARG parg;
parg.p_string = s;
error("%s", &parg);
}
#endif




public int
add_hometable(call_lesskey, envname, def_filename, sysvar)
int (*call_lesskey)(char *, int);
char *envname;
char *def_filename;
int sysvar;
{
char *filename;
int r;

if (envname != NULL && (filename = lgetenv(envname)) != NULL)
filename = save(filename);
else if (sysvar)
filename = save(def_filename);
else
{
char *xdg = lgetenv("XDG_CONFIG_HOME");
if (!isnullenv(xdg))
filename = dirfile(xdg, def_filename+1, 1);
if (filename == NULL)
filename = homefile(def_filename);
}
if (filename == NULL)
return -1;
r = (*call_lesskey)(filename, sysvar);
free(filename);
return (r);
}
#endif




public int
editchar(c, flags)
int c;
int flags;
{
int action;
int nch;
char *s;
char usercmd[MAX_CMDLEN+1];








if (c == erase_char || c == erase2_char)
return (EC_BACKSPACE);
if (c == kill_char)
{
#if MSDOS_COMPILER==WIN32C
if (!win32_kbhit())
#endif
return (EC_LINEKILL);
}





nch = 0;
do {
if (nch > 0)
c = getcc();
usercmd[nch] = c;
usercmd[nch+1] = '\0';
nch++;
action = ecmd_decode(usercmd, &s);
} while (action == A_PREFIX && nch < MAX_CMDLEN);

if (action == EC_X11MOUSE)
return (x11mouse_action(1));
if (action == EC_X116MOUSE)
return (x116mouse_action(1));

if (flags & ECF_NORIGHTLEFT)
{
switch (action)
{
case EC_RIGHT:
case EC_LEFT:
action = A_INVALID;
break;
}
}
#if CMD_HISTORY
if (flags & ECF_NOHISTORY)
{




switch (action)
{
case EC_UP:
case EC_DOWN:
action = A_INVALID;
break;
}
}
#endif
#if TAB_COMPLETE_FILENAME
if (flags & ECF_NOCOMPLETE)
{




switch (action)
{
case EC_F_COMPLETE:
case EC_B_COMPLETE:
case EC_EXPAND:
action = A_INVALID;
break;
}
}
#endif
if ((flags & ECF_PEEK) || action == A_INVALID)
{






while (nch > 1)
{
ungetcc(usercmd[--nch]);
}
} else
{
if (s != NULL)
ungetsc(s);
}
return action;
}

