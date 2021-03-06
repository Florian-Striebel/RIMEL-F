




































#include <string.h>

struct stackmark {
struct stack_block *stackp;
char *stacknxt;
int stacknleft;
};


extern char *stacknxt;
extern int stacknleft;
extern char *sstrend;

pointer ckmalloc(size_t);
pointer ckrealloc(pointer, int);
void ckfree(pointer);
char *savestr(const char *);
pointer stalloc(int);
void stunalloc(pointer);
char *stsavestr(const char *);
void setstackmark(struct stackmark *);
void popstackmark(struct stackmark *);
char *growstackstr(void);
char *makestrspace(int, char *);
char *stputbin(const char *data, size_t len, char *p);
char *stputs(const char *data, char *p);



#define stackblock() stacknxt
#define stackblocksize() stacknleft
#define grabstackblock(n) stalloc(n)
#define STARTSTACKSTR(p) p = stackblock()
#define STPUTC(c, p) do { if (p == sstrend) p = growstackstr(); *p++ = (c); } while(0)
#define CHECKSTRSPACE(n, p) { if ((size_t)(sstrend - p) < n) p = makestrspace(n, p); }
#define USTPUTC(c, p) (*p++ = (c))







#define STACKSTRNUL(p) (p == sstrend ? (p = growstackstr(), *p = '\0') : (*p = '\0'))
#define STUNPUTC(p) (--p)
#define STTOPC(p) p[-1]
#define STADJUST(amount, p) (p += (amount))
#define grabstackstr(p) stalloc((char *)p - stackblock())
#define ungrabstackstr(s, p) stunalloc((s))
#define STPUTBIN(s, len, p) p = stputbin((s), (len), p)
#define STPUTS(s, p) p = stputs((s), p)
