





























#if !defined(_STATE_H)
#define _STATE_H

#if defined(HAVE_DB_185_H)
#include <db_185.h>
#elif HAVE_DB_H
#include <db.h>
#else
#error "no db.h"
#endif
#include <time.h>

struct dbinfo {
int count;
time_t last;
char id[64];
};

__BEGIN_DECLS
struct sockaddr_storage;
struct conf;

DB *state_open(const char *, int, mode_t);
int state_close(DB *);
int state_get(DB *, const struct conf *, struct dbinfo *);
int state_put(DB *, const struct conf *, const struct dbinfo *);
int state_del(DB *, const struct conf *);
int state_iterate(DB *, struct conf *, struct dbinfo *, unsigned int);
int state_sync(DB *);
__END_DECLS

#endif
