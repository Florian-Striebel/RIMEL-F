






#if !defined(_REGEXP)
#define _REGEXP 1

#define NSUBEXP 10
typedef struct regexp {
char *startp[NSUBEXP];
char *endp[NSUBEXP];
char regstart;
char reganch;
char *regmust;
int regmlen;
char program[1];
} regexp;

#if defined(__STDC__) || defined(__cplusplus)
#define _ANSI_ARGS_(x) x
#else
#define _ANSI_ARGS_(x) ()
#endif

extern regexp *regcomp _ANSI_ARGS_((char *exp));
extern int regexec _ANSI_ARGS_((regexp *prog, char *string));
extern int regexec2 _ANSI_ARGS_((regexp *prog, char *string, int notbol));
extern void regsub _ANSI_ARGS_((regexp *prog, char *source, char *dest));
extern void regerror _ANSI_ARGS_((char *msg));

#endif
