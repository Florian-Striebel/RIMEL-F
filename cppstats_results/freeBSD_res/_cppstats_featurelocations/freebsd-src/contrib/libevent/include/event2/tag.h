

























#if !defined(EVENT2_TAG_H_INCLUDED_)
#define EVENT2_TAG_H_INCLUDED_







#include <event2/visibility.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <event2/event-config.h>
#if defined(EVENT__HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif


#include <event2/util.h>

struct evbuffer;







EVENT2_EXPORT_SYMBOL
void evtag_init(void);








EVENT2_EXPORT_SYMBOL
int evtag_unmarshal_header(struct evbuffer *evbuf, ev_uint32_t *ptag);

EVENT2_EXPORT_SYMBOL
void evtag_marshal(struct evbuffer *evbuf, ev_uint32_t tag, const void *data,
ev_uint32_t len);
EVENT2_EXPORT_SYMBOL
void evtag_marshal_buffer(struct evbuffer *evbuf, ev_uint32_t tag,
struct evbuffer *data);











EVENT2_EXPORT_SYMBOL
void evtag_encode_int(struct evbuffer *evbuf, ev_uint32_t number);
EVENT2_EXPORT_SYMBOL
void evtag_encode_int64(struct evbuffer *evbuf, ev_uint64_t number);

EVENT2_EXPORT_SYMBOL
void evtag_marshal_int(struct evbuffer *evbuf, ev_uint32_t tag,
ev_uint32_t integer);
EVENT2_EXPORT_SYMBOL
void evtag_marshal_int64(struct evbuffer *evbuf, ev_uint32_t tag,
ev_uint64_t integer);

EVENT2_EXPORT_SYMBOL
void evtag_marshal_string(struct evbuffer *buf, ev_uint32_t tag,
const char *string);

EVENT2_EXPORT_SYMBOL
void evtag_marshal_timeval(struct evbuffer *evbuf, ev_uint32_t tag,
struct timeval *tv);

EVENT2_EXPORT_SYMBOL
int evtag_unmarshal(struct evbuffer *src, ev_uint32_t *ptag,
struct evbuffer *dst);
EVENT2_EXPORT_SYMBOL
int evtag_peek(struct evbuffer *evbuf, ev_uint32_t *ptag);
EVENT2_EXPORT_SYMBOL
int evtag_peek_length(struct evbuffer *evbuf, ev_uint32_t *plength);
EVENT2_EXPORT_SYMBOL
int evtag_payload_length(struct evbuffer *evbuf, ev_uint32_t *plength);
EVENT2_EXPORT_SYMBOL
int evtag_consume(struct evbuffer *evbuf);

EVENT2_EXPORT_SYMBOL
int evtag_unmarshal_int(struct evbuffer *evbuf, ev_uint32_t need_tag,
ev_uint32_t *pinteger);
EVENT2_EXPORT_SYMBOL
int evtag_unmarshal_int64(struct evbuffer *evbuf, ev_uint32_t need_tag,
ev_uint64_t *pinteger);

EVENT2_EXPORT_SYMBOL
int evtag_unmarshal_fixed(struct evbuffer *src, ev_uint32_t need_tag,
void *data, size_t len);

EVENT2_EXPORT_SYMBOL
int evtag_unmarshal_string(struct evbuffer *evbuf, ev_uint32_t need_tag,
char **pstring);

EVENT2_EXPORT_SYMBOL
int evtag_unmarshal_timeval(struct evbuffer *evbuf, ev_uint32_t need_tag,
struct timeval *ptv);

#if defined(__cplusplus)
}
#endif

#endif
