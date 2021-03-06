
























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/sysctl.h>
#include <err.h>
#include <errno.h>
#include <kenv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(void);
static int kdumpenv(int dump_type);
static int kgetenv(const char *);
static int ksetenv(const char *, char *);
static int kunsetenv(const char *);

static int hflag = 0;
static int lflag = 0;
static int Nflag = 0;
static int qflag = 0;
static int sflag = 0;
static int uflag = 0;
static int vflag = 0;

static void
usage(void)
{
(void)fprintf(stderr, "%s\n%s\n%s\n",
"usage: kenv [-l|-s] [-hNq]",
" kenv [-qv] variable[=value]",
" kenv [-q] -u variable");
exit(1);
}

int
main(int argc, char **argv)
{
char *env, *eq, *val;
int ch, error;

val = NULL;
env = NULL;
while ((ch = getopt(argc, argv, "hlNqsuv")) != -1) {
switch (ch) {
case 'h':
hflag++;
break;
case 'l':
lflag++;
break;
case 'N':
Nflag++;
break;
case 'q':
qflag++;
break;
case 's':
sflag++;
break;
case 'u':
uflag++;
break;
case 'v':
vflag++;
break;
default:
usage();
}
}
argc -= optind;
argv += optind;
if (argc > 0) {
env = argv[0];
eq = strchr(env, '=');
if (eq != NULL) {
*eq++ = '\0';
val = eq;
}
argv++;
argc--;
}
if ((hflag || Nflag) && env != NULL)
usage();
if (lflag && sflag)
usage();
if (argc > 0 || ((uflag || vflag) && env == NULL))
usage();
if (env == NULL) {
if (lflag)
error = kdumpenv(KENV_DUMP_LOADER);
else if (sflag)
error = kdumpenv(KENV_DUMP_STATIC);
else
error = kdumpenv(KENV_DUMP);
if (error && !qflag) {
if (errno == ENOENT)
warnx("requested environment is unavailable");
else
warn("kdumpenv");
}
} else if (val == NULL) {
if (uflag) {
error = kunsetenv(env);
if (error && !qflag)
warnx("unable to unset %s", env);
} else {
error = kgetenv(env);
if (error && !qflag)
warnx("unable to get %s", env);
}
} else {
error = ksetenv(env, val);
if (error && !qflag)
warnx("unable to set %s to %s", env, val);
}
return (error);
}

static int
kdumpenv(int dump_type)
{
char *buf, *bp, *cp;
int buflen, envlen;

envlen = kenv(dump_type, NULL, NULL, 0);
if (envlen < 0)
return (-1);
for (;;) {
buflen = envlen * 120 / 100;
buf = calloc(1, buflen + 1);
if (buf == NULL)
return (-1);
envlen = kenv(dump_type, NULL, buf, buflen);
if (envlen < 0) {
free(buf);
return (-1);
}
if (envlen > buflen)
free(buf);
else
break;
}

for (bp = buf; *bp != '\0'; bp += strlen(bp) + 1) {
if (hflag) {
if (strncmp(bp, "hint.", 5) != 0)
continue;
}
cp = strchr(bp, '=');
if (cp == NULL)
continue;
*cp++ = '\0';
if (Nflag)
printf("%s\n", bp);
else
printf("%s=\"%s\"\n", bp, cp);
bp = cp;
}

free(buf);
return (0);
}

static int
kgetenv(const char *env)
{
char buf[1024];
int ret;

ret = kenv(KENV_GET, env, buf, sizeof(buf));
if (ret == -1)
return (ret);
if (vflag)
printf("%s=\"%s\"\n", env, buf);
else
printf("%s\n", buf);
return (0);
}

static int
ksetenv(const char *env, char *val)
{
int ret;

ret = kenv(KENV_SET, env, val, strlen(val) + 1);
if (ret == 0)
printf("%s=\"%s\"\n", env, val);
return (ret);
}

static int
kunsetenv(const char *env)
{
int ret;

ret = kenv(KENV_UNSET, env, NULL, 0);
return (ret);
}
