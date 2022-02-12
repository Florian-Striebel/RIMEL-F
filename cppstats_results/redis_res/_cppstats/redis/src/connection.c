#include "server.h"
#include "connhelpers.h"
ConnectionType CT_Socket;
connection *connCreateSocket() {
connection *conn = zcalloc(sizeof(connection));
conn->type = &CT_Socket;
conn->fd = -1;
return conn;
}
connection *connCreateAcceptedSocket(int fd) {
connection *conn = connCreateSocket();
conn->fd = fd;
conn->state = CONN_STATE_ACCEPTING;
return conn;
}
static int connSocketConnect(connection *conn, const char *addr, int port, const char *src_addr,
ConnectionCallbackFunc connect_handler) {
int fd = anetTcpNonBlockBestEffortBindConnect(NULL,addr,port,src_addr);
if (fd == -1) {
conn->state = CONN_STATE_ERROR;
conn->last_errno = errno;
return C_ERR;
}
conn->fd = fd;
conn->state = CONN_STATE_CONNECTING;
conn->conn_handler = connect_handler;
aeCreateFileEvent(server.el, conn->fd, AE_WRITABLE,
conn->type->ae_handler, conn);
return C_OK;
}
int connHasWriteHandler(connection *conn) {
return conn->write_handler != NULL;
}
int connHasReadHandler(connection *conn) {
return conn->read_handler != NULL;
}
void connSetPrivateData(connection *conn, void *data) {
conn->private_data = data;
}
void *connGetPrivateData(connection *conn) {
return conn->private_data;
}
static void connSocketClose(connection *conn) {
if (conn->fd != -1) {
aeDeleteFileEvent(server.el,conn->fd, AE_READABLE | AE_WRITABLE);
close(conn->fd);
conn->fd = -1;
}
if (connHasRefs(conn)) {
conn->flags |= CONN_FLAG_CLOSE_SCHEDULED;
return;
}
zfree(conn);
}
static int connSocketWrite(connection *conn, const void *data, size_t data_len) {
int ret = write(conn->fd, data, data_len);
if (ret < 0 && errno != EAGAIN) {
conn->last_errno = errno;
if (errno != EINTR && conn->state == CONN_STATE_CONNECTED)
conn->state = CONN_STATE_ERROR;
}
return ret;
}
static int connSocketRead(connection *conn, void *buf, size_t buf_len) {
int ret = read(conn->fd, buf, buf_len);
if (!ret) {
conn->state = CONN_STATE_CLOSED;
} else if (ret < 0 && errno != EAGAIN) {
conn->last_errno = errno;
if (errno != EINTR && conn->state == CONN_STATE_CONNECTED)
conn->state = CONN_STATE_ERROR;
}
return ret;
}
static int connSocketAccept(connection *conn, ConnectionCallbackFunc accept_handler) {
int ret = C_OK;
if (conn->state != CONN_STATE_ACCEPTING) return C_ERR;
conn->state = CONN_STATE_CONNECTED;
connIncrRefs(conn);
if (!callHandler(conn, accept_handler)) ret = C_ERR;
connDecrRefs(conn);
return ret;
}
static int connSocketSetWriteHandler(connection *conn, ConnectionCallbackFunc func, int barrier) {
if (func == conn->write_handler) return C_OK;
conn->write_handler = func;
if (barrier)
conn->flags |= CONN_FLAG_WRITE_BARRIER;
else
conn->flags &= ~CONN_FLAG_WRITE_BARRIER;
if (!conn->write_handler)
aeDeleteFileEvent(server.el,conn->fd,AE_WRITABLE);
else
if (aeCreateFileEvent(server.el,conn->fd,AE_WRITABLE,
conn->type->ae_handler,conn) == AE_ERR) return C_ERR;
return C_OK;
}
static int connSocketSetReadHandler(connection *conn, ConnectionCallbackFunc func) {
if (func == conn->read_handler) return C_OK;
conn->read_handler = func;
if (!conn->read_handler)
aeDeleteFileEvent(server.el,conn->fd,AE_READABLE);
else
if (aeCreateFileEvent(server.el,conn->fd,
AE_READABLE,conn->type->ae_handler,conn) == AE_ERR) return C_ERR;
return C_OK;
}
static const char *connSocketGetLastError(connection *conn) {
return strerror(conn->last_errno);
}
static void connSocketEventHandler(struct aeEventLoop *el, int fd, void *clientData, int mask)
{
UNUSED(el);
UNUSED(fd);
connection *conn = clientData;
if (conn->state == CONN_STATE_CONNECTING &&
(mask & AE_WRITABLE) && conn->conn_handler) {
int conn_error = connGetSocketError(conn);
if (conn_error) {
conn->last_errno = conn_error;
conn->state = CONN_STATE_ERROR;
} else {
conn->state = CONN_STATE_CONNECTED;
}
if (!conn->write_handler) aeDeleteFileEvent(server.el,conn->fd,AE_WRITABLE);
if (!callHandler(conn, conn->conn_handler)) return;
conn->conn_handler = NULL;
}
int invert = conn->flags & CONN_FLAG_WRITE_BARRIER;
int call_write = (mask & AE_WRITABLE) && conn->write_handler;
int call_read = (mask & AE_READABLE) && conn->read_handler;
if (!invert && call_read) {
if (!callHandler(conn, conn->read_handler)) return;
}
if (call_write) {
if (!callHandler(conn, conn->write_handler)) return;
}
if (invert && call_read) {
if (!callHandler(conn, conn->read_handler)) return;
}
}
static int connSocketBlockingConnect(connection *conn, const char *addr, int port, long long timeout) {
int fd = anetTcpNonBlockConnect(NULL,addr,port);
if (fd == -1) {
conn->state = CONN_STATE_ERROR;
conn->last_errno = errno;
return C_ERR;
}
if ((aeWait(fd, AE_WRITABLE, timeout) & AE_WRITABLE) == 0) {
conn->state = CONN_STATE_ERROR;
conn->last_errno = ETIMEDOUT;
}
conn->fd = fd;
conn->state = CONN_STATE_CONNECTED;
return C_OK;
}
static ssize_t connSocketSyncWrite(connection *conn, char *ptr, ssize_t size, long long timeout) {
return syncWrite(conn->fd, ptr, size, timeout);
}
static ssize_t connSocketSyncRead(connection *conn, char *ptr, ssize_t size, long long timeout) {
return syncRead(conn->fd, ptr, size, timeout);
}
static ssize_t connSocketSyncReadLine(connection *conn, char *ptr, ssize_t size, long long timeout) {
return syncReadLine(conn->fd, ptr, size, timeout);
}
static int connSocketGetType(connection *conn) {
(void) conn;
return CONN_TYPE_SOCKET;
}
ConnectionType CT_Socket = {
.ae_handler = connSocketEventHandler,
.close = connSocketClose,
.write = connSocketWrite,
.read = connSocketRead,
.accept = connSocketAccept,
.connect = connSocketConnect,
.set_write_handler = connSocketSetWriteHandler,
.set_read_handler = connSocketSetReadHandler,
.get_last_error = connSocketGetLastError,
.blocking_connect = connSocketBlockingConnect,
.sync_write = connSocketSyncWrite,
.sync_read = connSocketSyncRead,
.sync_readline = connSocketSyncReadLine,
.get_type = connSocketGetType
};
int connGetSocketError(connection *conn) {
int sockerr = 0;
socklen_t errlen = sizeof(sockerr);
if (getsockopt(conn->fd, SOL_SOCKET, SO_ERROR, &sockerr, &errlen) == -1)
sockerr = errno;
return sockerr;
}
int connPeerToString(connection *conn, char *ip, size_t ip_len, int *port) {
return anetFdToString(conn ? conn->fd : -1, ip, ip_len, port, FD_TO_PEER_NAME);
}
int connSockName(connection *conn, char *ip, size_t ip_len, int *port) {
return anetFdToString(conn->fd, ip, ip_len, port, FD_TO_SOCK_NAME);
}
int connFormatFdAddr(connection *conn, char *buf, size_t buf_len, int fd_to_str_type) {
return anetFormatFdAddr(conn ? conn->fd : -1, buf, buf_len, fd_to_str_type);
}
int connBlock(connection *conn) {
if (conn->fd == -1) return C_ERR;
return anetBlock(NULL, conn->fd);
}
int connNonBlock(connection *conn) {
if (conn->fd == -1) return C_ERR;
return anetNonBlock(NULL, conn->fd);
}
int connEnableTcpNoDelay(connection *conn) {
if (conn->fd == -1) return C_ERR;
return anetEnableTcpNoDelay(NULL, conn->fd);
}
int connDisableTcpNoDelay(connection *conn) {
if (conn->fd == -1) return C_ERR;
return anetDisableTcpNoDelay(NULL, conn->fd);
}
int connKeepAlive(connection *conn, int interval) {
if (conn->fd == -1) return C_ERR;
return anetKeepAlive(NULL, conn->fd, interval);
}
int connSendTimeout(connection *conn, long long ms) {
return anetSendTimeout(NULL, conn->fd, ms);
}
int connRecvTimeout(connection *conn, long long ms) {
return anetRecvTimeout(NULL, conn->fd, ms);
}
int connGetState(connection *conn) {
return conn->state;
}
const char *connGetInfo(connection *conn, char *buf, size_t buf_len) {
snprintf(buf, buf_len-1, "fd=%i", conn == NULL ? -1 : conn->fd);
return buf;
}
