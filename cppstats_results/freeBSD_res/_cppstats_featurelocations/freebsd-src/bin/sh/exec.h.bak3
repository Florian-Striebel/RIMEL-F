



































#define CMDUNKNOWN -1
#define CMDNORMAL 0
#define CMDBUILTIN 1
#define CMDFUNCTION 2


enum {
TYPECMD_SMALLV,
TYPECMD_BIGV,
TYPECMD_TYPE
};

union node;
struct cmdentry {
int cmdtype;
union param {
int index;
struct funcdef *func;
} u;
int special;
};



#define DO_ERR 0x01
#define DO_NOFUNC 0x02

void shellexec(char **, char **, const char *, int) __dead2;
char *padvance(const char **, const char **, const char *);
void find_command(const char *, struct cmdentry *, int, const char *);
int find_builtin(const char *, int *);
void hashcd(void);
void changepath(const char *);
void defun(const char *, union node *);
int unsetfunc(const char *);
int isfunc(const char *);
int typecmd_impl(int, char **, int, const char *);
void clearcmdentry(void);
