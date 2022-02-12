


































struct arglist {
char **args;
int count;
int capacity;
char *smallarg[1];
};




#define EXP_SPLIT 0x1
#define EXP_TILDE 0x2
#define EXP_VARTILDE 0x4
#define EXP_CASE 0x10
#define EXP_SPLIT_LIT 0x20
#define EXP_LIT_QUOTED 0x40
#define EXP_GLOB 0x80

#define EXP_FULL (EXP_SPLIT | EXP_GLOB)


void emptyarglist(struct arglist *);
void appendarglist(struct arglist *, char *);
union node;
void expandarg(union node *, struct arglist *, int);
void rmescapes(char *);
int casematch(union node *, const char *);
