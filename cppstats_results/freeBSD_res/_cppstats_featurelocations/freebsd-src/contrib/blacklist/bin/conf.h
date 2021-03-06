





























#if !defined(_CONF_H)
#define _CONF_H

#include <sys/socket.h>

struct conf {
struct sockaddr_storage c_ss;
int c_lmask;
int c_port;
int c_proto;
int c_family;
int c_uid;
int c_nfail;
char c_name[128];
int c_rmask;
int c_duration;
};

struct confset {
struct conf *cs_c;
size_t cs_n;
size_t cs_m;
};

#define CONFNAMESZ sizeof(((struct conf *)0)->c_name)

__BEGIN_DECLS
const char *conf_print(char *, size_t, const char *, const char *,
const struct conf *);
void conf_parse(const char *);
const struct conf *conf_find(int, uid_t, const struct sockaddr_storage *,
struct conf *);
__END_DECLS

#endif
