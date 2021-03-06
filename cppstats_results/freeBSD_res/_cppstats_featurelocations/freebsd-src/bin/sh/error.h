












































#include <setjmp.h>
#include <signal.h>

struct jmploc {
jmp_buf loc;
};

extern struct jmploc *handler;
extern volatile sig_atomic_t exception;


#define EXINT 0
#define EXERROR 1
#define EXEXIT 2









extern volatile sig_atomic_t suppressint;
extern volatile sig_atomic_t intpending;

#define INTOFF suppressint++
#define INTON { if (--suppressint == 0 && intpending) onint(); }
#define is_int_on() suppressint
#define SETINTON(s) do { suppressint = (s); if (suppressint == 0 && intpending) onint(); } while (0)
#define FORCEINTON {suppressint = 0; if (intpending) onint();}
#define SET_PENDING_INT intpending = 1
#define CLEAR_PENDING_INT intpending = 0
#define int_pending() intpending

void exraise(int) __dead2;
void onint(void) __dead2;
void warning(const char *, ...) __printflike(1, 2);
void error(const char *, ...) __printf0like(1, 2) __dead2;
void errorwithstatus(int, const char *, ...) __printf0like(2, 3) __dead2;







#define setjmp(jmploc) _setjmp(jmploc)
#define longjmp(jmploc, val) _longjmp(jmploc, val)
