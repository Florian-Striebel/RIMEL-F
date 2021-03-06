

























#if !defined(EVENT2_EVENT_STRUCT_H_INCLUDED_)
#define EVENT2_EVENT_STRUCT_H_INCLUDED_










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


#include <event2/keyvalq_struct.h>

#define EVLIST_TIMEOUT 0x01
#define EVLIST_INSERTED 0x02
#define EVLIST_SIGNAL 0x04
#define EVLIST_ACTIVE 0x08
#define EVLIST_INTERNAL 0x10
#define EVLIST_ACTIVE_LATER 0x20
#define EVLIST_FINALIZING 0x40
#define EVLIST_INIT 0x80

#define EVLIST_ALL 0xff


#if !defined(TAILQ_ENTRY)
#define EVENT_DEFINED_TQENTRY_
#define TAILQ_ENTRY(type) struct { struct type *tqe_next; struct type **tqe_prev; }




#endif

#if !defined(TAILQ_HEAD)
#define EVENT_DEFINED_TQHEAD_
#define TAILQ_HEAD(name, type) struct name { struct type *tqh_first; struct type **tqh_last; }




#endif


#if !defined(LIST_ENTRY)
#define EVENT_DEFINED_LISTENTRY_
#define LIST_ENTRY(type) struct { struct type *le_next; struct type **le_prev; }




#endif

#if !defined(LIST_HEAD)
#define EVENT_DEFINED_LISTHEAD_
#define LIST_HEAD(name, type) struct name { struct type *lh_first; }



#endif

struct event;

struct event_callback {
TAILQ_ENTRY(event_callback) evcb_active_next;
short evcb_flags;
ev_uint8_t evcb_pri;
ev_uint8_t evcb_closure;

union {
void (*evcb_callback)(evutil_socket_t, short, void *);
void (*evcb_selfcb)(struct event_callback *, void *);
void (*evcb_evfinalize)(struct event *, void *);
void (*evcb_cbfinalize)(struct event_callback *, void *);
} evcb_cb_union;
void *evcb_arg;
};

struct event_base;
struct event {
struct event_callback ev_evcallback;


union {
TAILQ_ENTRY(event) ev_next_with_common_timeout;
int min_heap_idx;
} ev_timeout_pos;
evutil_socket_t ev_fd;

struct event_base *ev_base;

union {

struct {
LIST_ENTRY (event) ev_io_next;
struct timeval ev_timeout;
} ev_io;


struct {
LIST_ENTRY (event) ev_signal_next;
short ev_ncalls;

short *ev_pncalls;
} ev_signal;
} ev_;

short ev_events;
short ev_res;
struct timeval ev_timeout;
};

TAILQ_HEAD (event_list, event);

#if defined(EVENT_DEFINED_TQENTRY_)
#undef TAILQ_ENTRY
#endif

#if defined(EVENT_DEFINED_TQHEAD_)
#undef TAILQ_HEAD
#endif

LIST_HEAD (event_dlist, event);

#if defined(EVENT_DEFINED_LISTENTRY_)
#undef LIST_ENTRY
#endif

#if defined(EVENT_DEFINED_LISTHEAD_)
#undef LIST_HEAD
#endif

#if defined(__cplusplus)
}
#endif

#endif
