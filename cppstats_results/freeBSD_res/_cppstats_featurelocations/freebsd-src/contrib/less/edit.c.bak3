









#include "less.h"
#include "position.h"
#if HAVE_STAT
#include <sys/stat.h>
#endif
#if OS2
#include <signal.h>
#endif

public int fd0 = 0;

extern int new_file;
extern int cbufs;
extern char *every_first_cmd;
extern int force_open;
extern int is_tty;
extern int sigs;
extern int hshift;
extern int want_filesize;
extern IFILE curr_ifile;
extern IFILE old_ifile;
extern struct scrpos initial_scrpos;
extern void *ml_examine;
#if SPACES_IN_FILENAMES
extern char openquote;
extern char closequote;
#endif

#if LOGFILE
extern int logfile;
extern int force_logfile;
extern char *namelogfile;
#endif

#if HAVE_STAT_INO
public dev_t curr_dev;
public ino_t curr_ino;
#endif








public void
init_textlist(tlist, str)
struct textlist *tlist;
char *str;
{
char *s;
#if SPACES_IN_FILENAMES
int meta_quoted = 0;
int delim_quoted = 0;
char *esc = get_meta_escape();
int esclen = (int) strlen(esc);
#endif

tlist->string = skipsp(str);
tlist->endstring = tlist->string + strlen(tlist->string);
for (s = str; s < tlist->endstring; s++)
{
#if SPACES_IN_FILENAMES
if (meta_quoted)
{
meta_quoted = 0;
} else if (esclen > 0 && s + esclen < tlist->endstring &&
strncmp(s, esc, esclen) == 0)
{
meta_quoted = 1;
s += esclen - 1;
} else if (delim_quoted)
{
if (*s == closequote)
delim_quoted = 0;
} else
{
if (*s == openquote)
delim_quoted = 1;
else if (*s == ' ')
*s = '\0';
}
#else
if (*s == ' ')
*s = '\0';
#endif
}
}

public char *
forw_textlist(tlist, prev)
struct textlist *tlist;
char *prev;
{
char *s;





if (prev == NULL)
s = tlist->string;
else
s = prev + strlen(prev);
if (s >= tlist->endstring)
return (NULL);
while (*s == '\0')
s++;
if (s >= tlist->endstring)
return (NULL);
return (s);
}

public char *
back_textlist(tlist, prev)
struct textlist *tlist;
char *prev;
{
char *s;





if (prev == NULL)
s = tlist->endstring;
else if (prev <= tlist->string)
return (NULL);
else
s = prev - 1;
while (*s == '\0')
s--;
if (s <= tlist->string)
return (NULL);
while (s[-1] != '\0' && s > tlist->string)
s--;
return (s);
}




static void
close_pipe(FILE *pipefd)
{
if (pipefd == NULL)
return;
#if OS2




kill(pipefd->_pid, SIGINT);
#endif
pclose(pipefd);
}




static void
close_file(VOID_PARAM)
{
struct scrpos scrpos;
int chflags;
FILE *altpipe;
char *altfilename;

if (curr_ifile == NULL_IFILE)
return;





get_scrpos(&scrpos, TOP);
if (scrpos.pos != NULL_POSITION)
{
store_pos(curr_ifile, &scrpos);
lastmark();
}



chflags = ch_getflags();
ch_close();




altfilename = get_altfilename(curr_ifile);
if (altfilename != NULL)
{
altpipe = get_altpipe(curr_ifile);
if (altpipe != NULL && !(chflags & CH_KEEPOPEN))
{
close_pipe(altpipe);
set_altpipe(curr_ifile, NULL);
}
close_altfile(altfilename, get_filename(curr_ifile));
set_altfilename(curr_ifile, NULL);
}
curr_ifile = NULL_IFILE;
#if HAVE_STAT_INO
curr_ino = curr_dev = 0;
#endif
}






public int
edit(filename)
char *filename;
{
if (filename == NULL)
return (edit_ifile(NULL_IFILE));
return (edit_ifile(get_ifile(filename, curr_ifile)));
}





public int
edit_ifile(ifile)
IFILE ifile;
{
int f;
int answer;
int chflags;
char *filename;
char *open_filename;
char *alt_filename;
void *altpipe;
IFILE was_curr_ifile;
PARG parg;

if (ifile == curr_ifile)
{



return (0);
}








#if LOGFILE
end_logfile();
#endif
was_curr_ifile = save_curr_ifile();
if (curr_ifile != NULL_IFILE)
{
chflags = ch_getflags();
close_file();
if ((chflags & CH_HELPFILE) && held_ifile(was_curr_ifile) <= 1)
{



del_ifile(was_curr_ifile);
was_curr_ifile = old_ifile;
}
}

if (ifile == NULL_IFILE)
{






unsave_ifile(was_curr_ifile);
return (0);
}

filename = save(get_filename(ifile));




altpipe = get_altpipe(ifile);
if (altpipe != NULL)
{






chflags = 0;
f = -1;
alt_filename = get_altfilename(ifile);
open_filename = (alt_filename != NULL) ? alt_filename : filename;
} else
{
if (strcmp(filename, FAKE_HELPFILE) == 0 ||
strcmp(filename, FAKE_EMPTYFILE) == 0)
alt_filename = NULL;
else
alt_filename = open_altfile(filename, &f, &altpipe);

open_filename = (alt_filename != NULL) ? alt_filename : filename;

chflags = 0;
if (altpipe != NULL)
{







chflags |= CH_POPENED;
if (strcmp(filename, "-") == 0)
chflags |= CH_KEEPOPEN;
} else if (strcmp(filename, "-") == 0)
{




f = fd0;
chflags |= CH_KEEPOPEN;



SET_BINARY(f);
#if MSDOS_COMPILER==DJGPPC





__djgpp_set_ctrl_c(1);
#endif
} else if (strcmp(open_filename, FAKE_EMPTYFILE) == 0)
{
f = -1;
chflags |= CH_NODATA;
} else if (strcmp(open_filename, FAKE_HELPFILE) == 0)
{
f = -1;
chflags |= CH_HELPFILE;
} else if ((parg.p_string = bad_file(open_filename)) != NULL)
{



error("%s", &parg);
free(parg.p_string);
err1:
if (alt_filename != NULL)
{
close_pipe(altpipe);
close_altfile(alt_filename, filename);
free(alt_filename);
}
del_ifile(ifile);
free(filename);



if (was_curr_ifile == ifile)
{




quit(QUIT_ERROR);
}
reedit_ifile(was_curr_ifile);
return (1);
} else if ((f = open(open_filename, OPEN_READ)) < 0)
{



parg.p_string = errno_message(filename);
error("%s", &parg);
free(parg.p_string);
goto err1;
} else
{
chflags |= CH_CANSEEK;
if (!force_open && !opened(ifile) && bin_file(f))
{




parg.p_string = filename;
answer = query("\"%s\" may be a binary file. See it anyway? ",
&parg);
if (answer != 'y' && answer != 'Y')
{
close(f);
goto err1;
}
}
}
}





if (was_curr_ifile != NULL_IFILE)
{
old_ifile = was_curr_ifile;
unsave_ifile(was_curr_ifile);
}
curr_ifile = ifile;
set_altfilename(curr_ifile, alt_filename);
set_altpipe(curr_ifile, altpipe);
set_open(curr_ifile);
get_pos(curr_ifile, &initial_scrpos);
new_file = TRUE;
ch_init(f, chflags);

if (!(chflags & CH_HELPFILE))
{
#if LOGFILE
if (namelogfile != NULL && is_tty)
use_logfile(namelogfile);
#endif
#if HAVE_STAT_INO

if (strcmp(open_filename, "-") != 0)
{
struct stat statbuf;
int r = stat(open_filename, &statbuf);
if (r == 0)
{
curr_ino = statbuf.st_ino;
curr_dev = statbuf.st_dev;
}
}
#endif
if (every_first_cmd != NULL)
{
ungetsc(every_first_cmd);
ungetcc_back(CHAR_END_COMMAND);
}
}

flush();

if (is_tty)
{







pos_clear();
clr_linenum();
#if HILITE_SEARCH
clr_hilite();
#endif
hshift = 0;
if (strcmp(filename, FAKE_HELPFILE) && strcmp(filename, FAKE_EMPTYFILE))
{
char *qfilename = shell_quote(filename);
cmd_addhist(ml_examine, qfilename, 1);
free(qfilename);
}
if (want_filesize)
scan_eof();
}
free(filename);
return (0);
}






public int
edit_list(filelist)
char *filelist;
{
IFILE save_ifile;
char *good_filename;
char *filename;
char *gfilelist;
char *gfilename;
char *qfilename;
struct textlist tl_files;
struct textlist tl_gfiles;

save_ifile = save_curr_ifile();
good_filename = NULL;







init_textlist(&tl_files, filelist);
filename = NULL;
while ((filename = forw_textlist(&tl_files, filename)) != NULL)
{
gfilelist = lglob(filename);
init_textlist(&tl_gfiles, gfilelist);
gfilename = NULL;
while ((gfilename = forw_textlist(&tl_gfiles, gfilename)) != NULL)
{
qfilename = shell_unquote(gfilename);
if (edit(qfilename) == 0 && good_filename == NULL)
good_filename = get_filename(curr_ifile);
free(qfilename);
}
free(gfilelist);
}



if (good_filename == NULL)
{
unsave_ifile(save_ifile);
return (1);
}
if (get_ifile(good_filename, curr_ifile) == curr_ifile)
{



unsave_ifile(save_ifile);
return (0);
}
reedit_ifile(save_ifile);
return (edit(good_filename));
}




public int
edit_first(VOID_PARAM)
{
if (nifile() == 0)
return (edit_stdin());
curr_ifile = NULL_IFILE;
return (edit_next(1));
}




public int
edit_last(VOID_PARAM)
{
curr_ifile = NULL_IFILE;
return (edit_prev(1));
}





static int
edit_istep(h, n, dir)
IFILE h;
int n;
int dir;
{
IFILE next;




for (;;)
{
next = (dir > 0) ? next_ifile(h) : prev_ifile(h);
if (--n < 0)
{
if (edit_ifile(h) == 0)
break;
}
if (next == NULL_IFILE)
{



return (1);
}
if (ABORT_SIGS())
{




return (1);
}
h = next;
}



return (0);
}

static int
edit_inext(h, n)
IFILE h;
int n;
{
return (edit_istep(h, n, +1));
}

public int
edit_next(n)
int n;
{
return edit_istep(curr_ifile, n, +1);
}

static int
edit_iprev(h, n)
IFILE h;
int n;
{
return (edit_istep(h, n, -1));
}

public int
edit_prev(n)
int n;
{
return edit_istep(curr_ifile, n, -1);
}




public int
edit_index(n)
int n;
{
IFILE h;

h = NULL_IFILE;
do
{
if ((h = next_ifile(h)) == NULL_IFILE)
{



return (1);
}
} while (get_index(h) != n);

return (edit_ifile(h));
}

public IFILE
save_curr_ifile(VOID_PARAM)
{
if (curr_ifile != NULL_IFILE)
hold_ifile(curr_ifile, 1);
return (curr_ifile);
}

public void
unsave_ifile(save_ifile)
IFILE save_ifile;
{
if (save_ifile != NULL_IFILE)
hold_ifile(save_ifile, -1);
}




public void
reedit_ifile(save_ifile)
IFILE save_ifile;
{
IFILE next;
IFILE prev;







unsave_ifile(save_ifile);
next = next_ifile(save_ifile);
prev = prev_ifile(save_ifile);
if (edit_ifile(save_ifile) == 0)
return;



if (next != NULL_IFILE && edit_inext(next, 0) == 0)
return;



if (prev != NULL_IFILE && edit_iprev(prev, 0) == 0)
return;



quit(QUIT_ERROR);
}

public void
reopen_curr_ifile(VOID_PARAM)
{
IFILE save_ifile = save_curr_ifile();
close_file();
reedit_ifile(save_ifile);
}




public int
edit_stdin(VOID_PARAM)
{
if (isatty(fd0))
{
error("Missing filename (\"less --help\" for help)", NULL_PARG);
quit(QUIT_OK);
}
return (edit("-"));
}





public void
cat_file(VOID_PARAM)
{
int c;

while ((c = ch_forw_get()) != EOI)
putchr(c);
flush();
}

#if LOGFILE

#define OVERWRITE_OPTIONS "Overwrite, Append, Don't log, or Quit?"






public void
use_logfile(filename)
char *filename;
{
int exists;
int answer;
PARG parg;

if (ch_getflags() & CH_CANSEEK)



return;




exists = open(filename, OPEN_READ);
if (exists >= 0)
close(exists);
exists = (exists >= 0);





if (!exists || force_logfile)
{



answer = 'O';
} else
{



parg.p_string = filename;
answer = query("Warning: \"%s\" exists; "OVERWRITE_OPTIONS" ", &parg);
}

loop:
switch (answer)
{
case 'O': case 'o':



logfile = creat(filename, 0644);
break;
case 'A': case 'a':



logfile = open(filename, OPEN_APPEND);
if (lseek(logfile, (off_t)0, SEEK_END) == BAD_LSEEK)
{
close(logfile);
logfile = -1;
}
break;
case 'D': case 'd':



return;
default:




answer = query(OVERWRITE_OPTIONS" (Type \"O\", \"A\", \"D\" or \"Q\") ", NULL_PARG);
goto loop;
}

if (logfile < 0)
{



parg.p_string = filename;
error("Cannot write to \"%s\"", &parg);
return;
}
SET_BINARY(logfile);
}

#endif
