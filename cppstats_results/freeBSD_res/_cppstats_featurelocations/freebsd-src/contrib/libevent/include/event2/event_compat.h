

























#if !defined(EVENT2_EVENT_COMPAT_H_INCLUDED_)
#define EVENT2_EVENT_COMPAT_H_INCLUDED_















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














EVENT2_EXPORT_SYMBOL
struct event_base *event_init(void);












EVENT2_EXPORT_SYMBOL
int event_dispatch(void);












EVENT2_EXPORT_SYMBOL
int event_loop(int);














EVENT2_EXPORT_SYMBOL
int event_loopexit(const struct timeval *);














EVENT2_EXPORT_SYMBOL
int event_loopbreak(void);










EVENT2_EXPORT_SYMBOL
int event_once(evutil_socket_t , short,
void (*)(evutil_socket_t, short, void *), void *, const struct timeval *);











EVENT2_EXPORT_SYMBOL
const char *event_get_method(void);











EVENT2_EXPORT_SYMBOL
int event_priority_init(int);








EVENT2_EXPORT_SYMBOL
void event_set(struct event *, evutil_socket_t, short, void (*)(evutil_socket_t, short, void *), void *);

#define evtimer_set(ev, cb, arg) event_set((ev), -1, 0, (cb), (arg))
#define evsignal_set(ev, x, cb, arg) event_set((ev), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))










#define timeout_add(ev, tv) event_add((ev), (tv))
#define timeout_set(ev, cb, arg) event_set((ev), -1, 0, (cb), (arg))
#define timeout_del(ev) event_del(ev)
#define timeout_pending(ev, tv) event_pending((ev), EV_TIMEOUT, (tv))
#define timeout_initialized(ev) event_initialized(ev)









#define signal_add(ev, tv) event_add((ev), (tv))
#define signal_set(ev, x, cb, arg) event_set((ev), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))

#define signal_del(ev) event_del(ev)
#define signal_pending(ev, tv) event_pending((ev), EV_SIGNAL, (tv))
#define signal_initialized(ev) event_initialized(ev)


#if !defined(EVENT_FD)

#define EVENT_FD(ev) ((int)event_get_fd(ev))
#define EVENT_SIGNAL(ev) event_get_signal(ev)
#endif

#if defined(__cplusplus)
}
#endif

#endif
