














#include "less.h"
#if MSDOS_COMPILER==WIN32C
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

public char * every_first_cmd = NULL;
public int new_file;
public int is_tty;
public IFILE curr_ifile = NULL_IFILE;
public IFILE old_ifile = NULL_IFILE;
public struct scrpos initial_scrpos;
public POSITION start_attnpos = NULL_POSITION;
public POSITION end_attnpos = NULL_POSITION;
public int wscroll;
public char * progname;
public int quitting;
public int secure;
public int dohelp;

#if LOGFILE
public int logfile = -1;
public int force_logfile = FALSE;
public char * namelogfile = NULL;
#endif

#if EDITOR
public char * editor;
public char * editproto;
#endif

#if TAGS
extern char * tags;
extern char * tagoption;
extern int jump_sline;
#endif

#if defined(WIN32)
static char consoleTitle[256];
#endif

public int one_screen;
extern int less_is_more;
extern int missing_cap;
extern int know_dumb;
extern int pr_type;
extern int quit_if_one_screen;
extern int no_init;
extern int errmsgs;





int
main(argc, argv)
int argc;
char *argv[];
{
IFILE ifile;
char *s;

#if defined(__EMX__)
_response(&argc, &argv);
_wildcard(&argc, &argv);
#endif

progname = *argv++;
argc--;

#if SECURE
secure = 1;
#else
secure = 0;
s = lgetenv("LESSSECURE");
if (!isnullenv(s))
secure = 1;
#endif

#if defined(WIN32)
if (getenv("HOME") == NULL)
{




char *drive = getenv("HOMEDRIVE");
char *path = getenv("HOMEPATH");
if (drive != NULL && path != NULL)
{
char *env = (char *) ecalloc(strlen(drive) +
strlen(path) + 6, sizeof(char));
strcpy(env, "HOME=");
strcat(env, drive);
strcat(env, path);
putenv(env);
}
}
GetConsoleTitle(consoleTitle, sizeof(consoleTitle)/sizeof(char));
#endif





is_tty = isatty(1);
init_mark();
init_cmds();
get_term();
init_charset();
init_line();
init_cmdhist();
init_option();
init_search();





s = last_component(progname);
if (strcmp(s, "more") == 0)
less_is_more = 1;

init_prompt();

if (less_is_more)
scan_option("-fG");

s = lgetenv(less_is_more ? "MORE" : "LESS");
if (s != NULL)
scan_option(save(s));

#define isoptstring(s) less_is_more ? (((s)[0] == '-') && (s)[1] != '\0') : (((s)[0] == '-' || (s)[0] == '+') && (s)[1] != '\0')

while (argc > 0 && (isoptstring(*argv) || isoptpending()))
{
s = *argv++;
argc--;
if (strcmp(s, "--") == 0)
break;
scan_option(s);
}
#undef isoptstring

if (isoptpending())
{




nopendopt();
quit(QUIT_OK);
}

if (less_is_more)
no_init = TRUE;

expand_cmd_tables();

#if EDITOR
editor = lgetenv("VISUAL");
if (editor == NULL || *editor == '\0')
{
editor = lgetenv("EDITOR");
if (isnullenv(editor))
editor = EDIT_PGM;
}
editproto = lgetenv("LESSEDIT");
if (isnullenv(editproto))
editproto = "%E ?lm+%lm. %g";
#endif





ifile = NULL_IFILE;
if (dohelp)
ifile = get_ifile(FAKE_HELPFILE, ifile);
while (argc-- > 0)
{
#if (MSDOS_COMPILER && MSDOS_COMPILER != DJGPPC)






struct textlist tlist;
char *filename;
char *gfilename;
char *qfilename;

gfilename = lglob(*argv++);
init_textlist(&tlist, gfilename);
filename = NULL;
while ((filename = forw_textlist(&tlist, filename)) != NULL)
{
qfilename = shell_unquote(filename);
(void) get_ifile(qfilename, ifile);
free(qfilename);
ifile = prev_ifile(NULL_IFILE);
}
free(gfilename);
#else
(void) get_ifile(*argv++, ifile);
ifile = prev_ifile(NULL_IFILE);
#endif
}



if (!is_tty)
{




set_output(1);
SET_BINARY(1);
if (edit_first() == 0)
{
do {
cat_file();
} while (edit_next(1) == 0);
}
quit(QUIT_OK);
}

if (missing_cap && !know_dumb && !less_is_more)
error("WARNING: terminal is not fully functional", NULL_PARG);
open_getchr();
raw_mode(1);
init_signals(1);




#if TAGS
if (tagoption != NULL || strcmp(tags, "-") == 0)
{






if (nifile() > 0)
{
error("No filenames allowed with -t option", NULL_PARG);
quit(QUIT_ERROR);
}
findtag(tagoption);
if (edit_tagfile())
quit(QUIT_ERROR);




initial_scrpos.pos = tagsearch();
if (initial_scrpos.pos == NULL_POSITION)
quit(QUIT_ERROR);
initial_scrpos.ln = jump_sline;
} else
#endif
{
if (edit_first())
quit(QUIT_ERROR);





if (quit_if_one_screen)
{
if (nifile() > 1)
quit_if_one_screen = FALSE;
else if (!no_init)
one_screen = get_one_screen();
}
}

if (errmsgs > 0)
{





less_printf("Press RETURN to continue ", NULL_PARG);
get_return();
putchr('\n');
}
set_output(1);
init();
commands();
quit(QUIT_OK);

return (0);
}





public char *
save(s)
constant char *s;
{
char *p;

p = (char *) ecalloc(strlen(s)+1, sizeof(char));
strcpy(p, s);
return (p);
}





public VOID_POINTER
ecalloc(count, size)
int count;
unsigned int size;
{
VOID_POINTER p;

p = (VOID_POINTER) calloc(count, size);
if (p != NULL)
return (p);
error("Cannot allocate memory", NULL_PARG);
quit(QUIT_ERROR);

return (NULL);
}




public char *
skipsp(s)
char *s;
{
while (*s == ' ' || *s == '\t')
s++;
return (s);
}






public int
sprefix(ps, s, uppercase)
char *ps;
char *s;
int uppercase;
{
int c;
int sc;
int len = 0;

for ( ; *s != '\0'; s++, ps++)
{
c = *ps;
if (uppercase)
{
if (len == 0 && ASCII_IS_LOWER(c))
return (-1);
if (ASCII_IS_UPPER(c))
c = ASCII_TO_LOWER(c);
}
sc = *s;
if (len > 0 && ASCII_IS_UPPER(sc))
sc = ASCII_TO_LOWER(sc);
if (c != sc)
break;
len++;
}
return (len);
}




public void
quit(status)
int status;
{
static int save_status;





if (status < 0)
status = save_status;
else
save_status = status;
#if LESSTEST
rstat('Q');
#endif
quitting = 1;
edit((char*)NULL);
save_cmdhist();
if (interactive())
clear_bot();
deinit();
flush();
raw_mode(0);
#if MSDOS_COMPILER && MSDOS_COMPILER != DJGPPC






close(2);
#endif
#if defined(WIN32)
SetConsoleTitle(consoleTitle);
#endif
close_getchr();
exit(status);
}
