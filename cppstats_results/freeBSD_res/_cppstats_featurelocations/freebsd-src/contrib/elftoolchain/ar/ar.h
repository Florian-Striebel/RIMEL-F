



























#include <libelf.h>

#include "_elftc.h"




#define AR_A 0x0001
#define AR_B 0x0002
#define AR_C 0x0004
#define AR_CC 0x0008
#define AR_J 0x0010
#define AR_O 0x0020
#define AR_S 0x0040
#define AR_SS 0x0080
#define AR_TR 0x0100
#define AR_U 0x0200
#define AR_V 0x0400
#define AR_Z 0x0800
#define AR_D 0x1000
#define AR_BSD 0x2000

#define DEF_BLKSZ 10240



#define AR_STRINGTAB_NAME_SVR4 "//"
#define AR_SYMTAB_NAME_BSD "__.SYMDEF"
#define AR_SYMTAB_NAME_SVR4 "/"




#define AC(CALL) do { if ((CALL)) bsdar_errc(bsdar, 0, "%s", archive_error_string(a)); } while (0)









#if ARCHIVE_VERSION_NUMBER >= 2000000
#define ACV(CALL) AC(CALL)
#else
#define ACV(CALL) do { (CALL); } while (0)


#endif




struct ar_obj {
Elf *elf;
char *name;
uid_t uid;
gid_t gid;
mode_t md;
size_t size;
time_t mtime;
dev_t dev;
ino_t ino;

TAILQ_ENTRY(ar_obj) objs;
};




struct bsdar {
const char *filename;
const char *addlib;
const char *posarg;
char mode;
int options;
FILE *output;

const char *progname;
int argc;
char **argv;

dev_t ar_dev;
ino_t ar_ino;




char *as;
size_t as_sz;
size_t as_cap;




uint32_t s_cnt;
uint32_t *s_so;
size_t s_so_cap;
char *s_sn;
size_t s_sn_cap;
size_t s_sn_sz;

off_t rela_off;

TAILQ_HEAD(, ar_obj) v_obj;
};

void ar_mode_script(struct bsdar *ar);
int ar_read_archive(struct bsdar *_ar, int _mode);
int ar_write_archive(struct bsdar *_ar, int _mode);
void bsdar_errc(struct bsdar *, int _code, const char *fmt, ...);
int bsdar_is_pseudomember(struct bsdar *_ar, const char *_name);
const char *bsdar_strmode(mode_t m);
void bsdar_warnc(struct bsdar *, int _code, const char *fmt, ...);
