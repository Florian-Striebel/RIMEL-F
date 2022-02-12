





























#if !defined(_BLACKLIST_H)
#define _BLACKLIST_H

#include <sys/socket.h>

__BEGIN_DECLS
struct blacklist *blacklist_open(void);
void blacklist_close(struct blacklist *);
int blacklist(int, int, const char *);
int blacklist_r(struct blacklist *, int, int, const char *);
int blacklist_sa(int, int, const struct sockaddr *, socklen_t, const char *);
int blacklist_sa_r(struct blacklist *, int, int,
const struct sockaddr *, socklen_t, const char *);
__END_DECLS


#define BLACKLIST_API_ENUM 1
enum {
BLACKLIST_AUTH_OK = 0,
BLACKLIST_AUTH_FAIL,
BLACKLIST_ABUSIVE_BEHAVIOR,
BLACKLIST_BAD_USER
};

#endif
