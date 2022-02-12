


































extern char *commandname;
extern int exitstatus;
extern int oexitstatus;
extern struct arglist *cmdenviron;


struct backcmd {
int fd;
char *buf;
int nleft;
struct job *jp;
};

void reseteval(void);


#define EV_EXIT 01
#define EV_TESTED 02
#define EV_BACKCMD 04

void evalstring(const char *, int);
union node;
void evaltree(union node *, int);
void evalbackcmd(union node *, struct backcmd *);


#define in_function() funcnest
extern int funcnest;
extern int evalskip;
extern int skipcount;


#define SKIPBREAK 1
#define SKIPCONT 2
#define SKIPRETURN 3
