









































#define USR_TB_SZ 317
#define GRP_TB_SZ 317

typedef struct usrt {
uid_t uid;
struct usrt *fow;
} USRT;

typedef struct grpt {
gid_t gid;
struct grpt *fow;
} GRPT;





#define ATOI2(s) ((((s)[0] - '0') * 10) + ((s)[1] - '0'))

typedef struct time_rng {
time_t low_time;
time_t high_time;
int flgs;
#define HASLOW 0x01
#define HASHIGH 0x02
#define CMPMTME 0x04
#define CMPCTME 0x08
#define CMPBOTH (CMPMTME|CMPCTME)
struct time_rng *fow;
} TIME_RNG;
