





























#if !defined(_CTF_IMPL_H)
#define _CTF_IMPL_H

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/sysmacros.h>
#include <sys/ctf_api.h>

#if defined(_KERNEL)

#include <sys/systm.h>
#include <sys/cmn_err.h>
#include <sys/varargs.h>

#define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')



#define MAP_FAILED ((void *)-1)

#else

#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>

#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct ctf_helem {
uint_t h_name;
ushort_t h_type;
ushort_t h_next;
} ctf_helem_t;

typedef struct ctf_hash {
ushort_t *h_buckets;
ctf_helem_t *h_chains;
ushort_t h_nbuckets;
ushort_t h_nelems;
uint_t h_free;
} ctf_hash_t;

typedef struct ctf_strs {
const char *cts_strs;
size_t cts_len;
} ctf_strs_t;

typedef struct ctf_dmodel {
const char *ctd_name;
int ctd_code;
size_t ctd_pointer;
size_t ctd_char;
size_t ctd_short;
size_t ctd_int;
size_t ctd_long;
} ctf_dmodel_t;

typedef struct ctf_lookup {
const char *ctl_prefix;
size_t ctl_len;
ctf_hash_t *ctl_hash;
} ctf_lookup_t;

typedef struct ctf_fileops {
ushort_t (*ctfo_get_kind)(ushort_t);
ushort_t (*ctfo_get_root)(ushort_t);
ushort_t (*ctfo_get_vlen)(ushort_t);
} ctf_fileops_t;

typedef struct ctf_list {
struct ctf_list *l_prev;
struct ctf_list *l_next;
} ctf_list_t;

typedef enum {
CTF_PREC_BASE,
CTF_PREC_POINTER,
CTF_PREC_ARRAY,
CTF_PREC_FUNCTION,
CTF_PREC_MAX
} ctf_decl_prec_t;

typedef struct ctf_decl_node {
ctf_list_t cd_list;
ctf_id_t cd_type;
uint_t cd_kind;
uint_t cd_n;
} ctf_decl_node_t;

typedef struct ctf_decl {
ctf_list_t cd_nodes[CTF_PREC_MAX];
int cd_order[CTF_PREC_MAX];
ctf_decl_prec_t cd_qualp;
ctf_decl_prec_t cd_ordp;
char *cd_buf;
char *cd_ptr;
char *cd_end;
size_t cd_len;
int cd_err;
} ctf_decl_t;

typedef struct ctf_dmdef {
ctf_list_t dmd_list;
char *dmd_name;
ctf_id_t dmd_type;
ulong_t dmd_offset;
int dmd_value;
} ctf_dmdef_t;

typedef struct ctf_dtdef {
ctf_list_t dtd_list;
struct ctf_dtdef *dtd_hash;
char *dtd_name;
ctf_id_t dtd_type;
ctf_type_t dtd_data;
int dtd_ref;
union {
ctf_list_t dtu_members;
ctf_arinfo_t dtu_arr;
ctf_encoding_t dtu_enc;
ctf_id_t *dtu_argv;
} dtd_u;
} ctf_dtdef_t;

typedef struct ctf_bundle {
ctf_file_t *ctb_file;
ctf_id_t ctb_type;
ctf_dtdef_t *ctb_dtd;
} ctf_bundle_t;













struct ctf_file {
const ctf_fileops_t *ctf_fileops;
ctf_sect_t ctf_data;
ctf_sect_t ctf_symtab;
ctf_sect_t ctf_strtab;
ctf_hash_t ctf_structs;
ctf_hash_t ctf_unions;
ctf_hash_t ctf_enums;
ctf_hash_t ctf_names;
ctf_lookup_t ctf_lookups[5];
ctf_strs_t ctf_str[2];
const uchar_t *ctf_base;
const uchar_t *ctf_buf;
size_t ctf_size;
uint_t *ctf_sxlate;
ulong_t ctf_nsyms;
uint_t *ctf_txlate;
ushort_t *ctf_ptrtab;
ulong_t ctf_typemax;
const ctf_dmodel_t *ctf_dmodel;
struct ctf_file *ctf_parent;
const char *ctf_parlabel;
const char *ctf_parname;
uint_t ctf_refcnt;
uint_t ctf_flags;
int ctf_errno;
int ctf_version;
ctf_dtdef_t **ctf_dthash;
ulong_t ctf_dthashlen;
ctf_list_t ctf_dtdefs;
size_t ctf_dtstrlen;
ulong_t ctf_dtnextid;
ulong_t ctf_dtoldid;
void *ctf_specific;
};

#define LCTF_INDEX_TO_TYPEPTR(fp, i) ((ctf_type_t *)((uintptr_t)(fp)->ctf_buf + (fp)->ctf_txlate[(i)]))


#define LCTF_INFO_KIND(fp, info) ((fp)->ctf_fileops->ctfo_get_kind(info))
#define LCTF_INFO_ROOT(fp, info) ((fp)->ctf_fileops->ctfo_get_root(info))
#define LCTF_INFO_VLEN(fp, info) ((fp)->ctf_fileops->ctfo_get_vlen(info))

#define LCTF_MMAP 0x0001
#define LCTF_CHILD 0x0002
#define LCTF_RDWR 0x0004
#define LCTF_DIRTY 0x0008

#define ECTF_BASE 1000

enum {
ECTF_FMT = ECTF_BASE,
ECTF_ELFVERS,
ECTF_CTFVERS,
ECTF_ENDIAN,
ECTF_SYMTAB,
ECTF_SYMBAD,
ECTF_STRBAD,
ECTF_CORRUPT,
ECTF_NOCTFDATA,
ECTF_NOCTFBUF,
ECTF_NOSYMTAB,
ECTF_NOPARENT,
ECTF_DMODEL,
ECTF_MMAP,
ECTF_ZMISSING,
ECTF_ZINIT,
ECTF_ZALLOC,
ECTF_DECOMPRESS,
ECTF_STRTAB,
ECTF_BADNAME,
ECTF_BADID,
ECTF_NOTSOU,
ECTF_NOTENUM,
ECTF_NOTSUE,
ECTF_NOTINTFP,
ECTF_NOTARRAY,
ECTF_NOTREF,
ECTF_NAMELEN,
ECTF_NOTYPE,
ECTF_SYNTAX,
ECTF_NOTFUNC,
ECTF_NOFUNCDAT,
ECTF_NOTDATA,
ECTF_NOTYPEDAT,
ECTF_NOLABEL,
ECTF_NOLABELDATA,
ECTF_NOTSUP,
ECTF_NOENUMNAM,
ECTF_NOMEMBNAM,
ECTF_RDONLY,
ECTF_DTFULL,
ECTF_FULL,
ECTF_DUPMEMBER,
ECTF_CONFLICT,
ECTF_REFERENCED,
ECTF_NOTDYN
};

extern ssize_t ctf_get_ctt_size(const ctf_file_t *, const ctf_type_t *,
ssize_t *, ssize_t *);

extern const ctf_type_t *ctf_lookup_by_id(ctf_file_t **, ctf_id_t);

extern int ctf_hash_create(ctf_hash_t *, ulong_t);
extern int ctf_hash_insert(ctf_hash_t *, ctf_file_t *, ushort_t, uint_t);
extern int ctf_hash_define(ctf_hash_t *, ctf_file_t *, ushort_t, uint_t);
extern ctf_helem_t *ctf_hash_lookup(ctf_hash_t *, ctf_file_t *,
const char *, size_t);
extern uint_t ctf_hash_size(const ctf_hash_t *);
extern void ctf_hash_destroy(ctf_hash_t *);

#define ctf_list_prev(elem) ((void *)(((ctf_list_t *)(elem))->l_prev))
#define ctf_list_next(elem) ((void *)(((ctf_list_t *)(elem))->l_next))

extern void ctf_list_append(ctf_list_t *, void *);
extern void ctf_list_prepend(ctf_list_t *, void *);
extern void ctf_list_delete(ctf_list_t *, void *);

extern void ctf_dtd_insert(ctf_file_t *, ctf_dtdef_t *);
extern void ctf_dtd_delete(ctf_file_t *, ctf_dtdef_t *);
extern ctf_dtdef_t *ctf_dtd_lookup(ctf_file_t *, ctf_id_t);

extern void ctf_decl_init(ctf_decl_t *, char *, size_t);
extern void ctf_decl_fini(ctf_decl_t *);
extern void ctf_decl_push(ctf_decl_t *, ctf_file_t *, ctf_id_t);
extern void ctf_decl_sprintf(ctf_decl_t *, const char *, ...);

extern const char *ctf_strraw(ctf_file_t *, uint_t);
extern const char *ctf_strptr(ctf_file_t *, uint_t);

extern ctf_file_t *ctf_set_open_errno(int *, int);
extern long ctf_set_errno(ctf_file_t *, int);

extern const void *ctf_sect_mmap(ctf_sect_t *, int);
extern void ctf_sect_munmap(const ctf_sect_t *);

extern void *ctf_data_alloc(size_t);
extern void ctf_data_free(void *, size_t);
extern void ctf_data_protect(void *, size_t);

extern void *ctf_alloc(size_t);
extern void ctf_free(void *, size_t);

extern char *ctf_strdup(const char *);
extern const char *ctf_strerror(int);
extern void ctf_dprintf(const char *, ...);

extern void *ctf_zopen(int *);

extern const char _CTF_SECTION[];
extern const char _CTF_NULLSTR[];

extern int _libctf_version;
extern int _libctf_debug;

#if defined(__cplusplus)
}
#endif

#endif
