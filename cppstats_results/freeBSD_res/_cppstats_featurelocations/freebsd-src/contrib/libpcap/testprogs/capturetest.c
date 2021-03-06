




















#include "varattrs.h"

#if !defined(lint)
static const char copyright[] _U_ =
"@(#) Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 2000\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#if defined(_WIN32)
#include "getopt.h"
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <sys/types.h>

#include <pcap.h>

#include "pcap/funcattrs.h"

#if defined(_WIN32)
#include "portability.h"
#endif

static char *program_name;


static void countme(u_char *, const struct pcap_pkthdr *, const u_char *);
static void PCAP_NORETURN usage(void);
static void PCAP_NORETURN error(const char *, ...) PCAP_PRINTFLIKE(1, 2);
static void warning(const char *, ...) PCAP_PRINTFLIKE(1, 2);
static char *copy_argv(char **);

static pcap_t *pd;

int
main(int argc, char **argv)
{
register int op;
register char *cp, *cmdbuf, *device;
long longarg;
char *p;
int timeout = 1000;
int immediate = 0;
int nonblock = 0;
pcap_if_t *devlist;
bpf_u_int32 localnet, netmask;
struct bpf_program fcode;
char ebuf[PCAP_ERRBUF_SIZE];
int status;
int packet_count;

device = NULL;
if ((cp = strrchr(argv[0], '/')) != NULL)
program_name = cp + 1;
else
program_name = argv[0];

opterr = 0;
while ((op = getopt(argc, argv, "i:mnt:")) != -1) {
switch (op) {

case 'i':
device = optarg;
break;

case 'm':
immediate = 1;
break;

case 'n':
nonblock = 1;
break;

case 't':
longarg = strtol(optarg, &p, 10);
if (p == optarg || *p != '\0') {
error("Timeout value \"%s\" is not a number",
optarg);

}
if (longarg < 0) {
error("Timeout value %ld is negative", longarg);

}
if (longarg > INT_MAX) {
error("Timeout value %ld is too large (> %d)",
longarg, INT_MAX);

}
timeout = (int)longarg;
break;

default:
usage();

}
}

if (device == NULL) {
if (pcap_findalldevs(&devlist, ebuf) == -1)
error("%s", ebuf);
if (devlist == NULL)
error("no interfaces available for capture");
device = strdup(devlist->name);
pcap_freealldevs(devlist);
}
*ebuf = '\0';
pd = pcap_create(device, ebuf);
if (pd == NULL)
error("%s", ebuf);
status = pcap_set_snaplen(pd, 65535);
if (status != 0)
error("%s: pcap_set_snaplen failed: %s",
device, pcap_statustostr(status));
if (immediate) {
status = pcap_set_immediate_mode(pd, 1);
if (status != 0)
error("%s: pcap_set_immediate_mode failed: %s",
device, pcap_statustostr(status));
}
status = pcap_set_timeout(pd, timeout);
if (status != 0)
error("%s: pcap_set_timeout failed: %s",
device, pcap_statustostr(status));
status = pcap_activate(pd);
if (status < 0) {



error("%s: %s\n(%s)", device,
pcap_statustostr(status), pcap_geterr(pd));
} else if (status > 0) {




warning("%s: %s\n(%s)", device,
pcap_statustostr(status), pcap_geterr(pd));
}
if (pcap_lookupnet(device, &localnet, &netmask, ebuf) < 0) {
localnet = 0;
netmask = 0;
warning("%s", ebuf);
}
cmdbuf = copy_argv(&argv[optind]);

if (pcap_compile(pd, &fcode, cmdbuf, 1, netmask) < 0)
error("%s", pcap_geterr(pd));

if (pcap_setfilter(pd, &fcode) < 0)
error("%s", pcap_geterr(pd));
if (pcap_setnonblock(pd, nonblock, ebuf) == -1)
error("pcap_setnonblock failed: %s", ebuf);
printf("Listening on %s\n", device);
for (;;) {
packet_count = 0;
status = pcap_dispatch(pd, -1, countme,
(u_char *)&packet_count);
if (status < 0)
break;
if (status != 0) {
printf("%d packets seen, %d packets counted after pcap_dispatch returns\n",
status, packet_count);
}
}
if (status == -2) {





putchar('\n');
}
(void)fflush(stdout);
if (status == -1) {



(void)fprintf(stderr, "%s: pcap_loop: %s\n",
program_name, pcap_geterr(pd));
}
pcap_close(pd);
pcap_freecode(&fcode);
free(cmdbuf);
exit(status == -1 ? 1 : 0);
}

static void
countme(u_char *user, const struct pcap_pkthdr *h _U_, const u_char *sp _U_)
{
int *counterp = (int *)user;

(*counterp)++;
}

static void
usage(void)
{
(void)fprintf(stderr, "Usage: %s [ -mn ] [ -i interface ] [ -t timeout] [expression]\n",
program_name);
exit(1);
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
warning(const char *fmt, ...)
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
