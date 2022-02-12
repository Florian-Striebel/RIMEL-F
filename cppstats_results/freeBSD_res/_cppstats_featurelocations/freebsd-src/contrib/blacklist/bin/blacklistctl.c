





























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD: blacklistctl.c,v 1.23 2018/05/24 19:21:01 christos Exp $");

#include <stdio.h>
#include <time.h>
#if defined(HAVE_LIBUTIL_H)
#include <libutil.h>
#endif
#if defined(HAVE_UTIL_H)
#include <util.h>
#endif
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "conf.h"
#include "state.h"
#include "internal.h"
#include "support.h"

static __dead void
usage(int c)
{
if (c == 0)
warnx("Missing/unknown command");
else
warnx("Unknown option `%c'", (char)c);
fprintf(stderr, "Usage: %s dump [-abdnrw]\n", getprogname());
exit(EXIT_FAILURE);
}

static const char *
star(char *buf, size_t len, int val)
{
if (val == -1)
return "*";
snprintf(buf, len, "%d", val);
return buf;
}

int
main(int argc, char *argv[])
{
const char *dbname = _PATH_BLSTATE;
DB *db;
struct conf c;
struct dbinfo dbi;
unsigned int i;
struct timespec ts;
int all, blocked, remain, wide, noheader;
int o;

noheader = wide = blocked = all = remain = 0;
lfun = dlog;

if (argc == 1 || strcmp(argv[1], "dump") != 0)
usage(0);

argc--;
argv++;

while ((o = getopt(argc, argv, "abD:dnrw")) != -1)
switch (o) {
case 'a':
all = 1;
blocked = 0;
break;
case 'b':
blocked = 1;
break;
case 'D':
dbname = optarg;
break;
case 'd':
debug++;
break;
case 'n':
noheader = 1;
break;
case 'r':
remain = 1;
break;
case 'w':
wide = 1;
break;
default:
usage(o);
break;
}

db = state_open(dbname, O_RDONLY, 0);
if (db == NULL)
err(EXIT_FAILURE, "Can't open `%s'", dbname);

clock_gettime(CLOCK_REALTIME, &ts);
wide = wide ? 8 * 4 + 7 : 4 * 3 + 3;
if (!noheader)
printf("%*.*s/ma:port\tid\tnfail\t%s\n", wide, wide,
"address", remain ? "remaining time" : "last access");
for (i = 1; state_iterate(db, &c, &dbi, i) != 0; i = 0) {
char buf[BUFSIZ];
char mbuf[64], pbuf[64];
if (!all) {
if (blocked) {
if (c.c_nfail == -1 || dbi.count < c.c_nfail)
continue;
} else {
if (dbi.count >= c.c_nfail)
continue;
}
}
sockaddr_snprintf(buf, sizeof(buf), "%a", (void *)&c.c_ss);
printf("%*.*s/%s:%s\t", wide, wide, buf,
star(mbuf, sizeof(mbuf), c.c_lmask),
star(pbuf, sizeof(pbuf), c.c_port));
if (c.c_duration == -1) {
strlcpy(buf, "never", sizeof(buf));
} else {
if (remain)
fmtydhms(buf, sizeof(buf),
c.c_duration - (ts.tv_sec - dbi.last));
else
fmttime(buf, sizeof(buf), dbi.last);
}
printf("%s\t%d/%s\t%-s\n", dbi.id, dbi.count,
star(mbuf, sizeof(mbuf), c.c_nfail), buf);
}
state_close(db);
return EXIT_SUCCESS;
}
