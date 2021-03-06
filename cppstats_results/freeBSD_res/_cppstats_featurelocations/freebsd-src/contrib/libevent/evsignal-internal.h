

























#if !defined(EVSIGNAL_INTERNAL_H_INCLUDED_)
#define EVSIGNAL_INTERNAL_H_INCLUDED_

#if !defined(evutil_socket_t)
#include "event2/util.h"
#endif
#include <signal.h>

typedef void (*ev_sighandler_t)(int);



struct evsig_info {

struct event ev_signal;

evutil_socket_t ev_signal_pair[2];

int ev_signal_added;

int ev_n_signals_added;



#if defined(EVENT__HAVE_SIGACTION)
struct sigaction **sh_old;
#else
ev_sighandler_t **sh_old;
#endif

int sh_old_max;
};
int evsig_init_(struct event_base *);
void evsig_dealloc_(struct event_base *);

void evsig_set_base_(struct event_base *base);
void evsig_free_globals_(void);

#endif
