






#if !defined(mathtest_intern_h)
#define mathtest_intern_h

#include <mpfr.h>
#include <mpc.h>

#include "types.h"
#include "wrappers.h"


typedef void (*funcptr)(void);


typedef int (*testfunc1)(mpfr_t, mpfr_t, mpfr_rnd_t);
typedef int (*testfunc2)(mpfr_t, mpfr_t, mpfr_t, mpfr_rnd_t);
typedef int (*testrred)(mpfr_t, mpfr_t, int *);
typedef char * (*testsemi1)(uint32 *, uint32 *);
typedef char * (*testsemi2)(uint32 *, uint32 *, uint32 *);
typedef char * (*testsemi2f)(uint32 *, uint32 *, uint32 *);
typedef char * (*testldexp)(uint32 *, uint32 *, uint32 *);
typedef char * (*testfrexp)(uint32 *, uint32 *, uint32 *);
typedef char * (*testmodf)(uint32 *, uint32 *, uint32 *);
typedef char * (*testclassify)(uint32 *, uint32 *);
typedef char * (*testclassifyf)(uint32 *, uint32 *);

typedef int (*testfunc1c)(mpc_t, mpc_t, mpc_rnd_t);
typedef int (*testfunc2c)(mpc_t, mpc_t, mpc_t, mpc_rnd_t);

typedef int (*testfunc1cr)(mpfr_t, mpc_t, mpfr_rnd_t);


typedef void (*casegen)(uint32 *, uint32, uint32);




enum {
args1,
args1f,
args2,
args2f,
rred,
rredf,
semi1,
semi1f,
semi2,
semi2f,
t_ldexp,
t_ldexpf,
t_frexp,
t_frexpf,
t_modf,
t_modff,
classify,
classifyf,
compare,
comparef,

args1c,
args2c,
args1fc,
args2fc,
args1cr,
args1fcr
};

typedef struct __testable Testable;
struct __testable {
char *name;
funcptr func;
int type;
wrapperfunc wrappers[MAXWRAPPERS];
casegen cases;
uint32 caseparam1, caseparam2;
};

extern Testable functions[];
extern const int nfunctions;

extern void init_pi(void);

int nargs_(Testable* f);

#endif
