
























#if !defined(DEFER_INTERNAL_H_INCLUDED_)
#define DEFER_INTERNAL_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

#include "event2/event-config.h"
#include "evconfig-private.h"

#include <sys/queue.h>

struct event_callback;
typedef void (*deferred_cb_fn)(struct event_callback *, void *);









void event_deferred_cb_init_(struct event_callback *, ev_uint8_t, deferred_cb_fn, void *);



void event_deferred_cb_set_priority_(struct event_callback *, ev_uint8_t);



void event_deferred_cb_cancel_(struct event_base *, struct event_callback *);





int event_deferred_cb_schedule_(struct event_base *, struct event_callback *);

#if defined(__cplusplus)
}
#endif

#endif

