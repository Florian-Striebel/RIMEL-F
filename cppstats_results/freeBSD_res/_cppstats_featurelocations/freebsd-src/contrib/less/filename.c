














#include "less.h"
#include "lglob.h"
#if MSDOS_COMPILER
#include <dos.h>
#if MSDOS_COMPILER==WIN32C && !defined(_MSC_VER)
#include <dir.h>
#endif
#if MSDOS_COMPILER==DJGPPC
#include <glob.h>
#include <dir.h>
#define _MAX_PATH PATH_MAX
#endif
#endif
#if defined(_OSK)
#include <rbf.h>
#if !defined(_OSK_MWC32)
#include <modes.h>
#endif
#endif

#if HAVE_STAT
#include <sys/stat.h>
#if !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#endif

extern int force_open;
extern int secure;
extern int use_lessopen;
extern int ctldisp;
extern int utf_mode;
extern IFILE curr_ifile;
extern IFILE old_ifile;
#if SPACES_IN_FILENAMES
extern char openquote;
extern char closequote;
#endif




public char *
shell_unquote(str)
char *str;
{
char *name;
char *p;

name = p = (char *) ecalloc(strlen(str)+1, sizeof(char));
if (*str == openquote)
{
str++;
while (*str != '\0')
{
if (*str == closequote)
{
if (str[1] != closequote)
break;
str++;
}
*p++ = *str++;
}
} else
{
char *esc = get_meta_escape();
int esclen = (int) strlen(esc);
while (*str != '\0')
{
if (esclen > 0 && strncmp(str, esc, esclen) == 0)
str += esclen;
*p++ = *str++;
}
}
*p = '\0';
return (name);
}




public char *
get_meta_escape(VOID_PARAM)
{
char *s;

s = lgetenv("LESSMETAESCAPE");
if (s == NULL)
s = DEF_METAESCAPE;
return (s);
}




static char *
metachars(VOID_PARAM)
{
static char *mchars = NULL;

if (mchars == NULL)
{
mchars = lgetenv("LESSMETACHARS");
if (mchars == NULL)
mchars = DEF_METACHARS;
}
return (mchars);
}




static int
metachar(c)
char c;
{
return (strchr(metachars(), c) != NULL);
}




public char *
shell_quote(s)
char *s;
{
char *p;
char *newstr;
int len;
char *esc = get_meta_escape();
int esclen = (int) strlen(esc);
int use_quotes = 0;
int have_quotes = 0;




len = 1;
for (p = s; *p != '\0'; p++)
{
len++;
if (*p == openquote || *p == closequote)
have_quotes = 1;
if (metachar(*p))
{
if (esclen == 0)
{




use_quotes = 1;
} else
{



len += esclen;
}
}
}
if (use_quotes)
{
if (have_quotes)



return (NULL);
len = (int) strlen(s) + 3;
}



newstr = p = (char *) ecalloc(len, sizeof(char));
if (use_quotes)
{
SNPRINTF3(newstr, len, "%c%s%c", openquote, s, closequote);
} else
{
while (*s != '\0')
{
if (metachar(*s))
{



strcpy(p, esc);
p += esclen;
}
*p++ = *s++;
}
*p = '\0';
}
return (newstr);
}





public char *
dirfile(dirname, filename, must_exist)
char *dirname;
char *filename;
int must_exist;
{
char *pathname;
int len;
int f;

if (dirname == NULL || *dirname == '\0')
return (NULL);



len = (int) (strlen(dirname) + strlen(filename) + 2);
pathname = (char *) calloc(len, sizeof(char));
if (pathname == NULL)
return (NULL);
SNPRINTF3(pathname, len, "%s%s%s", dirname, PATHNAME_SEP, filename);
if (must_exist)
{



f = open(pathname, OPEN_READ);
if (f < 0)
{
free(pathname);
pathname = NULL;
} else
{
close(f);
}
}
return (pathname);
}




public char *
homefile(filename)
char *filename;
{
char *pathname;


pathname = dirfile(lgetenv("HOME"), filename, 1);
if (pathname != NULL)
return (pathname);
#if OS2

pathname = dirfile(lgetenv("INIT"), filename, 1);
if (pathname != NULL)
return (pathname);
#endif
#if MSDOS_COMPILER || OS2

pathname = (char *) ecalloc(_MAX_PATH, sizeof(char));
#if MSDOS_COMPILER==DJGPPC
{
char *res = searchpath(filename);
if (res == 0)
*pathname = '\0';
else
strcpy(pathname, res);
}
#else
_searchenv(filename, "PATH", pathname);
#endif
if (*pathname != '\0')
return (pathname);
free(pathname);
#endif
return (NULL);
}








public char *
fexpand(s)
char *s;
{
char *fr, *to;
int n;
char *e;
IFILE ifile;

#define fchar_ifile(c) ((c) == '%' ? curr_ifile : (c) == '#' ? old_ifile : NULL_IFILE)







n = 0;
for (fr = s; *fr != '\0'; fr++)
{
switch (*fr)
{
case '%':
case '#':
if (fr > s && fr[-1] == *fr)
{




n++;
} else if (fr[1] != *fr)
{



ifile = fchar_ifile(*fr);
if (ifile == NULL_IFILE)
n++;
else
n += (int) strlen(get_filename(ifile));
}




break;
default:
n++;
break;
}
}

e = (char *) ecalloc(n+1, sizeof(char));




to = e;
for (fr = s; *fr != '\0'; fr++)
{
switch (*fr)
{
case '%':
case '#':
if (fr > s && fr[-1] == *fr)
{
*to++ = *fr;
} else if (fr[1] != *fr)
{
ifile = fchar_ifile(*fr);
if (ifile == NULL_IFILE)
*to++ = *fr;
else
{
strcpy(to, get_filename(ifile));
to += strlen(to);
}
}
break;
default:
*to++ = *fr;
break;
}
}
*to = '\0';
return (e);
}


#if TAB_COMPLETE_FILENAME





public char *
fcomplete(s)
char *s;
{
char *fpat;
char *qs;

if (secure)
return (NULL);



#if MSDOS_COMPILER && (MSDOS_COMPILER == MSOFTC || MSDOS_COMPILER == BORLANDC)







{
char *slash;
int len;
for (slash = s+strlen(s)-1; slash > s; slash--)
if (*slash == *PATHNAME_SEP || *slash == '/')
break;
len = (int) strlen(s) + 4;
fpat = (char *) ecalloc(len, sizeof(char));
if (strchr(slash, '.') == NULL)
SNPRINTF1(fpat, len, "%s*.*", s);
else
SNPRINTF1(fpat, len, "%s*", s);
}
#else
{
int len = (int) strlen(s) + 2;
fpat = (char *) ecalloc(len, sizeof(char));
SNPRINTF1(fpat, len, "%s*", s);
}
#endif
qs = lglob(fpat);
s = shell_unquote(qs);
if (strcmp(s,fpat) == 0)
{



free(qs);
qs = NULL;
}
free(s);
free(fpat);
return (qs);
}
#endif





public int
bin_file(f)
int f;
{
int n;
int bin_count = 0;
char data[256];
char* p;
char* edata;

if (!seekable(f))
return (0);
if (lseek(f, (off_t)0, SEEK_SET) == BAD_LSEEK)
return (0);
n = read(f, data, sizeof(data));
if (n <= 0)
return (0);
edata = &data[n];
for (p = data; p < edata; )
{
if (utf_mode && !is_utf8_well_formed(p, edata-data))
{
bin_count++;
utf_skip_to_lead(&p, edata);
} else
{
LWCHAR c = step_char(&p, +1, edata);
struct ansi_state *pansi;
if (ctldisp == OPT_ONPLUS && (pansi = ansi_start(c)) != NULL)
{
skip_ansi(pansi, &p, edata);
ansi_done(pansi);
} else if (binary_char(c))
bin_count++;
}
}




return (bin_count > 5);
}




static POSITION
seek_filesize(f)
int f;
{
off_t spos;

spos = lseek(f, (off_t)0, SEEK_END);
if (spos == BAD_LSEEK)
return (NULL_POSITION);
return ((POSITION) spos);
}

#if HAVE_POPEN




static char *
readfd(fd)
FILE *fd;
{
int len;
int ch;
char *buf;
char *p;





len = 100;
buf = (char *) ecalloc(len, sizeof(char));
for (p = buf; ; p++)
{
if ((ch = getc(fd)) == '\n' || ch == EOF)
break;
if (p - buf >= len-1)
{




len *= 2;
*p = '\0';
p = (char *) ecalloc(len, sizeof(char));
strcpy(p, buf);
free(buf);
buf = p;
p = buf + strlen(buf);
}
*p = ch;
}
*p = '\0';
return (buf);
}





static FILE *
shellcmd(cmd)
char *cmd;
{
FILE *fd;

#if HAVE_SHELL
char *shell;

shell = lgetenv("SHELL");
if (!isnullenv(shell))
{
char *scmd;
char *esccmd;





esccmd = shell_quote(cmd);
if (esccmd == NULL)
{
fd = popen(cmd, "r");
} else
{
int len = (int) (strlen(shell) + strlen(esccmd) + 5);
scmd = (char *) ecalloc(len, sizeof(char));
SNPRINTF3(scmd, len, "%s %s %s", shell, shell_coption(), esccmd);
free(esccmd);
fd = popen(scmd, "r");
free(scmd);
}
} else
#endif
{
fd = popen(cmd, "r");
}




SET_BINARY(0);
return (fd);
}

#endif





public char *
lglob(filename)
char *filename;
{
char *gfilename;

filename = fexpand(filename);
if (secure)
return (filename);

#if defined(DECL_GLOB_LIST)
{



int length;
char *p;
char *qfilename;
DECL_GLOB_LIST(list)

GLOB_LIST(filename, list);
if (GLOB_LIST_FAILED(list))
{
return (filename);
}
length = 1;
for (SCAN_GLOB_LIST(list, p))
{
INIT_GLOB_LIST(list, p);
qfilename = shell_quote(p);
if (qfilename != NULL)
{
length += strlen(qfilename) + 1;
free(qfilename);
}
}
gfilename = (char *) ecalloc(length, sizeof(char));
for (SCAN_GLOB_LIST(list, p))
{
INIT_GLOB_LIST(list, p);
qfilename = shell_quote(p);
if (qfilename != NULL)
{
sprintf(gfilename + strlen(gfilename), "%s ", qfilename);
free(qfilename);
}
}



*--p = '\0';
GLOB_LIST_DONE(list);
}
#else
#if defined(DECL_GLOB_NAME)
{




char *p;
int len;
int n;
char *pfilename;
char *qfilename;
DECL_GLOB_NAME(fnd,drive,dir,fname,ext,handle)

GLOB_FIRST_NAME(filename, &fnd, handle);
if (GLOB_FIRST_FAILED(handle))
{
return (filename);
}

_splitpath(filename, drive, dir, fname, ext);
len = 100;
gfilename = (char *) ecalloc(len, sizeof(char));
p = gfilename;
do {
n = (int) (strlen(drive) + strlen(dir) + strlen(fnd.GLOB_NAME) + 1);
pfilename = (char *) ecalloc(n, sizeof(char));
SNPRINTF3(pfilename, n, "%s%s%s", drive, dir, fnd.GLOB_NAME);
qfilename = shell_quote(pfilename);
free(pfilename);
if (qfilename != NULL)
{
n = (int) strlen(qfilename);
while (p - gfilename + n + 2 >= len)
{




len *= 2;
*p = '\0';
p = (char *) ecalloc(len, sizeof(char));
strcpy(p, gfilename);
free(gfilename);
gfilename = p;
p = gfilename + strlen(gfilename);
}
strcpy(p, qfilename);
free(qfilename);
p += n;
*p++ = ' ';
}
} while (GLOB_NEXT_NAME(handle, &fnd) == 0);




*--p = '\0';
GLOB_NAME_DONE(handle);
}
#else
#if HAVE_POPEN
{




FILE *fd;
char *s;
char *lessecho;
char *cmd;
char *esc;
int len;

esc = get_meta_escape();
if (strlen(esc) == 0)
esc = "-";
esc = shell_quote(esc);
if (esc == NULL)
{
return (filename);
}
lessecho = lgetenv("LESSECHO");
if (isnullenv(lessecho))
lessecho = "lessecho";



len = (int) (strlen(lessecho) + strlen(filename) + (7*strlen(metachars())) + 24);
cmd = (char *) ecalloc(len, sizeof(char));
SNPRINTF4(cmd, len, "%s -p0x%x -d0x%x -e%s ", lessecho, openquote, closequote, esc);
free(esc);
for (s = metachars(); *s != '\0'; s++)
sprintf(cmd + strlen(cmd), "-n0x%x ", *s);
sprintf(cmd + strlen(cmd), "-- %s", filename);
fd = shellcmd(cmd);
free(cmd);
if (fd == NULL)
{




return (filename);
}
gfilename = readfd(fd);
pclose(fd);
if (*gfilename == '\0')
{
free(gfilename);
return (filename);
}
}
#else



gfilename = save(filename);
#endif
#endif
#endif
free(filename);
return (gfilename);
}




public char *
lrealpath(path)
char *path;
{
#if HAVE_REALPATH
char rpath[PATH_MAX];
if (realpath(path, rpath) != NULL)
return (save(rpath));
#endif
return (save(path));
}

#if HAVE_POPEN




static int
num_pct_s(lessopen)
char *lessopen;
{
int num = 0;

while (*lessopen != '\0')
{
if (*lessopen == '%')
{
if (lessopen[1] == '%')
++lessopen;
else if (lessopen[1] == 's')
++num;
else
return (999);
}
++lessopen;
}
return (num);
}
#endif





public char *
open_altfile(filename, pf, pfd)
char *filename;
int *pf;
void **pfd;
{
#if !HAVE_POPEN
return (NULL);
#else
char *lessopen;
char *qfilename;
char *cmd;
int len;
FILE *fd;
#if HAVE_FILENO
int returnfd = 0;
#endif

if (!use_lessopen || secure)
return (NULL);
ch_ungetchar(-1);
if ((lessopen = lgetenv("LESSOPEN")) == NULL)
return (NULL);
while (*lessopen == '|')
{




#if !HAVE_FILENO
error("LESSOPEN pipe is not supported", NULL_PARG);
return (NULL);
#else
lessopen++;
returnfd++;
#endif
}
if (*lessopen == '-')
{



lessopen++;
} else
{
if (strcmp(filename, "-") == 0)
return (NULL);
}
if (num_pct_s(lessopen) != 1)
{
error("LESSOPEN ignored: must contain exactly one %%s", NULL_PARG);
return (NULL);
}

qfilename = shell_quote(filename);
len = (int) (strlen(lessopen) + strlen(qfilename) + 2);
cmd = (char *) ecalloc(len, sizeof(char));
SNPRINTF1(cmd, len, lessopen, qfilename);
free(qfilename);
fd = shellcmd(cmd);
free(cmd);
if (fd == NULL)
{



return (NULL);
}
#if HAVE_FILENO
if (returnfd)
{
char c;
int f;






f = fileno(fd);
SET_BINARY(f);
if (read(f, &c, 1) != 1)
{







int status = pclose(fd);
if (returnfd > 1 && status == 0) {

*pfd = NULL;
*pf = -1;
return (save(FAKE_EMPTYFILE));
}

return (NULL);
}

ch_ungetchar(c);
*pfd = (void *) fd;
*pf = f;
return (save("-"));
}
#endif

cmd = readfd(fd);
pclose(fd);
if (*cmd == '\0')
{



free(cmd);
return (NULL);
}
return (cmd);
#endif
}




public void
close_altfile(altfilename, filename)
char *altfilename;
char *filename;
{
#if HAVE_POPEN
char *lessclose;
FILE *fd;
char *cmd;
int len;

if (secure)
return;
ch_ungetchar(-1);
if ((lessclose = lgetenv("LESSCLOSE")) == NULL)
return;
if (num_pct_s(lessclose) > 2)
{
error("LESSCLOSE ignored; must contain no more than 2 %%s", NULL_PARG);
return;
}
len = (int) (strlen(lessclose) + strlen(filename) + strlen(altfilename) + 2);
cmd = (char *) ecalloc(len, sizeof(char));
SNPRINTF2(cmd, len, lessclose, filename, altfilename);
fd = shellcmd(cmd);
free(cmd);
if (fd != NULL)
pclose(fd);
#endif
}




public int
is_dir(filename)
char *filename;
{
int isdir = 0;

#if HAVE_STAT
{
int r;
struct stat statbuf;

r = stat(filename, &statbuf);
isdir = (r >= 0 && S_ISDIR(statbuf.st_mode));
}
#else
#if defined(_OSK)
{
int f;

f = open(filename, S_IREAD | S_IFDIR);
if (f >= 0)
close(f);
isdir = (f >= 0);
}
#endif
#endif
return (isdir);
}






public char *
bad_file(filename)
char *filename;
{
char *m = NULL;

if (!force_open && is_dir(filename))
{
static char is_a_dir[] = " is a directory";

m = (char *) ecalloc(strlen(filename) + sizeof(is_a_dir),
sizeof(char));
strcpy(m, filename);
strcat(m, is_a_dir);
} else
{
#if HAVE_STAT
int r;
struct stat statbuf;

r = stat(filename, &statbuf);
if (r < 0)
{
m = errno_message(filename);
} else if (force_open)
{
m = NULL;
} else if (!S_ISREG(statbuf.st_mode))
{
static char not_reg[] = " is not a regular file (use -f to see it)";
m = (char *) ecalloc(strlen(filename) + sizeof(not_reg),
sizeof(char));
strcpy(m, filename);
strcat(m, not_reg);
}
#endif
}
return (m);
}





public POSITION
filesize(f)
int f;
{
#if HAVE_STAT
struct stat statbuf;

if (fstat(f, &statbuf) >= 0)
return ((POSITION) statbuf.st_size);
#else
#if defined(_OSK)
long size;

if ((size = (long) _gs_size(f)) >= 0)
return ((POSITION) size);
#endif
#endif
return (seek_filesize(f));
}




public char *
shell_coption(VOID_PARAM)
{
return ("-c");
}




public char *
last_component(name)
char *name;
{
char *slash;

for (slash = name + strlen(name); slash > name; )
{
--slash;
if (*slash == *PATHNAME_SEP || *slash == '/')
return (slash + 1);
}
return (name);
}
