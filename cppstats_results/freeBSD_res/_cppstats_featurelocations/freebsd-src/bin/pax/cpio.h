








































#define TRAILER "TRAILER!!!"




#define C_ISDIR 040000
#define C_ISFIFO 010000
#define C_ISREG 0100000
#define C_ISBLK 060000
#define C_ISCHR 020000
#define C_ISCTG 0110000
#define C_ISLNK 0120000
#define C_ISOCK 0140000
#define C_IFMT 0170000




typedef struct {
char c_magic[6];
char c_dev[6];
char c_ino[6];
char c_mode[6];
char c_uid[6];
char c_gid[6];
char c_nlink[6];
char c_rdev[6];
char c_mtime[11];
char c_namesize[6];
char c_filesize[11];
} HD_CPIO __aligned(1);

#define MAGIC 070707

#if defined(_PAX_)
#define AMAGIC "070707"
#define CPIO_MASK 0x3ffff

#endif








typedef struct {
u_char h_magic[2];
u_char h_dev[2];
u_char h_ino[2];
u_char h_mode[2];
u_char h_uid[2];
u_char h_gid[2];
u_char h_nlink[2];
u_char h_rdev[2];
u_char h_mtime_1[2];
u_char h_mtime_2[2];
u_char h_namesize[2];
u_char h_filesize_1[2];
u_char h_filesize_2[2];
} HD_BCPIO __aligned(1);

#if defined(_PAX_)



#define SHRT_EXT(ch) ((((unsigned)(ch)[0])<<8) | (((unsigned)(ch)[1])&0xff))
#define RSHRT_EXT(ch) ((((unsigned)(ch)[1])<<8) | (((unsigned)(ch)[0])&0xff))
#define CHR_WR_0(val) ((char)(((val) >> 24) & 0xff))
#define CHR_WR_1(val) ((char)(((val) >> 16) & 0xff))
#define CHR_WR_2(val) ((char)(((val) >> 8) & 0xff))
#define CHR_WR_3(val) ((char)((val) & 0xff))




#define BCPIO_PAD(x) ((2 - ((x) & 1)) & 1)
#define BCPIO_MASK 0xffff
#endif




typedef struct {
char c_magic[6];
char c_ino[8];
char c_mode[8];
char c_uid[8];
char c_gid[8];
char c_nlink[8];
char c_mtime[8];
char c_filesize[8];
char c_maj[8];
char c_min[8];
char c_rmaj[8];
char c_rmin[8];
char c_namesize[8];
char c_chksum[8];
} HD_VCPIO __aligned(1);

#define VMAGIC 070701
#define VCMAGIC 070702
#if defined(_PAX_)
#define AVMAGIC "070701"
#define AVCMAGIC "070702"
#define VCPIO_PAD(x) ((4 - ((x) & 3)) & 3)
#define VCPIO_MASK 0xffffffff
#endif
