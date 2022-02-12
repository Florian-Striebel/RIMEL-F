































#if !defined(__CONFIG_PARAMS_H__)
#define __CONFIG_PARAMS_H__





#define MAX_LINE 2048
#define MAX_HOST_LIST 64000
#define MAX_ACTIVE_LIST 10

struct active_pars
{
char address[MAX_LINE + 1];
char port[MAX_LINE + 1];
int ai_family;
};

extern char hostlist[MAX_HOST_LIST + 1];
extern struct active_pars activelist[MAX_ACTIVE_LIST];
extern int nullAuthAllowed;
extern char loadfile[MAX_LINE + 1];

#endif
