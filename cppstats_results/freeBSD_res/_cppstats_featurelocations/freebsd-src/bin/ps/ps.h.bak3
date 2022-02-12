

































#include <sys/queue.h>

#define UNLIMITED 0
enum type { CHAR, UCHAR, SHORT, USHORT, INT, UINT, LONG, ULONG, KPTR, PGTOK };

typedef struct kinfo_str {
STAILQ_ENTRY(kinfo_str) ks_next;
char *ks_str;
} KINFO_STR;

typedef struct kinfo {
struct kinfo_proc *ki_p;
const char *ki_args;
const char *ki_env;
int ki_valid;
double ki_pcpu;
segsz_t ki_memsize;
union {
int level;
char *prefix;
} ki_d;
STAILQ_HEAD(, kinfo_str) ki_ks;
} KINFO;


typedef struct varent {
STAILQ_ENTRY(varent) next_ve;
const char *header;
struct var *var;
} VARENT;

typedef struct var {
const char *name;
const char *header;
const char *alias;
const char *field;
#define COMM 0x01
#define LJUST 0x02
#define USER 0x04
#define INF127 0x10
u_int flag;

char *(*oproc)(struct kinfo *, struct varent *);





size_t off;
enum type type;
const char *fmt;

short width;
} VAR;

#include "extern.h"
