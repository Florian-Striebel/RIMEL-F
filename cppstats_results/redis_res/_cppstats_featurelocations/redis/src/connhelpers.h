





























#if !defined(__REDIS_CONNHELPERS_H)
#define __REDIS_CONNHELPERS_H

#include "connection.h"

















static inline void connIncrRefs(connection *conn) {
conn->refs++;
}









static inline void connDecrRefs(connection *conn) {
conn->refs--;
}

static inline int connHasRefs(connection *conn) {
return conn->refs;
}






static inline int callHandler(connection *conn, ConnectionCallbackFunc handler) {
connIncrRefs(conn);
if (handler) handler(conn);
connDecrRefs(conn);
if (conn->flags & CONN_FLAG_CLOSE_SCHEDULED) {
if (!connHasRefs(conn)) connClose(conn);
return 0;
}
return 1;
}

#endif
