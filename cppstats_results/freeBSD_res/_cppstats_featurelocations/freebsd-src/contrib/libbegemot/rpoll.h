






























#if !defined(rpoll_h_)
#define rpoll_h_

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*poll_f)(int fd, int mask, void *arg);
typedef void (*timer_f)(int, void *);

int poll_register(int fd, poll_f func, void *arg, int mask);
void poll_unregister(int);
void poll_dispatch(int wait);
int poll_start_timer(u_int msecs, int repeat, timer_f func, void *arg);
int poll_start_utimer(unsigned long long usecs, int repeat, timer_f func,
void *arg);
void poll_stop_timer(int);

enum {
RPOLL_IN = 1,
RPOLL_OUT = 2,
RPOLL_EXCEPT = 4,
};

extern int rpoll_policy;
extern int rpoll_trace;

#if defined(__cplusplus)
}
#endif

#endif
