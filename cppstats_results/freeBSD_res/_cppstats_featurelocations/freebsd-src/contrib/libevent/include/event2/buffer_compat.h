

























#if !defined(EVENT2_BUFFER_COMPAT_H_INCLUDED_)
#define EVENT2_BUFFER_COMPAT_H_INCLUDED_

#include <event2/visibility.h>



















EVENT2_EXPORT_SYMBOL
char *evbuffer_readline(struct evbuffer *buffer);





















typedef void (*evbuffer_cb)(struct evbuffer *buffer, size_t old_len, size_t new_len, void *arg);




















EVENT2_EXPORT_SYMBOL
void evbuffer_setcb(struct evbuffer *buffer, evbuffer_cb cb, void *cbarg);










EVENT2_EXPORT_SYMBOL
unsigned char *evbuffer_find(struct evbuffer *buffer, const unsigned char *what, size_t len);


#define EVBUFFER_LENGTH(x) evbuffer_get_length(x)

#define EVBUFFER_DATA(x) evbuffer_pullup((x), -1)

#endif

