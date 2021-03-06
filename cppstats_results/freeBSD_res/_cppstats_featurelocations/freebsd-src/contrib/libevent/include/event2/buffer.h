
























#if !defined(EVENT2_BUFFER_H_INCLUDED_)
#define EVENT2_BUFFER_H_INCLUDED_















































#include <event2/visibility.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <event2/event-config.h>
#include <stdarg.h>
#if defined(EVENT__HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(EVENT__HAVE_SYS_UIO_H)
#include <sys/uio.h>
#endif
#include <event2/util.h>







struct evbuffer
#if defined(EVENT_IN_DOXYGEN_)
{}
#endif
;














struct evbuffer_ptr {
ev_ssize_t pos;



struct {
void *chain;
size_t pos_in_chain;
} internal_;
};






#if defined(EVENT__HAVE_SYS_UIO_H)
#define evbuffer_iovec iovec

#define EVBUFFER_IOVEC_IS_NATIVE_
#else
struct evbuffer_iovec {

void *iov_base;

size_t iov_len;
};
#endif







EVENT2_EXPORT_SYMBOL
struct evbuffer *evbuffer_new(void);





EVENT2_EXPORT_SYMBOL
void evbuffer_free(struct evbuffer *buf);













EVENT2_EXPORT_SYMBOL
int evbuffer_enable_locking(struct evbuffer *buf, void *lock);





EVENT2_EXPORT_SYMBOL
void evbuffer_lock(struct evbuffer *buf);





EVENT2_EXPORT_SYMBOL
void evbuffer_unlock(struct evbuffer *buf);

















#define EVBUFFER_FLAG_DRAINS_TO_FD 1








EVENT2_EXPORT_SYMBOL
int evbuffer_set_flags(struct evbuffer *buf, ev_uint64_t flags);







EVENT2_EXPORT_SYMBOL
int evbuffer_clear_flags(struct evbuffer *buf, ev_uint64_t flags);







EVENT2_EXPORT_SYMBOL
size_t evbuffer_get_length(const struct evbuffer *buf);













EVENT2_EXPORT_SYMBOL
size_t evbuffer_get_contiguous_space(const struct evbuffer *buf);











EVENT2_EXPORT_SYMBOL
int evbuffer_expand(struct evbuffer *buf, size_t datlen);


































EVENT2_EXPORT_SYMBOL
int
evbuffer_reserve_space(struct evbuffer *buf, ev_ssize_t size,
struct evbuffer_iovec *vec, int n_vec);






















EVENT2_EXPORT_SYMBOL
int evbuffer_commit_space(struct evbuffer *buf,
struct evbuffer_iovec *vec, int n_vecs);









EVENT2_EXPORT_SYMBOL
int evbuffer_add(struct evbuffer *buf, const void *data, size_t datlen);













EVENT2_EXPORT_SYMBOL
int evbuffer_remove(struct evbuffer *buf, void *data, size_t datlen);












EVENT2_EXPORT_SYMBOL
ev_ssize_t evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen);













EVENT2_EXPORT_SYMBOL
ev_ssize_t evbuffer_copyout_from(struct evbuffer *buf, const struct evbuffer_ptr *pos, void *data_out, size_t datlen);














EVENT2_EXPORT_SYMBOL
int evbuffer_remove_buffer(struct evbuffer *src, struct evbuffer *dst,
size_t datlen);



enum evbuffer_eol_style {









EVBUFFER_EOL_ANY,


EVBUFFER_EOL_CRLF,

EVBUFFER_EOL_CRLF_STRICT,

EVBUFFER_EOL_LF,

EVBUFFER_EOL_NUL
};















EVENT2_EXPORT_SYMBOL
char *evbuffer_readln(struct evbuffer *buffer, size_t *n_read_out,
enum evbuffer_eol_style eol_style);













EVENT2_EXPORT_SYMBOL
int evbuffer_add_buffer(struct evbuffer *outbuf, struct evbuffer *inbuf);














EVENT2_EXPORT_SYMBOL
int evbuffer_add_buffer_reference(struct evbuffer *outbuf,
struct evbuffer *inbuf);







typedef void (*evbuffer_ref_cleanup_cb)(const void *data,
size_t datalen, void *extra);
















EVENT2_EXPORT_SYMBOL
int evbuffer_add_reference(struct evbuffer *outbuf,
const void *data, size_t datlen,
evbuffer_ref_cleanup_cb cleanupfn, void *cleanupfn_arg);

























EVENT2_EXPORT_SYMBOL
int evbuffer_add_file(struct evbuffer *outbuf, int fd, ev_off_t offset,
ev_off_t length);









struct evbuffer_file_segment;






#define EVBUF_FS_CLOSE_ON_FREE 0x01




#define EVBUF_FS_DISABLE_MMAP 0x02









#define EVBUF_FS_DISABLE_SENDFILE 0x04






#define EVBUF_FS_DISABLE_LOCKING 0x08





typedef void (*evbuffer_file_segment_cleanup_cb)(
struct evbuffer_file_segment const* seg, int flags, void* arg);























EVENT2_EXPORT_SYMBOL
struct evbuffer_file_segment *evbuffer_file_segment_new(
int fd, ev_off_t offset, ev_off_t length, unsigned flags);








EVENT2_EXPORT_SYMBOL
void evbuffer_file_segment_free(struct evbuffer_file_segment *seg);








EVENT2_EXPORT_SYMBOL
void evbuffer_file_segment_add_cleanup_cb(struct evbuffer_file_segment *seg,
evbuffer_file_segment_cleanup_cb cb, void* arg);























EVENT2_EXPORT_SYMBOL
int evbuffer_add_file_segment(struct evbuffer *buf,
struct evbuffer_file_segment *seg, ev_off_t offset, ev_off_t length);













EVENT2_EXPORT_SYMBOL
int evbuffer_add_printf(struct evbuffer *buf, const char *fmt, ...)
#if defined(__GNUC__)
__attribute__((format(printf, 2, 3)))
#endif
;









EVENT2_EXPORT_SYMBOL
int evbuffer_add_vprintf(struct evbuffer *buf, const char *fmt, va_list ap)
#if defined(__GNUC__)
__attribute__((format(printf, 2, 0)))
#endif
;









EVENT2_EXPORT_SYMBOL
int evbuffer_drain(struct evbuffer *buf, size_t len);












EVENT2_EXPORT_SYMBOL
int evbuffer_write(struct evbuffer *buffer, evutil_socket_t fd);













EVENT2_EXPORT_SYMBOL
int evbuffer_write_atmost(struct evbuffer *buffer, evutil_socket_t fd,
ev_ssize_t howmuch);










EVENT2_EXPORT_SYMBOL
int evbuffer_read(struct evbuffer *buffer, evutil_socket_t fd, int howmuch);












EVENT2_EXPORT_SYMBOL
struct evbuffer_ptr evbuffer_search(struct evbuffer *buffer, const char *what, size_t len, const struct evbuffer_ptr *start);















EVENT2_EXPORT_SYMBOL
struct evbuffer_ptr evbuffer_search_range(struct evbuffer *buffer, const char *what, size_t len, const struct evbuffer_ptr *start, const struct evbuffer_ptr *end);





enum evbuffer_ptr_how {


EVBUFFER_PTR_SET,

EVBUFFER_PTR_ADD
};






















EVENT2_EXPORT_SYMBOL
int
evbuffer_ptr_set(struct evbuffer *buffer, struct evbuffer_ptr *ptr,
size_t position, enum evbuffer_ptr_how how);















EVENT2_EXPORT_SYMBOL
struct evbuffer_ptr evbuffer_search_eol(struct evbuffer *buffer,
struct evbuffer_ptr *start, size_t *eol_len_out,
enum evbuffer_eol_style eol_style);





























EVENT2_EXPORT_SYMBOL
int evbuffer_peek(struct evbuffer *buffer, ev_ssize_t len,
struct evbuffer_ptr *start_at,
struct evbuffer_iovec *vec_out, int n_vec);






struct evbuffer_cb_info {


size_t orig_size;

size_t n_added;

size_t n_deleted;
};




















typedef void (*evbuffer_cb_func)(struct evbuffer *buffer, const struct evbuffer_cb_info *info, void *arg);

struct evbuffer_cb_entry;











EVENT2_EXPORT_SYMBOL
struct evbuffer_cb_entry *evbuffer_add_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg);









EVENT2_EXPORT_SYMBOL
int evbuffer_remove_cb_entry(struct evbuffer *buffer,
struct evbuffer_cb_entry *ent);







EVENT2_EXPORT_SYMBOL
int evbuffer_remove_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg);






#define EVBUFFER_CB_ENABLED 1








EVENT2_EXPORT_SYMBOL
int evbuffer_cb_set_flags(struct evbuffer *buffer,
struct evbuffer_cb_entry *cb, ev_uint32_t flags);








EVENT2_EXPORT_SYMBOL
int evbuffer_cb_clear_flags(struct evbuffer *buffer,
struct evbuffer_cb_entry *cb, ev_uint32_t flags);

#if 0









EVENT2_EXPORT_SYMBOL
void evbuffer_cb_suspend(struct evbuffer *buffer, struct evbuffer_cb_entry *cb);








EVENT2_EXPORT_SYMBOL
void evbuffer_cb_unsuspend(struct evbuffer *buffer, struct evbuffer_cb_entry *cb);
#endif











EVENT2_EXPORT_SYMBOL
unsigned char *evbuffer_pullup(struct evbuffer *buf, ev_ssize_t size);










EVENT2_EXPORT_SYMBOL
int evbuffer_prepend(struct evbuffer *buf, const void *data, size_t size);









EVENT2_EXPORT_SYMBOL
int evbuffer_prepend_buffer(struct evbuffer *dst, struct evbuffer* src);















EVENT2_EXPORT_SYMBOL
int evbuffer_freeze(struct evbuffer *buf, int at_front);








EVENT2_EXPORT_SYMBOL
int evbuffer_unfreeze(struct evbuffer *buf, int at_front);

struct event_base;







EVENT2_EXPORT_SYMBOL
int evbuffer_defer_callbacks(struct evbuffer *buffer, struct event_base *base);














EVENT2_EXPORT_SYMBOL
size_t evbuffer_add_iovec(struct evbuffer * buffer, struct evbuffer_iovec * vec, int n_vec);

#if defined(__cplusplus)
}
#endif

#endif
