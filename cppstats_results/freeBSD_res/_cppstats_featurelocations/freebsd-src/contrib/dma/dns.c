


































#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <errno.h>
#include <netdb.h>
#include <resolv.h>
#include <string.h>
#include <stdlib.h>

#include "dma.h"

static int
sort_pref(const void *a, const void *b)
{
const struct mx_hostentry *ha = a, *hb = b;
int v;


v = ha->pref - hb->pref;
if (v != 0)
return (v);


v = - (ha->ai.ai_family - hb->ai.ai_family);
return (v);
}

static int
add_host(int pref, const char *host, int port, struct mx_hostentry **he, size_t *ps)
{
struct addrinfo hints, *res, *res0 = NULL;
char servname[10];
struct mx_hostentry *p;
const int count_inc = 10;

memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = IPPROTO_TCP;

snprintf(servname, sizeof(servname), "%d", port);
switch (getaddrinfo(host, servname, &hints, &res0)) {
case 0:
break;
case EAI_AGAIN:
case EAI_NONAME:











goto out;
default:
return(-1);
}

for (res = res0; res != NULL; res = res->ai_next) {
if (*ps + 1 >= roundup(*ps, count_inc)) {
size_t newsz = roundup(*ps + 2, count_inc);
*he = reallocf(*he, newsz * sizeof(**he));
if (*he == NULL)
goto out;
}

p = &(*he)[*ps];
strlcpy(p->host, host, sizeof(p->host));
p->pref = pref;
p->ai = *res;
p->ai.ai_addr = NULL;
bcopy(res->ai_addr, &p->sa, p->ai.ai_addrlen);

getnameinfo((struct sockaddr *)&p->sa, p->ai.ai_addrlen,
p->addr, sizeof(p->addr),
NULL, 0, NI_NUMERICHOST);

(*ps)++;
}
freeaddrinfo(res0);

return (0);

out:
if (res0 != NULL)
freeaddrinfo(res0);
return (1);
}

int
dns_get_mx_list(const char *host, int port, struct mx_hostentry **he, int no_mx)
{
char outname[MAXDNAME];
ns_msg msg;
ns_rr rr;
const char *searchhost;
const unsigned char *cp;
unsigned char *ans;
struct mx_hostentry *hosts = NULL;
size_t nhosts = 0;
size_t anssz;
int pref;
int cname_recurse;
int have_mx = 0;
int err;
int i;

res_init();
searchhost = host;
cname_recurse = 0;

anssz = 65536;
ans = malloc(anssz);
if (ans == NULL)
return (1);

if (no_mx)
goto out;

repeat:
err = res_search(searchhost, ns_c_in, ns_t_mx, ans, anssz);
if (err < 0) {
switch (h_errno) {
case NO_DATA:




goto out;
case TRY_AGAIN:

goto transerr;
case NO_RECOVERY:
case HOST_NOT_FOUND:
default:
errno = ENOENT;
goto err;
}
}

if (!ns_initparse(ans, anssz, &msg))
goto transerr;

switch (ns_msg_getflag(msg, ns_f_rcode)) {
case ns_r_noerror:
break;
case ns_r_nxdomain:
goto err;
default:
goto transerr;
}

for (i = 0; i < ns_msg_count(msg, ns_s_an); i++) {
if (ns_parserr(&msg, ns_s_an, i, &rr))
goto transerr;

cp = ns_rr_rdata(rr);

switch (ns_rr_type(rr)) {
case ns_t_mx:
have_mx = 1;
pref = ns_get16(cp);
cp += 2;
err = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),
cp, outname, sizeof(outname));
if (err < 0)
goto transerr;

err = add_host(pref, outname, port, &hosts, &nhosts);
if (err == -1)
goto err;
break;

case ns_t_cname:
err = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),
cp, outname, sizeof(outname));
if (err < 0)
goto transerr;


if (cname_recurse++ > 10)
goto err;

searchhost = outname;
goto repeat;

default:
break;
}
}

out:
err = 0;
if (0) {
transerr:
if (nhosts == 0)
err = 1;
}
if (0) {
err:
err = -1;
}

free(ans);

if (err == 0) {
if (!have_mx) {



err = add_host(0, host, port, &hosts, &nhosts);
} else if (nhosts == 0) {




err = 1;
}
}

if (nhosts > 0) {
qsort(hosts, nhosts, sizeof(*hosts), sort_pref);

*hosts[nhosts].host = 0;
} else {
if (hosts != NULL)
free(hosts);
hosts = NULL;
}

*he = hosts;
return (err);
}

#if defined(TESTING)
int
main(int argc, char **argv)
{
struct mx_hostentry *he, *p;
int err;

err = dns_get_mx_list(argv[1], 53, &he, 0);
if (err)
return (err);

for (p = he; *p->host != 0; p++) {
printf("%d\t%s\t%s\n", p->pref, p->host, p->addr);
}

return (0);
}
#endif
