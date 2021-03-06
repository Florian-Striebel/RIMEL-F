





























#if !defined(_INTERNAL_H)
#define _INTERNAL_H

#if !defined(_PATH_BLCONF)
#define _PATH_BLCONF "/etc/blacklistd.conf"
#endif
#if !defined(_PATH_BLCONTROL)
#define _PATH_BLCONTROL "/libexec/blacklistd-helper"
#endif
#if !defined(_PATH_BLSTATE)
#define _PATH_BLSTATE "/var/db/blacklistd.db"
#endif

extern struct confset rconf, lconf;
extern int debug;
extern const char *rulename;
extern const char *controlprog;
extern struct ifaddrs *ifas;

#if !defined(__syslog_attribute__) && !defined(__syslog__)
#define __syslog__ __printf__
#endif

extern void (*lfun)(int, const char *, ...)
__attribute__((__format__(__syslog__, 2, 3)));

#endif
