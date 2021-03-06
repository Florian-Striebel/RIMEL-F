









































#define VEXPORT 0x01
#define VREADONLY 0x02
#define VSTRFIXED 0x04
#define VTEXTFIXED 0x08
#define VSTACK 0x10
#define VUNSET 0x20
#define VNOFUNC 0x40
#define VNOSET 0x80
#define VNOLOCAL 0x100


struct var {
struct var *next;
int flags;
int name_len;
char *text;
void (*func)(const char *);


};


struct localvar {
struct localvar *next;
struct var *vp;
int flags;
char *text;
};


extern struct localvar *localvars;
extern int forcelocal;

extern struct var vifs;
extern struct var vmail;
extern struct var vmpath;
extern struct var vpath;
extern struct var vps1;
extern struct var vps2;
extern struct var vps4;
extern struct var vdisvfork;
#if !defined(NO_HISTORY)
extern struct var vhistsize;
extern struct var vterm;
#endif

extern int localeisutf8;

extern int initial_localeisutf8;







#define ifsval() (vifs.text + 4)
#define ifsset() ((vifs.flags & VUNSET) == 0)
#define mailval() (vmail.text + 5)
#define mpathval() (vmpath.text + 9)
#define pathval() (vpath.text + 5)
#define ps1val() (vps1.text + 4)
#define ps2val() (vps2.text + 4)
#define ps4val() (vps4.text + 4)
#define optindval() (voptind.text + 7)
#if !defined(NO_HISTORY)
#define histsizeval() (vhistsize.text + 9)
#define termval() (vterm.text + 5)
#endif

#define mpathset() ((vmpath.flags & VUNSET) == 0)
#define disvforkset() ((vdisvfork.flags & VUNSET) == 0)

void initvar(void);
void setvar(const char *, const char *, int);
void setvareq(char *, int);
struct arglist;
void listsetvar(struct arglist *, int);
char *lookupvar(const char *);
char *bltinlookup(const char *, int);
void bltinsetlocale(void);
void bltinunsetlocale(void);
void updatecharset(void);
void initcharset(void);
char **environment(void);
int showvarscmd(int, char **);
void mklocal(char *);
void poplocalvars(void);
int unsetvar(const char *);
int setvarsafe(const char *, const char *, int);
