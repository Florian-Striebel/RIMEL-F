

































void showtree(union node *);
#if defined(DEBUG)
void sh_trace(const char *, ...) __printflike(1, 2);
void trargs(char **);
void trputc(int);
void trputs(const char *);
void opentrace(void);

extern int debug;
#endif
