














#include "less.h"
#include <signal.h>
#include "position.h"

#if MSDOS_COMPILER
#include <dos.h>
#if MSDOS_COMPILER==WIN32C && defined(MINGW)
#include <direct.h>
#define setdisk(n) _chdrive((n)+1)
#else
#if defined(_MSC_VER)
#include <direct.h>
#define setdisk(n) _chdrive((n)+1)
#else
#include <dir.h>
#endif
#endif
#endif

extern int screen_trashed;
extern IFILE curr_ifile;


#if HAVE_SYSTEM





public void
lsystem(cmd, donemsg)
char *cmd;
char *donemsg;
{
int inp;
#if HAVE_SHELL
char *shell;
char *p;
#endif
IFILE save_ifile;
#if MSDOS_COMPILER && MSDOS_COMPILER!=WIN32C
char cwd[FILENAME_MAX+1];
#endif





if (cmd[0] == '-')
cmd++;
else
{
clear_bot();
putstr("!");
putstr(cmd);
putstr("\n");
}

#if MSDOS_COMPILER
#if MSDOS_COMPILER==WIN32C
if (*cmd == '\0')
cmd = getenv("COMSPEC");
#else







getcwd(cwd, FILENAME_MAX);
#endif
#endif




save_ifile = save_curr_ifile();
(void) edit_ifile(NULL_IFILE);




deinit();
flush();
raw_mode(0);
#if MSDOS_COMPILER==WIN32C
close_getchr();
#endif




init_signals(0);

#if HAVE_DUP





inp = dup(0);
close(0);
#if !MSDOS_COMPILER
#if OS2

if (__open(tty_device(), OPEN_READ) < 0)
#else
if (open(tty_device(), OPEN_READ) < 0)
#endif
#endif
dup(inp);
#endif







#if HAVE_SHELL
p = NULL;
if ((shell = lgetenv("SHELL")) != NULL && *shell != '\0')
{
if (*cmd == '\0')
p = save(shell);
else
{
char *esccmd = shell_quote(cmd);
if (esccmd != NULL)
{
int len = (int) (strlen(shell) + strlen(esccmd) + 5);
p = (char *) ecalloc(len, sizeof(char));
SNPRINTF3(p, len, "%s %s %s", shell, shell_coption(), esccmd);
free(esccmd);
}
}
}
if (p == NULL)
{
if (*cmd == '\0')
p = save("sh");
else
p = save(cmd);
}
system(p);
free(p);
#else
#if MSDOS_COMPILER==DJGPPC



setmode(0, O_TEXT);




__djgpp_exception_toggle();
system(cmd);
__djgpp_exception_toggle();
#else
system(cmd);
#endif
#endif

#if HAVE_DUP



close(0);
dup(inp);
close(inp);
#endif

#if MSDOS_COMPILER==WIN32C
open_getchr();
#endif
init_signals(1);
raw_mode(1);
if (donemsg != NULL)
{
putstr(donemsg);
putstr(" (press RETURN)");
get_return();
putchr('\n');
flush();
}
init();
screen_trashed = 1;

#if MSDOS_COMPILER && MSDOS_COMPILER!=WIN32C




chdir(cwd);
#if MSDOS_COMPILER != DJGPPC




if (cwd[1] == ':')
{
if (cwd[0] >= 'a' && cwd[0] <= 'z')
setdisk(cwd[0] - 'a');
else if (cwd[0] >= 'A' && cwd[0] <= 'Z')
setdisk(cwd[0] - 'A');
}
#endif
#endif




reedit_ifile(save_ifile);

#if defined(SIGWINCH) || defined(SIGWIND)






winch(0);
#endif
}

#endif

#if PIPEC













public int
pipe_mark(c, cmd)
int c;
char *cmd;
{
POSITION mpos, tpos, bpos;






mpos = markpos(c);
if (mpos == NULL_POSITION)
return (-1);
tpos = position(TOP);
if (tpos == NULL_POSITION)
tpos = ch_zero();
bpos = position(BOTTOM);

if (c == '.')
return (pipe_data(cmd, tpos, bpos));
else if (mpos <= tpos)
return (pipe_data(cmd, mpos, bpos));
else if (bpos == NULL_POSITION)
return (pipe_data(cmd, tpos, bpos));
else
return (pipe_data(cmd, tpos, mpos));
}





public int
pipe_data(cmd, spos, epos)
char *cmd;
POSITION spos;
POSITION epos;
{
FILE *f;
int c;







if (ch_seek(spos) != 0)
{
error("Cannot seek to start position", NULL_PARG);
return (-1);
}

if ((f = popen(cmd, "w")) == NULL)
{
error("Cannot create pipe", NULL_PARG);
return (-1);
}
clear_bot();
putstr("!");
putstr(cmd);
putstr("\n");

deinit();
flush();
raw_mode(0);
init_signals(0);
#if MSDOS_COMPILER==WIN32C
close_getchr();
#endif
#if defined(SIGPIPE)
LSIGNAL(SIGPIPE, SIG_IGN);
#endif

c = EOI;
while (epos == NULL_POSITION || spos++ <= epos)
{



c = ch_forw_get();
if (c == EOI)
break;
if (putc(c, f) == EOF)
break;
}




while (c != '\n' && c != EOI )
{
c = ch_forw_get();
if (c == EOI)
break;
if (putc(c, f) == EOF)
break;
}

pclose(f);

#if defined(SIGPIPE)
LSIGNAL(SIGPIPE, SIG_DFL);
#endif
#if MSDOS_COMPILER==WIN32C
open_getchr();
#endif
init_signals(1);
raw_mode(1);
init();
screen_trashed = 1;
#if defined(SIGWINCH) || defined(SIGWIND)

winch(0);
#endif
return (0);
}

#endif
