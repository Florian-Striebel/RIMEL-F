#include "fmacros.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "rio.h"
#include "util.h"
#include "crc64.h"
#include "config.h"
#include "server.h"
static size_t rioBufferWrite(rio *r, const void *buf, size_t len) {
r->io.buffer.ptr = sdscatlen(r->io.buffer.ptr,(char*)buf,len);
r->io.buffer.pos += len;
return 1;
}
static size_t rioBufferRead(rio *r, void *buf, size_t len) {
if (sdslen(r->io.buffer.ptr)-r->io.buffer.pos < len)
return 0;
memcpy(buf,r->io.buffer.ptr+r->io.buffer.pos,len);
r->io.buffer.pos += len;
return 1;
}
static off_t rioBufferTell(rio *r) {
return r->io.buffer.pos;
}
static int rioBufferFlush(rio *r) {
UNUSED(r);
return 1;
}
static const rio rioBufferIO = {
rioBufferRead,
rioBufferWrite,
rioBufferTell,
rioBufferFlush,
NULL,
0,
0,
0,
0,
{ { NULL, 0 } }
};
void rioInitWithBuffer(rio *r, sds s) {
*r = rioBufferIO;
r->io.buffer.ptr = s;
r->io.buffer.pos = 0;
}
static size_t rioFileWrite(rio *r, const void *buf, size_t len) {
if (!r->io.file.autosync) return fwrite(buf,len,1,r->io.file.fp);
size_t nwritten = 0;
while (len != nwritten) {
serverAssert(r->io.file.autosync > r->io.file.buffered);
size_t nalign = (size_t)(r->io.file.autosync - r->io.file.buffered);
size_t towrite = nalign > len-nwritten ? len-nwritten : nalign;
if (fwrite((char*)buf+nwritten,towrite,1,r->io.file.fp) == 0) return 0;
nwritten += towrite;
r->io.file.buffered += towrite;
if (r->io.file.buffered >= r->io.file.autosync) {
fflush(r->io.file.fp);
size_t processed = r->processed_bytes + nwritten;
serverAssert(processed % r->io.file.autosync == 0);
serverAssert(r->io.file.buffered == r->io.file.autosync);
#if HAVE_SYNC_FILE_RANGE
if (sync_file_range(fileno(r->io.file.fp),
processed - r->io.file.autosync, r->io.file.autosync,
SYNC_FILE_RANGE_WRITE) == -1)
return 0;
if (processed >= (size_t)r->io.file.autosync * 2) {
if (sync_file_range(fileno(r->io.file.fp),
processed - r->io.file.autosync*2,
r->io.file.autosync, SYNC_FILE_RANGE_WAIT_BEFORE|
SYNC_FILE_RANGE_WRITE|SYNC_FILE_RANGE_WAIT_AFTER) == -1)
return 0;
}
#else
if (redis_fsync(fileno(r->io.file.fp)) == -1) return 0;
#endif
r->io.file.buffered = 0;
}
}
return 1;
}
static size_t rioFileRead(rio *r, void *buf, size_t len) {
return fread(buf,len,1,r->io.file.fp);
}
static off_t rioFileTell(rio *r) {
return ftello(r->io.file.fp);
}
static int rioFileFlush(rio *r) {
return (fflush(r->io.file.fp) == 0) ? 1 : 0;
}
static const rio rioFileIO = {
rioFileRead,
rioFileWrite,
rioFileTell,
rioFileFlush,
NULL,
0,
0,
0,
0,
{ { NULL, 0 } }
};
void rioInitWithFile(rio *r, FILE *fp) {
*r = rioFileIO;
r->io.file.fp = fp;
r->io.file.buffered = 0;
r->io.file.autosync = 0;
}
static size_t rioConnWrite(rio *r, const void *buf, size_t len) {
UNUSED(r);
UNUSED(buf);
UNUSED(len);
return 0;
}
static size_t rioConnRead(rio *r, void *buf, size_t len) {
size_t avail = sdslen(r->io.conn.buf)-r->io.conn.pos;
if (sdslen(r->io.conn.buf) + sdsavail(r->io.conn.buf) < len)
r->io.conn.buf = sdsMakeRoomFor(r->io.conn.buf, len - sdslen(r->io.conn.buf));
if (len > avail && sdsavail(r->io.conn.buf) < len - avail) {
sdsrange(r->io.conn.buf, r->io.conn.pos, -1);
r->io.conn.pos = 0;
}
if (r->io.conn.read_limit != 0 && r->io.conn.read_limit < r->io.conn.read_so_far + len) {
errno = EOVERFLOW;
return 0;
}
while (len > sdslen(r->io.conn.buf) - r->io.conn.pos) {
size_t buffered = sdslen(r->io.conn.buf) - r->io.conn.pos;
size_t needs = len - buffered;
size_t toread = needs < PROTO_IOBUF_LEN ? PROTO_IOBUF_LEN: needs;
if (toread > sdsavail(r->io.conn.buf)) toread = sdsavail(r->io.conn.buf);
if (r->io.conn.read_limit != 0 &&
r->io.conn.read_so_far + buffered + toread > r->io.conn.read_limit)
{
toread = r->io.conn.read_limit - r->io.conn.read_so_far - buffered;
}
int retval = connRead(r->io.conn.conn,
(char*)r->io.conn.buf + sdslen(r->io.conn.buf),
toread);
if (retval == 0) {
return 0;
} else if (retval < 0) {
if (connLastErrorRetryable(r->io.conn.conn)) continue;
if (errno == EWOULDBLOCK) errno = ETIMEDOUT;
return 0;
}
sdsIncrLen(r->io.conn.buf, retval);
}
memcpy(buf, (char*)r->io.conn.buf + r->io.conn.pos, len);
r->io.conn.read_so_far += len;
r->io.conn.pos += len;
return len;
}
static off_t rioConnTell(rio *r) {
return r->io.conn.read_so_far;
}
static int rioConnFlush(rio *r) {
return rioConnWrite(r,NULL,0);
}
static const rio rioConnIO = {
rioConnRead,
rioConnWrite,
rioConnTell,
rioConnFlush,
NULL,
0,
0,
0,
0,
{ { NULL, 0 } }
};
void rioInitWithConn(rio *r, connection *conn, size_t read_limit) {
*r = rioConnIO;
r->io.conn.conn = conn;
r->io.conn.pos = 0;
r->io.conn.read_limit = read_limit;
r->io.conn.read_so_far = 0;
r->io.conn.buf = sdsnewlen(NULL, PROTO_IOBUF_LEN);
sdsclear(r->io.conn.buf);
}
void rioFreeConn(rio *r, sds *remaining) {
if (remaining && (size_t)r->io.conn.pos < sdslen(r->io.conn.buf)) {
if (r->io.conn.pos > 0) sdsrange(r->io.conn.buf, r->io.conn.pos, -1);
*remaining = r->io.conn.buf;
} else {
sdsfree(r->io.conn.buf);
if (remaining) *remaining = NULL;
}
r->io.conn.buf = NULL;
}
static size_t rioFdWrite(rio *r, const void *buf, size_t len) {
ssize_t retval;
unsigned char *p = (unsigned char*) buf;
int doflush = (buf == NULL && len == 0);
if (len > PROTO_IOBUF_LEN) {
if (sdslen(r->io.fd.buf)) {
if (rioFdWrite(r, NULL, 0) == 0)
return 0;
}
} else {
if (len) {
r->io.fd.buf = sdscatlen(r->io.fd.buf,buf,len);
if (sdslen(r->io.fd.buf) > PROTO_IOBUF_LEN)
doflush = 1;
if (!doflush)
return 1;
}
p = (unsigned char*) r->io.fd.buf;
len = sdslen(r->io.fd.buf);
}
size_t nwritten = 0;
while(nwritten != len) {
retval = write(r->io.fd.fd,p+nwritten,len-nwritten);
if (retval <= 0) {
if (retval == -1 && errno == EINTR) continue;
if (retval == -1 && errno == EWOULDBLOCK) errno = ETIMEDOUT;
return 0;
}
nwritten += retval;
}
r->io.fd.pos += len;
sdsclear(r->io.fd.buf);
return 1;
}
static size_t rioFdRead(rio *r, void *buf, size_t len) {
UNUSED(r);
UNUSED(buf);
UNUSED(len);
return 0;
}
static off_t rioFdTell(rio *r) {
return r->io.fd.pos;
}
static int rioFdFlush(rio *r) {
return rioFdWrite(r,NULL,0);
}
static const rio rioFdIO = {
rioFdRead,
rioFdWrite,
rioFdTell,
rioFdFlush,
NULL,
0,
0,
0,
0,
{ { NULL, 0 } }
};
void rioInitWithFd(rio *r, int fd) {
*r = rioFdIO;
r->io.fd.fd = fd;
r->io.fd.pos = 0;
r->io.fd.buf = sdsempty();
}
void rioFreeFd(rio *r) {
sdsfree(r->io.fd.buf);
}
void rioGenericUpdateChecksum(rio *r, const void *buf, size_t len) {
r->cksum = crc64(r->cksum,buf,len);
}
void rioSetAutoSync(rio *r, off_t bytes) {
if(r->write != rioFileIO.write) return;
r->io.file.autosync = bytes;
}
size_t rioWriteBulkCount(rio *r, char prefix, long count) {
char cbuf[128];
int clen;
cbuf[0] = prefix;
clen = 1+ll2string(cbuf+1,sizeof(cbuf)-1,count);
cbuf[clen++] = '\r';
cbuf[clen++] = '\n';
if (rioWrite(r,cbuf,clen) == 0) return 0;
return clen;
}
size_t rioWriteBulkString(rio *r, const char *buf, size_t len) {
size_t nwritten;
if ((nwritten = rioWriteBulkCount(r,'$',len)) == 0) return 0;
if (len > 0 && rioWrite(r,buf,len) == 0) return 0;
if (rioWrite(r,"\r\n",2) == 0) return 0;
return nwritten+len+2;
}
size_t rioWriteBulkLongLong(rio *r, long long l) {
char lbuf[32];
unsigned int llen;
llen = ll2string(lbuf,sizeof(lbuf),l);
return rioWriteBulkString(r,lbuf,llen);
}
size_t rioWriteBulkDouble(rio *r, double d) {
char dbuf[128];
unsigned int dlen;
dlen = snprintf(dbuf,sizeof(dbuf),"%.17g",d);
return rioWriteBulkString(r,dbuf,dlen);
}
