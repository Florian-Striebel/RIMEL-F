































int c_cchars(const void *, const void *);
int c_modes(const void *, const void *);
int csearch(char ***, struct info *);
void checkredirect(void);
void gprint(struct termios *, struct winsize *, int);
void gread(struct termios *, char *);
int ksearch(char ***, struct info *);
int msearch(char ***, struct info *);
void optlist(void);
void print(struct termios *, struct winsize *, int, enum FMT);
void usage(void) __dead2;

extern struct cchar cchars1[], cchars2[];
