






























#if !defined(LIB9P_FCALL_H)
#define LIB9P_FCALL_H

#include <stdint.h>

#define L9P_MAX_WELEM 256










enum l9p_ftype {
L9P__FIRST = 6,
L9P_TLERROR = 6,
L9P_RLERROR,
L9P_TSTATFS = 8,
L9P_RSTATFS,
L9P_TLOPEN = 12,
L9P_RLOPEN,
L9P_TLCREATE = 14,
L9P_RLCREATE,
L9P_TSYMLINK = 16,
L9P_RSYMLINK,
L9P_TMKNOD = 18,
L9P_RMKNOD,
L9P_TRENAME = 20,
L9P_RRENAME,
L9P_TREADLINK = 22,
L9P_RREADLINK,
L9P_TGETATTR = 24,
L9P_RGETATTR,
L9P_TSETATTR = 26,
L9P_RSETATTR,
L9P_TXATTRWALK = 30,
L9P_RXATTRWALK,
L9P_TXATTRCREATE = 32,
L9P_RXATTRCREATE,
L9P_TREADDIR = 40,
L9P_RREADDIR,
L9P_TFSYNC = 50,
L9P_RFSYNC,
L9P_TLOCK = 52,
L9P_RLOCK,
L9P_TGETLOCK = 54,
L9P_RGETLOCK,
L9P_TLINK = 70,
L9P_RLINK,
L9P_TMKDIR = 72,
L9P_RMKDIR,
L9P_TRENAMEAT = 74,
L9P_RRENAMEAT,
L9P_TUNLINKAT = 76,
L9P_RUNLINKAT,
L9P_TVERSION = 100,
L9P_RVERSION,
L9P_TAUTH = 102,
L9P_RAUTH,
L9P_TATTACH = 104,
L9P_RATTACH,
L9P_TERROR = 106,
L9P_RERROR,
L9P_TFLUSH = 108,
L9P_RFLUSH,
L9P_TWALK = 110,
L9P_RWALK,
L9P_TOPEN = 112,
L9P_ROPEN,
L9P_TCREATE = 114,
L9P_RCREATE,
L9P_TREAD = 116,
L9P_RREAD,
L9P_TWRITE = 118,
L9P_RWRITE,
L9P_TCLUNK = 120,
L9P_RCLUNK,
L9P_TREMOVE = 122,
L9P_RREMOVE,
L9P_TSTAT = 124,
L9P_RSTAT,
L9P_TWSTAT = 126,
L9P_RWSTAT,
L9P__LAST_PLUS_1,
};











































enum l9p_qid_type {
L9P_QTDIR = 0x80,
L9P_QTAPPEND = 0x40,
L9P_QTEXCL = 0x20,
L9P_QTMOUNT = 0x10,
L9P_QTAUTH = 0x08,
L9P_QTTMP = 0x04,
L9P_QTSYMLINK = 0x02,
L9P_QTFILE = 0x00
};




#define L9P_DMDIR 0x80000000
enum {
L9P_DMAPPEND = 0x40000000,
L9P_DMEXCL = 0x20000000,
L9P_DMMOUNT = 0x10000000,
L9P_DMAUTH = 0x08000000,
L9P_DMTMP = 0x04000000,
L9P_DMSYMLINK = 0x02000000,

L9P_DMDEVICE = 0x00800000,
L9P_DMNAMEDPIPE = 0x00200000,
L9P_DMSOCKET = 0x00100000,
L9P_DMSETUID = 0x00080000,
L9P_DMSETGID = 0x00040000,
};






enum l9p_omode {
L9P_OREAD = 0,
L9P_OWRITE = 1,
L9P_ORDWR = 2,
L9P_OEXEC = 3,
L9P_OACCMODE = 3,
L9P_OTRUNC = 16,
L9P_OCEXEC = 32,
L9P_ORCLOSE = 64,
L9P_ODIRECT = 128,
};


















enum l9p_l_o_flags {
L9P_L_O_CREAT = 000000100U,
L9P_L_O_EXCL = 000000200U,
L9P_L_O_NOCTTY = 000000400U,
L9P_L_O_TRUNC = 000001000U,
L9P_L_O_APPEND = 000002000U,
L9P_L_O_NONBLOCK = 000004000U,
L9P_L_O_DSYNC = 000010000U,
L9P_L_O_FASYNC = 000020000U,
L9P_L_O_DIRECT = 000040000U,
L9P_L_O_LARGEFILE = 000100000U,
L9P_L_O_DIRECTORY = 000200000U,
L9P_L_O_NOFOLLOW = 000400000U,
L9P_L_O_NOATIME = 001000000U,
L9P_L_O_CLOEXEC = 002000000U,
L9P_L_O_SYNC = 004000000U,
L9P_L_O_PATH = 010000000U,
L9P_L_O_TMPFILE = 020000000U,
};

struct l9p_hdr {
uint8_t type;
uint16_t tag;
uint32_t fid;
};

struct l9p_qid {
uint8_t type;
uint32_t version;
uint64_t path;
};

struct l9p_stat {
uint16_t type;
uint32_t dev;
struct l9p_qid qid;
uint32_t mode;
uint32_t atime;
uint32_t mtime;
uint64_t length;
char *name;
char *uid;
char *gid;
char *muid;
char *extension;
uint32_t n_uid;
uint32_t n_gid;
uint32_t n_muid;
};

#define L9P_FSTYPE 0x01021997

struct l9p_statfs {
uint32_t type;
uint32_t bsize;
uint64_t blocks;
uint64_t bfree;
uint64_t bavail;
uint64_t files;
uint64_t ffree;
uint64_t fsid;
uint32_t namelen;
};

struct l9p_f_version {
struct l9p_hdr hdr;
uint32_t msize;
char *version;
};

struct l9p_f_tflush {
struct l9p_hdr hdr;
uint16_t oldtag;
};

struct l9p_f_error {
struct l9p_hdr hdr;
char *ename;
uint32_t errnum;
};

struct l9p_f_ropen {
struct l9p_hdr hdr;
struct l9p_qid qid;
uint32_t iounit;
};

struct l9p_f_rauth {
struct l9p_hdr hdr;
struct l9p_qid aqid;
};

struct l9p_f_attach {
struct l9p_hdr hdr;
uint32_t afid;
char *uname;
char *aname;
uint32_t n_uname;
};
#define L9P_NOFID ((uint32_t)-1)
#define L9P_NONUNAME ((uint32_t)-1)

struct l9p_f_tcreate {
struct l9p_hdr hdr;
uint32_t perm;
char *name;
uint8_t mode;
char *extension;
};

struct l9p_f_twalk {
struct l9p_hdr hdr;
uint32_t newfid;
uint16_t nwname;
char *wname[L9P_MAX_WELEM];
};

struct l9p_f_rwalk {
struct l9p_hdr hdr;
uint16_t nwqid;
struct l9p_qid wqid[L9P_MAX_WELEM];
};

struct l9p_f_io {
struct l9p_hdr hdr;
uint64_t offset;
uint32_t count;
};

struct l9p_f_rstat {
struct l9p_hdr hdr;
struct l9p_stat stat;
};

struct l9p_f_twstat {
struct l9p_hdr hdr;
struct l9p_stat stat;
};

struct l9p_f_rstatfs {
struct l9p_hdr hdr;
struct l9p_statfs statfs;
};


struct l9p_f_tlcreate {
struct l9p_hdr hdr;
char *name;
uint32_t flags;
uint32_t mode;
uint32_t gid;
};

struct l9p_f_tsymlink {
struct l9p_hdr hdr;
char *name;
char *symtgt;
uint32_t gid;
};

struct l9p_f_tmknod {
struct l9p_hdr hdr;
char *name;
uint32_t mode;
uint32_t major;
uint32_t minor;
uint32_t gid;
};

struct l9p_f_trename {
struct l9p_hdr hdr;
uint32_t dfid;
char *name;
};

struct l9p_f_rreadlink {
struct l9p_hdr hdr;
char *target;
};

struct l9p_f_tgetattr {
struct l9p_hdr hdr;
uint64_t request_mask;
};

struct l9p_f_rgetattr {
struct l9p_hdr hdr;
uint64_t valid;
struct l9p_qid qid;
uint32_t mode;
uint32_t uid;
uint32_t gid;
uint64_t nlink;
uint64_t rdev;
uint64_t size;
uint64_t blksize;
uint64_t blocks;
uint64_t atime_sec;
uint64_t atime_nsec;
uint64_t mtime_sec;
uint64_t mtime_nsec;
uint64_t ctime_sec;
uint64_t ctime_nsec;
uint64_t btime_sec;
uint64_t btime_nsec;
uint64_t gen;
uint64_t data_version;
};


enum l9pl_getattr_flags {
L9PL_GETATTR_MODE = 0x00000001,
L9PL_GETATTR_NLINK = 0x00000002,
L9PL_GETATTR_UID = 0x00000004,
L9PL_GETATTR_GID = 0x00000008,
L9PL_GETATTR_RDEV = 0x00000010,
L9PL_GETATTR_ATIME = 0x00000020,
L9PL_GETATTR_MTIME = 0x00000040,
L9PL_GETATTR_CTIME = 0x00000080,
L9PL_GETATTR_INO = 0x00000100,
L9PL_GETATTR_SIZE = 0x00000200,
L9PL_GETATTR_BLOCKS = 0x00000400,

L9PL_GETATTR_BASIC = L9PL_GETATTR_MODE |
L9PL_GETATTR_NLINK |
L9PL_GETATTR_UID |
L9PL_GETATTR_GID |
L9PL_GETATTR_RDEV |
L9PL_GETATTR_ATIME |
L9PL_GETATTR_MTIME |
L9PL_GETATTR_CTIME |
L9PL_GETATTR_INO |
L9PL_GETATTR_SIZE |
L9PL_GETATTR_BLOCKS,
L9PL_GETATTR_BTIME = 0x00000800,
L9PL_GETATTR_GEN = 0x00001000,
L9PL_GETATTR_DATA_VERSION = 0x00002000,

L9PL_GETATTR_ALL = L9PL_GETATTR_BASIC |
L9PL_GETATTR_BTIME |
L9PL_GETATTR_GEN |
L9PL_GETATTR_DATA_VERSION,
};

struct l9p_f_tsetattr {
struct l9p_hdr hdr;
uint32_t valid;
uint32_t mode;
uint32_t uid;
uint32_t gid;
uint64_t size;
uint64_t atime_sec;
uint64_t atime_nsec;
uint64_t mtime_sec;
uint64_t mtime_nsec;
};


enum l9pl_setattr_flags {
L9PL_SETATTR_MODE = 0x00000001,
L9PL_SETATTR_UID = 0x00000002,
L9PL_SETATTR_GID = 0x00000004,
L9PL_SETATTR_SIZE = 0x00000008,
L9PL_SETATTR_ATIME = 0x00000010,
L9PL_SETATTR_MTIME = 0x00000020,
L9PL_SETATTR_CTIME = 0x00000040,
L9PL_SETATTR_ATIME_SET = 0x00000080,
L9PL_SETATTR_MTIME_SET = 0x00000100,
};

struct l9p_f_txattrwalk {
struct l9p_hdr hdr;
uint32_t newfid;
char *name;
};

struct l9p_f_rxattrwalk {
struct l9p_hdr hdr;
uint64_t size;
};

struct l9p_f_txattrcreate {
struct l9p_hdr hdr;
char *name;
uint64_t attr_size;
uint32_t flags;
};

struct l9p_f_tlock {
struct l9p_hdr hdr;
uint8_t type;
uint32_t flags;
uint64_t start;
uint64_t length;
uint32_t proc_id;
char *client_id;
};

enum l9pl_lock_type {
L9PL_LOCK_TYPE_RDLOCK = 0,
L9PL_LOCK_TYPE_WRLOCK = 1,
L9PL_LOCK_TYPE_UNLOCK = 2,
};

enum l9pl_lock_flags {
L9PL_LOCK_TYPE_BLOCK = 1,
L9PL_LOCK_TYPE_RECLAIM = 2,
};

struct l9p_f_rlock {
struct l9p_hdr hdr;
uint8_t status;
};

enum l9pl_lock_status {
L9PL_LOCK_SUCCESS = 0,
L9PL_LOCK_BLOCKED = 1,
L9PL_LOCK_ERROR = 2,
L9PL_LOCK_GRACE = 3,
};

struct l9p_f_getlock {
struct l9p_hdr hdr;
uint8_t type;
uint64_t start;
uint64_t length;
uint32_t proc_id;
char *client_id;
};

struct l9p_f_tlink {
struct l9p_hdr hdr;
uint32_t dfid;
char *name;
};

struct l9p_f_trenameat {
struct l9p_hdr hdr;
char *oldname;
uint32_t newdirfid;
char *newname;
};





enum l9p_l_unlinkat_flags {

L9PL_AT_REMOVEDIR = 0x0200,
};

union l9p_fcall {
struct l9p_hdr hdr;
struct l9p_f_version version;
struct l9p_f_tflush tflush;
struct l9p_f_ropen ropen;
struct l9p_f_ropen rcreate;
struct l9p_f_ropen rattach;
struct l9p_f_error error;
struct l9p_f_rauth rauth;
struct l9p_f_attach tattach;
struct l9p_f_attach tauth;
struct l9p_f_tcreate tcreate;
struct l9p_f_tcreate topen;
struct l9p_f_twalk twalk;
struct l9p_f_rwalk rwalk;
struct l9p_f_twstat twstat;
struct l9p_f_rstat rstat;
struct l9p_f_rstatfs rstatfs;
struct l9p_f_tlcreate tlopen;
struct l9p_f_ropen rlopen;
struct l9p_f_tlcreate tlcreate;
struct l9p_f_ropen rlcreate;
struct l9p_f_tsymlink tsymlink;
struct l9p_f_ropen rsymlink;
struct l9p_f_tmknod tmknod;
struct l9p_f_ropen rmknod;
struct l9p_f_trename trename;
struct l9p_f_rreadlink rreadlink;
struct l9p_f_tgetattr tgetattr;
struct l9p_f_rgetattr rgetattr;
struct l9p_f_tsetattr tsetattr;
struct l9p_f_txattrwalk txattrwalk;
struct l9p_f_rxattrwalk rxattrwalk;
struct l9p_f_txattrcreate txattrcreate;
struct l9p_f_tlock tlock;
struct l9p_f_rlock rlock;
struct l9p_f_getlock getlock;
struct l9p_f_tlink tlink;
struct l9p_f_tlcreate tmkdir;
struct l9p_f_ropen rmkdir;
struct l9p_f_trenameat trenameat;
struct l9p_f_tlcreate tunlinkat;
struct l9p_f_io io;
};

#endif
