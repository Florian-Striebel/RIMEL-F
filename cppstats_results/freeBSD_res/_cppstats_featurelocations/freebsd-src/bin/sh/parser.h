





































#define CTLESC '\300'
#define CTLVAR '\301'
#define CTLENDVAR '\371'
#define CTLBACKQ '\372'
#define CTLQUOTE 01

#define CTLARI '\374'
#define CTLENDARI '\375'
#define CTLQUOTEMARK '\376'
#define CTLQUOTEEND '\377'


#define VSTYPE 0x0f
#define VSNUL 0x10
#define VSLINENO 0x20

#define VSQUOTE 0x80


#define VSNORMAL 0x1
#define VSMINUS 0x2
#define VSPLUS 0x3
#define VSQUESTION 0x4
#define VSASSIGN 0x5
#define VSTRIMLEFT 0x6
#define VSTRIMLEFTMAX 0x7
#define VSTRIMRIGHT 0x8
#define VSTRIMRIGHTMAX 0x9
#define VSLENGTH 0xa
#define VSERROR 0xb






#define NEOF ((union node *)-1)
extern int whichprompt;
extern const char *const parsekwd[];


union node *parsecmd(int);
union node *parsewordexp(void);
void forcealias(void);
void fixredir(union node *, const char *, int);
int goodname(const char *);
int isassignment(const char *);
char *getprompt(void *);
const char *expandstr(const char *);
