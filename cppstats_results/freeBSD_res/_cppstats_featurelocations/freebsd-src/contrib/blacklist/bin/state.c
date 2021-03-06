





























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD: state.c,v 1.19 2016/09/26 19:43:43 christos Exp $");

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <netinet/in.h>

#include "bl.h"
#include "internal.h"
#include "conf.h"
#include "support.h"
#include "state.h"

static HASHINFO openinfo = {
4096,
32,
256,
8 * 1024 * 1024,
NULL,
0
};

int
state_close(DB *db)
{
if (db == NULL)
return -1;
if ((*db->close)(db) == -1) {
(*lfun)(LOG_ERR, "%s: can't close db (%m)", __func__);
return -1;
}
return 0;
}

DB *
state_open(const char *dbname, int flags, mode_t perm)
{
DB *db;

#if defined(__APPLE__)
flags &= O_CREAT|O_EXCL|O_EXLOCK|O_NONBLOCK|O_RDONLY|
O_RDWR|O_SHLOCK|O_TRUNC;
#endif
db = dbopen(dbname, flags, perm, DB_HASH, &openinfo);
if (db == NULL) {
if (errno == ENOENT && (flags & O_CREAT) == 0)
return NULL;
(*lfun)(LOG_ERR, "%s: can't open `%s' (%m)", __func__, dbname);
}
return db;
}

static int
state_sizecheck(const DBT *t)
{
if (sizeof(struct conf) == t->size)
return 0;
(*lfun)(LOG_ERR, "Key size mismatch %zu != %zu", sizeof(struct conf),
t->size);
return -1;
}

static void
dumpkey(const struct conf *k)
{
char buf[10240];
blhexdump(buf, sizeof(buf), __func__, k, sizeof(*k));
(*lfun)(LOG_DEBUG, "%s", buf);
(*lfun)(LOG_DEBUG, "%s: %s", __func__,
conf_print(buf, sizeof(buf), "", "", k));

}

int
state_del(DB *db, const struct conf *c)
{
int rv;
DBT k;

if (db == NULL)
return -1;

k.data = __UNCONST(c);
k.size = sizeof(*c);

switch (rv = (*db->del)(db, &k, 0)) {
case 0:
case 1:
if (debug > 1) {
(*lfun)(LOG_DEBUG, "%s: returns %d", __func__, rv);
(*db->sync)(db, 0);
}
return 0;
default:
(*lfun)(LOG_ERR, "%s: failed (%m)", __func__);
return -1;
}
}

int
state_get(DB *db, const struct conf *c, struct dbinfo *dbi)
{
int rv;
DBT k, v;

if (db == NULL)
return -1;

k.data = __UNCONST(c);
k.size = sizeof(*c);

switch (rv = (*db->get)(db, &k, &v, 0)) {
case 0:
case 1:
if (rv)
memset(dbi, 0, sizeof(*dbi));
else
memcpy(dbi, v.data, sizeof(*dbi));
if (debug > 1)
(*lfun)(LOG_DEBUG, "%s: returns %d", __func__, rv);
return 0;
default:
(*lfun)(LOG_ERR, "%s: failed (%m)", __func__);
return -1;
}
}

int
state_put(DB *db, const struct conf *c, const struct dbinfo *dbi)
{
int rv;
DBT k, v;

if (db == NULL)
return -1;

k.data = __UNCONST(c);
k.size = sizeof(*c);
v.data = __UNCONST(dbi);
v.size = sizeof(*dbi);

switch (rv = (*db->put)(db, &k, &v, 0)) {
case 0:
if (debug > 1) {
(*lfun)(LOG_DEBUG, "%s: returns %d", __func__, rv);
(*db->sync)(db, 0);
}
return 0;
case 1:
errno = EEXIST;

default:
(*lfun)(LOG_ERR, "%s: failed (%m)", __func__);
return -1;
}
}

int
state_iterate(DB *db, struct conf *c, struct dbinfo *dbi, unsigned int first)
{
int rv;
DBT k, v;

if (db == NULL) {
(*lfun)(LOG_ERR, "%s: called with no database file", __func__);
return -1;
}

first = first ? R_FIRST : R_NEXT;

switch (rv = (*db->seq)(db, &k, &v, first)) {
case 0:
if (state_sizecheck(&k) == -1)
return -1;
memcpy(c, k.data, sizeof(*c));
if (debug > 2)
dumpkey(c);
memcpy(dbi, v.data, sizeof(*dbi));
if (debug > 1)
(*lfun)(LOG_DEBUG, "%s: returns %d", __func__, rv);
return 1;
case 1:
if (debug > 1)
(*lfun)(LOG_DEBUG, "%s: returns %d", __func__, rv);
return 0;
default:
(*lfun)(LOG_ERR, "%s: failed (%m)", __func__);
return -1;
}
}

int
state_sync(DB *db)
{
return (*db->sync)(db, 0);
}
