
























#if !defined(EVENT2_BUFFEREVENT_SSL_H_INCLUDED_)
#define EVENT2_BUFFEREVENT_SSL_H_INCLUDED_





#include <event2/visibility.h>
#include <event2/event-config.h>
#include <event2/bufferevent.h>
#include <event2/util.h>

#if defined(__cplusplus)
extern "C" {
#endif


struct ssl_st;





enum bufferevent_ssl_state {
BUFFEREVENT_SSL_OPEN = 0,
BUFFEREVENT_SSL_CONNECTING = 1,
BUFFEREVENT_SSL_ACCEPTING = 2
};

#if defined(EVENT__HAVE_OPENSSL) || defined(EVENT_IN_DOXYGEN_)











EVENT2_EXPORT_SYMBOL
struct bufferevent *
bufferevent_openssl_filter_new(struct event_base *base,
struct bufferevent *underlying,
struct ssl_st *ssl,
enum bufferevent_ssl_state state,
int options);











EVENT2_EXPORT_SYMBOL
struct bufferevent *
bufferevent_openssl_socket_new(struct event_base *base,
evutil_socket_t fd,
struct ssl_st *ssl,
enum bufferevent_ssl_state state,
int options);

















EVENT2_EXPORT_SYMBOL
int bufferevent_openssl_get_allow_dirty_shutdown(struct bufferevent *bev);
EVENT2_EXPORT_SYMBOL
void bufferevent_openssl_set_allow_dirty_shutdown(struct bufferevent *bev,
int allow_dirty_shutdown);


EVENT2_EXPORT_SYMBOL
struct ssl_st *
bufferevent_openssl_get_ssl(struct bufferevent *bufev);


EVENT2_EXPORT_SYMBOL
int bufferevent_ssl_renegotiate(struct bufferevent *bev);


EVENT2_EXPORT_SYMBOL
unsigned long bufferevent_get_openssl_error(struct bufferevent *bev);

#endif

#if defined(__cplusplus)
}
#endif

#endif
