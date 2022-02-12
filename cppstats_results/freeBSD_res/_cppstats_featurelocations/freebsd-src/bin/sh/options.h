




































struct shparam {
int nparam;
unsigned char malloc;
unsigned char reset;
char **p;
char **optp;
char **optnext;
char *optptr;
};



#define eflag optval[0]
#define fflag optval[1]
#define Iflag optval[2]
#define iflag optval[3]
#define mflag optval[4]
#define nflag optval[5]
#define sflag optval[6]
#define xflag optval[7]
#define vflag optval[8]
#define Vflag optval[9]
#define Eflag optval[10]
#define Cflag optval[11]
#define aflag optval[12]
#define bflag optval[13]
#define uflag optval[14]
#define privileged optval[15]
#define Tflag optval[16]
#define Pflag optval[17]
#define hflag optval[18]
#define nologflag optval[19]
#define pipefailflag optval[20]
#define verifyflag optval[21]

#define NSHORTOPTS 19
#define NOPTS 22

extern char optval[NOPTS];
extern const char optletter[NSHORTOPTS];
#if defined(DEFINE_OPTIONS)
char optval[NOPTS];
const char optletter[NSHORTOPTS] = "efIimnsxvVECabupTPh";
static const unsigned char optname[] =
"\007errexit"
"\006noglob"
"\011ignoreeof"
"\013interactive"
"\007monitor"
"\006noexec"
"\005stdin"
"\006xtrace"
"\007verbose"
"\002vi"
"\005emacs"
"\011noclobber"
"\011allexport"
"\006notify"
"\007nounset"
"\012privileged"
"\012trapsasync"
"\010physical"
"\010trackall"
"\005nolog"
"\010pipefail"
"\006verify"
;
#endif


extern char *minusc;
extern char *arg0;
extern struct shparam shellparam;
extern char **argptr;
extern char *shoptarg;
extern char *nextopt_optptr;

void procargs(int, char **);
void optschanged(void);
void freeparam(struct shparam *);
int nextopt(const char *);
void getoptsreset(const char *);
