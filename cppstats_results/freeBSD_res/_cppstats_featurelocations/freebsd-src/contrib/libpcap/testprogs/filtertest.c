




















#include "varattrs.h"

#if !defined(lint)
static const char copyright[] _U_ =
"@(#) Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 2000\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#if defined(_WIN32)
#include "getopt.h"
#include "unix.h"
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <errno.h>
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "pcap/funcattrs.h"

#if defined(BDEBUG)







PCAP_API void pcap_set_optimizer_debug(int);
PCAP_API void pcap_set_print_dot_graph(int);
#endif

static char *program_name;


static void PCAP_NORETURN usage(void);
static void PCAP_NORETURN error(const char *, ...) PCAP_PRINTFLIKE(1, 2);
static void warn(const char *, ...) PCAP_PRINTFLIKE(1, 2);







#if !defined(O_BINARY)
#define O_BINARY 0
#endif

static char *
read_infile(char *fname)
{
register int i, fd, cc;
register char *cp;
struct stat buf;

fd = open(fname, O_RDONLY|O_BINARY);
if (fd < 0)
error("can't open %s: %s", fname, pcap_strerror(errno));

if (fstat(fd, &buf) < 0)
error("can't stat %s: %s", fname, pcap_strerror(errno));

cp = malloc((u_int)buf.st_size + 1);
if (cp == NULL)
error("malloc(%d) for %s: %s", (u_int)buf.st_size + 1,
fname, pcap_strerror(errno));
cc = read(fd, cp, (u_int)buf.st_size);
if (cc < 0)
error("read %s: %s", fname, pcap_strerror(errno));
if (cc != buf.st_size)
error("short read %s (%d != %d)", fname, cc, (int)buf.st_size);

close(fd);

for (i = 0; i < cc; i++) {
if (cp[i] == '#')
while (i < cc && cp[i] != '\n')
cp[i++] = ' ';
}
cp[cc] = '\0';
return (cp);
}


static void
error(const char *fmt, ...)
{
va_list ap;

(void)fprintf(stderr, "%s: ", program_name);
va_start(ap, fmt);
(void)vfprintf(stderr, fmt, ap);
va_end(ap);
if (*fmt) {
fmt += strlen(fmt);
if (fmt[-1] != '\n')
(void)fputc('\n', stderr);
}
exit(1);

}


static void
warn(const char *fmt, ...)
{
va_list ap;

(void)fprintf(stderr, "%s: WARNING: ", program_name);
va_start(ap, fmt);
(void)vfprintf(stderr, fmt, ap);
va_end(ap);
if (*fmt) {
fmt += strlen(fmt);
if (fmt[-1] != '\n')
(void)fputc('\n', stderr);
}
}




static char *
copy_argv(register char **argv)
{
register char **p;
register u_int len = 0;
char *buf;
char *src, *dst;

p = argv;
if (*p == 0)
return 0;

while (*p)
len += strlen(*p++) + 1;

buf = (char *)malloc(len);
if (buf == NULL)
error("copy_argv: malloc");

p = argv;
dst = buf;
while ((src = *p++) != NULL) {
while ((*dst++ = *src++) != '\0')
;
dst[-1] = ' ';
}
dst[-1] = '\0';

return buf;
}

int
main(int argc, char **argv)
{
char *cp;
int op;
int dflag;
int gflag;
char *infile;
int Oflag;
long snaplen;
char *p;
int dlt;
int have_fcode = 0;
bpf_u_int32 netmask = PCAP_NETMASK_UNKNOWN;
char *cmdbuf;
pcap_t *pd;
struct bpf_program fcode;

#if defined(_WIN32)
if (pcap_wsockinit() != 0)
return 1;
#endif

dflag = 1;
gflag = 0;

infile = NULL;
Oflag = 1;
snaplen = 68;

if ((cp = strrchr(argv[0], '/')) != NULL)
program_name = cp + 1;
else
program_name = argv[0];

opterr = 0;
while ((op = getopt(argc, argv, "dF:gm:Os:")) != -1) {
switch (op) {

case 'd':
++dflag;
break;

case 'g':
#if defined(BDEBUG)
++gflag;
#else
error("libpcap and filtertest not built with optimizer debugging enabled");
#endif
break;

case 'F':
infile = optarg;
break;

case 'O':
Oflag = 0;
break;

case 'm': {
bpf_u_int32 addr;

switch (inet_pton(AF_INET, optarg, &addr)) {

case 0:
error("invalid netmask %s", optarg);
break;

case -1:
error("invalid netmask %s: %s", optarg,
pcap_strerror(errno));
break;

case 1:
netmask = addr;
break;
}
break;
}

case 's': {
char *end;

snaplen = strtol(optarg, &end, 0);
if (optarg == end || *end != '\0'
|| snaplen < 0 || snaplen > 65535)
error("invalid snaplen %s", optarg);
else if (snaplen == 0)
snaplen = 65535;
break;
}

default:
usage();

}
}

if (optind >= argc) {
usage();

}

dlt = pcap_datalink_name_to_val(argv[optind]);
if (dlt < 0) {
dlt = (int)strtol(argv[optind], &p, 10);
if (p == argv[optind] || *p != '\0')
error("invalid data link type %s", argv[optind]);
}

if (infile)
cmdbuf = read_infile(infile);
else
cmdbuf = copy_argv(&argv[optind+1]);

#if defined(BDEBUG)
pcap_set_optimizer_debug(dflag);
pcap_set_print_dot_graph(gflag);
#endif

pd = pcap_open_dead(dlt, snaplen);
if (pd == NULL)
error("Can't open fake pcap_t");

if (pcap_compile(pd, &fcode, cmdbuf, Oflag, netmask) < 0)
error("%s", pcap_geterr(pd));

have_fcode = 1;
if (!bpf_validate(fcode.bf_insns, fcode.bf_len))
warn("Filter doesn't pass validation");

#if defined(BDEBUG)
if (cmdbuf != NULL) {

for (cp = cmdbuf; *cp != '\0'; ++cp) {
if (*cp == '\r' || *cp == '\n') {
*cp = ' ';
}
}

printf("machine codes for filter: %s\n", cmdbuf);
} else
printf("machine codes for empty filter:\n");
#endif

bpf_dump(&fcode, dflag);
free(cmdbuf);
if (have_fcode)
pcap_freecode (&fcode);
pcap_close(pd);
exit(0);
}

static void
usage(void)
{
(void)fprintf(stderr, "%s, with %s\n", program_name,
pcap_lib_version());
(void)fprintf(stderr,
#if defined(BDEBUG)
"Usage: %s [-dgO] [ -F file ] [ -m netmask] [ -s snaplen ] dlt [ expression ]\n",
#else
"Usage: %s [-dO] [ -F file ] [ -m netmask] [ -s snaplen ] dlt [ expression ]\n",
#endif
program_name);
exit(1);
}
