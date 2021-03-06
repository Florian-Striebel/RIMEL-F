#include <errno.h>
#define CONN_INFO_LEN 32
struct aeEventLoop;
typedef struct connection connection;
typedef enum {
CONN_STATE_NONE = 0,
CONN_STATE_CONNECTING,
CONN_STATE_ACCEPTING,
CONN_STATE_CONNECTED,
CONN_STATE_CLOSED,
CONN_STATE_ERROR
} ConnectionState;
#define CONN_FLAG_CLOSE_SCHEDULED (1<<0)
#define CONN_FLAG_WRITE_BARRIER (1<<1)
#define CONN_TYPE_SOCKET 1
#define CONN_TYPE_TLS 2
typedef void (*ConnectionCallbackFunc)(struct connection *conn);
typedef struct ConnectionType {
void (*ae_handler)(struct aeEventLoop *el, int fd, void *clientData, int mask);
int (*connect)(struct connection *conn, const char *addr, int port, const char *source_addr, ConnectionCallbackFunc connect_handler);
int (*write)(struct connection *conn, const void *data, size_t data_len);
int (*read)(struct connection *conn, void *buf, size_t buf_len);
void (*close)(struct connection *conn);
int (*accept)(struct connection *conn, ConnectionCallbackFunc accept_handler);
int (*set_write_handler)(struct connection *conn, ConnectionCallbackFunc handler, int barrier);
int (*set_read_handler)(struct connection *conn, ConnectionCallbackFunc handler);
const char *(*get_last_error)(struct connection *conn);
int (*blocking_connect)(struct connection *conn, const char *addr, int port, long long timeout);
ssize_t (*sync_write)(struct connection *conn, char *ptr, ssize_t size, long long timeout);
ssize_t (*sync_read)(struct connection *conn, char *ptr, ssize_t size, long long timeout);
ssize_t (*sync_readline)(struct connection *conn, char *ptr, ssize_t size, long long timeout);
int (*get_type)(struct connection *conn);
} ConnectionType;
struct connection {
ConnectionType *type;
ConnectionState state;
short int flags;
short int refs;
int last_errno;
void *private_data;
ConnectionCallbackFunc conn_handler;
ConnectionCallbackFunc write_handler;
ConnectionCallbackFunc read_handler;
int fd;
};
static inline int connAccept(connection *conn, ConnectionCallbackFunc accept_handler) {
return conn->type->accept(conn, accept_handler);
}
static inline int connConnect(connection *conn, const char *addr, int port, const char *src_addr,
ConnectionCallbackFunc connect_handler) {
return conn->type->connect(conn, addr, port, src_addr, connect_handler);
}
static inline int connBlockingConnect(connection *conn, const char *addr, int port, long long timeout) {
return conn->type->blocking_connect(conn, addr, port, timeout);
}
static inline int connWrite(connection *conn, const void *data, size_t data_len) {
return conn->type->write(conn, data, data_len);
}
static inline int connRead(connection *conn, void *buf, size_t buf_len) {
int ret = conn->type->read(conn, buf, buf_len);
return ret;
}
static inline int connSetWriteHandler(connection *conn, ConnectionCallbackFunc func) {
return conn->type->set_write_handler(conn, func, 0);
}
static inline int connSetReadHandler(connection *conn, ConnectionCallbackFunc func) {
return conn->type->set_read_handler(conn, func);
}
static inline int connSetWriteHandlerWithBarrier(connection *conn, ConnectionCallbackFunc func, int barrier) {
return conn->type->set_write_handler(conn, func, barrier);
}
static inline void connClose(connection *conn) {
conn->type->close(conn);
}
static inline const char *connGetLastError(connection *conn) {
return conn->type->get_last_error(conn);
}
static inline ssize_t connSyncWrite(connection *conn, char *ptr, ssize_t size, long long timeout) {
return conn->type->sync_write(conn, ptr, size, timeout);
}
static inline ssize_t connSyncRead(connection *conn, char *ptr, ssize_t size, long long timeout) {
return conn->type->sync_read(conn, ptr, size, timeout);
}
static inline ssize_t connSyncReadLine(connection *conn, char *ptr, ssize_t size, long long timeout) {
return conn->type->sync_readline(conn, ptr, size, timeout);
}
static inline int connGetType(connection *conn) {
return conn->type->get_type(conn);
}
static inline int connLastErrorRetryable(connection *conn) {
return conn->last_errno == EINTR;
}
connection *connCreateSocket();
connection *connCreateAcceptedSocket(int fd);
connection *connCreateTLS();
connection *connCreateAcceptedTLS(int fd, int require_auth);
void connSetPrivateData(connection *conn, void *data);
void *connGetPrivateData(connection *conn);
int connGetState(connection *conn);
int connHasWriteHandler(connection *conn);
int connHasReadHandler(connection *conn);
int connGetSocketError(connection *conn);
int connBlock(connection *conn);
int connNonBlock(connection *conn);
int connEnableTcpNoDelay(connection *conn);
int connDisableTcpNoDelay(connection *conn);
int connKeepAlive(connection *conn, int interval);
int connSendTimeout(connection *conn, long long ms);
int connRecvTimeout(connection *conn, long long ms);
int connPeerToString(connection *conn, char *ip, size_t ip_len, int *port);
int connFormatFdAddr(connection *conn, char *buf, size_t buf_len, int fd_to_str_type);
int connSockName(connection *conn, char *ip, size_t ip_len, int *port);
const char *connGetInfo(connection *conn, char *buf, size_t buf_len);
sds connTLSGetPeerCert(connection *conn);
int tlsHasPendingData();
int tlsProcessPendingData();
