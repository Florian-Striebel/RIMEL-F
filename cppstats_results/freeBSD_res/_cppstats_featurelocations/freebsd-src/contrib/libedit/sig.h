






































#if !defined(_h_el_sig)
#define _h_el_sig

#include <signal.h>





#define ALLSIGS _DO(SIGINT) _DO(SIGTSTP) _DO(SIGQUIT) _DO(SIGHUP) _DO(SIGTERM) _DO(SIGCONT) _DO(SIGWINCH)







#define ALLSIGSNO 7

typedef struct {
struct sigaction sig_action[ALLSIGSNO];
sigset_t sig_set;
volatile sig_atomic_t sig_no;
} *el_signal_t;

libedit_private void sig_end(EditLine*);
libedit_private int sig_init(EditLine*);
libedit_private void sig_set(EditLine*);
libedit_private void sig_clr(EditLine*);

#endif
