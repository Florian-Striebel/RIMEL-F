





























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD: internal.c,v 1.5 2015/01/27 19:40:37 christos Exp $");

#include <stdio.h>
#include <syslog.h>
#include "conf.h"
#include "internal.h"

int debug;
const char *rulename = "blacklistd";
const char *controlprog = _PATH_BLCONTROL;
struct confset lconf, rconf;
struct ifaddrs *ifas;
void (*lfun)(int, const char *, ...) = syslog;
