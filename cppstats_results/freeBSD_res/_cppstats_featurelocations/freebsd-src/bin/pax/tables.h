














































#define L_TAB_SZ 2503
#define F_TAB_SZ 50503
#define N_TAB_SZ 541
#define D_TAB_SZ 317
#define A_TAB_SZ 317
#define MAXKEYLEN 64





typedef struct hrdlnk {
char *name;
dev_t dev;
ino_t ino;
u_long nlink;
struct hrdlnk *fow;
} HRDLNK;












typedef struct ftm {
int namelen;
time_t mtime;
off_t seek;
struct ftm *fow;
} FTM;








typedef struct namt {
char *oname;
char *nname;
struct namt *fow;
} NAMT;



























typedef struct devt {
dev_t dev;
struct devt *fow;
struct dlist *list;
} DEVT;

typedef struct dlist {
ino_t trunc_bits;
dev_t dev;
struct dlist *fow;
} DLIST;









typedef struct atdir {
char *name;
dev_t dev;
ino_t ino;
time_t mtime;
time_t atime;
struct atdir *fow;
} ATDIR;












typedef struct dirdata {
int nlen;
off_t npos;
mode_t mode;
time_t mtime;
time_t atime;
int frc_mode;
} DIRDATA;
