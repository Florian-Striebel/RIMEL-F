#include <sys/param.h>
#include <sys/objfs.h>
#if !defined(illumos)
#include <sys/bitmap.h>
#include <sys/utsname.h>
#include <sys/ioccom.h>
#include <sys/time.h>
#include <string.h>
#endif
#include <setjmp.h>
#include <libctf.h>
#include <dtrace.h>
#include <gelf.h>
#if defined(illumos)
#include <synch.h>
#endif
#if defined(__cplusplus)
extern "C" {
#endif
#include <dt_parser.h>
#include <dt_regset.h>
#include <dt_inttab.h>
#include <dt_strtab.h>
#include <dt_ident.h>
#include <dt_list.h>
#include <dt_decl.h>
#include <dt_as.h>
#include <dt_proc.h>
#include <dt_dof.h>
#include <dt_pcb.h>
#include <dt_pq.h>
struct dt_module;
struct dt_pfdict;
struct dt_arg;
struct dt_provider;
struct dt_xlator;
typedef struct dt_intrinsic {
const char *din_name;
ctf_encoding_t din_data;
uint_t din_kind;
} dt_intrinsic_t;
typedef struct dt_typedef {
const char *dty_src;
const char *dty_dst;
} dt_typedef_t;
typedef struct dt_intdesc {
const char *did_name;
ctf_file_t *did_ctfp;
ctf_id_t did_type;
uintmax_t did_limit;
} dt_intdesc_t;
typedef struct dt_modops {
uint_t (*do_syminit)(struct dt_module *);
void (*do_symsort)(struct dt_module *);
GElf_Sym *(*do_symname)(struct dt_module *,
const char *, GElf_Sym *, uint_t *);
GElf_Sym *(*do_symaddr)(struct dt_module *,
GElf_Addr, GElf_Sym *, uint_t *);
} dt_modops_t;
typedef struct dt_arg {
int da_ndx;
int da_mapping;
ctf_id_t da_type;
ctf_file_t *da_ctfp;
dt_ident_t *da_xlator;
struct dt_arg *da_next;
} dt_arg_t;
typedef struct dt_sym {
uint_t ds_symid;
uint_t ds_next;
} dt_sym_t;
typedef struct dt_module {
dt_list_t dm_list;
char dm_name[DTRACE_MODNAMELEN];
char dm_file[MAXPATHLEN];
struct dt_module *dm_next;
const dt_modops_t *dm_ops;
Elf *dm_elf;
objfs_info_t dm_info;
ctf_sect_t dm_symtab;
ctf_sect_t dm_strtab;
ctf_sect_t dm_ctdata;
ctf_file_t *dm_ctfp;
uint_t *dm_symbuckets;
dt_sym_t *dm_symchains;
void *dm_asmap;
uint_t dm_symfree;
uint_t dm_nsymbuckets;
uint_t dm_nsymelems;
uint_t dm_asrsv;
uint_t dm_aslen;
uint_t dm_flags;
int dm_modid;
GElf_Addr dm_text_va;
GElf_Xword dm_text_size;
GElf_Addr dm_data_va;
GElf_Xword dm_data_size;
GElf_Addr dm_bss_va;
GElf_Xword dm_bss_size;
dt_idhash_t *dm_extern;
#if !defined(illumos)
caddr_t dm_reloc_offset;
uintptr_t *dm_sec_offsets;
#endif
pid_t dm_pid;
uint_t dm_nctflibs;
ctf_file_t **dm_libctfp;
char **dm_libctfn;
} dt_module_t;
#define DT_DM_LOADED 0x1
#define DT_DM_KERNEL 0x2
#define DT_DM_PRIMARY 0x4
#if defined(__FreeBSD__)
typedef struct dt_kmodule {
struct dt_kmodule *dkm_next;
char *dkm_name;
dt_module_t *dkm_module;
} dt_kmodule_t;
#endif
typedef struct dt_provmod {
char *dp_name;
struct dt_provmod *dp_next;
} dt_provmod_t;
typedef struct dt_ahashent {
struct dt_ahashent *dtahe_prev;
struct dt_ahashent *dtahe_next;
struct dt_ahashent *dtahe_prevall;
struct dt_ahashent *dtahe_nextall;
uint64_t dtahe_hashval;
size_t dtahe_size;
dtrace_aggdata_t dtahe_data;
void (*dtahe_aggregate)(int64_t *, int64_t *, size_t);
} dt_ahashent_t;
typedef struct dt_ahash {
dt_ahashent_t **dtah_hash;
dt_ahashent_t *dtah_all;
size_t dtah_size;
} dt_ahash_t;
typedef struct dt_aggregate {
dtrace_bufdesc_t dtat_buf;
int dtat_flags;
processorid_t dtat_ncpus;
processorid_t *dtat_cpus;
processorid_t dtat_ncpu;
processorid_t dtat_maxcpu;
dt_ahash_t dtat_hash;
} dt_aggregate_t;
typedef struct dt_print_aggdata {
dtrace_hdl_t *dtpa_dtp;
dtrace_aggvarid_t dtpa_id;
FILE *dtpa_fp;
int dtpa_allunprint;
int dtpa_agghist;
int dtpa_agghisthdr;
int dtpa_aggpack;
} dt_print_aggdata_t;
typedef struct dt_dirpath {
dt_list_t dir_list;
char *dir_path;
} dt_dirpath_t;
typedef struct dt_lib_depend {
dt_list_t dtld_deplist;
char *dtld_library;
char *dtld_libpath;
uint_t dtld_finish;
uint_t dtld_start;
uint_t dtld_loaded;
dt_list_t dtld_dependencies;
dt_list_t dtld_dependents;
} dt_lib_depend_t;
typedef uint32_t dt_version_t;
struct dtrace_hdl {
const dtrace_vector_t *dt_vector;
void *dt_varg;
dtrace_conf_t dt_conf;
char dt_errmsg[BUFSIZ];
const char *dt_errtag;
dt_pcb_t *dt_pcb;
ulong_t dt_gen;
dt_list_t dt_programs;
dt_list_t dt_xlators;
struct dt_xlator **dt_xlatormap;
id_t dt_xlatorid;
dt_ident_t *dt_externs;
dt_idhash_t *dt_macros;
dt_idhash_t *dt_aggs;
dt_idhash_t *dt_globals;
dt_idhash_t *dt_tls;
dt_list_t dt_modlist;
dt_module_t **dt_mods;
#if defined(__FreeBSD__)
dt_kmodule_t **dt_kmods;
#endif
uint_t dt_modbuckets;
uint_t dt_nmods;
dt_provmod_t *dt_provmod;
dt_module_t *dt_exec;
dt_module_t *dt_rtld;
dt_module_t *dt_cdefs;
dt_module_t *dt_ddefs;
dt_list_t dt_provlist;
struct dt_provider **dt_provs;
uint_t dt_provbuckets;
uint_t dt_nprovs;
dt_proc_hash_t *dt_procs;
char **dt_proc_env;
dt_intdesc_t dt_ints[6];
ctf_id_t dt_type_func;
ctf_id_t dt_type_fptr;
ctf_id_t dt_type_str;
ctf_id_t dt_type_dyn;
ctf_id_t dt_type_stack;
ctf_id_t dt_type_symaddr;
ctf_id_t dt_type_usymaddr;
size_t dt_maxprobe;
dtrace_eprobedesc_t **dt_edesc;
dtrace_probedesc_t **dt_pdesc;
size_t dt_maxagg;
dtrace_aggdesc_t **dt_aggdesc;
int dt_maxformat;
void **dt_formats;
int dt_maxstrdata;
char **dt_strdata;
dt_aggregate_t dt_aggregate;
dt_pq_t *dt_bufq;
struct dt_pfdict *dt_pfdict;
dt_version_t dt_vmax;
dtrace_attribute_t dt_amin;
char *dt_cpp_path;
char **dt_cpp_argv;
int dt_cpp_argc;
int dt_cpp_args;
char *dt_ld_path;
#if defined(__FreeBSD__)
char *dt_objcopy_path;
#endif
dt_list_t dt_lib_path;
uint_t dt_lazyload;
uint_t dt_droptags;
uint_t dt_active;
uint_t dt_stopped;
processorid_t dt_beganon;
processorid_t dt_endedon;
uint_t dt_oflags;
uint_t dt_cflags;
uint_t dt_dflags;
uint_t dt_prcmode;
uint_t dt_linkmode;
uint_t dt_linktype;
uint_t dt_xlatemode;
uint_t dt_stdcmode;
uint_t dt_encoding;
uint_t dt_treedump;
uint64_t dt_options[DTRACEOPT_MAX];
int dt_version;
int dt_ctferr;
int dt_errno;
#if !defined(illumos)
const char *dt_errfile;
int dt_errline;
#endif
int dt_fd;
int dt_ftfd;
int dt_fterr;
int dt_cdefs_fd;
int dt_ddefs_fd;
#if defined(illumos)
int dt_stdout_fd;
#else
FILE *dt_freopen_fp;
#endif
dtrace_handle_err_f *dt_errhdlr;
void *dt_errarg;
dtrace_prog_t *dt_errprog;
dtrace_handle_drop_f *dt_drophdlr;
void *dt_droparg;
dtrace_handle_proc_f *dt_prochdlr;
void *dt_procarg;
dtrace_handle_setopt_f *dt_setopthdlr;
void *dt_setoptarg;
dtrace_status_t dt_status[2];
int dt_statusgen;
hrtime_t dt_laststatus;
hrtime_t dt_lastswitch;
hrtime_t dt_lastagg;
char *dt_sprintf_buf;
int dt_sprintf_buflen;
const char *dt_filetag;
char *dt_buffered_buf;
size_t dt_buffered_offs;
size_t dt_buffered_size;
dtrace_handle_buffered_f *dt_bufhdlr;
void *dt_bufarg;
dt_dof_t dt_dof;
struct utsname dt_uts;
dt_list_t dt_lib_dep;
dt_list_t dt_lib_dep_sorted;
dtrace_flowkind_t dt_flow;
const char *dt_prefix;
int dt_indent;
dtrace_epid_t dt_last_epid;
uint64_t dt_last_timestamp;
boolean_t dt_has_sugar;
};
#define DT_ECB_DEFAULT 0
#define DT_ECB_ERROR 1
#define DT_LINK_KERNEL 0
#define DT_LINK_PRIMARY 1
#define DT_LINK_DYNAMIC 2
#define DT_LINK_STATIC 3
#define DT_LTYP_ELF 0
#define DT_LTYP_DOF 1
#define DT_XL_STATIC 0
#define DT_XL_DYNAMIC 1
#define DT_STDC_XA 0
#define DT_STDC_XC 1
#define DT_STDC_XS 2
#define DT_STDC_XT 3
#define DT_ENCODING_UNSET 0
#define DT_ENCODING_ASCII 1
#define DT_ENCODING_UTF8 2
#define DT_TREEDUMP_PASS(dtp, p) ((dtp)->dt_treedump & (1 << ((p) - 1)))
#define DT_INT_CTFP(dtp) ((dtp)->dt_ints[0].did_ctfp)
#define DT_INT_TYPE(dtp) ((dtp)->dt_ints[0].did_type)
#define DT_FUNC_CTFP(dtp) ((dtp)->dt_ddefs->dm_ctfp)
#define DT_FUNC_TYPE(dtp) ((dtp)->dt_type_func)
#define DT_FPTR_CTFP(dtp) ((dtp)->dt_ddefs->dm_ctfp)
#define DT_FPTR_TYPE(dtp) ((dtp)->dt_type_fptr)
#define DT_STR_CTFP(dtp) ((dtp)->dt_ddefs->dm_ctfp)
#define DT_STR_TYPE(dtp) ((dtp)->dt_type_str)
#define DT_DYN_CTFP(dtp) ((dtp)->dt_ddefs->dm_ctfp)
#define DT_DYN_TYPE(dtp) ((dtp)->dt_type_dyn)
#define DT_STACK_CTFP(dtp) ((dtp)->dt_ddefs->dm_ctfp)
#define DT_STACK_TYPE(dtp) ((dtp)->dt_type_stack)
#define DT_SYMADDR_CTFP(dtp) ((dtp)->dt_ddefs->dm_ctfp)
#define DT_SYMADDR_TYPE(dtp) ((dtp)->dt_type_symaddr)
#define DT_USYMADDR_CTFP(dtp) ((dtp)->dt_ddefs->dm_ctfp)
#define DT_USYMADDR_TYPE(dtp) ((dtp)->dt_type_usymaddr)
#define DT_ACT_BASE DIF_SUBR_MAX + 1
#define DT_ACT(n) (DT_ACT_BASE + (n))
#define DT_ACT_PRINTF DT_ACT(0)
#define DT_ACT_TRACE DT_ACT(1)
#define DT_ACT_TRACEMEM DT_ACT(2)
#define DT_ACT_STACK DT_ACT(3)
#define DT_ACT_STOP DT_ACT(4)
#define DT_ACT_BREAKPOINT DT_ACT(5)
#define DT_ACT_PANIC DT_ACT(6)
#define DT_ACT_SPECULATE DT_ACT(7)
#define DT_ACT_COMMIT DT_ACT(8)
#define DT_ACT_DISCARD DT_ACT(9)
#define DT_ACT_CHILL DT_ACT(10)
#define DT_ACT_EXIT DT_ACT(11)
#define DT_ACT_USTACK DT_ACT(12)
#define DT_ACT_PRINTA DT_ACT(13)
#define DT_ACT_RAISE DT_ACT(14)
#define DT_ACT_CLEAR DT_ACT(15)
#define DT_ACT_NORMALIZE DT_ACT(16)
#define DT_ACT_DENORMALIZE DT_ACT(17)
#define DT_ACT_TRUNC DT_ACT(18)
#define DT_ACT_SYSTEM DT_ACT(19)
#define DT_ACT_JSTACK DT_ACT(20)
#define DT_ACT_FTRUNCATE DT_ACT(21)
#define DT_ACT_FREOPEN DT_ACT(22)
#define DT_ACT_SYM DT_ACT(23)
#define DT_ACT_MOD DT_ACT(24)
#define DT_ACT_USYM DT_ACT(25)
#define DT_ACT_UMOD DT_ACT(26)
#define DT_ACT_UADDR DT_ACT(27)
#define DT_ACT_SETOPT DT_ACT(28)
#define DT_ACT_PRINT DT_ACT(29)
#define DT_ACT_PRINTM DT_ACT(30)
#define DT_FREOPEN_RESTORE "."
#define EDT_BASE 1000
enum {
EDT_VERSION = EDT_BASE,
EDT_VERSINVAL,
EDT_VERSUNDEF,
EDT_VERSREDUCED,
EDT_CTF,
EDT_COMPILER,
EDT_NOTUPREG,
EDT_NOMEM,
EDT_INT2BIG,
EDT_STR2BIG,
EDT_NOMOD,
EDT_NOPROV,
EDT_NOPROBE,
EDT_NOSYM,
EDT_NOSYMADDR,
EDT_NOTYPE,
EDT_NOVAR,
EDT_NOAGG,
EDT_BADSCOPE,
EDT_BADSPEC,
EDT_BADSPCV,
EDT_BADID,
EDT_NOTLOADED,
EDT_NOCTF,
EDT_DATAMODEL,
EDT_DIFVERS,
EDT_BADAGG,
EDT_FIO,
EDT_DIFINVAL,
EDT_DIFSIZE,
EDT_DIFFAULT,
EDT_BADPROBE,
EDT_BADPGLOB,
EDT_NOSCOPE,
EDT_NODECL,
EDT_DMISMATCH,
EDT_DOFFSET,
EDT_DALIGN,
EDT_BADOPTNAME,
EDT_BADOPTVAL,
EDT_BADOPTCTX,
EDT_CPPFORK,
EDT_CPPEXEC,
EDT_CPPENT,
EDT_CPPERR,
EDT_SYMOFLOW,
EDT_ACTIVE,
EDT_DESTRUCTIVE,
EDT_NOANON,
EDT_ISANON,
EDT_ENDTOOBIG,
EDT_NOCONV,
EDT_BADCONV,
EDT_BADERROR,
EDT_ERRABORT,
EDT_DROPABORT,
EDT_DIRABORT,
EDT_BADRVAL,
EDT_BADNORMAL,
EDT_BUFTOOSMALL,
EDT_BADTRUNC,
EDT_BUSY,
EDT_ACCESS,
EDT_NOENT,
EDT_BRICKED,
EDT_HARDWIRE,
EDT_ELFVERSION,
EDT_NOBUFFERED,
EDT_UNSTABLE,
EDT_BADSETOPT,
EDT_BADSTACKPC,
EDT_BADAGGVAR,
EDT_OVERSION,
EDT_ENABLING_ERR,
EDT_NOPROBES,
EDT_CANTLOAD
};
extern dtrace_attribute_t dt_attr_min(dtrace_attribute_t, dtrace_attribute_t);
extern dtrace_attribute_t dt_attr_max(dtrace_attribute_t, dtrace_attribute_t);
extern char *dt_attr_str(dtrace_attribute_t, char *, size_t);
extern int dt_attr_cmp(dtrace_attribute_t, dtrace_attribute_t);
#define DT_VERSION_STRMAX 16
#define DT_VERSION_MAJMAX 0xFF
#define DT_VERSION_MINMAX 0xFFF
#define DT_VERSION_MICMAX 0xFFF
#define DT_VERSION_NUMBER(M, m, u) ((((M) & 0xFF) << 24) | (((m) & 0xFFF) << 12) | ((u) & 0xFFF))
#define DT_VERSION_MAJOR(v) (((v) & 0xFF000000) >> 24)
#define DT_VERSION_MINOR(v) (((v) & 0x00FFF000) >> 12)
#define DT_VERSION_MICRO(v) ((v) & 0x00000FFF)
extern char *dt_version_num2str(dt_version_t, char *, size_t);
extern int dt_version_str2num(const char *, dt_version_t *);
extern int dt_version_defined(dt_version_t);
extern char *dt_cpp_add_arg(dtrace_hdl_t *, const char *);
extern char *dt_cpp_pop_arg(dtrace_hdl_t *);
#if defined(illumos)
extern int dt_set_errno(dtrace_hdl_t *, int);
#else
int _dt_set_errno(dtrace_hdl_t *, int, const char *, int);
void dt_get_errloc(dtrace_hdl_t *, const char **, int *);
#define dt_set_errno(_a,_b) _dt_set_errno(_a,_b,__FILE__,__LINE__)
#endif
extern void dt_set_errmsg(dtrace_hdl_t *, const char *, const char *,
const char *, int, const char *, va_list);
#if defined(illumos)
extern int dt_ioctl(dtrace_hdl_t *, int, void *);
#else
extern int dt_ioctl(dtrace_hdl_t *, u_long, void *);
#endif
extern int dt_status(dtrace_hdl_t *, processorid_t);
extern long dt_sysconf(dtrace_hdl_t *, int);
extern ssize_t dt_write(dtrace_hdl_t *, int, const void *, size_t);
extern int dt_printf(dtrace_hdl_t *, FILE *, const char *, ...);
extern void *dt_zalloc(dtrace_hdl_t *, size_t);
extern void *dt_alloc(dtrace_hdl_t *, size_t);
extern void dt_free(dtrace_hdl_t *, void *);
extern void dt_difo_free(dtrace_hdl_t *, dtrace_difo_t *);
extern int dt_gmatch(const char *, const char *);
extern char *dt_basename(char *);
extern ulong_t dt_popc(ulong_t);
extern ulong_t dt_popcb(const ulong_t *, ulong_t);
extern int dt_buffered_enable(dtrace_hdl_t *);
extern int dt_buffered_flush(dtrace_hdl_t *, dtrace_probedata_t *,
const dtrace_recdesc_t *, const dtrace_aggdata_t *, uint32_t flags);
extern void dt_buffered_disable(dtrace_hdl_t *);
extern void dt_buffered_destroy(dtrace_hdl_t *);
extern uint64_t dt_stddev(uint64_t *, uint64_t);
extern int dt_rw_read_held(pthread_rwlock_t *);
extern int dt_rw_write_held(pthread_rwlock_t *);
extern int dt_mutex_held(pthread_mutex_t *);
extern int dt_options_load(dtrace_hdl_t *);
#define DT_RW_READ_HELD(x) dt_rw_read_held(x)
#define DT_RW_WRITE_HELD(x) dt_rw_write_held(x)
#define DT_RW_LOCK_HELD(x) (DT_RW_READ_HELD(x) || DT_RW_WRITE_HELD(x))
#define DT_MUTEX_HELD(x) dt_mutex_held(x)
extern void dt_dprintf(const char *, ...);
extern void dt_setcontext(dtrace_hdl_t *, dtrace_probedesc_t *);
extern void dt_endcontext(dtrace_hdl_t *);
extern void dt_pragma(dt_node_t *);
extern int dt_reduce(dtrace_hdl_t *, dt_version_t);
extern void dt_cg(dt_pcb_t *, dt_node_t *);
extern dtrace_difo_t *dt_as(dt_pcb_t *);
extern void dt_dis(const dtrace_difo_t *, FILE *);
extern int dt_aggregate_go(dtrace_hdl_t *);
extern int dt_aggregate_init(dtrace_hdl_t *);
extern void dt_aggregate_destroy(dtrace_hdl_t *);
extern int dt_epid_lookup(dtrace_hdl_t *, dtrace_epid_t,
dtrace_eprobedesc_t **, dtrace_probedesc_t **);
extern void dt_epid_destroy(dtrace_hdl_t *);
extern int dt_aggid_lookup(dtrace_hdl_t *, dtrace_aggid_t, dtrace_aggdesc_t **);
extern void dt_aggid_destroy(dtrace_hdl_t *);
extern void *dt_format_lookup(dtrace_hdl_t *, int);
extern void dt_format_destroy(dtrace_hdl_t *);
extern const char *dt_strdata_lookup(dtrace_hdl_t *, int);
extern void dt_strdata_destroy(dtrace_hdl_t *);
extern int dt_print_quantize(dtrace_hdl_t *, FILE *,
const void *, size_t, uint64_t);
extern int dt_print_lquantize(dtrace_hdl_t *, FILE *,
const void *, size_t, uint64_t);
extern int dt_print_llquantize(dtrace_hdl_t *, FILE *,
const void *, size_t, uint64_t);
extern int dt_print_agg(const dtrace_aggdata_t *, void *);
extern int dt_handle(dtrace_hdl_t *, dtrace_probedata_t *);
extern int dt_handle_liberr(dtrace_hdl_t *,
const dtrace_probedata_t *, const char *);
extern int dt_handle_cpudrop(dtrace_hdl_t *, processorid_t,
dtrace_dropkind_t, uint64_t);
extern int dt_handle_status(dtrace_hdl_t *,
dtrace_status_t *, dtrace_status_t *);
extern int dt_handle_setopt(dtrace_hdl_t *, dtrace_setoptdata_t *);
extern int dt_lib_depend_add(dtrace_hdl_t *, dt_list_t *, const char *);
extern dt_lib_depend_t *dt_lib_depend_lookup(dt_list_t *, const char *);
extern dt_pcb_t *yypcb;
extern char yyintprefix;
extern char yyintsuffix[4];
extern int yyintdecimal;
extern char *yytext;
extern int yylineno;
extern int yydebug;
extern dt_node_t *yypragma;
extern const dtrace_attribute_t _dtrace_maxattr;
extern const dtrace_attribute_t _dtrace_defattr;
extern const dtrace_attribute_t _dtrace_symattr;
extern const dtrace_attribute_t _dtrace_typattr;
extern const dtrace_attribute_t _dtrace_prvattr;
extern const dtrace_pattr_t _dtrace_prvdesc;
extern const dt_version_t _dtrace_versions[];
extern const char *const _dtrace_version;
extern int _dtrace_strbuckets;
extern int _dtrace_intbuckets;
extern uint_t _dtrace_stkindent;
extern uint_t _dtrace_pidbuckets;
extern uint_t _dtrace_pidlrulim;
extern int _dtrace_debug;
extern size_t _dtrace_bufsize;
extern int _dtrace_argmax;
extern const char *_dtrace_libdir;
extern const char *_dtrace_moddir;
#if defined(__FreeBSD__)
extern int gmatch(const char *, const char *);
extern int yylex(void);
#endif
#if defined(__cplusplus)
}
#endif
