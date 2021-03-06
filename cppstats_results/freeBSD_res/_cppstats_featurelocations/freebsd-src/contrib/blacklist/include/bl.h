





























#if !defined(_BL_H)
#define _BL_H

#include <stdbool.h>
#include <stdarg.h>
#include <sys/param.h>
#include <sys/socket.h>
#include "blacklist.h"

typedef enum {
BL_INVALID,
BL_ADD,
BL_DELETE,
BL_ABUSE,
BL_BADUSER
} bl_type_t;

typedef struct {
bl_type_t bi_type;
int bi_fd;
uid_t bi_uid;
gid_t bi_gid;
socklen_t bi_slen;
struct sockaddr_storage bi_ss;
char bi_msg[1024];
} bl_info_t;

#define bi_cred bi_u._bi_cred

#if !defined(_PATH_BLSOCK)
#define _PATH_BLSOCK "/var/run/blacklistd.sock"
#endif

__BEGIN_DECLS

typedef struct blacklist *bl_t;

bl_t bl_create(bool, const char *, void (*)(int, const char *, va_list));
void bl_destroy(bl_t);
int bl_send(bl_t, bl_type_t, int, const struct sockaddr *, socklen_t,
const char *);
int bl_getfd(bl_t);
bl_info_t *bl_recv(bl_t);
bool bl_isconnected(bl_t);

__END_DECLS

#endif
