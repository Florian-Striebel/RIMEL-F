




































#define NO_PRINT 1

#define HUMANVALSTR_LEN 5

extern long blocksize;

extern int f_accesstime;
extern int f_birthtime;
extern int f_flags;
extern int f_humanval;
extern int f_label;
extern int f_inode;
extern int f_longform;
extern int f_octal;
extern int f_octal_escape;
extern int f_nonprint;
extern int f_samesort;
extern int f_sectime;
extern int f_size;
extern int f_slash;
extern int f_sortacross;
extern int f_statustime;
extern int f_thousands;
extern char *f_timeformat;
extern int f_notabs;
extern int f_type;
#if defined(COLORLS)
extern int f_color;
#endif

typedef struct {
FTSENT *list;
u_long btotal;
int entries;
int maxlen;
u_int s_block;
u_int s_flags;
u_int s_label;
u_int s_group;
u_int s_inode;
u_int s_nlink;
u_int s_size;
u_int s_user;
} DISPLAY;

typedef struct {
char *user;
char *group;
char *flags;
char *label;
char data[1];
} NAMES;
