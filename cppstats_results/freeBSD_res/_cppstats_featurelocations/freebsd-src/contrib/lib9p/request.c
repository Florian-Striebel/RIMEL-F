


























#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/uio.h>
#if defined(__FreeBSD__)
#include <sys/sbuf.h>
#else
#include "sbuf/sbuf.h"
#endif
#include "lib9p.h"
#include "lib9p_impl.h"
#include "fcall.h"
#include "fid.h"
#include "hashtable.h"
#include "log.h"
#include "linux_errno.h"
#include "backend/backend.h"
#include "threadpool.h"

#define N(x) (sizeof(x) / sizeof(x[0]))

static int l9p_dispatch_tversion(struct l9p_request *req);
static int l9p_dispatch_tattach(struct l9p_request *req);
static int l9p_dispatch_tclunk(struct l9p_request *req);
static int l9p_dispatch_tcreate(struct l9p_request *req);
static int l9p_dispatch_topen(struct l9p_request *req);
static int l9p_dispatch_tread(struct l9p_request *req);
static int l9p_dispatch_tremove(struct l9p_request *req);
static int l9p_dispatch_tstat(struct l9p_request *req);
static int l9p_dispatch_twalk(struct l9p_request *req);
static int l9p_dispatch_twrite(struct l9p_request *req);
static int l9p_dispatch_twstat(struct l9p_request *req);
static int l9p_dispatch_tstatfs(struct l9p_request *req);
static int l9p_dispatch_tlopen(struct l9p_request *req);
static int l9p_dispatch_tlcreate(struct l9p_request *req);
static int l9p_dispatch_tsymlink(struct l9p_request *req);
static int l9p_dispatch_tmknod(struct l9p_request *req);
static int l9p_dispatch_trename(struct l9p_request *req);
static int l9p_dispatch_treadlink(struct l9p_request *req);
static int l9p_dispatch_tgetattr(struct l9p_request *req);
static int l9p_dispatch_tsetattr(struct l9p_request *req);
static int l9p_dispatch_txattrwalk(struct l9p_request *req);
static int l9p_dispatch_txattrcreate(struct l9p_request *req);
static int l9p_dispatch_treaddir(struct l9p_request *req);
static int l9p_dispatch_tfsync(struct l9p_request *req);
static int l9p_dispatch_tlock(struct l9p_request *req);
static int l9p_dispatch_tgetlock(struct l9p_request *req);
static int l9p_dispatch_tlink(struct l9p_request *req);
static int l9p_dispatch_tmkdir(struct l9p_request *req);
static int l9p_dispatch_trenameat(struct l9p_request *req);
static int l9p_dispatch_tunlinkat(struct l9p_request *req);
















struct l9p_handler {
enum l9p_ftype type;
int (*handler)(struct l9p_request *);
bool must_run;
};

static const struct l9p_handler l9p_handlers_no_version[] = {
{L9P_TVERSION, l9p_dispatch_tversion, true},
};

static const struct l9p_handler l9p_handlers_base[] = {
{L9P_TVERSION, l9p_dispatch_tversion, true},
{L9P_TATTACH, l9p_dispatch_tattach, true},
{L9P_TCLUNK, l9p_dispatch_tclunk, true},
{L9P_TFLUSH, l9p_threadpool_tflush, true},
{L9P_TCREATE, l9p_dispatch_tcreate, false},
{L9P_TOPEN, l9p_dispatch_topen, false},
{L9P_TREAD, l9p_dispatch_tread, false},
{L9P_TWRITE, l9p_dispatch_twrite, false},
{L9P_TREMOVE, l9p_dispatch_tremove, true},
{L9P_TSTAT, l9p_dispatch_tstat, false},
{L9P_TWALK, l9p_dispatch_twalk, false},
{L9P_TWSTAT, l9p_dispatch_twstat, false}
};
static const struct l9p_handler l9p_handlers_dotu[] = {
{L9P_TVERSION, l9p_dispatch_tversion, true},
{L9P_TATTACH, l9p_dispatch_tattach, true},
{L9P_TCLUNK, l9p_dispatch_tclunk, true},
{L9P_TFLUSH, l9p_threadpool_tflush, true},
{L9P_TCREATE, l9p_dispatch_tcreate, false},
{L9P_TOPEN, l9p_dispatch_topen, false},
{L9P_TREAD, l9p_dispatch_tread, false},
{L9P_TWRITE, l9p_dispatch_twrite, false},
{L9P_TREMOVE, l9p_dispatch_tremove, true},
{L9P_TSTAT, l9p_dispatch_tstat, false},
{L9P_TWALK, l9p_dispatch_twalk, false},
{L9P_TWSTAT, l9p_dispatch_twstat, false}
};
static const struct l9p_handler l9p_handlers_dotL[] = {
{L9P_TVERSION, l9p_dispatch_tversion, true},
{L9P_TATTACH, l9p_dispatch_tattach, true},
{L9P_TCLUNK, l9p_dispatch_tclunk, true},
{L9P_TFLUSH, l9p_threadpool_tflush, true},
{L9P_TCREATE, l9p_dispatch_tcreate, false},
{L9P_TOPEN, l9p_dispatch_topen, false},
{L9P_TREAD, l9p_dispatch_tread, false},
{L9P_TWRITE, l9p_dispatch_twrite, false},
{L9P_TREMOVE, l9p_dispatch_tremove, true},
{L9P_TSTAT, l9p_dispatch_tstat, false},
{L9P_TWALK, l9p_dispatch_twalk, false},
{L9P_TWSTAT, l9p_dispatch_twstat, false},
{L9P_TSTATFS, l9p_dispatch_tstatfs, false},
{L9P_TLOPEN, l9p_dispatch_tlopen, false},
{L9P_TLCREATE, l9p_dispatch_tlcreate, false},
{L9P_TSYMLINK, l9p_dispatch_tsymlink, false},
{L9P_TMKNOD, l9p_dispatch_tmknod, false},
{L9P_TRENAME, l9p_dispatch_trename, false},
{L9P_TREADLINK, l9p_dispatch_treadlink, false},
{L9P_TGETATTR, l9p_dispatch_tgetattr, false},
{L9P_TSETATTR, l9p_dispatch_tsetattr, false},
{L9P_TXATTRWALK, l9p_dispatch_txattrwalk, false},
{L9P_TXATTRCREATE, l9p_dispatch_txattrcreate, false},
{L9P_TREADDIR, l9p_dispatch_treaddir, false},
{L9P_TFSYNC, l9p_dispatch_tfsync, false},
{L9P_TLOCK, l9p_dispatch_tlock, true},
{L9P_TGETLOCK, l9p_dispatch_tgetlock, true},
{L9P_TLINK, l9p_dispatch_tlink, false},
{L9P_TMKDIR, l9p_dispatch_tmkdir, false},
{L9P_TRENAMEAT, l9p_dispatch_trenameat, false},
{L9P_TUNLINKAT, l9p_dispatch_tunlinkat, false},
};






static const struct {
const char *name;
const struct l9p_handler *handlers;
int n_handlers;
} l9p_versions[] = {
{ "<none>", l9p_handlers_no_version, N(l9p_handlers_no_version) },
{ "9P2000", l9p_handlers_base, N(l9p_handlers_base) },
{ "9P2000.u", l9p_handlers_dotu, N(l9p_handlers_dotu), },
{ "9P2000.L", l9p_handlers_dotL, N(l9p_handlers_dotL), },
};





int
l9p_dispatch_request(struct l9p_request *req)
{
struct l9p_connection *conn;
#if defined(L9P_DEBUG)
struct sbuf *sb;
#endif
size_t i, n;
const struct l9p_handler *handlers, *hp;
bool flush_requested;

conn = req->lr_conn;
flush_requested = req->lr_flushstate == L9P_FLUSH_REQUESTED_PRE_START;

handlers = l9p_versions[conn->lc_version].handlers;
n = (size_t)l9p_versions[conn->lc_version].n_handlers;
for (hp = handlers, i = 0; i < n; hp++, i++)
if (req->lr_req.hdr.type == hp->type)
goto found;
hp = NULL;
found:

#if defined(L9P_DEBUG)
sb = sbuf_new_auto();
if (flush_requested) {
sbuf_cat(sb, "FLUSH requested pre-dispatch");
if (hp != NULL && hp->must_run)
sbuf_cat(sb, ", but must run");
sbuf_cat(sb, ": ");
}
l9p_describe_fcall(&req->lr_req, conn->lc_version, sb);
sbuf_finish(sb);

L9P_LOG(L9P_DEBUG, "%s", sbuf_data(sb));
sbuf_delete(sb);
#endif

if (hp != NULL) {
if (!flush_requested || hp->must_run)
return (hp->handler(req));
return (EINTR);
}

L9P_LOG(L9P_WARNING, "unknown request of type %d",
req->lr_req.hdr.type);
return (ENOSYS);
}




static inline int
e29p(int errnum)
{
static int const table[] = {
[ENOTEMPTY] = EPERM,
[EDQUOT] = EPERM,
[ENOSYS] = EPERM,
};

if ((size_t)errnum < N(table) && table[errnum] != 0)
return (table[errnum]);
if (errnum <= ERANGE)
return (errnum);
return (EIO);
}




static inline int
e2linux(int errnum)
{
static int const table[] = {
[EDEADLK] = LINUX_EDEADLK,
[EAGAIN] = LINUX_EAGAIN,
[EINPROGRESS] = LINUX_EINPROGRESS,
[EALREADY] = LINUX_EALREADY,
[ENOTSOCK] = LINUX_ENOTSOCK,
[EDESTADDRREQ] = LINUX_EDESTADDRREQ,
[EMSGSIZE] = LINUX_EMSGSIZE,
[EPROTOTYPE] = LINUX_EPROTOTYPE,
[ENOPROTOOPT] = LINUX_ENOPROTOOPT,
[EPROTONOSUPPORT] = LINUX_EPROTONOSUPPORT,
[ESOCKTNOSUPPORT] = LINUX_ESOCKTNOSUPPORT,
[EOPNOTSUPP] = LINUX_EOPNOTSUPP,
[EPFNOSUPPORT] = LINUX_EPFNOSUPPORT,
[EAFNOSUPPORT] = LINUX_EAFNOSUPPORT,
[EADDRINUSE] = LINUX_EADDRINUSE,
[EADDRNOTAVAIL] = LINUX_EADDRNOTAVAIL,
[ENETDOWN] = LINUX_ENETDOWN,
[ENETUNREACH] = LINUX_ENETUNREACH,
[ENETRESET] = LINUX_ENETRESET,
[ECONNABORTED] = LINUX_ECONNABORTED,
[ECONNRESET] = LINUX_ECONNRESET,
[ENOBUFS] = LINUX_ENOBUFS,
[EISCONN] = LINUX_EISCONN,
[ENOTCONN] = LINUX_ENOTCONN,
[ESHUTDOWN] = LINUX_ESHUTDOWN,
[ETOOMANYREFS] = LINUX_ETOOMANYREFS,
[ETIMEDOUT] = LINUX_ETIMEDOUT,
[ECONNREFUSED] = LINUX_ECONNREFUSED,
[ELOOP] = LINUX_ELOOP,
[ENAMETOOLONG] = LINUX_ENAMETOOLONG,
[EHOSTDOWN] = LINUX_EHOSTDOWN,
[EHOSTUNREACH] = LINUX_EHOSTUNREACH,
[ENOTEMPTY] = LINUX_ENOTEMPTY,
[EPROCLIM] = LINUX_EAGAIN,
[EUSERS] = LINUX_EUSERS,
[EDQUOT] = LINUX_EDQUOT,
[ESTALE] = LINUX_ESTALE,
[EREMOTE] = LINUX_EREMOTE,





[ENOLCK] = LINUX_ENOLCK,
[ENOSYS] = LINUX_ENOSYS,



[EIDRM] = LINUX_EIDRM,
[ENOMSG] = LINUX_ENOMSG,
[EOVERFLOW] = LINUX_EOVERFLOW,
[ECANCELED] = LINUX_ECANCELED,
[EILSEQ] = LINUX_EILSEQ,

[EBADMSG] = LINUX_EBADMSG,
[EMULTIHOP] = LINUX_EMULTIHOP,
[ENOLINK] = LINUX_ENOLINK,
[EPROTO] = LINUX_EPROTO,

#if defined(ECAPMODE)
[ECAPMODE] = EPERM,
#endif
#if defined(ENOTRECOVERABLE)
[ENOTRECOVERABLE] = LINUX_ENOTRECOVERABLE,
#endif
#if defined(EOWNERDEAD)
[EOWNERDEAD] = LINUX_EOWNERDEAD,
#endif
};








if (errnum < 0)
return (-errnum);

if ((size_t)errnum < N(table) && table[errnum] != 0)
return (table[errnum]);

if (errnum <= ERANGE)
return (errnum);

L9P_LOG(L9P_WARNING, "cannot map errno %d to anything reasonable",
errnum);

return (LINUX_ENOTRECOVERABLE);
}






void
l9p_respond(struct l9p_request *req, bool drop, bool rmtag)
{
struct l9p_connection *conn = req->lr_conn;
size_t iosize;
#if defined(L9P_DEBUG)
struct sbuf *sb;
const char *ftype;
#endif
int error;

req->lr_resp.hdr.tag = req->lr_req.hdr.tag;

error = req->lr_error;
if (error == 0)
req->lr_resp.hdr.type = req->lr_req.hdr.type + 1;
else {
if (conn->lc_version == L9P_2000L) {
req->lr_resp.hdr.type = L9P_RLERROR;
req->lr_resp.error.errnum = (uint32_t)e2linux(error);
} else {
req->lr_resp.hdr.type = L9P_RERROR;
req->lr_resp.error.ename = strerror(error);
req->lr_resp.error.errnum = (uint32_t)e29p(error);
}
}

#if defined(L9P_DEBUG)
sb = sbuf_new_auto();
l9p_describe_fcall(&req->lr_resp, conn->lc_version, sb);
sbuf_finish(sb);

switch (req->lr_flushstate) {
case L9P_FLUSH_NONE:
ftype = "";
break;
case L9P_FLUSH_REQUESTED_PRE_START:
ftype = "FLUSH requested pre-dispatch: ";
break;
case L9P_FLUSH_REQUESTED_POST_START:
ftype = "FLUSH requested while running: ";
break;
case L9P_FLUSH_TOOLATE:
ftype = "FLUSH requested too late: ";
break;
}
L9P_LOG(L9P_DEBUG, "%s%s%s",
drop ? "DROP: " : "", ftype, sbuf_data(sb));
sbuf_delete(sb);
#endif

error = drop ? 0 :
l9p_pufcall(&req->lr_resp_msg, &req->lr_resp, conn->lc_version);
if (rmtag)
ht_remove(&conn->lc_requests, req->lr_req.hdr.tag);
if (error != 0) {
L9P_LOG(L9P_ERROR, "cannot pack response");
drop = true;
}

if (drop) {
conn->lc_lt.lt_drop_response(req,
req->lr_resp_msg.lm_iov, req->lr_resp_msg.lm_niov,
conn->lc_lt.lt_aux);
} else {
iosize = req->lr_resp_msg.lm_size;





if (req->lr_resp.hdr.type == L9P_RREAD ||
req->lr_resp.hdr.type == L9P_RREADDIR)
iosize += req->lr_resp.io.count;

conn->lc_lt.lt_send_response(req,
req->lr_resp_msg.lm_iov, req->lr_resp_msg.lm_niov,
iosize, conn->lc_lt.lt_aux);
}

l9p_freefcall(&req->lr_req);
l9p_freefcall(&req->lr_resp);

free(req);
}













void
l9p_init_msg(struct l9p_message *msg, struct l9p_request *req,
enum l9p_pack_mode mode)
{

msg->lm_size = 0;
msg->lm_mode = mode;
msg->lm_cursor_iov = 0;
msg->lm_cursor_offset = 0;
msg->lm_niov = req->lr_data_niov;
memcpy(msg->lm_iov, req->lr_data_iov,
sizeof (struct iovec) * req->lr_data_niov);
}

enum fid_lookup_flags {
F_REQUIRE_OPEN = 0x01,
F_REQUIRE_DIR = 0x02,
F_REQUIRE_XATTR = 0x04,
F_REQUIRE_AUTH = 0x08,
F_FORBID_OPEN = 0x10,
F_FORBID_DIR = 0x20,
F_FORBID_XATTR = 0x40,
F_ALLOW_AUTH = 0x80,
};














static inline int
fid_lookup(struct l9p_connection *conn, uint32_t fid, int err, int flags,
struct l9p_fid **afile)
{
struct l9p_fid *file;

file = ht_find(&conn->lc_files, fid);
if (file == NULL)
return (err);











assert(l9p_fid_isvalid(file));





if ((flags & F_REQUIRE_OPEN) && !l9p_fid_isopen(file))
return (EINVAL);
if ((flags & F_FORBID_OPEN) && l9p_fid_isopen(file))
return (EINVAL);
if ((flags & F_REQUIRE_DIR) && !l9p_fid_isdir(file))
return (ENOTDIR);
if ((flags & F_FORBID_DIR) && l9p_fid_isdir(file))
return (EISDIR);
if ((flags & F_REQUIRE_XATTR) && !l9p_fid_isxattr(file))
return (EINVAL);
if ((flags & F_FORBID_XATTR) && l9p_fid_isxattr(file))
return (EINVAL);
if (l9p_fid_isauth(file)) {
if ((flags & (F_REQUIRE_AUTH | F_ALLOW_AUTH)) == 0)
return (EINVAL);
} else if (flags & F_REQUIRE_AUTH)
return (EINVAL);
*afile = file;
return (0);
}












int
l9p_pack_stat(struct l9p_message *msg, struct l9p_request *req,
struct l9p_stat *st)
{
struct l9p_connection *conn = req->lr_conn;
uint16_t size = l9p_sizeof_stat(st, conn->lc_version);
int ret = 0;

assert(msg->lm_mode == L9P_PACK);

if (req->lr_resp.io.count + size > req->lr_req.io.count ||
l9p_pustat(msg, st, conn->lc_version) < 0)
ret = -1;
else
req->lr_resp.io.count += size;
l9p_freestat(st);
return (ret);
}

static int
l9p_dispatch_tversion(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_server *server = conn->lc_server;
enum l9p_version remote_version = L9P_INVALID_VERSION;
size_t i;
const char *remote_version_name;

for (i = 0; i < N(l9p_versions); i++) {
if (strcmp(req->lr_req.version.version,
l9p_versions[i].name) == 0) {
remote_version = (enum l9p_version)i;
break;
}
}

if (remote_version == L9P_INVALID_VERSION) {
L9P_LOG(L9P_ERROR, "unsupported remote version: %s",
req->lr_req.version.version);
return (ENOSYS);
}

remote_version_name = l9p_versions[remote_version].name;
L9P_LOG(L9P_INFO, "remote version: %s", remote_version_name);
L9P_LOG(L9P_INFO, "local version: %s",
l9p_versions[server->ls_max_version].name);

conn->lc_version = MIN(remote_version, server->ls_max_version);
conn->lc_msize = MIN(req->lr_req.version.msize, conn->lc_msize);
conn->lc_max_io_size = conn->lc_msize - 24;
req->lr_resp.version.version = strdup(remote_version_name);
req->lr_resp.version.msize = conn->lc_msize;
return (0);
}

static int
l9p_dispatch_tattach(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid;
int error;








if (req->lr_req.tattach.afid != L9P_NOFID) {
error = fid_lookup(conn, req->lr_req.tattach.afid, EINVAL,
F_REQUIRE_AUTH, &req->lr_fid2);
if (error)
return (error);
} else
req->lr_fid2 = NULL;

fid = l9p_connection_alloc_fid(conn, req->lr_req.hdr.fid);
if (fid == NULL)
return (EINVAL);

be = conn->lc_server->ls_backend;

req->lr_fid = fid;


if (conn->lc_version == L9P_2000)
req->lr_req.tattach.n_uname = L9P_NONUNAME;
error = be->attach(be->softc, req);





if (error == 0) {
l9p_fid_setvalid(fid);
if (req->lr_resp.rattach.qid.type & L9P_QTDIR)
l9p_fid_setdir(fid);
} else
l9p_connection_remove_fid(conn, fid);
return (error);
}

static int
l9p_dispatch_tclunk(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_ALLOW_AUTH, &fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;
l9p_fid_unsetvalid(fid);










if (l9p_fid_isxattr(fid))
error = be->xattrclunk(be->softc, fid);
else
error = be->clunk(be->softc, fid);


l9p_connection_remove_fid(conn, fid);
return (error);
}

static int
l9p_dispatch_tcreate(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
uint32_t dmperm;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, EINVAL,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;
dmperm = req->lr_req.tcreate.perm;
#define MKDIR_OR_SIMILAR (L9P_DMDIR | L9P_DMSYMLINK | L9P_DMNAMEDPIPE | L9P_DMSOCKET | L9P_DMDEVICE)










error = be->create(be->softc, req);
if (error == 0 && (dmperm & MKDIR_OR_SIMILAR) == 0) {
l9p_fid_unsetdir(req->lr_fid);
l9p_fid_setopen(req->lr_fid);
}

return (error);
}

static int
l9p_dispatch_topen(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_OPEN | F_FORBID_XATTR, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;





error = be->open(be->softc, req);
if (error == 0)
l9p_fid_setopen(req->lr_fid);
return (error);
}

static int
l9p_dispatch_tread(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, EINVAL, 0, &req->lr_fid);
if (error)
return (error);







l9p_seek_iov(req->lr_resp_msg.lm_iov, req->lr_resp_msg.lm_niov,
req->lr_data_iov, &req->lr_data_niov, 11);










be = conn->lc_server->ls_backend;
fid = req->lr_fid;
if (l9p_fid_isxattr(fid)) {
error = be->xattrread(be->softc, req);
} else if (l9p_fid_isopen(fid)) {
error = be->read(be->softc, req);
} else {
error = EINVAL;
}

return (error);
}

static int
l9p_dispatch_tremove(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid;
int error;





error = fid_lookup(conn, req->lr_req.hdr.fid, EINVAL, 0, &fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;
l9p_fid_unsetvalid(fid);

error = be->remove(be->softc, fid);

l9p_connection_remove_fid(conn, fid);
return (error);
}

static int
l9p_dispatch_tstat(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_ALLOW_AUTH, &fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;
req->lr_fid = fid;
error = be->stat(be->softc, req);

if (error == 0) {
if (l9p_fid_isauth(fid))
req->lr_resp.rstat.stat.qid.type |= L9P_QTAUTH;


if (req->lr_resp.rstat.stat.qid.type &= L9P_QTDIR)
l9p_fid_setdir(fid);
else
l9p_fid_unsetdir(fid);
}

return (error);
}

static int
l9p_dispatch_twalk(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid, *newfid;
uint16_t n;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_XATTR, &fid);
if (error)
return (error);

if (req->lr_req.twalk.hdr.fid != req->lr_req.twalk.newfid) {
newfid = l9p_connection_alloc_fid(conn,
req->lr_req.twalk.newfid);
if (newfid == NULL)
return (EINVAL);
} else
newfid = fid;

be = conn->lc_server->ls_backend;
req->lr_fid = fid;
req->lr_newfid = newfid;
error = be->walk(be->softc, req);






if (newfid != fid) {
if (error == 0)
l9p_fid_setvalid(newfid);
else
l9p_connection_remove_fid(conn, newfid);
}









if (error == 0) {
n = req->lr_resp.rwalk.nwqid;
if (n > 0) {
if (req->lr_resp.rwalk.wqid[n - 1].type & L9P_QTDIR)
l9p_fid_setdir(newfid);
} else {
if (l9p_fid_isdir(fid))
l9p_fid_setdir(newfid);
}
}
return (error);
}

static int
l9p_dispatch_twrite(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, EINVAL,
F_FORBID_DIR, &req->lr_fid);
if (error)
return (error);








l9p_seek_iov(req->lr_req_msg.lm_iov, req->lr_req_msg.lm_niov,
req->lr_data_iov, &req->lr_data_niov, 23);







be = conn->lc_server->ls_backend;
fid = req->lr_fid;
if (l9p_fid_isxattr(fid)) {
error = be->xattrwrite != NULL ?
be->xattrwrite(be->softc, req) : ENOSYS;
} else if (l9p_fid_isopen(fid)) {
error = be->write != NULL ?
be->write(be->softc, req) : ENOSYS;
} else {
error = EINVAL;
}

return (error);
}

static int
l9p_dispatch_twstat(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, EINVAL,
F_FORBID_XATTR, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;
error = be->wstat != NULL ? be->wstat(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tstatfs(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, EINVAL, 0, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;
error = be->statfs(be->softc, req);
return (error);
}

static int
l9p_dispatch_tlopen(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_OPEN | F_FORBID_XATTR, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;





error = be->lopen != NULL ? be->lopen(be->softc, req) : ENOSYS;
if (error == 0)
l9p_fid_setopen(req->lr_fid);
return (error);
}

static int
l9p_dispatch_tlcreate(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;






error = be->lcreate != NULL ? be->lcreate(be->softc, req) : ENOSYS;
if (error == 0) {
l9p_fid_unsetdir(req->lr_fid);
l9p_fid_setopen(req->lr_fid);
}
return (error);
}

static int
l9p_dispatch_tsymlink(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;





error = be->symlink != NULL ? be->symlink(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tmknod(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;





error = be->mknod != NULL ? be->mknod(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_trename(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_XATTR, &req->lr_fid);
if (error)
return (error);


error = fid_lookup(conn, req->lr_req.trename.dfid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid2);
if (error)
return (error);

be = conn->lc_server->ls_backend;





error = be->rename != NULL ? be->rename(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_treadlink(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;






error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;

error = be->readlink != NULL ? be->readlink(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tgetattr(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_XATTR, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;

error = be->getattr != NULL ? be->getattr(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tsetattr(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_XATTR, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;

error = be->setattr != NULL ? be->setattr(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_txattrwalk(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid, *newfid;
int error;








error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_XATTR, &fid);
if (error)
return (error);

newfid = l9p_connection_alloc_fid(conn, req->lr_req.txattrwalk.newfid);
if (newfid == NULL)
return (EINVAL);

be = conn->lc_server->ls_backend;

req->lr_fid = fid;
req->lr_newfid = newfid;
error = be->xattrwalk != NULL ? be->xattrwalk(be->softc, req) : ENOSYS;






if (error == 0) {
l9p_fid_setvalid(newfid);
l9p_fid_setxattr(newfid);
} else {
l9p_connection_remove_fid(conn, newfid);
}
return (error);
}

static int
l9p_dispatch_txattrcreate(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
struct l9p_fid *fid;
int error;









error = fid_lookup(conn, req->lr_req.hdr.fid, EINVAL,
F_FORBID_XATTR | F_FORBID_OPEN, &fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;

req->lr_fid = fid;
error = be->xattrcreate != NULL ? be->xattrcreate(be->softc, req) :
ENOSYS;





if (error == 0) {
l9p_fid_unsetdir(fid);
l9p_fid_setxattr(fid);
}
return (error);
}

static int
l9p_dispatch_treaddir(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_DIR | F_REQUIRE_OPEN, &req->lr_fid);
if (error)
return (error);







l9p_seek_iov(req->lr_resp_msg.lm_iov, req->lr_resp_msg.lm_niov,
req->lr_data_iov, &req->lr_data_niov, 11);

be = conn->lc_server->ls_backend;

error = be->readdir != NULL ? be->readdir(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tfsync(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;

error = be->fsync != NULL ? be->fsync(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tlock(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;


error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;




error = be->lock != NULL ? be->lock(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tgetlock(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;




error = be->getlock != NULL ? be->getlock(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tlink(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;






error = fid_lookup(conn, req->lr_req.tlink.dfid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid2);
if (error)
return (error);

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_FORBID_DIR | F_FORBID_XATTR, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;

error = be->link != NULL ? be->link(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tmkdir(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);


if (strchr(req->lr_req.tlcreate.name, '/') != NULL)
return (EINVAL);

be = conn->lc_server->ls_backend;
error = be->mkdir != NULL ? be->mkdir(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_trenameat(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);

error = fid_lookup(conn, req->lr_req.trenameat.newdirfid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid2);
if (error)
return (error);

be = conn->lc_server->ls_backend;


error = be->renameat != NULL ? be->renameat(be->softc, req) : ENOSYS;
return (error);
}

static int
l9p_dispatch_tunlinkat(struct l9p_request *req)
{
struct l9p_connection *conn = req->lr_conn;
struct l9p_backend *be;
int error;

error = fid_lookup(conn, req->lr_req.hdr.fid, ENOENT,
F_REQUIRE_DIR | F_FORBID_OPEN, &req->lr_fid);
if (error)
return (error);

be = conn->lc_server->ls_backend;


error = be->unlinkat != NULL ? be->unlinkat(be->softc, req) : ENOSYS;
return (error);
}
