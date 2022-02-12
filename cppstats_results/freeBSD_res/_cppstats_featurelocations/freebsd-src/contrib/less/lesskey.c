

















































































#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lesskey.h"
#include "cmd.h"
#include "defines.h"

char fileheader[] = {
C0_LESSKEY_MAGIC,
C1_LESSKEY_MAGIC,
C2_LESSKEY_MAGIC,
C3_LESSKEY_MAGIC
};
char filetrailer[] = {
C0_END_LESSKEY_MAGIC,
C1_END_LESSKEY_MAGIC,
C2_END_LESSKEY_MAGIC
};
char cmdsection[1] = { CMD_SECTION };
char editsection[1] = { EDIT_SECTION };
char varsection[1] = { VAR_SECTION };
char endsection[1] = { END_SECTION };

char *infile = NULL;
char *outfile = NULL ;

extern char version[];

static void
usage(void)
{
fprintf(stderr, "usage: lesskey [-o output] [input]\n");
exit(1);
}

void
lesskey_parse_error(s)
char *s;
{
fprintf(stderr, "%s\n", s);
}

void *
ecalloc(count, size)
int count;
unsigned int size;
{
void *p;

p = calloc(count, size);
if (p != NULL)
return (p);
fprintf(stderr, "lesskey: cannot allocate %d bytes of memory\n", count*size);
exit(1);
}

static char *
mkpathname(dirname, filename)
char *dirname;
char *filename;
{
char *pathname;

pathname = ecalloc(strlen(dirname) + strlen(filename) + 2, sizeof(char));
strcpy(pathname, dirname);
strcat(pathname, PATHNAME_SEP);
strcat(pathname, filename);
return (pathname);
}




char *
homefile(filename)
char *filename;
{
char *p;
char *pathname;

if ((p = getenv("HOME")) != NULL && *p != '\0')
pathname = mkpathname(p, filename);
#if OS2
else if ((p = getenv("INIT")) != NULL && *p != '\0')
pathname = mkpathname(p, filename);
#endif
else
{
fprintf(stderr, "cannot find $HOME - using current directory\n");
pathname = mkpathname(".", filename);
}
return (pathname);
}




static void
parse_args(argc, argv)
int argc;
char **argv;
{
char *arg;

outfile = NULL;
while (--argc > 0)
{
arg = *++argv;
if (arg[0] != '-')

break;
if (arg[1] == '\0')

break;
if (arg[1] == '-' && arg[2] == '\0')
{

argc--;
argv++;
break;
}
switch (arg[1])
{
case '-':
if (strncmp(arg, "--output", 8) == 0)
{
if (arg[8] == '\0')
outfile = &arg[8];
else if (arg[8] == '=')
outfile = &arg[9];
else
usage();
goto opt_o;
}
if (strcmp(arg, "--version") == 0)
{
goto opt_V;
}
usage();
break;
case 'o':
outfile = &argv[0][2];
opt_o:
if (*outfile == '\0')
{
if (--argc <= 0)
usage();
outfile = *(++argv);
}
break;
case 'V':
opt_V:
printf("lesskey version %s\n", version);
exit(0);
default:
usage();
}
}
if (argc > 1)
usage();



if (argc > 0)
infile = *argv;
}




static void
fputbytes(fd, buf, len)
FILE *fd;
char *buf;
int len;
{
while (len-- > 0)
{
fwrite(buf, sizeof(char), 1, fd);
buf++;
}
}




static void
fputint(fd, val)
FILE *fd;
unsigned int val;
{
char c;

if (val >= KRADIX*KRADIX)
{
fprintf(stderr, "error: cannot write %d, max %d\n",
val, KRADIX*KRADIX);
exit(1);
}
c = val % KRADIX;
fwrite(&c, sizeof(char), 1, fd);
c = val / KRADIX;
fwrite(&c, sizeof(char), 1, fd);
}

int
main(argc, argv)
int argc;
char *argv[];
{
struct lesskey_tables tables;
FILE *out;
int errors;

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
#endif




parse_args(argc, argv);
errors = parse_lesskey(infile, &tables);
if (errors)
{
fprintf(stderr, "%d errors; no output produced\n", errors);
return (1);
}

fprintf(stderr, "NOTE: lesskey is deprecated.\n It is no longer necessary to run lesskey,\n when using less version 582 and later.\n");





if (outfile == NULL)
outfile = getenv("LESSKEY");
if (outfile == NULL)
outfile = homefile(LESSKEYFILE);
if ((out = fopen(outfile, "wb")) == NULL)
{
#if HAVE_PERROR
perror(outfile);
#else
fprintf(stderr, "Cannot open %s\n", outfile);
#endif
return (1);
}


fputbytes(out, fileheader, sizeof(fileheader));


fputbytes(out, cmdsection, sizeof(cmdsection));
fputint(out, tables.cmdtable.buf.end);
fputbytes(out, (char *)tables.cmdtable.buf.data, tables.cmdtable.buf.end);

fputbytes(out, editsection, sizeof(editsection));
fputint(out, tables.edittable.buf.end);
fputbytes(out, (char *)tables.edittable.buf.data, tables.edittable.buf.end);


fputbytes(out, varsection, sizeof(varsection));
fputint(out, tables.vartable.buf.end);
fputbytes(out, (char *)tables.vartable.buf.data, tables.vartable.buf.end);


fputbytes(out, endsection, sizeof(endsection));
fputbytes(out, filetrailer, sizeof(filetrailer));
return (0);
}
