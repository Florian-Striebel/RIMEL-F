


































#if 0
#if !defined(lint)
static char sccsid[] = "@(#)options.c 8.2 (Berkeley) 4/18/94";
#endif
#endif

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mtio.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <paths.h>
#include "pax.h"
#include "options.h"
#include "cpio.h"
#include "tar.h"
#include "extern.h"





static char flgch[] = FLGCH;
static OPLIST *ophead = NULL;
static OPLIST *optail = NULL;

static int no_op(void);
static void printflg(unsigned int);
static int c_frmt(const void *, const void *);
static off_t str_offt(char *);
static char *get_line(FILE *fp);
static void pax_options(int, char **);
static void pax_usage(void);
static void tar_options(int, char **);
static void tar_usage(void);
static void cpio_options(int, char **);
static void cpio_usage(void);


#define GETLINE_FILE_CORRUPT 1
#define GETLINE_OUT_OF_MEM 2
static int get_line_error;

char *chdname;

#define GZIP_CMD "gzip"
#define COMPRESS_CMD "compress"
#define BZIP2_CMD "bzip2"










FSUB fsub[] = {

{"bcpio", 5120, sizeof(HD_BCPIO), 1, 0, 0, 1, bcpio_id, cpio_strd,
bcpio_rd, bcpio_endrd, cpio_stwr, bcpio_wr, cpio_endwr, cpio_trail,
NULL, rd_wrfile, wr_rdfile, bad_opt},


{"cpio", 5120, sizeof(HD_CPIO), 1, 0, 0, 1, cpio_id, cpio_strd,
cpio_rd, cpio_endrd, cpio_stwr, cpio_wr, cpio_endwr, cpio_trail,
NULL, rd_wrfile, wr_rdfile, bad_opt},


{"sv4cpio", 5120, sizeof(HD_VCPIO), 1, 0, 0, 1, vcpio_id, cpio_strd,
vcpio_rd, vcpio_endrd, cpio_stwr, vcpio_wr, cpio_endwr, cpio_trail,
NULL, rd_wrfile, wr_rdfile, bad_opt},


{"sv4crc", 5120, sizeof(HD_VCPIO), 1, 0, 0, 1, crc_id, crc_strd,
vcpio_rd, vcpio_endrd, crc_stwr, vcpio_wr, cpio_endwr, cpio_trail,
NULL, rd_wrfile, wr_rdfile, bad_opt},


{"tar", 10240, BLKMULT, 0, 1, BLKMULT, 0, tar_id, no_op,
tar_rd, tar_endrd, no_op, tar_wr, tar_endwr, NULL, tar_trail,
rd_wrfile, wr_rdfile, tar_opt},


{"ustar", 10240, BLKMULT, 0, 1, BLKMULT, 0, ustar_id, ustar_strd,
ustar_rd, tar_endrd, ustar_stwr, ustar_wr, tar_endwr, NULL, tar_trail,
rd_wrfile, wr_rdfile, bad_opt},
};
#define F_OCPIO 0
#define F_ACPIO 1
#define F_CPIO 3
#define F_OTAR 4
#define F_TAR 5
#define DEFLT 5






int ford[] = {5, 4, 3, 2, 1, 0, -1 };







void
options(int argc, char **argv)
{




if ((argv0 = strrchr(argv[0], '/')) != NULL)
argv0++;
else
argv0 = argv[0];

if (strcmp(NM_TAR, argv0) == 0) {
tar_options(argc, argv);
return;
}
else if (strcmp(NM_CPIO, argv0) == 0) {
cpio_options(argc, argv);
return;
}



argv0 = NM_PAX;
pax_options(argc, argv);
return;
}







static void
pax_options(int argc, char **argv)
{
int c;
size_t i;
unsigned int flg = 0;
unsigned int bflg = 0;
char *pt;
FSUB tmp;




while ((c=getopt(argc,argv,"ab:cdf:iklno:p:rs:tuvwx:zB:DE:G:HLOPT:U:XYZ"))
!= -1) {
switch (c) {
case 'a':



flg |= AF;
break;
case 'b':



flg |= BF;
if ((wrblksz = (int)str_offt(optarg)) <= 0) {
paxwarn(1, "Invalid block size %s", optarg);
pax_usage();
}
break;
case 'c':



cflag = 1;
flg |= CF;
break;
case 'd':



dflag = 1;
flg |= DF;
break;
case 'f':



arcname = optarg;
flg |= FF;
break;
case 'i':



iflag = 1;
flg |= IF;
break;
case 'k':



kflag = 1;
flg |= KF;
break;
case 'l':



lflag = 1;
flg |= LF;
break;
case 'n':



nflag = 1;
flg |= NF;
break;
case 'o':



flg |= OF;
if (opt_add(optarg) < 0)
pax_usage();
break;
case 'p':



for (pt = optarg; *pt != '\0'; ++pt) {
switch(*pt) {
case 'a':



patime = 0;
break;
case 'e':




pids = 1;
pmode = 1;
patime = 1;
pmtime = 1;
break;
case 'm':



pmtime = 0;
break;
case 'o':



pids = 1;
break;
case 'p':



pmode = 1;
break;
default:
paxwarn(1, "Invalid -p string: %c", *pt);
pax_usage();
break;
}
}
flg |= PF;
break;
case 'r':



flg |= RF;
break;
case 's':



if (rep_add(optarg) < 0) {
pax_usage();
break;
}
flg |= SF;
break;
case 't':



tflag = 1;
flg |= TF;
break;
case 'u':



uflag = 1;
flg |= UF;
break;
case 'v':



vflag = 1;
flg |= VF;
break;
case 'w':



flg |= WF;
break;
case 'x':



tmp.name = optarg;
if ((frmt = (FSUB *)bsearch((void *)&tmp, (void *)fsub,
sizeof(fsub)/sizeof(FSUB), sizeof(FSUB), c_frmt)) != NULL) {
flg |= XF;
break;
}
paxwarn(1, "Unknown -x format: %s", optarg);
(void)fputs("pax: Known -x formats are:", stderr);
for (i = 0; i < (sizeof(fsub)/sizeof(FSUB)); ++i)
(void)fprintf(stderr, " %s", fsub[i].name);
(void)fputs("\n\n", stderr);
pax_usage();
break;
case 'z':



gzip_program = GZIP_CMD;
break;
case 'B':




if ((wrlimit = str_offt(optarg)) <= 0) {
paxwarn(1, "Invalid write limit %s", optarg);
pax_usage();
}
if (wrlimit % BLKMULT) {
paxwarn(1, "Write limit is not a %d byte multiple",
BLKMULT);
pax_usage();
}
flg |= CBF;
break;
case 'D':




Dflag = 1;
flg |= CDF;
break;
case 'E':





flg |= CEF;
if (strcmp(NONE, optarg) == 0)
maxflt = -1;
else if ((maxflt = atoi(optarg)) < 0) {
paxwarn(1, "Error count value must be positive");
pax_usage();
}
break;
case 'G':




if (grp_add(optarg) < 0) {
pax_usage();
break;
}
flg |= CGF;
break;
case 'H':



Hflag = 1;
flg |= CHF;
break;
case 'L':



Lflag = 1;
flg |= CLF;
break;
case 'O':



Oflag = 1;
break;
case 'P':



Lflag = 0;
flg |= CPF;
break;
case 'T':




if (trng_add(optarg) < 0) {
pax_usage();
break;
}
flg |= CTF;
break;
case 'U':




if (usr_add(optarg) < 0) {
pax_usage();
break;
}
flg |= CUF;
break;
case 'X':



Xflag = 1;
flg |= CXF;
break;
case 'Y':




Yflag = 1;
flg |= CYF;
break;
case 'Z':




Zflag = 1;
flg |= CZF;
break;
default:
pax_usage();
break;
}
}






if (ISLIST(flg)) {
act = LIST;
listf = stdout;
bflg = flg & BDLIST;
} else if (ISEXTRACT(flg)) {
act = EXTRACT;
bflg = flg & BDEXTR;
} else if (ISARCHIVE(flg)) {
act = ARCHIVE;
bflg = flg & BDARCH;
} else if (ISAPPND(flg)) {
act = APPND;
bflg = flg & BDARCH;
} else if (ISCOPY(flg)) {
act = COPY;
bflg = flg & BDCOPY;
} else
pax_usage();
if (bflg) {
printflg(flg);
pax_usage();
}






if (!(flg & XF) && (act == ARCHIVE))
frmt = &(fsub[DEFLT]);




switch (act) {
case LIST:
case EXTRACT:
for (; optind < argc; optind++)
if (pat_add(argv[optind], NULL) < 0)
pax_usage();
break;
case COPY:
if (optind >= argc) {
paxwarn(0, "Destination directory was not supplied");
pax_usage();
}
--argc;
dirptr = argv[argc];

case ARCHIVE:
case APPND:
for (; optind < argc; optind++)
if (ftree_add(argv[optind], 0) < 0)
pax_usage();



maxflt = 0;
break;
}
}








static void
tar_options(int argc, char **argv)
{
int c;
int fstdin = 0;
int tar_Oflag = 0;
int nincfiles = 0;
int incfiles_max = 0;
struct incfile {
char *file;
char *dir;
};
struct incfile *incfiles = NULL;




rmleadslash = 1;




while ((c = getoldopt(argc, argv,
"b:cef:hjmopqruts:vwxyzBC:HI:LOPXZ014578")) != -1) {
switch(c) {
case 'b':



if ((wrblksz = (int)str_offt(optarg)) <= 0) {
paxwarn(1, "Invalid block size %s", optarg);
tar_usage();
}
wrblksz *= 512;
break;
case 'c':



act = ARCHIVE;
break;
case 'e':



maxflt = 0;
break;
case 'f':



if ((optarg[0] == '-') && (optarg[1]== '\0')) {



fstdin = 1;
arcname = NULL;
break;
}
fstdin = 0;
arcname = optarg;
break;
case 'h':



Lflag = 1;
break;
case 'j':
case 'y':



gzip_program = BZIP2_CMD;
break;
case 'm':



pmtime = 0;
break;
case 'o':
if (opt_add("write_opt=nodir") < 0)
tar_usage();
case 'O':
tar_Oflag = 1;
break;
case 'p':



pmode = 1;
pids = 1;
break;
case 'q':



nflag = 1;
break;
case 'r':
case 'u':



act = APPND;
break;
case 's':



if (rep_add(optarg) < 0) {
tar_usage();
break;
}
break;
case 't':



act = LIST;
break;
case 'v':



vflag++;
break;
case 'w':



iflag = 1;
break;
case 'x':




act = EXTRACT;
pmtime = 1;
break;
case 'z':



gzip_program = GZIP_CMD;
break;
case 'B':



break;
case 'C':
chdname = optarg;
break;
case 'H':



Hflag = 1;
break;
case 'I':
if (++nincfiles > incfiles_max) {
incfiles_max = nincfiles + 3;
incfiles = realloc(incfiles,
sizeof(*incfiles) * incfiles_max);
if (incfiles == NULL) {
paxwarn(0, "Unable to allocate space "
"for option list");
exit(1);
}
}
incfiles[nincfiles - 1].file = optarg;
incfiles[nincfiles - 1].dir = chdname;
break;
case 'L':



Lflag = 1;
break;
case 'P':



rmleadslash = 0;
break;
case 'X':



Xflag = 1;
break;
case 'Z':



gzip_program = COMPRESS_CMD;
break;
case '0':
arcname = DEV_0;
break;
case '1':
arcname = DEV_1;
break;
case '4':
arcname = DEV_4;
break;
case '5':
arcname = DEV_5;
break;
case '7':
arcname = DEV_7;
break;
case '8':
arcname = DEV_8;
break;
default:
tar_usage();
break;
}
}
argc -= optind;
argv += optind;


if (fstdin == 1 && act == ARCHIVE)
listf = stderr;
else
listf = stdout;


if ((act == ARCHIVE || act == APPND) && argc == 0 && nincfiles == 0)
exit(0);





if (act == ARCHIVE || act == APPND)
frmt = &(fsub[tar_Oflag ? F_OTAR : F_TAR]);
else if (tar_Oflag) {
paxwarn(1, "The -O/-o options are only valid when writing an archive");
tar_usage();
}




switch (act) {
case LIST:
case EXTRACT:
default:
{
int sawpat = 0;
char *file, *dir = NULL;

while (nincfiles || *argv != NULL) {







if (nincfiles) {
file = incfiles->file;
dir = incfiles->dir;
incfiles++;
nincfiles--;
} else if (strcmp(*argv, "-I") == 0) {
if (*++argv == NULL)
break;
file = *argv++;
dir = chdname;
} else
file = NULL;
if (file != NULL) {
FILE *fp;
char *str;

if (strcmp(file, "-") == 0)
fp = stdin;
else if ((fp = fopen(file, "r")) == NULL) {
paxwarn(1, "Unable to open file '%s' for read", file);
tar_usage();
}
while ((str = get_line(fp)) != NULL) {
if (pat_add(str, dir) < 0)
tar_usage();
sawpat = 1;
}
if (strcmp(file, "-") != 0)
fclose(fp);
if (get_line_error) {
paxwarn(1, "Problem with file '%s'", file);
tar_usage();
}
} else if (strcmp(*argv, "-C") == 0) {
if (*++argv == NULL)
break;
chdname = *argv++;
} else if (pat_add(*argv++, chdname) < 0)
tar_usage();
else
sawpat = 1;
}





if (sawpat > 0)
chdname = NULL;
}
break;
case ARCHIVE:
case APPND:
if (chdname != NULL) {
if (ftree_add(chdname, 1) < 0)
tar_usage();
}

while (nincfiles || *argv != NULL) {
char *file, *dir = NULL;







if (nincfiles) {
file = incfiles->file;
dir = incfiles->dir;
incfiles++;
nincfiles--;
} else if (strcmp(*argv, "-I") == 0) {
if (*++argv == NULL)
break;
file = *argv++;
dir = NULL;
} else
file = NULL;
if (file != NULL) {
FILE *fp;
char *str;


if (dir) {
if (ftree_add(dir, 1) < 0)
tar_usage();
}

if (strcmp(file, "-") == 0)
fp = stdin;
else if ((fp = fopen(file, "r")) == NULL) {
paxwarn(1, "Unable to open file '%s' for read", file);
tar_usage();
}
while ((str = get_line(fp)) != NULL) {
if (ftree_add(str, 0) < 0)
tar_usage();
}
if (strcmp(file, "-") != 0)
fclose(fp);
if (get_line_error) {
paxwarn(1, "Problem with file '%s'",
file);
tar_usage();
}
} else if (strcmp(*argv, "-C") == 0) {
if (*++argv == NULL)
break;
if (ftree_add(*argv++, 1) < 0)
tar_usage();
} else if (ftree_add(*argv++, 0) < 0)
tar_usage();
}



maxflt = 0;
break;
}
if (!fstdin && ((arcname == NULL) || (*arcname == '\0'))) {
arcname = getenv("TAPE");
if ((arcname == NULL) || (*arcname == '\0'))
arcname = _PATH_DEFTAPE;
}
}

static int
mkpath(char *path)
{
struct stat sb;
char *slash;
int done = 0;

slash = path;

while (!done) {
slash += strspn(slash, "/");
slash += strcspn(slash, "/");

done = (*slash == '\0');
*slash = '\0';

if (stat(path, &sb)) {
if (errno != ENOENT || mkdir(path, 0777)) {
paxwarn(1, "%s", path);
return (-1);
}
} else if (!S_ISDIR(sb.st_mode)) {
syswarn(1, ENOTDIR, "%s", path);
return (-1);
}

if (!done)
*slash = '/';
}

return (0);
}






static void
cpio_options(int argc, char **argv)
{
int c;
size_t i;
char *str;
FSUB tmp;
FILE *fp;

kflag = 1;
pids = 1;
pmode = 1;
pmtime = 0;
arcname = NULL;
dflag = 1;
act = -1;
nodirs = 1;
while ((c=getopt(argc,argv,"abcdfiklmoprstuvzABC:E:F:H:I:LO:SZ6")) != -1)
switch (c) {
case 'a':



tflag = 1;
break;
case 'b':



break;
case 'c':



frmt = &(fsub[F_ACPIO]);
break;
case 'd':



nodirs = 0;
break;
case 'f':



cflag = 1;
break;
case 'i':



act = EXTRACT;
break;
case 'k':
break;
case 'l':



lflag = 1;
break;
case 'm':



pmtime = 1;
break;
case 'o':



act = ARCHIVE;
frmt = &(fsub[F_CPIO]);
break;
case 'p':



act = COPY;
break;
case 'r':



iflag = 1;
break;
case 's':



break;
case 't':



act = LIST;
listf = stdout;
break;
case 'u':



kflag = 0;
break;
case 'v':



vflag = 1;
break;
case 'z':



gzip_program = GZIP_CMD;
break;
case 'A':



act = APPND;
break;
case 'B':



wrblksz = 5120;
break;
case 'C':



wrblksz = atoi(optarg);
break;
case 'E':



if ((fp = fopen(optarg, "r")) == NULL) {
paxwarn(1, "Unable to open file '%s' for read", optarg);
cpio_usage();
}
while ((str = get_line(fp)) != NULL) {
pat_add(str, NULL);
}
fclose(fp);
if (get_line_error) {
paxwarn(1, "Problem with file '%s'", optarg);
cpio_usage();
}
break;
case 'F':
case 'I':
case 'O':



if ((optarg[0] == '-') && (optarg[1]== '\0')) {



arcname = NULL;
break;
}
arcname = optarg;
break;
case 'H':



tmp.name = optarg;
if ((frmt = (FSUB *)bsearch((void *)&tmp, (void *)fsub,
sizeof(fsub)/sizeof(FSUB), sizeof(FSUB), c_frmt)) != NULL)
break;
paxwarn(1, "Unknown -H format: %s", optarg);
(void)fputs("cpio: Known -H formats are:", stderr);
for (i = 0; i < (sizeof(fsub)/sizeof(FSUB)); ++i)
(void)fprintf(stderr, " %s", fsub[i].name);
(void)fputs("\n\n", stderr);
cpio_usage();
break;
case 'L':



Lflag = 1;
break;
case 'S':



break;
case 'Z':



gzip_program = COMPRESS_CMD;
break;
case '6':



frmt = &(fsub[F_OCPIO]);
break;
case '?':
default:
cpio_usage();
break;
}
argc -= optind;
argv += optind;




switch (act) {
case LIST:
case EXTRACT:
while (*argv != NULL)
if (pat_add(*argv++, NULL) < 0)
cpio_usage();
break;
case COPY:
if (*argv == NULL) {
paxwarn(0, "Destination directory was not supplied");
cpio_usage();
}
dirptr = *argv;
if (mkpath(dirptr) < 0)
cpio_usage();
--argc;
++argv;

case ARCHIVE:
case APPND:
if (*argv != NULL)
cpio_usage();



maxflt = 0;
while ((str = get_line(stdin)) != NULL) {
ftree_add(str, 0);
}
if (get_line_error) {
paxwarn(1, "Problem while reading stdin");
cpio_usage();
}
break;
default:
cpio_usage();
break;
}
}






static void
printflg(unsigned int flg)
{
int nxt;
int pos = 0;

(void)fprintf(stderr,"%s: Invalid combination of options:", argv0);
while ((nxt = ffs(flg)) != 0) {
flg = flg >> nxt;
pos += nxt;
(void)fprintf(stderr, " -%c", flgch[pos-1]);
}
(void)putc('\n', stderr);
}







static int
c_frmt(const void *a, const void *b)
{
return(strcmp(((const FSUB *)a)->name, ((const FSUB *)b)->name));
}









OPLIST *
opt_next(void)
{
OPLIST *opt;

if ((opt = ophead) != NULL)
ophead = ophead->fow;
return(opt);
}







int
bad_opt(void)
{
OPLIST *opt;

if (ophead == NULL)
return(0);



paxwarn(1,"These format options are not supported");
while ((opt = opt_next()) != NULL)
(void)fprintf(stderr, "\t%s = %s\n", opt->name, opt->value);
pax_usage();
return(0);
}










int
opt_add(const char *str)
{
OPLIST *opt;
char *frpt;
char *pt;
char *endpt;
char *lstr;

if ((str == NULL) || (*str == '\0')) {
paxwarn(0, "Invalid option name");
return(-1);
}
if ((lstr = strdup(str)) == NULL) {
paxwarn(0, "Unable to allocate space for option list");
return(-1);
}
frpt = endpt = lstr;






while ((frpt != NULL) && (*frpt != '\0')) {
if ((endpt = strchr(frpt, ',')) != NULL)
*endpt = '\0';
if ((pt = strchr(frpt, '=')) == NULL) {
paxwarn(0, "Invalid options format");
free(lstr);
return(-1);
}
if ((opt = (OPLIST *)malloc(sizeof(OPLIST))) == NULL) {
paxwarn(0, "Unable to allocate space for option list");
free(lstr);
return(-1);
}
lstr = NULL;
*pt++ = '\0';
opt->name = frpt;
opt->value = pt;
opt->fow = NULL;
if (endpt != NULL)
frpt = endpt + 1;
else
frpt = NULL;
if (ophead == NULL) {
optail = ophead = opt;
continue;
}
optail->fow = opt;
optail = opt;
}
free(lstr);
return(0);
}
















static off_t
str_offt(char *val)
{
char *expr;
off_t num, t;

#if defined(NET2_STAT)
num = strtol(val, &expr, 0);
if ((num == LONG_MAX) || (num <= 0) || (expr == val))
#else
num = strtoq(val, &expr, 0);
if ((num == QUAD_MAX) || (num <= 0) || (expr == val))
#endif
return(0);

switch(*expr) {
case 'b':
t = num;
num *= 512;
if (t > num)
return(0);
++expr;
break;
case 'k':
t = num;
num *= 1024;
if (t > num)
return(0);
++expr;
break;
case 'm':
t = num;
num *= 1048576;
if (t > num)
return(0);
++expr;
break;
case 'w':
t = num;
num *= sizeof(int);
if (t > num)
return(0);
++expr;
break;
}

switch(*expr) {
case '\0':
break;
case '*':
case 'x':
t = num;
num *= str_offt(expr + 1);
if (t > num)
return(0);
break;
default:
return(0);
}
return(num);
}

char *
get_line(FILE *f)
{
char *name, *temp;
size_t len;

name = fgetln(f, &len);
if (!name) {
get_line_error = ferror(f) ? GETLINE_FILE_CORRUPT : 0;
return(0);
}
if (name[len-1] != '\n')
len++;
temp = malloc(len);
if (!temp) {
get_line_error = GETLINE_OUT_OF_MEM;
return(0);
}
memcpy(temp, name, len-1);
temp[len-1] = 0;
return(temp);
}








static int
no_op(void)
{
return(0);
}






void
pax_usage(void)
{
(void)fputs("usage: pax [-cdnOvz] [-E limit] [-f archive] ", stderr);
(void)fputs("[-s replstr] ... [-U user] ...", stderr);
(void)fputs("\n [-G group] ... ", stderr);
(void)fputs("[-T [from_date][,to_date]] ... ", stderr);
(void)fputs("[pattern ...]\n", stderr);
(void)fputs(" pax -r [-cdiknOuvzDYZ] [-E limit] ", stderr);
(void)fputs("[-f archive] [-o options] ... \n", stderr);
(void)fputs(" [-p string] ... [-s replstr] ... ", stderr);
(void)fputs("[-U user] ... [-G group] ...\n ", stderr);
(void)fputs("[-T [from_date][,to_date]] ... ", stderr);
(void)fputs(" [pattern ...]\n", stderr);
(void)fputs(" pax -w [-dituvzHLOPX] [-b blocksize] ", stderr);
(void)fputs("[ [-a] [-f archive] ] [-x format] \n", stderr);
(void)fputs(" [-B bytes] [-s replstr] ... ", stderr);
(void)fputs("[-o options] ... [-U user] ...", stderr);
(void)fputs("\n [-G group] ... ", stderr);
(void)fputs("[-T [from_date][,to_date][/[c][m]]] ... ", stderr);
(void)fputs("[file ...]\n", stderr);
(void)fputs(" pax -r -w [-diklntuvDHLOPXYZ] ", stderr);
(void)fputs("[-p string] ... [-s replstr] ...", stderr);
(void)fputs("\n [-U user] ... [-G group] ... ", stderr);
(void)fputs("[-T [from_date][,to_date][/[c][m]]] ... ", stderr);
(void)fputs("\n [file ...] directory\n", stderr);
exit(1);
}






void
tar_usage(void)
{
(void)fputs("usage: tar [-]{crtux}[-befhjmopqsvwyzHLOPXZ014578] [blocksize] ",
stderr);
(void)fputs("[archive] [replstr] [-C directory] [-I file] [file ...]\n",
stderr);
exit(1);
}






void
cpio_usage(void)
{
(void)fputs("usage: cpio -o [-aABcLvVzZ] [-C bytes] [-H format] [-O archive]\n", stderr);
(void)fputs(" [-F archive] < name-list [> archive]\n", stderr);
(void)fputs(" cpio -i [-bBcdfmnrsStuvVzZ6] [-C bytes] [-E file] [-H format]\n", stderr);
(void)fputs(" [-I archive] [-F archive] [pattern...] [< archive]\n", stderr);
(void)fputs(" cpio -p [-adlLmuvV] destination-directory < name-list\n", stderr);
exit(1);
}
