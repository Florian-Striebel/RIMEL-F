






























#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1985, 1987, 1988, 1993\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if 0
#if !defined(lint)
static char sccsid[] = "@(#)date.c 8.2 (Berkeley) 4/28/95";
#endif
#endif

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <ctype.h>
#include <err.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <utmpx.h>

#include "vary.h"

#if !defined(TM_YEAR_BASE)
#define TM_YEAR_BASE 1900
#endif

static time_t tval;

static void badformat(void);
static void iso8601_usage(const char *);
static void multipleformats(void);
static void printdate(const char *);
static void printisodate(struct tm *);
static void setthetime(const char *, const char *, int);
static void usage(void);

static const struct iso8601_fmt {
const char *refname;
const char *format_string;
} iso8601_fmts[] = {
{ "date", "%Y-%m-%d" },
{ "hours", "T%H" },
{ "minutes", ":%M" },
{ "seconds", ":%S" },
};
static const struct iso8601_fmt *iso8601_selected;

static const char *rfc2822_format = "%a, %d %b %Y %T %z";

int
main(int argc, char *argv[])
{
int ch, rflag;
bool Iflag, jflag, Rflag;
const char *format;
char buf[1024];
char *fmt;
char *tmp;
struct vary *v;
const struct vary *badv;
struct tm *lt;
struct stat sb;
size_t i;

v = NULL;
fmt = NULL;
(void) setlocale(LC_TIME, "");
rflag = 0;
Iflag = jflag = Rflag = 0;
while ((ch = getopt(argc, argv, "f:I::jnRr:uv:")) != -1)
switch((char)ch) {
case 'f':
fmt = optarg;
break;
case 'I':
if (Rflag)
multipleformats();
Iflag = 1;
if (optarg == NULL) {
iso8601_selected = iso8601_fmts;
break;
}
for (i = 0; i < nitems(iso8601_fmts); i++)
if (strcmp(optarg, iso8601_fmts[i].refname) == 0)
break;
if (i == nitems(iso8601_fmts))
iso8601_usage(optarg);

iso8601_selected = &iso8601_fmts[i];
break;
case 'j':
jflag = 1;
break;
case 'n':
break;
case 'R':
if (Iflag)
multipleformats();
Rflag = 1;
break;
case 'r':
rflag = 1;
tval = strtoq(optarg, &tmp, 0);
if (*tmp != 0) {
if (stat(optarg, &sb) == 0)
tval = sb.st_mtim.tv_sec;
else
usage();
}
break;
case 'u':
(void)setenv("TZ", "UTC0", 1);
break;
case 'v':
v = vary_append(v, optarg);
break;
default:
usage();
}
argc -= optind;
argv += optind;

if (!rflag && time(&tval) == -1)
err(1, "time");

format = "%+";

if (Rflag)
format = rfc2822_format;


if (*argv && **argv == '+') {
if (Iflag)
multipleformats();
format = *argv + 1;
++argv;
}

if (*argv) {
setthetime(fmt, *argv, jflag);
++argv;
} else if (fmt != NULL)
usage();

if (*argv && **argv == '+') {
if (Iflag)
multipleformats();
format = *argv + 1;
}

lt = localtime(&tval);
if (lt == NULL)
errx(1, "invalid time");
badv = vary_apply(v, lt);
if (badv) {
fprintf(stderr, "%s: Cannot apply date adjustment\n",
badv->arg);
vary_destroy(v);
usage();
}
vary_destroy(v);

if (Iflag)
printisodate(lt);

if (format == rfc2822_format)




setlocale(LC_TIME, "C");

(void)strftime(buf, sizeof(buf), format, lt);
printdate(buf);
}

static void
printdate(const char *buf)
{
(void)printf("%s\n", buf);
if (fflush(stdout))
err(1, "stdout");
exit(EXIT_SUCCESS);
}

static void
printisodate(struct tm *lt)
{
const struct iso8601_fmt *it;
char fmtbuf[32], buf[32], tzbuf[8];

fmtbuf[0] = 0;
for (it = iso8601_fmts; it <= iso8601_selected; it++)
strlcat(fmtbuf, it->format_string, sizeof(fmtbuf));

(void)strftime(buf, sizeof(buf), fmtbuf, lt);

if (iso8601_selected > iso8601_fmts) {
(void)strftime(tzbuf, sizeof(tzbuf), "%z", lt);
memmove(&tzbuf[4], &tzbuf[3], 3);
tzbuf[3] = ':';
strlcat(buf, tzbuf, sizeof(buf));
}

printdate(buf);
}

#define ATOI2(s) ((s) += 2, ((s)[-2] - '0') * 10 + ((s)[-1] - '0'))

static void
setthetime(const char *fmt, const char *p, int jflag)
{
struct utmpx utx;
struct tm *lt;
struct timeval tv;
const char *dot, *t;
int century;

lt = localtime(&tval);
if (lt == NULL)
errx(1, "invalid time");
lt->tm_isdst = -1;

if (fmt != NULL) {
t = strptime(p, fmt, lt);
if (t == NULL) {
fprintf(stderr, "Failed conversion of ``%s''"
" using format ``%s''\n", p, fmt);
badformat();
} else if (*t != '\0')
fprintf(stderr, "Warning: Ignoring %ld extraneous"
" characters in date string (%s)\n",
(long) strlen(t), t);
} else {
for (t = p, dot = NULL; *t; ++t) {
if (isdigit(*t))
continue;
if (*t == '.' && dot == NULL) {
dot = t;
continue;
}
badformat();
}

if (dot != NULL) {
dot++;
if (strlen(dot) != 2)
badformat();
lt->tm_sec = ATOI2(dot);
if (lt->tm_sec > 61)
badformat();
} else
lt->tm_sec = 0;

century = 0;

switch (strlen(p) - ((dot != NULL) ? 3 : 0)) {
case 12:
lt->tm_year = ATOI2(p) * 100 - TM_YEAR_BASE;
century = 1;

case 10:
if (century)
lt->tm_year += ATOI2(p);
else {
lt->tm_year = ATOI2(p);
if (lt->tm_year < 69)
lt->tm_year += 2000 - TM_YEAR_BASE;
else
lt->tm_year += 1900 - TM_YEAR_BASE;
}

case 8:
lt->tm_mon = ATOI2(p);
if (lt->tm_mon > 12)
badformat();
--lt->tm_mon;

case 6:
lt->tm_mday = ATOI2(p);
if (lt->tm_mday > 31)
badformat();

case 4:
lt->tm_hour = ATOI2(p);
if (lt->tm_hour > 23)
badformat();

case 2:
lt->tm_min = ATOI2(p);
if (lt->tm_min > 59)
badformat();
break;
default:
badformat();
}
}


if ((tval = mktime(lt)) == -1)
errx(1, "nonexistent time");

if (!jflag) {
utx.ut_type = OLD_TIME;
memset(utx.ut_id, 0, sizeof(utx.ut_id));
(void)gettimeofday(&utx.ut_tv, NULL);
pututxline(&utx);
tv.tv_sec = tval;
tv.tv_usec = 0;
if (settimeofday(&tv, NULL) != 0)
err(1, "settimeofday (timeval)");
utx.ut_type = NEW_TIME;
(void)gettimeofday(&utx.ut_tv, NULL);
pututxline(&utx);

if ((p = getlogin()) == NULL)
p = "???";
syslog(LOG_AUTH | LOG_NOTICE, "date set by %s", p);
}
}

static void
badformat(void)
{
warnx("illegal time format");
usage();
}

static void
iso8601_usage(const char *badarg)
{
errx(1, "invalid argument '%s' for -I", badarg);
}

static void
multipleformats(void)
{
errx(1, "multiple output formats specified");
}

static void
usage(void)
{
(void)fprintf(stderr, "%s\n%s\n%s\n",
"usage: date [-jnRu] [-I[date|hours|minutes|seconds]] [-f input_fmt]",
" "
"[-r filename|seconds] [-v[+|-]val[y|m|w|d|H|M|S]]",
" "
"[[[[[[cc]yy]mm]dd]HH]MM[.SS] | new_date] [+output_fmt]"
);
exit(1);
}
