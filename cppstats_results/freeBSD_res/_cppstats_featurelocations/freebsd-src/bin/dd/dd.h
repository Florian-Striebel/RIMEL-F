






































typedef struct {
u_char *db;
u_char *dbp;
ssize_t dbcnt;
ssize_t dbrcnt;
ssize_t dbsz;

#define ISCHR 0x01
#define ISPIPE 0x02
#define ISTAPE 0x04
#define ISSEEK 0x08
#define NOREAD 0x10
#define ISTRUNC 0x20
u_int flags;

const char *name;
int fd;
off_t offset;
off_t seek_offset;
} IO;

typedef struct {
uintmax_t in_full;
uintmax_t in_part;
uintmax_t out_full;
uintmax_t out_part;
uintmax_t trunc;
uintmax_t swab;
uintmax_t bytes;
struct timespec start;
} STAT;


#define C_ASCII 0x0000000000000001ULL
#define C_BLOCK 0x0000000000000002ULL
#define C_BS 0x0000000000000004ULL
#define C_CBS 0x0000000000000008ULL
#define C_COUNT 0x0000000000000010ULL
#define C_EBCDIC 0x0000000000000020ULL
#define C_FILES 0x0000000000000040ULL
#define C_IBS 0x0000000000000080ULL
#define C_IF 0x0000000000000100ULL
#define C_LCASE 0x0000000000000200ULL
#define C_NOERROR 0x0000000000000400ULL
#define C_NOTRUNC 0x0000000000000800ULL
#define C_OBS 0x0000000000001000ULL
#define C_OF 0x0000000000002000ULL
#define C_OSYNC 0x0000000000004000ULL
#define C_PAREVEN 0x0000000000008000ULL
#define C_PARNONE 0x0000000000010000ULL
#define C_PARODD 0x0000000000020000ULL
#define C_PARSET 0x0000000000040000ULL
#define C_SEEK 0x0000000000080000ULL
#define C_SKIP 0x0000000000100000ULL
#define C_SPARSE 0x0000000000200000ULL
#define C_SWAB 0x0000000000400000ULL
#define C_SYNC 0x0000000000800000ULL
#define C_UCASE 0x0000000001000000ULL
#define C_UNBLOCK 0x0000000002000000ULL
#define C_FILL 0x0000000004000000ULL
#define C_STATUS 0x0000000008000000ULL
#define C_NOXFER 0x0000000010000000ULL
#define C_NOINFO 0x0000000020000000ULL
#define C_PROGRESS 0x0000000040000000ULL
#define C_FSYNC 0x0000000080000000ULL
#define C_FDATASYNC 0x0000000100000000ULL
#define C_OFSYNC 0x0000000200000000ULL
#define C_IFULLBLOCK 0x0000000400000000ULL
#define C_IDIRECT 0x0000000800000000ULL
#define C_ODIRECT 0x0000001000000000ULL

#define C_PARITY (C_PAREVEN | C_PARODD | C_PARNONE | C_PARSET)

#define BISZERO(p, s) ((s) > 0 && *((const char *)p) == 0 && !memcmp( (const void *)(p), (const void *) ((const char *)p + 1), (s) - 1))

