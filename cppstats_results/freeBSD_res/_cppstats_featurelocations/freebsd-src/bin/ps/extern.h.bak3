

































struct kinfo;
struct nlist;
struct var;
struct varent;

extern fixpt_t ccpu;
extern int cflag, eval, fscale, nlistread, rawcpu;
extern unsigned long mempages;
extern time_t now;
extern int showthreads, sumrusage, termwidth;
extern STAILQ_HEAD(velisthead, varent) varlist;

__BEGIN_DECLS
char *arguments(KINFO *, VARENT *);
char *command(KINFO *, VARENT *);
char *cputime(KINFO *, VARENT *);
char *cpunum(KINFO *, VARENT *);
int donlist(void);
char *elapsed(KINFO *, VARENT *);
char *elapseds(KINFO *, VARENT *);
char *emulname(KINFO *, VARENT *);
VARENT *find_varentry(VAR *);
const char *fmt_argv(char **, char *, char *, size_t);
double getpcpu(const KINFO *);
char *jailname(KINFO *, VARENT *);
char *kvar(KINFO *, VARENT *);
char *label(KINFO *, VARENT *);
char *loginclass(KINFO *, VARENT *);
char *logname(KINFO *, VARENT *);
char *longtname(KINFO *, VARENT *);
char *lstarted(KINFO *, VARENT *);
char *maxrss(KINFO *, VARENT *);
char *lockname(KINFO *, VARENT *);
char *mwchan(KINFO *, VARENT *);
char *nwchan(KINFO *, VARENT *);
char *pagein(KINFO *, VARENT *);
void parsefmt(const char *, int);
char *pcpu(KINFO *, VARENT *);
char *pmem(KINFO *, VARENT *);
char *pri(KINFO *, VARENT *);
void printheader(void);
char *priorityr(KINFO *, VARENT *);
char *egroupname(KINFO *, VARENT *);
char *rgroupname(KINFO *, VARENT *);
char *runame(KINFO *, VARENT *);
char *rvar(KINFO *, VARENT *);
void showkey(void);
char *started(KINFO *, VARENT *);
char *state(KINFO *, VARENT *);
char *systime(KINFO *, VARENT *);
char *tdev(KINFO *, VARENT *);
char *tdnam(KINFO *, VARENT *);
char *tname(KINFO *, VARENT *);
char *ucomm(KINFO *, VARENT *);
char *username(KINFO *, VARENT *);
char *upr(KINFO *, VARENT *);
char *usertime(KINFO *, VARENT *);
char *vsize(KINFO *, VARENT *);
char *wchan(KINFO *, VARENT *);
__END_DECLS
