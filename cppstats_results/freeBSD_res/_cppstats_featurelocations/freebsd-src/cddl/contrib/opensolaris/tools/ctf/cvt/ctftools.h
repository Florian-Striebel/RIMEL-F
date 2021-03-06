
























#if !defined(_CTFTOOLS_H)
#define _CTFTOOLS_H





#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libelf.h>
#include <gelf.h>
#include <pthread.h>

#include <sys/ccompile.h>
#include <sys/endian.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include "list.h"
#include "hash.h"

#if !defined(DEBUG_LEVEL)
#define DEBUG_LEVEL 0
#endif
#if !defined(DEBUG_PARSE)
#define DEBUG_PARSE 0
#endif

#if !defined(DEBUG_STREAM)
#define DEBUG_STREAM stderr
#endif

#if !defined(MAX)
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif

#if !defined(MIN)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif


#if !defined(BYTE_ORDER)
#error "Missing BYTE_ORDER defines"
#elif !defined(_LITTLE_ENDIAN)
#error "Missing _LITTLE_ENDIAN defines"
#elif !defined(_BIG_ENDIAN)
#error "Missing _BIG_ENDIAN defines"
#endif

#define TRUE 1
#define FALSE 0

#define CTF_ELF_SCN_NAME ".SUNW_ctf"

#define CTF_LABEL_LASTIDX -1

#define CTF_DEFAULT_LABEL "*** No Label Provided ***"




#define TDATA_LAYOUT_HASH_SIZE 8191
#define TDATA_ID_HASH_SIZE 997
#define IIDESC_HASH_SIZE 8191






#define FUNCARG_DEF 5

extern const char *progname;
extern int debug_level;
extern int debug_parse;
extern char *curhdr;





typedef struct stab {
uint32_t n_strx;
uint8_t n_type;
int8_t n_other;
int16_t n_desc;
uint32_t n_value;
} stab_t;

#define N_GSYM 0x20
#define N_FUN 0x24
#define N_STSYM 0x26
#define N_LCSYM 0x28
#define N_ROSYM 0x2c
#define N_OPT 0x3c
#define N_RSYM 0x40
#define N_SO 0x64
#define N_LSYM 0x80
#define N_SOL 0x84
#define N_PSYM 0xa0
#define N_LBRAC 0xc0
#define N_RBRAC 0xe0
#define N_BINCL 0x82
#define N_EINCL 0xa2









typedef enum stabtype {
STABTYPE_FIRST,
INTRINSIC,
POINTER,
ARRAY,
FUNCTION,
STRUCT,
UNION,
ENUM,
FORWARD,
TYPEDEF,
TYPEDEF_UNRES,
VOLATILE,
CONST,
RESTRICT,
STABTYPE_LAST
} stabtype_t;

typedef struct tdesc tdesc_t;


typedef struct ardef {
tdesc_t *ad_contents;
tdesc_t *ad_idxtype;
uint_t ad_nelems;
} ardef_t;


typedef struct mlist {
int ml_offset;
int ml_size;
char *ml_name;
struct tdesc *ml_type;
struct mlist *ml_next;
} mlist_t;


typedef struct elist {
char *el_name;
int el_number;
struct elist *el_next;
} elist_t;


typedef enum {
INTR_INT,
INTR_REAL
} intrtype_t;

typedef struct intr {
intrtype_t intr_type;
int intr_signed;
union {
char _iformat;
int _fformat;
} _u;
int intr_offset;
int intr_nbits;
} intr_t;

#define intr_iformat _u._iformat
#define intr_fformat _u._fformat

typedef struct fnarg {
char *fna_name;
struct tdesc *fna_type;
} fnarg_t;

#define FN_F_GLOBAL 0x1
#define FN_F_VARARGS 0x2

typedef struct fndef {
struct tdesc *fn_ret;
uint_t fn_nargs;
tdesc_t **fn_args;
uint_t fn_vargs;
} fndef_t;

typedef int32_t tid_t;









struct tdesc {
char *t_name;
tdesc_t *t_next;

tid_t t_id;
tdesc_t *t_hash;

stabtype_t t_type;
int t_size;

union {
intr_t *intr;
tdesc_t *tdesc;
ardef_t *ardef;
mlist_t *members;
elist_t *emem;
fndef_t *fndef;
} t_data;

int t_flags;
int t_vgen;
int t_emark;
};

#define t_intr t_data.intr
#define t_tdesc t_data.tdesc
#define t_ardef t_data.ardef
#define t_members t_data.members
#define t_emem t_data.emem
#define t_fndef t_data.fndef

#define TDESC_F_ISROOT 0x1
#define TDESC_F_GLOBAL 0x2
#define TDESC_F_RESOLVED 0x4






typedef enum iitype {
II_NOT = 0,
II_GFUN,
II_SFUN,
II_GVAR,
II_SVAR,
II_PSYM,
II_SOU,
II_TYPE
} iitype_t;

typedef struct iidesc {
iitype_t ii_type;
char *ii_name;
tdesc_t *ii_dtype;
char *ii_owner;
int ii_flags;


int ii_nargs;
tdesc_t **ii_args;
int ii_vargs;
} iidesc_t;

#define IIDESC_F_USED 0x1






typedef struct labelent {
char *le_name;
int le_idx;
} labelent_t;





typedef struct tdata {
int td_curemark;
int td_curvgen;
int td_nextid;
hash_t *td_iihash;

hash_t *td_layouthash;
hash_t *td_idhash;
list_t *td_fwdlist;

char *td_parlabel;
char *td_parname;
list_t *td_labels;

pthread_mutex_t td_mergelock;

int td_ref;
} tdata_t;







typedef struct iiburst {
int iib_nfuncs;
int iib_curfunc;
iidesc_t **iib_funcs;

int iib_nobjts;
int iib_curobjt;
iidesc_t **iib_objts;

list_t *iib_types;
int iib_maxtypeid;

tdata_t *iib_td;
struct tdtrav_data *iib_tdtd;
} iiburst_t;

typedef struct ctf_buf ctf_buf_t;

typedef struct symit_data symit_data_t;


void cvt_fixstabs(tdata_t *);
void cvt_fixups(tdata_t *, size_t);


caddr_t ctf_gen(iiburst_t *, size_t *, int);
tdata_t *ctf_load(char *, caddr_t, size_t, symit_data_t *, char *);


iidesc_t *iidesc_new(char *);
int iidesc_hash(int, void *);
void iter_iidescs_by_name(tdata_t *, const char *,
int (*)(void *, void *), void *);
iidesc_t *iidesc_dup(iidesc_t *);
iidesc_t *iidesc_dup_rename(iidesc_t *, char const *, char const *);
void iidesc_add(hash_t *, iidesc_t *);
void iidesc_free(void *, void *);
int iidesc_count_type(void *, void *);
void iidesc_stats(hash_t *);
int iidesc_dump(iidesc_t *);


typedef enum source_types {
SOURCE_NONE = 0,
SOURCE_UNKNOWN = 1,
SOURCE_C = 2,
SOURCE_S = 4
} source_types_t;

source_types_t built_source_types(Elf *, const char *);
int count_files(char **, int);
int read_ctf(char **, int, char *, int (*)(tdata_t *, char *, void *),
void *, int);
int read_ctf_save_cb(tdata_t *, char *, void *);
symit_data_t *symit_new(Elf *, const char *);
void symit_reset(symit_data_t *);
char *symit_curfile(symit_data_t *);
GElf_Sym *symit_next(symit_data_t *, int);
char *symit_name(symit_data_t *);
void symit_free(symit_data_t *);


void merge_into_master(tdata_t *, tdata_t *, tdata_t *, int);


#define CTF_FUZZY_MATCH 0x1
#define CTF_USE_DYNSYM 0x2
#define CTF_COMPRESS 0x4
#define CTF_KEEP_STABS 0x8
#define CTF_SWAP_BYTES 0x10

void write_ctf(tdata_t *, const char *, const char *, int);


void parse_init(tdata_t *);
void parse_finish(tdata_t *);
int parse_stab(stab_t *, char *, iidesc_t **);
tdesc_t *lookup(int);
tdesc_t *lookupname(const char *);
void check_hash(void);
void resolve_typed_bitfields(void);


int stabs_read(tdata_t *, Elf *, char *);


int dw_read(tdata_t *, Elf *, char *);
const char *dw_tag2str(uint_t);


tdata_t *tdata_new(void);
void tdata_free(tdata_t *);
void tdata_build_hashes(tdata_t *td);
const char *tdesc_name(tdesc_t *);
int tdesc_idhash(int, void *);
int tdesc_idcmp(void *, void *);
int tdesc_namehash(int, void *);
int tdesc_namecmp(void *, void *);
int tdesc_layouthash(int, void *);
int tdesc_layoutcmp(void *, void *);
void tdesc_free(tdesc_t *);
void tdata_label_add(tdata_t *, const char *, int);
labelent_t *tdata_label_top(tdata_t *);
int tdata_label_find(tdata_t *, char *);
void tdata_label_free(tdata_t *);
void tdata_merge(tdata_t *, tdata_t *);
void tdata_label_newmax(tdata_t *, int);


int streq(const char *, const char *);
int findelfsecidx(Elf *, const char *, const char *);
size_t elf_ptrsz(Elf *);
char *mktmpname(const char *, const char *);
void terminate(const char *, ...) __NORETURN;
void aborterr(const char *, ...) __NORETURN;
void set_terminate_cleanup(void (*)(void));
void elfterminate(const char *, const char *, ...);
void warning(const char *, ...);
void vadebug(int, const char *, va_list);
void debug(int, const char *, ...);


void watch_dump(int);
void watch_set(void *, int);

#if defined(__cplusplus)
}
#endif

#endif
