



























#include <sys/queue.h>
#include <gelf.h>
#include <libelftc.h>

#include "_elftc.h"





struct symop {
const char *name;
const char *newname;

#define SYMOP_KEEP 0x0001U
#define SYMOP_STRIP 0x0002U
#define SYMOP_GLOBALIZE 0x0004U
#define SYMOP_LOCALIZE 0x0008U
#define SYMOP_KEEPG 0x0010U
#define SYMOP_WEAKEN 0x0020U
#define SYMOP_REDEF 0x0040U

unsigned int op;

STAILQ_ENTRY(symop) symop_list;
};


struct symfile {
dev_t dev;
ino_t ino;
size_t size;
char *data;
unsigned int op;

STAILQ_ENTRY(symfile) symfile_list;
};


struct sec_action {
const char *name;
const char *addopt;
const char *newname;
const char *string;
uint64_t lma;
uint64_t vma;
int64_t lma_adjust;
int64_t vma_adjust;

#define SF_ALLOC 0x0001U
#define SF_LOAD 0x0002U
#define SF_NOLOAD 0x0004U
#define SF_READONLY 0x0008U
#define SF_DEBUG 0x0010U
#define SF_CODE 0x0020U
#define SF_DATA 0x0040U
#define SF_ROM 0x0080U
#define SF_SHARED 0X0100U
#define SF_CONTENTS 0x0200U

int flags;
int add;
int append;
int compress;
int copy;
int print;
int remove;
int rename;
int setflags;
int setlma;
int setvma;

STAILQ_ENTRY(sec_action) sac_list;
};


struct sec_add {
char *name;
char *content;
size_t size;

STAILQ_ENTRY(sec_add) sadd_list;
};

struct segment;


struct section {
struct segment *seg;
struct segment *seg_tls;
const char *name;
char *newname;
Elf_Scn *is;
Elf_Scn *os;
void *buf;
uint8_t *pad;
uint64_t off;
uint64_t sz;
uint64_t cap;
uint64_t align;
uint64_t type;
uint64_t flags;
uint64_t vma;
uint64_t lma;
uint64_t pad_sz;
int loadable;
int pseudo;
int nocopy;

Elftc_String_Table *strtab;

TAILQ_ENTRY(section) sec_list;
};

TAILQ_HEAD(sectionlist, section);


struct segment {
uint64_t vaddr;
uint64_t paddr;
uint64_t off;
uint64_t fsz;
uint64_t msz;
uint64_t type;
int remove;
int nsec;
struct section **v_sec;

STAILQ_ENTRY(segment) seg_list;
};




struct ar_obj {
char *name;
char *buf;
void *maddr;
uid_t uid;
gid_t gid;
mode_t md;
size_t size;
time_t mtime;

STAILQ_ENTRY(ar_obj) objs;
};




struct elfcopy {
const char *progname;
int iec;
Elftc_Bfd_Target_Flavor itf;
Elftc_Bfd_Target_Flavor otf;
const char *otgt;
int oec;
unsigned char oed;
int oem;
int abi;
Elf *ein;
Elf *eout;
int iphnum;
int ophnum;
int nos;

enum {
STRIP_NONE = 0,
STRIP_ALL,
STRIP_DEBUG,
STRIP_DWO,
STRIP_NONDEBUG,
STRIP_NONDWO,
STRIP_UNNEEDED
} strip;

#define EXECUTABLE 0x00000001U
#define DYNAMIC 0x00000002U
#define RELOCATABLE 0x00000004U
#define SYMTAB_EXIST 0x00000010U
#define SYMTAB_INTACT 0x00000020U
#define KEEP_GLOBAL 0x00000040U
#define DISCARD_LOCAL 0x00000080U
#define WEAKEN_ALL 0x00000100U
#define PRESERVE_DATE 0x00001000U
#define SREC_FORCE_S3 0x00002000U
#define SREC_FORCE_LEN 0x00004000U
#define SET_START 0x00008000U
#define GAP_FILL 0x00010000U
#define WILDCARD 0x00020000U
#define NO_CHANGE_WARN 0x00040000U
#define SEC_ADD 0x00080000U
#define SEC_APPEND 0x00100000U
#define SEC_COMPRESS 0x00200000U
#define SEC_PRINT 0x00400000U
#define SEC_REMOVE 0x00800000U
#define SEC_COPY 0x01000000U
#define DISCARD_LLABEL 0x02000000U
#define LOCALIZE_HIDDEN 0x04000000U

int flags;
int64_t change_addr;
int64_t change_start;
uint64_t set_start;
unsigned long srec_len;
uint64_t pad_to;
uint8_t fill;
char *prefix_sec;
char *prefix_alloc;
char *prefix_sym;
char *debuglink;
struct section *symtab;
struct section *strtab;
struct section *shstrtab;
uint64_t *secndx;
uint64_t *symndx;
unsigned char *v_rel;
unsigned char *v_grp;
unsigned char *v_secsym;
STAILQ_HEAD(, segment) v_seg;
STAILQ_HEAD(, sec_action) v_sac;
STAILQ_HEAD(, sec_add) v_sadd;
STAILQ_HEAD(, symop) v_symop;
STAILQ_HEAD(, symfile) v_symfile;
TAILQ_HEAD(, section) v_sec;




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
STAILQ_HEAD(, ar_obj) v_arobj;
};

void add_section(struct elfcopy *_ecp, const char *_optarg);
void add_to_shstrtab(struct elfcopy *_ecp, const char *_name);
void add_to_symop_list(struct elfcopy *_ecp, const char *_name,
const char *_newname, unsigned int _op);
void add_to_symtab(struct elfcopy *_ecp, const char *_name,
uint64_t _st_value, uint64_t _st_size, uint16_t _st_shndx,
unsigned char _st_info, unsigned char _st_other, int _ndx_known);
int add_to_inseg_list(struct elfcopy *_ecp, struct section *_sec);
void adjust_addr(struct elfcopy *_ecp);
int cleanup_tempfile(char *_fn);
void copy_content(struct elfcopy *_ecp);
void copy_data(struct section *_s);
void copy_phdr(struct elfcopy *_ecp);
void copy_shdr(struct elfcopy *_ecp, struct section *_s, const char *_name,
int _copy, int _sec_flags);
void create_binary(int _ifd, int _ofd);
void create_elf(struct elfcopy *_ecp);
void create_elf_from_binary(struct elfcopy *_ecp, int _ifd, const char *ifn);
void create_elf_from_ihex(struct elfcopy *_ecp, int _ifd);
void create_elf_from_srec(struct elfcopy *_ecp, int _ifd);
struct section *create_external_section(struct elfcopy *_ecp, const char *_name,
char *_newname, void *_buf, uint64_t _size, uint64_t _off, uint64_t _stype,
Elf_Type _dtype, uint64_t flags, uint64_t _align, uint64_t _vma,
int _loadable);
void create_external_symtab(struct elfcopy *_ecp);
void create_ihex(int _ifd, int _ofd);
void create_pe(struct elfcopy *_ecp, int _ifd, int _ofd);
void create_scn(struct elfcopy *_ecp);
void create_srec(struct elfcopy *_ecp, int _ifd, int _ofd, const char *_ofn);
void create_symtab(struct elfcopy *_ecp);
void create_symtab_data(struct elfcopy *_ecp);
void create_tempfile(const char *_src, char **_fn, int *_fd);
void finalize_external_symtab(struct elfcopy *_ecp);
void free_elf(struct elfcopy *_ecp);
void free_sec_act(struct elfcopy *_ecp);
void free_sec_add(struct elfcopy *_ecp);
void free_symtab(struct elfcopy *_ecp);
void init_shstrtab(struct elfcopy *_ecp);
void insert_to_sec_list(struct elfcopy *_ecp, struct section *_sec,
int _tail);
struct section *insert_shtab(struct elfcopy *_ecp, int tail);
int is_remove_reloc_sec(struct elfcopy *_ecp, uint32_t _sh_info);
int is_remove_section(struct elfcopy *_ecp, const char *_name);
struct sec_action *lookup_sec_act(struct elfcopy *_ecp,
const char *_name, int _add);
struct symop *lookup_symop_list(struct elfcopy *_ecp, const char *_name,
unsigned int _op);
void resync_sections(struct elfcopy *_ecp);
void setup_phdr(struct elfcopy *_ecp);
void update_shdr(struct elfcopy *_ecp, int _update_link);

#if !defined(LIBELF_AR)
int ac_detect_ar(int _ifd);
void ac_create_ar(struct elfcopy *_ecp, int _ifd, int _ofd);
#endif
