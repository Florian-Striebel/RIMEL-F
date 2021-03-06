









































#define MAXBLK 64512


#define MAXBLK_POSIX 32256
#define BLKMULT 512

#define DEVBLK 8192
#define FILEBLK 10240
#define PAXPATHLEN 3072





#define LIST 0
#define EXTRACT 1
#define ARCHIVE 2
#define APPND 3
#define COPY 4
#define DEFOP LIST




#define ISREG 0
#define ISCHR 1
#define ISBLK 2
#define ISTAPE 3
#define ISPIPE 4

typedef struct archd ARCHD;
typedef struct fsub FSUB;
typedef struct oplist OPLIST;
typedef struct pattern PATTERN;











struct fsub {
const char *name;

int bsz;



int hsz;




int udev;






int hlk;


int blkalgn;
int inhead;


int (*id)(char *, int);

int (*st_rd)(void);


int (*rd)(ARCHD *, char *);












off_t (*end_rd)(void);



int (*st_wr)(void);
int (*wr)(ARCHD *);









int (*end_wr)(void);


int (*trail_cpio)(ARCHD *);
int (*trail_tar)(char *, int, int *);






int (*rd_data)(ARCHD *, int, off_t *);

int (*wr_data)(ARCHD *, int, off_t *);

int (*options)(void);
};






struct pattern {
char *pstr;
char *pend;
char *chdname;
int plen;
int flgs;
#define MTCH 0x1
#define DIR_MTCH 0x2
struct pattern *fow;
};













struct archd {
int nlen;
char name[PAXPATHLEN+1];
int ln_nlen;
char ln_name[PAXPATHLEN+1];
char *org_name;
PATTERN *pat;
struct stat sb;
off_t pad;
off_t skip;



u_long crc;
int type;
#define PAX_DIR 1
#define PAX_CHR 2
#define PAX_BLK 3
#define PAX_REG 4
#define PAX_SLK 5
#define PAX_SCK 6
#define PAX_FIF 7
#define PAX_HLK 8
#define PAX_HRG 9
#define PAX_CTG 10
};






struct oplist {
char *name;
char *value;
struct oplist *fow;
};




#if !defined(MIN)
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#define MAJOR(x) major(x)
#define MINOR(x) minor(x)
#define TODEV(x, y) makedev((x), (y))




#define HEX 16
#define OCT 8
#define _PAX_ 1
#define _TFILE_BASE "paxXXXXXXXXXX"
