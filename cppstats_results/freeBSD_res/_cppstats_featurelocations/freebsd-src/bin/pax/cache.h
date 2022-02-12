
















































#define UNMLEN 32
#define GNMLEN 32
#define UID_SZ 317
#define UNM_SZ 317
#define GID_SZ 251
#define GNM_SZ 317
#define VALID 1
#define INVALID 2





typedef struct uidc {
int valid;
char name[UNMLEN];
uid_t uid;
} UIDC;

typedef struct gidc {
int valid;
char name[GNMLEN];
gid_t gid;
} GIDC;
