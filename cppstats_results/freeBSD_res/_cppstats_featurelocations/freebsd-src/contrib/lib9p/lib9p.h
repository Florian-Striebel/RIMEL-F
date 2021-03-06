



























#if !defined(LIB9P_LIB9P_H)
#define LIB9P_LIB9P_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/uio.h>
#include <pthread.h>

#if defined(__FreeBSD__)
#include <sys/sbuf.h>
#else
#include "sbuf/sbuf.h"
#endif

#include "fcall.h"
#include "threadpool.h"
#include "hashtable.h"

#define L9P_DEFAULT_MSIZE 8192
#define L9P_MAX_IOV 128
#define L9P_NUMTHREADS 8

struct l9p_request;
struct l9p_backend;
struct l9p_fid;





















struct l9p_transport {
void *lt_aux;
int (*lt_get_response_buffer)(struct l9p_request *, struct iovec *,
size_t *, void *);
int (*lt_send_response)(struct l9p_request *, const struct iovec *,
size_t, size_t, void *);
void (*lt_drop_response)(struct l9p_request *, const struct iovec *,
size_t, void *);
};

enum l9p_pack_mode {
L9P_PACK,
L9P_UNPACK
};

enum l9p_integer_type {
L9P_BYTE = 1,
L9P_WORD = 2,
L9P_DWORD = 4,
L9P_QWORD = 8
};

enum l9p_version {
L9P_INVALID_VERSION = 0,
L9P_2000 = 1,
L9P_2000U = 2,
L9P_2000L = 3
};









struct l9p_message {
enum l9p_pack_mode lm_mode;
struct iovec lm_iov[L9P_MAX_IOV];
size_t lm_niov;
size_t lm_cursor_iov;
size_t lm_cursor_offset;
size_t lm_size;
};






















struct l9p_request {
struct l9p_message lr_req_msg;
struct l9p_message lr_resp_msg;
union l9p_fcall lr_req;
union l9p_fcall lr_resp;

struct l9p_fid *lr_fid;
struct l9p_fid *lr_fid2;
struct l9p_fid *lr_newfid;

struct l9p_connection *lr_conn;
void *lr_aux;

struct iovec lr_data_iov[L9P_MAX_IOV];
size_t lr_data_niov;

int lr_error;


enum l9p_workstate lr_workstate;
enum l9p_flushstate lr_flushstate;
struct l9p_worker *lr_worker;
STAILQ_ENTRY(l9p_request) lr_worklink;


struct l9p_request_queue lr_flushq;
STAILQ_ENTRY(l9p_request) lr_flushlink;
};


struct l9p_dirent {
struct l9p_qid qid;
uint64_t offset;
uint8_t type;
char *name;
};














struct l9p_connection {
struct l9p_server *lc_server;
struct l9p_transport lc_lt;
struct l9p_threadpool lc_tp;
enum l9p_version lc_version;
uint32_t lc_msize;
uint32_t lc_max_io_size;
struct ht lc_files;
struct ht lc_requests;
LIST_ENTRY(l9p_connection) lc_link;
};

struct l9p_server {
struct l9p_backend *ls_backend;
enum l9p_version ls_max_version;
LIST_HEAD(, l9p_connection) ls_conns;
};

int l9p_pufcall(struct l9p_message *msg, union l9p_fcall *fcall,
enum l9p_version version);
ssize_t l9p_pustat(struct l9p_message *msg, struct l9p_stat *s,
enum l9p_version version);
uint16_t l9p_sizeof_stat(struct l9p_stat *stat, enum l9p_version version);
int l9p_pack_stat(struct l9p_message *msg, struct l9p_request *req,
struct l9p_stat *s);
ssize_t l9p_pudirent(struct l9p_message *msg, struct l9p_dirent *de);

int l9p_server_init(struct l9p_server **serverp, struct l9p_backend *backend);

int l9p_connection_init(struct l9p_server *server,
struct l9p_connection **connp);
void l9p_connection_free(struct l9p_connection *conn);
void l9p_connection_recv(struct l9p_connection *conn, const struct iovec *iov,
size_t niov, void *aux);
void l9p_connection_close(struct l9p_connection *conn);
struct l9p_fid *l9p_connection_alloc_fid(struct l9p_connection *conn,
uint32_t fid);
void l9p_connection_remove_fid(struct l9p_connection *conn,
struct l9p_fid *fid);

int l9p_dispatch_request(struct l9p_request *req);
void l9p_respond(struct l9p_request *req, bool drop, bool rmtag);

void l9p_init_msg(struct l9p_message *msg, struct l9p_request *req,
enum l9p_pack_mode mode);
void l9p_seek_iov(struct iovec *iov1, size_t niov1, struct iovec *iov2,
size_t *niov2, size_t seek);
size_t l9p_truncate_iov(struct iovec *iov, size_t niov, size_t length);
void l9p_describe_fcall(union l9p_fcall *fcall, enum l9p_version version,
struct sbuf *sb);
void l9p_freefcall(union l9p_fcall *fcall);
void l9p_freestat(struct l9p_stat *stat);

gid_t *l9p_getgrlist(const char *, gid_t, int *);

#endif
