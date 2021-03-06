









































extern int plinno;
extern int parsenleft;
extern const char *parsenextc;

struct alias;
struct parsefile;

void resetinput(void);
int pgetc(void);
int preadbuffer(void);
int preadateof(void);
void pungetc(void);
void pushstring(const char *, int, struct alias *);
void setinputfile(const char *, int, int);
void setinputfd(int, int);
void setinputstring(const char *, int);
void popfile(void);
struct parsefile *getcurrentfile(void);
void popfilesupto(struct parsefile *);
void popallfiles(void);
void closescript(void);

#define pgetc_macro() (--parsenleft >= 0? *parsenextc++ : preadbuffer())
