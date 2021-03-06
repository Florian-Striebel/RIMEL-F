








































#define CHK_LEN 8
#define TNMSZ 100
#if defined(_PAX_)
#define NULLCNT 2
#define CHK_OFFSET 148
#define BLNKSUM 256L
#endif





#define REGTYPE '0'
#define AREGTYPE '\0'
#define LNKTYPE '1'
#define SYMTYPE '2'
#define CHRTYPE '3'
#define BLKTYPE '4'
#define DIRTYPE '5'
#define FIFOTYPE '6'
#define CONTTYPE '7'




#define TSUID 04000
#define TSGID 02000
#define TSVTX 01000
#define TUREAD 00400
#define TUWRITE 00200
#define TUEXEC 00100
#define TGREAD 00040
#define TGWRITE 00020
#define TGEXEC 00010
#define TOREAD 00004
#define TOWRITE 00002
#define TOEXEC 00001

#if defined(_PAX_)




#define TAR_PAD(x) ((512 - ((x) & 511)) & 511)
#endif




typedef struct {
char name[TNMSZ];
char mode[8];
char uid[8];
char gid[8];
char size[12];
char mtime[12];
char chksum[CHK_LEN];
char linkflag;
char linkname[TNMSZ];
} HD_TAR __aligned(1);

#if defined(_PAX_)



#define TAR_NODIR "nodir"
#define TAR_OPTION "write_opt"




#define DEV_0 "/dev/rmt0"
#define DEV_1 "/dev/rmt1"
#define DEV_4 "/dev/rmt4"
#define DEV_5 "/dev/rmt5"
#define DEV_7 "/dev/rmt7"
#define DEV_8 "/dev/rmt8"
#endif




#define TPFSZ 155
#define TMAGIC "ustar"
#define TMAGLEN 6
#define TVERSION "00"
#define TVERSLEN 2

typedef struct {
char name[TNMSZ];
char mode[8];
char uid[8];
char gid[8];
char size[12];
char mtime[12];
char chksum[CHK_LEN];
char typeflag;
char linkname[TNMSZ];
char magic[TMAGLEN];
char version[TVERSLEN];
char uname[32];
char gname[32];
char devmajor[8];
char devminor[8];
char prefix[TPFSZ];
} HD_USTAR __aligned(1);
