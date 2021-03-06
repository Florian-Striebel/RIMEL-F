

























#include "../util-internal.h"
#include "event2/event-config.h"

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(EVENT__HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event.h>
#include <evutil.h>

struct timeval timeout = {3, 0};

static void
closed_cb(evutil_socket_t fd, short event, void *arg)
{
if (EV_TIMEOUT & event) {
printf("%s: Timeout!\n", __func__);
exit(1);
}

if (EV_CLOSED & event) {
printf("%s: detected socket close with success\n", __func__);
return;
}

printf("%s: unable to detect socket close\n", __func__);
exit(1);
}

int
main(int argc, char **argv)
{
struct event_base *base;
struct event_config *cfg;
struct event *ev;
const char *test = "test string";
evutil_socket_t pair[2];




cfg = event_config_new();
event_config_require_features(cfg, EV_FEATURE_EARLY_CLOSE);
base = event_base_new_with_config(cfg);
event_config_free(cfg);
if (!base) {

return 0;
}


if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
return (1);


if (send(pair[0], test, (int)strlen(test)+1, 0) < 0)
return (1);
shutdown(pair[0], EVUTIL_SHUT_WR);


ev = event_new(base, pair[1], EV_CLOSED | EV_TIMEOUT, closed_cb, event_self_cbarg());
event_add(ev, &timeout);
event_base_dispatch(base);


event_base_free(base);
return 0;
}

